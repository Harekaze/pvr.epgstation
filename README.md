# Harekaze2 for Kodi/XBMC
EPGStation PVR addon for Kodi

[![Build status](https://img.shields.io/github/workflow/status/Harekaze/pvr.epgstation/CI?logo=github&style=for-the-badge)](https://travis-ci.org/Harekaze/pvr.epgstation/)
[![GitHub release](https://img.shields.io/github/release/Harekaze/pvr.epgstation.svg?maxAge=259200&style=for-the-badge)](https://github.com/Harekaze/pvr.epgstation/releases)
[![GitHub release downloads](https://img.shields.io/github/downloads/Harekaze/pvr.epgstation/total.svg?style=for-the-badge)](https://github.com/Harekaze/pvr.epgstation/releases)
[![GitHub issues](https://img.shields.io/github/issues/Harekaze/pvr.epgstation.svg?style=for-the-badge)](https://github.com/Harekaze/pvr.epgstation/issues)
[![GitHub stars](https://img.shields.io/github/stars/Harekaze/pvr.epgstation.svg?style=for-the-badge)](https://github.com/Harekaze/pvr.epgstation/stargazers)
[![GitHub license](https://img.shields.io/github/license/Harekaze/pvr.epgstation.svg?style=for-the-badge)](https://raw.githubusercontent.com/Harekaze/pvr.epgstation/master/LICENSE)

![fanart](./template/pvr.epgstation/fanart.png)

## Supported environment

### Backend
- EPGStation [v1.7](https://github.com/l3tnun/EPGStation/tree/v1.7.0)

### Frontend
- Kodi **18.7** Leia
  + for macOS (x86_64)
  + for Raspberry Pi OS (armv7)
  + for Windows 10 (32 bit)

It might work on other platforms, but it hasn't been tested.

## Latest release

[pvr.epgstation/releases](https://github.com/Harekaze/pvr.epgstation/releases)

## Building from source

### macOS/Raspberry Pi OS
```sh
$ ./bootstrap
$ ./configure
$ make
$ make release
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

## Installation

1. Build or download the appropriate version for your platform.
2. Launch Kodi.
3. Navigate to System -> Add-ons -> Install from zip file
4. Select the zip file which you get in first step.

## Configuration

See [wiki/configuration](https://github.com/Harekaze/pvr.epgstation/wiki/configuration)

## Contribution

See [wiki/contribution](https://github.com/Harekaze/pvr.epgstation/wiki/contribution)

## License

[GPLv3](LICENSE)
