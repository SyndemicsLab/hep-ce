@ECHO OFF

cd ..
:: Install Conan
ECHO "Checking Python Avaliability"
python3 --version 3>nul

if errorlevel 1 (
    ECHO.
    ECHO Error^: Python not installed
    ECHO.
    ECHO.
    ECHO "Downloading Python 3.10.0..."
    IF EXIST "%CD%\python-3.10.0.exe" (
    ECHO Found Installer at "%CD%\python-3.10.0.exe"
    ) ELSE (
    powershell -Command "& {[Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12, [Net.SecurityProtocolType]::Tls11, [Net.SecurityProtocolType]::Ssl3, [Net.SecurityProtocolType]::Tls; Invoke-WebRequest -Uri 'https://www.python.org/ftp/python/3.10.0/python-3.10.0.exe' -OutFile '%CD%\python-3.10.0.exe';}"
    ECHO "Python Download Completed."
    )

    ECHO "Installing Python..."
    powershell "%CD%\python-3.10.0.exe /quiet InstallAllUsers=0 PrependPath=1 Include_test=0 TargetDir=c:\Python\Python3100"
    setx path "%PATH%;C:\Python\Python3100\"
    set "path=%PATH%;C:\Python\Python3100\"

    timeout /t 30 /nobreak > nul
    ECHO "Python Installation completed."
) ELSE (
    ECHO.
    ECHO "Python Found"
    ECHO.
)

ECHO "Checking Conan Version"
conan --version 2>nul
if errorlevel 1 (
    ECHO.
    ECHO Error^: "Conan not installed" 
    ECHO.
    ECHO.
    ECHO "Installing Conan..."
    CALL cmd /k python -m pip install conan
) ELSE (
    ECHO.
    ECHO "Conan Found"
    ECHO.
)

ECHO "Checking Chocolatey Version"
powershell choco --version >nul
if errorlevel 1 (
    ECHO.
    ECHO Error^: "Chocolatey not installed" 
    ECHO.
    ECHO.
    ECHO "Downloading Chocolatey..."
    powershell -Command "& {Set-ExecutionPolicy Bypass -Scope Process -Force; [System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072; iex ((New-Object System.Net.WebClient).DownloadString('https://community.chocolatey.org/install.ps1'))}"
    ECHO "Chocolatey Install Completed."
) ELSE (
    ECHO.
    ECHO "Chocolatey Found"
    ECHO.
)

ECHO "Checking GCC Version"
CALL gcc --version >nul
if errorlevel 1 (
    ECHO.
    ECHO Error^: "GCC not installed" 
    ECHO.
    ECHO.
    powershell choco install gcc /quiet
    ECHO "GCC Installed Successfully"
) ELSE (
    ECHO.
    ECHO "GCC Found"
    ECHO.
)

ECHO "Checking Make Version"
CALL make --version >nul
if errorlevel 1 (
    ECHO.
    ECHO Error^: "Make not installed" 
    ECHO.
    ECHO.
    powershell choco install make /quiet
    ECHO "Make Installed Successfully"
) ELSE (
    ECHO.
    ECHO "Make Found"
    ECHO.
)

ECHO "Conan Dependency Installs Started"
CALL conan profile detect --force
CALL conan install . --build=missing --settings=build_type=Debug
ECHO "Conan Installed Dependencies Successfully"
cd build
CALL ./Debug/generators/conanbuild.bat
ECHO "CMake Build Started"
CALL cmake .. -G "MinGW Makefiles" -DCMAKE_TOOLCHAIN_FILE=Debug/generators/conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Debug
CALL cmake --build .
CALL ./Debug/generators/deactivate_conanbuild.bat
ECHO "CMake Built Successfully."
ECHO "Tests Started."
CALL "./tests/respondTest.exe"
ECHO "Full Build Finished"

cmd /k