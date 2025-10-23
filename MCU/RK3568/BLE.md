配置引脚

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



蓝牙接口初始化

```


/ # cat usr/bin/bt_pcba_test (bt_init.sh)
#!/bin/sh
killall brcm_patchram_plus1
echo 0 > /sys/class/rfkill/rfkill0/state # 下电
echo 0 > /proc/bluetooth/sleep/btwrite
sleep 2
echo 1 > /sys/class/rfkill/rfkill0/state # 上电
echo 1 > /proc/bluetooth/sleep/btwrite
sleep 2

brcm_patchram_plus1 --bd_addr_rand --enable_hci --no2bytes --use_baudrate_for_download --tosleep 200000 --baudrate 1500000 --patchram /system/etc/firmware/BCM4362A2.hcd /dev/ttyS1 &

brcm_patchram_plus1 --bd_addr_rand --enable_hci --no2bytes --use_baudrate_for_download --tosleep 200000 --baudrate 3000000 --patchram /system/etc/firmware/BCM4362A2.hcd /dev/ttyS1 &

brcm_patchram_plus1 --bd_addr_rand --enable_hci --no2bytes --use_baudrate_for_download --tosleep 200000 --baudrate 115200 --patchram /system/etc/firmware/BCM4362A2.hcd /dev/ttyS1 &

 

hciconfig hci0 up
hciconfig –a
正常的情况下可以看到：
BT扫描： hcitool scan

#注意：每次启动测试时要先kill掉brcm_patchram_plus1进程

hcitool cmd 0x01 0x0019 执行命令
```



#### WIFI 配置

```
### 配置
# 检查内核Wi-Fi配置，打开如下几个配置：
CONFIG_WL_ROCKCHIP=y
CONFIG_WIFI_BUILD_MODULE=y
CONFIG_BCMDHD=y
CONFIG_AP6XXX=m
CONFIG_BCMDHD_PCIE=y #PCIE接口，与SDIO互斥，不是PCIE可不配
CONFIG_BCMDHD_SDIO=y #SDIO接口，与PCIE互斥
```

