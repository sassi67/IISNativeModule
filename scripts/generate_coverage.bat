@echo off
REM ============================================================
REM  Coverage workflow: Windows + clang-cl + llvm-cov
REM ============================================================
setlocal

REM Run from the repo root regardless of the caller's working directory,
REM since `cmake --preset` looks for CMakePresets.json in the cwd.
cd /d "%~dp0.."

set PRESET=windows-x86-64-coverage
set BUILD_DIR=_build\%PRESET%
set TEST_EXE=%BUILD_DIR%\test\Debug\ModernCppTest.exe
REM Must be absolute: ctest runs each test with its cwd set to the test's
REM own binary dir, not the repo root, so a relative path here would land
REM %BUILD_DIR% underneath _build\%PRESET%\test\ instead.
set PROFRAW=%CD%\%BUILD_DIR%\default.profraw
set PROFDATA=%BUILD_DIR%\coverage.profdata
set REPORT_DIR=%BUILD_DIR%\coverage-report

echo [1/5] Configure
cmake --preset %PRESET%
if %ERRORLEVEL% neq 0 exit /b %ERRORLEVEL%

echo [2/5] Build
cmake --build --preset %PRESET%
if %ERRORLEVEL% neq 0 exit /b %ERRORLEVEL%

echo [3/5] Run tests (generates .profraw)
set LLVM_PROFILE_FILE=%PROFRAW%
ctest --preset %PRESET%
if %ERRORLEVEL% neq 0 exit /b %ERRORLEVEL%

echo [4/5] Merge profile data
llvm-profdata merge -sparse %PROFRAW% -o %PROFDATA%
if %ERRORLEVEL% neq 0 exit /b %ERRORLEVEL%

echo [5/5] Generate HTML report
REM llvm-cov's built-in Demangle library handles MSVC-mangled names already;
REM -Xdemangler=undname was dropped since undname.exe lives inside a
REM version-pinned MSVC toolset dir and isn't reliably on PATH.
llvm-cov show %TEST_EXE% ^
    -instr-profile=%PROFDATA% ^
    -format=html ^
    -output-dir=%REPORT_DIR% ^
    -show-line-counts-or-regions
if %ERRORLEVEL% neq 0 exit /b %ERRORLEVEL%

echo.
echo === Coverage Summary ===
llvm-cov report %TEST_EXE% -instr-profile=%PROFDATA%
echo.
echo HTML report generated at: %REPORT_DIR%\index.html
start "" %REPORT_DIR%\index.html
endlocal
