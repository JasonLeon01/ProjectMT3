#include "Graphics/bind_Shader.hpp"

#include <algorithm>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

void bind_Shader(sol::state_view lua) {
    sol::table sf = lua_sf::sf_table(lua);
    auto type_sf__Shader = sf.new_usertype<sf::Shader>("Shader", sol::no_constructor);
    sol::table table_sf__Shader = sf["Shader"].get<sol::table>();
    lua_sf::mark_shared_usertype<sf::Shader>(lua);
    LUASF_STUB_DOC("\\brief Shader class (vertex, geometry and fragment)");
    LUASF_STUB_CLASS("sf.Shader");
    LUASF_STUB_DOC("\\brief Construct from vertex, geometry and fragment shader files\n\nThis constructor loads the vertex, geometry and fragment\nshaders. If one of them fails to load, the shader is left\nempty (the valid shader is unloaded).\nThe sources must be text files containing valid shaders\nin GLSL language. GLSL is a C-like language dedicated to\nOpenGL shaders; you'll probably need to read a good documentation\nfor it before writing your own shaders.\n\n\\param vertexShaderFilename   Path of the vertex shader file to load\n\\param geometryShaderFilename Path of the geometry shader file to load\n\\param fragmentShaderFilename Path of the fragment shader file to load\n\n\\throws sf::Exception if loading was unsuccessful\n\n\\see `loadFromFile`, `loadFromMemory`, `loadFromStream`");
    LUASF_STUB_FUNCTION("sf.Shader", "new", "fun(vertexShaderFilename: string, geometryShaderFilename: string, fragmentShaderFilename: string): sf.Shader");
    LUASF_STUB_OVERLOAD("sf.Shader", "new", "fun(vertexShaderStream: sf.InputStream, geometryShaderStream: sf.InputStream, fragmentShaderStream: sf.InputStream): sf.Shader");
    LUASF_STUB_OVERLOAD("sf.Shader", "new", "fun(filename: string, type: sf.Shader.Type): sf.Shader");
    LUASF_STUB_OVERLOAD("sf.Shader", "new", "fun(vertexShaderFilename: string, fragmentShaderFilename: string): sf.Shader");
    LUASF_STUB_OVERLOAD("sf.Shader", "new", "fun(stream: sf.InputStream, type: sf.Shader.Type): sf.Shader");
    LUASF_STUB_OVERLOAD("sf.Shader", "new", "fun(vertexShaderStream: sf.InputStream, fragmentShaderStream: sf.InputStream): sf.Shader");
    LUASF_STUB_OVERLOAD("sf.Shader", "new", "fun(): sf.Shader");
    type_sf__Shader.set_function("new", sol::factories(
        [](std::string vertexShaderFilename, std::string geometryShaderFilename, std::string fragmentShaderFilename) {
            return lua_sf::makeLuaSharedObject<sf::Shader>(std::filesystem::path(vertexShaderFilename), std::filesystem::path(geometryShaderFilename), std::filesystem::path(fragmentShaderFilename));
        },
        [](sf::InputStream& vertexShaderStream, sf::InputStream& geometryShaderStream, sf::InputStream& fragmentShaderStream) {
            return lua_sf::makeLuaSharedObject<sf::Shader>(vertexShaderStream, geometryShaderStream, fragmentShaderStream);
        },
        [](std::string filename, sf::Shader::Type type) {
            return lua_sf::makeLuaSharedObject<sf::Shader>(std::filesystem::path(filename), type);
        },
        [](std::string vertexShaderFilename, std::string fragmentShaderFilename) {
            return lua_sf::makeLuaSharedObject<sf::Shader>(std::filesystem::path(vertexShaderFilename), std::filesystem::path(fragmentShaderFilename));
        },
        [](sf::InputStream& stream, sf::Shader::Type type) {
            return lua_sf::makeLuaSharedObject<sf::Shader>(stream, type);
        },
        [](sf::InputStream& vertexShaderStream, sf::InputStream& fragmentShaderStream) {
            return lua_sf::makeLuaSharedObject<sf::Shader>(vertexShaderStream, fragmentShaderStream);
        },
        []() {
            return lua_sf::makeLuaSharedObject<sf::Shader>();
        }
    ));
    LUASF_STUB_DOC("\\brief Load the vertex, geometry and fragment shaders from files\n\nThis function loads the vertex, geometry and fragment\nshaders. If one of them fails to load, the shader is left\nempty (the valid shader is unloaded).\nThe sources must be text files containing valid shaders\nin GLSL language. GLSL is a C-like language dedicated to\nOpenGL shaders; you'll probably need to read a good documentation\nfor it before writing your own shaders.\n\n\\param vertexShaderFilename   Path of the vertex shader file to load\n\\param geometryShaderFilename Path of the geometry shader file to load\n\\param fragmentShaderFilename Path of the fragment shader file to load\n\n\\return `true` if loading succeeded, `false` if it failed\n\n\\see `loadFromMemory`, `loadFromStream`");
    LUASF_STUB_FUNCTION("sf.Shader", "loadFromFile", "fun(self: sf.Shader, vertexShaderFilename: string, geometryShaderFilename: string, fragmentShaderFilename: string): boolean");
    LUASF_STUB_OVERLOAD("sf.Shader", "loadFromFile", "fun(self: sf.Shader, filename: string, type: sf.Shader.Type): boolean");
    LUASF_STUB_OVERLOAD("sf.Shader", "loadFromFile", "fun(self: sf.Shader, vertexShaderFilename: string, fragmentShaderFilename: string): boolean");
    type_sf__Shader.set_function("loadFromFile",
        sol::overload(
            [](sf::Shader& self, std::string vertexShaderFilename, std::string geometryShaderFilename, std::string fragmentShaderFilename) -> bool {
                return self.loadFromFile(std::filesystem::path(vertexShaderFilename), std::filesystem::path(geometryShaderFilename), std::filesystem::path(fragmentShaderFilename));
            },
            [](sf::Shader& self, std::string filename, sf::Shader::Type type) -> bool {
                return self.loadFromFile(std::filesystem::path(filename), type);
            },
            [](sf::Shader& self, std::string vertexShaderFilename, std::string fragmentShaderFilename) -> bool {
                return self.loadFromFile(std::filesystem::path(vertexShaderFilename), std::filesystem::path(fragmentShaderFilename));
            }
        )
    );
    LUASF_STUB_DOC("\\brief Load the vertex, geometry and fragment shaders from source codes in memory\n\nThis function loads the vertex, geometry and fragment\nshaders. If one of them fails to load, the shader is left\nempty (the valid shader is unloaded).\nThe sources must be valid shaders in GLSL language. GLSL is\na C-like language dedicated to OpenGL shaders; you'll\nprobably need to read a good documentation for it before\nwriting your own shaders.\n\n\\param vertexShader   String containing the source code of the vertex shader\n\\param geometryShader String containing the source code of the geometry shader\n\\param fragmentShader String containing the source code of the fragment shader\n\n\\return `true` if loading succeeded, `false` if it failed\n\n\\see `loadFromFile`, `loadFromStream`");
    LUASF_STUB_FUNCTION("sf.Shader", "loadFromMemory", "fun(self: sf.Shader, vertexShader: string, geometryShader: string, fragmentShader: string): boolean");
    LUASF_STUB_OVERLOAD("sf.Shader", "loadFromMemory", "fun(self: sf.Shader, shader: string, type: sf.Shader.Type): boolean");
    LUASF_STUB_OVERLOAD("sf.Shader", "loadFromMemory", "fun(self: sf.Shader, vertexShader: string, fragmentShader: string): boolean");
    type_sf__Shader.set_function("loadFromMemory",
        sol::overload(
            [](sf::Shader& self, std::string vertexShader, std::string geometryShader, std::string fragmentShader) -> bool {
                return self.loadFromMemory(vertexShader, geometryShader, fragmentShader);
            },
            [](sf::Shader& self, std::string shader, sf::Shader::Type type) -> bool {
                return self.loadFromMemory(shader, type);
            },
            [](sf::Shader& self, std::string vertexShader, std::string fragmentShader) -> bool {
                return self.loadFromMemory(vertexShader, fragmentShader);
            }
        )
    );
    LUASF_STUB_DOC("\\brief Load the vertex, geometry and fragment shaders from custom streams\n\nThis function loads the vertex, geometry and fragment\nshaders. If one of them fails to load, the shader is left\nempty (the valid shader is unloaded).\nThe source codes must be valid shaders in GLSL language.\nGLSL is a C-like language dedicated to OpenGL shaders;\nyou'll probably need to read a good documentation for\nit before writing your own shaders.\n\n\\param vertexShaderStream   Source stream to read the vertex shader from\n\\param geometryShaderStream Source stream to read the geometry shader from\n\\param fragmentShaderStream Source stream to read the fragment shader from\n\n\\return `true` if loading succeeded, `false` if it failed\n\n\\see `loadFromFile`, `loadFromMemory`");
    LUASF_STUB_FUNCTION("sf.Shader", "loadFromStream", "fun(self: sf.Shader, vertexShaderStream: sf.InputStream, geometryShaderStream: sf.InputStream, fragmentShaderStream: sf.InputStream): boolean");
    LUASF_STUB_OVERLOAD("sf.Shader", "loadFromStream", "fun(self: sf.Shader, stream: sf.InputStream, type: sf.Shader.Type): boolean");
    LUASF_STUB_OVERLOAD("sf.Shader", "loadFromStream", "fun(self: sf.Shader, vertexShaderStream: sf.InputStream, fragmentShaderStream: sf.InputStream): boolean");
    type_sf__Shader.set_function("loadFromStream",
        sol::overload(
            [](sf::Shader& self, sf::InputStream& vertexShaderStream, sf::InputStream& geometryShaderStream, sf::InputStream& fragmentShaderStream) -> bool {
                return self.loadFromStream(vertexShaderStream, geometryShaderStream, fragmentShaderStream);
            },
            [](sf::Shader& self, sf::InputStream& stream, sf::Shader::Type type) -> bool {
                return self.loadFromStream(stream, type);
            },
            [](sf::Shader& self, sf::InputStream& vertexShaderStream, sf::InputStream& fragmentShaderStream) -> bool {
                return self.loadFromStream(vertexShaderStream, fragmentShaderStream);
            }
        )
    );
    LUASF_STUB_DOC("\\brief Specify value for \\p float uniform\n\n\\param name Name of the uniform variable in GLSL\n\\param x    Value of the float scalar");
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
    type_sf__Shader.set_function("setUniform",
        sol::overload(
            [](sf::Shader& self, std::string name, float x) {
                self.setUniform(name, x);
            },
            [](sf::Shader& self, std::string name, sf::Glsl::Vec2 vector) {
                self.setUniform(name, vector);
            },
            [](sf::Shader& self, std::string name, const sf::Glsl::Vec3& vector) {
                self.setUniform(name, vector);
            },
            [](sf::Shader& self, std::string name, const sf::Glsl::Vec4& vector) {
                self.setUniform(name, vector);
            },
            [](sf::Shader& self, std::string name, lua_sf::LuaIntegral<int> x) {
                self.setUniform(name, x.value());
            },
            [](sf::Shader& self, std::string name, sf::Glsl::Ivec2 vector) {
                self.setUniform(name, vector);
            },
            [](sf::Shader& self, std::string name, const sf::Glsl::Ivec3& vector) {
                self.setUniform(name, vector);
            },
            [](sf::Shader& self, std::string name, const sf::Glsl::Ivec4& vector) {
                self.setUniform(name, vector);
            },
            [](sf::Shader& self, std::string name, bool x) {
                self.setUniform(name, x);
            },
            [](sf::Shader& self, std::string name, sf::Glsl::Bvec2 vector) {
                self.setUniform(name, vector);
            },
            [](sf::Shader& self, std::string name, const sf::Glsl::Bvec3& vector) {
                self.setUniform(name, vector);
            },
            [](sf::Shader& self, std::string name, const sf::Glsl::Bvec4& vector) {
                self.setUniform(name, vector);
            },
            [](sf::Shader& self, std::string name, const sf::Glsl::Mat3& matrix) {
                self.setUniform(name, matrix);
            },
            [](sf::Shader& self, std::string name, const sf::Glsl::Mat4& matrix) {
                self.setUniform(name, matrix);
            },
            [](sf::Shader& self, std::string name, const sf::Texture& texture) {
                self.setUniform(name, texture);
            },
            [](sf::Shader& self, std::string name, sf::Shader::CurrentTextureType arg1) {
                self.setUniform(name, arg1);
            }
        )
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
    LUASF_STUB_DOC("\\brief Get the underlying OpenGL handle of the shader.\n\nYou shouldn't need to use this function, unless you have\nvery specific stuff to implement that SFML doesn't support,\nor implement a temporary workaround until a bug is fixed.\n\n\\return OpenGL handle of the shader or 0 if not yet loaded");
    LUASF_STUB_FUNCTION("sf.Shader", "getNativeHandle", "fun(self: sf.Shader): integer");
    type_sf__Shader.set_function("getNativeHandle",
        [](sf::Shader& self) -> unsigned int {
            return self.getNativeHandle();
        }
    );
    LUASF_STUB_DOC("\\brief Bind a shader for rendering\n\nThis function is not part of the graphics API, it mustn't be\nused when drawing SFML entities. It must be used only if you\nmix `sf::Shader` with OpenGL code.\n\n\\code\nsf::Shader s1, s2;\n...\nsf::Shader::bind(&s1);\n// draw OpenGL stuff that use s1...\nsf::Shader::bind(&s2);\n// draw OpenGL stuff that use s2...\nsf::Shader::bind(nullptr);\n// draw OpenGL stuff that use no shader...\n\\endcode\n\n\\param shader Shader to bind, can be null to use no shader");
    LUASF_STUB_FUNCTION("sf.Shader", "bind", "fun(shader: sf.Shader)");
    type_sf__Shader.set_function("bind",
        [](const sf::Shader* shader) {
            sf::Shader::bind(shader);
        }
    );
    LUASF_STUB_DOC("\\brief Tell whether or not the system supports shaders\n\nThis function should always be called before using\nthe shader features. If it returns `false`, then\nany attempt to use `sf::Shader` will fail.\n\n\\return `true` if shaders are supported, `false` otherwise");
    LUASF_STUB_FUNCTION("sf.Shader", "isAvailable", "fun(): boolean");
    type_sf__Shader.set_function("isAvailable",
        []() -> bool {
            return sf::Shader::isAvailable();
        }
    );
    LUASF_STUB_DOC("\\brief Tell whether or not the system supports geometry shaders\n\nThis function should always be called before using\nthe geometry shader features. If it returns `false`, then\nany attempt to use `sf::Shader` geometry shader features will fail.\n\nThis function can only return `true` if isAvailable() would also\nreturn `true`, since shaders in general have to be supported in\norder for geometry shaders to be supported as well.\n\nNote: The first call to this function, whether by your\ncode or SFML will result in a context switch.\n\n\\return `true` if geometry shaders are supported, `false` otherwise");
    LUASF_STUB_FUNCTION("sf.Shader", "isGeometryAvailable", "fun(): boolean");
    type_sf__Shader.set_function("isGeometryAvailable",
        []() -> bool {
            return sf::Shader::isGeometryAvailable();
        }
    );
    LUASF_STUB_DOC("\\brief Types of shaders");
    LUASF_STUB_CLASS("sf.Shader.Type");
    LUASF_STUB_DOC("%Vertex shader");
    LUASF_STUB_FIELD("Vertex", "sf.Shader.Type");
    LUASF_STUB_DOC("Geometry shader");
    LUASF_STUB_FIELD("Geometry", "sf.Shader.Type");
    LUASF_STUB_DOC("Fragment (pixel) shader");
    LUASF_STUB_FIELD("Fragment", "sf.Shader.Type");
    table_sf__Shader.new_enum("Type",
        "Vertex", sf::Shader::Type::Vertex,
        "Geometry", sf::Shader::Type::Geometry,
        "Fragment", sf::Shader::Type::Fragment
    );
    auto type_sf__Shader__CurrentTextureType = table_sf__Shader.new_usertype<sf::Shader::CurrentTextureType>("CurrentTextureType", sol::no_constructor);
    sol::table table_sf__Shader__CurrentTextureType = table_sf__Shader["CurrentTextureType"].get<sol::table>();
    lua_sf::mark_shared_usertype<sf::Shader::CurrentTextureType>(lua);
    LUASF_STUB_DOC("\\brief Special type that can be passed to setUniform(),\nand that represents the texture of the object being drawn\n\n\\see `setUniform(const std::string&, CurrentTextureType)`");
    LUASF_STUB_CLASS("sf.Shader.CurrentTextureType");
    LUASF_STUB_FUNCTION("sf.Shader.CurrentTextureType", "new", "fun(): sf.Shader.CurrentTextureType");
    type_sf__Shader__CurrentTextureType.set_function("new", sol::factories(
        []() {
            return lua_sf::makeLuaSharedObject<sf::Shader::CurrentTextureType>();
        }
    ));
    LUASF_STUB_DOC("\\brief Represents the texture of the object being drawn\n\n\\see `setUniform(const std::string&, CurrentTextureType)`");
    LUASF_STUB_VALUE("sf.Shader", "CurrentTexture", "sf.Shader.CurrentTextureType");
    table_sf__Shader["CurrentTexture"] = sf::Shader::CurrentTexture;
}
