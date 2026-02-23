import struct
import random
import math
import os
import operator

# ==========================================
# 0. 使用者可調參數 (Run005 / Detector Configured)
# ==========================================

# --- 存檔路徑設定 ---
OUTPUT_DIR   = "D:/Desktop/自動化工具/TestFloder/TestFiles/" 
OuputName    = "Simulated"
# --- [新增] 探測器 ID 與檔名後綴對應表 ---
# 這樣您就可以自由新增或修改 ID 與對應的檔名
DETECTOR_MAP = {
    2: "Monitor1",
    3: "Monitor2",
    4: "PbWO4"
}

# --- 運作模式 ---
bFullRead    = True 

# --- 模擬設定 ---
NUM_EVENTS   = 5000           
RUN_LABEL    = "Run005"       
TIME_TAG     = "000001.111"   

# --- 物理參數 ---
BEAM_E_MEAN  = 796.0; BEAM_E_SIGMA = 10.0
BEAM_X_MEAN  = 29.0;  BEAM_X_SIGMA = 14.0
BEAM_Y_MEAN  = 135.0; BEAM_Y_SIGMA = 6.9

# --- 閾值設定 ---
THRESHOLD_CFG = {
    'Monitor1': { 'ROC0': {'MEAN': 18.0, 'SIGMA': 2.0}, 'ROC1': {'MEAN': 18.0, 'SIGMA': 2.0} },
    'Monitor2': { 'ROC0': {'MEAN': 18.0, 'SIGMA': 2.0}, 'ROC1': {'MEAN': 18.0, 'SIGMA': 2.0} },
    'PWO':      { 'ROC0': {'MEAN': 5000.0, 'SIGMA': 200.0},'ROC1': {'MEAN': 5000.0, 'SIGMA': 200.0} }
}

# --- 其他常數 ---
NOISE_BM_MEAN=0.0; 
NOISE_BM_SIGMA=0.0; 
NOISE_PWO_MEAN=0.0; 
NOISE_PWO_SIGMA=0.0  
MIP_MEAN_MEV=1.0; 
MIP_SIGMA_MEV=0.3; 
ADC_PER_MEV=100.0
BM_CROSS_TALK=0.1; 
PWO_GAIN=90.0; 
AVG_TIME_GAP=10000.0       
TICKS_PER_SEC=260417; 
GTMID_MASTER=0x00

# ==========================================
# 1. 硬體與幾何參數定義
# ==========================================
proGID_D2 = [63,53,43,37,31,20,16,4,62,57,52,47,42,36,30,21,17,10,5,0,61,56,51,46,41,35,29,22,18,11,6,1,60,55,50,45,40,34,28,23,19,12,7,2,59,54,49,44,39,33,27,24,15,13,8,3,58,48,38,32,26,25,14,9]
propX_D2 = [-21.1,-0.1,20.9,20.9,41.9,41.9,62.9,83.9]*6; propX_D2 = [-21.10,-0.10,20.90,20.90,41.90,41.90,62.90,83.90,-21.10,-21.10,-0.10,-0.10,20.90,20.90,41.90,41.90,62.90,62.90,83.90,83.90,-21.10,-21.10,-0.10,-0.10,20.90,20.90,41.90,41.90,62.90,62.90,83.90,83.90,-21.10,-21.10,-0.10,-0.10,20.90,20.90,41.90,41.90,62.90,62.90,83.90,83.90,-21.10,-21.10,-0.10,-0.10,20.90,20.90,41.90,41.90,62.90,62.90,83.90,83.90,-21.10,-0.10,20.90,20.90,41.90,41.90,62.90,83.90]
propY_D2 = [195.62]*8 + [174.62]*12 + [153.62]*12 + [132.62]*12 + [111.62]*12 + [90.62]*8
PWO_POS_MAP = {}
for i in range(64):
    gid = proGID_D2[i]; x = round(propX_D2[i], 2); y = round(propY_D2[i], 2)
    if (x, y) not in PWO_POS_MAP: PWO_POS_MAP[(x, y)] = []
    PWO_POS_MAP[(x, y)].append(gid)

T_GIDS = [15,16,14,17,13,18,12,19,11,20,10,21,9,22,8,23,7,24,6,25,5,26,4,27,3,28,2,29,1,30,0,31,48,47,49,46,50,45,51,44,52,43,53,42,54,41,55,40,56,39,57,38,58,37,59,36,60,35,61,34,62,33,63,32]
T_PROP_X = [18.17]*32 + [-11.318,-9.219,-7.12,-5.021,-2.922,-0.823,1.276,3.375,5.474,7.573,9.672,11.771,13.87,15.969,18.068,20.167,22.266,24.365,26.464,28.563,30.662,32.761,34.86,36.959,39.058,41.157,43.256,45.355,47.454,49.553,51.652,53.751]
T_PROP_Y = [165.501,163.402,161.303,159.204,157.105,155.006,152.907,150.808,148.709,146.61,144.511,142.412,140.313,138.214,136.115,134.016,131.917,129.818,127.719,125.62,123.521,121.422,119.323,117.224,115.125,113.026,110.927,108.828,106.729,104.63,102.531,100.432] + [136.02]*32

class MonitorMapping:
    def __init__(self, gids, prop_x, prop_y):
        self.x_strips = []; self.y_strips = []
        for i in range(64):
            gid = gids[i]; x = prop_x[i]; y = prop_y[i]
            if i < 32: self.y_strips.append({'gid': gid, 'pos': y}) 
            else:      self.x_strips.append({'gid': gid, 'pos': x})
        self.x_strips.sort(key=operator.itemgetter('pos')); self.y_strips.sort(key=operator.itemgetter('pos'))

T1_MAP = MonitorMapping(T_GIDS, T_PROP_X, T_PROP_Y); T2_MAP = MonitorMapping(T_GIDS, T_PROP_X, T_PROP_Y)

# ==========================================
# 2. 物理模擬核心
# ==========================================
class EventSimulator:
    def __init__(self): self.moliere_radius = 22.0 
    def get_threshold(self, det_name, roc_id):
        # 簡單防呆: 若 DETECTOR_MAP 有未知名稱，預設給 Monitor1 的閾值
        if det_name not in THRESHOLD_CFG: det_name = 'Monitor1'
        
        key = 'ROC0' if roc_id == 0 else 'ROC1'; cfg = THRESHOLD_CFG[det_name][key]
        return random.gauss(cfg['MEAN'], cfg['SIGMA'])
        
    def find_closest_index(self, strips, target_pos):
        best_idx = -1; min_dist = float('inf')
        for i, strip in enumerate(strips):
            dist = abs(strip['pos'] - target_pos)
            if dist < min_dist: min_dist = dist; best_idx = i
        return best_idx
        
    def process_bm_plane(self, hits_list, did, det_name, strips, hit_pos):
        idx = self.find_closest_index(strips, hit_pos)
        if idx == -1: return 0.0 
        eloss_mev = random.gauss(MIP_MEAN_MEV, MIP_SIGMA_MEV)
        if eloss_mev < 0.1: return 0.0 
        e_center = eloss_mev * (1.0 - 2 * BM_CROSS_TALK); e_side = eloss_mev * BM_CROSS_TALK
        targets = [(idx, e_center)]; 
        if idx > 0: targets.append((idx - 1, e_side))      
        if idx < len(strips) - 1: targets.append((idx + 1, e_side)) 
        for t_idx, t_e in targets:
            if t_e <= 0: continue
            noise = random.gauss(NOISE_BM_MEAN, NOISE_BM_SIGMA); adc = int(t_e * ADC_PER_MEV + noise)
            gid = strips[t_idx]['gid']; roc_id = gid // 32; thr = self.get_threshold(det_name, roc_id)
            if adc > thr: hits_list.append({'did': did, 'roc': roc_id, 'ch': gid % 32, 'adc': min(adc, 16383), 'hg': 0, 'fire_bit': 1})
        return eloss_mev
        
    def generate_event(self):
        hits = [] 
        evt_x = random.gauss(BEAM_X_MEAN, BEAM_X_SIGMA); evt_y = random.gauss(BEAM_Y_MEAN, BEAM_Y_SIGMA)
        evt_e = random.gauss(BEAM_E_MEAN, BEAM_E_SIGMA); 
        if evt_e < 0: evt_e = 0
        loss_t1_x = self.process_bm_plane(hits, 2, 'Monitor1', T1_MAP.x_strips, evt_x)
        loss_t1_y = self.process_bm_plane(hits, 2, 'Monitor1', T1_MAP.y_strips, evt_y)
        loss_t2_x = self.process_bm_plane(hits, 3, 'Monitor2', T2_MAP.x_strips, evt_x)
        loss_t2_y = self.process_bm_plane(hits, 3, 'Monitor2', T2_MAP.y_strips, evt_y)
        evt_e -= (loss_t1_x + loss_t1_y + loss_t2_x + loss_t2_y); 
        if evt_e < 0: evt_e = 0
        total_adc = evt_e * 2.5 
        for (c_x, c_y), gids in PWO_POS_MAP.items():
            dist = math.sqrt((evt_x - c_x)**2 + (evt_y - c_y)**2)
            adc_val = total_adc * math.exp(-dist / self.moliere_radius) * 0.1 * PWO_GAIN
            n_sipm = len(gids)
            for gid in gids:
                ratio = random.gauss(1.0/n_sipm, 0.05); noise = random.gauss(NOISE_PWO_MEAN, NOISE_PWO_SIGMA)
                ch_adc = int(adc_val * ratio + noise); roc_id = gid // 32; thr = self.get_threshold('PWO', roc_id)
                pass_thr = (ch_adc > thr)
                if pass_thr or bFullRead:
                    fire_status = 1 if pass_thr else 0
                    if ch_adc < 0: ch_adc = 0 
                    if ch_adc > 16383: ch_adc = 16383
                    hits.append({'did': 4, 'roc': roc_id, 'ch': gid % 32, 'adc': ch_adc, 'hg': 1, 'fire_bit': fire_status})
        return hits

# ==========================================
# 3. 寫入器 (v30: No Changes, Handles Stream)
# ==========================================
class StreamWriter:
    def __init__(self, filename):
        self.filename = filename
        self.file = open(filename, "wb")
        self.current_payload = []; self.current_payload_size = 0; self.seq_num = 0
        self.MAX_WORDS = 367 
        print(f"Initialized: {self.filename}")

    def add_word(self, word_int):
        self.current_payload.append(word_int)
        if len(self.current_payload) >= self.MAX_WORDS:
            self.flush_packet()
            return True
        return False

    def add_sync_packet(self, pcnt):
        w0 = (0xCA << 16) | (GTMID_MASTER << 8) | (pcnt & 0xFF)
        w1=0; w2=0; w3=517; w4=258; w5=1537; w6=16; w7=0; w8=19
        sync_words = [w0, w1, w2, w3, w4, w5, w6, w7, w8]
        sync_words.extend([0x000000] * 5) 
        sync_words.append(0xF2F5FA)
        
        if len(self.current_payload) + 15 > self.MAX_WORDS:
            needed = self.MAX_WORDS - len(self.current_payload)
            for _ in range(needed): self.current_payload.append(0x000000)
            self.flush_packet()
            
        flushed = False
        for w in sync_words:
            if self.add_word(w): flushed = True
        return flushed

    def flush_packet(self):
        if not self.current_payload: return
        packet = bytearray()
        packet.extend(bytes.fromhex("FE 01 60 00 FE 00 00 00 00 00 00 00 00 04 56"))
        packet.append(0x17); packet.append(0x88); packet.append(0x55); packet.append(0x00)
        packet.append(self.seq_num % 256); packet.append(0x04); packet.append(0x4F)
        
        payload_bytes = bytearray()
        chunk = self.current_payload[:self.MAX_WORDS]
        for word in chunk:
            payload_bytes.append((word >> 16) & 0xFF)
            payload_bytes.append((word >> 8) & 0xFF)
            payload_bytes.append(word & 0xFF)
            
        target_payload = 1103
        if len(payload_bytes) < target_payload:
            payload_bytes.extend(b'\x00' * (target_payload - len(payload_bytes)))
        
        packet.extend(payload_bytes)
        packet.append(0xFF); packet.append(0xFF) 
        self.file.write(packet)
        self.current_payload = self.current_payload[self.MAX_WORDS:]
        self.seq_num += 1

    def close(self):
        self.file.close()
        print(f"Closed: {self.filename}, Packets: {self.seq_num}")
    
    def has_pending_data(self): return len(self.current_payload) > 0

# ==========================================
# 4. 主程式 (v34: Configured Writers)
# ==========================================
def encode_hit_word(hit):
    word = 0
    is_hit = hit.get('fire_bit', 1)
    word |= (1 << 22) if is_hit else 0
    word |= (hit['roc'] & 0x1) << 20; word |= (hit['ch'] & 0x1F) << 15
    word |= (hit['hg'] & 0x1) << 14; word |= (hit['adc'] & 0x3FFF)
    return word

def encode_time_word(fcnt, buffer_id=0):
    word = 0; word |= (1 << 23); word |= (buffer_id & 0xF) << 18; word |= (fcnt & 0x3FFFF)
    return word

def main():
    sim = EventSimulator()
    
    # 自動建立資料夾
    if OUTPUT_DIR and not os.path.exists(OUTPUT_DIR):
        try:
            os.makedirs(OUTPUT_DIR)
            print(f"Directory created: {OUTPUT_DIR}")
        except OSError as e:
            print(f"Error creating directory: {e}")
            return

    def get_path(suffix):
        fname = f"{OuputName}_{RUN_LABEL}_{TIME_TAG}{suffix}.bin"
        return os.path.join(OUTPUT_DIR, fname)

    # [修改] 使用 DETECTOR_MAP 自動建立 Writers
    writers = {}
    for did, name in DETECTOR_MAP.items():
        writers[did] = StreamWriter(get_path(name))
    
    current_time = 0.0; recorded_events = 0; pcnt = 0; next_sync_time = TICKS_PER_SEC 

    print(f"Start Simulation (v34: Detector Config Map)...")
    print(f"Saving to: {OUTPUT_DIR}")
    print(f"Detectors: {DETECTOR_MAP}")
    
    # Initial Sync
    for w in writers.values(): w.add_sync_packet(pcnt)
    pcnt += 1

    for i in range(NUM_EVENTS):
        delta_t = random.expovariate(1.0 / AVG_TIME_GAP); 
        if delta_t < 1.0: delta_t = 1.0
        current_time += delta_t; fcnt = int(current_time)

        if current_time >= next_sync_time:
            for w in writers.values(): w.add_sync_packet(pcnt)
            pcnt += 1; next_sync_time += TICKS_PER_SEC

        hits = sim.generate_event()
        if not hits: continue
        
        # Dual ROC Splitting
        hits_by_did_roc = {}
        for hit in hits:
            did = hit['did']; roc = hit['roc']
            if did not in hits_by_did_roc: hits_by_did_roc[did] = {0: [], 1: []}
            hits_by_did_roc[did][roc].append(hit)
            
        recorded_events += 1
        w_time = encode_time_word(fcnt)
        
        # [修改] 遍歷 writers 字典，這樣就能動態支援 DETECTOR_MAP 中的所有探測器
        for did, writer in writers.items():
            if did not in hits_by_did_roc: continue
            
            # ROC 0
            if len(hits_by_did_roc[did][0]) > 0:
                writer.add_word(w_time) 
                for hit in hits_by_did_roc[did][0]:
                    writer.add_word(encode_hit_word(hit))
            
            # ROC 1
            if len(hits_by_did_roc[did][1]) > 0:
                writer.add_word(w_time)
                for hit in hits_by_did_roc[did][1]:
                    writer.add_word(encode_hit_word(hit))
        
        if (i+1) % 1000 == 0: print(f"Processed {i+1} triggers... Recorded: {recorded_events}")

    print("\nEnd-of-Run Padding...")
    for w in writers.values():
        if w.has_pending_data():
            while True:
                if w.add_word(encode_time_word(fcnt, 0xF)): break
                    
    for w in writers.values(): w.close()
    print("Done.")



if __name__ == "__main__":
    import time
    from datetime import datetime, timedelta

    # 設定起始時間與小數點部分 (HHMMSS.subsec)
    base_time_str = "000001"  # 00時00分01秒
    base_subsec   = ".111"    # 固定小數部分
    
    # 將字串轉換為時間物件以便計算
    start_dt = datetime.strptime(base_time_str, "%H%M%S")

    # 迴圈：從 1 到 30
    for i in range(5, 10):
        # 1. 修改全域變數 RUN_LABEL (格式化為 Run0000XX)
        RUN_LABEL = f"Run{i:06d}"

        # 2. 修改全域變數 TIME_TAG (每輪增加 30 秒)
        # 計算時間偏移量：第1輪+0秒，第2輪+30秒...
        time_delta = timedelta(seconds=30 * (i - 1))
        current_dt = start_dt + time_delta
        
        # 組合新的時間字串 HHMMSS + .subsec
        TIME_TAG = current_dt.strftime("%H%M%S") + base_subsec

        print(f"\n[Batch Job] Generating {RUN_LABEL} with TimeTag {TIME_TAG}...")
        
        # 3. 執行主程式 (main 會讀取上方修改過的全域變數)
        main()

        # 4. 等待 10 秒 (如果是最後一次則不用等)
        if i != 10 -1:
            print("Waiting 20 seconds for next run...")
            time.sleep(20)
            
    print("\nAll 30 Runs generated successfully.")