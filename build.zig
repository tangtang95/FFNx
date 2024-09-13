const std = @import("std");

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{ .default_target = .{ .cpu_arch = .x86, .abi = .msvc, .os_tag = .windows } });
    const optimize = b.standardOptimizeOption(.{ .preferred_optimize_mode = .ReleaseFast });
    const dll = b.addSharedLibrary(.{ .name = "ffnx", .target = target, .optimize = optimize, .version = .{ .major = 0, .minor = 0, .patch = 0, .build = "0" } });
    dll.addCSourceFiles(.{
        .files = &.{
            "src/achievement.cpp",
            "src/api.cpp",
            "src/audio/memorystream/memorystream.cpp",
            "src/audio/vgmstream/vgmstream.cpp",
            "src/audio.cpp",
            "src/cfg.cpp",
            "src/common.cpp",
            "src/crashdump.cpp",
            "src/exe_data.cpp",
            "src/external_mesh.cpp",
            "src/fake_dd.cpp",
            "src/ff7/battle/animations.cpp",
            "src/ff7/battle/battle.cpp",
            "src/ff7/battle/camera.cpp",
            "src/ff7/battle/effect.cpp",
            "src/ff7/battle/menu.cpp",
            "src/ff7/dsound.cpp",
            "src/ff7/field/background.cpp",
            "src/ff7/field/camera.cpp",
            "src/ff7/field/field.cpp",
            "src/ff7/field/model.cpp",
            "src/ff7/field/opcode.cpp",
            "src/ff7/file.cpp",
            "src/ff7/graphics.cpp",
            "src/ff7/kernel.cpp",
            "src/ff7/loaders.cpp",
            "src/ff7/menu.cpp",
            "src/ff7/minigames.cpp",
            "src/ff7/misc.cpp",
            "src/ff7/time.cpp",
            "src/ff7/widescreen.cpp",
            "src/ff7/world/camera.cpp",
            "src/ff7/world/player.cpp",
            "src/ff7/world/renderer.cpp",
            "src/ff7/world/world.cpp",
            "src/ff7_opengl.cpp",
            "src/ff8/ambient.cpp",
            "src/ff8/battle/stage.cpp",
            "src/ff8/engine.cpp",
            "src/ff8/field/background.cpp",
            "src/ff8/field/chara_one.cpp",
            "src/ff8/file.cpp",
            "src/ff8/mod.cpp",
            "src/ff8/texture_packer.cpp",
            "src/ff8/uv_patch.cpp",
            "src/ff8/vibration.cpp",
            "src/ff8/vram.cpp",
            "src/ff8/world/chara_one.cpp",
            "src/ff8/world/wmset.cpp",
            "src/ff8_data.cpp",
            "src/ff8_opengl.cpp",
            "src/field.cpp",
            "src/game_cfg.cpp",
            "src/gamehacks.cpp",
            "src/gamepad.cpp",
            "src/gl/deferred.cpp",
            "src/gl/gl.cpp",
            "src/gl/special_case.cpp",
            "src/gl/texture.cpp",
            "src/hext.cpp",
            "src/image/image.cpp",
            "src/image/tim.cpp",
            "src/input.cpp",
            "src/joystick.cpp",
            "src/lighting.cpp",
            "src/lighting_debug.cpp",
            "src/log.cpp",
            "src/matrix.cpp",
            "src/md5.cpp",
            "src/metadata.cpp",
            "src/movies.cpp",
            "src/music.cpp",
            "src/overlay.cpp",
            "src/patch.cpp",
            "src/redirect.cpp",
            "src/renderer.cpp",
            "src/saveload.cpp",
            "src/sfx.cpp",
            "src/utils.cpp",
            "src/vibration.cpp",
            "src/video/movies.cpp",
            "src/voice.cpp",
            "src/world.cpp",
        },
        .flags = &.{
            "-std=c++20",
            "-fms-compatibility-version=19.10",
            "-m32",
            "-D_MT",
            "-DWIN32",
            "-D_WINDOWS",
            "-DFFNx_EXPORTS",
            "-D_DEBUG_FUNCTIONAL_MACHINERY",
            "-Wall",
            "-Wmicrosoft",
            "-Wno-invalid-token-paste",
            "-Wno-unknown-pragmas",
            "-Wno-unused-value",
            "-DBX_CONFIG_DEBUG=1", // For BX debug mode
            "-DDIRECTINPUT_VERSION=0x0800",
            "-D_CRT_SECURE_NO_WARNINGS",
            "-DPATCH_COLLECT_DUPLICATES", // TODO: put under options
            "-DNOMINMAX",
        },
    });

    // For msvc abi, we just need libc (no need for libcpp)
    dll.linkLibC();

    // ------ vcpkg deps
    dll.linkSystemLibrary("DirectXTex");
    dll.linkSystemLibrary("HWinfo");
    dll.linkSystemLibrary("PSXCore");
    dll.linkSystemLibrary("StackWalker");
    dll.linkSystemLibrary("bgfxRelease");
    dll.linkSystemLibrary("bimgRelease");
    dll.linkSystemLibrary("bimg_decodeRelease");
    dll.linkSystemLibrary("bimg_encodeRelease");
    dll.linkSystemLibrary("bxRelease");
    dll.linkSystemLibrary("clog");
    dll.linkSystemLibrary("cpuinfo");
    dll.linkSystemLibrary("fcppRelease");
    dll.linkSystemLibrary("glsl-optimizerRelease");
    dll.linkSystemLibrary("glslangRelease");
    dll.linkSystemLibrary("imgui");
    dll.linkSystemLibrary("libpng16");
    dll.linkSystemLibrary("libvgmstream");
    dll.linkSystemLibrary("mpg123");
    dll.linkSystemLibrary("out123");
    dll.linkSystemLibrary("postproc");
    dll.linkSystemLibrary("psflib");
    dll.linkSystemLibrary("pugixml");
    dll.linkSystemLibrary("soloud_static_x86");
    dll.linkSystemLibrary("speex");
    dll.linkSystemLibrary("spirv-crossRelease");
    dll.linkSystemLibrary("spirv-optRelease");
    dll.linkSystemLibrary("steam_api");
    dll.linkSystemLibrary("syn123");
    dll.linkSystemLibrary("texturecRelease");
    dll.linkSystemLibrary("theora");
    dll.linkSystemLibrary("vorbisfile");
    dll.linkSystemLibrary("xxhash");
    dll.linkSystemLibrary("zlib");

    // ffmpeg known deps
    dll.linkSystemLibrary("avcodec");
    dll.linkSystemLibrary("avdevice");
    dll.linkSystemLibrary("avfilter");
    dll.linkSystemLibrary("avformat");
    dll.linkSystemLibrary("avutil");
    dll.linkSystemLibrary("swscale");
    dll.linkSystemLibrary("swresample");

    // libavformat deps (from pkgconfig .pc)
    dll.linkSystemLibrary("secur32");
    dll.linkSystemLibrary("ws2_32");

    // libavcoded deps (from pkgconfig .pc)
    dll.linkSystemLibrary("dav1d");
    dll.linkSystemLibrary("libmfx");
    dll.linkSystemLibrary("libx264");
    dll.linkSystemLibrary("mfuuid");
    dll.linkSystemLibrary("ogg");
    dll.linkSystemLibrary("ole32");
    dll.linkSystemLibrary("opus");
    dll.linkSystemLibrary("strmiids");
    dll.linkSystemLibrary("theoradec");
    dll.linkSystemLibrary("theoraenc");
    dll.linkSystemLibrary("user32");
    dll.linkSystemLibrary("vorbis");
    dll.linkSystemLibrary("vorbisenc");
    dll.linkSystemLibrary("vpx");
    dll.linkSystemLibrary("x265-static");

    // libavdevice deps (from pkgconfig .pc)
    dll.linkSystemLibrary("psapi");
    dll.linkSystemLibrary("ole32");
    dll.linkSystemLibrary("strmiids");
    dll.linkSystemLibrary("uuid");
    dll.linkSystemLibrary("oleaut32");
    dll.linkSystemLibrary("shlwapi");
    dll.linkSystemLibrary("gdi32");
    dll.linkSystemLibrary("vfw32");

    // libavfilter deps (from pkgconfig .pc)
    dll.linkSystemLibrary("advapi32");
    dll.linkSystemLibrary("cfgmgr32");
    dll.linkSystemLibrary("libmfx");
    dll.linkSystemLibrary("ole32");
    dll.linkSystemLibrary("openCL");

    // libavutil deps (from pkgconfig .pc)
    dll.linkSystemLibrary("advapi32");
    dll.linkSystemLibrary("bcrypt");
    dll.linkSystemLibrary("cfgmgr32");
    dll.linkSystemLibrary("libmfx");
    dll.linkSystemLibrary("ole32");
    dll.linkSystemLibrary("openCL");
    dll.linkSystemLibrary("user32");

    // libmpg123 deps
    dll.linkSystemLibrary("shlwapi");

    // ------

    // other needed library from libc paths
    dll.linkSystemLibrary("dbghelp");
    dll.linkSystemLibrary("dinput8");
    dll.linkSystemLibrary("dwmapi");
    dll.linkSystemLibrary("dxguid");
    dll.linkSystemLibrary("shell32");
    dll.linkSystemLibrary("kernel32");
    dll.linkSystemLibrary("winspool");
    dll.linkSystemLibrary("comdlg32");
    dll.linkSystemLibrary("winmm");
    dll.linkSystemLibrary("wsock32");
    dll.linkSystemLibrary("XINPUT9_1_0");

    // Need to manually include C include path and library path due to zig unable to auto detect x86-windows-msvc paths
    dll.addIncludePath(b.path(".build\\vcpkg_installed\\x86-windows-static\\include"));
    dll.addIncludePath(b.path(".build\\vcpkg_installed\\x86-windows-static\\include\\compat\\msvc")); // bx deps include required
    dll.addIncludePath(b.path("src"));
    dll.addLibraryPath(.{ .cwd_relative = "C:\\Program Files (x86)\\Windows Kits\\10\\Lib\\10.0.22621.0\\um\\x86" });
    dll.addLibraryPath(b.path(".build\\vcpkg_installed\\x86-windows-static\\lib"));

    b.installArtifact(dll);
}
