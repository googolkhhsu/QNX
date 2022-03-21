# socket-resmgr
修改 Makefile:
  * 修改 TCC: 此為 QNX 編譯器的路徑位置，把它改成對應環境下的路徑
  * 修改 SER_ADDR: 此為 socket server 端的 IP 地址，將它改成相對應的地址

編譯:
 * 在 Linux terminal 環境下，移動到 socket-resmgr 的路徑下，執行 make 指令
 * 編譯完成會將執行檔輸出到 out 目錄

執行:
 * 將 out/bj-tcp-server 複製到 Linux 機器上，並且運行它
 * 將 out/app-* 以及 out/powertrain 複製到 QNX-VM
 * 先運行 powertrain ，讓 Resource Manager 先運行起來
 * 然後按照需求運行 app-*
 	* app-read 使用 read() 讀取一次 ResMgr
 	* app-read2 使用 devctl() 循環讀取 ResMgr
 	* app-write 使用 write() 寫入一次 ResMgr
 	* app-devctl 使用 devctl() 對 ResMgr 執行一次寫入和一次讀取
