#include <TSystem.h>
#include <TString.h>
#include <iostream>
#include <fstream>

using namespace std;

/**
 * ExportToMHTML
 * 將生成的 test.html 及其圖片打包成單一檔案 (支援虛擬路徑以相容切換模式)
 * @param htmlPath 原始 HTML 的路徑
 * @param targetName 目標檔案名稱
 */
void ExportToMHTML(
    string htmlPath = "/data8/ZDC/EMCal/PbWO4SiPM/AnaCode1/Save/EScanTypicalRuns/Run2013_796MeV_HV17_VF650_650_x4_Pos145mm_-346mm_0mm_232554.122PbWO4/RunDisplay.html",
    string targetName = "/data8/ZDC/EMCal/PbWO4SiPM/AnaCode1/Save/EScanTypicalRuns/Run2013_796MeV_HV17_VF650_650_x4_Pos145mm_-346mm_0mm_232554.122PbWO4/RunDisplayMonofile.html"
) {
    
    string SavePath = htmlPath.substr(0,htmlPath.rfind("/")+1);
    if (gSystem->AccessPathName(htmlPath.c_str())) {
        cout << "錯誤：找不到來源檔案 " << htmlPath << endl;
        return;
    }

    // --- 方法：Python 數據映射與虛擬路徑 Hook 方案 ---
    cout << "正在掃描資源並生成全內嵌 HTML (優化模式切換與即時預覽)..." << endl;
    
    string pyScript = SavePath+"packer_v4.py";
    ofstream fout(pyScript);
    fout << "import base64, os, json, re" << endl;
    fout << "html_file = r'" << htmlPath << "'" << endl;
    fout << "out_file = r'" << TString(targetName).ReplaceAll(".mhtml", ".self_contained.html") << "'" << endl;
    fout << "base_dir = os.path.dirname(os.path.abspath(html_file))" << endl;
    fout << "" << endl;
    fout << "img_map = {}" << endl;
    // --- 新增：讀取 RunInfo.dat 內容 ---
    fout << "" << endl;
    fout << "for root, dirs, files in os.walk(base_dir):" << endl;
    fout << "    for file in files:" << endl;
    fout << "        if file.lower().endswith('.gif'):" << endl;
    fout << "            full_path = os.path.join(root, file)" << endl;
    fout << "            rel_path = './' + os.path.relpath(full_path, base_dir).replace('\\\\', '/')" << endl;
    fout << "            with open(full_path, 'rb') as f:" << endl;
    fout << "                b64 = base64.b64encode(f.read()).decode()" << endl;
    fout << "            img_map[rel_path] = f'data:image/gif;base64,{b64}'" << endl;
    fout << "" << endl;
    fout << "with open(html_file, 'r', encoding='utf-8') as f: content = f.read()" << endl;
    fout << "" << endl;
    fout << "# 替換指定的 .dat 標籤為實際內容" << endl;
    ifstream in((SavePath + "The_Run_Infos_file.dat").data());
    string datContent = "";
    if (in.is_open()) {
        string line;
        while (getline(in, line)) {
            // 將內容累加，並把換行符號轉成 HTML 的 <br> 標籤
            datContent += line + "<br>"; 
        }
        in.close();
    } else {
        datContent = "The_Run_Infos_file.dat not found";
    }
    cout<<datContent<<endl;
    fout << "content = content.replace('The_Run_Infos_file', \""<<datContent<<"\")" << endl;
    fout << "print(\""<<datContent<<"\")" << endl;
    fout << "" << endl;
    fout << "# 1. 植入虛擬路徑攔截器，讓 img.src 讀取時仍回傳路徑而非 Base64" << endl;
    fout << "hook_js = f'''" << endl;
    fout << "<script>" << endl;
    fout << "window._IMG_MAP = {json.dumps(img_map)};" << endl;
    fout << "(function() {{" << endl;
    fout << "    const imgProto = HTMLImageElement.prototype;" << endl;
    fout << "    const descriptor = Object.getOwnPropertyDescriptor(imgProto, 'src');" << endl;
    fout << "    const virtualSrcMap = new WeakMap();" << endl;
    fout << "" << endl;
    fout << "    Object.defineProperty(imgProto, 'src', {{" << endl;
    fout << "        get: function() {{ " << endl;
    fout << "            return virtualSrcMap.get(this) || descriptor.get.call(this); " << endl;
    fout << "        }}," << endl;
    fout << "        set: function(val) {{" << endl;
    fout << "            if (typeof val !== 'string' || val.startsWith('data:')) {{" << endl;
    fout << "                descriptor.set.call(this, val); return;" << endl;
    fout << "            }}" << endl;
    fout << "            virtualSrcMap.set(this, val); // 儲存虛擬路徑以供 JS 判斷" << endl;
    fout << "            let key = val;" << endl;
    fout << "            if (val.includes('/')) {{" << endl;
    fout << "                const parts = val.split('/');" << endl;
    fout << "                key = './' + parts.slice(-2).join('/');" << endl;
    fout << "            }}" << endl;
    fout << "            const b64 = window._IMG_MAP[key] || window._IMG_MAP['./' + key] || window._IMG_MAP[val];" << endl;
    fout << "            descriptor.set.call(this, b64 || val);" << endl;
    fout << "        }}," << endl;
    fout << "        configurable: true" << endl;
    fout << "    }});" << endl;
    fout << "" << endl;
    fout << "    // 確保頁面載入後自動刷一次所有圖片" << endl;
    fout << "    window.addEventListener('DOMContentLoaded', () => {{" << endl;
    fout << "        document.querySelectorAll('img').forEach(img => {{" << endl;
    fout << "            const s = img.getAttribute('src');" << endl;
    fout << "            if(s) img.src = s;" << endl;
    fout << "        }});" << endl;
    fout << "    }});" << endl;
    fout << "}})();" << endl;
    fout << "</script>" << endl;
    fout << "'''" << endl;
    fout << "" << endl;
    fout << "if '<head>' in content:" << endl;
    fout << "    new_content = content.replace('<head>', '<head>' + hook_js)" << endl;
    fout << "else:" << endl;
    fout << "    new_content = hook_js + content" << endl;
    fout << "" << endl;
    fout << "# 2. 靜態替換：處理 HTML 中已存在的靜態標籤" << endl;
    fout << "for rel_p, b64_d in img_map.items():" << endl;
    fout << "    new_content = new_content.replace(f'src=\"{rel_p}\"', f'src=\"{b64_d}\"')" << endl;
    fout << "" << endl;
    fout << "with open(out_file, 'w', encoding='utf-8') as f: f.write(new_content)" << endl;
    fout << "print(f'成功打包並修復模式切換功能')" << endl;
    fout.close();

    // 執行 Python 腳本
    TString runCmd = "python3 " + TString(pyScript);
    int status = gSystem->Exec(runCmd.Data());

    // 清理臨時檔案
    gSystem->Exec(("rm " + pyScript).c_str());

    if (status == 0) {
        cout << "打包作業完成。現在模式切換 (mm/normal) 應可正常雙向運作。" << endl;
    } else {
        cout << "錯誤：Python 執行失敗。" << endl;
    }
}

void run_export() {
    ExportToMHTML();
}