// Test double for RTCPeerConnection. Node has no WebRTC implementation.
// Two peers created by one process are paired after both have a DataChannel
// and a remote description. send() delivers one copy to the other channel.
const sfmlFakePeers = [];

function sfmlFakeBytes(data)
{
    if (data instanceof ArrayBuffer)
        return new Uint8Array(data);
    if (ArrayBuffer.isView(data))
        return new Uint8Array(data.buffer, data.byteOffset, data.byteLength);
    return new Uint8Array();
}

function sfmlFakeTryPair()
{
    const ready = sfmlFakePeers.filter((peer) => peer._channels.length > 0 && peer.remoteDescription && !peer._paired);
    if (ready.length < 2)
        return;

    const left         = ready[0];
    const right        = ready[1];
    left._paired       = true;
    right._paired      = true;
    const leftChannel  = left._channels[0];
    const rightChannel = right._channels[0];
    leftChannel._other = rightChannel;
    rightChannel._other = leftChannel;
    leftChannel.readyState  = 'open';
    rightChannel.readyState = 'open';
    setTimeout(() =>
    {
        if (leftChannel.onopen)
            leftChannel.onopen();
        if (rightChannel.onopen)
            rightChannel.onopen();
    }, 0);
}

class SfmlFakeRTCPeerConnection
{
    constructor()
    {
        this.onicecandidate    = null;
        this.localDescription  = null;
        this.remoteDescription = null;
        this._channels         = [];
        this._paired           = false;
        sfmlFakePeers.push(this);
    }

    createDataChannel(label)
    {
        const channel = {
            label,
            readyState: 'connecting',
            binaryType: 'arraybuffer',
            onopen: null,
            onmessage: null,
            onclose: null,
            onerror: null,
            _other: null,
            send(data)
            {
                const other = this._other;
                if (!other || !other.onmessage)
                    return;
                const copy = sfmlFakeBytes(data).slice();
                setTimeout(() => other.onmessage({data: copy.buffer}), 0);
            },
            close()
            {
                this.readyState = 'closed';
                if (this.onclose)
                    this.onclose();
            }
        };
        this._channels.push(channel);
        sfmlFakeTryPair();
        return channel;
    }

    createOffer()
    {
        return Promise.resolve({type: 'offer', sdp: 'v=0\r\n'});
    }

    setLocalDescription(description)
    {
        this.localDescription = description;
        if (this.onicecandidate)
            setTimeout(() => this.onicecandidate({candidate: null}), 0);
        return Promise.resolve();
    }

    setRemoteDescription(description)
    {
        this.remoteDescription = description;
        sfmlFakeTryPair();
        return Promise.resolve();
    }

    addIceCandidate()
    {
        return Promise.resolve();
    }

    close()
    {
    }
}

globalThis.RTCPeerConnection = SfmlFakeRTCPeerConnection;
