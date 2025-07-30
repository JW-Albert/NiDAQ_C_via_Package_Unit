# NiDAQ C++ 數據採集系統

**NiDAQ C++ 數據採集系統** 是一個基於 C++ 開發的高性能數據採集工具，專為 NI（National Instruments）硬體設計。該專案提供了簡化的 API 介面，支援多通道數據採集、實時數據處理和 CSV 格式輸出。

## 🚀 功能特點

- **多通道數據採集**：支援同時採集多個通道的類比信號
- **實時數據處理**：使用多執行緒技術實現高效能的實時數據處理
- **CSV 格式輸出**：自動將採集數據保存為 CSV 格式，便於後續分析
- **INI 配置檔案**：支援通過 INI 檔案進行硬體參數配置
- **模組化設計**：清晰的類別結構，便於擴展和維護
- **跨平台支援**：支援 Windows 和 Linux 系統

## 📋 系統需求

### 硬體需求
- NI-DAQmx 相容的數據採集硬體
- 支援的 NI 設備（如 USB-6000 系列、PCIe 系列等）

### 軟體需求
- **編譯器**：GCC 4.9+ 或 MinGW-w64
- **C++ 標準**：C++11 或更高版本
- **作業系統**：Windows 10/11 或 Linux
- **NI-DAQmx 驅動程式**：需要安裝 NI-DAQmx 驅動程式

## 🛠️ 安裝與編譯

### 1. 克隆專案
```bash
git clone https://github.com/JW-Albert/NiDAQ_C_via_Package.git
cd NiDAQ_C_via_Package
```

### 2. 安裝依賴
確保您的系統已安裝：
- NI-DAQmx 驅動程式
- GCC 編譯器（Windows 用戶建議使用 MinGW-w64）

### 3. 編譯專案

#### 使用 Makefile（推薦）
```bash
# 清理之前的編譯檔案
mingw32-make clean

# 編譯專案
mingw32-make
```

#### 手動編譯
```bash
g++ -std=c++17 -pthread -I./include -I./include/iniReader -I./include/NiDAQmx/include \
    main.cpp include/NiDAQ.cpp include/CSVWriter.cpp \
    include/iniReader/INIReader.cpp include/iniReader/ini.c \
    -L./include/NiDAQmx/lib64/msvc -lNIDAQmx -static-libgcc -static-libstdc++ \
    -o main.exe
```

### 4. 運行程式
```bash
./main.exe
```

## 📁 專案結構

```
NiDAQ_C_via_Package/
├── main.cpp                 # 主程式入口
├── Makefile                 # 編譯配置文件
├── README.md               # 專案說明文件
├── API/                    # 配置文件目錄
│   ├── Master.ini         # 主配置文件
│   └── NiDAQ.ini          # NI-DAQ 硬體配置
├── include/                # 標頭檔案和源碼
│   ├── NiDAQ.h            # NI-DAQ 處理類別標頭檔
│   ├── NiDAQ.cpp          # NI-DAQ 處理類別實現
│   ├── CSVWriter.h        # CSV 寫入器標頭檔
│   ├── CSVWriter.cpp      # CSV 寫入器實現
│   ├── iniReader/         # INI 檔案讀取器
│   │   ├── INIReader.h
│   │   ├── INIReader.cpp
│   │   ├── ini.h
│   │   └── ini.c
│   └── NiDAQmx/           # NI-DAQmx 庫檔案
│       ├── include/       # 標頭檔案
│       └── lib64/         # 庫檔案
├── Master_ini_Maker/      # INI 檔案生成工具
│   ├── main.cpp
│   └── Makefile
└── output/                # 輸出數據目錄
    └── NiDAQ/            # NI-DAQ 數據輸出
```

## ⚙️ 配置說明

### 主配置文件 (API/Master.ini)
```ini
[SaveUnit]
second = 10    # 數據保存間隔（秒）
```

### NI-DAQ 配置文件 (API/NiDAQ.ini)
```ini
[DAQ]
device = Dev1              # 設備名稱
channels = ai0,ai1,ai2     # 採集通道
sampleRate = 1000          # 採樣率 (Hz)
bufferSize = 1000          # 緩衝區大小
```

## 📊 輸出格式

程式會將採集的數據保存為 CSV 格式，檔案結構如下：
- **檔案位置**：`output/NiDAQ/`
- **檔案命名**：`datadir_YYYYMMDD_HHMMSS.csv`
- **數據格式**：每行包含時間戳和所有通道的數據值

## 🔧 故障排除

### 常見編譯錯誤

1. **C++ 標準支援問題**
   ```
   error: This file requires compiler and library support for the ISO C++ 2011 standard
   ```
   **解決方案**：確保使用 `-std=c++11` 或更高版本的編譯參數

2. **NIDAQmx.h 找不到**
   ```
   fatal error: NIDAQmx.h: No such file or directory
   ```
   **解決方案**：檢查包含路徑是否正確指向 `./include/NiDAQmx/include`

3. **庫檔案連結失敗**
   ```
   cannot find -lnidaqmx: No such file or directory
   ```
   **解決方案**：確保使用正確的庫檔案路徑（Windows 使用 MSVC 格式）

### 運行時問題

1. **設備未連接**：確保 NI 硬體正確連接並安裝驅動程式
2. **權限不足**：以管理員身份運行程式
3. **配置文件錯誤**：檢查 INI 檔案格式和參數設置

## 🤝 貢獻指南

歡迎提交 Issue 和 Pull Request 來改善這個專案。

### 開發環境設置
1. Fork 本專案
2. 創建功能分支：`git checkout -b feature/new-feature`
3. 提交變更：`git commit -am 'Add new feature'`
4. 推送分支：`git push origin feature/new-feature`
5. 提交 Pull Request

## 📄 授權條款

本專案採用 MIT 授權條款 - 詳見 [LICENSE](LICENSE) 檔案

## 📞 聯絡資訊

- **作者**：JW-Albert
- **專案網址**：https://github.com/JW-Albert/NiDAQ_C_via_Package
- **問題回報**：請使用 GitHub Issues

## 🔄 更新日誌

### v1.0.0
- 初始版本發布
- 支援多通道數據採集
- 實時 CSV 數據輸出
- INI 配置文件支援
