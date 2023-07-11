@REM set ccache folder to env

set CCACHE_DIR=%CD%\.ccache

@REM update yara resource file

python utils\arc4.py -f .\rsrc\ioc\rule.yara

@REM Set decrypt key to env

set /p DECRYPT_KEY=<.\rsrc\ioc\rule.yara.key

@REM Update Compiler option
cd build
cmake -DDECRYPT_KEY=%DECRYPT_KEY% ^
      -DCMAKE_C_COMPILER_LAUNCHER="C:/ProgramData/chocolatey/bin/ccache.exe" ^
      -DCMAKE_CXX_COMPILER_LAUNCHER="C:/ProgramData/chocolatey/bin/ccache.exe" ^
      -DCMAKE_C_COMPILER="C:/Program Files/LLVM/bin/clang.exe" ^
      -DCMAKE_CXX_COMPILER="C:/Program Files/LLVM/bin/clang++.exe" ^
      -DCMAKE_RC_COMPILER="C:/Program Files/LLVM/bin/llvm-rc.exe" ^
      -DCMAKE_BUILD_TYPE=Release ^
      -G Ninja ..

@REM fource update resource object
ninja -t clean CMakeFiles/Yama.dir/rsrc/resources.rc.res > yama-ninja.log
ninja -v CMakeFiles/Yama.dir/rsrc/resources.rc.res >> yama-ninja.log

@REM build YAMA scanner

ninja >> yama-ninja.log
cd ..\