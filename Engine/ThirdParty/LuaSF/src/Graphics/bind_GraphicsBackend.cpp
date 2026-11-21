#include "Graphics/bind_GraphicsBackend.hpp"

#include <algorithm>
#include <array>
#include <string_view>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

namespace { constexpr std::array<std::string_view, 4> docs = {
    "Select a graphics backend and optional minimum version before creating GPU resources.\nReturns false if the backend is unavailable in this build, the settings are invalid,\nor an already initialized device cannot meet the request.",
    "Return the active backend, or Automatic before the first successful initialization.",
    "Merge previously exported driver pipeline cache data into the active device's cache.\nSupported by Vulkan and Metal. Initializes the selected backend if necessary;\nselect it with setGraphicsBackend first. Data is copied during the call and must be\nan unmodified blob previously returned by getPipelineCacheData (or vkGetPipelineCacheData).\nOn Metal the blob is a serialized MTLBinaryArchive; several blobs may be loaded.\nReturns false for null/empty data, incompatible cache headers, unsupported backends,\nor a driver error. The cache is shared by all shaders, including the built-in shaders.\nLoad before drawing to accelerate subsequent pipeline creation; SPIR-V is still required.\nFile storage, integrity checks, prewarming and cache invalidation belong to the application.",
    "Export the active device's driver pipeline cache for reuse in a later run.\nSupported by Vulkan and Metal. Initializes the selected backend if necessary.\nReturns nullopt for unsupported backends or a driver error. Export after drawing or\nprewarming the desired shader/render-state combinations. Cache hits and compilation\navoidance are driver-dependent; hardware/driver changes can invalidate the data.\nOn Metal the export contains the pipelines created since the device was initialized,\nas an MTLBinaryArchive that is specific to the GPU and OS version.\nDirect3D 11 driver caches are managed by the OS/driver and cannot be exported here.",
}; }

void bind_GraphicsBackend(lua_glue::StateView lua) {
    lua_glue::Table sf = lua_sf::sf_table(lua);
    LUASF_STUB_CLASS("sf.GraphicsBackend");
    LUASF_STUB_FIELD("Automatic", "sf.GraphicsBackend");
    LUASF_STUB_FIELD("OpenGL", "sf.GraphicsBackend");
    LUASF_STUB_FIELD("OpenGLES", "sf.GraphicsBackend");
    LUASF_STUB_FIELD("Vulkan", "sf.GraphicsBackend");
    LUASF_STUB_FIELD("DirectX", "sf.GraphicsBackend");
    LUASF_STUB_FIELD("Metal", "sf.GraphicsBackend");
    lua_glue::BindEnum<sf::GraphicsBackend>(sf, "GraphicsBackend", {
        {"Automatic", sf::GraphicsBackend::Automatic},
        {"OpenGL", sf::GraphicsBackend::OpenGL},
        {"OpenGLES", sf::GraphicsBackend::OpenGLES},
        {"Vulkan", sf::GraphicsBackend::Vulkan},
        {"DirectX", sf::GraphicsBackend::DirectX},
        {"Metal", sf::GraphicsBackend::Metal}
    });
    LUASF_STUB_DOC(docs[0]);
    LUASF_STUB_FUNCTION("sf", "setGraphicsBackend", "fun(backend: sf.GraphicsBackend, settings?: nil|sf.OpenGLSettings|sf.OpenGLESSettings|sf.VulkanSettings|sf.DirectXSettings|sf.MetalSettings): boolean");
    lua_glue::BindCallable(sf, "setGraphicsBackend",
        [](sf::GraphicsBackend backend, const std::variant<std::monostate, sf::OpenGLSettings, sf::OpenGLESSettings, sf::VulkanSettings, sf::DirectXSettings, sf::MetalSettings>& settings) -> bool {
            return sf::setGraphicsBackend(backend, settings);
        },
        lua_glue::Defaults{lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return std::variant<std::monostate, sf::OpenGLSettings, sf::OpenGLESSettings, sf::VulkanSettings, sf::DirectXSettings, sf::MetalSettings>{ };
        }}},
        docs[0]
    );
    LUASF_STUB_DOC(docs[1]);
    LUASF_STUB_FUNCTION("sf", "getGraphicsBackend", "fun(): sf.GraphicsBackend");
    lua_glue::BindCallable(sf, "getGraphicsBackend",
        []() -> sf::GraphicsBackend {
            return sf::getGraphicsBackend();
        },
        docs[1]
    );
    LUASF_STUB_DOC(docs[2]);
    LUASF_STUB_FUNCTION("sf", "loadPipelineCacheFromMemory", "fun(data: any): boolean");
    lua_glue::BindCallable(sf, "loadPipelineCacheFromMemory",
        [](lua_glue::Object data) -> bool {
            auto data_buffer = lua_sf::array_from_object<std::byte>(data);
            return sf::loadPipelineCacheFromMemory(data_buffer.data(), static_cast<std::size_t>(data_buffer.size()));
        },
        docs[2]
    );
    LUASF_STUB_DOC(docs[3]);
    LUASF_STUB_FUNCTION("sf", "getPipelineCacheData", "fun(): integer[]|nil");
    lua_glue::BindCallable(sf, "getPipelineCacheData",
        [lua]() -> lua_glue::Object {
            return lua_sf::optional_to_object(lua, sf::getPipelineCacheData());
        },
        docs[3]
    );
}
