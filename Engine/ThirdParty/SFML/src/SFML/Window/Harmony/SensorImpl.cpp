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

#include <SFML/Window/SensorImpl.hpp>

#include <SFML/System/EnumArray.hpp>
#include <SFML/System/Err.hpp>

#include <algorithm>
#include <mutex>
#include <optional>
#include <ostream>
#include <sensors/oh_sensor.h>

#include <cstdint>


namespace
{
sf::priv::EnumArray<sf::Sensor::Type, bool, sf::Sensor::Count>         available;
sf::priv::EnumArray<sf::Sensor::Type, sf::Vector3f, sf::Sensor::Count> values;
sf::priv::EnumArray<sf::Sensor::Type, std::int64_t, sf::Sensor::Count> samplingIntervals;
std::mutex                                                             sensorMutex;

constexpr std::int64_t defaultSamplingInterval = 16'666'667;


std::optional<Sensor_Type> toHarmonyType(sf::Sensor::Type type)
{
    switch (type)
    {
        case sf::Sensor::Type::Accelerometer:
            return SENSOR_TYPE_ACCELEROMETER;
        case sf::Sensor::Type::Gyroscope:
            return SENSOR_TYPE_GYROSCOPE;
        case sf::Sensor::Type::Magnetometer:
            return SENSOR_TYPE_MAGNETIC_FIELD;
        case sf::Sensor::Type::Gravity:
            return SENSOR_TYPE_GRAVITY;
        case sf::Sensor::Type::UserAcceleration:
            return SENSOR_TYPE_LINEAR_ACCELERATION;
        case sf::Sensor::Type::Orientation:
            return SENSOR_TYPE_ORIENTATION;
    }
    return std::nullopt;
}


std::optional<sf::Sensor::Type> fromHarmonyType(Sensor_Type type)
{
    switch (type)
    {
        case SENSOR_TYPE_ACCELEROMETER:
            return sf::Sensor::Type::Accelerometer;
        case SENSOR_TYPE_GYROSCOPE:
            return sf::Sensor::Type::Gyroscope;
        case SENSOR_TYPE_MAGNETIC_FIELD:
            return sf::Sensor::Type::Magnetometer;
        case SENSOR_TYPE_GRAVITY:
            return sf::Sensor::Type::Gravity;
        case SENSOR_TYPE_LINEAR_ACCELERATION:
            return sf::Sensor::Type::UserAcceleration;
        case SENSOR_TYPE_ORIENTATION:
            return sf::Sensor::Type::Orientation;
        default:
            return std::nullopt;
    }
}


void onSensorEvent(Sensor_Event* event)
{
    if (!event)
        return;

    Sensor_Type   nativeType{};
    float*        data = nullptr;
    std::uint32_t size = 0;
    if (OH_SensorEvent_GetType(event, &nativeType) != SENSOR_SUCCESS ||
        OH_SensorEvent_GetData(event, &data, &size) != SENSOR_SUCCESS || !data || !size)
        return;

    const auto type = fromHarmonyType(nativeType);
    if (!type)
        return;

    sf::Vector3f value;
    value.x = data[0];
    if (size > 1)
        value.y = data[1];
    if (size > 2)
        value.z = data[2];

    if (*type == sf::Sensor::Type::Orientation)
    {
        constexpr float degreesToRadians = 0.01745329251994329577f;
        value *= degreesToRadians;
    }

    const std::lock_guard lock(sensorMutex);
    values[*type] = value;
}
} // namespace


namespace sf::priv
{
void SensorImpl::initialize()
{
    available.fill(false);
    values.fill({});
    samplingIntervals.fill(defaultSamplingInterval);

    // OH_Sensor_GetInfos uses a two-step query: a null array first obtains the
    // device sensor count, then an array of exactly that size receives the
    // Sensor_Info objects.
    std::uint32_t count{};
    const auto    countResult = OH_Sensor_GetInfos(nullptr, &count);
    if (countResult != SENSOR_SUCCESS)
    {
        err() << "Failed to query the Harmony sensor count (error " << static_cast<int>(countResult) << ')' << std::endl;
        return;
    }

    if (count == 0)
    {
        err() << "The Harmony sensor service reported no sensors" << std::endl;
        return;
    }

    const std::uint32_t capacity = count;
    Sensor_Info** const infos    = OH_Sensor_CreateInfos(capacity);
    if (!infos)
    {
        err() << "Failed to allocate information for " << capacity << " Harmony sensors" << std::endl;
        return;
    }

    const auto infosResult = OH_Sensor_GetInfos(infos, &count);
    if (infosResult != SENSOR_SUCCESS)
    {
        err() << "Failed to query Harmony sensor information (error " << static_cast<int>(infosResult) << ')' << std::endl;
        OH_Sensor_DestroyInfos(infos, capacity);
        return;
    }

    if (count > capacity)
        err() << "The Harmony sensor inventory grew while it was being queried; ignoring " << count - capacity
              << " new sensors" << std::endl;

    unsigned int sfmlSensorCount{};
    for (std::uint32_t index = 0; index < std::min(count, capacity); ++index)
    {
        if (!infos[index])
        {
            err() << "The Harmony sensor service returned a null sensor at index " << index << std::endl;
            continue;
        }

        Sensor_Type nativeType{};
        const auto  typeResult = OH_SensorInfo_GetType(infos[index], &nativeType);
        if (typeResult != SENSOR_SUCCESS)
        {
            err() << "Failed to query the Harmony sensor type at index " << index << " (error "
                  << static_cast<int>(typeResult) << ')' << std::endl;
            continue;
        }

        const auto type = fromHarmonyType(nativeType);
        if (!type || available[*type])
            continue;

        available[*type] = true;
        ++sfmlSensorCount;

        std::int64_t minimumInterval{};
        std::int64_t maximumInterval{};
        const bool hasMinimum = OH_SensorInfo_GetMinSamplingInterval(infos[index], &minimumInterval) == SENSOR_SUCCESS;
        const bool hasMaximum = OH_SensorInfo_GetMaxSamplingInterval(infos[index], &maximumInterval) == SENSOR_SUCCESS;

        if (hasMinimum && minimumInterval > 0)
            samplingIntervals[*type] = std::max(samplingIntervals[*type], minimumInterval);
        if (hasMaximum && maximumInterval > 0)
            samplingIntervals[*type] = std::min(samplingIntervals[*type], maximumInterval);

        if (hasMinimum && hasMaximum && minimumInterval > 0 && maximumInterval >= minimumInterval)
        {
            samplingIntervals[*type] = std::clamp(defaultSamplingInterval, minimumInterval, maximumInterval);
        }
    }

    const auto destroyResult = OH_Sensor_DestroyInfos(infos, capacity);
    if (destroyResult != SENSOR_SUCCESS)
        err() << "Failed to release Harmony sensor information (error " << destroyResult << ')' << std::endl;

    if (sfmlSensorCount == 0)
        err() << "The Harmony sensor service reported no sensor types supported by SFML" << std::endl;
}

void SensorImpl::cleanup()
{
}

bool SensorImpl::isAvailable(Sensor::Type sensor)
{
    return available[sensor];
}

bool SensorImpl::open(Sensor::Type sensor)
{
    m_type                = sensor;
    const auto nativeType = toHarmonyType(sensor);
    if (!nativeType || !available[sensor])
        return false;

    m_subscriptionId = OH_Sensor_CreateSubscriptionId();
    m_attribute      = OH_Sensor_CreateSubscriptionAttribute();
    m_subscriber     = OH_Sensor_CreateSubscriber();
    if (!m_subscriptionId || !m_attribute || !m_subscriber)
    {
        err() << "Failed to allocate the Harmony sensor subscription for SFML sensor " << static_cast<int>(sensor)
              << std::endl;
        close();
        return false;
    }

    const auto typeResult = OH_SensorSubscriptionId_SetType(m_subscriptionId, *nativeType);
    if (typeResult != SENSOR_SUCCESS)
    {
        err() << "Failed to select Harmony sensor type " << static_cast<int>(*nativeType) << " (error "
              << static_cast<int>(typeResult) << ')' << std::endl;
        close();
        return false;
    }

    const auto intervalResult = OH_SensorSubscriptionAttribute_SetSamplingInterval(m_attribute, samplingIntervals[sensor]);
    if (intervalResult != SENSOR_SUCCESS)
    {
        err() << "Failed to set the Harmony sensor sampling interval for type " << static_cast<int>(*nativeType)
              << " (error " << static_cast<int>(intervalResult) << ')' << std::endl;
        close();
        return false;
    }

    const auto callbackResult = OH_SensorSubscriber_SetCallback(m_subscriber, onSensorEvent);
    if (callbackResult != SENSOR_SUCCESS)
    {
        err() << "Failed to set the Harmony sensor callback for type " << static_cast<int>(*nativeType) << " (error "
              << static_cast<int>(callbackResult) << ')' << std::endl;
        close();
        return false;
    }

    return true;
}

void SensorImpl::close()
{
    setEnabled(false);
    if (m_subscriber)
        OH_Sensor_DestroySubscriber(m_subscriber);
    if (m_attribute)
        OH_Sensor_DestroySubscriptionAttribute(m_attribute);
    if (m_subscriptionId)
        OH_Sensor_DestroySubscriptionId(m_subscriptionId);
    m_subscriber     = nullptr;
    m_attribute      = nullptr;
    m_subscriptionId = nullptr;
    m_enabled        = false;
}

Vector3f SensorImpl::update() const
{
    const std::lock_guard lock(sensorMutex);
    return values[m_type];
}

void SensorImpl::setEnabled(bool enabled)
{
    if (!m_subscriptionId || !m_attribute || !m_subscriber || enabled == m_enabled)
        return;

    if (enabled)
    {
        const auto result = OH_Sensor_Subscribe(m_subscriptionId, m_attribute, m_subscriber);
        m_enabled         = result == SENSOR_SUCCESS;
        if (!m_enabled)
            err() << "Failed to subscribe to Harmony sensor " << static_cast<int>(*toHarmonyType(m_type)) << " (error "
                  << static_cast<int>(result) << ')' << std::endl;
    }
    else
    {
        const auto result = OH_Sensor_Unsubscribe(m_subscriptionId, m_subscriber);
        if (result != SENSOR_SUCCESS)
            err() << "Failed to unsubscribe from Harmony sensor " << static_cast<int>(*toHarmonyType(m_type))
                  << " (error " << static_cast<int>(result) << ')' << std::endl;
        m_enabled = false;
    }
}

} // namespace sf::priv
