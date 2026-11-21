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

#pragma once

#include <SFML/Graphics/Export.hpp>

#include <SFML/Window/ContextSettings.hpp>

#include <optional>
#include <vector>

#include <cstddef>
#include <cstdint>

namespace sf
{
enum class GraphicsBackend
{
    Automatic,
    OpenGL,
    OpenGLES,
    Vulkan,
    DirectX,
    Metal
};

/// Select a graphics backend and optional minimum version before creating GPU resources.
/// Returns false if the backend is unavailable in this build, the settings are invalid,
/// or an already initialized device cannot meet the request.
[[nodiscard]] SFML_GRAPHICS_API bool setGraphicsBackend(GraphicsBackend                         backend,
                                                        const ContextSettings::BackendSettings& settings = {});

/// Return the active backend, or Automatic before the first successful initialization.
[[nodiscard]] SFML_GRAPHICS_API GraphicsBackend getGraphicsBackend();

/// Merge previously exported driver pipeline cache data into the active device's cache.
/// Supported by Vulkan and Metal. Initializes the selected backend if necessary;
/// select it with setGraphicsBackend first. Data is copied during the call and must be
/// an unmodified blob previously returned by getPipelineCacheData (or vkGetPipelineCacheData).
/// On Metal the blob is a serialized MTLBinaryArchive; several blobs may be loaded.
/// Returns false for null/empty data, incompatible cache headers, unsupported backends,
/// or a driver error. The cache is shared by all shaders, including the built-in shaders.
/// Load before drawing to accelerate subsequent pipeline creation; SPIR-V is still required.
/// File storage, integrity checks, prewarming and cache invalidation belong to the application.
[[nodiscard]] SFML_GRAPHICS_API bool loadPipelineCacheFromMemory(const void* data, std::size_t size);

/// Export the active device's driver pipeline cache for reuse in a later run.
/// Supported by Vulkan and Metal. Initializes the selected backend if necessary.
/// Returns nullopt for unsupported backends or a driver error. Export after drawing or
/// prewarming the desired shader/render-state combinations. Cache hits and compilation
/// avoidance are driver-dependent; hardware/driver changes can invalidate the data.
/// On Metal the export contains the pipelines created since the device was initialized,
/// as an MTLBinaryArchive that is specific to the GPU and OS version.
/// Direct3D 11 driver caches are managed by the OS/driver and cannot be exported here.
[[nodiscard]] SFML_GRAPHICS_API std::optional<std::vector<std::uint8_t>> getPipelineCacheData();
} // namespace sf
