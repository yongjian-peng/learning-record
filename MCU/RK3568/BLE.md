#### 配置引脚

```
PCIE20_WAKEN_M1 => GPIO2_D1 89
PCIE20_CLKREQN_M1 => GPIO2_D0 88

WL_HOST_WAKE => GPIO2_B2 74
WL_REG_ON => GPIO2_B1 73
PCIE20_REFCLKN => 
PCIE20_REFCLKP =>

WL_GPIO_2 => GPIO3_D3 96+27 123
WL_GPIO_1 => GPIO3_D0 96+24 120
PCIE20_RXN =>
PCIE20_RXP =>
PCIE20_TXN =>
PCIE20_TXP =>

// 下面四个 测试引脚，并没有接出来
BT_PCM_CLK => 
BT_PCM_OUT =>
BT_PCM_IN =>
BT_PCM_SYNC =>


BT_HOST_WAKE => GPIO2_C0 64+16 80
BT_WAKE => GPIO2_C1 81
BT_REG_ON => GPIO2_B7 79
PCIE_PME_L => GPIO3_D1 96+25 121

4G_DISABLE_GPIO4_C2 => GPIO4_C2 128+18 146
PCIE20_PERSTN_M1 => GPIO3_C1  113
UART1_RTSN_M0 => GPIO2_B5 64+13 77
UART1_CTSN_M0 => GPIO2_B6 68

USB2_HOST3_MINIPCIE_DM =>
USB2_HOST3_MINIPCIE_DP =>

UART1_RX_M0 => GPIO2_B3 75
UART1_TX_M0 => UART1_TX_M0  GPIO2_B4 76

```



#### 蓝牙接口初始化

```

使用的是串口接口
/ # cat usr/bin/bt_pcba_test (bt_init.sh)
killall brcm_patchram_plus1
echo 0 > /sys/class/rfkill/rfkill0/state # 下电
echo 0 > /proc/bluetooth/sleep/btwrite
sleep 2
echo 1 > /sys/class/rfkill/rfkill0/state # 上电
echo 1 > /proc/bluetooth/sleep/btwrite
sleep 2

brcm_patchram_plus1 -d --enable_hci --no2bytes --tosleep 300000 --baudrate 1500000 --patchram /system/etc/firmware/BCM4362A2.hcd /dev/ttyS1 &

# 3000000 BCM4362A2.hcd 
#brcm_patchram_plus1 --bd_addr_rand --enable_hci --no2bytes --use_baudrate_for_download --tosleep 200000 --baudrate 3000000 --patchram /system/etc/firmware/BCM4362A2.hcd /dev/ttyS1 &

# 1500000 BCM4362A2.hcd
#brcm_patchram_plus1 --bd_addr_rand --enable_hci --no2bytes --use_baudrate_for_download --tosleep 200000 --baudrate 1500000 --patchram /system/etc/firmware/BCM4362A2.hcd /dev/ttyS1 &

# 115200 BCM4362A2.hcd
brcm_patchram_plus1 --bd_addr_rand --enable_hci --no2bytes --use_baudrate_for_download --tosleep 200000 --baudrate 115200 --patchram /system/etc/firmware/BCM4362A2.hcd /dev/ttyS1 &

# 1500000 BCM43013A0.hcd
#brcm_patchram_plus1 --bd_addr_rand --enable_hci --no2bytes --use_baudrate_for_download --tosleep 200000 --baudrate 1500000 --patchram /system/etc/firmware/BCM43013A0.hcd /dev/ttyS1 &

# 1500000 BCM4335C0.hcd
#brcm_patchram_plus1 --bd_addr_rand --enable_hci --no2bytes --use_baudrate_for_download --tosleep 200000 --baudrate 1500000 --patchram /system/etc/firmware/BCM4335C0.hcd /dev/ttyS1 &

# 1500000 BCM43430B0.hcd
brcm_patchram_plus1 --bd_addr_rand --enable_hci --scopcm BCM43430B0.hcd --no2bytes --use_baudrate_for_download --tosleep 200000 --baudrate 1500000 --patchram /system/etc/firmware/BCM43430B0.hcd /dev/ttyS1 &

# 1500000 BCM4343A1.hcd
#brcm_patchram_plus1 --bd_addr_rand --enable_hci --no2bytes --use_baudrate_for_download --tosleep 200000 --baudrate 1500000 --patchram /system/etc/firmware/BCM4343A1.hcd /dev/ttyS1 &

# 1500000 BCM4345C0.hcd
brcm_patchram_plus1 --bd_addr_rand --enable_hci --no2bytes --use_baudrate_for_download --tosleep 200000 --baudrate 1500000 --patchram /system/etc/firmware/BCM4345C0.hcd /dev/ttyS1 &

hciconfig hci0 up
hciconfig –a
正常的情况下可以看到：
BT扫描： hcitool scan

#注意：每次启动测试时要先kill掉brcm_patchram_plus1进程

hcitool cmd 0x01 0x0019 执行命令
hcidump 
hciconfig hci0 reset
hcitool -i hci0 info 3F:F5:4D:83:6E:03


hcitool scan --length=30
bluetoothctl scan on
bluetoothctl show
bluetoothctl devices
bluetoothctl menu gatt
bluetoothctl pairable on
btmon

lspci： 检查 PCIe 网络卡
hciconfig -a  ：显示 Bluetooth 接口（hci0）

启动 Bluetooth
bluetoothctl power on
hciconfig hci0 up

设置可发现（可选，让其他设备能找到您）
bluetoothctl discoverable on

开始扫描：
bluetoothctl scan on

BLE（低功耗）扫描：hcitool lescan

停止扫描: bluetoothctl scan off

配对和连接（扫描后）：
bluetoothctl pair <MAC_ADDRESS>  # 如 bluetoothctl pair 00:11:22:33:44:55
bluetoothctl connect <MAC_ADDRESS>
bluetoothctl trust <MAC_ADDRESS>  # 信任设备，实现自动重连

查看已配对设备
bluetoothctl paired-devices

移除设备
bluetoothctl remove <MAC_ADDRESS>
```



#### 扫描成功日志

```
 bluetoothctl scan on
Discovery started
[CHG] Controller F3:DE:6B:B0:EC:C3 Discovering: yes
[NEW] Device 4A:0E:16:53:42:C0 4A-0E-16-53-42-C0
[CHG] Device 4A:0E:16:53:42:C0 ManufacturerData Key: 0x004c (76)
[CHG] Device 4A:0E:16:53:42:C0 ManufacturerData Value:
  10 05 2d 18 2c 04 2e                             ..-.,..         
[CHG] Device 4A:0E:16:53:42:C0 ManufacturerData Key: 0x004c (76)
[CHG] Device 4A:0E:16:53:42:C0 ManufacturerData Value:
  10 05 2d 98 2c 04 2e                             ..-.,..         
[CHG] Device 4A:0E:16:53:42:C0 TxPower: 0xffffff88 (-120)
[CHG] Device 4A:0E:16:53:42:C0 ManufacturerData Key: 0x004c (76)
[CHG] Device 4A:0E:16:53:42:C0 ManufacturerData Value:
  10 05 2d 18 2c 04 2e 
  
  
  bluetoothctl trust 64:6C:37:ED:B8:4F
[CHG] Device 64:6C:37:ED:B8:4F Trusted: yes
Changing 64:6C:37:ED:B8:4F trust succeeded

```





#### WIFI 配置

```
使用的是 PCIE2.0 接口
### 配置
# 检查内核Wi-Fi配置，打开如下几个配置：
CONFIG_WL_ROCKCHIP=y
CONFIG_WIFI_BUILD_MODULE=y
CONFIG_BCMDHD=y
CONFIG_AP6XXX=m
CONFIG_BCMDHD_PCIE=y #PCIE接口，与SDIO互斥，不是PCIE可不配
CONFIG_BCMDHD_SDIO=y #SDIO接口，与PCIE互斥

// rfkill-wlan.c wifi 驱动文件
	gpio_direction_output 设置方向
// brcm_patchram_plus1.c
// bluez5-util.c 


```



#### WIFI 相关命令

```
dmesg | grep brcm
型号是：正基 AP6275P
使用的驱动是: bcmdhd.101.10.591.95  
固件是：clm_bcm43752a2_pcie_ag.blob fw_bcm43752a2_pcie_ag.bin 

查看Wi-Fi的服务进程启动
wpa_supplicant -B -i wlan0 -c /etc/wpa_supplicant.conf

关闭Wi-Fi
ifconfig wlan0 down
killall wpa_supplicant

扫描热点
wpa_cli -i wlan0 -p /var/run/wpa_supplicant scan_results
用 wpa_cli 查看扫描结果（连接结果）：
wpa_cli -i wlan0 scan_results

#让wpa_supplicant进程重新读取上述配置，命令如下：
wpa_cli -i wlan0 -p /var/run/wpa_supplicant reconfigure
#发起连接：
wpa_cli -i wlan0 -p /var/run/wpa_supplicant reconnect
#查看连接
wpa_cli -i wlan0 -p /var/run/wpa_supplicant status

检查驱动支持情况
iw list

验证命令
iwconfig wlan0
iw wlan0 station dump
iw dev wlan0 link



```



#### WIFI  相关日志

```
root@rk3568-buildroot:/# wpa_supplicant -B -i wlan0 -c /etc/wpa_supplicant.conf 
Successfully initialized wpa_supplicant -B -i wlan0 -c /etc/wpa_supplicant.conf  
nl80211: kernel reports: Match already configured
nl80211: kernel reports: Match already configured
nl80211: kernel reports: Match already configured
nl80211: kernel reports: Match already configured
nl80211: kernel reports: Match already configured
nl80211: kernel reports: Match already configured
nl80211: kernel reports: Match already configured
nl80211: kernel reports: Match already configured
nl80211: kernel reports: Match already configured
nl80211: kernel reports: Match already configured
nl80211: kernel reports: Match already configured
nl80211: kernel reports: Match already configured
nl80211: kernel reports: Match already configured
nl80211: kernel reports: Match already configured
nl80211: kernel reports: Match already configured
nl80211: kernel reports: Match already configured
nl80211: kernel reports: Match already configured
nl80211: kernel reports: Match already configured
nl80211: kernel reports: Match already configured
nl80211: kernel reports: Match already configured
nl80211: kernel reports: Match already configured
nl80211: kernel reports: Match already configured
nl80211: kernel reports: Match already configured
nl80211: kernel reports: Match already configured
ctrl_iface exists and seems to be in use - cannot override it
Delete '/var/run/wpa_supplicant/wlan0' manually if it is not used anymore
Failed to initialize control interface '/var/run/wpa_supplicant'.
You may have another wpa_supplicant process already running or the file was
left by an unclean termination of wpa_supplicant in which case you will need
to manually remove this file before starting wpa_supplicant again.

wlan0: Do not deauthenticate as part of interface deinit since WoWLAN is enabled
wlan0: CTRL-EVENT-DSCP-POLICY clear_all
nl80211: deinit ifname=wlan0 disabled_11b_rates=0


#ctrl_interface接口配置
#如果有修改的话对应wpa_cli命令-p参数要相应进行修改，wpa_cli –i wlan0 –p <ctrl_interface>
xxx
$ vi /etc/wpa_supplicant.conf
ctrl_interface=/var/run/wpa_supplicant #默认不建议修改!
ap_scan=1
update_config=1 #这个配置使wpa_cli命令配置的热点保存到conf文件里面（wpa_cli
save_config）
#AP配置项
network={
	ssid="WiFi-AP" # Wi-Fi名字
	psk="12345678" # Wi-Fi密码
	key_mgmt=WPA-PSK # 加密配置，不加密则改为：key_mgmt=NONE
}

/// 扫描到的热点 
root@rk3568-buildroot:/# wpa_cli -i wlan0 -p /var/run/wpa_supplicant scan
OK
[  590.720691] [dhd] [wlan0] wl_run_escan : LEGACY_SCAN sync ID: 1, bssidx: 0
soot@rk3568-buildroot:/# wpa_cli -i wlan0 -p /var/run/wpa_supplicant scan_results
bssid / frequency / signal level / flags / ssid
48:a7:3c:f7:18:66	5260	-43	[WPA-PSK-CCMP+TKIP][WPA2-PSK-CCMP+TKIP][ESS]	YNKJ-5G
9c:2b:a6:2d:a6:a8	5180	-57	[WPA-PSK-CCMP+TKIP][WPA2-PSK-CCMP+TKIP][ESS]	\xe9\x9d\x92\xe7\xa8\x9eDigiNeer507
44:32:62:0f:bb:b7	5240	-63	[WPA2-PSK-CCMP][ESS]	DigiNeer507
d8:32:14:e9:d8:ac	5200	-64	[WPA-PSK-CCMP+TKIP][WPA2-PSK-CCMP+TKIP][WPS][ESS]	OUSUYAN3949-5G
ec:b9:70:96:fa:fc	5300	-65	[WPA-PSK-CCMP+TKIP][WPA2-PSK-CCMP+TKIP][ESS]	\xe9\x9d\x92\xe7\xa8\x9eDigiNeer507
a2:3b:bb:16:dc:68	5180	-73	[WPA2-PSK-CCMP][WPS][ESS]	\xe5\x8d\x8e\xe4\xb8\xba\xe7\xbd\x91\xe7\xbb\x9c
5c:e8:d3:01:3e:cd	2462	-27	[WPA2-PSK-CCMP][WPS][ESS]	HB_013ECD
a2:3b:bb:16:dc:73	5180	-71	[WPA2-PSK-CCMP][ESS]	\xe5\x8d\x8e\xe4\xb8\xba\xe7\xbd\x91\xe7\xbb\x9c_Wi-Fi5

// 查看连接详情
root@rk3568-buildroot:/# wpa_cli -i wlan0 -p /var/run/wpa_supplicant status
bssid=48:a7:3c:f7:18:66
freq=5260
ssid=YNKJ-5G
id=0
mode=station
wifi_generation=5
pairwise_cipher=CCMP
group_cipher=TKIP
key_mgmt=WPA2-PSK
wpa_state=COMPLETED
ip_address=192.168.1.14
p2p_device_address=9e:b8:b4:5f:0a:82
address=9c:b8:b4:5f:0a:82
uuid=e935c971-0cfa-572e-a0ee-51addb553f0a
ieee80211ac=1

```

