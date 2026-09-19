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

#include <SFML/Window/JoystickImpl.hpp>

#include <SFML/System/Err.hpp>
#include <SFML/System/String.hpp>

#include <GameControllerKit/game_device.h>
#include <GameControllerKit/game_pad.h>
#include <algorithm>
#include <arkui/native_key_event.h>
#include <array>
#include <memory>
#include <mutex>
#include <optional>
#include <ostream>
#include <string>
#include <string_view>

#include <cctype>
#include <cstdlib>


namespace
{
struct Device
{
    std::string                  id;
    sf::Joystick::Identification identification;
    sf::priv::JoystickState      state;
};

std::array<Device, sf::Joystick::Count> devices;
std::mutex                              devicesMutex;


// Keep the Harmony button layout compatible with the Android mobile backend.
// The two gaps are intentional: GameControllerKit has no Z button and exposes
// Menu/Home instead of a distinct Select/Mode pair on every controller.
enum ButtonIndex : unsigned int
{
    buttonA         = 0,
    buttonB         = 1,
    buttonC         = 2,
    buttonX         = 3,
    buttonY         = 4,
    buttonL1        = 6,
    buttonR1        = 7,
    buttonL2        = 8,
    buttonR2        = 9,
    buttonL3        = 10,
    buttonR3        = 11,
    buttonStart     = 12,
    buttonSelect    = 13,
    buttonCapture   = 14,
    buttonDpadUp    = 15,
    buttonDpadDown  = 16,
    buttonDpadLeft  = 17,
    buttonDpadRight = 18,
    buttonSpan      = 19
};


using RegisterButtonMonitor  = GameController_ErrorCode (*)(GamePad_ButtonInputMonitorCallback);
using RegisterAxisMonitor    = GameController_ErrorCode (*)(GamePad_AxisInputMonitorCallback);
using UnregisterInputMonitor = GameController_ErrorCode (*)();

struct ButtonMonitor
{
    const char*            name;
    RegisterButtonMonitor  registerMonitor;
    UnregisterInputMonitor unregisterMonitor;
    unsigned int           button;
};

struct AxisMonitor
{
    const char*                       name;
    RegisterAxisMonitor               registerMonitor;
    UnregisterInputMonitor            unregisterMonitor;
    std::array<sf::Joystick::Axis, 2> axes;
};

const std::array buttonMonitors{
    ButtonMonitor{"left shoulder",
                  OH_GamePad_LeftShoulder_RegisterButtonInputMonitor,
                  OH_GamePad_LeftShoulder_UnregisterButtonInputMonitor,
                  buttonL1},
    ButtonMonitor{"right shoulder",
                  OH_GamePad_RightShoulder_RegisterButtonInputMonitor,
                  OH_GamePad_RightShoulder_UnregisterButtonInputMonitor,
                  buttonR1},
    ButtonMonitor{"left trigger button",
                  OH_GamePad_LeftTrigger_RegisterButtonInputMonitor,
                  OH_GamePad_LeftTrigger_UnregisterButtonInputMonitor,
                  buttonL2},
    ButtonMonitor{"right trigger button",
                  OH_GamePad_RightTrigger_RegisterButtonInputMonitor,
                  OH_GamePad_RightTrigger_UnregisterButtonInputMonitor,
                  buttonR2},
    ButtonMonitor{"menu",
                  OH_GamePad_ButtonMenu_RegisterButtonInputMonitor,
                  OH_GamePad_ButtonMenu_UnregisterButtonInputMonitor,
                  buttonStart},
    ButtonMonitor{"home",
                  OH_GamePad_ButtonHome_RegisterButtonInputMonitor,
                  OH_GamePad_ButtonHome_UnregisterButtonInputMonitor,
                  buttonCapture},
    ButtonMonitor{"A", OH_GamePad_ButtonA_RegisterButtonInputMonitor, OH_GamePad_ButtonA_UnregisterButtonInputMonitor, buttonA},
    ButtonMonitor{"B", OH_GamePad_ButtonB_RegisterButtonInputMonitor, OH_GamePad_ButtonB_UnregisterButtonInputMonitor, buttonB},
    ButtonMonitor{"X", OH_GamePad_ButtonX_RegisterButtonInputMonitor, OH_GamePad_ButtonX_UnregisterButtonInputMonitor, buttonX},
    ButtonMonitor{"Y", OH_GamePad_ButtonY_RegisterButtonInputMonitor, OH_GamePad_ButtonY_UnregisterButtonInputMonitor, buttonY},
    ButtonMonitor{"C", OH_GamePad_ButtonC_RegisterButtonInputMonitor, OH_GamePad_ButtonC_UnregisterButtonInputMonitor, buttonC},
    ButtonMonitor{"d-pad left",
                  OH_GamePad_Dpad_LeftButton_RegisterButtonInputMonitor,
                  OH_GamePad_Dpad_LeftButton_UnregisterButtonInputMonitor,
                  buttonDpadLeft},
    ButtonMonitor{"d-pad right",
                  OH_GamePad_Dpad_RightButton_RegisterButtonInputMonitor,
                  OH_GamePad_Dpad_RightButton_UnregisterButtonInputMonitor,
                  buttonDpadRight},
    ButtonMonitor{"d-pad up",
                  OH_GamePad_Dpad_UpButton_RegisterButtonInputMonitor,
                  OH_GamePad_Dpad_UpButton_UnregisterButtonInputMonitor,
                  buttonDpadUp},
    ButtonMonitor{"d-pad down",
                  OH_GamePad_Dpad_DownButton_RegisterButtonInputMonitor,
                  OH_GamePad_Dpad_DownButton_UnregisterButtonInputMonitor,
                  buttonDpadDown},
    ButtonMonitor{"left thumbstick",
                  OH_GamePad_LeftThumbstick_RegisterButtonInputMonitor,
                  OH_GamePad_LeftThumbstick_UnregisterButtonInputMonitor,
                  buttonL3},
    ButtonMonitor{"right thumbstick",
                  OH_GamePad_RightThumbstick_RegisterButtonInputMonitor,
                  OH_GamePad_RightThumbstick_UnregisterButtonInputMonitor,
                  buttonR3}};

const std::array
    axisMonitors{AxisMonitor{"left trigger",
                             OH_GamePad_LeftTrigger_RegisterAxisInputMonitor,
                             OH_GamePad_LeftTrigger_UnregisterAxisInputMonitor,
                             {sf::Joystick::Axis::U, sf::Joystick::Axis::U}},
                 AxisMonitor{"right trigger",
                             OH_GamePad_RightTrigger_RegisterAxisInputMonitor,
                             OH_GamePad_RightTrigger_UnregisterAxisInputMonitor,
                             {sf::Joystick::Axis::V, sf::Joystick::Axis::V}},
                 AxisMonitor{"d-pad",
                             OH_GamePad_Dpad_RegisterAxisInputMonitor,
                             OH_GamePad_Dpad_UnregisterAxisInputMonitor,
                             {sf::Joystick::Axis::PovX, sf::Joystick::Axis::PovY}},
                 AxisMonitor{"left thumbstick",
                             OH_GamePad_LeftThumbstick_RegisterAxisInputMonitor,
                             OH_GamePad_LeftThumbstick_UnregisterAxisInputMonitor,
                             {sf::Joystick::Axis::X, sf::Joystick::Axis::Y}},
                 AxisMonitor{"right thumbstick",
                             OH_GamePad_RightThumbstick_RegisterAxisInputMonitor,
                             OH_GamePad_RightThumbstick_UnregisterAxisInputMonitor,
                             {sf::Joystick::Axis::Z, sf::Joystick::Axis::R}}};

bool                                      deviceMonitorActive{};
std::array<bool, buttonMonitors.size()>   buttonMonitorActive{};
std::array<bool, axisMonitors.size()>     axisMonitorActive{};
std::array<bool, sf::Joystick::AxisCount> availableAxes{};
unsigned int                              availableButtonCount{};


std::string takeString(char* value)
{
    const std::unique_ptr<char, decltype(&std::free)> owned(value, &std::free);
    return owned ? owned.get() : "";
}


std::optional<unsigned int> findDevice(std::string_view id)
{
    for (unsigned int index = 0; index < devices.size(); ++index)
    {
        if (devices[index].id == id)
            return index;
    }
    return std::nullopt;
}


void updateDevice(const GameDevice_DeviceInfo* info, bool connected)
{
    if (!info)
        return;

    GameDevice_DeviceType type = UNKNOWN;
    if (OH_GameDevice_DeviceInfo_GetDeviceType(info, &type) != GAME_CONTROLLER_SUCCESS || type != GAME_PAD)
        return;

    char* rawId = nullptr;
    if (OH_GameDevice_DeviceInfo_GetDeviceId(info, &rawId) != GAME_CONTROLLER_SUCCESS)
        return;
    const std::string id = takeString(rawId);
    if (id.empty())
        return;

    const std::lock_guard lock(devicesMutex);
    auto                  index = findDevice(id);
    if (!index && connected)
    {
        for (unsigned int candidate = 0; candidate < devices.size(); ++candidate)
        {
            if (devices[candidate].id.empty() || !devices[candidate].state.connected)
            {
                index = candidate;
                break;
            }
        }
    }
    if (!index)
        return;

    Device& device = devices[*index];
    if (!connected)
    {
        device.state = {};
        return;
    }

    char* rawName = nullptr;
    OH_GameDevice_DeviceInfo_GetName(info, &rawName);
    const std::string name    = takeString(rawName);
    std::int32_t      product = 0;
    OH_GameDevice_DeviceInfo_GetProduct(info, &product);

    device.id                       = id;
    device.identification.name      = name.empty() ? sf::String("Harmony GamePad")
                                                   : sf::String::fromUtf8(name.begin(), name.end());
    device.identification.vendorId  = 0;
    device.identification.productId = static_cast<unsigned int>(std::max(product, 0));
    device.state.connected          = true;
}


void onDeviceEvent(const GameDevice_DeviceEvent* event)
{
    GameDevice_StatusChangedType status = OFFLINE;
    GameDevice_DeviceInfo*       info   = nullptr;
    if (OH_GameDevice_DeviceEvent_GetChangedType(event, &status) == GAME_CONTROLLER_SUCCESS &&
        OH_GameDevice_DeviceEvent_GetDeviceInfo(event, &info) == GAME_CONTROLLER_SUCCESS)
    {
        updateDevice(info, status == ONLINE);
    }
    if (info)
        OH_GameDevice_DestroyDeviceInfo(&info);
}


std::optional<unsigned int> buttonIndex(std::string name, std::int32_t code)
{
    name.erase(std::remove_if(name.begin(), name.end(), [](unsigned char character) { return !std::isalnum(character); }),
               name.end());
    std::transform(name.begin(),
                   name.end(),
                   name.begin(),
                   [](unsigned char character) { return static_cast<char>(std::tolower(character)); });

    if (name == "a" || name.find("buttona") != std::string::npos)
        return buttonA;
    if (name == "b" || name.find("buttonb") != std::string::npos)
        return buttonB;
    if (name == "c" || name.find("buttonc") != std::string::npos)
        return buttonC;
    if (name == "x" || name.find("buttonx") != std::string::npos)
        return buttonX;
    if (name == "y" || name.find("buttony") != std::string::npos)
        return buttonY;
    if (name.find("leftshoulder") != std::string::npos || name.find("buttonl1") != std::string::npos)
        return buttonL1;
    if (name.find("rightshoulder") != std::string::npos || name.find("buttonr1") != std::string::npos)
        return buttonR1;
    if (name.find("lefttrigger") != std::string::npos || name.find("buttonl2") != std::string::npos)
        return buttonL2;
    if (name.find("righttrigger") != std::string::npos || name.find("buttonr2") != std::string::npos)
        return buttonR2;
    if (name.find("leftthumb") != std::string::npos || name.find("buttonthumbl") != std::string::npos)
        return buttonL3;
    if (name.find("rightthumb") != std::string::npos || name.find("buttonthumbr") != std::string::npos)
        return buttonR3;
    if (name.find("select") != std::string::npos)
        return buttonSelect;
    if (name.find("menu") != std::string::npos || name.find("start") != std::string::npos)
        return buttonStart;
    if (name.find("home") != std::string::npos || name.find("mode") != std::string::npos)
        return buttonCapture;
    if (name.find("dpadup") != std::string::npos)
        return buttonDpadUp;
    if (name.find("dpaddown") != std::string::npos)
        return buttonDpadDown;
    if (name.find("dpadleft") != std::string::npos)
        return buttonDpadLeft;
    if (name.find("dpadright") != std::string::npos)
        return buttonDpadRight;

    // GameControllerKit uses the platform key-code namespace. Never fold an
    // unknown code into the SFML button array: modulo mapping aliases unrelated
    // controls and can also release the wrong button in a pressed-button list.
    switch (code)
    {
        case ARKUI_KEYCODE_BUTTON_A:
            return buttonA;
        case ARKUI_KEYCODE_BUTTON_B:
            return buttonB;
        case ARKUI_KEYCODE_BUTTON_X:
            return buttonX;
        case ARKUI_KEYCODE_BUTTON_Y:
            return buttonY;
        case ARKUI_KEYCODE_BUTTON_L1:
            return buttonL1;
        case ARKUI_KEYCODE_BUTTON_R1:
            return buttonR1;
        case ARKUI_KEYCODE_BUTTON_L2:
            return buttonL2;
        case ARKUI_KEYCODE_BUTTON_R2:
            return buttonR2;
        case ARKUI_KEYCODE_BUTTON_THUMBL:
            return buttonL3;
        case ARKUI_KEYCODE_BUTTON_THUMBR:
            return buttonR3;
        case ARKUI_KEYCODE_BUTTON_START:
            return buttonStart;
        case ARKUI_KEYCODE_BUTTON_SELECT:
            return buttonSelect;
        case ARKUI_KEYCODE_BUTTON_MODE:
            return buttonCapture;
        case ARKUI_KEYCODE_DPAD_UP:
            return buttonDpadUp;
        case ARKUI_KEYCODE_DPAD_DOWN:
            return buttonDpadDown;
        case ARKUI_KEYCODE_DPAD_LEFT:
            return buttonDpadLeft;
        case ARKUI_KEYCODE_DPAD_RIGHT:
            return buttonDpadRight;
        default:
            return std::nullopt;
    }
}


void onButtonEvent(const GamePad_ButtonEvent* event)
{
    char* rawId = nullptr;
    if (OH_GamePad_ButtonEvent_GetDeviceId(event, &rawId) != GAME_CONTROLLER_SUCCESS)
        return;
    const std::string id = takeString(rawId);

    const std::lock_guard lock(devicesMutex);
    const auto            index = findDevice(id);
    if (!index)
        return;

    auto& buttons = devices[*index].state.buttons;
    buttons.fill(false);

    std::int32_t count = 0;
    if (OH_GamePad_PressedButtons_GetCount(event, &count) != GAME_CONTROLLER_SUCCESS)
        return;

    for (std::int32_t item = 0; item < count; ++item)
    {
        GamePad_PressedButton* pressed = nullptr;
        if (OH_GamePad_PressedButtons_GetButtonInfo(event, item, &pressed) != GAME_CONTROLLER_SUCCESS || !pressed)
            continue;

        std::int32_t code    = -1;
        char*        rawName = nullptr;
        OH_GamePad_PressedButton_GetButtonCode(pressed, &code);
        OH_GamePad_PressedButton_GetButtonCodeName(pressed, &rawName);
        if (const auto button = buttonIndex(takeString(rawName), code))
            buttons[*button] = true;
        OH_GamePad_DestroyPressedButton(&pressed);
    }
}


float axisValue(double value)
{
    return static_cast<float>(std::clamp(value, -1.0, 1.0) * 100.0);
}


void onAxisEvent(const GamePad_AxisEvent* event)
{
    char* rawId = nullptr;
    if (OH_GamePad_AxisEvent_GetDeviceId(event, &rawId) != GAME_CONTROLLER_SUCCESS)
        return;
    const std::string id = takeString(rawId);

    GamePad_AxisSourceType source{};
    if (OH_GamePad_AxisEvent_GetAxisSourceType(event, &source) != GAME_CONTROLLER_SUCCESS)
        return;

    const std::lock_guard lock(devicesMutex);
    const auto            index = findDevice(id);
    if (!index)
        return;
    auto& axes = devices[*index].state.axes;

    switch (source)
    {
        case LEFT_THUMBSTICK:
        {
            double x = 0;
            double y = 0;
            if (OH_GamePad_AxisEvent_GetXAxisValue(event, &x) == GAME_CONTROLLER_SUCCESS)
                axes[sf::Joystick::Axis::X] = axisValue(x);
            if (OH_GamePad_AxisEvent_GetYAxisValue(event, &y) == GAME_CONTROLLER_SUCCESS)
                axes[sf::Joystick::Axis::Y] = axisValue(y);
            break;
        }
        case RIGHT_THUMBSTICK:
        {
            double z  = 0;
            double rz = 0;
            if (OH_GamePad_AxisEvent_GetZAxisValue(event, &z) == GAME_CONTROLLER_SUCCESS)
                axes[sf::Joystick::Axis::Z] = axisValue(z);
            if (OH_GamePad_AxisEvent_GetRZAxisValue(event, &rz) == GAME_CONTROLLER_SUCCESS)
                axes[sf::Joystick::Axis::R] = axisValue(rz);
            break;
        }
        case DPAD:
        {
            double x = 0;
            double y = 0;
            if (OH_GamePad_AxisEvent_GetHatXAxisValue(event, &x) == GAME_CONTROLLER_SUCCESS)
                axes[sf::Joystick::Axis::PovX] = axisValue(x);
            if (OH_GamePad_AxisEvent_GetHatYAxisValue(event, &y) == GAME_CONTROLLER_SUCCESS)
                axes[sf::Joystick::Axis::PovY] = axisValue(y);
            break;
        }
        case LEFT_TRIGGER:
        {
            double brake = 0;
            if (OH_GamePad_AxisEvent_GetBrakeAxisValue(event, &brake) == GAME_CONTROLLER_SUCCESS)
                axes[sf::Joystick::Axis::U] = axisValue(brake);
            break;
        }
        case RIGHT_TRIGGER:
        {
            double gas = 0;
            if (OH_GamePad_AxisEvent_GetGasAxisValue(event, &gas) == GAME_CONTROLLER_SUCCESS)
                axes[sf::Joystick::Axis::V] = axisValue(gas);
            break;
        }
    }
}


void registerMonitors()
{
    deviceMonitorActive = OH_GameDevice_RegisterDeviceMonitor(onDeviceEvent) == GAME_CONTROLLER_SUCCESS;
    if (!deviceMonitorActive)
        sf::err() << "Failed to register the Harmony game-device monitor" << std::endl;

    buttonMonitorActive  = {};
    axisMonitorActive    = {};
    availableAxes        = {};
    availableButtonCount = 0;

    for (std::size_t index = 0; index < buttonMonitors.size(); ++index)
    {
        const auto& monitor        = buttonMonitors[index];
        buttonMonitorActive[index] = monitor.registerMonitor(onButtonEvent) == GAME_CONTROLLER_SUCCESS;
        if (buttonMonitorActive[index])
        {
            availableButtonCount = std::max(availableButtonCount, monitor.button + 1);
        }
        else
        {
            sf::err() << "Failed to register the Harmony gamepad " << monitor.name << " button monitor" << std::endl;
        }
    }

    for (std::size_t index = 0; index < axisMonitors.size(); ++index)
    {
        const auto& monitor      = axisMonitors[index];
        axisMonitorActive[index] = monitor.registerMonitor(onAxisEvent) == GAME_CONTROLLER_SUCCESS;
        if (axisMonitorActive[index])
        {
            for (const auto axis : monitor.axes)
                availableAxes[static_cast<std::size_t>(axis)] = true;
        }
        else
        {
            sf::err() << "Failed to register the Harmony gamepad " << monitor.name << " axis monitor" << std::endl;
        }
    }
}


void unregisterMonitors()
{
    if (deviceMonitorActive)
    {
        if (OH_GameDevice_UnregisterDeviceMonitor() != GAME_CONTROLLER_SUCCESS)
            sf::err() << "Failed to unregister the Harmony game-device monitor" << std::endl;
        deviceMonitorActive = false;
    }

    for (std::size_t index = 0; index < buttonMonitors.size(); ++index)
    {
        if (!buttonMonitorActive[index])
            continue;
        if (buttonMonitors[index].unregisterMonitor() != GAME_CONTROLLER_SUCCESS)
            sf::err() << "Failed to unregister the Harmony gamepad " << buttonMonitors[index].name << " button monitor"
                      << std::endl;
        buttonMonitorActive[index] = false;
    }

    for (std::size_t index = 0; index < axisMonitors.size(); ++index)
    {
        if (!axisMonitorActive[index])
            continue;
        if (axisMonitors[index].unregisterMonitor() != GAME_CONTROLLER_SUCCESS)
            sf::err() << "Failed to unregister the Harmony gamepad " << axisMonitors[index].name << " axis monitor"
                      << std::endl;
        axisMonitorActive[index] = false;
    }

    availableAxes        = {};
    availableButtonCount = 0;
}
} // namespace


namespace sf::priv
{
void JoystickImpl::initialize()
{
    GameDevice_AllDeviceInfos* infos = nullptr;
    if (OH_GameDevice_GetAllDeviceInfos(&infos) == GAME_CONTROLLER_SUCCESS && infos)
    {
        std::int32_t count = 0;
        if (OH_GameDevice_AllDeviceInfos_GetCount(infos, &count) == GAME_CONTROLLER_SUCCESS)
        {
            for (std::int32_t index = 0; index < count; ++index)
            {
                GameDevice_DeviceInfo* info = nullptr;
                if (OH_GameDevice_AllDeviceInfos_GetDeviceInfo(infos, index, &info) == GAME_CONTROLLER_SUCCESS)
                    updateDevice(info, true);
                if (info)
                    OH_GameDevice_DestroyDeviceInfo(&info);
            }
        }
        OH_GameDevice_DestroyAllDeviceInfos(&infos);
    }
    registerMonitors();
}


void JoystickImpl::cleanup()
{
    unregisterMonitors();
    const std::lock_guard lock(devicesMutex);
    devices = {};
}


bool JoystickImpl::isConnected(unsigned int index)
{
    if (index >= devices.size())
        return false;
    const std::lock_guard lock(devicesMutex);
    return devices[index].state.connected;
}


bool JoystickImpl::open(unsigned int index)
{
    if (!isConnected(index))
        return false;
    m_index = index;
    return true;
}


void JoystickImpl::close()
{
    m_index.reset();
}


JoystickCaps JoystickImpl::getCapabilities() const
{
    JoystickCaps capabilities;
    if (!m_index)
        return capabilities;
    // GameControllerKit exposes a standardized GamePad surface rather than a
    // per-device query. Report only monitors that were actually accepted by
    // the service; failures remain visible instead of yielding inert controls.
    capabilities.buttonCount = availableButtonCount;
    for (std::size_t index = 0; index < availableAxes.size(); ++index)
        capabilities.axes[static_cast<Joystick::Axis>(index)] = availableAxes[index];
    return capabilities;
}


Joystick::Identification JoystickImpl::getIdentification() const
{
    if (!m_index)
        return {};
    const std::lock_guard lock(devicesMutex);
    return devices[*m_index].identification;
}


JoystickState JoystickImpl::update()
{
    if (!m_index)
        return {};
    const std::lock_guard lock(devicesMutex);
    return devices[*m_index].state;
}

} // namespace sf::priv
