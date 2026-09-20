#include "Graphics/bind_Shader.hpp"

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

namespace { constexpr std::array<std::string_view, 43> docs = {
    "\\brief Shader class (vertex, geometry and fragment)",
    "\\brief Default constructor\n\nThis constructor creates an empty shader.\n\nBinding an empty shader has the same effect as not\nbinding any shader.",
    "\\brief Construct from a shader file\n\nThis constructor loads a single vertex or fragment shader and\ncompletes it with SFML's matching baseline stage. A geometry\nshader must be loaded with both a vertex and fragment shader.\nThe source must be a text file containing a valid\nshader in GLSL language. GLSL is a C-like language\ndedicated to OpenGL shaders; you'll probably need to\nread a good documentation for it before writing your\nown shaders.\n\n\\param filename Path of the vertex, geometry or fragment shader file to load\n\\param type     Type of shader (vertex, geometry or fragment)\n\n\\throws sf::Exception if loading was unsuccessful\n\n\\see `loadFromFile`, `loadFromMemory`, `loadFromStream`",
    "\\brief Construct from vertex and fragment shader files\n\nThis constructor loads both the vertex and the fragment\nshaders. If one of them fails to load, the shader is left\nempty (the valid shader is unloaded).\nThe sources must be text files containing valid shaders\nin GLSL language. GLSL is a C-like language dedicated to\nOpenGL shaders; you'll probably need to read a good documentation\nfor it before writing your own shaders.\n\n\\param vertexShaderFilename   Path of the vertex shader file to load\n\\param fragmentShaderFilename Path of the fragment shader file to load\n\n\\throws sf::Exception if loading was unsuccessful\n\n\\see `loadFromFile`, `loadFromMemory`, `loadFromStream`",
    "\\brief Construct from vertex, geometry and fragment shader files\n\nThis constructor loads the vertex, geometry and fragment\nshaders. If one of them fails to load, the shader is left\nempty (the valid shader is unloaded).\nThe sources must be text files containing valid shaders\nin GLSL language. GLSL is a C-like language dedicated to\nOpenGL shaders; you'll probably need to read a good documentation\nfor it before writing your own shaders.\n\n\\param vertexShaderFilename   Path of the vertex shader file to load\n\\param geometryShaderFilename Path of the geometry shader file to load\n\\param fragmentShaderFilename Path of the fragment shader file to load\n\n\\throws sf::Exception if loading was unsuccessful\n\n\\see `loadFromFile`, `loadFromMemory`, `loadFromStream`",
    "\\brief Construct from a shader stream\n\nThis constructor loads a single vertex or fragment shader and\ncompletes it with SFML's matching baseline stage. A geometry\nshader must be loaded with both a vertex and fragment shader.\nThe source code must be a valid shader in GLSL language.\nGLSL is a C-like language dedicated to OpenGL shaders;\nyou'll probably need to read a good documentation for it\nbefore writing your own shaders.\n\n\\param stream Source stream to read from\n\\param type   Type of shader (vertex, geometry or fragment)\n\n\\throws sf::Exception if loading was unsuccessful\n\n\\see `loadFromFile`, `loadFromMemory`, `loadFromStream`",
    "\\brief Construct from vertex and fragment shader streams\n\nThis constructor loads both the vertex and the fragment\nshaders. If one of them fails to load, the shader is left\nempty (the valid shader is unloaded).\nThe source codes must be valid shaders in GLSL language.\nGLSL is a C-like language dedicated to OpenGL shaders;\nyou'll probably need to read a good documentation for\nit before writing your own shaders.\n\n\\param vertexShaderStream   Source stream to read the vertex shader from\n\\param fragmentShaderStream Source stream to read the fragment shader from\n\n\\throws sf::Exception if loading was unsuccessful\n\n\\see `loadFromFile`, `loadFromMemory`, `loadFromStream`",
    "\\brief Construct from vertex, geometry and fragment shader streams\n\nThis constructor loads the vertex, geometry and fragment\nshaders. If one of them fails to load, the shader is left\nempty (the valid shader is unloaded).\nThe source codes must be valid shaders in GLSL language.\nGLSL is a C-like language dedicated to OpenGL shaders;\nyou'll probably need to read a good documentation for\nit before writing your own shaders.\n\n\\param vertexShaderStream   Source stream to read the vertex shader from\n\\param geometryShaderStream Source stream to read the geometry shader from\n\\param fragmentShaderStream Source stream to read the fragment shader from\n\n\\throws sf::Exception if loading was unsuccessful\n\n\\see `loadFromFile`, `loadFromMemory`, `loadFromStream`",
    "\\brief Load the vertex, geometry or fragment shader from a file\n\nThis function loads a single vertex or fragment shader and\ncompletes it with SFML's matching baseline stage. A geometry\nshader must be loaded with both a vertex and fragment shader.\nThe source must be a text file containing a valid\nshader in GLSL language. GLSL is a C-like language\ndedicated to OpenGL shaders; you'll probably need to\nread a good documentation for it before writing your\nown shaders.\n\n\\param filename Path of the vertex, geometry or fragment shader file to load\n\\param type     Type of shader (vertex, geometry or fragment)\n\n\\return `true` if loading succeeded, `false` if it failed\n\n\\see `loadFromMemory`, `loadFromStream`",
    "\\brief Load both the vertex and fragment shaders from files\n\nThis function loads both the vertex and the fragment\nshaders. If one of them fails to load, the shader is left\nempty (the valid shader is unloaded).\nThe sources must be text files containing valid shaders\nin GLSL language. GLSL is a C-like language dedicated to\nOpenGL shaders; you'll probably need to read a good documentation\nfor it before writing your own shaders.\n\n\\param vertexShaderFilename   Path of the vertex shader file to load\n\\param fragmentShaderFilename Path of the fragment shader file to load\n\n\\return `true` if loading succeeded, `false` if it failed\n\n\\see `loadFromMemory`, `loadFromStream`",
    "\\brief Load the vertex, geometry and fragment shaders from files\n\nThis function loads the vertex, geometry and fragment\nshaders. If one of them fails to load, the shader is left\nempty (the valid shader is unloaded).\nThe sources must be text files containing valid shaders\nin GLSL language. GLSL is a C-like language dedicated to\nOpenGL shaders; you'll probably need to read a good documentation\nfor it before writing your own shaders.\n\n\\param vertexShaderFilename   Path of the vertex shader file to load\n\\param geometryShaderFilename Path of the geometry shader file to load\n\\param fragmentShaderFilename Path of the fragment shader file to load\n\n\\return `true` if loading succeeded, `false` if it failed\n\n\\see `loadFromMemory`, `loadFromStream`",
    "\\brief Load the vertex, geometry or fragment shader from a source code in memory\n\nThis function loads a single vertex or fragment shader and\ncompletes it with SFML's matching baseline stage. A geometry\nshader must be loaded with both a vertex and fragment shader.\nThe source code must be a valid shader in GLSL language.\nGLSL is a C-like language dedicated to OpenGL shaders;\nyou'll probably need to read a good documentation for\nit before writing your own shaders.\n\n\\param shader String containing the source code of the shader\n\\param type   Type of shader (vertex, geometry or fragment)\n\n\\return `true` if loading succeeded, `false` if it failed\n\n\\see `loadFromFile`, `loadFromStream`",
    "\\brief Load both the vertex and fragment shaders from source codes in memory\n\nThis function loads both the vertex and the fragment\nshaders. If one of them fails to load, the shader is left\nempty (the valid shader is unloaded).\nThe sources must be valid shaders in GLSL language. GLSL is\na C-like language dedicated to OpenGL shaders; you'll\nprobably need to read a good documentation for it before\nwriting your own shaders.\n\n\\param vertexShader   String containing the source code of the vertex shader\n\\param fragmentShader String containing the source code of the fragment shader\n\n\\return `true` if loading succeeded, `false` if it failed\n\n\\see `loadFromFile`, `loadFromStream`",
    "\\brief Load the vertex, geometry and fragment shaders from source codes in memory\n\nThis function loads the vertex, geometry and fragment\nshaders. If one of them fails to load, the shader is left\nempty (the valid shader is unloaded).\nThe sources must be valid shaders in GLSL language. GLSL is\na C-like language dedicated to OpenGL shaders; you'll\nprobably need to read a good documentation for it before\nwriting your own shaders.\n\n\\param vertexShader   String containing the source code of the vertex shader\n\\param geometryShader String containing the source code of the geometry shader\n\\param fragmentShader String containing the source code of the fragment shader\n\n\\return `true` if loading succeeded, `false` if it failed\n\n\\see `loadFromFile`, `loadFromStream`",
    "\\brief Load the vertex, geometry or fragment shader from a custom stream\n\nThis function loads a single vertex or fragment shader and\ncompletes it with SFML's matching baseline stage. A geometry\nshader must be loaded with both a vertex and fragment shader.\nThe source code must be a valid shader in GLSL language.\nGLSL is a C-like language dedicated to OpenGL shaders;\nyou'll probably need to read a good documentation for it\nbefore writing your own shaders.\n\n\\param stream Source stream to read from\n\\param type   Type of shader (vertex, geometry or fragment)\n\n\\return `true` if loading succeeded, `false` if it failed\n\n\\see `loadFromFile`, `loadFromMemory`",
    "\\brief Load both the vertex and fragment shaders from custom streams\n\nThis function loads both the vertex and the fragment\nshaders. If one of them fails to load, the shader is left\nempty (the valid shader is unloaded).\nThe source codes must be valid shaders in GLSL language.\nGLSL is a C-like language dedicated to OpenGL shaders;\nyou'll probably need to read a good documentation for\nit before writing your own shaders.\n\n\\param vertexShaderStream   Source stream to read the vertex shader from\n\\param fragmentShaderStream Source stream to read the fragment shader from\n\n\\return `true` if loading succeeded, `false` if it failed\n\n\\see `loadFromFile`, `loadFromMemory`",
    "\\brief Load the vertex, geometry and fragment shaders from custom streams\n\nThis function loads the vertex, geometry and fragment\nshaders. If one of them fails to load, the shader is left\nempty (the valid shader is unloaded).\nThe source codes must be valid shaders in GLSL language.\nGLSL is a C-like language dedicated to OpenGL shaders;\nyou'll probably need to read a good documentation for\nit before writing your own shaders.\n\n\\param vertexShaderStream   Source stream to read the vertex shader from\n\\param geometryShaderStream Source stream to read the geometry shader from\n\\param fragmentShaderStream Source stream to read the fragment shader from\n\n\\return `true` if loading succeeded, `false` if it failed\n\n\\see `loadFromFile`, `loadFromMemory`",
    "\\brief Specify value for \\p float uniform\n\n\\param name Name of the uniform variable in GLSL\n\\param x    Value of the float scalar",
    "\\brief Specify value for \\p vec2 uniform\n\n\\param name   Name of the uniform variable in GLSL\n\\param vector Value of the vec2 vector",
    "\\brief Specify value for \\p vec3 uniform\n\n\\param name   Name of the uniform variable in GLSL\n\\param vector Value of the vec3 vector",
    "\\brief Specify value for \\p vec4 uniform\n\nThis overload can also be called with `sf::Color` objects\nthat are converted to `sf::Glsl::Vec4`.\n\nIt is important to note that the components of the color are\nnormalized before being passed to the shader. Therefore,\nthey are converted from range [0 .. 255] to range [0 .. 1].\nFor example, a `sf::Color(255, 127, 0, 255)` will be transformed\nto a `vec4(1.0, 0.5, 0.0, 1.0)` in the shader.\n\n\\param name   Name of the uniform variable in GLSL\n\\param vector Value of the vec4 vector",
    "\\brief Specify value for \\p int uniform\n\n\\param name Name of the uniform variable in GLSL\n\\param x    Value of the int scalar",
    "\\brief Specify value for \\p ivec2 uniform\n\n\\param name   Name of the uniform variable in GLSL\n\\param vector Value of the ivec2 vector",
    "\\brief Specify value for \\p ivec3 uniform\n\n\\param name   Name of the uniform variable in GLSL\n\\param vector Value of the ivec3 vector",
    "\\brief Specify value for \\p ivec4 uniform\n\nThis overload can also be called with `sf::Color` objects\nthat are converted to `sf::Glsl::Ivec4`.\n\nIf color conversions are used, the ivec4 uniform in GLSL\nwill hold the same values as the original `sf::Color`\ninstance. For example, `sf::Color(255, 127, 0, 255)` is\nmapped to `ivec4(255, 127, 0, 255)`.\n\n\\param name   Name of the uniform variable in GLSL\n\\param vector Value of the ivec4 vector",
    "\\brief Specify value for \\p bool uniform\n\n\\param name Name of the uniform variable in GLSL\n\\param x    Value of the bool scalar",
    "\\brief Specify value for \\p bvec2 uniform\n\n\\param name   Name of the uniform variable in GLSL\n\\param vector Value of the bvec2 vector",
    "\\brief Specify value for \\p bvec3 uniform\n\n\\param name   Name of the uniform variable in GLSL\n\\param vector Value of the bvec3 vector",
    "\\brief Specify value for \\p bvec4 uniform\n\n\\param name   Name of the uniform variable in GLSL\n\\param vector Value of the bvec4 vector",
    "\\brief Specify value for \\p mat3 matrix\n\n\\param name   Name of the uniform variable in GLSL\n\\param matrix Value of the mat3 matrix",
    "\\brief Specify value for \\p mat4 matrix\n\n\\param name   Name of the uniform variable in GLSL\n\\param matrix Value of the mat4 matrix",
    "\\brief Specify a texture as \\p sampler2D uniform\n\n\\a name is the name of the variable to change in the shader.\nThe corresponding parameter in the shader must be a 2D texture\n(\\p sampler2D GLSL type).\n\nExample:\n\\code\nuniform sampler2D the_texture; // this is the variable in the shader\n\\endcode\n\\code\nsf::Texture texture;\n...\nshader.setUniform(\"the_texture\", texture);\n\\endcode\nIt is important to note that `texture` must remain alive as long\nas the shader uses it, no copy is made internally.\n\nTo use the texture of the object being drawn, which cannot be\nknown in advance, you can pass the special value\n`sf::Shader::CurrentTexture`:\n\\code\nshader.setUniform(\"the_texture\", sf::Shader::CurrentTexture).\n\\endcode\n\n\\param name    Name of the texture in the shader\n\\param texture Texture to assign",
    "\\brief Specify current texture as \\p sampler2D uniform\n\nThis overload maps a shader texture variable to the\ntexture of the object being drawn, which cannot be\nknown in advance. The second argument must be\n`sf::Shader::CurrentTexture`.\nThe corresponding parameter in the shader must be a 2D texture\n(\\p sampler2D GLSL type).\n\nExample:\n\\code\nuniform sampler2D current; // this is the variable in the shader\n\\endcode\n\\code\nshader.setUniform(\"current\", sf::Shader::CurrentTexture);\n\\endcode\n\n\\param name Name of the texture in the shader",
    "\\brief Get the underlying OpenGL handle of the shader.\n\nYou shouldn't need to use this function, unless you have\nvery specific stuff to implement that SFML doesn't support,\nor implement a temporary workaround until a bug is fixed.\n\n\\return OpenGL handle of the shader or 0 if not yet loaded",
    "\\brief Bind a shader for rendering\n\nThis function is not part of the graphics API, it mustn't be\nused when drawing SFML entities. It must be used only if you\nmix `sf::Shader` with OpenGL code.\n\n\\code\nsf::Shader s1, s2;\n...\nsf::Shader::bind(&s1);\n// draw OpenGL stuff that use s1...\nsf::Shader::bind(&s2);\n// draw OpenGL stuff that use s2...\nsf::Shader::bind(nullptr);\n// draw OpenGL stuff that use no shader...\n\\endcode\n\n\\param shader Shader to bind, can be null to use no shader",
    "\\brief Tell whether or not the system supports shaders\n\nThis function should always be called before using\nthe shader features. If it returns `false`, then\nany attempt to use `sf::Shader` will fail.\n\n\\return `true` if shaders are supported, `false` otherwise",
    "\\brief Tell whether or not the system supports geometry shaders\n\nThis function should always be called before using\nthe geometry shader features. If it returns `false`, then\nany attempt to use `sf::Shader` geometry shader features will fail.\n\nThis function can only return `true` if isAvailable() would also\nreturn `true`, since shaders in general have to be supported in\norder for geometry shaders to be supported as well.\n\nNote: The first call to this function, whether by your\ncode or SFML will result in a context switch.\n\n\\return `true` if geometry shaders are supported, `false` otherwise",
    "\\brief Types of shaders",
    "%Vertex shader",
    "Geometry shader",
    "Fragment (pixel) shader",
    "\\brief Special type that can be passed to setUniform(),\nand that represents the texture of the object being drawn\n\n\\see `setUniform(const std::string&, CurrentTextureType)`",
    "\\brief Represents the texture of the object being drawn\n\n\\see `setUniform(const std::string&, CurrentTextureType)`",
}; }

void bind_Shader(lua_glue::StateView lua) {
    lua_glue::Table sf = lua_sf::sf_table(lua);
    auto type_sf__Shader = lua_glue::BindClass<sf::Shader>(sf, "Shader");
    lua_glue::Table table_sf__Shader = sf["Shader"].get<lua_glue::Table>();
    lua_sf::mark_shared_usertype<sf::Shader>(lua);
    LUASF_STUB_DOC(docs[0]);
    LUASF_STUB_CLASS("sf.Shader");
    LUASF_STUB_DOC(docs[4]);
    LUASF_STUB_FUNCTION("sf.Shader", "new", "fun(vertexShaderFilename: string, geometryShaderFilename: string, fragmentShaderFilename: string): sf.Shader");
    LUASF_STUB_OVERLOAD("sf.Shader", "new", "fun(vertexShaderStream: sf.InputStream, geometryShaderStream: sf.InputStream, fragmentShaderStream: sf.InputStream): sf.Shader");
    LUASF_STUB_OVERLOAD("sf.Shader", "new", "fun(filename: string, type: sf.Shader.Type): sf.Shader");
    LUASF_STUB_OVERLOAD("sf.Shader", "new", "fun(vertexShaderFilename: string, fragmentShaderFilename: string): sf.Shader");
    LUASF_STUB_OVERLOAD("sf.Shader", "new", "fun(stream: sf.InputStream, type: sf.Shader.Type): sf.Shader");
    LUASF_STUB_OVERLOAD("sf.Shader", "new", "fun(vertexShaderStream: sf.InputStream, fragmentShaderStream: sf.InputStream): sf.Shader");
    LUASF_STUB_OVERLOAD("sf.Shader", "new", "fun(): sf.Shader");
    lua_glue::BindCallable(type_sf__Shader, "new",
        [](std::string vertexShaderFilename, std::string geometryShaderFilename, std::string fragmentShaderFilename) {
            return lua_sf::makeLuaSharedObject<sf::Shader>(std::filesystem::path(vertexShaderFilename), std::filesystem::path(geometryShaderFilename), std::filesystem::path(fragmentShaderFilename));
        },
        docs[4]
    );
    lua_glue::BindCallable(type_sf__Shader, "new",
        [](sf::InputStream& vertexShaderStream, sf::InputStream& geometryShaderStream, sf::InputStream& fragmentShaderStream) {
            return lua_sf::makeLuaSharedObject<sf::Shader>(vertexShaderStream, geometryShaderStream, fragmentShaderStream);
        },
        docs[7]
    );
    lua_glue::BindCallable(type_sf__Shader, "new",
        [](std::string filename, sf::Shader::Type type) {
            return lua_sf::makeLuaSharedObject<sf::Shader>(std::filesystem::path(filename), type);
        },
        docs[2]
    );
    lua_glue::BindCallable(type_sf__Shader, "new",
        [](std::string vertexShaderFilename, std::string fragmentShaderFilename) {
            return lua_sf::makeLuaSharedObject<sf::Shader>(std::filesystem::path(vertexShaderFilename), std::filesystem::path(fragmentShaderFilename));
        },
        docs[3]
    );
    lua_glue::BindCallable(type_sf__Shader, "new",
        [](sf::InputStream& stream, sf::Shader::Type type) {
            return lua_sf::makeLuaSharedObject<sf::Shader>(stream, type);
        },
        docs[5]
    );
    lua_glue::BindCallable(type_sf__Shader, "new",
        [](sf::InputStream& vertexShaderStream, sf::InputStream& fragmentShaderStream) {
            return lua_sf::makeLuaSharedObject<sf::Shader>(vertexShaderStream, fragmentShaderStream);
        },
        docs[6]
    );
    lua_glue::BindCallable(type_sf__Shader, "new",
        []() {
            return lua_sf::makeLuaSharedObject<sf::Shader>();
        },
        docs[1]
    );
    LUASF_STUB_DOC(docs[10]);
    LUASF_STUB_FUNCTION("sf.Shader", "loadFromFile", "fun(self: sf.Shader, vertexShaderFilename: string, geometryShaderFilename: string, fragmentShaderFilename: string): boolean");
    LUASF_STUB_OVERLOAD("sf.Shader", "loadFromFile", "fun(self: sf.Shader, filename: string, type: sf.Shader.Type): boolean");
    LUASF_STUB_OVERLOAD("sf.Shader", "loadFromFile", "fun(self: sf.Shader, vertexShaderFilename: string, fragmentShaderFilename: string): boolean");
    lua_glue::BindCallable(type_sf__Shader, "loadFromFile",
        [](sf::Shader& self, std::string vertexShaderFilename, std::string geometryShaderFilename, std::string fragmentShaderFilename) -> bool {
            return self.loadFromFile(std::filesystem::path(vertexShaderFilename), std::filesystem::path(geometryShaderFilename), std::filesystem::path(fragmentShaderFilename));
        },
        docs[10]
    );
    lua_glue::BindCallable(type_sf__Shader, "loadFromFile",
        [](sf::Shader& self, std::string filename, sf::Shader::Type type) -> bool {
            return self.loadFromFile(std::filesystem::path(filename), type);
        },
        docs[8]
    );
    lua_glue::BindCallable(type_sf__Shader, "loadFromFile",
        [](sf::Shader& self, std::string vertexShaderFilename, std::string fragmentShaderFilename) -> bool {
            return self.loadFromFile(std::filesystem::path(vertexShaderFilename), std::filesystem::path(fragmentShaderFilename));
        },
        docs[9]
    );
    LUASF_STUB_DOC(docs[13]);
    LUASF_STUB_FUNCTION("sf.Shader", "loadFromMemory", "fun(self: sf.Shader, vertexShader: string, geometryShader: string, fragmentShader: string): boolean");
    LUASF_STUB_OVERLOAD("sf.Shader", "loadFromMemory", "fun(self: sf.Shader, shader: string, type: sf.Shader.Type): boolean");
    LUASF_STUB_OVERLOAD("sf.Shader", "loadFromMemory", "fun(self: sf.Shader, vertexShader: string, fragmentShader: string): boolean");
    lua_glue::BindCallable(type_sf__Shader, "loadFromMemory",
        [](sf::Shader& self, std::string vertexShader, std::string geometryShader, std::string fragmentShader) -> bool {
            return self.loadFromMemory(vertexShader, geometryShader, fragmentShader);
        },
        docs[13]
    );
    lua_glue::BindCallable(type_sf__Shader, "loadFromMemory",
        [](sf::Shader& self, std::string shader, sf::Shader::Type type) -> bool {
            return self.loadFromMemory(shader, type);
        },
        docs[11]
    );
    lua_glue::BindCallable(type_sf__Shader, "loadFromMemory",
        [](sf::Shader& self, std::string vertexShader, std::string fragmentShader) -> bool {
            return self.loadFromMemory(vertexShader, fragmentShader);
        },
        docs[12]
    );
    LUASF_STUB_DOC(docs[16]);
    LUASF_STUB_FUNCTION("sf.Shader", "loadFromStream", "fun(self: sf.Shader, vertexShaderStream: sf.InputStream, geometryShaderStream: sf.InputStream, fragmentShaderStream: sf.InputStream): boolean");
    LUASF_STUB_OVERLOAD("sf.Shader", "loadFromStream", "fun(self: sf.Shader, stream: sf.InputStream, type: sf.Shader.Type): boolean");
    LUASF_STUB_OVERLOAD("sf.Shader", "loadFromStream", "fun(self: sf.Shader, vertexShaderStream: sf.InputStream, fragmentShaderStream: sf.InputStream): boolean");
    lua_glue::BindCallable(type_sf__Shader, "loadFromStream",
        [](sf::Shader& self, sf::InputStream& vertexShaderStream, sf::InputStream& geometryShaderStream, sf::InputStream& fragmentShaderStream) -> bool {
            return self.loadFromStream(vertexShaderStream, geometryShaderStream, fragmentShaderStream);
        },
        docs[16]
    );
    lua_glue::BindCallable(type_sf__Shader, "loadFromStream",
        [](sf::Shader& self, sf::InputStream& stream, sf::Shader::Type type) -> bool {
            return self.loadFromStream(stream, type);
        },
        docs[14]
    );
    lua_glue::BindCallable(type_sf__Shader, "loadFromStream",
        [](sf::Shader& self, sf::InputStream& vertexShaderStream, sf::InputStream& fragmentShaderStream) -> bool {
            return self.loadFromStream(vertexShaderStream, fragmentShaderStream);
        },
        docs[15]
    );
    LUASF_STUB_DOC(docs[17]);
    LUASF_STUB_FUNCTION("sf.Shader", "setUniform", "fun(self: sf.Shader, name: string, x: number)");
    LUASF_STUB_OVERLOAD("sf.Shader", "setUniform", "fun(self: sf.Shader, name: string, vector: sf.Vector2f)");
    LUASF_STUB_OVERLOAD("sf.Shader", "setUniform", "fun(self: sf.Shader, name: string, vector: sf.Vector3f)");
    LUASF_STUB_OVERLOAD("sf.Shader", "setUniform", "fun(self: sf.Shader, name: string, vector: sf.Vector4f)");
    LUASF_STUB_OVERLOAD("sf.Shader", "setUniform", "fun(self: sf.Shader, name: string, x: integer)");
    LUASF_STUB_OVERLOAD("sf.Shader", "setUniform", "fun(self: sf.Shader, name: string, vector: sf.Vector2i)");
    LUASF_STUB_OVERLOAD("sf.Shader", "setUniform", "fun(self: sf.Shader, name: string, vector: sf.Vector3i)");
    LUASF_STUB_OVERLOAD("sf.Shader", "setUniform", "fun(self: sf.Shader, name: string, vector: sf.Vector4i)");
    LUASF_STUB_OVERLOAD("sf.Shader", "setUniform", "fun(self: sf.Shader, name: string, x: boolean)");
    LUASF_STUB_OVERLOAD("sf.Shader", "setUniform", "fun(self: sf.Shader, name: string, vector: sf.Vector2b)");
    LUASF_STUB_OVERLOAD("sf.Shader", "setUniform", "fun(self: sf.Shader, name: string, vector: sf.Vector3b)");
    LUASF_STUB_OVERLOAD("sf.Shader", "setUniform", "fun(self: sf.Shader, name: string, vector: sf.Vector4b)");
    LUASF_STUB_OVERLOAD("sf.Shader", "setUniform", "fun(self: sf.Shader, name: string, matrix: sf.Mat3)");
    LUASF_STUB_OVERLOAD("sf.Shader", "setUniform", "fun(self: sf.Shader, name: string, matrix: sf.Mat4)");
    LUASF_STUB_OVERLOAD("sf.Shader", "setUniform", "fun(self: sf.Shader, name: string, texture: sf.Texture)");
    LUASF_STUB_OVERLOAD("sf.Shader", "setUniform", "fun(self: sf.Shader, name: string, arg1: sf.Shader.CurrentTextureType)");
    lua_glue::BindCallable(type_sf__Shader, "setUniform",
        [](sf::Shader& self, std::string name, float x) {
            self.setUniform(name, x);
        },
        docs[17]
    );
    lua_glue::BindCallable(type_sf__Shader, "setUniform",
        [](sf::Shader& self, std::string name, sf::Glsl::Vec2 vector) {
            self.setUniform(name, vector);
        },
        docs[18]
    );
    lua_glue::BindCallable(type_sf__Shader, "setUniform",
        [](sf::Shader& self, std::string name, const sf::Glsl::Vec3& vector) {
            self.setUniform(name, vector);
        },
        docs[19]
    );
    lua_glue::BindCallable(type_sf__Shader, "setUniform",
        [](sf::Shader& self, std::string name, const sf::Glsl::Vec4& vector) {
            self.setUniform(name, vector);
        },
        docs[20]
    );
    lua_glue::BindCallable(type_sf__Shader, "setUniform",
        [](sf::Shader& self, std::string name, lua_sf::LuaIntegral<int> x) {
            self.setUniform(name, x.value());
        },
        docs[21]
    );
    lua_glue::BindCallable(type_sf__Shader, "setUniform",
        [](sf::Shader& self, std::string name, sf::Glsl::Ivec2 vector) {
            self.setUniform(name, vector);
        },
        docs[22]
    );
    lua_glue::BindCallable(type_sf__Shader, "setUniform",
        [](sf::Shader& self, std::string name, const sf::Glsl::Ivec3& vector) {
            self.setUniform(name, vector);
        },
        docs[23]
    );
    lua_glue::BindCallable(type_sf__Shader, "setUniform",
        [](sf::Shader& self, std::string name, const sf::Glsl::Ivec4& vector) {
            self.setUniform(name, vector);
        },
        docs[24]
    );
    lua_glue::BindCallable(type_sf__Shader, "setUniform",
        [](sf::Shader& self, std::string name, bool x) {
            self.setUniform(name, x);
        },
        docs[25]
    );
    lua_glue::BindCallable(type_sf__Shader, "setUniform",
        [](sf::Shader& self, std::string name, sf::Glsl::Bvec2 vector) {
            self.setUniform(name, vector);
        },
        docs[26]
    );
    lua_glue::BindCallable(type_sf__Shader, "setUniform",
        [](sf::Shader& self, std::string name, const sf::Glsl::Bvec3& vector) {
            self.setUniform(name, vector);
        },
        docs[27]
    );
    lua_glue::BindCallable(type_sf__Shader, "setUniform",
        [](sf::Shader& self, std::string name, const sf::Glsl::Bvec4& vector) {
            self.setUniform(name, vector);
        },
        docs[28]
    );
    lua_glue::BindCallable(type_sf__Shader, "setUniform",
        [](sf::Shader& self, std::string name, const sf::Glsl::Mat3& matrix) {
            self.setUniform(name, matrix);
        },
        docs[29]
    );
    lua_glue::BindCallable(type_sf__Shader, "setUniform",
        [](sf::Shader& self, std::string name, const sf::Glsl::Mat4& matrix) {
            self.setUniform(name, matrix);
        },
        docs[30]
    );
    lua_glue::BindCallable(type_sf__Shader, "setUniform",
        [](sf::Shader& self, std::string name, const sf::Texture& texture) {
            self.setUniform(name, texture);
        },
        docs[31]
    );
    lua_glue::BindCallable(type_sf__Shader, "setUniform",
        [](sf::Shader& self, std::string name, sf::Shader::CurrentTextureType arg1) {
            self.setUniform(name, arg1);
        },
        docs[32]
    );
    lua_sf::detail::bindShaderUniformArrays(
        type_sf__Shader,
        "sf.Shader",
        "setUniformArray",
        lua_sf::detail::shaderUniformArrayVariant<float>("setUniformFloatArray", "number[]"),
        lua_sf::detail::shaderUniformArrayVariant<sf::Glsl::Vec2>("setUniformVec2Array", "sf.Vector2f[]"),
        lua_sf::detail::shaderUniformArrayVariant<sf::Glsl::Vec3>("setUniformVec3Array", "sf.Vector3f[]"),
        lua_sf::detail::shaderUniformArrayVariant<sf::Glsl::Vec4>("setUniformVec4Array", "sf.Vector4f[]"),
        lua_sf::detail::shaderUniformArrayVariant<sf::Glsl::Mat3>("setUniformMat3Array", "sf.Mat3[]"),
        lua_sf::detail::shaderUniformArrayVariant<sf::Glsl::Mat4>("setUniformMat4Array", "sf.Mat4[]")
    );
    LUASF_STUB_DOC(docs[33]);
    LUASF_STUB_FUNCTION("sf.Shader", "getNativeHandle", "fun(self: sf.Shader): integer");
    lua_glue::BindCallable(type_sf__Shader, "getNativeHandle",
        [](const sf::Shader& self) -> unsigned int {
            return self.getNativeHandle();
        },
        docs[33]
    );
    LUASF_STUB_DOC(docs[34]);
    LUASF_STUB_FUNCTION("sf.Shader", "bind", "fun(shader: sf.Shader)");
    lua_glue::BindCallable(type_sf__Shader, "bind",
        [](const sf::Shader* shader) {
            sf::Shader::bind(shader);
        },
        docs[34]
    );
    LUASF_STUB_DOC(docs[35]);
    LUASF_STUB_FUNCTION("sf.Shader", "isAvailable", "fun(): boolean");
    lua_glue::BindCallable(type_sf__Shader, "isAvailable",
        []() -> bool {
            return sf::Shader::isAvailable();
        },
        docs[35]
    );
    LUASF_STUB_DOC(docs[36]);
    LUASF_STUB_FUNCTION("sf.Shader", "isGeometryAvailable", "fun(): boolean");
    lua_glue::BindCallable(type_sf__Shader, "isGeometryAvailable",
        []() -> bool {
            return sf::Shader::isGeometryAvailable();
        },
        docs[36]
    );
    LUASF_STUB_DOC(docs[37]);
    LUASF_STUB_CLASS("sf.Shader.Type");
    LUASF_STUB_DOC(docs[38]);
    LUASF_STUB_FIELD("Vertex", "sf.Shader.Type");
    LUASF_STUB_DOC(docs[39]);
    LUASF_STUB_FIELD("Geometry", "sf.Shader.Type");
    LUASF_STUB_DOC(docs[40]);
    LUASF_STUB_FIELD("Fragment", "sf.Shader.Type");
    lua_glue::BindEnum<sf::Shader::Type>(table_sf__Shader, "Type", {
        {"Vertex", sf::Shader::Type::Vertex},
        {"Geometry", sf::Shader::Type::Geometry},
        {"Fragment", sf::Shader::Type::Fragment}
    });
    auto type_sf__Shader__CurrentTextureType = lua_glue::BindClass<sf::Shader::CurrentTextureType>(table_sf__Shader, "CurrentTextureType");
    lua_glue::Table table_sf__Shader__CurrentTextureType = table_sf__Shader["CurrentTextureType"].get<lua_glue::Table>();
    lua_sf::mark_shared_usertype<sf::Shader::CurrentTextureType>(lua);
    LUASF_STUB_DOC(docs[41]);
    LUASF_STUB_CLASS("sf.Shader.CurrentTextureType");
    LUASF_STUB_FUNCTION("sf.Shader.CurrentTextureType", "new", "fun(): sf.Shader.CurrentTextureType");
    lua_glue::BindCallable(type_sf__Shader__CurrentTextureType, "new",
        []() {
            return lua_sf::makeLuaSharedObject<sf::Shader::CurrentTextureType>();
        }
    );
    LUASF_STUB_DOC(docs[42]);
    LUASF_STUB_VALUE("sf.Shader", "CurrentTexture", "sf.Shader.CurrentTextureType");
    lua_glue::BindStaticAttr<sf::Shader::CurrentTextureType>(table_sf__Shader, "CurrentTexture", &sf::Shader::CurrentTexture);
}
