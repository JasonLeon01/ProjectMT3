#include "Graphics/bind_Texture.hpp"

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

namespace { constexpr std::array<std::string_view, 37> docs = {
    "\\brief Image living on the graphics card that can be used for drawing",
    "\\brief Default constructor\n\nCreates a texture with width 0 and height 0.\n\n\\see `resize`",
    "\\brief Construct the texture from a file on disk\n\nThe maximum size for a texture depends on the graphics\ndriver and can be retrieved with the getMaximumSize function.\n\n\\param filename Path of the image file to load\n\\param sRgb     `true` to enable sRGB conversion, `false` to disable it\n\n\\throws sf::Exception if loading was unsuccessful\n\n\\see `loadFromFile`, `loadFromMemory`, `loadFromStream`, `loadFromImage`",
    "\\brief Construct the texture from a sub-rectangle of a file on disk\n\nThe `area` argument can be used to load only a sub-rectangle\nof the whole image. If you want the entire image then leave\nthe default value (which is an empty `IntRect`).\nIf the `area` rectangle crosses the bounds of the image, it\nis adjusted to fit the image size.\n\nThe maximum size for a texture depends on the graphics\ndriver and can be retrieved with the `getMaximumSize` function.\n\n\\param filename Path of the image file to load\n\\param sRgb     `true` to enable sRGB conversion, `false` to disable it\n\\param area     Area of the image to load\n\n\\throws sf::Exception if loading was unsuccessful\n\n\\see `loadFromFile`, `loadFromMemory`, `loadFromStream`, `loadFromImage`",
    "\\brief Construct the texture from a file in memory\n\nThe maximum size for a texture depends on the graphics\ndriver and can be retrieved with the `getMaximumSize` function.\n\n\\param data Pointer to the file data in memory\n\\param size Size of the data to load, in bytes\n\\param sRgb `true` to enable sRGB conversion, `false` to disable it\n\n\\throws sf::Exception if loading was unsuccessful\n\n\\see `loadFromFile`, `loadFromMemory`, `loadFromStream`, `loadFromImage`",
    "\\brief Construct the texture from a sub-rectangle of a file in memory\n\nThe `area` argument can be used to load only a sub-rectangle\nof the whole image. If you want the entire image then leave\nthe default value (which is an empty `IntRect`).\nIf the `area` rectangle crosses the bounds of the image, it\nis adjusted to fit the image size.\n\nThe maximum size for a texture depends on the graphics\ndriver and can be retrieved with the `getMaximumSize` function.\n\n\\param data Pointer to the file data in memory\n\\param size Size of the data to load, in bytes\n\\param sRgb `true` to enable sRGB conversion, `false` to disable it\n\\param area Area of the image to load\n\n\\throws sf::Exception if loading was unsuccessful\n\n\\see `loadFromFile`, `loadFromMemory`, `loadFromStream`, `loadFromImage`",
    "\\brief Construct the texture from a custom stream\n\nThe maximum size for a texture depends on the graphics\ndriver and can be retrieved with the `getMaximumSize` function.\n\n\\param stream Source stream to read from\n\\param sRgb   `true` to enable sRGB conversion, `false` to disable it\n\n\\throws sf::Exception if loading was unsuccessful\n\n\\see `loadFromFile`, `loadFromMemory`, `loadFromStream`, `loadFromImage`",
    "\\brief Construct the texture from a sub-rectangle of a custom stream\n\nThe `area` argument can be used to load only a sub-rectangle\nof the whole image. If you want the entire image then leave\nthe default value (which is an empty `IntRect`).\nIf the `area` rectangle crosses the bounds of the image, it\nis adjusted to fit the image size.\n\nThe maximum size for a texture depends on the graphics\ndriver and can be retrieved with the `getMaximumSize` function.\n\n\\param stream Source stream to read from\n\\param sRgb   `true` to enable sRGB conversion, `false` to disable it\n\\param area   Area of the image to load\n\n\\throws sf::Exception if loading was unsuccessful\n\n\\see `loadFromFile`, `loadFromMemory`, `loadFromStream`, `loadFromImage`",
    "\\brief Construct the texture from an image\n\nThe maximum size for a texture depends on the graphics\ndriver and can be retrieved with the `getMaximumSize` function.\n\n\\param image Image to load into the texture\n\\param sRgb  `true` to enable sRGB conversion, `false` to disable it\n\n\\throws sf::Exception if loading was unsuccessful\n\n\\see `loadFromFile`, `loadFromMemory`, `loadFromStream`, `loadFromImage`",
    "\\brief Construct the texture from a sub-rectangle of an image\n\nThe `area` argument is used to load only a sub-rectangle\nof the whole image.\nIf the `area` rectangle crosses the bounds of the image, it\nis adjusted to fit the image size.\n\nThe maximum size for a texture depends on the graphics\ndriver and can be retrieved with the `getMaximumSize` function.\n\n\\param image Image to load into the texture\n\\param sRgb  `true` to enable sRGB conversion, `false` to disable it\n\\param area  Area of the image to load\n\n\\throws sf::Exception if loading was unsuccessful\n\n\\see `loadFromFile`, `loadFromMemory`, `loadFromStream`, `loadFromImage`",
    "\\brief Construct the texture with a given size\n\n\\param size Width and height of the texture\n\\param sRgb `true` to enable sRGB conversion, `false` to disable it\n\n\\throws sf::Exception if construction was unsuccessful",
    "\\brief Resize the texture\n\nIf this function fails, the texture is left unchanged.\n\n\\param size Width and height of the texture\n\\param sRgb `true` to enable sRGB conversion, `false` to disable it\n\n\\return `true` if resizing was successful, `false` if it failed",
    "\\brief Load the texture from a file on disk\n\nThe `area` argument can be used to load only a sub-rectangle\nof the whole image. If you want the entire image then leave\nthe default value (which is an empty `IntRect`).\nIf the `area` rectangle crosses the bounds of the image, it\nis adjusted to fit the image size.\n\nThe maximum size for a texture depends on the graphics\ndriver and can be retrieved with the `getMaximumSize` function.\n\nIf this function fails, the texture is left unchanged.\n\n\\param filename Path of the image file to load\n\\param sRgb     `true` to enable sRGB conversion, `false` to disable it\n\\param area     Area of the image to load\n\n\\return `true` if loading was successful, `false` if it failed\n\n\\see `loadFromMemory`, `loadFromStream`, `loadFromImage`",
    "\\brief Load the texture from a file in memory\n\nThe `area` argument can be used to load only a sub-rectangle\nof the whole image. If you want the entire image then leave\nthe default value (which is an empty `IntRect`).\nIf the `area` rectangle crosses the bounds of the image, it\nis adjusted to fit the image size.\n\nThe maximum size for a texture depends on the graphics\ndriver and can be retrieved with the `getMaximumSize` function.\n\nIf this function fails, the texture is left unchanged.\n\n\\param data Pointer to the file data in memory\n\\param size Size of the data to load, in bytes\n\\param sRgb `true` to enable sRGB conversion, `false` to disable it\n\\param area Area of the image to load\n\n\\return `true` if loading was successful, `false` if it failed\n\n\\see `loadFromFile`, `loadFromStream`, `loadFromImage`",
    "\\brief Load the texture from a custom stream\n\nThe `area` argument can be used to load only a sub-rectangle\nof the whole image. If you want the entire image then leave\nthe default value (which is an empty `IntRect`).\nIf the `area` rectangle crosses the bounds of the image, it\nis adjusted to fit the image size.\n\nThe maximum size for a texture depends on the graphics\ndriver and can be retrieved with the `getMaximumSize` function.\n\nIf this function fails, the texture is left unchanged.\n\n\\param stream Source stream to read from\n\\param sRgb   `true` to enable sRGB conversion, `false` to disable it\n\\param area   Area of the image to load\n\n\\return `true` if loading was successful, `false` if it failed\n\n\\see `loadFromFile`, `loadFromMemory`, `loadFromImage`",
    "\\brief Load the texture from an image\n\nThe `area` argument can be used to load only a sub-rectangle\nof the whole image. If you want the entire image then leave\nthe default value (which is an empty `IntRect`).\nIf the `area` rectangle crosses the bounds of the image, it\nis adjusted to fit the image size.\n\nThe maximum size for a texture depends on the graphics\ndriver and can be retrieved with the `getMaximumSize` function.\n\nIf this function fails, the texture is left unchanged.\n\n\\param image Image to load into the texture\n\\param sRgb  `true` to enable sRGB conversion, `false` to disable it\n\\param area  Area of the image to load\n\n\\return `true` if loading was successful, `false` if it failed\n\n\\see `loadFromFile`, `loadFromMemory`",
    "\\brief Return the size of the texture\n\n\\return Size in pixels",
    "\\brief Copy the texture pixels to an image\n\nThis function performs a slow operation that downloads\nthe texture's pixels from the graphics card and copies\nthem to a new image, potentially applying transformations\nto pixels if necessary (texture may be padded or flipped).\n\n\\return Image containing the texture's pixels\n\n\\see `loadFromImage`",
    "\\brief Update the whole texture from an array of pixels\n\nThe pixel array is assumed to have the same size as\nthe `area` rectangle, and to contain 32-bits RGBA pixels.\n\nNo additional check is performed on the size of the pixel\narray. Passing invalid arguments will lead to an undefined\nbehavior.\n\nThis function does nothing if `pixels` is `nullptr`\nor if the texture was not previously created.\n\n\\param pixels Array of pixels to copy to the texture",
    "\\brief Update a part of the texture from an array of pixels\n\nThe size of the pixel array must match the `size` argument,\nand it must contain 32-bits RGBA pixels.\n\nNo additional check is performed on the size of the pixel\narray or the bounds of the area to update. Passing invalid\narguments will lead to an undefined behavior.\n\nThis function does nothing if `pixels` is null or if the\ntexture was not previously created.\n\n\\param pixels Array of pixels to copy to the texture\n\\param size   Width and height of the pixel region contained in `pixels`\n\\param dest   Coordinates of the destination position",
    "\\brief Update a part of this texture from another texture\n\nAlthough the source texture can be smaller than this texture,\nthis function is usually used for updating the whole texture.\nThe other overload, which has an additional destination\nargument, is more convenient for updating a sub-area of this\ntexture.\n\nNo additional check is performed on the size of the passed\ntexture. Passing a texture bigger than this texture\nwill lead to an undefined behavior.\n\nThis function does nothing if either texture was not\npreviously created.\n\n\\param texture Source texture to copy to this texture",
    "\\brief Update a part of this texture from another texture\n\nNo additional check is performed on the size of the texture.\nPassing an invalid combination of texture size and destination\nwill lead to an undefined behavior.\n\nThis function does nothing if either texture was not\npreviously created.\n\n\\param texture Source texture to copy to this texture\n\\param dest    Coordinates of the destination position",
    "\\brief Update the texture from an image\n\nAlthough the source image can be smaller than the texture,\nthis function is usually used for updating the whole texture.\nThe other overload, which has an additional destination\nargument, is more convenient for updating a sub-area of the\ntexture.\n\nNo additional check is performed on the size of the image.\nPassing an image bigger than the texture will lead to an\nundefined behavior.\n\nThis function does nothing if the texture was not\npreviously created.\n\n\\param image Image to copy to the texture",
    "\\brief Update a part of the texture from an image\n\nNo additional check is performed on the size of the image.\nPassing an invalid combination of image size and destination\nwill lead to an undefined behavior.\n\nThis function does nothing if the texture was not\npreviously created.\n\n\\param image Image to copy to the texture\n\\param dest  Coordinates of the destination position",
    "\\brief Update the texture from the contents of a window\n\nAlthough the source window can be smaller than the texture,\nthis function is usually used for updating the whole texture.\nThe other overload, which has an additional destination\nargument, is more convenient for updating a sub-area of the\ntexture.\n\nNo additional check is performed on the size of the window.\nPassing a window bigger than the texture will lead to an\nundefined behavior.\n\nThis function does nothing if either the texture or the window\nwas not previously created.\n\n\\param window Window to copy to the texture",
    "\\brief Update a part of the texture from the contents of a window\n\nNo additional check is performed on the size of the window.\nPassing an invalid combination of window size and destination\nwill lead to an undefined behavior.\n\nThis function does nothing if either the texture or the window\nwas not previously created.\n\n\\param window Window to copy to the texture\n\\param dest   Coordinates of the destination position",
    "\\brief Enable or disable the smooth filter\n\nWhen the filter is activated, the texture appears smoother\nso that pixels are less noticeable. However if you want\nthe texture to look exactly the same as its source file,\nyou should leave it disabled.\nThe smooth filter is disabled by default.\n\n\\param smooth `true` to enable smoothing, `false` to disable it\n\n\\see `isSmooth`",
    "\\brief Tell whether the smooth filter is enabled or not\n\n\\return `true` if smoothing is enabled, `false` if it is disabled\n\n\\see `setSmooth`",
    "\\brief Tell whether the texture source is converted from sRGB or not\n\n\\return `true` if the texture source is converted from sRGB, `false` if not\n\n\\see `setSrgb`",
    "\\brief Enable or disable repeating\n\nRepeating is involved when using texture coordinates\noutside the texture rectangle [0, 0, width, height].\nIn this case, if repeat mode is enabled, the whole texture\nwill be repeated as many times as needed to reach the\ncoordinate (for example, if the X texture coordinate is\n3 * width, the texture will be repeated 3 times).\nIf repeat mode is disabled, the \"extra space\" will instead\nbe filled with border pixels.\nWarning: on very old graphics cards, white pixels may appear\nwhen the texture is repeated. With such cards, repeat mode\ncan be used reliably only if the texture has power-of-two\ndimensions (such as 256x128).\nRepeating is disabled by default.\n\n\\param repeated `true` to repeat the texture, `false` to disable repeating\n\n\\see `isRepeated`",
    "\\brief Tell whether the texture is repeated or not\n\n\\return `true` if repeat mode is enabled, `false` if it is disabled\n\n\\see `setRepeated`",
    "\\brief Generate a mipmap using the current texture data\n\nMipmaps are pre-computed chains of optimized textures. Each\nlevel of texture in a mipmap is generated by halving each of\nthe previous level's dimensions. This is done until the final\nlevel has the size of 1x1. The textures generated in this process may\nmake use of more advanced filters which might improve the visual quality\nof textures when they are applied to objects much smaller than they are.\nThis is known as minification. Because fewer texels (texture elements)\nhave to be sampled from when heavily minified, usage of mipmaps\ncan also improve rendering performance in certain scenarios.\n\nMipmap generation relies on the necessary OpenGL extension being\navailable. If it is unavailable or generation fails due to another\nreason, this function will return `false`. Mipmap data is only valid from\nthe time it is generated until the next time the base level image is\nmodified, at which point this function will have to be called again to\nregenerate it.\n\n\\return `true` if mipmap generation was successful, `false` if unsuccessful",
    "\\brief Swap the contents of this texture with those of another\n\n\\param right Instance to swap with",
    "\\brief Get the underlying OpenGL handle of the texture.\n\nYou shouldn't need to use this function, unless you have\nvery specific stuff to implement that SFML doesn't support,\nor implement a temporary workaround until a bug is fixed.\n\n\\return OpenGL handle of the texture or 0 if not yet created",
    "\\brief Bind a texture for rendering\n\nThis function is not part of the graphics API, it mustn't be\nused when drawing SFML entities. It must be used only if you\nmix `sf::Texture` with OpenGL code.\nIt only changes the `GL_TEXTURE_2D` binding. Direct OpenGL\nrendering code is responsible for converting texture coordinates\nin its shader when pixel coordinates are used.\n\n\\code\nsf::Texture t1, t2;\n...\nsf::Texture::bind(&t1);\n// draw OpenGL stuff that use t1...\nsf::Texture::bind(&t2);\n// draw OpenGL stuff that use t2...\nsf::Texture::bind(nullptr);\n// draw OpenGL stuff that use no texture...\n\\endcode\n\n\\param texture Pointer to the texture to bind, can be null to use no texture",
    "\\brief Get the maximum texture size allowed\n\nThis maximum size is defined by the graphics driver.\nYou can expect a value of 512 pixels for low-end graphics\ncard, and up to 8192 pixels or more for newer hardware.\n\n\\return Maximum size allowed for textures, in pixels",
    "\\brief Swap the contents of one texture with those of another\n\n\\param left First instance to swap\n\\param right Second instance to swap",
}; }

void bind_Texture(lua_glue::StateView lua) {
    lua_glue::Table sf = lua_sf::sf_table(lua);
    auto type_sf__Texture = lua_glue::BindClass<sf::Texture>(sf, "Texture");
    lua_glue::Table table_sf__Texture = sf["Texture"].get<lua_glue::Table>();
    lua_sf::mark_shared_usertype<sf::Texture>(lua);
    LUASF_STUB_DOC(docs[0]);
    LUASF_STUB_CLASS("sf.Texture");
    LUASF_STUB_DOC(docs[3]);
    LUASF_STUB_FUNCTION("sf.Texture", "new", "fun(filename: string, sRgb: boolean, area: sf.IntRect): sf.Texture");
    LUASF_STUB_OVERLOAD("sf.Texture", "new", "fun(stream: sf.InputStream, sRgb: boolean, area: sf.IntRect): sf.Texture");
    LUASF_STUB_OVERLOAD("sf.Texture", "new", "fun(image: sf.Image, sRgb: boolean, area: sf.IntRect): sf.Texture");
    LUASF_STUB_OVERLOAD("sf.Texture", "new", "fun(filename: string, sRgb?: boolean): sf.Texture");
    LUASF_STUB_OVERLOAD("sf.Texture", "new", "fun(stream: sf.InputStream, sRgb?: boolean): sf.Texture");
    LUASF_STUB_OVERLOAD("sf.Texture", "new", "fun(image: sf.Image, sRgb?: boolean): sf.Texture");
    LUASF_STUB_OVERLOAD("sf.Texture", "new", "fun(size: sf.Vector2u, sRgb?: boolean): sf.Texture");
    LUASF_STUB_OVERLOAD("sf.Texture", "new", "fun(): sf.Texture");
    LUASF_STUB_OVERLOAD("sf.Texture", "new", "fun(data: any, sRgb: boolean, area: sf.IntRect): sf.Texture");
    LUASF_STUB_OVERLOAD("sf.Texture", "new", "fun(data: any, sRgb?: boolean): sf.Texture");
    lua_glue::BindCallable(type_sf__Texture, "new",
        [](std::string filename, bool sRgb, const sf::IntRect& area) {
            return lua_sf::makeLuaSharedObject<sf::Texture>(std::filesystem::path(filename), sRgb, area);
        },
        docs[3]
    );
    lua_glue::BindCallable(type_sf__Texture, "new",
        [](sf::InputStream& stream, bool sRgb, const sf::IntRect& area) {
            return lua_sf::makeLuaSharedObject<sf::Texture>(stream, sRgb, area);
        },
        docs[7]
    );
    lua_glue::BindCallable(type_sf__Texture, "new",
        [](const sf::Image& image, bool sRgb, const sf::IntRect& area) {
            return lua_sf::makeLuaSharedObject<sf::Texture>(image, sRgb, area);
        },
        docs[9]
    );
    lua_glue::BindCallable(type_sf__Texture, "new",
        [](std::string filename, bool sRgb) {
            return lua_sf::makeLuaSharedObject<sf::Texture>(std::filesystem::path(filename), sRgb);
        },
        lua_glue::Defaults{lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return static_cast<bool>(false);
        }}},
        docs[2]
    );
    lua_glue::BindCallable(type_sf__Texture, "new",
        [](sf::InputStream& stream, bool sRgb) {
            return lua_sf::makeLuaSharedObject<sf::Texture>(stream, sRgb);
        },
        lua_glue::Defaults{lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return static_cast<bool>(false);
        }}},
        docs[6]
    );
    lua_glue::BindCallable(type_sf__Texture, "new",
        [](const sf::Image& image, bool sRgb) {
            return lua_sf::makeLuaSharedObject<sf::Texture>(image, sRgb);
        },
        lua_glue::Defaults{lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return static_cast<bool>(false);
        }}},
        docs[8]
    );
    lua_glue::BindCallable(type_sf__Texture, "new",
        [](sf::Vector2u size, bool sRgb) {
            return lua_sf::makeLuaSharedObject<sf::Texture>(size, sRgb);
        },
        lua_glue::Defaults{lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return static_cast<bool>(false);
        }}},
        docs[10]
    );
    lua_glue::BindCallable(type_sf__Texture, "new",
        []() {
            return lua_sf::makeLuaSharedObject<sf::Texture>();
        },
        docs[1]
    );
    lua_glue::BindCallable(type_sf__Texture, "new",
        [](lua_glue::Object data, bool sRgb, const sf::IntRect& area) {
            auto data_buffer = lua_sf::array_from_object<std::byte>(data);
            return lua_sf::makeLuaSharedObject<sf::Texture>(data_buffer.data(), static_cast<std::size_t>(data_buffer.size()), sRgb, area);
        },
        docs[5]
    );
    lua_glue::BindCallable(type_sf__Texture, "new",
        [](lua_glue::Object data, bool sRgb) {
            auto data_buffer = lua_sf::array_from_object<std::byte>(data);
            return lua_sf::makeLuaSharedObject<sf::Texture>(data_buffer.data(), static_cast<std::size_t>(data_buffer.size()), sRgb);
        },
        lua_glue::Defaults{lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return static_cast<bool>(false);
        }}},
        docs[4]
    );
    LUASF_STUB_DOC(docs[11]);
    LUASF_STUB_FUNCTION("sf.Texture", "resize", "fun(self: sf.Texture, size: sf.Vector2u, sRgb?: boolean): boolean");
    lua_glue::BindCallable(type_sf__Texture, "resize",
        [](sf::Texture& self, sf::Vector2u size, bool sRgb) -> bool {
            return self.resize(size, sRgb);
        },
        lua_glue::Defaults{lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return static_cast<bool>(false);
        }}},
        docs[11]
    );
    LUASF_STUB_DOC(docs[12]);
    LUASF_STUB_FUNCTION("sf.Texture", "loadFromFile", "fun(self: sf.Texture, filename: string, sRgb?: boolean, area?: sf.IntRect): boolean");
    lua_glue::BindCallable(type_sf__Texture, "loadFromFile",
        [](sf::Texture& self, std::string filename, bool sRgb, const sf::IntRect& area) -> bool {
            return self.loadFromFile(std::filesystem::path(filename), sRgb, area);
        },
        lua_glue::Defaults{lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return static_cast<bool>(false);
        }},
        lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return sf::Rect<int>{ };
        }}},
        docs[12]
    );
    LUASF_STUB_DOC(docs[13]);
    LUASF_STUB_FUNCTION("sf.Texture", "loadFromMemory", "fun(self: sf.Texture, data: any, sRgb?: boolean, area?: sf.IntRect): boolean");
    lua_glue::BindCallable(type_sf__Texture, "loadFromMemory",
        [](sf::Texture& self, lua_glue::Object data, bool sRgb, const sf::IntRect& area) -> bool {
            auto data_buffer = lua_sf::array_from_object<std::byte>(data);
            return self.loadFromMemory(data_buffer.data(), static_cast<std::size_t>(data_buffer.size()), sRgb, area);
        },
        lua_glue::Defaults{lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return static_cast<bool>(false);
        }},
        lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return sf::Rect<int>{ };
        }}},
        docs[13]
    );
    LUASF_STUB_DOC(docs[14]);
    LUASF_STUB_FUNCTION("sf.Texture", "loadFromStream", "fun(self: sf.Texture, stream: sf.InputStream, sRgb?: boolean, area?: sf.IntRect): boolean");
    lua_glue::BindCallable(type_sf__Texture, "loadFromStream",
        [](sf::Texture& self, sf::InputStream& stream, bool sRgb, const sf::IntRect& area) -> bool {
            return self.loadFromStream(stream, sRgb, area);
        },
        lua_glue::Defaults{lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return static_cast<bool>(false);
        }},
        lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return sf::Rect<int>{ };
        }}},
        docs[14]
    );
    LUASF_STUB_DOC(docs[15]);
    LUASF_STUB_FUNCTION("sf.Texture", "loadFromImage", "fun(self: sf.Texture, image: sf.Image, sRgb?: boolean, area?: sf.IntRect): boolean");
    lua_glue::BindCallable(type_sf__Texture, "loadFromImage",
        [](sf::Texture& self, const sf::Image& image, bool sRgb, const sf::IntRect& area) -> bool {
            return self.loadFromImage(image, sRgb, area);
        },
        lua_glue::Defaults{lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return static_cast<bool>(false);
        }},
        lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return sf::Rect<int>{ };
        }}},
        docs[15]
    );
    LUASF_STUB_DOC(docs[16]);
    LUASF_STUB_FUNCTION("sf.Texture", "getSize", "fun(self: sf.Texture): sf.Vector2u");
    lua_glue::BindCallable(type_sf__Texture, "getSize",
        [](const sf::Texture& self) -> sf::Vector2u {
            return self.getSize();
        },
        docs[16]
    );
    LUASF_STUB_DOC(docs[17]);
    LUASF_STUB_FUNCTION("sf.Texture", "copyToImage", "fun(self: sf.Texture): sf.Image");
    lua_glue::BindCallable(type_sf__Texture, "copyToImage",
        [](const sf::Texture& self) -> sf::Image {
            return self.copyToImage();
        },
        docs[17]
    );
    LUASF_STUB_DOC(docs[21]);
    LUASF_STUB_FUNCTION("sf.Texture", "update", "fun(self: sf.Texture, texture: sf.Texture, dest: sf.Vector2u)");
    LUASF_STUB_OVERLOAD("sf.Texture", "update", "fun(self: sf.Texture, image: sf.Image, dest: sf.Vector2u)");
    LUASF_STUB_OVERLOAD("sf.Texture", "update", "fun(self: sf.Texture, window: sf.Window, dest: sf.Vector2u)");
    LUASF_STUB_OVERLOAD("sf.Texture", "update", "fun(self: sf.Texture, texture: sf.Texture)");
    LUASF_STUB_OVERLOAD("sf.Texture", "update", "fun(self: sf.Texture, image: sf.Image)");
    LUASF_STUB_OVERLOAD("sf.Texture", "update", "fun(self: sf.Texture, window: sf.Window)");
    LUASF_STUB_OVERLOAD("sf.Texture", "update", "fun(self: sf.Texture, pixels: any, size: sf.Vector2u, dest: sf.Vector2u)");
    LUASF_STUB_OVERLOAD("sf.Texture", "update", "fun(self: sf.Texture, pixels: any)");
    lua_glue::BindCallable(type_sf__Texture, "update",
        [](sf::Texture& self, const sf::Texture& texture, sf::Vector2u dest) {
            self.update(texture, dest);
        },
        docs[21]
    );
    lua_glue::BindCallable(type_sf__Texture, "update",
        [](sf::Texture& self, const sf::Image& image, sf::Vector2u dest) {
            self.update(image, dest);
        },
        docs[23]
    );
    lua_glue::BindCallable(type_sf__Texture, "update",
        [](sf::Texture& self, const sf::Window& window, sf::Vector2u dest) {
            self.update(window, dest);
        },
        docs[25]
    );
    lua_glue::BindCallable(type_sf__Texture, "update",
        [](sf::Texture& self, const sf::Texture& texture) {
            self.update(texture);
        },
        docs[20]
    );
    lua_glue::BindCallable(type_sf__Texture, "update",
        [](sf::Texture& self, const sf::Image& image) {
            self.update(image);
        },
        docs[22]
    );
    lua_glue::BindCallable(type_sf__Texture, "update",
        [](sf::Texture& self, const sf::Window& window) {
            self.update(window);
        },
        docs[24]
    );
    lua_glue::BindCallable(type_sf__Texture, "update",
        [](sf::Texture& self, lua_glue::Object pixels, sf::Vector2u size, sf::Vector2u dest) {
            auto pixels_buffer = lua_sf::array_from_object<std::uint8_t>(pixels);
            self.update(pixels_buffer.data(), size, dest);
        },
        docs[19]
    );
    lua_glue::BindCallable(type_sf__Texture, "update",
        [](sf::Texture& self, lua_glue::Object pixels) {
            auto pixels_buffer = lua_sf::array_from_object<std::uint8_t>(pixels);
            self.update(pixels_buffer.data());
        },
        docs[18]
    );
    LUASF_STUB_DOC(docs[26]);
    LUASF_STUB_FUNCTION("sf.Texture", "setSmooth", "fun(self: sf.Texture, smooth: boolean)");
    lua_glue::BindCallable(type_sf__Texture, "setSmooth",
        [](sf::Texture& self, bool smooth) {
            self.setSmooth(smooth);
        },
        docs[26]
    );
    LUASF_STUB_DOC(docs[27]);
    LUASF_STUB_FUNCTION("sf.Texture", "isSmooth", "fun(self: sf.Texture): boolean");
    lua_glue::BindCallable(type_sf__Texture, "isSmooth",
        [](const sf::Texture& self) -> bool {
            return self.isSmooth();
        },
        docs[27]
    );
    LUASF_STUB_DOC(docs[28]);
    LUASF_STUB_FUNCTION("sf.Texture", "isSrgb", "fun(self: sf.Texture): boolean");
    lua_glue::BindCallable(type_sf__Texture, "isSrgb",
        [](const sf::Texture& self) -> bool {
            return self.isSrgb();
        },
        docs[28]
    );
    LUASF_STUB_DOC(docs[29]);
    LUASF_STUB_FUNCTION("sf.Texture", "setRepeated", "fun(self: sf.Texture, repeated: boolean)");
    lua_glue::BindCallable(type_sf__Texture, "setRepeated",
        [](sf::Texture& self, bool repeated) {
            self.setRepeated(repeated);
        },
        docs[29]
    );
    LUASF_STUB_DOC(docs[30]);
    LUASF_STUB_FUNCTION("sf.Texture", "isRepeated", "fun(self: sf.Texture): boolean");
    lua_glue::BindCallable(type_sf__Texture, "isRepeated",
        [](const sf::Texture& self) -> bool {
            return self.isRepeated();
        },
        docs[30]
    );
    LUASF_STUB_DOC(docs[31]);
    LUASF_STUB_FUNCTION("sf.Texture", "generateMipmap", "fun(self: sf.Texture): boolean");
    lua_glue::BindCallable(type_sf__Texture, "generateMipmap",
        [](sf::Texture& self) -> bool {
            return self.generateMipmap();
        },
        docs[31]
    );
    LUASF_STUB_DOC(docs[32]);
    LUASF_STUB_FUNCTION("sf.Texture", "swap", "fun(self: sf.Texture, right: sf.Texture)");
    lua_glue::BindCallable(type_sf__Texture, "swap",
        [](sf::Texture& self, sf::Texture& right) {
            self.swap(right);
        },
        docs[32]
    );
    LUASF_STUB_DOC(docs[33]);
    LUASF_STUB_FUNCTION("sf.Texture", "getNativeHandle", "fun(self: sf.Texture): integer");
    lua_glue::BindCallable(type_sf__Texture, "getNativeHandle",
        [](const sf::Texture& self) -> unsigned int {
            return self.getNativeHandle();
        },
        docs[33]
    );
    LUASF_STUB_DOC(docs[34]);
    LUASF_STUB_FUNCTION("sf.Texture", "bind", "fun(texture: sf.Texture)");
    lua_glue::BindCallable(type_sf__Texture, "bind",
        [](const sf::Texture* texture) {
            sf::Texture::bind(texture);
        },
        docs[34]
    );
    LUASF_STUB_DOC(docs[35]);
    LUASF_STUB_FUNCTION("sf.Texture", "getMaximumSize", "fun(): integer");
    lua_glue::BindCallable(type_sf__Texture, "getMaximumSize",
        []() -> unsigned int {
            return sf::Texture::getMaximumSize();
        },
        docs[35]
    );
    LUASF_STUB_DOC(docs[36]);
    LUASF_STUB_FUNCTION("sf", "swap", "fun(left: sf.Texture, right: sf.Texture)");
    lua_glue::BindCallable(sf, "swap",
        [](sf::Texture& left, sf::Texture& right) {
            sf::swap(left, right);
        },
        docs[36]
    );
}
