import crypto from 'node:crypto';
import http from 'node:http';
import net from 'node:net';
import {spawn} from 'node:child_process';

const wasmJs = process.argv[2];
if (!wasmJs)
{
    console.error('usage: node run.mjs <sfml-network-node-test.js>');
    process.exit(1);
}

if (Number(process.versions.node.split('.')[0]) < 22)
{
    console.error('Node.js 22 or newer is required for the global WebSocket client');
    process.exit(1);
}

function encodeName(name)
{
    const bytes = [];
    for (const label of name.split('.'))
    {
        if (!label.length)
            continue;
        bytes.push(label.length);
        for (let index = 0; index < label.length; ++index)
            bytes.push(label.charCodeAt(index));
    }
    bytes.push(0);
    return Buffer.from(bytes);
}

function readName(buffer, offset)
{
    let position = offset;
    const labels = [];
    let jumped = false;
    let end = offset;
    for (let guard = 0; guard < 16; ++guard)
    {
        if (position >= buffer.length)
            return null;
        const length = buffer[position];
        if (length === 0)
        {
            if (!jumped)
                end = position + 1;
            return {name: labels.join('.'), end};
        }
        if ((length & 0xc0) === 0xc0)
        {
            if (position + 1 >= buffer.length)
                return null;
            if (!jumped)
                end = position + 2;
            position = ((length & 0x3f) << 8) | buffer[position + 1];
            jumped = true;
            continue;
        }
        position += 1;
        labels.push(buffer.subarray(position, position + length).toString('ascii'));
        position += length;
        if (!jumped)
            end = position;
    }
    return null;
}

function dnsResponse(request)
{
    if (request.length < 12)
        return Buffer.alloc(0);
    const name = readName(request, 12);
    if (!name)
        return Buffer.alloc(0);
    const qtype = request.readUInt16BE(name.end);
    const question = request.subarray(12, name.end + 4);
    const header = Buffer.alloc(12);
    request.copy(header, 0, 0, 2);
    header.writeUInt16BE(0x8180, 2);
    header.writeUInt16BE(1, 4);

    let rdata = null;
    const lower = name.name.toLowerCase();
    if ((lower === 'test.sfml') && (qtype === 1))
        rdata = Buffer.from([203, 0, 113, 5]);
    else if ((lower === 'test.sfml') && (qtype === 2))
        rdata = encodeName('ns.sfml');
    else if ((lower === 'test.sfml') && (qtype === 15))
        rdata = Buffer.concat([Buffer.from([0, 10]), encodeName('mail.sfml')]);
    else if ((lower === 'test.sfml') && (qtype === 33))
        rdata = Buffer.concat([Buffer.from([0, 1, 0, 2, 0, 9]), encodeName('srv.sfml')]);
    else if ((lower === 'test.sfml') && (qtype === 16))
        rdata = Buffer.from([4, 0x73, 0x66, 0x6d, 0x6c]);

    if (!rdata)
        return Buffer.concat([header, question]);

    header.writeUInt16BE(1, 6);
    const record = Buffer.alloc(12 + rdata.length);
    record[0] = 0xc0;
    record[1] = 0x0c;
    record.writeUInt16BE(qtype, 2);
    record.writeUInt16BE(1, 4);
    record.writeUInt32BE(60, 6);
    record.writeUInt16BE(rdata.length, 10);
    rdata.copy(record, 12);
    return Buffer.concat([header, question, record]);
}

function readBody(request)
{
    return new Promise((resolve) =>
    {
        const chunks = [];
        request.on('data', (chunk) => chunks.push(chunk));
        request.on('end', () => resolve(Buffer.concat(chunks)));
    });
}

function listen(server)
{
    return new Promise((resolve) =>
    {
        server.listen(0, '127.0.0.1', () => resolve(server.address().port));
    });
}

function encodeFrame(data, opcode)
{
    const payload = Buffer.from(data);
    let header;
    if (payload.length < 126)
        header = Buffer.from([0x80 | opcode, payload.length]);
    else if (payload.length < 65536)
    {
        header = Buffer.alloc(4);
        header[0] = 0x80 | opcode;
        header[1] = 126;
        header.writeUInt16BE(payload.length, 2);
    }
    else
    {
        header = Buffer.alloc(10);
        header[0] = 0x80 | opcode;
        header[1] = 127;
        header.writeBigUInt64BE(BigInt(payload.length), 2);
    }
    return Buffer.concat([header, payload]);
}

function parseFrames(buffer)
{
    const messages = [];
    let offset = 0;
    while (buffer.length - offset >= 2)
    {
        const first = buffer[offset];
        const second = buffer[offset + 1];
        const opcode = first & 0x0f;
        const masked = (second & 0x80) !== 0;
        let length = second & 0x7f;
        let header = 2;
        if (length === 126)
        {
            if (buffer.length - offset < 4)
                break;
            length = buffer.readUInt16BE(offset + 2);
            header = 4;
        }
        else if (length === 127)
        {
            if (buffer.length - offset < 10)
                break;
            length = Number(buffer.readBigUInt64BE(offset + 2));
            header = 10;
        }
        const maskLength = masked ? 4 : 0;
        if (buffer.length - offset < header + maskLength + length)
            break;
        let payload = buffer.subarray(offset + header + maskLength, offset + header + maskLength + length);
        if (masked)
        {
            const mask = buffer.subarray(offset + header, offset + header + 4);
            const unmasked = Buffer.alloc(payload.length);
            for (let index = 0; index < payload.length; ++index)
                unmasked[index] = payload[index] ^ mask[index % 4];
            payload = unmasked;
        }
        offset += header + maskLength + length;
        if (opcode === 0x8)
            messages.push({close: true, data: payload});
        else if (opcode === 0x9)
            messages.push({ping: true, data: payload});
        else if ((opcode === 0x1) || (opcode === 0x2) || (opcode === 0x0))
            messages.push({data: payload, text: opcode === 0x1});
    }
    return {rest: buffer.subarray(offset), messages};
}

function startWebSocket(handler)
{
    const server = net.createServer((socket) =>
    {
        let buffer = Buffer.alloc(0);
        let accepted = false;
        const connection = {
            protocol: '',
            send(data, opcode = 2)
            {
                socket.write(encodeFrame(data, opcode));
            },
            close()
            {
                socket.end();
            }
        };
        socket.on('data', (chunk) =>
        {
            buffer = Buffer.concat([buffer, chunk]);
            if (!accepted)
            {
                const end = buffer.indexOf('\r\n\r\n');
                if (end < 0)
                    return;
                const text = buffer.subarray(0, end).toString('latin1');
                buffer = buffer.subarray(end + 4);
                const key = text.match(/Sec-WebSocket-Key:\s*(.+)\r\n/i);
                if (!key)
                {
                    socket.destroy();
                    return;
                }
                const protocol = text.match(/Sec-WebSocket-Protocol:\s*(.+)\r\n/i);
                connection.protocol = protocol ? protocol[1].split(',')[0].trim() : '';
                const accept = crypto.createHash('sha1').update(key[1].trim() + '258EAFA5-E914-47DA-95CA-C5AB0DC85B11').digest('base64');
                const headers = [
                    'HTTP/1.1 101 Switching Protocols',
                    'Upgrade: websocket',
                    'Connection: Upgrade',
                    `Sec-WebSocket-Accept: ${accept}`
                ];
                if (connection.protocol)
                    headers.push(`Sec-WebSocket-Protocol: ${connection.protocol}`);
                socket.write(headers.join('\r\n') + '\r\n\r\n');
                accepted = true;
                handler(connection);
            }
            const parsed = parseFrames(buffer);
            buffer = parsed.rest;
            for (const message of parsed.messages)
            {
                if (message.ping)
                    connection.send(message.data, 0xA);
                else if (message.close)
                    connection.close();
                else
                    connection.onmessage?.(message);
            }
        });
        socket.on('error', () => {});
    });
    return server;
}

const httpServer = http.createServer(async (request, response) =>
{
    const body = await readBody(request);
    if (request.url.startsWith('/dns-query'))
    {
        response.writeHead(200, {'content-type': 'application/dns-message'});
        response.end(dnsResponse(body));
        return;
    }
    if (request.url.startsWith('/echo'))
    {
        response.writeHead(200, {'content-type': 'text/plain', 'x-test': 'sfml'});
        response.end(Buffer.concat([Buffer.from('hello '), body]));
        return;
    }
    response.writeHead(404);
    response.end('missing');
});

const tcpServer = startWebSocket((connection) =>
{
    connection.onmessage = (message) =>
    {
        if (message.close)
            connection.close();
        else if (message.data)
            connection.send(message.data, 2);
    };
});

const udpServer = startWebSocket((connection) =>
{
    connection.onmessage = (message) =>
    {
        if (!message.data)
            return;
        let json;
        try
        {
            json = JSON.parse(message.data.toString('utf8'));
        }
        catch
        {
            return;
        }
        if (json.type === 'offer')
            connection.send(JSON.stringify({type: 'answer', sdp: 'v=0\r\n'}), 1);
    };
});

const ftpServer = startWebSocket((connection) =>
{
    connection.send(Buffer.from('421 Service not available\r\n'));
    connection.close();
});

const httpPort = await listen(httpServer);
const tcpPort = await listen(tcpServer);
const udpPort = await listen(udpServer);
const ftpPort = await listen(ftpServer);

const child = spawn(process.execPath, [wasmJs], {
    env: {
        ...process.env,
        SFML_DOH_URL: `http://127.0.0.1:${httpPort}/dns-query`,
        SFML_TEST_HTTP_PORT: String(httpPort),
        SFML_TEST_TCP_PORT: String(tcpPort),
        SFML_TEST_UDP_PORT: String(udpPort),
        SFML_TEST_FTP_PORT: String(ftpPort)
    }
});

let stderr = '';
child.stdout.on('data', (chunk) => process.stdout.write(chunk));
child.stderr.on('data', (chunk) =>
{
    stderr += chunk.toString();
    process.stderr.write(chunk);
});

const hang = setTimeout(() =>
{
    console.error('network web test timed out');
    child.kill();
}, 20000);

child.on('exit', (code) =>
{
    clearTimeout(hang);
    const required = [
        'IpAddress::getLocalAddress',
        'TcpSocket::setupTlsClient with certificate verification disabled',
        'TcpSocket::setupTlsClient with a caller-supplied certificate',
        'TcpSocket::setupTlsServer',
        'Http::sendRequest with certificate verification disabled',
        'Http::setHost with an address type',
        'Dns query with a caller-supplied server',
        'UdpSocket::send to a broadcast or multicast address'
    ];
    let failed = code !== 0;
    for (const text of required)
    {
        const count = stderr.split(text).length - 1;
        if (count !== 1)
        {
            console.error(`expected one skip notice for ${text}, found ${count}`);
            failed = true;
        }
    }
    httpServer.close();
    tcpServer.close();
    udpServer.close();
    ftpServer.close();
    process.exit(failed ? 1 : 0);
});
