#!/bin/sh

# edk2を用いてブートローダをビルドするスクリプト
# edk2がビルドされていて、環境設定が終わっていることが前提
# 終わっていない場合はdocs/howToCompile.mdを参照

EDK2DIR="$HOME/workspace/edk2"

cd $EDK2DIR
source $EDK2DIR/edksetup.sh
build
