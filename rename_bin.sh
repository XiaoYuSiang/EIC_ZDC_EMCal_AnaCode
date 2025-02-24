#!/bin/bash

# 設定目標資料夾
TARGET_DIR="$1"

# 檢查是否提供目標資料夾
if [[ -z "$TARGET_DIR" ]]; then
    echo "使用方法: $0 <目標資料夾>"
    exit 1
fi

# 確保目標資料夾存在
if [[ ! -d "$TARGET_DIR" ]]; then
    echo "錯誤: 目標資料夾不存在"
    exit 1
fi

# 進入目標資料夾
cd "$TARGET_DIR" || exit

# 迭代處理所有 .bin 檔案
for file in *.bin; do
    # 確保是檔案
    [[ -f "$file" ]] || continue

    # 取代非法字元 (、)、- 為 _
    new_name=$(echo "$file" | sed 's/[()\-]/_/g')

    # 確保新名稱不與舊名稱相同
    if [[ "$file" != "$new_name" ]]; then
        mv "$file" "$new_name"
        echo "重命名: $file -> $new_name"
    fi
done

echo "重命名完成"


# Usage:
	# ./rename_bin.sh /path/to/target_directory
