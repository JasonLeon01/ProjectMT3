////////////////////////////////////////////////////////////
//
// SFML - Simple and Fast Multimedia Library
// Copyright (C) 2007-2026 Laurent Gomila (laurent@sfml-dev.org)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <SFML/Network/Emscripten/WebTransport.hpp>

#include <SFML/System/Err.hpp>

#include <array>
#include <emscripten.h>
#include <limits>
#include <mbedtls/threading.h>
#include <memory>
#include <mutex>
#include <new>
#include <ostream>
#include <string>
#include <utility>

#include <cstdlib>


namespace
{
////////////////////////////////////////////////////////////
struct WebMbedtlsThreading
{
    WebMbedtlsThreading()
    {
#if defined(MBEDTLS_THREADING_C) && defined(MBEDTLS_THREADING_ALT)
        mbedtls_threading_set_alt(
            [](mbedtls_threading_mutex_t* mutex)
            {
                if (mutex)
                    *mutex = new std::mutex;
            },
            [](mbedtls_threading_mutex_t* mutex)
            {
                if (!mutex)
                    return;

                delete static_cast<std::mutex*>(*mutex);
                *mutex = nullptr;
            },
            [](mbedtls_threading_mutex_t* mutex)
            {
                if (!mutex || !*mutex)
                    return MBEDTLS_ERR_THREADING_BAD_INPUT_DATA;

                static_cast<std::mutex*>(*mutex)->lock();
                return 0;
            },
            [](mbedtls_threading_mutex_t* mutex)
            {
                if (!mutex || !*mutex)
                    return MBEDTLS_ERR_THREADING_BAD_INPUT_DATA;

                static_cast<std::mutex*>(*mutex)->unlock();
                return 0;
            });
#endif
    }
};

const WebMbedtlsThreading webMbedtlsThreading;
} // namespace

EM_JS_DEPS(sfmlWebDependencies, "$stringToNewUTF8,$stringToUTF8,$UTF8ToString");

// JavaScript bodies must not be formatted as C++.
// clang-format off
EM_JS(void, sfmlWebInit, (), {
    if (globalThis.sfmlNet)
        return;

    const sockets = new Map();
    let nextId = 1;

    function wake(sock)
    {
        const waiters = Array.from(sock.waiters);
        for (const waiter of waiters)
            waiter();
    }

    function waitUntil(sock, predicate, timeoutMs)
    {
        if (predicate())
            return Promise.resolve(true);

        return new Promise((resolve) =>
        {
            let timer = null;
            let settled = false;
            const finish = (value) =>
            {
                if (settled)
                    return;
                settled = true;
                sock.waiters.delete(onWake);
                if (timer !== null)
                    clearTimeout(timer);
                resolve(value);
            };
            const onWake = () =>
            {
                if (predicate())
                    finish(true);
            };
            sock.waiters.add(onWake);
            if (timeoutMs >= 0)
                timer = setTimeout(() => finish(false), timeoutMs);
            if (predicate())
                finish(true);
        });
    }

    function toBytes(data)
    {
        if (data instanceof Uint8Array)
            return data;
        if (data instanceof ArrayBuffer)
            return new Uint8Array(data);
        if (ArrayBuffer.isView(data))
            return new Uint8Array(data.buffer, data.byteOffset, data.byteLength);
        if (typeof data === 'string')
            return new TextEncoder().encode(data);
        return new Uint8Array();
    }

    function pendingBytes(sock)
    {
        let count = 0;
        for (const chunk of sock.chunks)
            count += chunk.data.length - chunk.offset;
        return count;
    }

    function readBytes(sock, destination, size)
    {
        let written = 0;
        while (sock.chunks.length > 0 && written < size)
        {
            const chunk = sock.chunks[0];
            const take = Math.min(size - written, chunk.data.length - chunk.offset);
            HEAPU8.set(chunk.data.subarray(chunk.offset, chunk.offset + take), destination + written);
            chunk.offset += take;
            written += take;
            if (chunk.offset >= chunk.data.length)
                sock.chunks.shift();
        }
        return written;
    }

    function enqueue(sock, data, connection)
    {
        const store = (bytes) =>
        {
            if (sock.ws !== connection)
                return;
            if (bytes.length > 0)
            {
                sock.receivedApp = true;
                sock.chunks.push({data: bytes, offset: 0});
            }
            wake(sock);
        };
        if (data && typeof data.arrayBuffer === 'function' && !(data instanceof Uint8Array))
        {
            data.arrayBuffer().then((buffer) => store(new Uint8Array(buffer))).catch(() =>
            {
                if (sock.ws !== connection)
                    return;
                sock.failed = true;
                wake(sock);
            });
            return;
        }
        store(toBytes(data));
    }

    function formatHost(host)
    {
        if (host.includes(':') && !host.startsWith('['))
            return '[' + host + ']';
        return host;
    }

    function webSocketUrl(transport, host, port, secure)
    {
        secure = !!secure || (globalThis.location && globalThis.location.protocol === 'https:');
        secure = !!secure;
        const config = Module['sfmlNetwork'];
        const resolver = config && config['resolveWebSocketUrl'];
        if (resolver !== undefined && typeof resolver !== 'function')
            throw new TypeError('resolveWebSocketUrl must be a function');
        const value = resolver
            ? resolver({transport, host, port, secure})
            : (secure ? 'wss' : 'ws') + '://' + formatHost(host) + ':' + port + '/';
        if (typeof value !== 'string')
            throw new TypeError('resolveWebSocketUrl must return an absolute WebSocket URL');
        const url = new URL(value);
        if ((url.protocol !== 'ws:' && url.protocol !== 'wss:') || url.hash || url.username || url.password ||
            (secure && url.protocol !== 'wss:'))
            throw new TypeError('Invalid or insecure WebSocket URL');
        return url;
    }

    function closeTcp(sock)
    {
        if (sock.ws)
        {
            const previous = sock.ws;
            sock.ws = null;
            previous.onopen = null;
            previous.onmessage = null;
            previous.onerror = null;
            previous.onclose = null;
            try { previous.close(); } catch (error) {}
        }
    }

    function openTcp(sock, host, port, secure, endpoint)
    {
        closeTcp(sock);

        sock.host = host;
        sock.port = port;
        sock.secure = !!secure;
        sock.open = false;
        sock.failed = false;
        sock.closed = false;
        sock.sentApp = false;
        sock.receivedApp = false;
        sock.chunks = [];
        sock.url = "";

        if (!host || !globalThis.WebSocket)
        {
            sock.failed = true;
            sock.closed = true;
            return;
        }

        try
        {
            const url = endpoint || webSocketUrl('tcp', host, port, secure);
            const ws = new WebSocket(url.href, ['sfml.tcp']);
            sock.ws = ws;
            sock.url = url.href;
            sock.secure = url.protocol === 'wss:';
            ws.binaryType = 'arraybuffer';
            ws.onopen = () =>
            {
                sock.open = true;
                wake(sock);
            };
            ws.onmessage = (event) => enqueue(sock, event.data, ws);
            ws.onerror = () =>
            {
                sock.failed = true;
                wake(sock);
            };
            ws.onclose = () =>
            {
                const wasOpen = sock.open;
                sock.open = false;
                sock.closed = true;
                if (!wasOpen)
                    sock.failed = true;
                wake(sock);
            };
        }
        catch (error)
        {
            sock.failed = true;
            sock.closed = true;
        }
        wake(sock);
    }

    function udpFailed(sock)
    {
        if (sock.closed)
            return true;
        if (sock.peers.size === 0)
            return false;
        for (const peer of sock.peers.values())
            if (!peer.failed)
                return false;
        return true;
    }

    function endPeer(sock, peer)
    {
        peer.open = false;
        peer.failed = true;
        for (const object of [peer.ws, peer.channel, peer.pc])
        {
            if (!object)
                continue;
            object.onopen = null;
            object.onmessage = null;
            object.onerror = null;
            object.onclose = null;
            object.onicecandidate = null;
            object.onconnectionstatechange = null;
            object.oniceconnectionstatechange = null;
            try { object.close(); } catch (error) {}
        }
        peer.ws = null;
        peer.channel = null;
        peer.pc = null;
        wake(sock);
    }

    function readable(sock)
    {
        if (!sock)
            return false;
        if (sock.kind === 'udp')
            return sock.datagrams.length > 0 || udpFailed(sock);
        return pendingBytes(sock) > 0 || sock.closed || sock.failed;
    }

    function writable(sock)
    {
        if (!sock)
            return false;
        if (sock.kind === 'udp')
        {
            if (sock.closed)
                return true;
            if (sock.peers.size === 0)
                return true;
            for (const peer of sock.peers.values())
            {
                if (peer.open || peer.failed)
                    return true;
            }
            return false;
        }
        return sock.open || sock.failed;
    }

    function openPeer(sock, host, port)
    {
        const peer = {host, port, open: false, failed: false, channel: null, pc: null, ws: null};
        try
        {
            const PeerConnection = globalThis.RTCPeerConnection;
            if (!PeerConnection || !globalThis.WebSocket || !host)
                throw new Error('WebRTC or WebSocket is unavailable');
            const url = webSocketUrl('udp', host, port, false);

            const pc = new PeerConnection({iceServers: [{urls: 'stun:stun.l.google.com:19302'}]});
            peer.pc = pc;
            const failedConnection = () =>
            {
                if (pc.connectionState === 'failed' || pc.connectionState === 'closed' ||
                    pc.iceConnectionState === 'failed' || pc.iceConnectionState === 'closed')
                    endPeer(sock, peer);
            };
            pc.onconnectionstatechange = failedConnection;
            pc.oniceconnectionstatechange = failedConnection;
            const channel = pc.createDataChannel('sfml', {ordered: false, maxRetransmits: 0});
            peer.channel = channel;
            channel.binaryType = 'arraybuffer';
            channel.onopen = () =>
            {
                if (peer.failed || sock.closed)
                    return;
                peer.open = true;
                wake(sock);
            };
            channel.onclose = () => endPeer(sock, peer);
            channel.onerror = () => endPeer(sock, peer);
            channel.onmessage = (event) =>
            {
                const deliver = (bytes) =>
                {
                    if (peer.failed || sock.closed)
                        return;
                    sock.datagrams.push({data: bytes, host, port});
                    wake(sock);
                };
                const data = event.data;
                if (data && typeof data.arrayBuffer === 'function' && !(data instanceof Uint8Array))
                {
                    data.arrayBuffer().then((buffer) => deliver(new Uint8Array(buffer))).catch(() =>
                    {
                        endPeer(sock, peer);
                    });
                    return;
                }
                deliver(toBytes(data));
            };

            const ws = new WebSocket(url.href, ['sfml.udp']);
            peer.ws = ws;
            ws.binaryType = 'arraybuffer';
            pc.onicecandidate = (event) =>
            {
                if (!event.candidate || ws.readyState !== 1)
                    return;
                ws.send(JSON.stringify({
                    type: 'candidate',
                    candidate: event.candidate.candidate,
                    sdpMid: event.candidate.sdpMid,
                    sdpMLineIndex: event.candidate.sdpMLineIndex
                }));
            };
            ws.onmessage = (event) =>
            {
                const parse = async (data) =>
                {
                    let text = data;
                    if (data instanceof ArrayBuffer)
                        text = new TextDecoder().decode(data);
                    else if (data && typeof data.text === 'function')
                        text = await data.text();
                    if (peer.failed || sock.closed)
                        return;
                    const message = JSON.parse(text);
                    if (message.type === 'answer')
                        await pc.setRemoteDescription({type: 'answer', sdp: message.sdp});
                    else if (message.type === 'candidate' && message.candidate)
                        await pc.addIceCandidate({
                            candidate: message.candidate,
                            sdpMid: message.sdpMid || "",
                            sdpMLineIndex: message.sdpMLineIndex || 0
                        });
                    wake(sock);
                };
                parse(event.data).catch(() =>
                {
                    endPeer(sock, peer);
                });
            };
            ws.onopen = async () =>
            {
                try
                {
                    const offer = await pc.createOffer();
                    if (peer.failed || sock.closed)
                        return;
                    await pc.setLocalDescription(offer);
                    if (!peer.failed && !sock.closed && ws.readyState === 1)
                        ws.send(JSON.stringify({type: 'offer', sdp: pc.localDescription.sdp}));
                }
                catch (error)
                {
                    endPeer(sock, peer);
                }
            };
            ws.onerror = () =>
            {
                if (!peer.open)
                    endPeer(sock, peer);
            };
            ws.onclose = () =>
            {
                if (!peer.open)
                    endPeer(sock, peer);
            };
        }
        catch (error)
        {
            endPeer(sock, peer);
        }
        return peer;
    }

    globalThis.sfmlNet = {
        create(kind)
        {
            const id = nextId++;
            sockets.set(id, {
                id,
                kind,
                ws: null,
                open: false,
                failed: false,
                closed: false,
                sentApp: false,
                receivedApp: false,
                tlsHost: null,
                url: "",
                host: "",
                port: 0,
                secure: false,
                chunks: [],
                waiters: new Set(),
                bound: false,
                localPort: 0,
                peers: new Map(),
                datagrams: []
            });
            return id;
        },

        close(id)
        {
            const sock = sockets.get(id);
            if (!sock)
                return;
            sock.closed = true;
            sock.open = false;
            closeTcp(sock);
            for (const peer of sock.peers.values())
                endPeer(sock, peer);
            wake(sock);
            sockets.delete(id);
        },

        async tcpConnect(id, host, port, secure, wait, timeoutMs)
        {
            const sock = sockets.get(id);
            if (!sock)
                return 4;
            sock.tlsHost = null;
            openTcp(sock, host, port, secure);
            if (sock.failed)
                return 4;
            if (!wait)
                return sock.open ? 0 : 1;
            const opened = await waitUntil(sock, () => sock.open || sock.failed || sock.closed, timeoutMs);
            if (sock.open)
                return 0;
            return opened ? 4 : 1;
        },

        async tcpSecure(id, host, wait, timeoutMs)
        {
            const sock = sockets.get(id);
            if (!sock || !sock.ws)
                return 2;
            const requestedHost = host || sock.host;
            const tlsHost = requestedHost.toLowerCase();
            if (sock.tlsHost !== null && sock.tlsHost !== tlsHost)
                return 4;
            if (sock.failed || sock.closed)
                return 4;
            if (sock.tlsHost === null)
            {
                let url;
                try { url = webSocketUrl('tcp', requestedHost, sock.port, true); }
                catch (error) { return 4; }
                if (sock.url !== url.href || !sock.secure)
                {
                    if (sock.secure || sock.sentApp || sock.receivedApp || pendingBytes(sock) > 0)
                        return 4;
                    sock.tlsHost = tlsHost;
                    openTcp(sock, requestedHost, sock.port, true, url);
                }
                else
                    sock.tlsHost = tlsHost;
            }
            if (sock.failed || sock.closed)
                return 4;
            if (!wait)
                return sock.open ? 0 : 1;
            const opened = await waitUntil(sock, () => sock.open || sock.failed || sock.closed, timeoutMs);
            if (sock.open)
                return 0;
            return opened ? 4 : 1;
        },

        async tcpSend(id, data, size, wait)
        {
            const sock = sockets.get(id);
            if (!sock || !sock.ws)
                return {status: 4, sent: 0};
            if (!sock.open)
            {
                if (sock.failed || sock.closed)
                    return {status: 4, sent: 0};
                if (!wait)
                    return {status: 1, sent: 0};
                await waitUntil(sock, () => sock.open || sock.failed || sock.closed, -1);
                if (!sock.open)
                    return {status: sock.failed ? 4 : 3, sent: 0};
            }
            if (sock.ws.readyState !== 1)
                return {status: 3, sent: 0};
            try
            {
                sock.ws.send(HEAPU8.slice(data, data + size));
            }
            catch (error)
            {
                return {status: 4, sent: 0};
            }
            if (size > 0)
                sock.sentApp = true;
            return {status: 0, sent: size};
        },

        async tcpReceive(id, data, size, wait)
        {
            const sock = sockets.get(id);
            if (!sock || !sock.ws)
                return {status: 4, received: 0};
            if (pendingBytes(sock) === 0)
            {
                if (sock.closed || sock.failed)
                    return {status: 3, received: 0};
                if (!wait)
                    return {status: 1, received: 0};
                await waitUntil(sock, () => pendingBytes(sock) > 0 || sock.closed || sock.failed, -1);
            }
            if (pendingBytes(sock) > 0)
                return {status: 0, received: readBytes(sock, data, size)};
            if (sock.closed || sock.failed)
                return {status: 3, received: 0};
            return {status: 1, received: 0};
        },

        tcpOpen(id)
        {
            const sock = sockets.get(id);
            return !!(sock && sock.open && !sock.failed);
        },

        udpBind(id, port)
        {
            const sock = sockets.get(id);
            if (!sock)
                return 4;
            sock.bound = true;
            sock.localPort = port;
            return 0;
        },

        udpPort(id)
        {
            const sock = sockets.get(id);
            return sock ? sock.localPort : 0;
        },

        async udpSend(id, data, size, host, port, wait)
        {
            const sock = sockets.get(id);
            if (!sock || sock.closed)
                return 4;
            const key = host + '\n' + port;
            let peer = sock.peers.get(key);
            if (!peer)
            {
                peer = openPeer(sock, host, port);
                sock.peers.set(key, peer);
            }
            if (peer.failed)
                return 4;
            if (!peer.open)
            {
                if (!wait)
                    return 1;
                await waitUntil(sock, () => peer.open || peer.failed || sock.closed, -1);
                if (!peer.open || peer.failed || sock.closed)
                    return 4;
            }
            try
            {
                peer.channel.send(HEAPU8.slice(data, data + size));
            }
            catch (error)
            {
                return 4;
            }
            return 0;
        },

        async udpReceive(id, data, size, wait, outHost, outPort, outReceived)
        {
            const sock = sockets.get(id);
            if (!sock)
                return 4;
            if (sock.datagrams.length === 0)
            {
                if (udpFailed(sock))
                    return 4;
                if (!wait)
                    return 1;
                await waitUntil(sock, () => sock.datagrams.length > 0 || udpFailed(sock), -1);
            }
            if (sock.datagrams.length === 0)
                return udpFailed(sock) ? 4 : 1;
            const item = sock.datagrams.shift();
            const take = Math.min(size, item.data.length);
            if (take > 0)
                HEAPU8.set(item.data.subarray(0, take), data);
            const hostPtr = stringToNewUTF8(String(item.host));
            setValue(outHost, hostPtr, '*');
            setValue(outPort, item.port, 'i32');
            setValue(outReceived, take, 'i32');
            return 0;
        },

        isReadable(id)
        {
            return readable(sockets.get(id));
        },

        isWritable(id)
        {
            return writable(sockets.get(id));
        },

        async wait(ids, masks, timeoutMs)
        {
            const items = [];
            for (let index = 0; index < ids.length; ++index)
                items.push({id: ids[index], mask: masks[index]});
            const ready = () => items.some((item) =>
            {
                const sock = sockets.get(item.id);
                return ((item.mask & 1) && readable(sock)) || ((item.mask & 2) && writable(sock));
            });
            if (ready())
                return 1;
            if (items.length === 0)
            {
                if (timeoutMs < 0)
                    await new Promise(() => {});
                else
                    await new Promise((resolve) => setTimeout(resolve, timeoutMs));
                return 0;
            }
            return await new Promise((resolve) =>
            {
                const watchers = [];
                let timer = null;
                let settled = false;
                const finish = (value) =>
                {
                    if (settled)
                        return;
                    settled = true;
                    if (timer !== null)
                        clearTimeout(timer);
                    for (const watcher of watchers)
                        watcher.sock.waiters.delete(watcher.fn);
                    resolve(value);
                };
                for (const item of items)
                {
                    const sock = sockets.get(item.id);
                    if (!sock)
                        continue;
                    const fn = () =>
                    {
                        if (ready())
                            finish(1);
                    };
                    sock.waiters.add(fn);
                    watchers.push({sock, fn});
                }
                if (timeoutMs >= 0)
                    timer = setTimeout(() => finish(0), timeoutMs);
                if (ready())
                    finish(1);
            });
        },

        async fetch(url, method, headerText, body, bodyLength, hasBody, timeoutMs, outHeader, outBody, outBodyLength)
        {
            try
            {
                const headers = {};
                for (const line of headerText.split('\n'))
                {
                    let trimmed = line;
                    if (trimmed.length > 0 && trimmed.charCodeAt(trimmed.length - 1) === 13)
                        trimmed = trimmed.slice(0, -1);
                    const colon = trimmed.indexOf(':');
                    if (colon <= 0)
                        continue;
                    headers[trimmed.slice(0, colon).trim()] = trimmed.slice(colon + 1).trim();
                }
                const payload = hasBody ? (body ? HEAPU8.slice(body, body + bodyLength) : new Uint8Array()) : undefined;
                const init = {method, headers, body: payload, redirect: 'manual', cache: 'no-store'};
                if (timeoutMs > 0)
                {
                    if (typeof AbortSignal !== 'undefined' && typeof AbortSignal.timeout === 'function')
                        init.signal = AbortSignal.timeout(timeoutMs);
                    else
                    {
                        const controller = new AbortController();
                        setTimeout(() => controller.abort(), timeoutMs);
                        init.signal = controller.signal;
                    }
                }
                const response = await fetch(url, init);
                // Browsers hide manual redirects instead of exposing their 3xx status.
                if (response.type === 'opaqueredirect' || response.status === 0)
                    return -1;
                const headerLines = [];
                response.headers.forEach((value, name) =>
                {
                    if (name === 'transfer-encoding' || name === 'content-encoding')
                        return;
                    headerLines.push(name + ': ' + value);
                });
                const reason = response.statusText || 'Status';
                const raw = 'HTTP/1.1 ' + response.status + ' ' + reason + '\r\n' + headerLines.join('\r\n') + '\r\n\r\n';
                const bytes = new Uint8Array(await response.arrayBuffer());
                const headerPtr = stringToNewUTF8(raw);
                const bodyPtr = _malloc(bytes.length || 1);
                if (bytes.length > 0)
                    HEAPU8.set(bytes, bodyPtr);
                setValue(outHeader, headerPtr, '*');
                setValue(outBody, bodyPtr, '*');
                setValue(outBodyLength, bytes.length, 'i32');
                return 1;
            }
            catch (error)
            {
                return 0;
            }
        },

        async doh(url, query, queryLength, timeoutMs, outBytes, outLength)
        {
            try
            {
                const init = {
                    method: 'POST',
                    headers: {'content-type': 'application/dns-message', accept: 'application/dns-message'},
                    body: HEAPU8.slice(query, query + queryLength)
                };
                if (timeoutMs > 0)
                {
                    if (typeof AbortSignal !== 'undefined' && typeof AbortSignal.timeout === 'function')
                        init.signal = AbortSignal.timeout(timeoutMs);
                    else
                    {
                        const controller = new AbortController();
                        setTimeout(() => controller.abort(), timeoutMs);
                        init.signal = controller.signal;
                    }
                }
                const response = await fetch(url, init);
                if (!response.ok)
                    return 0;
                const bytes = new Uint8Array(await response.arrayBuffer());
                const ptr = _malloc(bytes.length || 1);
                if (bytes.length > 0)
                    HEAPU8.set(bytes, ptr);
                setValue(outBytes, ptr, '*');
                setValue(outLength, bytes.length, 'i32');
                return 1;
            }
            catch (error)
            {
                return 0;
            }
        }
    };
});

EM_ASYNC_JS(int, sfmlWebTcpConnect, (int id, const char* host, int port, int secure, int wait, int timeoutMs), {
    return await globalThis.sfmlNet.tcpConnect(id, UTF8ToString(host), port, secure, wait, timeoutMs);
});

EM_ASYNC_JS(int, sfmlWebTcpSecure, (int id, const char* host, int wait, int timeoutMs), {
    return await globalThis.sfmlNet.tcpSecure(id, UTF8ToString(host), wait, timeoutMs);
});

EM_ASYNC_JS(int, sfmlWebTcpSend, (int id, const void* data, int size, int wait, int* sent), {
    const result = await globalThis.sfmlNet.tcpSend(id, data, size, wait);
    setValue(sent, result.sent, 'i32');
    return result.status;
});

EM_ASYNC_JS(int, sfmlWebTcpReceive, (int id, void* data, int size, int wait, int* received), {
    const result = await globalThis.sfmlNet.tcpReceive(id, data, size, wait);
    setValue(received, result.received, 'i32');
    return result.status;
});

EM_JS(int, sfmlWebTcpOpen, (int id), {
    return globalThis.sfmlNet.tcpOpen(id) ? 1 : 0;
});

EM_JS(int, sfmlWebUdpBind, (int id, int port), {
    return globalThis.sfmlNet.udpBind(id, port);
});

EM_JS(int, sfmlWebUdpPort, (int id), {
    return globalThis.sfmlNet.udpPort(id);
});

EM_ASYNC_JS(int, sfmlWebUdpSend, (int id, const void* data, int size, const char* host, int port, int wait), {
    return await globalThis.sfmlNet.udpSend(id, data, size, UTF8ToString(host), port, wait);
});

EM_ASYNC_JS(int, sfmlWebUdpReceive, (int id, void* data, int size, int wait, char** host, int* port, int* received), {
    return await globalThis.sfmlNet.udpReceive(id, data, size, wait, host, port, received);
});

EM_JS(int, sfmlWebIsReadable, (int id), {
    return globalThis.sfmlNet.isReadable(id) ? 1 : 0;
});

EM_JS(int, sfmlWebIsWritable, (int id), {
    return globalThis.sfmlNet.isWritable(id) ? 1 : 0;
});

EM_ASYNC_JS(int, sfmlWebWait, (const int* handles, const int* masks, int count, int timeoutMs), {
    const ids = [];
    const bits = [];
    for (let index = 0; index < count; ++index)
    {
        ids.push(HEAP32[(handles >> 2) + index]);
        bits.push(HEAP32[(masks >> 2) + index]);
    }
    return await globalThis.sfmlNet.wait(ids, bits, timeoutMs);
});

EM_ASYNC_JS(int, sfmlWebFetch, (const char* url, const char* method, const char* headers, const char* body, int bodyLength, int hasBody, int timeoutMs, char** outHeader, unsigned char** outBody, int* outBodyLength), {
    return await globalThis.sfmlNet.fetch(UTF8ToString(url), UTF8ToString(method), UTF8ToString(headers), body, bodyLength, hasBody, timeoutMs, outHeader, outBody, outBodyLength);
});

EM_ASYNC_JS(int, sfmlWebDoh, (const char* url, const unsigned char* query, int queryLength, int timeoutMs, unsigned char** outBytes, int* outLength), {
    return await globalThis.sfmlNet.doh(UTF8ToString(url), query, queryLength, timeoutMs, outBytes, outLength);
});

EM_JS(int, sfmlWebCreate, (int kind), {
    return globalThis.sfmlNet.create(kind ? 'udp' : 'tcp');
});

EM_JS(void, sfmlWebClose, (int id), {
    globalThis.sfmlNet.close(id);
});
// clang-format on


namespace
{
void ensureWeb()
{
    static const bool ready = []
    {
        sfmlWebInit();
        return true;
    }();
    (void)ready;
}

[[nodiscard]] int checkedSize(std::size_t size)
{
    if (size > static_cast<std::size_t>(std::numeric_limits<int>::max()))
        return -1;
    return static_cast<int>(size);
}

struct FreeDeleter
{
    void operator()(void* pointer) const
    {
        std::free(pointer); // NOLINT(cppcoreguidelines-no-malloc)
    }
};

template <typename T>
using MallocPointer = std::unique_ptr<T, FreeDeleter>;
} // namespace


namespace sf::priv
{
////////////////////////////////////////////////////////////
void webSkip(const char* message)
{
    static std::array<const char*, 16> seen{}; // NOLINT(misc-const-correctness)
    static std::size_t                 count = 0;
    for (std::size_t index = 0; index < count; ++index)
    {
        if (seen[index] == message)
            return;
    }

    if (count < seen.size())
        seen[count++] = message;

    err() << message << " is not available in the web build and was skipped" << std::endl;
}


////////////////////////////////////////////////////////////
SocketHandle webCreateTcp()
{
    ensureWeb();
    return sfmlWebCreate(0);
}


////////////////////////////////////////////////////////////
SocketHandle webCreateUdp()
{
    ensureWeb();
    return sfmlWebCreate(1);
}


////////////////////////////////////////////////////////////
void webClose(SocketHandle handle)
{
    if (handle < 0)
        return;

    ensureWeb();
    sfmlWebClose(static_cast<int>(handle));
}


////////////////////////////////////////////////////////////
int webTcpConnect(SocketHandle handle, const char* host, unsigned short port, bool secure, bool wait, int timeoutMs)
{
    ensureWeb();
    return sfmlWebTcpConnect(static_cast<int>(handle), host, port, secure ? 1 : 0, wait ? 1 : 0, timeoutMs);
}


////////////////////////////////////////////////////////////
int webTcpSecure(SocketHandle handle, const char* host, bool wait, int timeoutMs)
{
    ensureWeb();
    return sfmlWebTcpSecure(static_cast<int>(handle), host, wait ? 1 : 0, timeoutMs);
}


////////////////////////////////////////////////////////////
int webTcpSend(SocketHandle handle, const void* data, std::size_t size, std::size_t& sent, bool wait)
{
    ensureWeb();
    sent            = 0;
    const int count = checkedSize(size);
    if (count < 0 || (!data && count > 0))
        return 4;

    int       written = 0;
    const int status  = sfmlWebTcpSend(static_cast<int>(handle), data, count, wait ? 1 : 0, &written);
    if (written > 0)
        sent = static_cast<std::size_t>(written);
    return status;
}


////////////////////////////////////////////////////////////
int webTcpReceive(SocketHandle handle, void* data, std::size_t size, std::size_t& received, bool wait)
{
    ensureWeb();
    received        = 0;
    const int count = checkedSize(size);
    if (count < 0 || !data)
        return 4;

    int       got    = 0;
    const int status = sfmlWebTcpReceive(static_cast<int>(handle), data, count, wait ? 1 : 0, &got);
    if (got > 0)
        received = static_cast<std::size_t>(got);
    return status;
}


////////////////////////////////////////////////////////////
bool webTcpIsOpen(SocketHandle handle)
{
    if (handle < 0)
        return false;

    ensureWeb();
    return sfmlWebTcpOpen(static_cast<int>(handle)) != 0;
}


////////////////////////////////////////////////////////////
int webUdpBind(SocketHandle handle, unsigned short port)
{
    ensureWeb();
    return sfmlWebUdpBind(static_cast<int>(handle), port);
}


////////////////////////////////////////////////////////////
unsigned short webUdpLocalPort(SocketHandle handle)
{
    if (handle < 0)
        return 0;

    ensureWeb();
    return static_cast<unsigned short>(sfmlWebUdpPort(static_cast<int>(handle)));
}


////////////////////////////////////////////////////////////
int webUdpSend(SocketHandle handle, const void* data, std::size_t size, const char* host, unsigned short port, bool wait)
{
    ensureWeb();
    const int count = checkedSize(size);
    if (count < 0 || (!data && count > 0))
        return 4;

    return sfmlWebUdpSend(static_cast<int>(handle), data, count, host, port, wait ? 1 : 0);
}


////////////////////////////////////////////////////////////
int webUdpReceive(SocketHandle    handle,
                  void*           data,
                  std::size_t     size,
                  std::size_t&    received,
                  std::string&    host,
                  unsigned short& port,
                  bool            wait)
{
    ensureWeb();
    received = 0;
    host.clear();
    port            = 0;
    const int count = checkedSize(size);
    if (count < 0 || !data)
        return 4;

    char*     hostRaw = nullptr;
    int       gotPort = 0;
    int       got     = 0;
    const int status = sfmlWebUdpReceive(static_cast<int>(handle), data, count, wait ? 1 : 0, &hostRaw, &gotPort, &got);
    const MallocPointer<char> hostOwner(hostRaw);
    if (hostOwner)
        host = hostOwner.get();
    if (status == 0)
    {
        received = static_cast<std::size_t>(got);
        port     = static_cast<unsigned short>(gotPort);
    }
    return status;
}


////////////////////////////////////////////////////////////
bool webIsReadable(SocketHandle handle)
{
    if (handle < 0)
        return false;

    ensureWeb();
    return sfmlWebIsReadable(static_cast<int>(handle)) != 0;
}


////////////////////////////////////////////////////////////
bool webIsWritable(SocketHandle handle)
{
    if (handle < 0)
        return false;

    ensureWeb();
    return sfmlWebIsWritable(static_cast<int>(handle)) != 0;
}


////////////////////////////////////////////////////////////
int webWait(const int* handles, const int* masks, int count, int timeoutMs)
{
    ensureWeb();
    return sfmlWebWait(handles, masks, count, timeoutMs);
}


////////////////////////////////////////////////////////////
bool webFetch(const std::string& url,
              const std::string& method,
              const std::string& headers,
              const std::string* body,
              int                timeoutMs,
              std::string&       rawResponse)
{
    ensureWeb();
    rawResponse.clear();
    if (body && body->size() > static_cast<std::size_t>(std::numeric_limits<int>::max()))
        return false;

    char*                              headerRaw  = nullptr;
    unsigned char*                     payloadRaw = nullptr;
    int                                payloadLen = 0;
    const int                          ok         = sfmlWebFetch(url.c_str(),
                                method.c_str(),
                                headers.c_str(),
                                body ? body->data() : nullptr,
                                body ? static_cast<int>(body->size()) : 0,
                                body ? 1 : 0,
                                timeoutMs,
                                &headerRaw,
                                &payloadRaw,
                                &payloadLen);
    const MallocPointer<char>          header(headerRaw);
    const MallocPointer<unsigned char> payload(payloadRaw);
    if (ok == -1)
        webSkip("Http::sendRequest with an opaque browser response or redirect");
    if (ok != 1)
        return false;

    if (header)
        rawResponse = header.get();
    if (payload && payloadLen > 0)
        rawResponse.append(reinterpret_cast<const char*>(payload.get()), static_cast<std::size_t>(payloadLen));
    return true;
}


////////////////////////////////////////////////////////////
std::optional<std::vector<std::uint8_t>> webDoh(const std::string& url, const std::uint8_t* query, std::size_t querySize, int timeoutMs)
{
    ensureWeb();
    if (!query || querySize > static_cast<std::size_t>(std::numeric_limits<int>::max()))
        return std::nullopt;

    unsigned char* bytesRaw = nullptr;
    int            length   = 0;
    const int      ok = sfmlWebDoh(url.c_str(), query, static_cast<int>(querySize), timeoutMs, &bytesRaw, &length);
    const MallocPointer<unsigned char> bytes(bytesRaw);
    if (!ok || !bytes)
        return std::nullopt;

    std::vector<std::uint8_t> result;
    if (length > 0)
        result.assign(bytes.get(), bytes.get() + length);
    return result;
}
} // namespace sf::priv
