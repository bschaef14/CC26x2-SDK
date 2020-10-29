cd %~dp0
python -m venv .venv
call .venv/scripts/activate.bat

cd core
pip install . --disable-pip-version-check

cd ../transport
pip install . --disable-pip-version-check
cd ..

for /d %%i in (modules/*) do (
cd "%%i"
pip install .
pause
)
