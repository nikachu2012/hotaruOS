# qemu_run.sh
# 以下の操作を行ったイメージファイルを作りQEMUで実行します
# - 第１引数に指定されたパスをEFI/BOOT/BOOTX64.EFIにコピー
# - 第２引数に指定されたディレクトリの中身をすべてコピー

if [ $# -lt 2 ]; then
    echo "Usage: $0 <Bootloader path> <pack directory>"
    exit 1
fi

IMG_SIZE="100M"
IMG_FILENAME="hotaruOS_img.img"
IMG_MOUNTPOINT="mnt"

qemu-img create -f raw $IMG_FILENAME $IMG_SIZE
mkfs.fat -n "HOTARU_OS" -s 2 -f 2 -R 32 -F 32 $IMG_FILENAME
mkdir -p mnt 

if [[ $(uname) =~ ^Darwin* ]]; then
    # Macの場合
    hdiutil attach -mountpoint $IMG_MOUNTPOINT $IMG_FILENAME
else
    sudo mount -o loop $IMG_FILENAME $IMG_MOUNTPOINT
fi

sudo mkdir -p mnt/efi/boot/
sudo cp $1 mnt/efi/boot/bootx64.efi
sudo cp -rL $2/* mnt/

if [[ $(uname) =~ ^Darwin* ]]; then
    # Macの場合
    hdiutil detach $IMG_MOUNTPOINT
else
    sudo umount $IMG_MOUNTPOINT
fi

sudo qemu-system-x86_64 -bios OVMF.fd -drive format=raw,file=./$IMG_FILENAME -monitor stdio -device qemu-xhci -device usb-mouse -device usb-kbd

exit 0

