#!/bin/bash

# 現在の日付と時刻を取得（年を省く）
datetime=$(date "+%m%d_%H%M%S")

# 保存先のディレクトリパス
save_dir=~/Desktop/user_prog_asm/${datetime}

# ディレクトリを作成
mkdir -p ${save_dir}

# ASMファイルの内容を保存
cp ./obj/user_yama/printyama.asm ${save_dir}/printyama.asm
cp ./obj/user_libc/printlibc.asm ${save_dir}/printlibc.asm

# objdumpの結果を保存
objdump -d ./obj/user_yama/printyama.o > ${save_dir}/printyama_objdump.txt
objdump -d ./obj/user_libc/printlibc.o > ${save_dir}/printlibc_objdump.txt

echo "Files have been saved to ${save_dir}"
