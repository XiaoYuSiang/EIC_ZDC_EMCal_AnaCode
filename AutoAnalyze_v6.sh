#!/bin/bash

# ================= 配置區域 (User Settings) =================
WATCH_DIR="/data8/ZDC/EMCal/BTDataStreamTest/Simulated_Files"
TAG_MAIN="PbWO4"
TAG_MON1="Monitor1"
TAG_MON2="Monitor2"
EXT=".bin"

ROOT_MACRO="Read.C"
LOG_FILE="processed_history.log"
RCLONE_REMOTE="EIC_ZDC"

# 雲端路徑設定
DRIVE_RESULT="TestAutoUploadSystem/FastResult"
DRIVE_RAW_DATA="TestAutoUploadSystem/Data"

MAX_JOBS=36         # 提升至 36 並行任務
CHECK_INTERVAL=2    # 掃描間隔
# ==========================================================

touch "$LOG_FILE"
# 顏色設定
GREEN='\033[0;32m'
CYAN='\033[0;36m'
YELLOW='\033[1;33m'
MAGENTA='\033[0;35m'
RED='\033[0;31m'
NC='\033[0m'

mkdir -p "$WATCH_DIR"

function is_file_busy() {
    if lsof "$1" > /dev/null 2>&1; then return 0; else return 1; fi
}

# 執行 Rclone 上傳結果 (HTML)
function upload_result() {
    local html_path="$1"
    local run_id="$2"
    local log_dest="$3"
    local h_name=$(basename "$html_path")

    echo -e "${CYAN}[結果上傳]${NC} 同步 $h_name..."
    rclone copy "$WATCH_DIR/" "${RCLONE_REMOTE}:${DRIVE_RESULT}/" --include "$h_name" -L --update >> "$log_dest" 2>&1
    
    if [ $? -eq 0 ]; then
        [[ "$run_id" != "SKIP_LOG" ]] && echo "$run_id" >> "$LOG_FILE"
        echo -e "${GREEN}[成功]${NC} $h_name 已存至 FastResult。"
        return 0
    fi
    return 1
}

# [新增] 執行 Rclone 上傳原始數據 (.bin)
function upload_raw_data() {
    local file_path="$1"
    local f_name=$(basename "$file_path")
    
    # 這裡使用背景上傳，不佔用腳本主線程
    (
        echo -e "${MAGENTA}[背景備份]${NC} 正在上傳原始檔: $f_name"
        rclone copy "$WATCH_DIR/" "${RCLONE_REMOTE}:${DRIVE_RAW_DATA}/" --include "$f_name" --update > /dev/null 2>&1
        if [ $? -eq 0 ]; then
            echo -e "${MAGENTA}[備份完成]${NC} $f_name 已存至 Data。"
        fi
    ) &
}

echo -e "${CYAN}=============================================${NC}"
echo -e "${CYAN}    ZDC 全自動分析與異地備份系統 v12         ${NC}"
echo -e " 分析並行上限: $MAX_JOBS"
echo -e " 原始數據備份: 啟用 (背景異步上傳)"
echo -e "${CYAN}=============================================${NC}"

while true; do
    # --- 階段 1: 處理分析與原始檔上傳 ---
    for file_main in $(find "$WATCH_DIR" -maxdepth 1 -name "*${TAG_MAIN}${EXT}" | sort); do
        filename=$(basename "$file_main")
        prefix=${filename%${TAG_MAIN}${EXT}}
        lock_file="${WATCH_DIR}/${prefix}.lock"

        # 若 LOG 已紀錄代表此 Run 已完成分析與所有同步
        if grep -q "$prefix" "$LOG_FILE"; then continue; fi
        if [ -f "$lock_file" ]; then continue; fi

        path_main="$file_main"
        path_mon1="${WATCH_DIR}/${prefix}${TAG_MON1}${EXT}"
        path_mon2="${WATCH_DIR}/${prefix}${TAG_MON2}${EXT}"

        if [[ -f "$path_mon1" && -f "$path_mon2" ]]; then
            # 確保 SCP 寫入完成
            if is_file_busy "$path_main" || is_file_busy "$path_mon1" || is_file_busy "$path_mon2"; then continue; fi
            
            # 並行控制
            if [ $(jobs -r | grep -v "rclone" | wc -l) -ge "$MAX_JOBS" ]; then break; fi

            touch "$lock_file"
            echo -e "${GREEN}[分析啟動]${NC} $prefix"
            
            (
                run_log="${WATCH_DIR}/${prefix}_process.log"
                
                # 1. 執行 ROOT 分析
                root -l -b -q "${ROOT_MACRO}(\"${path_main}\",\"${path_main}\",\"${path_mon1}\",\"${path_mon2}\",\"\",31)" >> "$run_log" 2>&1
                
                if [ $? -eq 0 ]; then
                    # 2. 分析成功後，啟動原始檔案的背景備份 (不卡住主進程)
                    upload_raw_data "$path_main"
                    upload_raw_data "$path_mon1"
                    upload_raw_data "$path_mon2"

                    # 3. 搜尋並上傳結果 HTML
                    run_num=$(echo "$prefix" | grep -o "Run[0-9]\+")
                    result_html=$(find "$WATCH_DIR" -name "RunDisplay_${run_num}.html" | head -n 1)
                    
                    if [ -n "$result_html" ]; then
                        upload_result "$result_html" "$prefix" "$run_log"
                    fi
                else
                    echo -e "${RED}[失敗]${NC} $prefix 分析出錯。"
                fi
                rm -f "$lock_file"
            ) & 
            sleep 0.2
        fi
    done

    # --- 階段 2: 補傳檢查 (略) ---
    # 此處保留 v11 的補傳邏輯...

    echo -ne "${NC}監控中... 活動分析: $(jobs -r | grep -v "rclone" | wc -l) | 備份中: $(jobs -r | grep "rclone" | wc -l)  \r"
    sleep "$CHECK_INTERVAL"
done