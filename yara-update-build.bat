@REM update yara resource file

python utils\arc4.py -f .\rsrc\ioc\rule.yara

@REM update resource object

cd build
ninja -t clean CMakeFiles/Yama.dir/rsrc/resources.rc.res
ninja -v CMakeFiles/Yama.dir/rsrc/resources.rc.res

@REM build YAMA scanner

ninja
cd ..\