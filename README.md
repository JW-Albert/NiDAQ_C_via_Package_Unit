# NiDAQ C via Package

**NiDAQ C via Package** 是一個用於控制和數據採集的 C 語言工具包，專為 NI（National Instruments）硬體設計。該專案簡化了與 NI 硬體的交互過程，並提供了穩定高效的數據處理功能。

## 功能特點

- 簡化的 API，易於快速上手。
- 支援多種 NI 硬體設備。
- 高效的數據採集與處理能力。
- 模組化設計，方便擴展與整合。

## 系統環境

此專案基於 **Debian 12** 系統開發，並使用 `make` 工具實現自動化編譯。

## 安裝方式

1. **克隆專案：**
   ```bash
   git clone https://github.com/JW-Albert/NiDAQ_C_via_Package.git
   ```

2. **進入專案目錄：**
   ```bash
   cd NiDAQ_C_via_Package
   ```

3. **編譯專案：**
   使用 `make` 進行自動化編譯：
   ```bash
   make
   ```

4. **運行程式：**
   編譯完成後，您可以直接運行可執行檔案。
   ```bash
   ./nidaq
   ```

## 文件結構

- `include/`：公共標頭檔案。
- `API/`：ini 組態檔存儲。
- `output/`：輸出檔案存放，注意務必確保資料夾存在否則資料會無法正常存入。
- `main.cpp`：主程式。
- `Makefile`：用於自動化編譯的 Makefile。
