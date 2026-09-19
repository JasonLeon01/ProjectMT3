#include "Graphics/bind_Image.hpp"

#include <algorithm>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

void bind_Image(sol::state_view lua) {
    sol::table sf = lua_sf::sf_table(lua);
    auto type_sf__Image = sf.new_usertype<sf::Image>("Image", sol::no_constructor);
    sol::table table_sf__Image = sf["Image"].get<sol::table>();
    lua_sf::mark_shared_usertype<sf::Image>(lua);
    LUASF_STUB_DOC("\\brief Class for loading, manipulating and saving images");
    LUASF_STUB_CLASS("sf.Image");
    LUASF_STUB_DOC("\\brief Construct the image and fill it with a unique color\n\n\\param size  Width and height of the image\n\\param color Fill color");
    LUASF_STUB_FUNCTION("sf.Image", "new", "fun(size: sf.Vector2u, color: sf.Color): sf.Image");
    LUASF_STUB_OVERLOAD("sf.Image", "new", "fun(size: sf.Vector2u): sf.Image");
    LUASF_STUB_OVERLOAD("sf.Image", "new", "fun(filename: string): sf.Image");
    LUASF_STUB_OVERLOAD("sf.Image", "new", "fun(stream: sf.InputStream): sf.Image");
    LUASF_STUB_OVERLOAD("sf.Image", "new", "fun(): sf.Image");
    LUASF_STUB_OVERLOAD("sf.Image", "new", "fun(size: sf.Vector2u, pixels: any): sf.Image");
    LUASF_STUB_OVERLOAD("sf.Image", "new", "fun(data: any): sf.Image");
    type_sf__Image.set_function("new", sol::factories(
        [](sf::Vector2u size, sf::Color color) {
            return lua_sf::makeLuaSharedObject<sf::Image>(size, color);
        },
        [](sf::Vector2u size) {
            return lua_sf::makeLuaSharedObject<sf::Image>(size);
        },
        [](std::string filename) {
            return lua_sf::makeLuaSharedObject<sf::Image>(std::filesystem::path(filename));
        },
        [](sf::InputStream& stream) {
            return lua_sf::makeLuaSharedObject<sf::Image>(stream);
        },
        []() {
            return lua_sf::makeLuaSharedObject<sf::Image>();
        },
        [](sf::Vector2u size, sol::object pixels) {
            auto pixels_buffer = lua_sf::array_from_object<std::uint8_t>(pixels);
            return lua_sf::makeLuaSharedObject<sf::Image>(size, pixels_buffer.data());
        },
        [](sol::object data) {
            auto data_buffer = lua_sf::array_from_object<std::byte>(data);
            return lua_sf::makeLuaSharedObject<sf::Image>(data_buffer.data(), static_cast<std::size_t>(data_buffer.size()));
        }
    ));
    LUASF_STUB_DOC("\\brief Resize the image and fill it with a unique color\n\n\\param size  Width and height of the image\n\\param color Fill color");
    LUASF_STUB_FUNCTION("sf.Image", "resize", "fun(self: sf.Image, size: sf.Vector2u, color: sf.Color)");
    LUASF_STUB_OVERLOAD("sf.Image", "resize", "fun(self: sf.Image, size: sf.Vector2u)");
    LUASF_STUB_OVERLOAD("sf.Image", "resize", "fun(self: sf.Image, size: sf.Vector2u, pixels: any)");
    type_sf__Image.set_function("resize",
        sol::overload(
            [](sf::Image& self, sf::Vector2u size, sf::Color color) {
                self.resize(size, color);
            },
            [](sf::Image& self, sf::Vector2u size) {
                self.resize(size);
            },
            [](sf::Image& self, sf::Vector2u size, sol::object pixels) {
                auto pixels_buffer = lua_sf::array_from_object<std::uint8_t>(pixels);
                self.resize(size, pixels_buffer.data());
            }
        )
    );
    LUASF_STUB_DOC("\\brief Load the image from a file on disk\n\nThe supported image formats are bmp, png, tga, jpg, gif,\npsd, hdr, pic and pnm. Some format options are not supported,\nlike jpeg with arithmetic coding or ASCII pnm.\nIf this function fails, the image is left unchanged.\n\n\\param filename Path of the image file to load\n\n\\return `true` if loading was successful\n\n\\see `loadFromMemory`, `loadFromStream`, `saveToFile`");
    LUASF_STUB_FUNCTION("sf.Image", "loadFromFile", "fun(self: sf.Image, filename: string): boolean");
    type_sf__Image.set_function("loadFromFile",
        [](sf::Image& self, std::string filename) -> bool {
            return self.loadFromFile(std::filesystem::path(filename));
        }
    );
    LUASF_STUB_DOC("\\brief Load the image from a file in memory\n\nThe supported image formats are bmp, png, tga, jpg, gif,\npsd, hdr, pic and pnm. Some format options are not supported,\nlike jpeg with arithmetic coding or ASCII pnm.\nIf this function fails, the image is left unchanged.\n\n\\param data Pointer to the file data in memory\n\\param size Size of the data to load, in bytes\n\n\\return `true` if loading was successful\n\n\\see `loadFromFile`, `loadFromStream`, `saveToMemory`");
    LUASF_STUB_FUNCTION("sf.Image", "loadFromMemory", "fun(self: sf.Image, data: any): boolean");
    type_sf__Image.set_function("loadFromMemory",
        [](sf::Image& self, sol::object data) -> bool {
            auto data_buffer = lua_sf::array_from_object<std::byte>(data);
            return self.loadFromMemory(data_buffer.data(), static_cast<std::size_t>(data_buffer.size()));
        }
    );
    LUASF_STUB_DOC("\\brief Load the image from a custom stream\n\nThe supported image formats are bmp, png, tga, jpg, gif,\npsd, hdr, pic and pnm. Some format options are not supported,\nlike jpeg with arithmetic coding or ASCII pnm.\nIf this function fails, the image is left unchanged.\n\n\\param stream Source stream to read from\n\n\\return `true` if loading was successful\n\n\\see `loadFromFile`, `loadFromMemory`");
    LUASF_STUB_FUNCTION("sf.Image", "loadFromStream", "fun(self: sf.Image, stream: sf.InputStream): boolean");
    type_sf__Image.set_function("loadFromStream",
        [](sf::Image& self, sf::InputStream& stream) -> bool {
            return self.loadFromStream(stream);
        }
    );
    LUASF_STUB_DOC("\\brief Save the image to a file on disk\n\nThe format of the image is automatically deduced from\nthe extension. The supported image formats are bmp, png,\ntga and jpg. The destination file is overwritten\nif it already exists. This function fails if the image is empty.\n\n\\param filename Path of the file to save\n\n\\return `true` if saving was successful\n\n\\see `saveToMemory`, `loadFromFile`");
    LUASF_STUB_FUNCTION("sf.Image", "saveToFile", "fun(self: sf.Image, filename: string): boolean");
    type_sf__Image.set_function("saveToFile",
        [](sf::Image& self, std::string filename) -> bool {
            return self.saveToFile(std::filesystem::path(filename));
        }
    );
    LUASF_STUB_DOC("\\brief Save the image to a buffer in memory\n\nThe format of the image must be specified.\nThe supported image formats are bmp, png, tga and jpg.\nThis function fails if the image is empty, or if\nthe format was invalid.\n\n\\param format Encoding format to use\n\n\\return Buffer with encoded data if saving was successful,\notherwise `std::nullopt`\n\n\\see `saveToFile`, `loadFromMemory`");
    LUASF_STUB_FUNCTION("sf.Image", "saveToMemory", "fun(self: sf.Image, format: string): integer[]|nil");
    type_sf__Image.set_function("saveToMemory",
        [lua](sf::Image& self, std::string format) -> sol::object {
            return lua_sf::optional_to_object(lua, self.saveToMemory(format));
        }
    );
    LUASF_STUB_DOC("\\brief Return the size (width and height) of the image\n\n\\return Size of the image, in pixels");
    LUASF_STUB_FUNCTION("sf.Image", "getSize", "fun(self: sf.Image): sf.Vector2u");
    type_sf__Image.set_function("getSize",
        [](sf::Image& self) -> sf::Vector2u {
            return self.getSize();
        }
    );
    LUASF_STUB_DOC("\\brief Create a transparency mask from a specified color-key\n\nThis function sets the alpha value of every pixel matching\nthe given color to `alpha` (0 by default), so that they\nbecome transparent.\n\n\\param color Color to make transparent\n\\param alpha Alpha value to assign to transparent pixels");
    LUASF_STUB_FUNCTION("sf.Image", "createMaskFromColor", "fun(self: sf.Image, color: sf.Color, alpha: integer)");
    LUASF_STUB_OVERLOAD("sf.Image", "createMaskFromColor", "fun(self: sf.Image, color: sf.Color)");
    type_sf__Image.set_function("createMaskFromColor",
        sol::overload(
            [](sf::Image& self, sf::Color color, lua_sf::LuaIntegral<std::uint8_t> alpha) {
                self.createMaskFromColor(color, alpha.value());
            },
            [](sf::Image& self, sf::Color color) {
                self.createMaskFromColor(color);
            }
        )
    );
    LUASF_STUB_DOC("\\brief Copy pixels from another image onto this one\n\nThis function does a slow pixel copy and should not be\nused intensively. It can be used to prepare a complex\nstatic image from several others, but if you need this\nkind of feature in real-time you'd better use `sf::RenderTexture`.\n\nIf `sourceRect` is empty, the whole image is copied.\nIf `applyAlpha` is set to `true`, alpha blending is\napplied from the source pixels to the destination pixels\nusing the \\b over operator. If it is `false`, the source\npixels are copied unchanged with their alpha value.\n\nSee https://en.wikipedia.org/wiki/Alpha_compositing for\ndetails on the \\b over operator.\n\nNote that this function can fail if either image is invalid\n(i.e. zero-sized width or height), or if `sourceRect` is\nnot within the boundaries of the `source` parameter, or\nif the destination area is out of the boundaries of this image.\n\nOn failure, the destination image is left unchanged.\n\n\\param source     Source image to copy\n\\param dest       Coordinates of the destination position\n\\param sourceRect Sub-rectangle of the source image to copy\n\\param applyAlpha Should the copy take into account the source transparency?\n\n\\return `true` if the operation was successful, `false` otherwise");
    LUASF_STUB_FUNCTION("sf.Image", "copy", "fun(self: sf.Image, source: sf.Image, dest: sf.Vector2u, sourceRect: sf.IntRect, applyAlpha: boolean): boolean");
    LUASF_STUB_OVERLOAD("sf.Image", "copy", "fun(self: sf.Image, source: sf.Image, dest: sf.Vector2u, sourceRect: sf.IntRect): boolean");
    LUASF_STUB_OVERLOAD("sf.Image", "copy", "fun(self: sf.Image, source: sf.Image, dest: sf.Vector2u): boolean");
    type_sf__Image.set_function("copy",
        sol::overload(
            [](sf::Image& self, const sf::Image& source, sf::Vector2u dest, const sf::IntRect& sourceRect, bool applyAlpha) -> bool {
                return self.copy(source, dest, sourceRect, applyAlpha);
            },
            [](sf::Image& self, const sf::Image& source, sf::Vector2u dest, const sf::IntRect& sourceRect) -> bool {
                return self.copy(source, dest, sourceRect);
            },
            [](sf::Image& self, const sf::Image& source, sf::Vector2u dest) -> bool {
                return self.copy(source, dest);
            }
        )
    );
    LUASF_STUB_DOC("\\brief Change the color of a pixel\n\nThis function doesn't check the validity of the pixel\ncoordinates, using out-of-range values will result in\nan undefined behavior.\n\n\\param coords Coordinates of pixel to change\n\\param color  New color of the pixel\n\n\\see `getPixel`");
    LUASF_STUB_FUNCTION("sf.Image", "setPixel", "fun(self: sf.Image, coords: sf.Vector2u, color: sf.Color)");
    type_sf__Image.set_function("setPixel",
        [](sf::Image& self, sf::Vector2u coords, sf::Color color) {
            self.setPixel(coords, color);
        }
    );
    LUASF_STUB_DOC("\\brief Get the color of a pixel\n\nThis function doesn't check the validity of the pixel\ncoordinates, using out-of-range values will result in\nan undefined behavior.\n\n\\param coords Coordinates of pixel to change\n\n\\return Color of the pixel at given coordinates\n\n\\see `setPixel`");
    LUASF_STUB_FUNCTION("sf.Image", "getPixel", "fun(self: sf.Image, coords: sf.Vector2u): sf.Color");
    type_sf__Image.set_function("getPixel",
        [](sf::Image& self, sf::Vector2u coords) -> sf::Color {
            return self.getPixel(coords);
        }
    );
    LUASF_STUB_DOC("\\brief Get a read-only pointer to the array of pixels\n\nThe returned value points to an array of RGBA pixels made of\n8 bit integer components. The size of the array is\n`width * height * 4 (getSize().x * getSize().y * 4)`.\nWarning: the returned pointer may become invalid if you\nmodify the image, so you should never store it for too long.\nIf the image is empty, a null pointer is returned.\n\n\\return Read-only pointer to the array of pixels");
    LUASF_STUB_FUNCTION("sf.Image", "getPixelsPtr", "fun(self: sf.Image): integer[]");
    type_sf__Image.set_function("getPixelsPtr",
        [](sf::Image& self) {
            const auto* result = self.getPixelsPtr();
            if (!result)
                return sol::as_table(std::vector<std::uint8_t>{});
            std::vector<std::uint8_t> result_values(result, result + static_cast<std::size_t>(self.getSize().x * self.getSize().y * 4));
            return sol::as_table(std::move(result_values));
        }
    );
    LUASF_STUB_DOC("\\brief Flip the image horizontally (left <-> right)");
    LUASF_STUB_FUNCTION("sf.Image", "flipHorizontally", "fun(self: sf.Image)");
    type_sf__Image.set_function("flipHorizontally",
        [](sf::Image& self) {
            self.flipHorizontally();
        }
    );
    LUASF_STUB_DOC("\\brief Flip the image vertically (top <-> bottom)");
    LUASF_STUB_FUNCTION("sf.Image", "flipVertically", "fun(self: sf.Image)");
    type_sf__Image.set_function("flipVertically",
        [](sf::Image& self) {
            self.flipVertically();
        }
    );
}
