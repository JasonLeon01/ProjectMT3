////////////////////////////////////////////////////////////
//
// SFML 3.1.0-ME OpenHarmony/HarmonyOS diagnostics example
//
////////////////////////////////////////////////////////////

#define VK_USE_PLATFORM_OHOS
#include <SFML/Graphics.hpp>

#include <SFML/Audio.hpp>

#include <SFML/Network.hpp>

#include <SFML/Window.hpp>

#include <SFML/System/FileInputStream.hpp>
#include <SFML/System/Sleep.hpp>

#include <EGL/egl.h>
#include <SFML/Main.hpp>
#include <SFML/OpenGL.hpp>
#include <algorithm>
#include <array>
#include <atomic>
#include <filesystem>
#include <fstream>
#include <hilog/log.h>
#include <iomanip>
#include <mutex>
#include <optional>
#include <set>
#include <sstream>
#include <string>
#include <string_view>
#include <system_error>
#include <thread>
#include <unordered_set>
#include <utility>
#include <vector>
#include <vulkan/vulkan.h>

#include <cmath>
#include <cstdint>
#include <cstdlib>

namespace
{
constexpr unsigned int logDomain = 0x5346;
constexpr const char*  logTag    = "SFML-Harmony";

#if defined(SFML_HARMONY_2IN1)
constexpr std::string_view deviceFormName = "2in1";
#else
constexpr std::string_view deviceFormName = "mobile";
#endif

#if defined(SFML_OPENGL_ES)
constexpr std::string_view graphicsApiName = "OpenGL ES";
#else
constexpr std::string_view graphicsApiName = "OpenGL";
#endif

std::atomic<std::uint32_t> surfaceFallbackAcks{};
std::atomic<std::uint32_t> surfaceRestoreAcks{};
std::atomic<std::uint32_t> surfaceStressFailures{};

constexpr std::array sensorTypes{sf::Sensor::Type::Accelerometer,
                                 sf::Sensor::Type::Gyroscope,
                                 sf::Sensor::Type::Magnetometer,
                                 sf::Sensor::Type::Gravity,
                                 sf::Sensor::Type::UserAcceleration,
                                 sf::Sensor::Type::Orientation};
constexpr std::array sensorNames{"accelerometer", "gyroscope", "magnetometer", "gravity", "user-accel", "orientation"};
constexpr std::array joystickAxes{sf::Joystick::Axis::X,
                                  sf::Joystick::Axis::Y,
                                  sf::Joystick::Axis::Z,
                                  sf::Joystick::Axis::R,
                                  sf::Joystick::Axis::U,
                                  sf::Joystick::Axis::V,
                                  sf::Joystick::Axis::PovX,
                                  sf::Joystick::Axis::PovY};
constexpr std::array joystickAxisNames{"X", "Y", "Z", "R", "U", "V", "PovX", "PovY"};

struct SensorDiagnostics
{
    bool          available{};
    std::uint64_t eventCount{};
    sf::Vector3f  eventValue{};
    sf::Vector3f  polledValue{};
};

struct InputDiagnostics
{
    std::uint64_t                    touchBegan{};
    std::uint64_t                    touchMoved{};
    std::uint64_t                    touchEnded{};
    std::size_t                      maximumTouches{};
    std::size_t                      polledTouches{};
    std::unordered_set<unsigned int> activeTouches;

    std::uint64_t                                     mouseMoved{};
    std::array<std::uint64_t, sf::Mouse::ButtonCount> mousePressed{};
    std::array<std::uint64_t, sf::Mouse::ButtonCount> mouseReleased{};
    std::array<bool, sf::Mouse::ButtonCount>          mousePolled{};
    sf::Vector2i                                      mousePosition{};
    std::uint64_t                                     verticalWheel{};
    std::uint64_t                                     horizontalWheel{};
    float                                             verticalWheelDelta{};
    float                                             horizontalWheelDelta{};

    std::uint64_t                    keyPressed{};
    std::uint64_t                    keyReleased{};
    std::uint64_t                    keyRepeated{};
    std::uint64_t                    textEntered{};
    std::uint64_t                    imeCodepoints{};
    std::uint64_t                    backspaces{};
    std::uint64_t                    imeRequests{};
    std::uint64_t                    clipboardRoundTrips{};
    std::uint64_t                    cursorChanges{};
    std::uint64_t                    visibilityChanges{};
    int                              lastKey{-1};
    int                              lastScancode{-1};
    bool                             alt{};
    bool                             control{};
    bool                             shift{};
    bool                             system{};
    std::set<sf::Keyboard::Scancode> keysDown;
    sf::String                       committedText;
};

struct JoystickDiagnostics
{
    bool                                       connected{};
    std::uint64_t                              connectedEvents{};
    std::uint64_t                              disconnectedEvents{};
    std::uint64_t                              movedEvents{};
    std::uint64_t                              buttonPressedEvents{};
    std::uint64_t                              buttonReleasedEvents{};
    unsigned int                               buttonCount{};
    sf::Joystick::Identification               identification;
    std::array<bool, sf::Joystick::AxisCount>  axisAvailable{};
    std::array<float, sf::Joystick::AxisCount> axisValues{};
    std::uint32_t                              pressedButtonMask{};
};

struct SurfaceDiagnostics
{
    std::uint64_t resizeEvents{};
    std::uint64_t zeroSizeEvents{};
    std::uint64_t surfaceLost{};
    std::uint64_t surfaceRecreated{};
    std::uint64_t persistenceChecks{};
    std::uint64_t persistenceFailures{};
    bool          hadSurface{};
    bool          zeroSizeActive{};
    bool          awaitingFallbackCheck{};
    bool          awaitingPersistenceCheck{};
    bool          restoreCheckReady{};
    bool          restoreCheckPassed{};
};

struct RenderTextureDiagnostics
{
    bool colorOnly{};
    bool depthOnly{};
    bool stencilOnly{};
    bool depthStencil{};
    bool aaPath{};
    bool copyPixel{};
};

struct AudioDiagnostics
{
    std::vector<std::string>     playbackDevices;
    std::optional<std::string>   defaultPlaybackDevice;
    std::optional<std::string>   currentPlaybackDevice;
    std::optional<std::uint32_t> playbackSampleRate;
    std::vector<std::string>     captureDevices;
    std::string                  defaultCaptureDevice;
    std::string                  currentCaptureDevice;
    bool                         playbackSelection{};
    bool                         captureSelection{};
    std::uint64_t                notifications{};
    std::string                  lastNotification{"none"};
    std::uint64_t                recordedSamples{};
    std::int32_t                 recordedPeak{};
    unsigned int                 recordingRate{44'100};
    unsigned int                 recordingChannels{1};
    bool                         nullRoute{};
};

struct Diagnostics
{
    bool                                             rawfile{};
    bool                                             rawfileFont{};
    bool                                             systemFont{};
    bool                                             sandbox{};
    bool                                             tcp{};
    bool                                             udp{};
    bool                                             dns{};
    bool                                             renderTexture{};
    bool                                             shader{};
    bool                                             srgb{};
    bool                                             vertexBuffer{};
    bool                                             audio{};
    bool                                             vulkan{};
    bool                                             glVersion{};
    bool                                             glStateRestore{true};
    bool                                             sensorEvent{};
    bool                                             gamepad{};
    unsigned int                                     surfaceResizeEvents{};
    std::uint64_t                                    glStateChecks{};
    GLenum                                           glError{GL_NO_ERROR};
    std::string                                      glCapability{"QueryGL n/a"};
    std::string                                      srgbMipmap{"not tested"};
    std::array<SensorDiagnostics, sf::Sensor::Count> sensors;
    InputDiagnostics                                 input;
    JoystickDiagnostics                              joystick;
    SurfaceDiagnostics                               surface;
    RenderTextureDiagnostics                         renderTextures;
    AudioDiagnostics                                 audioState;
};

void logInfo(const std::string& message)
{
    (void)OH_LOG_Print(LOG_APP, LOG_INFO, logDomain, logTag, "%{public}s", message.c_str());
}

void logError(const std::string& message)
{
    (void)OH_LOG_Print(LOG_APP, LOG_ERROR, logDomain, logTag, "%{public}s", message.c_str());
}

class ExternalTlsDiagnostic
{
public:
    ~ExternalTlsDiagnostic()
    {
        stop();
    }

    ExternalTlsDiagnostic()                                        = default;
    ExternalTlsDiagnostic(const ExternalTlsDiagnostic&)            = delete;
    ExternalTlsDiagnostic& operator=(const ExternalTlsDiagnostic&) = delete;

    void start()
    {
        {
            const std::lock_guard lock(m_mutex);
            if (m_running)
                return;
        }

        if (m_worker.joinable())
            m_worker.join();

        m_cancel.store(false, std::memory_order_relaxed);
        {
            const std::lock_guard lock(m_mutex);
            for (auto& result : m_results)
                result = {};
            m_running = true;
        }
        logInfo("optional external TLS certificate suite started");
        m_worker = std::thread([this] { run(); });
    }

    void stop()
    {
        m_cancel.store(true, std::memory_order_relaxed);
        if (m_worker.joinable())
            m_worker.join();
    }

    [[nodiscard]] std::string snapshot() const
    {
        const std::lock_guard lock(m_mutex);
        std::ostringstream    stream;
        for (std::size_t index = 0; index < targets.size(); ++index)
        {
            if (index)
                stream << '\n';
            stream << (targets[index].expectHandshake ? "+ " : "- ") << targets[index].label << ": "
                   << stateName(m_results[index].state);
            if (!m_results[index].detail.empty())
                stream << " - " << m_results[index].detail;
        }
        return stream.str();
    }

private:
    struct Target
    {
        std::string_view hostname;
        std::string_view label;
        bool             expectHandshake;
    };

    static constexpr std::array targets{Target{"sha256.badssl.com", "valid-chain", true},
                                        Target{"wrong.host.badssl.com", "wrong-host", false},
                                        Target{"self-signed.badssl.com", "self-signed", false},
                                        Target{"expired.badssl.com", "expired", false}};

    enum class State
    {
        Idle,
        Pending,
        Resolving,
        Connecting,
        Handshaking,
        Passed,
        Failed,
        TransportFailed,
        Cancelled
    };

    struct Result
    {
        State       state{State::Idle};
        std::string detail;
    };

    [[nodiscard]] static std::string stateName(State state)
    {
        switch (state)
        {
            case State::Idle:
                return "idle";
            case State::Pending:
                return "pending";
            case State::Resolving:
                return "resolving";
            case State::Connecting:
                return "connecting";
            case State::Handshaking:
                return "handshaking";
            case State::Passed:
                return "PASS";
            case State::Failed:
                return "FAIL";
            case State::TransportFailed:
                return "network/timeout";
            case State::Cancelled:
                return "cancelled";
        }
        return "unknown";
    }

    void setState(std::size_t index, State state, std::string detail = {})
    {
        std::string line;
        {
            const std::lock_guard lock(m_mutex);
            m_results[index].state  = state;
            m_results[index].detail = std::move(detail);
            line                    = "optional TLS " + std::string(targets[index].hostname) + ' ' + stateName(state) +
                   (m_results[index].detail.empty() ? "" : ": " + m_results[index].detail);
        }
        logInfo(line);
    }

    [[nodiscard]] bool cancelled() const
    {
        return m_cancel.load(std::memory_order_relaxed);
    }

    [[nodiscard]] bool runTarget(std::size_t index)
    {
        const Target& target = targets[index];
        if (cancelled())
        {
            setState(index, State::Cancelled);
            return false;
        }
        setState(index, State::Resolving, std::string(target.hostname));
        const auto addresses = sf::Dns::resolve(std::string(target.hostname));
        if (cancelled())
        {
            setState(index, State::Cancelled);
            return false;
        }
        if (!addresses || addresses->empty())
        {
            setState(index, State::TransportFailed, "DNS failed (inconclusive)");
            return true;
        }

        std::vector<sf::IpAddress> candidates;
        if (const auto ipv4 = std::find_if(addresses->begin(),
                                           addresses->end(),
                                           [](const sf::IpAddress& address) { return address.isV4(); });
            ipv4 != addresses->end())
        {
            candidates.push_back(*ipv4);
        }
        if (const auto ipv6 = std::find_if(addresses->begin(),
                                           addresses->end(),
                                           [](const sf::IpAddress& address) { return address.isV6(); });
            ipv6 != addresses->end())
        {
            candidates.push_back(*ipv6);
        }

        sf::TcpSocket socket;
        bool          connected{};
        for (const auto& address : candidates)
        {
            if (cancelled())
            {
                setState(index, State::Cancelled);
                return false;
            }
            setState(index, State::Connecting, address.toString() + ":443");
            if (socket.connect(address, 443, sf::seconds(2)) == sf::Socket::Status::Done)
            {
                connected = true;
                break;
            }
        }
        if (!connected)
        {
            setState(index, State::TransportFailed, "TCP failed (inconclusive)");
            return true;
        }

        socket.setBlocking(false);
        setState(index, State::Handshaking, std::string(target.hostname) + ", verify=true");
        sf::Clock handshakeClock;
        while (!cancelled() && handshakeClock.getElapsedTime() < sf::seconds(5))
        {
            const auto status = socket.setupTlsClient(sf::String(target.hostname), true);
            if (status == sf::TcpSocket::TlsStatus::HandshakeComplete)
            {
                const auto cipher = socket.getCurrentCiphersuiteName();
                if (target.expectHandshake)
                {
                    setState(index, State::Passed, "verified peer");
                    if (cipher)
                        logInfo("optional TLS valid-chain ciphersuite: " + *cipher);
                }
                else
                    setState(index, State::Failed, "UNSAFE: invalid certificate accepted");
                socket.disconnect();
                return true;
            }
            if (status == sf::TcpSocket::TlsStatus::Error)
            {
                if (target.expectHandshake)
                    setState(index, State::Failed, "valid peer rejected");
                else
                    setState(index, State::Passed, "certificate rejected");
                socket.disconnect();
                return true;
            }
            if (status == sf::TcpSocket::TlsStatus::NotConnected)
            {
                setState(index, State::TransportFailed, "connection lost (inconclusive)");
                socket.disconnect();
                return true;
            }
            sf::sleep(sf::milliseconds(10));
        }

        if (cancelled())
            setState(index, State::Cancelled);
        else
            setState(index, State::TransportFailed, "TLS timeout (inconclusive)");
        socket.disconnect();
        return !cancelled();
    }

    void run()
    {
        {
            const std::lock_guard lock(m_mutex);
            for (auto& result : m_results)
                result = {State::Pending, "waiting"};
        }

        std::size_t index{};
        for (; index < targets.size(); ++index)
        {
            if (!runTarget(index))
                break;
        }

        {
            const std::lock_guard lock(m_mutex);
            for (std::size_t remaining = index + 1; remaining < m_results.size(); ++remaining)
            {
                if (m_results[remaining].state == State::Pending)
                    m_results[remaining] = {State::Cancelled, {}};
            }
            m_running = false;
        }
        logInfo(cancelled() ? "optional external TLS certificate suite cancelled"
                            : "optional external TLS certificate suite finished");
    }

    mutable std::mutex                 m_mutex;
    std::array<Result, targets.size()> m_results;
    bool                               m_running{};
    std::atomic<bool>                  m_cancel{};
    std::thread                        m_worker;
};

std::size_t sensorIndex(sf::Sensor::Type type)
{
    return static_cast<std::size_t>(type);
}

std::string utf8(const sf::String& string)
{
    const auto encoded = string.toUtf8();
    return {encoded.begin(), encoded.end()};
}

std::string glString(GLenum name)
{
    using GetStringFunction = const GLubyte* (*)(GLenum);

    const auto  getString = reinterpret_cast<GetStringFunction>(sf::Context::getFunction("glGetString"));
    const auto* value     = getString ? getString(name) : nullptr;
    return value ? reinterpret_cast<const char*>(value) : "<unavailable>";
}

std::string queryGlCapability()
{
#if defined(SFML_HARMONY_2IN1)
    using QueryGlFunction = EGLBoolean (*)();
    const auto queryGl    = reinterpret_cast<QueryGlFunction>(eglGetProcAddress("OH_Graphics_QueryGL"));
    if (!queryGl)
        return "QueryGL missing";
    return queryGl() == EGL_TRUE ? "QueryGL desktop=yes" : "QueryGL desktop=no";
#else
    return "QueryGL n/a";
#endif
}

#if defined(SFML_OPENGL_ES)
bool hasGlExtension(std::string_view extension)
{
    const std::string extensions = " " + glString(GL_EXTENSIONS) + " ";
    const std::string token      = " " + std::string(extension) + " ";
    return extensions.find(token) != std::string::npos;
}
#endif

GLenum getGlError()
{
    using GetErrorFunction = GLenum (*)();

    static const auto getError = reinterpret_cast<GetErrorFunction>(sf::Context::getFunction("glGetError"));
    return getError ? getError() : GL_INVALID_OPERATION;
}

template <typename Function>
Function loadGlFunction(const char* name)
{
    return reinterpret_cast<Function>(sf::Context::getFunction(name));
}

void logGlIdentity()
{
    logInfo(std::string("SFML_DEVICE_FORM=") + std::string(deviceFormName));
    logInfo(std::string("SFML_GRAPHICS_API=") + std::string(graphicsApiName));
    logInfo(queryGlCapability());
    logInfo("GL_VENDOR=" + glString(GL_VENDOR));
    logInfo("GL_RENDERER=" + glString(GL_RENDERER));
    logInfo("GL_VERSION=" + glString(GL_VERSION));
    logInfo("GL_SHADING_LANGUAGE_VERSION=" + glString(GL_SHADING_LANGUAGE_VERSION));
}

struct RawGlState
{
    GLint                    program{};
    GLint                    activeTexture{};
    GLint                    texture1Binding{};
    GLint                    arrayBuffer{};
    GLint                    attribute0Enabled{};
    std::array<GLint, 4>     viewport{};
    std::array<GLint, 4>     scissorBox{};
    std::array<GLboolean, 4> colorMask{};
    std::array<GLfloat, 4>   clearColor{};
    GLint                    clearStencil{};
    GLboolean                blendEnabled{};
    GLboolean                stencilEnabled{};
    GLboolean                scissorEnabled{};
    GLboolean                cullEnabled{};
    GLboolean                depthEnabled{};
};

struct RawStateGl
{
    PFNGLACTIVETEXTUREPROC            activeTexture{};
    PFNGLBINDBUFFERPROC               bindBuffer{};
    PFNGLBINDTEXTUREPROC              bindTexture{};
    PFNGLCLEARCOLORPROC               clearColor{};
    PFNGLCLEARSTENCILPROC             clearStencil{};
    PFNGLCOLORMASKPROC                colorMask{};
    PFNGLDISABLEPROC                  disable{};
    PFNGLDISABLEVERTEXATTRIBARRAYPROC disableVertexAttribArray{};
    PFNGLENABLEPROC                   enable{};
    PFNGLGETBOOLEANVPROC              getBoolean{};
    PFNGLGETFLOATVPROC                getFloat{};
    PFNGLGETINTEGERVPROC              getInteger{};
    PFNGLGETVERTEXATTRIBIVPROC        getVertexAttribInteger{};
    PFNGLISENABLEDPROC                isEnabled{};
    PFNGLSCISSORPROC                  scissor{};
    PFNGLUSEPROGRAMPROC               useProgram{};
    PFNGLVIEWPORTPROC                 viewport{};

    [[nodiscard]] explicit operator bool() const
    {
        return activeTexture && bindBuffer && bindTexture && clearColor && clearStencil && colorMask && disable &&
               disableVertexAttribArray && enable && getBoolean && getFloat && getInteger && getVertexAttribInteger &&
               isEnabled && scissor && useProgram && viewport;
    }
};

const RawStateGl& getRawStateGl()
{
    static const RawStateGl functions = []
    {
        RawStateGl result;
        result.activeTexture            = loadGlFunction<PFNGLACTIVETEXTUREPROC>("glActiveTexture");
        result.bindBuffer               = loadGlFunction<PFNGLBINDBUFFERPROC>("glBindBuffer");
        result.bindTexture              = loadGlFunction<PFNGLBINDTEXTUREPROC>("glBindTexture");
        result.clearColor               = loadGlFunction<PFNGLCLEARCOLORPROC>("glClearColor");
        result.clearStencil             = loadGlFunction<PFNGLCLEARSTENCILPROC>("glClearStencil");
        result.colorMask                = loadGlFunction<PFNGLCOLORMASKPROC>("glColorMask");
        result.disable                  = loadGlFunction<PFNGLDISABLEPROC>("glDisable");
        result.disableVertexAttribArray = loadGlFunction<PFNGLDISABLEVERTEXATTRIBARRAYPROC>(
            "glDisableVertexAttribArray");
        result.enable                 = loadGlFunction<PFNGLENABLEPROC>("glEnable");
        result.getBoolean             = loadGlFunction<PFNGLGETBOOLEANVPROC>("glGetBooleanv");
        result.getFloat               = loadGlFunction<PFNGLGETFLOATVPROC>("glGetFloatv");
        result.getInteger             = loadGlFunction<PFNGLGETINTEGERVPROC>("glGetIntegerv");
        result.getVertexAttribInteger = loadGlFunction<PFNGLGETVERTEXATTRIBIVPROC>("glGetVertexAttribiv");
        result.isEnabled              = loadGlFunction<PFNGLISENABLEDPROC>("glIsEnabled");
        result.scissor                = loadGlFunction<PFNGLSCISSORPROC>("glScissor");
        result.useProgram             = loadGlFunction<PFNGLUSEPROGRAMPROC>("glUseProgram");
        result.viewport               = loadGlFunction<PFNGLVIEWPORTPROC>("glViewport");
        return result;
    }();
    return functions;
}

RawGlState captureRawGlState(const RawStateGl& gl)
{
    RawGlState state;
    gl.getInteger(GL_CURRENT_PROGRAM, &state.program);
    gl.getInteger(GL_ACTIVE_TEXTURE, &state.activeTexture);
    gl.activeTexture(GL_TEXTURE1);
    gl.getInteger(GL_TEXTURE_BINDING_2D, &state.texture1Binding);
    gl.activeTexture(static_cast<GLenum>(state.activeTexture));
    gl.getInteger(GL_ARRAY_BUFFER_BINDING, &state.arrayBuffer);
    gl.getVertexAttribInteger(0, GL_VERTEX_ATTRIB_ARRAY_ENABLED, &state.attribute0Enabled);
    gl.getInteger(GL_VIEWPORT, state.viewport.data());
    gl.getInteger(GL_SCISSOR_BOX, state.scissorBox.data());
    gl.getBoolean(GL_COLOR_WRITEMASK, state.colorMask.data());
    gl.getFloat(GL_COLOR_CLEAR_VALUE, state.clearColor.data());
    gl.getInteger(GL_STENCIL_CLEAR_VALUE, &state.clearStencil);
    state.blendEnabled   = gl.isEnabled(GL_BLEND);
    state.stencilEnabled = gl.isEnabled(GL_STENCIL_TEST);
    state.scissorEnabled = gl.isEnabled(GL_SCISSOR_TEST);
    state.cullEnabled    = gl.isEnabled(GL_CULL_FACE);
    state.depthEnabled   = gl.isEnabled(GL_DEPTH_TEST);
    return state;
}

bool equalRawGlStates(const RawGlState& left, const RawGlState& right)
{
    return left.program == right.program && left.activeTexture == right.activeTexture &&
           left.texture1Binding == right.texture1Binding && left.arrayBuffer == right.arrayBuffer &&
           left.attribute0Enabled == right.attribute0Enabled && left.viewport == right.viewport &&
           left.scissorBox == right.scissorBox && left.colorMask == right.colorMask &&
           left.clearColor == right.clearColor && left.clearStencil == right.clearStencil &&
           left.blendEnabled == right.blendEnabled && left.stencilEnabled == right.stencilEnabled &&
           left.scissorEnabled == right.scissorEnabled && left.cullEnabled == right.cullEnabled &&
           left.depthEnabled == right.depthEnabled;
}

bool checkGlStateRestoration(sf::RenderWindow& window)
{
    const auto& gl = getRawStateGl();
    if (!gl)
        return false;

    const RawGlState before = captureRawGlState(gl);
    window.pushGLStates();

    gl.useProgram(0);
    gl.activeTexture(GL_TEXTURE1);
    gl.bindTexture(GL_TEXTURE_2D, 0);
    gl.bindBuffer(GL_ARRAY_BUFFER, 0);
    gl.disableVertexAttribArray(0);
    gl.viewport(1, 2, 3, 4);
    gl.scissor(5, 6, 7, 8);
    gl.disable(GL_BLEND);
    gl.enable(GL_STENCIL_TEST);
    gl.enable(GL_SCISSOR_TEST);
    gl.enable(GL_CULL_FACE);
    gl.enable(GL_DEPTH_TEST);
    gl.colorMask(GL_FALSE, GL_TRUE, GL_FALSE, GL_TRUE);
    gl.clearColor(0.125f, 0.25f, 0.5f, 0.75f);
    gl.clearStencil(19);

    window.popGLStates();
    return equalRawGlStates(before, captureRawGlState(gl));
}

class MeterRecorder : public sf::SoundBufferRecorder
{
public:
    void resetMeter()
    {
        m_sampleCount.store(0, std::memory_order_relaxed);
        m_peak.store(0, std::memory_order_relaxed);
    }

    [[nodiscard]] std::uint64_t getMeteredSampleCount() const
    {
        return m_sampleCount.load(std::memory_order_relaxed);
    }

    [[nodiscard]] std::int32_t getPeak() const
    {
        return m_peak.load(std::memory_order_relaxed);
    }

protected:
    bool onProcessSamples(const std::int16_t* samples, std::size_t sampleCount) override
    {
        std::int32_t peak = m_peak.load(std::memory_order_relaxed);
        for (std::size_t index = 0; index < sampleCount; ++index)
            peak = std::max(peak, std::abs(static_cast<std::int32_t>(samples[index])));
        m_peak.store(peak, std::memory_order_relaxed);
        m_sampleCount.fetch_add(sampleCount, std::memory_order_relaxed);
        return sf::SoundBufferRecorder::onProcessSamples(samples, sampleCount);
    }

private:
    std::atomic<std::uint64_t> m_sampleCount{};
    std::atomic<std::int32_t>  m_peak{};
};

void setAllSensorsEnabled(Diagnostics& diagnostics, bool enabled)
{
    for (std::size_t index = 0; index < sensorTypes.size(); ++index)
    {
        auto& sensor     = diagnostics.sensors[index];
        sensor.available = sf::Sensor::isAvailable(sensorTypes[index]);
        if (sensor.available)
            sf::Sensor::setEnabled(sensorTypes[index], enabled);
    }
    logInfo(std::string("all available sensors ") + (enabled ? "enabled" : "disabled"));
}

void pollSensors(Diagnostics& diagnostics)
{
    for (std::size_t index = 0; index < sensorTypes.size(); ++index)
    {
        if (diagnostics.sensors[index].available)
            diagnostics.sensors[index].polledValue = sf::Sensor::getValue(sensorTypes[index]);
    }
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
void                     pollTouches(InputDiagnostics& diagnostics, const sf::WindowBase& window)
{
    diagnostics.polledTouches = 0;
    for (const unsigned int finger : diagnostics.activeTouches)
    {
        if (sf::Touch::isDown(finger))
        {
            ++diagnostics.polledTouches;
            static_cast<void>(sf::Touch::getPosition(finger, window));
        }
    }
}
#pragma clang diagnostic pop

void pollMouse(InputDiagnostics& diagnostics, const sf::WindowBase& window)
{
    diagnostics.mousePosition = sf::Mouse::getPosition(window);
    for (std::size_t index = 0; index < diagnostics.mousePolled.size(); ++index)
        diagnostics.mousePolled[index] = sf::Mouse::isButtonPressed(static_cast<sf::Mouse::Button>(index));
}

void pollJoystick(JoystickDiagnostics& diagnostics)
{
    sf::Joystick::update();
    diagnostics.connected = sf::Joystick::isConnected(0);
    if (!diagnostics.connected)
    {
        diagnostics.identification    = {};
        diagnostics.buttonCount       = 0;
        diagnostics.pressedButtonMask = 0;
        diagnostics.axisAvailable.fill(false);
        diagnostics.axisValues.fill(0.f);
        return;
    }

    diagnostics.identification = sf::Joystick::getIdentification(0);
    diagnostics.buttonCount    = sf::Joystick::getButtonCount(0);
    for (std::size_t index = 0; index < joystickAxes.size(); ++index)
    {
        diagnostics.axisAvailable[index] = sf::Joystick::hasAxis(0, joystickAxes[index]);
        diagnostics.axisValues[index]    = sf::Joystick::getAxisPosition(0, joystickAxes[index]);
    }

    diagnostics.pressedButtonMask  = 0;
    const unsigned int buttonLimit = std::min(diagnostics.buttonCount, 32u);
    for (unsigned int button = 0; button < buttonLimit; ++button)
    {
        if (sf::Joystick::isButtonPressed(0, button))
            diagnostics.pressedButtonMask |= (1u << button);
    }
}

struct DepthDiagnosticGl
{
    PFNGLATTACHSHADERPROC            attachShader{};
    PFNGLBINDATTRIBLOCATIONPROC      bindAttribLocation{};
    PFNGLBINDBUFFERPROC              bindBuffer{};
    PFNGLCHECKFRAMEBUFFERSTATUSPROC  checkFramebufferStatus{};
    PFNGLCLEARPROC                   clear{};
    PFNGLCLEARCOLORPROC              clearColor{};
    PFNGLCLEARDEPTHFPROC             clearDepth{};
    PFNGLCOLORMASKPROC               colorMask{};
    PFNGLCOMPILESHADERPROC           compileShader{};
    PFNGLCREATEPROGRAMPROC           createProgram{};
    PFNGLCREATESHADERPROC            createShader{};
    PFNGLDELETESHADERPROC            deleteShader{};
    PFNGLDELETEPROGRAMPROC           deleteProgram{};
    PFNGLDEPTHFUNCPROC               depthFunc{};
    PFNGLDEPTHMASKPROC               depthMask{};
    PFNGLDISABLEPROC                 disable{};
    PFNGLDRAWARRAYSPROC              drawArrays{};
    PFNGLENABLEPROC                  enable{};
    PFNGLENABLEVERTEXATTRIBARRAYPROC enableVertexAttribArray{};
    PFNGLGETBOOLEANVPROC             getBoolean{};
    PFNGLGETERRORPROC                getError{};
    PFNGLGETFLOATVPROC               getFloat{};
    PFNGLGETINTEGERVPROC             getInteger{};
    PFNGLGETPROGRAMIVPROC            getProgramInteger{};
    PFNGLGETSHADERIVPROC             getShaderInteger{};
    PFNGLGETUNIFORMLOCATIONPROC      getUniformLocation{};
    PFNGLLINKPROGRAMPROC             linkProgram{};
    PFNGLSHADERSOURCEPROC            shaderSource{};
    PFNGLUNIFORM1FPROC               uniform1f{};
    PFNGLUNIFORM4FPROC               uniform4f{};
    PFNGLUSEPROGRAMPROC              useProgram{};
    PFNGLVERTEXATTRIBPOINTERPROC     vertexAttribPointer{};
    PFNGLVIEWPORTPROC                viewport{};

    [[nodiscard]] explicit operator bool() const
    {
        return attachShader && bindAttribLocation && bindBuffer && checkFramebufferStatus && clear && clearColor &&
               clearDepth && colorMask && compileShader && createProgram && createShader && deleteShader && deleteProgram &&
               depthFunc && depthMask && disable && drawArrays && enable && enableVertexAttribArray && getBoolean &&
               getError && getFloat && getInteger && getProgramInteger && getShaderInteger && getUniformLocation &&
               linkProgram && shaderSource && uniform1f && uniform4f && useProgram && vertexAttribPointer && viewport;
    }
};

const DepthDiagnosticGl& getDepthDiagnosticGl()
{
    static const DepthDiagnosticGl functions = []
    {
        DepthDiagnosticGl result;
        result.attachShader            = loadGlFunction<PFNGLATTACHSHADERPROC>("glAttachShader");
        result.bindAttribLocation      = loadGlFunction<PFNGLBINDATTRIBLOCATIONPROC>("glBindAttribLocation");
        result.bindBuffer              = loadGlFunction<PFNGLBINDBUFFERPROC>("glBindBuffer");
        result.checkFramebufferStatus  = loadGlFunction<PFNGLCHECKFRAMEBUFFERSTATUSPROC>("glCheckFramebufferStatus");
        result.clear                   = loadGlFunction<PFNGLCLEARPROC>("glClear");
        result.clearColor              = loadGlFunction<PFNGLCLEARCOLORPROC>("glClearColor");
        result.clearDepth              = loadGlFunction<PFNGLCLEARDEPTHFPROC>("glClearDepthf");
        result.colorMask               = loadGlFunction<PFNGLCOLORMASKPROC>("glColorMask");
        result.compileShader           = loadGlFunction<PFNGLCOMPILESHADERPROC>("glCompileShader");
        result.createProgram           = loadGlFunction<PFNGLCREATEPROGRAMPROC>("glCreateProgram");
        result.createShader            = loadGlFunction<PFNGLCREATESHADERPROC>("glCreateShader");
        result.deleteShader            = loadGlFunction<PFNGLDELETESHADERPROC>("glDeleteShader");
        result.deleteProgram           = loadGlFunction<PFNGLDELETEPROGRAMPROC>("glDeleteProgram");
        result.depthFunc               = loadGlFunction<PFNGLDEPTHFUNCPROC>("glDepthFunc");
        result.depthMask               = loadGlFunction<PFNGLDEPTHMASKPROC>("glDepthMask");
        result.disable                 = loadGlFunction<PFNGLDISABLEPROC>("glDisable");
        result.drawArrays              = loadGlFunction<PFNGLDRAWARRAYSPROC>("glDrawArrays");
        result.enable                  = loadGlFunction<PFNGLENABLEPROC>("glEnable");
        result.enableVertexAttribArray = loadGlFunction<PFNGLENABLEVERTEXATTRIBARRAYPROC>("glEnableVertexAttribArray");
        result.getBoolean              = loadGlFunction<PFNGLGETBOOLEANVPROC>("glGetBooleanv");
        result.getError                = loadGlFunction<PFNGLGETERRORPROC>("glGetError");
        result.getFloat                = loadGlFunction<PFNGLGETFLOATVPROC>("glGetFloatv");
        result.getInteger              = loadGlFunction<PFNGLGETINTEGERVPROC>("glGetIntegerv");
        result.getProgramInteger       = loadGlFunction<PFNGLGETPROGRAMIVPROC>("glGetProgramiv");
        result.getShaderInteger        = loadGlFunction<PFNGLGETSHADERIVPROC>("glGetShaderiv");
        result.getUniformLocation      = loadGlFunction<PFNGLGETUNIFORMLOCATIONPROC>("glGetUniformLocation");
        result.linkProgram             = loadGlFunction<PFNGLLINKPROGRAMPROC>("glLinkProgram");
        result.shaderSource            = loadGlFunction<PFNGLSHADERSOURCEPROC>("glShaderSource");
        result.uniform1f               = loadGlFunction<PFNGLUNIFORM1FPROC>("glUniform1f");
        result.uniform4f               = loadGlFunction<PFNGLUNIFORM4FPROC>("glUniform4f");
        result.useProgram              = loadGlFunction<PFNGLUSEPROGRAMPROC>("glUseProgram");
        result.vertexAttribPointer     = loadGlFunction<PFNGLVERTEXATTRIBPOINTERPROC>("glVertexAttribPointer");
        result.viewport                = loadGlFunction<PFNGLVIEWPORTPROC>("glViewport");
        return result;
    }();
    return functions;
}

GLuint compileDepthDiagnosticShader(const DepthDiagnosticGl& gl, GLenum type, const char* source)
{
    const GLuint shader = gl.createShader(type);
    if (!shader)
        return 0;

    gl.shaderSource(shader, 1, &source, nullptr);
    gl.compileShader(shader);
    GLint compiled{};
    gl.getShaderInteger(shader, GL_COMPILE_STATUS, &compiled);
    if (!compiled)
    {
        logError("RenderTexture depth diagnostic shader compilation failed");
        gl.deleteShader(shader);
        return 0;
    }
    return shader;
}

GLuint makeDepthDiagnosticProgram(const DepthDiagnosticGl& gl)
{
    constexpr const char* vertexSource   = R"(
attribute highp vec2 position;
uniform highp float depth;
void main()
{
    gl_Position = vec4(position, depth, 1.0);
}
)";
    constexpr const char* fragmentSource = R"(
precision mediump float;
uniform lowp vec4 fillColor;
void main()
{
    gl_FragColor = fillColor;
}
)";

    const GLuint vertexShader = compileDepthDiagnosticShader(gl, GL_VERTEX_SHADER, vertexSource);
    if (!vertexShader)
        return 0;

    const GLuint fragmentShader = compileDepthDiagnosticShader(gl, GL_FRAGMENT_SHADER, fragmentSource);
    if (!fragmentShader)
    {
        gl.deleteShader(vertexShader);
        return 0;
    }

    const GLuint program = gl.createProgram();
    if (program)
    {
        gl.attachShader(program, vertexShader);
        gl.attachShader(program, fragmentShader);
        gl.bindAttribLocation(program, 0, "position");
        gl.linkProgram(program);
    }
    gl.deleteShader(vertexShader);
    gl.deleteShader(fragmentShader);

    GLint linked{};
    if (program)
        gl.getProgramInteger(program, GL_LINK_STATUS, &linked);
    if (!linked)
    {
        logError("RenderTexture depth diagnostic shader link failed");
        if (program)
            gl.deleteProgram(program);
        return 0;
    }
    return program;
}

bool colorsApproximatelyEqual(sf::Color left, sf::Color right)
{
    const auto close = [](std::uint8_t first, std::uint8_t second)
    { return std::abs(static_cast<int>(first) - static_cast<int>(second)) <= 1; };
    return close(left.r, right.r) && close(left.g, right.g) && close(left.b, right.b) && close(left.a, right.a);
}

bool hasUsableCurrentGlContext()
{
    using GetIntegerFunction = void (*)(GLenum, GLint*);

    const auto getInteger = loadGlFunction<GetIntegerFunction>("glGetIntegerv");
    GLint      maximumTextureSize{-1};
    if (getInteger)
        getInteger(GL_MAX_TEXTURE_SIZE, &maximumTextureSize);

    return glString(GL_VERSION) != "<unavailable>" && maximumTextureSize > 0;
}

bool hasCurrentWindowEglSurface(sf::Vector2u expectedSize)
{
    if (!expectedSize.x || !expectedSize.y)
        return false;

    // Discard any already-reported EGL error so this check describes only
    // the post-present surface queries below.
    static_cast<void>(eglGetError());
    const EGLDisplay display = eglGetCurrentDisplay();
    const EGLSurface surface = eglGetCurrentSurface(EGL_DRAW);
    if (display == EGL_NO_DISPLAY || surface == EGL_NO_SURFACE)
        return false;

    EGLint     width{};
    EGLint     height{};
    const bool queried = eglQuerySurface(display, surface, EGL_WIDTH, &width) == EGL_TRUE &&
                         eglQuerySurface(display, surface, EGL_HEIGHT, &height) == EGL_TRUE;
    const bool noError = eglGetError() == EGL_SUCCESS;
    return queried && noError && width > 1 && height > 1 && static_cast<unsigned int>(width) == expectedSize.x &&
           static_cast<unsigned int>(height) == expectedSize.y;
}

bool checkSharedResourcesOnCurrentContext(const sf::Image&                  checkerboard,
                                          const sf::Texture&                checkerTexture,
                                          sf::Shader&                       shader,
                                          bool                              shaderAvailable,
                                          std::optional<sf::RenderTexture>& renderTexture)
{
    if (!hasUsableCurrentGlContext() || !checkerTexture.getNativeHandle() || !renderTexture)
        return false;

    constexpr sf::Vector2u samplePixel{1, 1};
    const sf::Image        textureCopy   = checkerTexture.copyToImage();
    const bool             texturePassed = textureCopy.getSize() == checkerboard.getSize() &&
                               checkerboard.getSize().x > samplePixel.x && checkerboard.getSize().y > samplePixel.y &&
                               textureCopy.getPixel(samplePixel) == checkerboard.getPixel(samplePixel);

    const bool shaderPassed = !shaderAvailable || shader.getNativeHandle() != 0;
    if (!texturePassed || !shaderPassed)
        return false;

    if (shaderAvailable)
        shader.setUniform("pulse", 1.f);
    renderTexture->clear(sf::Color(91, 33, 182));
    sf::RectangleShape textureQuad(sf::Vector2f(checkerboard.getSize()));
    textureQuad.setTexture(&checkerTexture);
    sf::RenderStates states;
    states.shader = shaderAvailable ? &shader : nullptr;
    renderTexture->draw(textureQuad, states);
    renderTexture->display();

    const sf::Image renderCopy = renderTexture->getTexture().copyToImage();
    return renderTexture->getTexture().getNativeHandle() != 0 && renderCopy.getSize() == sf::Vector2u(320, 320) &&
           colorsApproximatelyEqual(renderCopy.getPixel(samplePixel), checkerboard.getPixel(samplePixel));
}

bool checkRenderTextureDepthOperation(sf::RenderTexture& target, sf::Color clearColor)
{
    if (!target.setActive(true))
        return false;

    const auto& gl = getDepthDiagnosticGl();
    if (!gl)
    {
        logError("RenderTexture depth diagnostic GL dispatch is unavailable");
        return false;
    }

    const GLenum inheritedError = gl.getError();
    GLint        depthBits{};
    GLint        framebuffer{};
    gl.getInteger(GL_DEPTH_BITS, &depthBits);
    gl.getInteger(GL_FRAMEBUFFER_BINDING, &framebuffer);
    const GLenum framebufferStatus = gl.checkFramebufferStatus(GL_FRAMEBUFFER);

    GLint     previousDepthFunction{};
    GLboolean previousDepthMask{};
    GLfloat   previousClearDepth{};
    gl.getInteger(GL_DEPTH_FUNC, &previousDepthFunction);
    gl.getBoolean(GL_DEPTH_WRITEMASK, &previousDepthMask);
    gl.getFloat(GL_DEPTH_CLEAR_VALUE, &previousClearDepth);

    target.pushGLStates();
    const GLuint program = makeDepthDiagnosticProgram(gl);
    bool         rendered{};
    GLenum       renderError{GL_NO_ERROR};
    if (program)
    {
        constexpr std::array<GLfloat, 8> positions{-1.f, -1.f, 1.f, -1.f, -1.f, 1.f, 1.f, 1.f};
        const GLint                      depthLocation = gl.getUniformLocation(program, "depth");
        const GLint                      colorLocation = gl.getUniformLocation(program, "fillColor");
        if (depthLocation >= 0 && colorLocation >= 0)
        {
            const auto setColor = [&gl, colorLocation](sf::Color color)
            { gl.uniform4f(colorLocation, color.r / 255.f, color.g / 255.f, color.b / 255.f, color.a / 255.f); };

            gl.viewport(0, 0, 48, 48);
            gl.colorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
            gl.clearColor(clearColor.r / 255.f, clearColor.g / 255.f, clearColor.b / 255.f, clearColor.a / 255.f);
            gl.clearDepth(1.f);
            gl.depthMask(GL_TRUE);
            gl.clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            gl.disable(GL_BLEND);
            gl.enable(GL_DEPTH_TEST);
            gl.depthFunc(GL_LESS);
            gl.useProgram(program);
            gl.bindBuffer(GL_ARRAY_BUFFER, 0);
            gl.enableVertexAttribArray(0);
            gl.vertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, positions.data());

            constexpr sf::Color nearColor(19, 201, 101);
            constexpr sf::Color farColor(239, 68, 68);
            gl.uniform1f(depthLocation, -0.6f);
            setColor(nearColor);
            gl.drawArrays(GL_TRIANGLE_STRIP, 0, 4);
            gl.uniform1f(depthLocation, 0.6f);
            setColor(farColor);
            gl.drawArrays(GL_TRIANGLE_STRIP, 0, 4);
            renderError = gl.getError();
            rendered    = renderError == GL_NO_ERROR;
        }
        gl.useProgram(0);
        gl.deleteProgram(program);
    }

    gl.depthFunc(static_cast<GLenum>(previousDepthFunction));
    gl.depthMask(previousDepthMask);
    gl.clearDepth(previousClearDepth);
    target.popGLStates();
    target.display();

    if (!rendered)
    {
        std::ostringstream message;
        message << "RenderTexture depth draw failed: inheritedError=0x" << std::hex << inheritedError
                << " renderError=0x" << renderError << " framebuffer=0x" << framebuffer << " status=0x"
                << framebufferStatus << std::dec << " depthBits=" << depthBits;
        logError(message.str());
        return false;
    }

    constexpr sf::Color nearColor(19, 201, 101);
    const sf::Image     copy   = target.getTexture().copyToImage();
    const sf::Color     center = copy.getSize() == sf::Vector2u(48, 48) ? copy.getPixel({24, 24}) : sf::Color{};
    const bool          passed = copy.getSize() == sf::Vector2u(48, 48) && colorsApproximatelyEqual(center, nearColor);
    if (!passed)
    {
        std::ostringstream message;
        message << "RenderTexture depth pixels failed: size=" << copy.getSize().x << 'x' << copy.getSize().y
                << " center=" << static_cast<unsigned int>(center.r) << ',' << static_cast<unsigned int>(center.g) << ','
                << static_cast<unsigned int>(center.b) << ',' << static_cast<unsigned int>(center.a) << " depthBits="
                << depthBits << " framebuffer=0x" << std::hex << framebuffer << " status=0x" << framebufferStatus;
        logError(message.str());
    }
    return passed;
}

bool checkRenderTextureStencilOperation(sf::RenderTexture& target, sf::Color clearColor)
{
    constexpr sf::Color maskedColor(147, 51, 234);
    target.clear(clearColor, 0);

    sf::RectangleShape mask({24.f, 48.f});
    mask.setFillColor(sf::Color::White);
    sf::RenderStates maskState;
    maskState.stencilMode = {sf::StencilComparison::Always, sf::StencilUpdateOperation::Replace, 7, ~0u, true};
    target.draw(mask, maskState);

    sf::RectangleShape overlay({48.f, 48.f});
    overlay.setFillColor(maskedColor);
    sf::RenderStates overlayState;
    overlayState.stencilMode = {sf::StencilComparison::Equal, sf::StencilUpdateOperation::Keep, 7, ~0u, false};
    target.draw(overlay, overlayState);
    target.display();

    const sf::Image copy = target.getTexture().copyToImage();
    return copy.getSize() == sf::Vector2u(48, 48) && copy.getPixel({8, 24}) == maskedColor &&
           copy.getPixel({40, 24}) == clearColor;
}

bool checkSmallRenderTexture(const sf::ContextSettings& settings, sf::Color color)
{
    sf::RenderTexture target({48, 48}, settings);
    if (settings.stencilBits)
        target.clear(color, 1);
    else
        target.clear(color);
    sf::RectangleShape mark({16.f, 16.f});
    mark.setPosition({16.f, 16.f});
    mark.setFillColor(sf::Color::White);
    target.draw(mark);
    target.display();

    const sf::Image copy        = target.getTexture().copyToImage();
    const bool      colorPixels = copy.getSize() == sf::Vector2u(48, 48) && copy.getPixel({0, 0}) == color &&
                             copy.getPixel({24, 24}) == sf::Color::White;
    const bool depthPixels   = !settings.depthBits || checkRenderTextureDepthOperation(target, color);
    const bool stencilPixels = !settings.stencilBits || checkRenderTextureStencilOperation(target, color);
    return colorPixels && depthPixels && stencilPixels;
}

RenderTextureDiagnostics checkRenderTextureConfigurations()
{
    RenderTextureDiagnostics result;
    try
    {
        result.colorOnly    = checkSmallRenderTexture({}, sf::Color(20, 30, 40));
        result.depthOnly    = checkSmallRenderTexture(sf::ContextSettings{16, 0, 0}, sf::Color(30, 40, 50));
        result.stencilOnly  = checkSmallRenderTexture(sf::ContextSettings{0, 8, 0}, sf::Color(40, 50, 60));
        result.depthStencil = checkSmallRenderTexture(sf::ContextSettings{16, 8, 0}, sf::Color(50, 60, 70));
        result.copyPixel    = result.colorOnly && result.depthOnly && result.stencilOnly && result.depthStencil;
    } catch (const sf::Exception& exception)
    {
        logError(std::string("RenderTexture configuration check failed: ") + exception.what());
    }

    const unsigned int maximumAa = sf::RenderTexture::getMaximumAntiAliasingLevel();
    if (maximumAa == 0)
    {
        try
        {
            sf::RenderTexture unsupportedAa({32, 32}, sf::ContextSettings{0, 0, 2});
            static_cast<void>(unsupportedAa);
        } catch (const sf::Exception&)
        {
            result.aaPath = true;
        }
    }
    else
    {
        try
        {
            const unsigned int requestedAa = std::min(2u, maximumAa);
            result.aaPath = checkSmallRenderTexture(sf::ContextSettings{0, 0, requestedAa}, sf::Color(60, 70, 80));
        } catch (const sf::Exception& exception)
        {
            logError(std::string("RenderTexture multisample check failed: ") + exception.what());
        }
    }

    logInfo(std::string("RenderTexture color/depth/stencil/depth+stencil/copy/AA=") + (result.colorOnly ? "1" : "0") +
            (result.depthOnly ? "1" : "0") + (result.stencilOnly ? "1" : "0") + (result.depthStencil ? "1" : "0") +
            (result.copyPixel ? "1" : "0") + (result.aaPath ? "1" : "0"));
    return result;
}

sf::Image makeCheckerboard()
{
    sf::Image image({128, 128}, sf::Color::Black);
    for (unsigned int y = 0; y < 128; ++y)
    {
        for (unsigned int x = 0; x < 128; ++x)
        {
            const bool      odd   = ((x / 16) + (y / 16)) % 2 != 0;
            const sf::Color color = odd ? sf::Color(56, 189, 248) : sf::Color(15, 23, 42);
            image.setPixel({x, y}, color);
        }
    }
    return image;
}

std::vector<std::int16_t> makeTone(unsigned int sampleRate, unsigned int channelCount)
{
    constexpr float frequency = 523.25f;
    constexpr float pi        = 3.14159265358979323846f;

    const std::size_t         frameCount = sampleRate / 5;
    std::vector<std::int16_t> samples(frameCount * channelCount);
    for (std::size_t frame = 0; frame < frameCount; ++frame)
    {
        const float fade  = 1.f - static_cast<float>(frame) / static_cast<float>(frameCount);
        const auto  value = static_cast<std::int16_t>(
            std::sin(2.f * pi * frequency * static_cast<float>(frame) / sampleRate) * fade * 8'000.f);
        for (unsigned int channel = 0; channel < channelCount; ++channel)
            samples[frame * channelCount + channel] = value;
    }
    return samples;
}

bool checkRawfile()
{
    sf::FileInputStream stream;
    if (!stream.open("rawfile:/diagnostics.txt"))
    {
        logError("rawfile:/diagnostics.txt could not be opened");
        return false;
    }

    const auto size = stream.getSize();
    if (!size || (*size == 0))
        return false;

    std::vector<char> data(*size);
    const auto        read = stream.read(data.data(), data.size());
    const bool        ok   = read && (*read == data.size());
    logInfo(ok ? "rawfile resource check passed" : "rawfile resource was truncated");
    return ok;
}

bool checkSandboxFile()
{
    std::error_code error;
    auto            path = std::filesystem::temp_directory_path(error);
    if (error)
    {
        logError("Unable to locate the application sandbox temporary directory: " + error.message());
        return false;
    }

    path /= "sfml-harmony-diagnostics.tmp";
    {
        std::ofstream output(path, std::ios::binary | std::ios::trunc);
        output << "sandbox";
        if (!output)
        {
            logError("Unable to write the application sandbox diagnostic file");
            return false;
        }
    }

    bool result{};
    {
        sf::FileInputStream stream;
        std::array<char, 7> data{};
        const auto          read = stream.open(path) ? stream.read(data.data(), data.size()) : std::nullopt;
        result = read && (*read == data.size()) && (std::string_view(data.data(), data.size()) == "sandbox");
    }

    std::filesystem::remove(path, error);
    if (error)
        logError("Unable to remove the application sandbox diagnostic file: " + error.message());

    logInfo(result ? "sandbox file check passed" : "sandbox file check failed");
    return result && !error;
}

bool checkTcpLoopback(const sf::IpAddress& address)
{
    sf::TcpListener listener;
    if (listener.listen(sf::Socket::AnyPort, address) != sf::Socket::Status::Done)
        return false;

    sf::TcpSocket client;
    if (client.connect(address, listener.getLocalPort(), sf::seconds(1)) != sf::Socket::Status::Done)
        return false;

    sf::TcpSocket server;
    if (listener.accept(server) != sf::Socket::Status::Done)
        return false;

    server.setBlocking(false);
    std::array<std::uint8_t, 4> received{};
    std::size_t                 receivedSize{};
    if (server.receive(received.data(), received.size(), receivedSize) != sf::Socket::Status::NotReady)
        return false;

    constexpr std::array<std::uint8_t, 4> sent{0x53, 0x46, 0x4d, 0x4c};
    if (client.send(sent.data(), sent.size()) != sf::Socket::Status::Done)
        return false;

    bool               callbackReady{};
    sf::SocketSelector selector;
    if (!selector.add(server,
                      sf::SocketSelector::Receive,
                      [&callbackReady](sf::SocketSelector::ReadinessType readiness)
                      { callbackReady = (readiness & sf::SocketSelector::Receive) != 0; }) ||
        !selector.wait(sf::seconds(1)) || !selector.isReady(server, sf::SocketSelector::Receive))
    {
        return false;
    }
    selector.dispatchReadyCallbacks();

    const bool result = callbackReady &&
                        (server.receive(received.data(), received.size(), receivedSize) == sf::Socket::Status::Done) &&
                        (receivedSize == sent.size()) && (received == sent);
    logInfo(std::string(address.isV6() ? "IPv6" : "IPv4") +
            (result ? " TCP/nonblocking/selector check passed" : " TCP/nonblocking/selector check failed"));
    return result;
}

bool checkUdpLoopback(const sf::IpAddress& address)
{
    sf::UdpSocket receiver;
    if (receiver.bind(sf::Socket::AnyPort, address) != sf::Socket::Status::Done)
        return false;

    sf::UdpSocket                         sender;
    constexpr std::array<std::uint8_t, 4> sent{0x4f, 0x48, 0x4f, 0x53};
    if (sender.send(sent.data(), sent.size(), address, receiver.getLocalPort()) != sf::Socket::Status::Done)
        return false;

    std::array<std::uint8_t, 4>  received{};
    std::size_t                  receivedSize{};
    std::optional<sf::IpAddress> remoteAddress;
    unsigned short               remotePort{};
    const bool result = receiver.receive(received.data(), received.size(), receivedSize, remoteAddress, remotePort) ==
                            sf::Socket::Status::Done &&
                        (receivedSize == sent.size()) && (received == sent) && remoteAddress.has_value() &&
                        (remotePort != 0);
    logInfo(std::string(address.isV6() ? "IPv6" : "IPv4") + (result ? " UDP check passed" : " UDP check failed"));
    return result;
}

bool checkDns()
{
    const auto addresses = sf::Dns::resolve("localhost");
    const bool result    = addresses && !addresses->empty();
    logInfo(result ? "DNS localhost resolution passed" : "DNS localhost resolution failed");
    return result;
}

bool checkVulkanSurface(sf::WindowBase& window)
{
    if (!sf::Vulkan::isAvailable())
        return false;

    const auto createInstance = reinterpret_cast<PFN_vkCreateInstance>(sf::Vulkan::getFunction("vkCreateInstance"));
    const auto getInstanceProcAddr = reinterpret_cast<PFN_vkGetInstanceProcAddr>(
        sf::Vulkan::getFunction("vkGetInstanceProcAddr"));
    if (!createInstance || !getInstanceProcAddr)
        return false;

    const auto&       extensions = sf::Vulkan::getGraphicsRequiredInstanceExtensions();
    VkApplicationInfo applicationInfo{};
    applicationInfo.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    applicationInfo.pApplicationName   = "SFML Harmony diagnostics";
    applicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    applicationInfo.pEngineName        = "SFML";
    applicationInfo.engineVersion      = VK_MAKE_VERSION(3, 1, 0);
    applicationInfo.apiVersion         = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo        = &applicationInfo;
    createInfo.enabledExtensionCount   = static_cast<std::uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    VkInstance instance{};
    if (createInstance(&createInfo, nullptr, &instance) != VK_SUCCESS)
        return false;

    auto destroyInstance = reinterpret_cast<PFN_vkDestroyInstance>(getInstanceProcAddr(instance, "vkDestroyInstance"));
    if (!destroyInstance)
        destroyInstance = reinterpret_cast<PFN_vkDestroyInstance>(sf::Vulkan::getFunction("vkDestroyInstance"));
    const auto destroySurface = reinterpret_cast<PFN_vkDestroySurfaceKHR>(
        getInstanceProcAddr(instance, "vkDestroySurfaceKHR"));

    bool         result{};
    VkSurfaceKHR surface{};
    if (window.createVulkanSurface(instance, surface) && destroySurface)
    {
        destroySurface(instance, surface, nullptr);
        result = true;
    }

    if (destroyInstance)
        destroyInstance(instance, nullptr);
    else
        logError("Vulkan instance was created but vkDestroyInstance is unavailable");

    return result;
}

std::string shortName(const std::optional<std::string>& value)
{
    if (!value)
        return "<none>";
    return value->size() <= 28 ? *value : value->substr(0, 25) + "...";
}

std::string shortName(const std::string& value)
{
    return value.empty() ? "<none>" : (value.size() <= 28 ? value : value.substr(0, 25) + "...");
}

std::string vectorText(const sf::Vector3f& value)
{
    std::ostringstream stream;
    stream << std::fixed << std::setprecision(2) << value.x << ',' << value.y << ',' << value.z;
    return stream.str();
}

sf::String makeStatusText(const Diagnostics&           diagnostics,
                          const sf::ContextSettings&   settings,
                          const ExternalTlsDiagnostic& externalTls,
                          bool                         recording,
                          bool                         sensorsEnabled,
                          unsigned int                 page)
{
    const auto         flag = [](bool value) { return value ? "ok" : "--"; };
    std::ostringstream stream;
    stream << '[' << (page + 1) << "/6] SFML Harmony diagnostics  P/top-right: next\n";

    switch (page)
    {
        case 0:
            stream << graphicsApiName << " actual " << settings.majorVersion << '.' << settings.minorVersion << " / "
                   << deviceFormName << ' ' << flag(diagnostics.glVersion) << " sRGB " << flag(settings.sRgbCapable)
                   << " GLerr 0x" << std::hex << diagnostics.glError << std::dec << '\n'
                   << diagnostics.glCapability << '\n'
                   << "raw Image/Texture/Shader " << flag(diagnostics.rawfile) << " sandbox "
                   << flag(diagnostics.sandbox) << '\n'
                   << "TCP4+6 " << flag(diagnostics.tcp) << " UDP4+6 " << flag(diagnostics.udp) << " DNS-local "
                   << flag(diagnostics.dns) << " Vulkan " << flag(diagnostics.vulkan) << '\n'
                   << "Shader " << flag(diagnostics.shader) << " sRGB texture " << flag(diagnostics.srgb) << " VBO "
                   << flag(diagnostics.vertexBuffer) << '\n'
                   << "sRGB mipmap: " << diagnostics.srgbMipmap << '\n'
                   << "push/pop raw GL "
                   << (diagnostics.glStateChecks == 0 ? "pending" : (diagnostics.glStateRestore ? "ok" : "FAIL"))
                   << " #" << diagnostics.glStateChecks << '\n'
                   << "Optional TLS (not baseline):\n  " << externalTls.snapshot() << '\n'
                   << "Font rawfile " << (diagnostics.rawfileFont ? "ok" : "FAIL") << " system "
                   << (diagnostics.systemFont ? "ok" : "unavailable") << " status "
                   << (diagnostics.systemFont ? "system" : (diagnostics.rawfileFont ? "rawfile" : "unavailable"))
                   << "\nAudio: generated PCM\n"
                   << "T/bottom: external TLS  I IME  C clipboard  Esc exit";
            break;

        case 1:
            stream << "Sensors " << (sensorsEnabled ? "ENABLED" : "DISABLED") << "  values=poll XYZ; count=events\n";
            for (std::size_t index = 0; index < diagnostics.sensors.size(); ++index)
            {
                const auto& sensor = diagnostics.sensors[index];
                stream << sensorNames[index] << (index == 5 ? " (radians) " : " ") << flag(sensor.available) << " #"
                       << sensor.eventCount << " event " << vectorText(sensor.eventValue) << '\n'
                       << "  poll " << vectorText(sensor.polledValue) << '\n';
            }
            stream << "S or tap bottom toggles all six available sensors";
            break;

        case 2:
        {
            const auto& input = diagnostics.input;
            stream << "Touch begin/move/end " << input.touchBegan << '/' << input.touchMoved << '/' << input.touchEnded
                   << " active/poll/max " << input.activeTouches.size() << '/' << input.polledTouches << '/'
                   << input.maximumTouches << '\n'
                   << "Mouse xy " << input.mousePosition.x << ',' << input.mousePosition.y << " moves "
                   << input.mouseMoved << " buttons L/R/M/X1/X2 poll ";
            for (const bool pressed : input.mousePolled)
                stream << (pressed ? '1' : '0');
            stream << " events ";
            for (std::size_t index = 0; index < input.mousePressed.size(); ++index)
                stream << input.mousePressed[index] << '/' << input.mouseReleased[index] << ' ';
            stream << '\n'
                   << "Wheel V/H count " << input.verticalWheel << '/' << input.horizontalWheel << " sum "
                   << input.verticalWheelDelta << '/' << input.horizontalWheelDelta << '\n'
                   << "Key down/up/repeat " << input.keyPressed << '/' << input.keyReleased << '/' << input.keyRepeated
                   << " code/scan " << input.lastKey << '/' << input.lastScancode << '\n'
                   << "Modifiers A/C/S/Sys " << input.alt << '/' << input.control << '/' << input.shift << '/'
                   << input.system << " currentlyDown " << input.keysDown.size() << '\n'
                   << "Unicode/IME codepoints " << input.textEntered << '/' << input.imeCodepoints << " backspace "
                   << input.backspaces << " IME-open " << input.imeRequests << '\n'
                   << "Clipboard/cursor/visibility " << input.clipboardRoundTrips << '/' << input.cursorChanges << '/'
                   << input.visibilityChanges << '\n';
            break;
        }

        case 3:
        {
            const auto& joystick = diagnostics.joystick;
            stream << "Gamepad " << (joystick.connected ? "CONNECTED" : "disconnected") << " events +/- "
                   << joystick.connectedEvents << '/' << joystick.disconnectedEvents << '\n'
                   << "name " << (joystick.connected ? utf8(joystick.identification.name) : "<none>") << '\n'
                   << "vendor/product " << joystick.identification.vendorId << '/' << joystick.identification.productId
                   << " buttons " << joystick.buttonCount << " mask 0x" << std::hex << joystick.pressedButtonMask
                   << std::dec << '\n'
                   << "button down/up " << joystick.buttonPressedEvents << '/' << joystick.buttonReleasedEvents
                   << " axis events " << joystick.movedEvents << '\n';
            for (std::size_t index = 0; index < joystickAxes.size(); index += 2)
            {
                stream << joystickAxisNames[index] << (joystick.axisAvailable[index] ? "=" : "(--)=") << std::fixed
                       << std::setprecision(1) << joystick.axisValues[index] << "  " << joystickAxisNames[index + 1]
                       << (joystick.axisAvailable[index + 1] ? "=" : "(--)=") << joystick.axisValues[index + 1] << '\n';
            }
            break;
        }

        case 4:
        {
            const auto& surface = diagnostics.surface;
            const auto& rt      = diagnostics.renderTextures;
            stream << "Surface resize/zero/lost/recreate " << surface.resizeEvents << '/' << surface.zeroSizeEvents
                   << '/' << surface.surfaceLost << '/' << surface.surfaceRecreated << '\n'
                   << "Resource persistence checks/fail " << surface.persistenceChecks << '/'
                   << surface.persistenceFailures << '\n'
                   << "RT color/depth/stencil/combined " << flag(rt.colorOnly) << '/' << flag(rt.depthOnly) << '/'
                   << flag(rt.stencilOnly) << '/' << flag(rt.depthStencil) << '\n'
                   << "RT copy+pixel " << flag(rt.copyPixel) << " AA path " << flag(rt.aaPath) << " maxAA "
                   << sf::RenderTexture::getMaximumAntiAliasingLevel() << '\n'
                   << "Rotate, background/foreground, lock/unlock device\n"
                   << "Existing Texture + Shader + RenderTexture rechecked after restore";
            break;
        }

        case 5:
        {
            const auto& audio = diagnostics.audioState;
            stream << "Playback devices " << audio.playbackDevices.size() << " default "
                   << shortName(audio.defaultPlaybackDevice) << '\n'
                   << "current " << shortName(audio.currentPlaybackDevice) << " rate "
                   << audio.playbackSampleRate.value_or(0) << " default? " << sf::PlaybackDevice::isDefaultDevice()
                   << " null? " << audio.nullRoute << '\n'
                   << "Selectable playback/capture " << flag(audio.playbackSelection) << '/'
                   << flag(audio.captureSelection) << '\n'
                   << "Audio notify " << audio.notifications << " last " << audio.lastNotification << '\n'
                   << "Capture devices " << audio.captureDevices.size() << " default "
                   << shortName(audio.defaultCaptureDevice) << '\n'
                   << "capture current " << shortName(audio.currentCaptureDevice) << '\n'
                   << "Record " << audio.recordingRate << " Hz x" << audio.recordingChannels << " samples/peak "
                   << audio.recordedSamples << '/' << audio.recordedPeak << (recording ? " [RECORDING]" : "") << '\n'
                   << "R record/stop+play  M 44100x1 <-> 48000x2\n"
                   << "N null route  D default route  Space tone  A pause\n"
                   << "Bottom taps: record | format | null | default | tone";
            break;
        }
        default:
            break;
    }

    sf::String result(stream.str());
    if (page == 2)
    {
        result += sf::String("IME text: ");
        result += diagnostics.input.committedText;
        result += sf::String("\nBottom taps: IME | clipboard | cursor | visible");
    }
    return result;
}

std::vector<std::uint8_t> makeCustomCursorPixels()
{
    constexpr sf::Vector2u    size{32, 32};
    std::vector<std::uint8_t> pixels(static_cast<std::size_t>(size.x) * size.y * 4u);
    for (unsigned int y = 0; y < size.y; ++y)
    {
        for (unsigned int x = 0; x < size.x; ++x)
        {
            const bool visible = (x == y) || (x + y == size.x - 1) || (x == size.x / 2) || (y == size.y / 2);
            const auto index   = (static_cast<std::size_t>(y) * size.x + x) * 4u;
            pixels[index]      = 34;
            pixels[index + 1]  = 211;
            pixels[index + 2]  = 238;
            pixels[index + 3]  = visible ? 255 : 0;
        }
    }
    return pixels;
}

const char* audioNotificationName(sf::PlaybackDevice::Notification notification)
{
    switch (notification)
    {
        case sf::PlaybackDevice::Notification::DeviceStarted:
            return "started";
        case sf::PlaybackDevice::Notification::DeviceStopped:
            return "stopped";
        case sf::PlaybackDevice::Notification::DeviceRerouted:
            return "rerouted";
        case sf::PlaybackDevice::Notification::DeviceInterruptionBegan:
            return "interruption began";
        case sf::PlaybackDevice::Notification::DeviceInterruptionEnded:
            return "interruption ended";
        case sf::PlaybackDevice::Notification::DeviceUnlocked:
            return "unlocked";
    }
    return "unknown";
}

void configureView(sf::RenderWindow& window, sf::View& view)
{
    const sf::Vector2u size = window.getSize();
    if ((size.x == 0) || (size.y == 0))
        return;

    constexpr float designWidth  = 1000.f;
    constexpr float designHeight = 1600.f;
    const float     targetAspect = designWidth / designHeight;
    const float     actualAspect = static_cast<float>(size.x) / static_cast<float>(size.y);

    view = sf::View({designWidth / 2.f, designHeight / 2.f}, {designWidth, designHeight});
    if (actualAspect > targetAspect)
        view.setSize({designHeight * actualAspect, designHeight});
    else
        view.setSize({designWidth, designWidth / actualAspect});
    window.setView(view);
}

std::optional<sf::Vector2f> normalizedSurfacePoint(sf::Vector2i pixel, sf::Vector2u surfaceSize)
{
    if (!surfaceSize.x || !surfaceSize.y)
        return std::nullopt;

    return sf::Vector2f(std::clamp(static_cast<float>(pixel.x) / static_cast<float>(surfaceSize.x), 0.f, 1.f),
                        std::clamp(static_cast<float>(pixel.y) / static_cast<float>(surfaceSize.y), 0.f, 1.f));
}

unsigned int normalizedSegment(float position, unsigned int segmentCount)
{
    return std::min(static_cast<unsigned int>(position * static_cast<float>(segmentCount)), segmentCount - 1);
}
} // namespace

extern "C" std::uint32_t sfmlExampleSurfaceFallbackAck()
{
    return surfaceFallbackAcks.load(std::memory_order_acquire);
}

extern "C" std::uint32_t sfmlExampleSurfaceRestoreAck()
{
    return surfaceRestoreAcks.load(std::memory_order_acquire);
}

extern "C" std::uint32_t sfmlExampleSurfaceStressFailures()
{
    return surfaceStressFailures.load(std::memory_order_acquire);
}

int main()
{
    sf::ContextSettings requested;
    requested.depthBits         = 16;
    requested.stencilBits       = 8;
    requested.antiAliasingLevel = 4;
#if defined(SFML_OPENGL_ES)
    requested.majorVersion = 2;
    requested.minorVersion = 0;
#else
    requested.majorVersion = 3;
    requested.minorVersion = 0;
#endif
    requested.sRgbCapable = true;

#if defined(SFML_HARMONY_2IN1)
    constexpr sf::Vector2u initialSize{1200, 800};
    constexpr sf::State    initialState = sf::State::Windowed;
#else
    constexpr sf::Vector2u initialSize{1000, 1600};
    constexpr sf::State    initialState = sf::State::Fullscreen;
#endif

    sf::RenderWindow window(sf::VideoMode(initialSize), "SFML OpenHarmony diagnostics", sf::Style::Default, initialState, requested);
    window.setVerticalSyncEnabled(true);
    window.setKeyRepeatEnabled(true);

    sf::View view;
    configureView(window, view);

    Diagnostics           diagnostics;
    ExternalTlsDiagnostic externalTls;
    const auto&           actualSettings = window.getSettings();
#if defined(SFML_OPENGL_ES)
    diagnostics.glVersion = actualSettings.majorVersion >= 2;
#else
    diagnostics.glVersion = actualSettings.majorVersion >= 3;
#endif
    diagnostics.glCapability = queryGlCapability();
    diagnostics.rawfile      = checkRawfile();
    diagnostics.sandbox      = checkSandboxFile();
    const bool tcpV4         = checkTcpLoopback(sf::IpAddress::LocalHostV4);
    const bool tcpV6         = checkTcpLoopback(sf::IpAddress::LocalHostV6);
    const bool udpV4         = checkUdpLoopback(sf::IpAddress::LocalHostV4);
    const bool udpV6         = checkUdpLoopback(sf::IpAddress::LocalHostV6);
    diagnostics.tcp          = tcpV4 && tcpV6;
    diagnostics.udp          = udpV4 && udpV6;
    diagnostics.dns          = checkDns();

    sf::Image   checkerboard;
    sf::Image   relativeRawfileImage;
    sf::Texture explicitRawfileTexture;
    sf::Texture fallbackRawfileTexture;
    sf::Shader  explicitRawfileShader;
    const bool  explicitImage   = checkerboard.loadFromFile("rawfile:/checker.ppm");
    const bool  fallbackImage   = relativeRawfileImage.loadFromFile("checker.ppm");
    const bool  explicitTexture = explicitRawfileTexture.loadFromFile("rawfile:/checker.ppm");
    const bool  fallbackTexture = fallbackRawfileTexture.loadFromFile("checker.ppm");
    const bool explicitShader = explicitRawfileShader.loadFromFile("rawfile:/diagnostic.frag", sf::Shader::Type::Fragment);
    diagnostics.rawfile = diagnostics.rawfile && explicitImage && fallbackImage && explicitTexture && fallbackTexture &&
                          explicitShader && (checkerboard.getSize() == relativeRawfileImage.getSize()) &&
                          (explicitRawfileTexture.getSize() == fallbackRawfileTexture.getSize());
    if (!explicitImage)
        checkerboard = makeCheckerboard();

    const sf::Texture checkerTexture(checkerboard);
    try
    {
        sf::Texture sRgbTexture(checkerboard, true);
        if (!sRgbTexture.isSrgb())
        {
            diagnostics.srgb       = false;
            diagnostics.srgbMipmap = "sRGB unavailable (linear fallback)";
        }
        else
        {
            const bool generated = sRgbTexture.generateMipmap();
#if !defined(SFML_OPENGL_ES)
            diagnostics.srgb       = generated;
            diagnostics.srgbMipmap = generated ? "desktop GL generated" : "desktop GL FAILED";
#else
            if (actualSettings.majorVersion >= 3)
            {
                diagnostics.srgb       = generated;
                diagnostics.srgbMipmap = generated ? "ES3 core generated" : "ES3 core FAILED";
            }
            else if (hasGlExtension("GL_NV_generate_mipmap_sRGB"))
            {
                diagnostics.srgb       = generated;
                diagnostics.srgbMipmap = generated ? "ES2 NV generated" : "ES2 NV FAILED";
            }
            else
            {
                diagnostics.srgb       = !generated;
                diagnostics.srgbMipmap = generated ? "ES2 EXT unexpectedly generated" : "ES2 EXT rejected (expected)";
            }
#endif
        }
        logInfo("sRGB mipmap diagnostic: " + diagnostics.srgbMipmap);
    } catch (const sf::Exception& exception)
    {
        logError(std::string("sRGB texture initialization failed: ") + exception.what());
    }
    sf::Sprite checker(checkerTexture);
    checker.setScale({2.5f, 2.5f});
    checker.setPosition({90.f, 590.f});

    sf::CircleShape orbit(125.f, 64);
    orbit.setOrigin({125.f, 125.f});
    orbit.setPosition({500.f, 440.f});
    orbit.setFillColor(sf::Color(250, 204, 21, 210));
    orbit.setOutlineColor(sf::Color::White);
    orbit.setOutlineThickness(6.f);

    sf::RectangleShape panel({880.f, 650.f});
    panel.setPosition({60.f, 900.f});
    panel.setFillColor(sf::Color(30, 41, 59, 220));

    sf::VertexArray cpuVertices(sf::PrimitiveType::Triangles, 3);
    cpuVertices[0] = {{610.f, 680.f}, sf::Color(244, 63, 94)};
    cpuVertices[1] = {{880.f, 900.f}, sf::Color(34, 211, 238)};
    cpuVertices[2] = {{550.f, 930.f}, sf::Color(168, 85, 247)};

    const std::array<sf::Vertex, 3> vboVertices{
        {{{120.f, 1020.f}, sf::Color(74, 222, 128)},
         {{430.f, 1020.f}, sf::Color(250, 204, 21)},
         {{280.f, 780.f}, sf::Color(59, 130, 246)}}};
    sf::VertexBuffer vbo(sf::PrimitiveType::Triangles, sf::VertexBuffer::Usage::Static);
    diagnostics.vertexBuffer = vbo.create(vboVertices.size()) && vbo.update(vboVertices.data());

    sf::Shader shader;
    diagnostics.shader = shader.loadFromFile("diagnostic.frag", sf::Shader::Type::Fragment) && explicitShader;

    std::optional<sf::RenderTexture> renderTexture;
    diagnostics.renderTextures = checkRenderTextureConfigurations();
    try
    {
        renderTexture.emplace(sf::Vector2u{320, 320}, sf::ContextSettings{16, 8, 0});
        diagnostics.renderTexture = diagnostics.renderTextures.aaPath && diagnostics.renderTextures.copyPixel;
    } catch (const sf::Exception& exception)
    {
        logError(std::string("RenderTexture initialization failed: ") + exception.what());
    }

    sf::Font rawfileFont;
    diagnostics.rawfileFont = rawfileFont.openFromFile("rawfile:/diagnostic.bdf") && rawfileFont.hasGlyph(U'S') &&
                              rawfileFont.hasGlyph(U'F') && rawfileFont.hasGlyph(U'M') && rawfileFont.hasGlyph(U'L') &&
                              rawfileFont.hasGlyph(U'0') && rawfileFont.hasGlyph(U'a') &&
                              rawfileFont.getGlyph(U'S', 7, false).advance > 0.f &&
                              rawfileFont.getLineSpacing(7) > 0.f && rawfileFont.getTexture(7).getNativeHandle() != 0;
    logInfo(std::string("rawfile font open/glyph/atlas ") + (diagnostics.rawfileFont ? "passed" : "failed"));

    sf::Font font;
    for (const std::filesystem::path candidate :
         {"/system/fonts/HarmonyOS_Sans_SC.ttf",
          "/system/fonts/HarmonyOS_Sans.ttf",
          "/system/fonts/NotoSansCJK-Regular.ttc"})
    {
        if (font.openFromFile(candidate))
        {
            diagnostics.systemFont = true;
            break;
        }
    }

    std::optional<sf::Text> statusText;
    const sf::Font* statusFont = diagnostics.systemFont ? &font : (diagnostics.rawfileFont ? &rawfileFont : nullptr);
    if (statusFont)
    {
        statusText.emplace(*statusFont, "", diagnostics.systemFont ? 23 : 7);
        statusText->setFillColor(sf::Color::White);
        statusText->setPosition({90.f, 930.f});
        if (!diagnostics.systemFont)
            statusText->setScale({2.f, 2.f});
    }

    std::optional<sf::Cursor> handCursor   = sf::Cursor::createFromSystem(sf::Cursor::Type::Hand);
    const auto                cursorPixels = makeCustomCursorPixels();
    std::optional<sf::Cursor> customCursor = sf::Cursor::createFromPixels(cursorPixels.data(), {32, 32}, {16, 16});
    bool                      customCursorActive{};
    bool                      pointerVisible{true};
    if (handCursor)
        window.setMouseCursor(*handCursor);

    bool sensorsEnabled = true;
    setAllSensorsEnabled(diagnostics, sensorsEnabled);

    auto& audioState                 = diagnostics.audioState;
    audioState.playbackDevices       = sf::PlaybackDevice::getAvailableDevices();
    audioState.defaultPlaybackDevice = sf::PlaybackDevice::getDefaultDevice();
    audioState.captureDevices        = sf::SoundRecorder::getAvailableDevices();
    audioState.defaultCaptureDevice  = sf::SoundRecorder::getDefaultDevice();

    std::vector<std::int16_t>      toneSamples;
    std::optional<sf::SoundBuffer> toneBuffer;
    std::optional<sf::Sound>       tone;
    std::atomic<int>               audioNotification{-1};
    std::atomic<std::uint64_t>     audioNotificationCount{};
    sf::PlaybackDevice::setNotificationCallback(
        [&audioNotification, &audioNotificationCount](sf::PlaybackDevice::Notification notification)
        {
            audioNotification.store(static_cast<int>(notification), std::memory_order_relaxed);
            audioNotificationCount.fetch_add(1, std::memory_order_relaxed);
        });

    const auto configureTone = [&]
    {
        if (tone)
            tone->stop();
        tone.reset();
        toneBuffer.reset();
        toneSamples = makeTone(audioState.recordingRate, audioState.recordingChannels);
        const std::vector<sf::SoundChannel>
            channelMap = audioState.recordingChannels == 1
                             ? std::vector<sf::SoundChannel>{sf::SoundChannel::Mono}
                             : std::vector<sf::SoundChannel>{sf::SoundChannel::FrontLeft, sf::SoundChannel::FrontRight};
        toneBuffer.emplace(toneSamples.data(), toneSamples.size(), audioState.recordingChannels, audioState.recordingRate, channelMap);
        tone.emplace(*toneBuffer);
    };

    try
    {
        configureTone();
        diagnostics.audio = true;
    } catch (const sf::Exception& exception)
    {
        logError(std::string("Audio initialization failed: ") + exception.what());
    }

    audioState.playbackSelection = !audioState.playbackDevices.empty();
    for (const auto& name : audioState.playbackDevices)
    {
        const bool selected          = sf::PlaybackDevice::setDevice(name);
        audioState.playbackSelection = audioState.playbackSelection && selected &&
                                       (sf::PlaybackDevice::getDevice() == std::optional<std::string>(name));
    }
    audioState.playbackSelection = audioState.playbackSelection && sf::PlaybackDevice::setDeviceToDefault();
    logInfo(std::string("select every advertised playback device ") + (audioState.playbackSelection ? "passed" : "failed"));

    audioState.currentPlaybackDevice = sf::PlaybackDevice::getDevice();
    audioState.playbackSampleRate    = sf::PlaybackDevice::getDeviceSampleRate();
    logInfo("playback device count=" + std::to_string(audioState.playbackDevices.size()) + " default=" +
            shortName(audioState.defaultPlaybackDevice) + " current=" + shortName(audioState.currentPlaybackDevice) +
            " rate=" + std::to_string(audioState.playbackSampleRate.value_or(0)));
    for (std::size_t index = 0; index < audioState.playbackDevices.size(); ++index)
        logInfo("playback[" + std::to_string(index) + "]=" + audioState.playbackDevices[index]);
    logInfo("capture device count=" + std::to_string(audioState.captureDevices.size()) +
            " default=" + shortName(audioState.defaultCaptureDevice));
    for (std::size_t index = 0; index < audioState.captureDevices.size(); ++index)
        logInfo("capture[" + std::to_string(index) + "]=" + audioState.captureDevices[index]);

    std::optional<MeterRecorder> recorder;
    std::optional<sf::Sound>     recordedSound;
    bool                         recording{};
    bool                         resumeToneAfterFocus{};
    bool                         imeVisibleRequested{};
    unsigned int                 diagnosticPage{};

    audioState.captureSelection = !audioState.captureDevices.empty();
    {
        MeterRecorder selectionProbe;
        for (const auto& name : audioState.captureDevices)
        {
            const bool selected = selectionProbe.setDevice(name);
            audioState.captureSelection = audioState.captureSelection && selected && (selectionProbe.getDevice() == name);
        }
        if (!audioState.defaultCaptureDevice.empty())
            audioState.captureSelection = audioState.captureSelection &&
                                          selectionProbe.setDevice(audioState.defaultCaptureDevice);
    }
    logInfo(std::string("select every advertised capture device ") + (audioState.captureSelection ? "passed" : "failed"));

    diagnostics.surface.hadSurface = window.getNativeHandle() && window.getSize().x && window.getSize().y;

    const auto requestIme = [&]
    {
        ++diagnostics.input.imeRequests;
        imeVisibleRequested = true;
        diagnosticPage      = 2;
        sf::Keyboard::setVirtualKeyboardVisible(true);
    };
    const auto checkClipboard = [&]
    {
        constexpr std::string_view clipboardUtf8     = "SFML OpenHarmony clipboard ✓";
        const sf::String           expectedClipboard = sf::String::fromUtf8(clipboardUtf8.begin(), clipboardUtf8.end());
        sf::Clipboard::setString(expectedClipboard);
        const sf::String clipboard = sf::Clipboard::getString();
        diagnostics.input.clipboardRoundTrips += clipboard == expectedClipboard;
        logInfo("clipboard round-trip: " + utf8(clipboard));
    };
    const auto toggleCursor = [&]
    {
        ++diagnostics.input.cursorChanges;
        customCursorActive = !customCursorActive;
        if (customCursorActive && customCursor)
            window.setMouseCursor(*customCursor);
        else if (handCursor)
            window.setMouseCursor(*handCursor);
    };
    const auto togglePointerVisibility = [&]
    {
        ++diagnostics.input.visibilityChanges;
        pointerVisible = !pointerVisible;
        window.setMouseCursorVisible(pointerVisible);
    };
    const auto toggleSensors = [&]
    {
        sensorsEnabled = !sensorsEnabled;
        setAllSensorsEnabled(diagnostics, sensorsEnabled);
    };
    const auto switchAudioFormat = [&]
    {
        if (recording)
            return;
        if (audioState.recordingChannels == 1)
        {
            audioState.recordingRate     = 48'000;
            audioState.recordingChannels = 2;
        }
        else
        {
            audioState.recordingRate     = 44'100;
            audioState.recordingChannels = 1;
        }
        try
        {
            configureTone();
            diagnostics.audio = true;
            logInfo("audio format switched to " + std::to_string(audioState.recordingRate) + "Hz x" +
                    std::to_string(audioState.recordingChannels));
        } catch (const sf::Exception& exception)
        {
            diagnostics.audio = false;
            logError(std::string("audio format switch failed: ") + exception.what());
        }
    };
    const auto toggleRecording = [&]
    {
        if (!sf::SoundRecorder::isAvailable())
        {
            logError("recording is unavailable or microphone permission was denied");
            return;
        }
        if (!recording)
        {
            if (recordedSound)
                recordedSound->stop();
            recordedSound.reset();
            recorder.emplace();
            recorder->resetMeter();
            recorder->setChannelCount(audioState.recordingChannels);
            recording                       = recorder->start(audioState.recordingRate);
            audioState.currentCaptureDevice = recorder->getDevice();
            logInfo(std::string("recording start ") + (recording ? "passed " : "failed ") +
                    std::to_string(audioState.recordingRate) + "Hz x" + std::to_string(audioState.recordingChannels));
            return;
        }

        recorder->stop();
        recording                  = false;
        audioState.recordedSamples = recorder->getMeteredSampleCount();
        audioState.recordedPeak    = recorder->getPeak();
        if (recorder->getBuffer().getSampleCount() != 0)
        {
            recordedSound.emplace(recorder->getBuffer());
            recordedSound->play();
        }
        logInfo("recording stop samples=" + std::to_string(audioState.recordedSamples) +
                " peak=" + std::to_string(audioState.recordedPeak));
    };
    const auto setNullRoute = [&]
    {
        audioState.nullRoute = sf::PlaybackDevice::setDeviceToNull();
        logInfo(std::string("set null playback route ") + (audioState.nullRoute ? "passed" : "failed"));
    };
    const auto setDefaultRoute = [&]
    {
        const bool result    = sf::PlaybackDevice::setDeviceToDefault();
        audioState.nullRoute = false;
        logInfo(std::string("set default playback route ") + (result ? "passed" : "failed"));
    };
    const auto handleControlTap = [&](sf::Vector2i pixel)
    {
        const auto position = normalizedSurfacePoint(pixel, window.getSize());
        if (!position)
            return;

        if (position->x >= 0.7f && position->y <= 0.2f)
        {
            diagnosticPage = (diagnosticPage + 1) % 6;
            return;
        }

        if (position->y < 0.8f)
            return;

        if (diagnosticPage == 0)
            externalTls.start();
        else if (diagnosticPage == 1)
            toggleSensors();
        else if (diagnosticPage == 2)
        {
            switch (normalizedSegment(position->x, 4))
            {
                case 0:
                    requestIme();
                    break;
                case 1:
                    checkClipboard();
                    break;
                case 2:
                    toggleCursor();
                    break;
                case 3:
                    togglePointerVisibility();
                    break;
            }
        }
        else if (diagnosticPage == 5)
        {
            switch (normalizedSegment(position->x, 5))
            {
                case 0:
                    toggleRecording();
                    break;
                case 1:
                    switchAudioFormat();
                    break;
                case 2:
                    setNullRoute();
                    break;
                case 3:
                    setDefaultRoute();
                    break;
                case 4:
                    if (tone)
                        tone->play();
                    break;
            }
        }
    };

    diagnostics.vulkan = checkVulkanSurface(window);
    logInfo(utf8(makeStatusText(diagnostics, actualSettings, externalTls, recording, sensorsEnabled, diagnosticPage)));

    sf::Clock     animationClock;
    std::uint64_t frame{};
    bool          glIdentityLogged{};
    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();

            if (const auto* resized = event->getIf<sf::Event::Resized>())
            {
                ++diagnostics.surfaceResizeEvents;
                ++diagnostics.surface.resizeEvents;
                logInfo("surface resize " + std::to_string(resized->size.x) + "x" + std::to_string(resized->size.y));
                configureView(window, view);
            }

            if (event->is<sf::Event::FocusLost>() && tone && tone->getStatus() == sf::SoundSource::Status::Playing)
            {
                tone->pause();
                resumeToneAfterFocus = true;
            }

            if (event->is<sf::Event::FocusGained>() && tone && resumeToneAfterFocus)
            {
                tone->play();
                resumeToneAfterFocus = false;
            }

            if (const auto* touch = event->getIf<sf::Event::TouchBegan>())
            {
                auto& input = diagnostics.input;
                ++input.touchBegan;
                input.activeTouches.insert(touch->finger);
                input.maximumTouches = std::max(input.maximumTouches, input.activeTouches.size());
                orbit.setPosition(window.mapPixelToCoords(sf::Vector2i(touch->position)));
                if (tone)
                    tone->play();
                if (input.touchBegan == 1)
                    logInfo("first TouchBegan finger=" + std::to_string(touch->finger));
            }

            if (const auto* touch = event->getIf<sf::Event::TouchMoved>())
            {
                auto& input = diagnostics.input;
                ++input.touchMoved;
                input.activeTouches.insert(touch->finger);
                input.maximumTouches = std::max(input.maximumTouches, input.activeTouches.size());
                if (input.touchMoved == 1)
                    logInfo("first TouchMoved finger=" + std::to_string(touch->finger));
            }

            if (const auto* touch = event->getIf<sf::Event::TouchEnded>())
            {
                auto& input = diagnostics.input;
                ++input.touchEnded;
                input.activeTouches.erase(touch->finger);
                handleControlTap(sf::Vector2i(touch->position));
                if (input.touchEnded == 1)
                    logInfo("first TouchEnded finger=" + std::to_string(touch->finger));
            }

            if (const auto* mouse = event->getIf<sf::Event::MouseButtonPressed>())
            {
                const auto index = static_cast<std::size_t>(mouse->button);
                if (index < diagnostics.input.mousePressed.size())
                    ++diagnostics.input.mousePressed[index];
                orbit.setPosition(window.mapPixelToCoords(mouse->position));
                if (tone)
                    tone->play();
                logInfo("mouse button pressed " + std::to_string(index));
            }

            if (const auto* mouse = event->getIf<sf::Event::MouseButtonReleased>())
            {
                const auto index = static_cast<std::size_t>(mouse->button);
                if (index < diagnostics.input.mouseReleased.size())
                    ++diagnostics.input.mouseReleased[index];
            }

            if (const auto* mouse = event->getIf<sf::Event::MouseMoved>())
            {
                ++diagnostics.input.mouseMoved;
                diagnostics.input.mousePosition = mouse->position;
                if (diagnostics.input.mouseMoved == 1)
                    logInfo("first MouseMoved event");
            }

            if (const auto* wheel = event->getIf<sf::Event::MouseWheelScrolled>())
            {
                if (wheel->wheel == sf::Mouse::Wheel::Vertical)
                {
                    ++diagnostics.input.verticalWheel;
                    diagnostics.input.verticalWheelDelta += wheel->delta;
                }
                else
                {
                    ++diagnostics.input.horizontalWheel;
                    diagnostics.input.horizontalWheelDelta += wheel->delta;
                }
                const float scale = std::clamp(orbit.getScale().x + wheel->delta * 0.05f, 0.5f, 2.f);
                orbit.setScale({scale, scale});
                logInfo(std::string("mouse wheel ") +
                        (wheel->wheel == sf::Mouse::Wheel::Vertical ? "vertical " : "horizontal ") +
                        std::to_string(wheel->delta));
            }

            if (const auto* key = event->getIf<sf::Event::KeyPressed>())
            {
                auto& input = diagnostics.input;
                ++input.keyPressed;
                input.keyRepeated += !input.keysDown.insert(key->scancode).second;
                input.lastKey      = static_cast<int>(key->code);
                input.lastScancode = static_cast<int>(key->scancode);
                input.alt          = key->alt;
                input.control      = key->control;
                input.shift        = key->shift;
                input.system       = key->system;
                logInfo("key down code=" + std::to_string(input.lastKey) +
                        " scan=" + std::to_string(input.lastScancode) + " mods=" + std::to_string(input.alt) +
                        std::to_string(input.control) + std::to_string(input.shift) + std::to_string(input.system));

                if (key->code == sf::Keyboard::Key::Escape)
                    window.close();
                else if (key->code == sf::Keyboard::Key::Space && tone)
                    tone->play();
                else if (key->code == sf::Keyboard::Key::A && tone)
                {
                    if (tone->getStatus() == sf::SoundSource::Status::Playing)
                        tone->pause();
                    else
                        tone->play();
                }
                else if (key->code == sf::Keyboard::Key::C)
                    checkClipboard();
                else if (key->code == sf::Keyboard::Key::R)
                    toggleRecording();
                else if (key->code == sf::Keyboard::Key::M)
                    switchAudioFormat();
                else if (key->code == sf::Keyboard::Key::N)
                    setNullRoute();
                else if (key->code == sf::Keyboard::Key::D)
                    setDefaultRoute();
                else if (key->code == sf::Keyboard::Key::I)
                    requestIme();
                else if (key->code == sf::Keyboard::Key::P)
                {
                    diagnosticPage = (diagnosticPage + 1) % 6;
                }
                else if (key->code == sf::Keyboard::Key::T)
                    externalTls.start();
                else if (key->code == sf::Keyboard::Key::S)
                    toggleSensors();
                else if (key->code == sf::Keyboard::Key::K)
                    toggleCursor();
                else if (key->code == sf::Keyboard::Key::V)
                    togglePointerVisibility();
            }

            if (const auto* key = event->getIf<sf::Event::KeyReleased>())
            {
                ++diagnostics.input.keyReleased;
                diagnostics.input.keysDown.erase(key->scancode);
                if (key->code == sf::Keyboard::Key::Backspace)
                    ++diagnostics.input.backspaces;
            }

            if (const auto* text = event->getIf<sf::Event::TextEntered>())
            {
                ++diagnostics.input.textEntered;
                if (text->unicode == U'\r' || text->unicode == U'\n')
                {
                    imeVisibleRequested = false;
                    sf::Keyboard::setVirtualKeyboardVisible(false);
                }
                else if (text->unicode == U'\b')
                {
                    if (!diagnostics.input.committedText.isEmpty())
                        diagnostics.input.committedText.erase(diagnostics.input.committedText.getSize() - 1);
                }
                else if (text->unicode >= U' ')
                {
                    diagnostics.input.imeCodepoints += imeVisibleRequested;
                    diagnostics.input.committedText += sf::String(text->unicode);
                    if (diagnostics.input.committedText.getSize() > 28)
                        diagnostics.input.committedText.erase(0);
                    logInfo("received Unicode code point " + std::to_string(static_cast<std::uint32_t>(text->unicode)));
                }
            }

            if (const auto* sensor = event->getIf<sf::Event::SensorChanged>())
            {
                diagnostics.sensorEvent = true;
                const std::size_t index = sensorIndex(sensor->type);
                if (index < diagnostics.sensors.size())
                {
                    auto& state      = diagnostics.sensors[index];
                    state.eventValue = sensor->value;
                    ++state.eventCount;
                    if (state.eventCount == 1)
                        logInfo(std::string("first sensor event ") + sensorNames[index] +
                                " XYZ=" + vectorText(sensor->value) +
                                (sensor->type == sf::Sensor::Type::Orientation ? " radians" : ""));
                }
            }

            if (const auto* connected = event->getIf<sf::Event::JoystickConnected>())
            {
                diagnostics.gamepad = true;
                ++diagnostics.joystick.connectedEvents;
                logInfo("gamepad connected id=" + std::to_string(connected->joystickId));
            }
            if (const auto* disconnected = event->getIf<sf::Event::JoystickDisconnected>())
            {
                ++diagnostics.joystick.disconnectedEvents;
                logInfo("gamepad disconnected id=" + std::to_string(disconnected->joystickId));
            }
            if (const auto* moved = event->getIf<sf::Event::JoystickMoved>())
            {
                diagnostics.gamepad = true;
                ++diagnostics.joystick.movedEvents;
                if (diagnostics.joystick.movedEvents == 1)
                    logInfo("first gamepad axis event id=" + std::to_string(moved->joystickId));
            }
            if (const auto* button = event->getIf<sf::Event::JoystickButtonPressed>())
            {
                diagnostics.gamepad = true;
                ++diagnostics.joystick.buttonPressedEvents;
                logInfo("gamepad button down id=" + std::to_string(button->joystickId) +
                        " button=" + std::to_string(button->button));
            }
            if (event->is<sf::Event::JoystickButtonReleased>())
                ++diagnostics.joystick.buttonReleasedEvents;
        }

        // Closing destroys the window context immediately. Do not let the
        // remainder of this frame issue diagnostics or rendering calls on it.
        if (!window.isOpen())
            break;

        pollSensors(diagnostics);
        pollTouches(diagnostics.input, window);
        pollMouse(diagnostics.input, window);
        pollJoystick(diagnostics.joystick);
        diagnostics.gamepad = diagnostics.gamepad || diagnostics.joystick.connected;

        if (recording && recorder)
        {
            audioState.recordedSamples = recorder->getMeteredSampleCount();
            audioState.recordedPeak    = recorder->getPeak();
        }

        const int notification = audioNotification.exchange(-1, std::memory_order_relaxed);
        if (notification >= 0)
        {
            audioState.lastNotification = audioNotificationName(static_cast<sf::PlaybackDevice::Notification>(notification));
            logInfo(std::string("audio device ") +
                    audioNotificationName(static_cast<sf::PlaybackDevice::Notification>(notification)));
        }
        audioState.notifications = audioNotificationCount.load(std::memory_order_relaxed);

        if ((frame % 120) == 0)
        {
            audioState.playbackDevices       = sf::PlaybackDevice::getAvailableDevices();
            audioState.defaultPlaybackDevice = sf::PlaybackDevice::getDefaultDevice();
            audioState.currentPlaybackDevice = sf::PlaybackDevice::getDevice();
            audioState.playbackSampleRate    = sf::PlaybackDevice::getDeviceSampleRate();
            audioState.captureDevices        = sf::SoundRecorder::getAvailableDevices();
            audioState.defaultCaptureDevice  = sf::SoundRecorder::getDefaultDevice();
        }

        const sf::Vector2u surfaceSize  = window.getSize();
        const bool         zeroSize     = !surfaceSize.x || !surfaceSize.y;
        const bool         validSurface = window.getNativeHandle() && !zeroSize;
        if (zeroSize && !diagnostics.surface.zeroSizeActive)
        {
            diagnostics.surface.zeroSizeActive = true;
            ++diagnostics.surface.zeroSizeEvents;
            logInfo("surface entered zero-size state");
        }
        else if (!zeroSize)
        {
            diagnostics.surface.zeroSizeActive = false;
        }

        if (!validSurface && diagnostics.surface.hadSurface)
        {
            diagnostics.surface.hadSurface               = false;
            diagnostics.surface.awaitingFallbackCheck    = true;
            diagnostics.surface.awaitingPersistenceCheck = true;
            ++diagnostics.surface.surfaceLost;
            logInfo("native surface lost");
        }
        else if (validSurface && !diagnostics.surface.hadSurface)
        {
            diagnostics.surface.hadSurface = true;
            ++diagnostics.surface.surfaceRecreated;
            logInfo("native surface recreated");
        }

        if (!validSurface)
        {
            // Reconcile and release an EGLSurface that ArkUI has invalidated,
            // then avoid a busy loop until a non-zero surface is restored.
            window.display();
            if (diagnostics.surface.awaitingFallbackCheck)
            {
                bool passed{};
                try
                {
                    passed = checkSharedResourcesOnCurrentContext(checkerboard, checkerTexture, shader, diagnostics.shader, renderTexture);
                } catch (const std::exception& exception)
                {
                    logError(std::string("surface fallback resource check threw: ") + exception.what());
                }

                diagnostics.surface.awaitingFallbackCheck = false;
                if (passed)
                {
                    const auto sequence = surfaceFallbackAcks.fetch_add(1, std::memory_order_release) + 1;
                    logInfo("surface fallback acknowledgement " + std::to_string(sequence) + " passed");
                }
                else
                {
                    surfaceStressFailures.fetch_add(1, std::memory_order_release);
                    logError("surface fallback resource check failed");
                }
            }
            ++frame;
            sf::sleep(sf::milliseconds(16));
            continue;
        }

        if (diagnostics.surface.awaitingPersistenceCheck)
        {
            bool persisted{};
            try
            {
                persisted = window.setActive() &&
                            checkSharedResourcesOnCurrentContext(checkerboard, checkerTexture, shader, diagnostics.shader, renderTexture);
            } catch (const std::exception& exception)
            {
                logError(std::string("surface restore resource check threw: ") + exception.what());
            }

            ++diagnostics.surface.persistenceChecks;
            diagnostics.surface.persistenceFailures += !persisted;
            diagnostics.surface.awaitingPersistenceCheck = false;
            diagnostics.surface.restoreCheckReady        = true;
            diagnostics.surface.restoreCheckPassed       = persisted;
            logInfo(std::string("resource persistence after surface restore ") + (persisted ? "passed" : "failed"));
        }

        const float elapsed = animationClock.getElapsedTime().asSeconds();
        orbit.setRotation(sf::radians(elapsed));
        if (diagnostics.shader)
            shader.setUniform("pulse", 0.5f + 0.5f * std::sin(elapsed * 2.f));

        if (renderTexture)
        {
            renderTexture->clear(sf::Color(7, 89, 133), 0);
            sf::CircleShape mask(125.f, 64);
            mask.setPosition({35.f, 35.f});
            sf::RenderStates maskState;
            maskState.stencilMode = {sf::StencilComparison::Always, sf::StencilUpdateOperation::Replace, 1, ~0u, true};
            renderTexture->draw(mask, maskState);

            sf::RenderStates contentState;
            contentState.stencilMode = {sf::StencilComparison::Equal, sf::StencilUpdateOperation::Keep, 1, ~0u, false};
            contentState.shader      = diagnostics.shader ? &shader : nullptr;
            contentState.texture     = &checkerTexture;
            sf::RectangleShape textureQuad({320.f, 320.f});
            textureQuad.setTexture(&checkerTexture);
            renderTexture->draw(textureQuad, contentState);
            renderTexture->display();
        }

        window.clear(sf::Color(15, 23, 42), 0);
        window.draw(checker, diagnostics.shader ? sf::RenderStates(&shader) : sf::RenderStates::Default);
        window.draw(cpuVertices, sf::RenderStates(sf::BlendAdd));
        if (diagnostics.vertexBuffer)
            window.draw(vbo);
        window.draw(orbit);

        if (renderTexture)
        {
            sf::Sprite preview(renderTexture->getTexture());
            preview.setPosition({570.f, 120.f});
            window.draw(preview);
        }

        window.draw(panel);
        if (statusText)
        {
            statusText->setString(
                makeStatusText(diagnostics, actualSettings, externalTls, recording, sensorsEnabled, diagnosticPage));
            window.draw(*statusText);
        }

        if (window.setActive())
        {
            if (!glIdentityLogged)
            {
                logGlIdentity();
                glIdentityLogged = true;
            }
            if ((frame % 120) == 0)
            {
                const bool restored = checkGlStateRestoration(window);
                if (diagnostics.glStateChecks == 0)
                    logInfo(std::string("raw GL state restoration ") + (restored ? "passed" : "failed"));
                diagnostics.glStateRestore = diagnostics.glStateRestore && restored;
                ++diagnostics.glStateChecks;
            }
            const GLenum error = getGlError();
            if (diagnostics.glError == GL_NO_ERROR && error != GL_NO_ERROR)
                diagnostics.glError = error;
        }
        window.display();

        if (diagnostics.surface.restoreCheckReady)
        {
            const bool passed = diagnostics.surface.restoreCheckPassed && window.setActive() &&
                                hasUsableCurrentGlContext() && hasCurrentWindowEglSurface(window.getSize()) &&
                                getGlError() == GL_NO_ERROR;
            diagnostics.surface.restoreCheckReady = false;
            if (passed)
            {
                const auto sequence = surfaceRestoreAcks.fetch_add(1, std::memory_order_release) + 1;
                logInfo("surface restore acknowledgement " + std::to_string(sequence) + " passed");
            }
            else
            {
                surfaceStressFailures.fetch_add(1, std::memory_order_release);
                logError("surface restore presentation check failed");
            }
        }

        if (renderTexture && ((frame % 120) == 0))
        {
            const sf::Image snapshot  = renderTexture->getTexture().copyToImage();
            diagnostics.renderTexture = diagnostics.renderTexture && (snapshot.getSize() == sf::Vector2u(320, 320)) &&
                                        (snapshot.getPixel({160, 160}).a != 0);
        }
        ++frame;
    }

    if (recording && recorder)
        recorder->stop();
    externalTls.stop();
    sf::PlaybackDevice::setNotificationCallback({});
    sf::Keyboard::setVirtualKeyboardVisible(false);
    if (audioState.nullRoute)
        static_cast<void>(sf::PlaybackDevice::setDeviceToDefault());
    setAllSensorsEnabled(diagnostics, false);
    return diagnostics.glError == GL_NO_ERROR ? EXIT_SUCCESS : EXIT_FAILURE;
}
