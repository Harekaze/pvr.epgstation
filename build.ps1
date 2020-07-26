Param([ValidateSet("x86","x64")][String] $Platform = "x86")
$ErrorActionPreference = "Stop"
& ${env:ProgramFiles(x86)}\Microsoft` Visual` Studio\2019\Enterprise\MSBuild\Current\Bin\MSBuild.exe .\VS2019\pvr.epgstation.sln /t:Clean,Build /p:Configuration=Release /p:Platform=$Platform
New-Item dist -Force -ItemType Directory
Copy-Item -r .\template\pvr.epgstation .\dist\
Copy-Item .\ChangeLog.txt .\dist\pvr.epgstation
Copy-Item .\LICENSE .\dist\pvr.epgstation
Copy-Item .\VS2019\Release\pvr.epgstation*.dll .\dist\pvr.epgstation
Set-Location .\dist
Compress-Archive -Force -CompressionLevel NoCompression -Path .\pvr.epgstation -DestinationPath ..\pvr.epgstation.zip
