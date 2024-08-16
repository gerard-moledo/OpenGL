if ["%1"] == ["debug"] GOTO DEBUG
if NOT ["%1"] == ["debug"] GOTO RELEASE

:DEBUG
start "" builds/mingw/debug/program.exe

GOTO DONE
:RELEASE
start "" builds/mingw/release/program.exe

GOTO DONE
:DONE