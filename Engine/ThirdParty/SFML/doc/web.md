# Building SFML for the browser

The Web platform uses Emscripten 7.0.0, WebGL 2, and static libraries. It supports
one SFML window on one canvas per page, the Graphics module, the Audio module
with the browser's default playback and recording devices, and the Network module
through the browser transports described below. No additional SFML API is required.

## Build and run

Install and activate Emscripten 7.0.0, then enter its environment (`emsdk_env.bat`
on Windows or `source emsdk_env.sh` on Linux/macOS). The `EMSDK` environment
variable must identify the SDK directory. CMake 3.28+, Ninja and Python are also
required. An existing Node installation can be selected through the SDK's
`NODE_JS` configuration; this does not require replacing the system Node.

```sh
cmake --preset web
cmake --build build/web --parallel
cmake --install build/web
npx --yes http-server build/web/bin -a 127.0.0.1 -p 8080 -c-1
```

Open `http://127.0.0.1:8080/tennis.html`. The example uses the existing tennis C++
source without a browser-specific main loop. Click the canvas to enable audio,
press Space to start, use the arrow keys to play, and Escape to close the SFML
window. The canvas is hidden on close. When tennis returns from `main()`, its Web
runtime shuts down and the page shows an exit message with a Restart button.
Closing the SFML window does not close the browser tab; library users can still
recreate a window on the same canvas. Tennis enables `EXIT_RUNTIME` only for its
own executable, without imposing application shutdown on other SFML consumers.

Use `-DCMAKE_BUILD_TYPE=Debug` for a debug build (`tennis-d.html`). Only the tennis example is built
for Web; examples requiring terminal input, extra windows, or networking are not
included. The existing all-module Catch2 suite remains a native test suite.

## Applications and resources

Compile the application with the same Emscripten SDK. For an installed SFML, set
`SFML_STATIC_LIBRARIES=ON` and `SFML_DIR` to the installation's `lib/cmake/SFML`
directory, then link the usual `SFML::Graphics` and `SFML::Audio` targets.
The targets propagate WebGL 2, Asyncify, exception handling and memory-growth
requirements. Do not override these with incompatible application linker flags.

The host page supplies `Module.canvas`, defaulting to an element with ID `canvas`.
See `examples/web/shell.html` for a minimal host. `sf::WindowHandle` is the CSS
selector of this canvas. Another canvas or a second active SFML window is rejected.
Textures may be loaded before opening the window, and resources survive closing
and reopening that window. Multiple `sf::RenderTexture` objects are supported.

Use Emscripten's `--preload-file path@virtual-path` on the application's final
link to package images, fonts and audio. Files are loaded before `main()`, so the
existing SFML file-loading APIs work unchanged. Memory-loading APIs and C/C++
file writes operate on the virtual filesystem. Its contents are lost on reload.
Deploy the HTML, JavaScript, Wasm and data files together through HTTP(S), serving
Wasm as `application/wasm`; opening the HTML as a local file is not supported.

Cookies, localStorage and sessionStorage belong to the host page. Use their normal
browser APIs there, and use Emscripten's existing JavaScript/C++ interop or `FS`
operations when the application needs those values. SFML does not map these stores
to files or automatically persist virtual files. Browser storage availability and
origin rules still apply. Host resource downloads are independent of SFML Network.

## Runtime behavior

- Keep the normal `while (window.isOpen())` loop. `display()`, `waitEvent()` and
  `sf::sleep()` yield through Asyncify so the browser can render and dispatch
  events. A loop that never reaches any yielding operation will still block the
  page. This build runs on the browser main thread and does not require pthreads,
  SharedArrayBuffer, or cross-origin isolation headers.
- Vsync uses browser animation frames. Disabling vsync still yields on display;
  it cannot make the browser present faster than its own compositor. Frame limits
  account for time already spent waiting. Background tabs are subject to browser
  throttling. Do not invoke a yielding SFML operation from an audio callback or
  reenter it from a JavaScript callback while an Asyncify wait is pending.
- Window sizes and mouse coordinates use drawing-buffer pixels. CSS size and
  device-pixel ratio changes update the buffer and produce resize events.
  Keyboard/scancode mappings cover common desktop keys; layout discovery and IME
  composition are not provided. Focus requires the visible, active browser tab
  and its focused canvas. Leaving or returning to the tab updates focus events;
  focus loss clears pressed-key/button state.
- Fullscreen and pointer lock are browser-controlled and may be deferred until a
  user gesture. Pointer locking does not provide desktop cursor warping. Native
  window positioning, icons, custom pixel cursors and sensors are not implemented.
  Clipboard access uses
  browser permissions; denial returns an empty string or leaves the clipboard
  unchanged.
- Touch events and `sf::Touch` queries use stable finger slots while each touch
  remains active. Coordinates use the same canvas drawing-buffer pixels as the
  mouse. The canvas disables browser touch gestures and compatibility mouse events
  so one touch is not also reported as a mouse click. A browser cancellation,
  hidden page, lost focus, hidden window or lost graphics context releases active
  touches through the existing `sf::Event::TouchEnded` event. Applications that
  distinguish cancellation from release can observe the canvas `touchcancel`
  event in their host page. Cancelling a touch alone does not produce a focus event.
- Gamepads with the browser's `standard` mapping are exposed through the existing
  joystick API. The browser index is the SFML joystick index, up to
  `sf::Joystick::Count`; unsupported mappings are not reported as connected.
  Button numbers retain the standard browser numbering: 0–3 are A/B/X/Y, 4–5
  are shoulders, 8–9 are View/Menu, 10–11 are stick clicks and 16 is Home.
  Browser axes 0–3 map to X/Y/Z/R in the range −100 to 100. Trigger buttons 6–7
  also map to U/V from 0 to 100. The directional pad maps to PovX (right minus
  left) and PovY (up minus down), each from −100 to 100. No additional dead zone
  is applied. Browser permissions and user-interaction requirements determine
  when a connected gamepad first becomes visible.
- WebGL 1 and geometry shaders are not supported. GLSL shaders must target GLES3 /
  WebGL 2. Actual depth, stencil and antialiasing settings are reported by the
  context; the browser controls the default framebuffer. A lost context stops
  drawing and queues the existing `Closed` event. Reload the page to recover GPU
  resources; transparent context restoration is not implemented.
- Audio is unlocked by user interaction. Sound, Music, SoundStream, Listener,
  effects and the existing audio codecs use the existing mixer and codecs.
  This main-thread implementation uses miniaudio's ScriptProcessorNode path;
  browsers may report a deprecation warning. Long uninterrupted C++ work can
  interrupt playback, so continue yielding regularly.
- Recording uses the default microphone and requires localhost or HTTPS plus
  browser permission. `SoundRecorder::start()` waits for the permission decision
  while allowing browser events to run, and returns false on denial/failure.
  Stop/destruction releases microphone tracks. Only default input/output devices
  are exposed; unknown device selections fail. Null playback remains available
  through a silent Web Audio output, with the same browser activation rules.

## Network

Public headers stay the same. Platform code lives in two sibling directories under `src/SFML/Network`: `Native/` and `Emscripten/`. The files beside those directories (`Socket.cpp`, `Http.cpp`, `IpAddress.cpp`, `Packet.cpp`, `Ftp.cpp`, `Sftp.cpp`) keep only the behavior that both backends share. `src/SFML/Network/Backend.hpp` is the list of functions a backend fills in: creating and closing a socket, sending one HTTP request, and reading the local address. Each backend also supplies the full bodies of `Dns`, `TcpSocket`, `UdpSocket`, and `SocketSelector`. A backend that cannot accept connections leaves out `TcpListener.cpp` and hides `sf::TcpListener` in the public headers. `Native/Unix` and `Native/Win32` stay inside the native backend; they are operating-system details, not a third transport.

The web build uses one implementation in
the browser and under Node, so a local Node test exercises the page's path.
Blocking calls wait through Asyncify. Non-blocking calls still return
`sf::Socket::Status::NotReady`. `SocketSelector::wait` waits the same way.
`Time::Zero` keeps its existing meaning: a blocking connect or selector wait
does not time out. `SocketHandle` remains an `int`, but it identifies an
internal socket rather than an operating-system descriptor.
Selector callbacks may add, remove, replace, or clear registrations. Dispatch
uses a snapshot: canceled or replaced callbacks are skipped, and newly added
registrations are considered after the next wait.

`fetch` and WebSocket calls are subject to CORS, mixed content, and the
browser's certificate policy. An HTTPS page cannot open `ws://`. `fetch` cannot
turn certificate checks off or choose an address family.

WebSocket connections and WebRTC signaling use WSS by default on HTTPS pages,
and WS on HTTP pages or under Node. An explicit TLS request always requires
WSS. To use a certificate hostname or a non-root path with the existing
IP-based C++ socket API, configure the host's Module object before loading the
generated JavaScript:

```js
Module['sfmlNetwork'] = {
    resolveWebSocketUrl: ({transport, host, port, secure}) =>
        transport === 'tcp'
            ? 'wss://game.example.net/stream'
            : 'wss://game.example.net/signaling'
};
```

The optional callback receives `transport` (`tcp` or `udp`), the requested
`host` and `port`, and whether a secure connection is required. It must synchronously return
an absolute WS/WSS URL; `host` can be the IP from `connect` or the hostname from
`setupTlsClient`. Map both to the same endpoint when they identify one service.
Certificates are verified by the browser for the final URL's hostname.
The peer must still speak `sfml.tcp` or `sfml.udp`; this callback does not turn
an ordinary TCP/UDP server into a browser endpoint. Invalid URLs, callback or
constructor exceptions, and attempts to use WS when WSS is required return
the existing socket error status. The implementation never silently downgrades
WSS or reports success before the WebSocket handshake completes.

| SFML call | Web replacement |
| --- | --- |
| `sf::Http` | `fetch`, using the browser's own hostname resolution without a preliminary DoH query. Method, fields, body, timeout, and "do not follow redirects" are preserved. The response is turned back into an HTTP message and parsed by the existing parser. `fetch` has already decoded the body, so transfer and content encodings are left out of that message. Ordinary HTTPS uses the browser trust store. |
| `sf::Dns` without a server list | DNS-over-HTTPS (`application/dns-message`) to `https://cloudflare-dns.com/dns-query`. `localhost` resolves to the loopback addresses locally. `SFML_DOH_URL` selects another endpoint for tests. |
| `sf::TcpSocket` connect, send, receive, and `sf::Packet` | WebSocket binary frames at the selected WS/WSS endpoint with subprotocol `sfml.tcp`. Frames are queued and read as a byte stream. |
| `TcpSocket::setupTlsClient(hostname)` with the default verifier | Reuses an existing WSS connection to the same resolved endpoint. A WS connection can be reopened as WSS only before application data is sent or received. Non-blocking calls return `HandshakeStarted` until the same handshake completes; repeated calls do not reconnect. Once TLS setup starts, changing its hostname requires disconnecting and reconnecting. |
| `sf::UdpSocket` send and receive | Unreliable, unordered WebRTC data channel named `sfml`. Signaling is an outbound WebSocket to the selected WS/WSS endpoint with subprotocol `sfml.udp`. The messages are JSON: `{"type":"offer","sdp":...}`, `{"type":"answer","sdp":...}`, and `{"type":"candidate","candidate":...,"sdpMid":...,"sdpMLineIndex":...}`. ICE uses `stun:stun.l.google.com:19302`. After the channel opens, the address and port still identify that peer. |
| `sf::Ftp` and `sf::Sftp` | They still call `sf::TcpSocket`, so their bytes use the `sfml.tcp` stream. SFTP encryption remains libssh2 and Mbed TLS. The peer must accept that WebSocket convention; it is not raw FTP or SSH on the port. |

Web `Http::setHost` does not check whether a hostname resolves. A successful
return accepts the host parameters; actual resolution and connection failures
are reported by `sendRequest`.

`UdpSocket::bind` records the requested port and returns `Done`. It does not
open a host port. `AnyPort` therefore stays `0`. A later `receive` only returns
datagrams from data channels that this socket has already opened.
Closed or failed channels wake pending sends and return `Error`; they are not
automatically reopened. Rebind or recreate the socket to reset those peers.
Receive drains queued datagrams first, then returns `Error` when all previously
opened peers have terminated. Other live peers continue to work. Local socket
closure also releases pending operations.

Browser fetch hides manual redirects as `opaqueredirect`: their real 3xx status
and Location cannot be read. Those responses, and other opaque responses with
status zero, return `Http::Response::Status::ConnectionFailed` with one diagnostic.
They are not followed or synthesized into an HTTP status-zero response. Under
Node, a readable manual 3xx response retains its status and headers.

SFTP's cryptographic entropy requires `crypto.getRandomValues`. If it is missing
or fails, entropy collection fails; there is no fallback to `Math.random`.

`sf::TcpListener` is not declared or compiled for web. A browser cannot listen
for or accept connections, and the type is absent rather than stubbed. Native
builds still provide it. Referencing `sf::TcpListener` in a web translation
unit is a compile error.

These calls stay on their classes, log one explanation through `sf::err()`, and
return the existing failure value:

- `TcpSocket::setupTlsServer`, both overloads: `TlsStatus::Error`.
- `TcpSocket::setupTlsClient` when the caller supplies a certificate, and when
  `verifyPeer` is false: `TlsStatus::Error`.
- `Http::sendRequest` when `verifyServer` is false: `ConnectionFailed`.
  `Http::setHost` or `sendRequest` with an address type: `setHost` returns
  false, and `sendRequest` returns `ConnectionFailed`.
- `Dns::resolve`, `queryNs`, `queryMx`, `querySrv`, and `queryTxt` when the
  caller passes DNS server addresses: an empty result.
- `IpAddress::getLocalAddress`: `std::nullopt`.
- `UdpSocket::send` to a broadcast or multicast address: `Socket::Status::Error`.

Build the web preset from the repository root, then run the programs with Node. The `.js` file and its `.wasm` stay side by side in `build/web/bin`.

Enable `SFML_BUILD_TEST_SUITE` to build the network test programs from a checkout that includes `test/`. The Catch2 network suite is `test-sfml-network.js`. Cases that need `sf::TcpListener` are left out of the web build. Internet cases stay off unless the corresponding CMake options are enabled.

```sh
cmake --preset web -DSFML_BUILD_TEST_SUITE=ON
cmake --build build/web --target test-sfml-network sfml-network-node-test --parallel
node build/web/bin/test-sfml-network.js
```

The network examples are the same sources as the native programs, linked for Node:

```sh
node build/web/bin/http.js
node build/web/bin/ftp.js
node build/web/bin/sftp.js
node build/web/bin/sockets.js
node build/web/bin/voip.js
```

`sockets.js` and `voip.js` can run as clients. Choosing the server role prints that `sf::TcpListener` is absent and returns. `ftp.js`, `sftp.js`, and a TCP sockets or VoIP client use the selected WS/WSS endpoint with subprotocol `sfml.tcp`; the peer has to speak that convention.

The smaller transport check still has its own runner:

```sh
node test/Network/emscripten/run.mjs build/web/bin/sfml-network-node-test.js
```

The script hosts HTTP, DNS-over-HTTPS, and the two WebSocket subprotocols. Its
WebRTC peer is a test double that completes signaling and exchanges one
datagram. Browser applications use the platform `RTCPeerConnection`. The web
test program does not name `sf::TcpListener`.

## Formatting and analysis

Use clang-format **17**, as required by `cmake/Format.cmake`. Web analysis uses
clang-tidy **21.1.6**, which can parse the SDK's libc++ headers. Supply its path
with `-DCLANG_TIDY_EXECUTABLE=...` and make `run-clang-tidy` available on PATH.
The Web CI job passes `CLANG_TIDY_CHECKS` filters for four compatibility checks:
`portability-avoid-pragma-once`, `cppcoreguidelines-use-enum-class`,
`bugprone-dynamic-static-initializers`, and
`readability-use-concise-preprocessor-directives`. These preserve the project's
existing header conventions and public API under Clang 21. Other checks and
warnings-as-errors remain enabled. The option defaults to empty, so native jobs
continue using the shared `.clang-tidy` configuration unchanged.

```sh
cmake --build build/web --target format
cmake --preset web -DCMAKE_UNITY_BUILD=ON -DCLANG_TIDY_EXECUTABLE=/path/to/clang-tidy
cmake --build build/web --target tidy
```

As in the native analyze jobs, analysis uses a Unity build. The tidy adapter supplies the Wasm target, SDK sysroot and compatibility include
paths in a separate compilation database. It retains the project's checks and
warnings-as-errors settings and analyzes the Web implementation. JavaScript bodies
inside Emscripten macros are locally excluded from C++ formatting to preserve their
syntax. Existing native build, format and analyze jobs remain in place.
