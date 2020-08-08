# Harekaze2 for Kodi/XBMC
Kodi のための EPGStation PVR アドオン

[![Build status](https://img.shields.io/github/workflow/status/Harekaze/pvr.epgstation/CI?logo=github&style=for-the-badge)](https://travis-ci.org/Harekaze/pvr.epgstation/)
[![GitHub release](https://img.shields.io/github/release/Harekaze/pvr.epgstation.svg?maxAge=259200&style=for-the-badge)](https://github.com/Harekaze/pvr.epgstation/releases)
[![GitHub release downloads](https://img.shields.io/github/downloads/Harekaze/pvr.epgstation/total.svg?style=for-the-badge)](https://github.com/Harekaze/pvr.epgstation/releases)
[![GitHub issues](https://img.shields.io/github/issues/Harekaze/pvr.epgstation.svg?style=for-the-badge)](https://github.com/Harekaze/pvr.epgstation/issues)
[![GitHub stars](https://img.shields.io/github/stars/Harekaze/pvr.epgstation.svg?style=for-the-badge)](https://github.com/Harekaze/pvr.epgstation/stargazers)
[![GitHub license](https://img.shields.io/github/license/Harekaze/pvr.epgstation.svg?style=for-the-badge)](https://raw.githubusercontent.com/Harekaze/pvr.epgstation/master/LICENSE)

![fanart](./template/pvr.epgstation/fanart.png)

## サポート環境

### バックエンド
- EPGStation [v1.7](https://github.com/l3tnun/EPGStation/tree/v1.7.0)

### フロントエンド
- Kodi **18.7** Leia
  + for macOS (x86_64)
  + for Raspberry Pi OS (armv7)
  + for Windows 10 (64 bit/Microsoft Store)

他の環境でも動くかもしれませんが、動作確認してません。

## 最新のリリース

[pvr.epgstation/releases](https://github.com/Harekaze/pvr.epgstation/releases)

## ソースコードからのビルド方法

### macOS/Raspberry Pi OS
```sh
$ ./bootstrap
$ ./configure
$ make
$ make release
$ ls pvr.epgstation.zip
```

### Windows

必要なツール:
- Visual Studio 2019
- PowerShell v5

```powershell
> ./build.ps1
> ls ./pvr.epgstation.zip
```

## インストール方法

1. ソースコードからビルドするか、最適なバージョンのリリースをダウンロードする
2. Kodiを起動する
3. 画面を操作し、System -> Add-ons -> Install from zip file へと行く
4. 画面でzipファイルを選択してインストールする

## 設定

[wiki/configuration](https://github.com/Harekaze/pvr.epgstation/wiki/configuration) に書いてあります

## 貢献

[wiki/contribution](https://github.com/Harekaze/pvr.epgstation/wiki/contribution) に書いてあります

## ライセンス

[GPLv3](LICENSE)
