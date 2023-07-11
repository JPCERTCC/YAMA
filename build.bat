@REM Set ccache folder to env

set CCACHE_DIR=%CD%\.ccache

@REM Encrypt yara rule to avoid unncessary security alerts.

python utils\arc4.py -f .\rsrc\ioc\rule.yara

@REM Set decrypt key to env

set /p DECRYPT_KEY=<.\rsrc\ioc\rule.yara.key

@REM Initialize build derectory

mkdir build
cd .\build
echo y | del *

@REM Build YAMA Scanner

cmake -DDECRYPT_KEY=%DECRYPT_KEY% ^
      -DCMAKE_C_COMPILER_LAUNCHER="C:/ProgramData/chocolatey/bin/ccache.exe" ^
      -DCMAKE_CXX_COMPILER_LAUNCHER="C:/ProgramData/chocolatey/bin/ccache.exe" ^
      -DCMAKE_C_COMPILER="C:/Program Files/LLVM/bin/clang.exe" ^
      -DCMAKE_CXX_COMPILER="C:/Program Files/LLVM/bin/clang++.exe" ^
      -DCMAKE_RC_COMPILER="C:/Program Files/LLVM/bin/llvm-rc.exe" ^
      -DCMAKE_BUILD_TYPE=Release ^
      -G Ninja ..
ninja -v > yama-ninja.log
cd ..\
