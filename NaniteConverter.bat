@echo off
setlocal enabledelayedexpansion

:: ================================
:: CONFIGURACIÓN DE RUTAS
:: ================================

:: Ruta oficial de Unreal Engine instalado
set "ENGINE_ROOT=C:\Program Files\Epic Games\UE_5.4"

:: Rutas completas a ejecutables necesarios
set "UE_PATH=%ENGINE_ROOT%\Engine\Binaries\Win64\UnrealEditor.exe"
set "UNREAL_PAK=%ENGINE_ROOT%\Engine\Binaries\Win64\UnrealPak.exe"

:: Nombre del proyecto (.uproject en la misma carpeta que el .bat)
set "UPROJECT=NaniteConverter.uproject"
set "STARTUP_MAP=/Game/_StartupMap._StartupMap"

:: Ir a la carpeta del .bat
pushd %~dp0

:: ================================
:: VALIDACIONES INICIALES
:: ================================
if not exist "%UE_PATH%" (
    echo [ERROR] No se encontró UnrealEditor.exe en "%UE_PATH%"
    popd
    exit /b 1
)

if "%~1"=="" (
    echo Uso: NaniteConverter.bat "ruta\al\modelo.fbx"
    popd
    exit /b 1
)

for %%F in ("%~1") do (
    set FBXNAME=%%~nF
)

:: Sanitizar nombre (reemplazar espacios por guion bajo)
set FBXNAME_SANITIZED=%FBXNAME: =_%

set STATIC_MESH_PATH=/Game/FBX_Imported/%FBXNAME_SANITIZED%/%FBXNAME_SANITIZED%
set TEMP_MAP_NAME=_StartupMap_Temp_%FBXNAME_SANITIZED%
set TEMP_MAP=/Game/%TEMP_MAP_NAME%

:: ================================
:: LOGS
:: ================================
set LOG_DIR=%~dp0Log\%FBXNAME_SANITIZED%
mkdir "%LOG_DIR%" 2>nul
set LOG_IMPORT=%LOG_DIR%\Import.log
set LOG_COOK=%LOG_DIR%\Cook.log
set LOG_ERROR=%LOG_DIR%\Error.log

echo ===============================================
echo Importando y convirtiendo a Nanite: %FBXNAME_SANITIZED%
echo ===============================================

:: ================================
:: IMPORTAR FBX A NANITE
:: ================================
"%UE_PATH%" "%~dp0%UPROJECT%" -run=FBXToNaniteCommandlet -FBXPath="%~1" -log -abslog="%LOG_IMPORT%"

:: ================================
:: COCINAR STATIC MESH EN MAPA TEMPORAL
:: ================================
echo Cocinando el static mesh con el mapa temporal: %TEMP_MAP% ...
"%UE_PATH%" "%~dp0%UPROJECT%" -run=cook -targetplatform=Windows -map=%TEMP_MAP% -compressed -log -abslog="%LOG_COOK%"

:: ================================
:: DETECTAR Y COPIAR CONTENIDO
:: ================================
set COOKED_DIR=%~dp0Saved\Cooked\Windows\NaniteConverter\Content\FBX_Imported\%FBXNAME_SANITIZED%
if not exist "%COOKED_DIR%" (
    echo [ERROR] No se encontraron assets cocinados para %FBXNAME_SANITIZED% >> "%LOG_ERROR%"
    echo Verifica el log: "%LOG_COOK%"
    popd
    exit /b 1
)

set OUTPUT_DIR=%~dp0Output\%FBXNAME_SANITIZED%
mkdir "%OUTPUT_DIR%" 2>nul

echo Copiando archivos cocinados...
xcopy /E /I /Y "%COOKED_DIR%\*" "%OUTPUT_DIR%\Cooked" >nul

:: ================================
:: GENERAR filelist.txt PARA PAK
:: ================================
set LIST_FILE=%OUTPUT_DIR%\filelist.txt
if exist "%LIST_FILE%" del "%LIST_FILE%" >nul

for /R "%OUTPUT_DIR%\Cooked" %%F in (*.uasset *.uexp *.ubulk *.umap) do (
    set "FILE=%%F"
    set "REL=%%F"
    set "REL=!REL:%OUTPUT_DIR%\Cooked\=!"
    set "LOGICAL_PATH=/Game/FBX_Imported/%FBXNAME_SANITIZED%/!REL!"
    set "LOGICAL_PATH=!LOGICAL_PATH:\=/!"
    echo !FILE! !LOGICAL_PATH! >> "%LIST_FILE%"
)

:: ================================
:: GENERAR .PAK
:: ================================
set PAK_FILE=%OUTPUT_DIR%\%FBXNAME_SANITIZED%.pak
echo Generando .pak...
"%UNREAL_PAK%" "%PAK_FILE%" -create="%LIST_FILE%" -compress >> "%LOG_DIR%\Pak.log" 2>>"%LOG_ERROR%"

if exist "%PAK_FILE%" (
    echo [OK] Pak generado correctamente: %PAK_FILE%
) else (
    echo [ERROR] No se generó el archivo .pak >> "%LOG_ERROR%"
    echo Revisa el log en: %LOG_DIR%\Pak.log
    popd
    exit /b 1
)

:: ================================
:: LIMPIEZA
:: ================================
echo Limpiando carpeta Saved...
rmdir /S /Q "%~dp0Saved" >nul 2>&1

echo Limpiando carpeta Content/FBX_Imported/%FBXNAME_SANITIZED%...
rmdir /S /Q "%~dp0Content\FBX_Imported\%FBXNAME_SANITIZED%" >nul 2>&1

echo Limpiando temporales...
rmdir /S /Q "%OUTPUT_DIR%\Cooked" >nul 2>&1
del "%LIST_FILE%" >nul 2>&1

:: ================================
:: LIMPIAR MAPA TEMPORAL
:: ================================
set TEMP_UMAP_PATH=%~dp0Content\%TEMP_MAP_NAME%.umap
if exist "%TEMP_UMAP_PATH%" (
    echo Eliminando mapa temporal: %TEMP_UMAP_PATH%
    del /Q "%TEMP_UMAP_PATH%" >nul 2>&1
)

set TEMP_COOKED_DIR=%~dp0Saved\Cooked\Windows\NaniteConverter\Content\%TEMP_MAP_NAME%
set TEMP_INTERMEDIATE_DIR=%~dp0Intermediate\%TEMP_MAP_NAME%

if exist "%TEMP_COOKED_DIR%" (
    echo Eliminando temporales cocinados del mapa...
    rmdir /S /Q "%TEMP_COOKED_DIR%" >nul 2>&1
)

if exist "%TEMP_INTERMEDIATE_DIR%" (
    echo Eliminando archivos intermedios del mapa...
    rmdir /S /Q "%TEMP_INTERMEDIATE_DIR%" >nul 2>&1
)

echo ===============================================
echo ¡Conversión y empaquetado completados!
echo Archivo final: %PAK_FILE%
echo ===============================================

popd
endlocal