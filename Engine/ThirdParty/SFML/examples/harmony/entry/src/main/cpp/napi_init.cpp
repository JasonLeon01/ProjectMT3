////////////////////////////////////////////////////////////
//
// SFML 3.1.0-ME OpenHarmony Stage module registration
//
////////////////////////////////////////////////////////////

#include <SFML/Main/MainHarmony.hpp>
#include <array>
#include <napi/native_api.h>

#include <cstdint>


extern "C" std::uint32_t sfmlExampleSurfaceFallbackAck();
extern "C" std::uint32_t sfmlExampleSurfaceRestoreAck();
extern "C" std::uint32_t sfmlExampleSurfaceStressFailures();


namespace
{
napi_value makeUint32(napi_env env, std::uint32_t value)
{
    napi_value result{};
    return napi_create_uint32(env, value, &result) == napi_ok ? result : nullptr;
}


napi_value surfaceFallbackAck(napi_env env, napi_callback_info)
{
    return makeUint32(env, sfmlExampleSurfaceFallbackAck());
}


napi_value surfaceRestoreAck(napi_env env, napi_callback_info)
{
    return makeUint32(env, sfmlExampleSurfaceRestoreAck());
}


napi_value surfaceStressFailures(napi_env env, napi_callback_info)
{
    return makeUint32(env, sfmlExampleSurfaceStressFailures());
}
} // namespace


extern "C" napi_value initializeModule(napi_env env, napi_value exports)
{
    const auto initializedExports = sf::priv::Harmony::initializeNativeApp(env, exports);
    if (!initializedExports)
        return nullptr;

    const std::array
        properties{napi_property_descriptor{"surfaceFallbackAck", nullptr, ::surfaceFallbackAck, nullptr, nullptr, nullptr, napi_default, nullptr},
                   napi_property_descriptor{"surfaceRestoreAck", nullptr, ::surfaceRestoreAck, nullptr, nullptr, nullptr, napi_default, nullptr},
                   napi_property_descriptor{"surfaceStressFailures",
                                            nullptr,
                                            ::surfaceStressFailures,
                                            nullptr,
                                            nullptr,
                                            nullptr,
                                            napi_default,
                                            nullptr}};

    if (napi_define_properties(env, initializedExports, properties.size(), properties.data()) != napi_ok)
        napi_throw_error(env, nullptr, "Failed to define the SFML Harmony example diagnostics exports");

    return initializedExports;
}


static napi_module entryModule{
    1,
    0,
    nullptr,
    initializeModule,
    "entry",
    nullptr,
    {nullptr, nullptr, nullptr, nullptr},
};


extern "C" __attribute__((constructor)) void registerEntryModule()
{
    napi_module_register(&entryModule);
}
