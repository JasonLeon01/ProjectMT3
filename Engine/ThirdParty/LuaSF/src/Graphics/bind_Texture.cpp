#include "Graphics/bind_Texture.hpp"

#include <algorithm>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

void bind_Texture(sol::state_view lua) {
    sol::table sf = lua_sf::sf_table(lua);
    auto type_sf__Texture = sf.new_usertype<sf::Texture>("Texture", sol::no_constructor);
    sol::table table_sf__Texture = sf["Texture"].get<sol::table>();
    lua_sf::mark_shared_usertype<sf::Texture>(lua);
    LUASF_STUB_DOC("\\brief Image living on the graphics card that can be used for drawing");
    LUASF_STUB_CLASS("sf.Texture");
    LUASF_STUB_DOC("\\brief Construct the texture from a sub-rectangle of a file on disk\n\nThe `area` argument can be used to load only a sub-rectangle\nof the whole image. If you want the entire image then leave\nthe default value (which is an empty `IntRect`).\nIf the `area` rectangle crosses the bounds of the image, it\nis adjusted to fit the image size.\n\nThe maximum size for a texture depends on the graphics\ndriver and can be retrieved with the `getMaximumSize` function.\n\n\\param filename Path of the image file to load\n\\param sRgb     `true` to enable sRGB conversion, `false` to disable it\n\\param area     Area of the image to load\n\n\\throws sf::Exception if loading was unsuccessful\n\n\\see `loadFromFile`, `loadFromMemory`, `loadFromStream`, `loadFromImage`");
    LUASF_STUB_FUNCTION("sf.Texture", "new", "fun(filename: string, sRgb: boolean, area: sf.IntRect): sf.Texture");
    LUASF_STUB_OVERLOAD("sf.Texture", "new", "fun(stream: sf.InputStream, sRgb: boolean, area: sf.IntRect): sf.Texture");
    LUASF_STUB_OVERLOAD("sf.Texture", "new", "fun(image: sf.Image, sRgb: boolean, area: sf.IntRect): sf.Texture");
    LUASF_STUB_OVERLOAD("sf.Texture", "new", "fun(filename: string, sRgb: boolean): sf.Texture");
    LUASF_STUB_OVERLOAD("sf.Texture", "new", "fun(stream: sf.InputStream, sRgb: boolean): sf.Texture");
    LUASF_STUB_OVERLOAD("sf.Texture", "new", "fun(image: sf.Image, sRgb: boolean): sf.Texture");
    LUASF_STUB_OVERLOAD("sf.Texture", "new", "fun(size: sf.Vector2u, sRgb: boolean): sf.Texture");
    LUASF_STUB_OVERLOAD("sf.Texture", "new", "fun(filename: string): sf.Texture");
    LUASF_STUB_OVERLOAD("sf.Texture", "new", "fun(stream: sf.InputStream): sf.Texture");
    LUASF_STUB_OVERLOAD("sf.Texture", "new", "fun(image: sf.Image): sf.Texture");
    LUASF_STUB_OVERLOAD("sf.Texture", "new", "fun(size: sf.Vector2u): sf.Texture");
    LUASF_STUB_OVERLOAD("sf.Texture", "new", "fun(): sf.Texture");
    LUASF_STUB_OVERLOAD("sf.Texture", "new", "fun(data: any, sRgb: boolean, area: sf.IntRect): sf.Texture");
    LUASF_STUB_OVERLOAD("sf.Texture", "new", "fun(data: any, sRgb: boolean): sf.Texture");
    LUASF_STUB_OVERLOAD("sf.Texture", "new", "fun(data: any): sf.Texture");
    type_sf__Texture.set_function("new", sol::factories(
        [](std::string filename, bool sRgb, const sf::IntRect& area) {
            return lua_sf::makeLuaSharedObject<sf::Texture>(std::filesystem::path(filename), sRgb, area);
        },
        [](sf::InputStream& stream, bool sRgb, const sf::IntRect& area) {
            return lua_sf::makeLuaSharedObject<sf::Texture>(stream, sRgb, area);
        },
        [](const sf::Image& image, bool sRgb, const sf::IntRect& area) {
            return lua_sf::makeLuaSharedObject<sf::Texture>(image, sRgb, area);
        },
        [](std::string filename, bool sRgb) {
            return lua_sf::makeLuaSharedObject<sf::Texture>(std::filesystem::path(filename), sRgb);
        },
        [](sf::InputStream& stream, bool sRgb) {
            return lua_sf::makeLuaSharedObject<sf::Texture>(stream, sRgb);
        },
        [](const sf::Image& image, bool sRgb) {
            return lua_sf::makeLuaSharedObject<sf::Texture>(image, sRgb);
        },
        [](sf::Vector2u size, bool sRgb) {
            return lua_sf::makeLuaSharedObject<sf::Texture>(size, sRgb);
        },
        [](std::string filename) {
            return lua_sf::makeLuaSharedObject<sf::Texture>(std::filesystem::path(filename));
        },
        [](sf::InputStream& stream) {
            return lua_sf::makeLuaSharedObject<sf::Texture>(stream);
        },
        [](const sf::Image& image) {
            return lua_sf::makeLuaSharedObject<sf::Texture>(image);
        },
        [](sf::Vector2u size) {
            return lua_sf::makeLuaSharedObject<sf::Texture>(size);
        },
        []() {
            return lua_sf::makeLuaSharedObject<sf::Texture>();
        },
        [](sol::object data, bool sRgb, const sf::IntRect& area) {
            auto data_buffer = lua_sf::array_from_object<std::byte>(data);
            return lua_sf::makeLuaSharedObject<sf::Texture>(data_buffer.data(), static_cast<std::size_t>(data_buffer.size()), sRgb, area);
        },
        [](sol::object data, bool sRgb) {
            auto data_buffer = lua_sf::array_from_object<std::byte>(data);
            return lua_sf::makeLuaSharedObject<sf::Texture>(data_buffer.data(), static_cast<std::size_t>(data_buffer.size()), sRgb);
        },
        [](sol::object data) {
            auto data_buffer = lua_sf::array_from_object<std::byte>(data);
            return lua_sf::makeLuaSharedObject<sf::Texture>(data_buffer.data(), static_cast<std::size_t>(data_buffer.size()));
        }
    ));
    LUASF_STUB_DOC("\\brief Resize the texture\n\nIf this function fails, the texture is left unchanged.\n\n\\param size Width and height of the texture\n\\param sRgb `true` to enable sRGB conversion, `false` to disable it\n\n\\return `true` if resizing was successful, `false` if it failed");
    LUASF_STUB_FUNCTION("sf.Texture", "resize", "fun(self: sf.Texture, size: sf.Vector2u, sRgb: boolean): boolean");
    LUASF_STUB_OVERLOAD("sf.Texture", "resize", "fun(self: sf.Texture, size: sf.Vector2u): boolean");
    type_sf__Texture.set_function("resize",
        sol::overload(
            [](sf::Texture& self, sf::Vector2u size, bool sRgb) -> bool {
                return self.resize(size, sRgb);
            },
            [](sf::Texture& self, sf::Vector2u size) -> bool {
                return self.resize(size);
            }
        )
    );
    LUASF_STUB_DOC("\\brief Load the texture from a file on disk\n\nThe `area` argument can be used to load only a sub-rectangle\nof the whole image. If you want the entire image then leave\nthe default value (which is an empty `IntRect`).\nIf the `area` rectangle crosses the bounds of the image, it\nis adjusted to fit the image size.\n\nThe maximum size for a texture depends on the graphics\ndriver and can be retrieved with the `getMaximumSize` function.\n\nIf this function fails, the texture is left unchanged.\n\n\\param filename Path of the image file to load\n\\param sRgb     `true` to enable sRGB conversion, `false` to disable it\n\\param area     Area of the image to load\n\n\\return `true` if loading was successful, `false` if it failed\n\n\\see `loadFromMemory`, `loadFromStream`, `loadFromImage`");
    LUASF_STUB_FUNCTION("sf.Texture", "loadFromFile", "fun(self: sf.Texture, filename: string, sRgb: boolean, area: sf.IntRect): boolean");
    LUASF_STUB_OVERLOAD("sf.Texture", "loadFromFile", "fun(self: sf.Texture, filename: string, sRgb: boolean): boolean");
    LUASF_STUB_OVERLOAD("sf.Texture", "loadFromFile", "fun(self: sf.Texture, filename: string): boolean");
    type_sf__Texture.set_function("loadFromFile",
        sol::overload(
            [](sf::Texture& self, std::string filename, bool sRgb, const sf::IntRect& area) -> bool {
                return self.loadFromFile(std::filesystem::path(filename), sRgb, area);
            },
            [](sf::Texture& self, std::string filename, bool sRgb) -> bool {
                return self.loadFromFile(std::filesystem::path(filename), sRgb);
            },
            [](sf::Texture& self, std::string filename) -> bool {
                return self.loadFromFile(std::filesystem::path(filename));
            }
        )
    );
    LUASF_STUB_DOC("\\brief Load the texture from a file in memory\n\nThe `area` argument can be used to load only a sub-rectangle\nof the whole image. If you want the entire image then leave\nthe default value (which is an empty `IntRect`).\nIf the `area` rectangle crosses the bounds of the image, it\nis adjusted to fit the image size.\n\nThe maximum size for a texture depends on the graphics\ndriver and can be retrieved with the `getMaximumSize` function.\n\nIf this function fails, the texture is left unchanged.\n\n\\param data Pointer to the file data in memory\n\\param size Size of the data to load, in bytes\n\\param sRgb `true` to enable sRGB conversion, `false` to disable it\n\\param area Area of the image to load\n\n\\return `true` if loading was successful, `false` if it failed\n\n\\see `loadFromFile`, `loadFromStream`, `loadFromImage`");
    LUASF_STUB_FUNCTION("sf.Texture", "loadFromMemory", "fun(self: sf.Texture, data: any, sRgb: boolean, area: sf.IntRect): boolean");
    LUASF_STUB_OVERLOAD("sf.Texture", "loadFromMemory", "fun(self: sf.Texture, data: any, sRgb: boolean): boolean");
    LUASF_STUB_OVERLOAD("sf.Texture", "loadFromMemory", "fun(self: sf.Texture, data: any): boolean");
    type_sf__Texture.set_function("loadFromMemory",
        sol::overload(
            [](sf::Texture& self, sol::object data, bool sRgb, const sf::IntRect& area) -> bool {
                auto data_buffer = lua_sf::array_from_object<std::byte>(data);
                return self.loadFromMemory(data_buffer.data(), static_cast<std::size_t>(data_buffer.size()), sRgb, area);
            },
            [](sf::Texture& self, sol::object data, bool sRgb) -> bool {
                auto data_buffer = lua_sf::array_from_object<std::byte>(data);
                return self.loadFromMemory(data_buffer.data(), static_cast<std::size_t>(data_buffer.size()), sRgb);
            },
            [](sf::Texture& self, sol::object data) -> bool {
                auto data_buffer = lua_sf::array_from_object<std::byte>(data);
                return self.loadFromMemory(data_buffer.data(), static_cast<std::size_t>(data_buffer.size()));
            }
        )
    );
    LUASF_STUB_DOC("\\brief Load the texture from a custom stream\n\nThe `area` argument can be used to load only a sub-rectangle\nof the whole image. If you want the entire image then leave\nthe default value (which is an empty `IntRect`).\nIf the `area` rectangle crosses the bounds of the image, it\nis adjusted to fit the image size.\n\nThe maximum size for a texture depends on the graphics\ndriver and can be retrieved with the `getMaximumSize` function.\n\nIf this function fails, the texture is left unchanged.\n\n\\param stream Source stream to read from\n\\param sRgb   `true` to enable sRGB conversion, `false` to disable it\n\\param area   Area of the image to load\n\n\\return `true` if loading was successful, `false` if it failed\n\n\\see `loadFromFile`, `loadFromMemory`, `loadFromImage`");
    LUASF_STUB_FUNCTION("sf.Texture", "loadFromStream", "fun(self: sf.Texture, stream: sf.InputStream, sRgb: boolean, area: sf.IntRect): boolean");
    LUASF_STUB_OVERLOAD("sf.Texture", "loadFromStream", "fun(self: sf.Texture, stream: sf.InputStream, sRgb: boolean): boolean");
    LUASF_STUB_OVERLOAD("sf.Texture", "loadFromStream", "fun(self: sf.Texture, stream: sf.InputStream): boolean");
    type_sf__Texture.set_function("loadFromStream",
        sol::overload(
            [](sf::Texture& self, sf::InputStream& stream, bool sRgb, const sf::IntRect& area) -> bool {
                return self.loadFromStream(stream, sRgb, area);
            },
            [](sf::Texture& self, sf::InputStream& stream, bool sRgb) -> bool {
                return self.loadFromStream(stream, sRgb);
            },
            [](sf::Texture& self, sf::InputStream& stream) -> bool {
                return self.loadFromStream(stream);
            }
        )
    );
    LUASF_STUB_DOC("\\brief Load the texture from an image\n\nThe `area` argument can be used to load only a sub-rectangle\nof the whole image. If you want the entire image then leave\nthe default value (which is an empty `IntRect`).\nIf the `area` rectangle crosses the bounds of the image, it\nis adjusted to fit the image size.\n\nThe maximum size for a texture depends on the graphics\ndriver and can be retrieved with the `getMaximumSize` function.\n\nIf this function fails, the texture is left unchanged.\n\n\\param image Image to load into the texture\n\\param sRgb  `true` to enable sRGB conversion, `false` to disable it\n\\param area  Area of the image to load\n\n\\return `true` if loading was successful, `false` if it failed\n\n\\see `loadFromFile`, `loadFromMemory`");
    LUASF_STUB_FUNCTION("sf.Texture", "loadFromImage", "fun(self: sf.Texture, image: sf.Image, sRgb: boolean, area: sf.IntRect): boolean");
    LUASF_STUB_OVERLOAD("sf.Texture", "loadFromImage", "fun(self: sf.Texture, image: sf.Image, sRgb: boolean): boolean");
    LUASF_STUB_OVERLOAD("sf.Texture", "loadFromImage", "fun(self: sf.Texture, image: sf.Image): boolean");
    type_sf__Texture.set_function("loadFromImage",
        sol::overload(
            [](sf::Texture& self, const sf::Image& image, bool sRgb, const sf::IntRect& area) -> bool {
                return self.loadFromImage(image, sRgb, area);
            },
            [](sf::Texture& self, const sf::Image& image, bool sRgb) -> bool {
                return self.loadFromImage(image, sRgb);
            },
            [](sf::Texture& self, const sf::Image& image) -> bool {
                return self.loadFromImage(image);
            }
        )
    );
    LUASF_STUB_DOC("\\brief Return the size of the texture\n\n\\return Size in pixels");
    LUASF_STUB_FUNCTION("sf.Texture", "getSize", "fun(self: sf.Texture): sf.Vector2u");
    type_sf__Texture.set_function("getSize",
        [](sf::Texture& self) -> sf::Vector2u {
            return self.getSize();
        }
    );
    LUASF_STUB_DOC("\\brief Copy the texture pixels to an image\n\nThis function performs a slow operation that downloads\nthe texture's pixels from the graphics card and copies\nthem to a new image, potentially applying transformations\nto pixels if necessary (texture may be padded or flipped).\n\n\\return Image containing the texture's pixels\n\n\\see `loadFromImage`");
    LUASF_STUB_FUNCTION("sf.Texture", "copyToImage", "fun(self: sf.Texture): sf.Image");
    type_sf__Texture.set_function("copyToImage",
        [](sf::Texture& self) -> sf::Image {
            return self.copyToImage();
        }
    );
    LUASF_STUB_DOC("\\brief Update a part of this texture from another texture\n\nNo additional check is performed on the size of the texture.\nPassing an invalid combination of texture size and destination\nwill lead to an undefined behavior.\n\nThis function does nothing if either texture was not\npreviously created.\n\n\\param texture Source texture to copy to this texture\n\\param dest    Coordinates of the destination position");
    LUASF_STUB_FUNCTION("sf.Texture", "update", "fun(self: sf.Texture, texture: sf.Texture, dest: sf.Vector2u)");
    LUASF_STUB_OVERLOAD("sf.Texture", "update", "fun(self: sf.Texture, image: sf.Image, dest: sf.Vector2u)");
    LUASF_STUB_OVERLOAD("sf.Texture", "update", "fun(self: sf.Texture, window: sf.Window, dest: sf.Vector2u)");
    LUASF_STUB_OVERLOAD("sf.Texture", "update", "fun(self: sf.Texture, texture: sf.Texture)");
    LUASF_STUB_OVERLOAD("sf.Texture", "update", "fun(self: sf.Texture, image: sf.Image)");
    LUASF_STUB_OVERLOAD("sf.Texture", "update", "fun(self: sf.Texture, window: sf.Window)");
    LUASF_STUB_OVERLOAD("sf.Texture", "update", "fun(self: sf.Texture, pixels: any, size: sf.Vector2u, dest: sf.Vector2u)");
    LUASF_STUB_OVERLOAD("sf.Texture", "update", "fun(self: sf.Texture, pixels: any)");
    type_sf__Texture.set_function("update",
        sol::overload(
            [](sf::Texture& self, const sf::Texture& texture, sf::Vector2u dest) {
                self.update(texture, dest);
            },
            [](sf::Texture& self, const sf::Image& image, sf::Vector2u dest) {
                self.update(image, dest);
            },
            [](sf::Texture& self, const sf::Window& window, sf::Vector2u dest) {
                self.update(window, dest);
            },
            [](sf::Texture& self, const sf::Texture& texture) {
                self.update(texture);
            },
            [](sf::Texture& self, const sf::Image& image) {
                self.update(image);
            },
            [](sf::Texture& self, const sf::Window& window) {
                self.update(window);
            },
            [](sf::Texture& self, sol::object pixels, sf::Vector2u size, sf::Vector2u dest) {
                auto pixels_buffer = lua_sf::array_from_object<std::uint8_t>(pixels);
                self.update(pixels_buffer.data(), size, dest);
            },
            [](sf::Texture& self, sol::object pixels) {
                auto pixels_buffer = lua_sf::array_from_object<std::uint8_t>(pixels);
                self.update(pixels_buffer.data());
            }
        )
    );
    LUASF_STUB_DOC("\\brief Enable or disable the smooth filter\n\nWhen the filter is activated, the texture appears smoother\nso that pixels are less noticeable. However if you want\nthe texture to look exactly the same as its source file,\nyou should leave it disabled.\nThe smooth filter is disabled by default.\n\n\\param smooth `true` to enable smoothing, `false` to disable it\n\n\\see `isSmooth`");
    LUASF_STUB_FUNCTION("sf.Texture", "setSmooth", "fun(self: sf.Texture, smooth: boolean)");
    type_sf__Texture.set_function("setSmooth",
        [](sf::Texture& self, bool smooth) {
            self.setSmooth(smooth);
        }
    );
    LUASF_STUB_DOC("\\brief Tell whether the smooth filter is enabled or not\n\n\\return `true` if smoothing is enabled, `false` if it is disabled\n\n\\see `setSmooth`");
    LUASF_STUB_FUNCTION("sf.Texture", "isSmooth", "fun(self: sf.Texture): boolean");
    type_sf__Texture.set_function("isSmooth",
        [](sf::Texture& self) -> bool {
            return self.isSmooth();
        }
    );
    LUASF_STUB_DOC("\\brief Tell whether the texture source is converted from sRGB or not\n\n\\return `true` if the texture source is converted from sRGB, `false` if not\n\n\\see `setSrgb`");
    LUASF_STUB_FUNCTION("sf.Texture", "isSrgb", "fun(self: sf.Texture): boolean");
    type_sf__Texture.set_function("isSrgb",
        [](sf::Texture& self) -> bool {
            return self.isSrgb();
        }
    );
    LUASF_STUB_DOC("\\brief Enable or disable repeating\n\nRepeating is involved when using texture coordinates\noutside the texture rectangle [0, 0, width, height].\nIn this case, if repeat mode is enabled, the whole texture\nwill be repeated as many times as needed to reach the\ncoordinate (for example, if the X texture coordinate is\n3 * width, the texture will be repeated 3 times).\nIf repeat mode is disabled, the \"extra space\" will instead\nbe filled with border pixels.\nWarning: on very old graphics cards, white pixels may appear\nwhen the texture is repeated. With such cards, repeat mode\ncan be used reliably only if the texture has power-of-two\ndimensions (such as 256x128).\nRepeating is disabled by default.\n\n\\param repeated `true` to repeat the texture, `false` to disable repeating\n\n\\see `isRepeated`");
    LUASF_STUB_FUNCTION("sf.Texture", "setRepeated", "fun(self: sf.Texture, repeated: boolean)");
    type_sf__Texture.set_function("setRepeated",
        [](sf::Texture& self, bool repeated) {
            self.setRepeated(repeated);
        }
    );
    LUASF_STUB_DOC("\\brief Tell whether the texture is repeated or not\n\n\\return `true` if repeat mode is enabled, `false` if it is disabled\n\n\\see `setRepeated`");
    LUASF_STUB_FUNCTION("sf.Texture", "isRepeated", "fun(self: sf.Texture): boolean");
    type_sf__Texture.set_function("isRepeated",
        [](sf::Texture& self) -> bool {
            return self.isRepeated();
        }
    );
    LUASF_STUB_DOC("\\brief Generate a mipmap using the current texture data\n\nMipmaps are pre-computed chains of optimized textures. Each\nlevel of texture in a mipmap is generated by halving each of\nthe previous level's dimensions. This is done until the final\nlevel has the size of 1x1. The textures generated in this process may\nmake use of more advanced filters which might improve the visual quality\nof textures when they are applied to objects much smaller than they are.\nThis is known as minification. Because fewer texels (texture elements)\nhave to be sampled from when heavily minified, usage of mipmaps\ncan also improve rendering performance in certain scenarios.\n\nMipmap generation relies on the necessary OpenGL extension being\navailable. If it is unavailable or generation fails due to another\nreason, this function will return `false`. Mipmap data is only valid from\nthe time it is generated until the next time the base level image is\nmodified, at which point this function will have to be called again to\nregenerate it.\n\n\\return `true` if mipmap generation was successful, `false` if unsuccessful");
    LUASF_STUB_FUNCTION("sf.Texture", "generateMipmap", "fun(self: sf.Texture): boolean");
    type_sf__Texture.set_function("generateMipmap",
        [](sf::Texture& self) -> bool {
            return self.generateMipmap();
        }
    );
    LUASF_STUB_DOC("\\brief Swap the contents of this texture with those of another\n\n\\param right Instance to swap with");
    LUASF_STUB_FUNCTION("sf.Texture", "swap", "fun(self: sf.Texture, right: sf.Texture)");
    type_sf__Texture.set_function("swap",
        [](sf::Texture& self, sf::Texture& right) {
            self.swap(right);
        }
    );
    LUASF_STUB_DOC("\\brief Get the underlying OpenGL handle of the texture.\n\nYou shouldn't need to use this function, unless you have\nvery specific stuff to implement that SFML doesn't support,\nor implement a temporary workaround until a bug is fixed.\n\n\\return OpenGL handle of the texture or 0 if not yet created");
    LUASF_STUB_FUNCTION("sf.Texture", "getNativeHandle", "fun(self: sf.Texture): integer");
    type_sf__Texture.set_function("getNativeHandle",
        [](sf::Texture& self) -> unsigned int {
            return self.getNativeHandle();
        }
    );
    LUASF_STUB_DOC("\\brief Bind a texture for rendering\n\nThis function is not part of the graphics API, it mustn't be\nused when drawing SFML entities. It must be used only if you\nmix `sf::Texture` with OpenGL code.\nIt only changes the `GL_TEXTURE_2D` binding. Direct OpenGL\nrendering code is responsible for converting texture coordinates\nin its shader when pixel coordinates are used.\n\n\\code\nsf::Texture t1, t2;\n...\nsf::Texture::bind(&t1);\n// draw OpenGL stuff that use t1...\nsf::Texture::bind(&t2);\n// draw OpenGL stuff that use t2...\nsf::Texture::bind(nullptr);\n// draw OpenGL stuff that use no texture...\n\\endcode\n\n\\param texture Pointer to the texture to bind, can be null to use no texture");
    LUASF_STUB_FUNCTION("sf.Texture", "bind", "fun(texture: sf.Texture)");
    type_sf__Texture.set_function("bind",
        [](const sf::Texture* texture) {
            sf::Texture::bind(texture);
        }
    );
    LUASF_STUB_DOC("\\brief Get the maximum texture size allowed\n\nThis maximum size is defined by the graphics driver.\nYou can expect a value of 512 pixels for low-end graphics\ncard, and up to 8192 pixels or more for newer hardware.\n\n\\return Maximum size allowed for textures, in pixels");
    LUASF_STUB_FUNCTION("sf.Texture", "getMaximumSize", "fun(): integer");
    type_sf__Texture.set_function("getMaximumSize",
        []() -> unsigned int {
            return sf::Texture::getMaximumSize();
        }
    );
    LUASF_STUB_DOC("\\brief Swap the contents of one texture with those of another\n\n\\param left First instance to swap\n\\param right Second instance to swap");
    LUASF_STUB_FUNCTION("sf", "swap", "fun(left: sf.Texture, right: sf.Texture)");
    sf.set_function("swap",
        [](sf::Texture& left, sf::Texture& right) {
            sf::swap(left, right);
        }
    );
}
