# コンパイル方法
Written by nikachu2012 

EDK2は`~/workspace/edk2`に、標準ライブラリは`~/workspace/x86_64-elf`に配置すると、各種シェルスクリプトを変更無しで利用できるのでおすすめ

## ソースコードの取得
```
git clone https://github.com/nikachu2012/hotaruOS
```

## 依存関係のインストール

### ライブラリのインストール
Arch Linux(pacman+AUR)ならこれをインストール
```bash
$ yay -S base-devel llvm lld clang nasm acpica uuid qemu-full xorg-xauth unzip dosfstools
```

Ubuntu(apt)ならこれをインストール(未検証)
```bash
$ sudo apt install build-essential llvm lld clang nasm acpica-tools uuid-dev qemu-system-x86 qemu-utils xauth unzip python3-distutils ca-certificates
```

### EDK2の準備
EDK2をクローンし、ビルドする。
```bash
$ git clone --recursive https://github.com/tianocore/edk2.git
```
```bash
$ cd /path/to/edk2/BaseTools/Source/C
$ make
```

> [!NOTE]
> macOS上でビルドを行う場合、macOSのバージョンによって以下のようなエラーでリンクに失敗します。  
> ```
> ld: archive member '/' not a mach-o file in '../libs/libCommon.a'
> ```
>
> この場合は、homebrewからインストールされたGNU binutilsの削除で治るかもしれません。
>
> ```bash
> $ brew uninstall binutils
> ```
> https://stackoverflow.com/questions/77444892/xcode-15-0-1-macos-sonoma-clang-archive-or-linking-issue

### 標準ライブラリのダウンロード
以下のコマンドでビルド済みの標準ライブラリをダウンロードし、展開する
```bash
$ wget https://github.com/uchan-nos/mikanos-build/releases/download/v2.0/x86_64-elf.tar.gz
$ tar -xzf x86_64-elf.tar.gz
```

また、`/path/to/hotaruOS/tools/buildenv.sh`の`BASEDIR`と`EDK2DIR`のパスを正しいパスに書き換える。

例えば標準ライブラリが`~/workspace/x86_64-elf/`、EDK2が`~/workspace/edk2/`にある場合は以下のようになる。
```
BASEDIR="$HOME/workspace/x86_64-elf"
EDK2DIR="$HOME/workspace/edk2"
```


## ブートローダのビルド

EDK2のディレクトリにブートローダのディレクトリをリンクする。
```bash
$ cd /path/to/edk2
$ ln -s /path/to/hotaruOS/bootloader ./hotaruBootloaderPkg
```

`edksetup.sh` で環境変数をセットする。
```bash
$ source edksetup.sh
```

`Conf/target.txt` が作成されるので編集する。エディタはお好きなもので
```bash
$ nano Conf/target.txt
```

以下の項目を以下のように変更する。
| key             | value                                       |
| --------------- | ------------------------------------------- |
| ACTIVE PLATFORM | hotaruBootloaderPkg/hotaruBootloaderPkg.dsc |
| TARGET          | DEBUG                                       |
| TARGET_ARCH     | X64                                         |
| TOOL_CHAIN_TAG  | CLANGPDB                                    |

これでブートローダをビルドする準備が整った。
```bash
$ build
```

以下のように`Done`で完了していればビルド成功している。
```
- Done -
Build end time: 09:41:31, Nov.30 2024
Build total time: 00:00:01
```

ビルド後のEFIファイルは以下のパスに生成される。
```
/path/to/edk2/Build/hotaruBootloaderPkg/DEBUG_CLANGPDB/X64/Loader.efi
```

## OSカーネルのビルド
以下のコマンドで実行できる。
```bash
$ cd /path/to/hotaruOS/kernel
$ source ../script/buildenv.sh
$ make
```

`/path/to/hotaruOS/kernel`の中に`kernel.elf`が生成される。

## シェルスクリプトのパスを指定
付属しているシェルスクリプトを使用する場合は、パスの設定が必要。
EDK2は`~/workspace/edk2`に、標準ライブラリは`~/workspace/x86_64-elf`に配置していない場合は以下の操作が必要になる。

- `script/build_bootloader.sh`の`EDK2DIR`変数を正しいパスに設定する。
