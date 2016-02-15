@echo off
cd /d %~dp0 
mklink /d /j c:\boost E:\rover-self-work\cpp\boost_1_53_0 >nul 2>nul
for %%i in (src, src_tools, 3rdparty, publish, include, lib, product, sln, document, tmp) do (
    md %%i >nul 2>nul
)

::mklink /d /j c:\softmgr %~dp0 >nul 2>nul
::explorer /e,/root,\local disk,c:\softmgr

echo ______________________________________________________________________
echo 3rdparty       放第三方开源代码
echo src            放项目工程代码
echo src_tools      放项目工程生成模板和编译脚本
echo publish        放引用其他工程的接口头文件
echo include        放公共引用的版本信息头文件
echo sln            放.sln文件
echo tmp            放项目编译生成的临时文件，可添加到svn忽略列表
echo lib            放编译的第三方库的lib
echo product        放编译的dll和exe
echo document       放文档
echo ______________________________________________________________________

pause