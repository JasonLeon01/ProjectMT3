# LuaGlue

LuaGlue is a C++20 binding runtime for generated Lua 5.5 interfaces. It depends only on Lua and supports builds with RTTI disabled. Type registration, overload dispatch, properties, and userdata ownership live in the compiled runtime; templates adapt concrete C++ signatures.

[Full reference and example](README.md)

## CMake integration

Keep this directory as a submodule or a source dependency. Provide the existing Lua target before adding the project:

```cmake
set(LUAGLUE_LUA_TARGET MyLuaTarget)
add_subdirectory(third_party/LuaGlue)
target_link_libraries(MyApplication PRIVATE LuaGlue::LuaGlue)
```

If `LUAGLUE_LUA_TARGET` is empty, the project's finder locates Lua 5.5 and verifies its header version, including with CMake versions that cannot recognize Lua 5.5's numeric version macros. Set `LUA_INCLUDE_DIR` and `LUA_LIBRARY` explicitly, or supply the installation prefix through `CMAKE_PREFIX_PATH`. LuaGlue does not download Lua. `LUAGLUE_BUILD_SHARED` defaults to `ON` on desktop and `OFF` on iOS, Android, and OHOS. All modules in a process must use the same Lua and LuaGlue runtimes. Distribute the shared library with desktop consumers.

The supported interface is the `LuaGlue::LuaGlue` target created by `add_subdirectory`. An installed `find_package(LuaGlue)` export is not currently provided.

## Binding a fixed interface

The [complete example](README.md#binding-a-fixed-interface) demonstrates `BindClass`, `BindConstructor`, `BindAttr`, `BindStaticAttr`, `BindMethod`, and `BindFunction`. These interfaces take explicit Lua names; the generator must select the exact C++ signature for each overloaded member function.

Constructors are available through `MyClass.new(...)`; registering a class alone does not add a constructor. Instance properties access the actual instance, and static properties access the C++ static storage. Const properties are read-only. Container values convert to ordinary Lua tables; assigning the entire property writes a converted container back.

`BindCallable` and `BindFactory` accept captured or uncaptured lambdas. Register each overload separately under the same Lua name. `BindProperty` takes a getter and an optional setter; `BindMetamethod` takes a Lua metamethod name such as `"__add"`. `BindBase<Derived, Base>` registers a base-pointer conversion and Lua member inheritance. Use `BindCast<Derived, Base>` for a native interface conversion that does not expose Lua inheritance; that interface needs no Lua class descriptor. `BindEnum<Enum>` accepts an initializer list of name/value pairs.

`Defaults{...}` supplies trailing arguments only. Missing arguments use their defaults; explicit `nil` goes through the parameter codec. Use `DefaultFactory{[] { return expression; }}` for values that must be evaluated for every call. Documentation is a `std::string_view` binding option and can share a source-level `constexpr std::array` with the stub generator.

Omitted nullable arguments, including `const std::optional<T>&`, receive `nil` when no explicit binding default exists. Missing non-nullable arguments still fail overload matching.

## Values and lifetime

`BindStruct<T>` requires an enabled `StructTraits<T>` and adds `copy()`, `deepcopy()`, `__copy`, and `__deepcopy`. Derive that trait from `IndependentValue<T>` only after verifying that the complete C++ state, including private members, has independent value semantics. Its shallow and deep operations both use the native copy constructor. Types containing resource pointers, shared ownership, or Lua references need a complete custom `DeepCopy` policy or must remain `BindClass` types.

Values and smart pointers retain ownership. Borrowed pointers and references require their native owner to remain alive. `ReturnPolicy::ReferenceInternal` keeps the first Lua argument alive with a returned userdata; instance property getters apply this policy by default. `BindingOptions::keep_alive` expresses further relationships using `0` for the result and `1` for the first Lua argument. Access native pointers, shared ownership, and type queries through the public APIs rather than interpreting userdata memory layout.

The host creates and closes the VM. Call `InitializeState` before registration and serialize access with the host execution hooks and `ExecutionScope`. Stop callback producers, then call `QuiesceState` and `ShutdownState` before `lua_close`. Registry-backed handles detach during shutdown and must not be used afterward. LuaGlue does not make a Lua state safe for concurrent use by itself.

`Object`, `Table`, `Function`, `StateView`, `Arguments`, and `CallResult` provide registry-backed values, table operations, stack views, and protected calls. `CallResult` preserves the position of nil results. Use protected calls and let LuaGlue translate C++ errors at its dispatch boundary; a direct `lua_error` can bypass C++ destructors.

LuaGlue is distributed under the [MIT License](LICENSE).
