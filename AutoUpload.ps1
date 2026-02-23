# ================= 程序單一執行檢查 (Instance Check) =================
# 確保電腦上只有一個 AutoUpload.ps1 在執行
$CurrentProcess = [System.Diagnostics.Process]::GetCurrentProcess()
$MyPath = $MyInvocation.MyCommand.Definition
$MyFileName = [System.IO.Path]::GetFileName($MyPath)

# 尋找所有正在執行的 PowerShell 或 PowerShell Core 程序
$OtherProcesses = Get-CimInstance Win32_Process -Filter "Name = 'powershell.exe' OR Name = 'pwsh.exe'" | 
    Where-Object { $_.CommandLine -like "*$MyFileName*" -and $_.ProcessId -ne $CurrentProcess.Id }

if ($OtherProcesses) {
    Write-Host "偵測到相同腳本正在執行，正在清理舊程序..." -ForegroundColor Yellow
    foreach ($P in $OtherProcesses) {
        try {
            Stop-Process -Id $P.ProcessId -Force -ErrorAction SilentlyContinue
            Write-Host "  -> 已終止舊 PID: $($P.ProcessId)" -ForegroundColor Gray
        } catch {
            Write-Host "  -> 無法終止 PID: $($P.ProcessId)" -ForegroundColor Red
        }
    }
}
# ====================================================================

# ================= 配置區域 (User Settings) =================
$SourceFolder = "D:\Desktop\自動化工具\TestFloder\TestFiles"

# [掃描設定]
$CheckInterval = 1
$Detectors    = @("Monitor1", "Monitor2", "PbWO4")

# Linux 設定 (NAS / chip03)
$LinuxUser = "yusiang"
$LinuxIP   = "chip03.phy.ncu.edu.tw"
$LinuxPath = "/data8/ZDC/EMCal/BTDataStreamTest/Simulated_Files"

# SSH 參數陣列 (修正 PS5.1 參數解析問題)
$SSHOptions = @(
    "-o", "BatchMode=yes",
    "-o", "StrictHostKeyChecking=no", 
    "-o", "ConnectTimeout=10", 
    "-o", "ConnectionAttempts=3",
    "-o", "ServerAliveInterval=5", 
    "-o", "ServerAliveCountMax=3"
)
# ==========================================================

if (!(Test-Path -Path $SourceFolder)) { 
    New-Item -ItemType Directory -Path $SourceFolder -Force | Out-Null
    Write-Host "已自動建立本地來源目錄: $SourceFolder" -ForegroundColor Gray
}

# 建立本地已上傳紀錄字典 (避免重複上傳)
$VerifiedCache = [System.Collections.Concurrent.ConcurrentDictionary[string,bool]]::new()

# MD5 輔助函數
function Get-FileHash-MD5 {
    param([string]$Path)
    try {
        if (Test-Path $Path) {
            return (Get-FileHash -Path $Path -Algorithm MD5).Hash.ToLower()
        }
    } catch {}
    return $null
}

# 檔案鎖定檢查 (避免上傳正在寫入中的檔案)
function Test-FileLock {
    param([string]$Path)
    try {
        $stream = [System.IO.File]::Open($Path, "Open", "Read", "None")
        $stream.Close()
        return $false 
    } catch {
        return $true 
    }
}

Write-Host "=============================================" -ForegroundColor Cyan
Write-Host " ?? ZDC 數據傳輸系統 v8.0 (Linux 優先模式)" -ForegroundColor Yellow
Write-Host " 來源: $SourceFolder"
Write-Host " 目標: $LinuxIP (SCP Only)"
Write-Host "=============================================" -ForegroundColor Cyan

while ($true) {
    Write-Host "Scanning files..." -ForegroundColor DarkGray
    
    # 1. 取得所有相關檔案
    $AllFiles = Get-ChildItem -Path $SourceFolder -File | Where-Object { 
        $Name = $_.Name
        ($Detectors | Where-Object { $Name -match $_ })
    }

    # 2. 依照 Run ID 分組
    $Groups = $AllFiles | Group-Object -Property { 
        $_.Name -replace "[-_]?(Monitor1|Monitor2|PbWO4).*", "" 
    } | Sort-Object Name 

    # 3. 過濾掉已完成的 Run
    $PendingGroups = New-Object System.Collections.Generic.List[Object]
    foreach ($Group in $Groups) {
        $GroupAllDone = $true
        foreach ($File in $Group.Group) {
            if (!$VerifiedCache.ContainsKey($File.Name)) { 
                $GroupAllDone = $false 
                break 
            }
        }
        if (!$GroupAllDone) { $PendingGroups.Add($Group) }
    }

    if ($PendingGroups.Count -eq 0) {
        Write-Host "All runs up to date. Waiting..." -ForegroundColor Gray
    } else {
        Write-Host "Found $($PendingGroups.Count) pending Runs." -ForegroundColor Yellow
    }

    # ==========================================
    # 核心處理迴圈 (僅處理 Linux SCP)
    # ==========================================
    foreach ($Group in $PendingGroups) {
        $RunID = $Group.Name
        $Files = $Group.Group
        
        $TimeStr = Get-Date -Format "HH:mm:ss"
        Write-Host "[$TimeStr] [Processing] $RunID" -ForegroundColor Cyan

        foreach ($File in $Files) {
            if ($VerifiedCache.ContainsKey($File.Name)) { continue } 
            
            if (Test-FileLock $File.FullName) {
                Write-Host "  -> 檔案鎖定中 (DAQ寫入中): $($File.Name)" -ForegroundColor DarkGray
                continue
            }

            $LocalHash = Get-FileHash-MD5 $File.FullName
            if ([string]::IsNullOrEmpty($LocalHash)) { continue }

            # --- 執行 Linux SCP 同步 ---
            $LinuxOK = $false
            $LinuxRemoteFilePath = "$LinuxPath/$($File.Name)"
            $CheckCmd = "md5sum '$LinuxRemoteFilePath' 2>/dev/null"
            
            try {
                # 檢查遠端是否已有相同檔案
                $Res = ssh $SSHOptions "${LinuxUser}@${LinuxIP}" $CheckCmd
                $ExistingLinuxHash = if ($Res) { (-split $Res)[0].Trim().ToLower() } else { $null }

                if ($ExistingLinuxHash -eq $LocalHash) {
                    $LinuxOK = $true
                } else {
                    Write-Host "  -> 上傳中: $($File.Name)..." -NoNewline
                    
                    # 執行 SCP 指令
                    $ScpCmd = "scp $SSHOptions `"$($File.FullName)`" ${LinuxUser}@${LinuxIP}:`"$LinuxRemoteFilePath`""
                    Invoke-Expression $ScpCmd 2>&1 | Out-Null
                    
                    if ($LASTEXITCODE -eq 0) {
                        # 上傳後再次校驗 MD5
                        $Res2 = ssh $SSHOptions "${LinuxUser}@${LinuxIP}" $CheckCmd
                        $NewLinuxHash = if ($Res2) { (-split $Res2)[0].Trim().ToLower() } else { $null }
                        
                        if ($NewLinuxHash -eq $LocalHash) { 
                            $LinuxOK = $true
                            Write-Host " [成功]" -ForegroundColor Green
                        } else {
                            Write-Host " [校驗失敗]" -ForegroundColor Red
                        }
                    } else {
                        Write-Host " [傳輸失敗]" -ForegroundColor Red
                    }
                }
            } catch {
                Write-Host "  -> SSH 連線錯誤: $_" -ForegroundColor Red
            }

            # 結算
            if ($LinuxOK) {
                $VerifiedCache.TryAdd($File.Name, $true) | Out-Null
            }
        } 
    } 

    # 倒數計時
    for ($i = $CheckInterval; $i -gt 0; $i--) {
        Write-Host "Waiting... Next scan in: $i s      `r" -NoNewline -ForegroundColor DarkGray
        Start-Sleep -Seconds 1
    }
    Write-Host "                                      `r" -NoNewline
}