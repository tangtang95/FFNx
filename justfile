alias c := cmake
alias b := build
alias r := run-exe

set windows-shell := ["powershell.exe", "-NoLogo", "-Command"]
set export
CFLAGS := "-DMINHOOK_DISABLE_INTRINSICS"
FF7_DIR := "C:\\Program Files (x86)\\Steam\\steamapps\\common\\FINAL FANTASY VII"
FF7_LAUNCHER := "C:\\Program Files (x86)\\Steam\\steamapps\\common\\FINAL FANTASY VII\\FF7_Launcher.exe"

cmake:
  cmake --preset RelWithDebInfo

build:
  cmake --build --preset RelWithDebInfo

run-exe:
  Start-Process -FilePath "C:\\Program Files (x86)\\Steam\\steamapps\\common\\FINAL FANTASY VII\\FF7_Launcher.exe" -WorkingDirectory "C:\\Program Files (x86)\\Steam\\steamapps\\common\\FINAL FANTASY VII"
