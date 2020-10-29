@echo off
cd %~dp0

REM Get proxy argument. If not specified, use default value.
IF "%1"=="" (
    SET ProxyServer="wwwgate.ti.com"
) ELSE (
    SET ProxyServer="%1"
)

IF "%1"=="" (
    echo Defaulting to --proxy %ProxyServer%, call this script with an argument to customise
) ELSE (
    echo Using --proxy %ProxyServer% for calls to pip
)

python -m venv .venv
call .venv/scripts/activate.bat

cd core
pip install . --proxy %ProxyServer% --disable-pip-version-check

cd ../transport
pip install . --proxy %ProxyServer% --disable-pip-version-check
cd ..

for /d %%i in (modules/*) do (
cd "%%i"
pip install .
pause
)
