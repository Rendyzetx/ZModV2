@echo off
setlocal
set PATH=C:\Program Files (x86)\Microsoft Visual Studio\Installer;%PATH%
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" >nul
echo VCToolsInstallDir=%VCToolsInstallDir%
echo WindowsSdkDir=%WindowsSdkDir%
echo WindowsSDKVersion=%WindowsSDKVersion%
echo INCLUDE_LEN_BEFORE_MSBUILD=
echo %INCLUDE% | find /c "ucrt"
echo --- MSBUILD ---
msbuild "CodeSpark.sln" /p:Configuration=Release /p:Platform=x64 /p:UseEnv=true /m /v:m /nologo
