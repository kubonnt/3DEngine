@echo off
set BUILD_DIR=%1
set CONFIG=%2

cmake --build %BUILD_DIR% --config %CONFIG% > %BUILD_DIR%\build_log.txt 2>&1