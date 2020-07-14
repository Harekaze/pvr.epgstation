# Harekaze2 for Kodi/XBMC
EPGStation PVR addon for Kodi

[![Build status: master](https://img.shields.io/travis/Harekaze/pvr.epgstation/master.svg?maxAge=259200&style=flat-square)](https://travis-ci.org/Harekaze/pvr.epgstation/)
[![AppVeyor](https://img.shields.io/appveyor/ci/mzyy94/pvr-epgstation.svg?maxAge=2592000&style=flat-square)](https://ci.appveyor.com/project/mzyy94/pvr-epgstation)
[![GitHub release](https://img.shields.io/github/release/Harekaze/pvr.epgstation.svg?maxAge=259200&style=flat-square)](https://github.com/Harekaze/pvr.epgstation/releases)
[![GitHub release downloads](https://img.shields.io/github/downloads/Harekaze/pvr.epgstation/total.svg?style=flat-square)](https://github.com/Harekaze/pvr.epgstation/releases)
[![GitHub issues](https://img.shields.io/github/issues/Harekaze/pvr.epgstation.svg?style=flat-square)](https://github.com/Harekaze/pvr.epgstation/issues)
[![GitHub stars](https://img.shields.io/github/stars/Harekaze/pvr.epgstation.svg?style=flat-square)](https://github.com/Harekaze/pvr.epgstation/stargazers)
[![GitHub license](https://img.shields.io/badge/license-GPLv3-orange.svg?style=flat-square)](https://raw.githubusercontent.com/Harekaze/pvr.epgstation/master/LICENSE)

![fanart](/template/pvr.epgstation/fanart.png)

## Supported environment

### Backend
- EPGStation [v1.7](https://github.com/l3tnun/EPGStation/tree/v1.7.0)

### Frontend
- Kodi **18.x** Leia
  + for macOS
  + for Android ARM
  + for Linux x64
  + for Raspberry Pi
  + for Windows
  + for iOS (beta)
  + for tvOS (beta)

#### Old versions

|  Tag       | Kodi             | Platforms
|------------|:-----------------|:--
| v0.x, v1.x | Kodi 15 Isengard | macOS/Linux/Raspberry Pi/(Windows)
| v2.x, v3.x | Kodi 16 Jarvis   | macOS/Linux/Raspberry Pi/Windows/(Android)
| v4.x       | Kodi 17 Krypton  | macOS/Linux/Raspberry Pi/Windows/Android/(iOS)
| v5.x       | Kodi 18 Leia     | macOS/Linux/Raspberry Pi/Windows/Android/(iOS)/(tvOS)

:warning: *No features backpors, No security backports* :warning:

## Latest release

[pvr.epgstation/releases](https://github.com/Harekaze/pvr.epgstation/releases)

## Building from source

### Linux / macOS
```sh
$ ./bootstrap
$ ./configure
$ make
$ make release
$ ls pvr.epgstation.zip
```
> TIPS: If a warning about AC_CHECK_HEADER_STDBOOL has occurred, install gnulib and execute bootstrap with
> AUTORECONF_FLAGS option with gnulib's m4 directory (e.g. `AUTORECONF_FLAGS=-I/usr/share/gnulib/m4 ./bootstrap`)

### Android ARM
*Android NDK is required.*

```sh
$ ndk-build APP_ABI=armeabi-v7a
$ ./jni/pack.sh APP_ABI=armeabi-v7a
$ ls pvr.epgstation.zip
```

### Windows

Requirements:
- Visual Studio 2019
- PowerShell v5

```powershell
> ./build.ps1
> ls ./pvr.epgstation.zip
```
> TIPS: If a powershell warning about Execution Policies has occurred, run `Set-ExecutionPolocy Unrestricted`
> with Administrator privileges. After building this project, run `Set-ExecutionPolocy RemoteSigned`, please.

> NOTE: PowerShell command 'Compress-Archive' creates broken zip file.
> Please unzip created archive yourself, and re-zip it with other compression tool.

### iOS
```sh
$ ./bootstrap
$ ./configure --host=arm-apple-darwin
$ make
$ ls pvr.epgstation.zip
```
> NOTE: iOS targeted package can't install to Kodi for iOS with zip installation.
> You should build Kodi.app including pvr.epgstation addon.

## Installing

1. Build or download the appropriate version for your platform.
2. Launch Kodi.
3. Navigate to System -> Add-ons -> Install from zip file
4. Select the zip file which you get in first step.

> NOTE: In some cases, Kodi for android installs addons into non-executable device. As a result, you need to do something more.
> See [wiki/android-installation](https://github.com/Harekaze/pvr.epgstation/wiki/android-installation).

## Configuration

See [wiki/configuration](https://github.com/Harekaze/pvr.epgstation/wiki/configuration)

## Contribution

See [wiki/contribution](https://github.com/Harekaze/pvr.epgstation/wiki/contribution)

## License

[GPLv3](LICENSE)
