# FFNx with Zig as compiler

Currently it is possible to compile with zig build system. However the behavior in-game is buggy (can be seen in FIELD mode as soon as new game is played) and different if compiled with CMake.
It is possible to compile with release optimization.

Here is how to build with Zig:
1. Generate `.build` directory with CMake
2. Fix Windows SDK absolute path inside [build.zig](build.zig) at line 235
3. Run the following zig command:

```bash
zig build --libc x86-windows-msvc-libc.ini --release
```

NOTE: There will be some warning about linker error LNK4099 related to not found debug files ".pdb" about vcpkg dependency libraries

## Known limitations

- Module definition [FFNx.def](misc/FFNx.def) is not used in the linker step
- It is impossible to set linker flags for zig build system (https://github.com/ziglang/zig/issues/3382) such as /FORCE:MULTIPLE (previously used in CMake, now removed)
- Zig uses clang as compiler, which is different than the compiler used by CMake (i.e. `cl.exe` from MSVC)
- build.zig does not support debug and other flags or run/build steps but they can be added
- cmrc library has been removed which is used to statically import into DLL the FFNx logo image (This library depends on CMake so it's not possible to use it with Zig)
- openpsf and mimalloc library has been removed because it could not be compiled unless /FORCE:MULTIPLE linker flag is enabled

