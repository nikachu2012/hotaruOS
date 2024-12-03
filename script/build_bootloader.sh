#!/bin/sh

# edk2を用いてブートローダをビルドし
# ビルド後のファイルを実行フォルダにコピーするスクリプト
# edk2がビルドされていて、環境設定が終わっていることが前提
# 終わっていない場合はdocs/howToCompile.mdを参照

EDK2DIR="$HOME/workspace/edk2"

DIR=$(pwd)
cd $EDK2DIR
source $EDK2DIR/edksetup.sh
build

cd $DIR
cp $EDK2DIR/Build/hotaruBootloaderX64/DEBUG_CLANGPDB/X64/Loader.efi ./Loader.efi
