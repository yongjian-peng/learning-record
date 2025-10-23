查看命令

cat /sys/kernel/debug/clk/clk_summary | grep pcie



lspci







添加配置

```
CONFIG_PCI=y
CONFIG_PCI_DOMAINS=y
CONFIG_PCI_DOMAINS_GENERIC=y
CONFIG_PCI_SYSCALL=y
CONFIG_PCI_BUS_ADDR_T_64BIT=y
CONFIG_PCI_MSI=y
CONFIG_PCI_MSI_IRQ_DOMAIN=y
CONFIG_PHY_ROCKCHIP_PCIE=y
CONFIG_PCIE_ROCKCHIP=y
CONFIG_PCIEPORTBUS=y
CONFIG_PCIEASPM=y
CONFIG_PCIEASPM_POWERSAVE=y
CONFIG_PCIE_PME=y
CONFIG_GENERIC_MSI_IRQ=y
CONFIG_GENERIC_MSI_IRQ_DOMAIN=y
CONFIG_IRQ_DOMAIN=y
CONFIG_IRQ_DOMAIN_HIERARCHY=y

新增工具
buildroot
BR2_PACKAGE_FIO=y


pcie30_phy_grfs

引脚：
PCIE_PWR_EN GPIO1_A4
PCIE30X1_WAKEN_M1 GPIO2_D3
PCIE30X1_CLKREQN_M1 GPIO2_D2
PCIE30X1_PERSTN_M1 GPIO3_A1

kernel
CONFIG_PCIEASPM_POWERSAVE=y
CONFIG_PCIEASPM_EXT=y

CONFIG_USB_OTG_FSM=y
CONFIG_USB_DWC3_HOST=y
```



#### 性能测试命令

```
fio --name=seq_read_max --filename=/dev/nvme0n1 --rw=read \
--bs=1M --numjobs=4 --iodepth=32 --runtime=30s --time_based --group_reporting

说明：

--numjobs=4：启用 4 个并发线程；

--iodepth=32：每线程保持 32 个未完成请求（能更好利用队列深度）；

这两个参数对 NVMe 非常关键，否则性能会被严重低估。
```





#### 生效 over

```
&pcie30phy {
	//rockchip,bifurcation;
	//rockchip,pcie30-phymode = <PHY_MODE_PCIE_NABIBI>;

	rockchip,pipe-grf = <&pipegrf>; // pipegrf pcie30_phy_grf
	//rockchip,pipe-phy-grf = <&pipe_phy_grf1>; // pipe_phy_grf0 pipe_phy_grf1

	status = "okay";
};

&pcie3x2 {
	// num-lanes = <1>;
	// rockchip,bifurcation;
	// supports-clkreq;


	reset-gpios = <&gpio3 RK_PA1 GPIO_ACTIVE_HIGH>;
	vpcie3v3-supply = <&vcc3v3_pcie>;

	// prsnt-gpios = <&gpio4 RK_PB7 GPIO_ACTIVE_LOW>;

	// PCIe 唤醒、时钟请求的 GPIO 定义（若驱动支持）
	// 注意：部分驱动不直接使用这些信号，而是通过硬件连线
	wake-gpios = <&gpio2 RK_PD3 GPIO_ACTIVE_LOW>;   // WAKEN 信号：GPIO2_D3
	clkreq-gpios = <&gpio2 RK_PD2 GPIO_ACTIVE_HIGH>; // CLKREQN 信号：GPIO2_D2

	// 新增 WAKEN、CLKREQN 信号映射
	// 这些信号通常用于 PCIe 电源管理（PME#唤醒、时钟请求）
	pinctrl-names = "default";
	pinctrl-0 = <&pcie30x1m1_pins>;

	status = "okay";
};

vcc3v3_pcie: gpio-regulator {
    compatible = "regulator-fixed";
    regulator-name = "vcc3v3_pcie";
    regulator-always-on;
    regulator-boot-on;
    regulator-min-microvolt = <3300000>;
    regulator-max-microvolt = <3300000>;
    enable-active-high;
    gpio = <&gpio1 RK_PA4 GPIO_ACTIVE_HIGH>;
    startup-delay-us = <5000>;
    vin-supply = <&dc_12v>;
};
```



#### 备份003

```
&pcie3x1 {
	// num-lanes = <1>;
	bus-range = <0x0 0xf>;
	//rockchip,bifurcation;
	// reset-gpios = <&gpio2 RK_PD6 GPIO_ACTIVE_HIGH>;
	reset-gpios = <&gpio3 RK_PA1 GPIO_ACTIVE_HIGH>;
	reset-delay-ms = <100>;
	rockchip,pipe-grf = <&pipegrf>;
	rockchip,pipe-phy-grf = <&pipe_phy_grf0>;

	// rockchip,perst-inactive-ms = <500>;

	vpcie3v3-supply = <&vcc3v3_pcie>;

	prsnt-gpios = <&gpio4 RK_PB7 GPIO_ACTIVE_LOW>;

	// PCIe 唤醒、时钟请求的 GPIO 定义（若驱动支持）
	// 注意：部分驱动不直接使用这些信号，而是通过硬件连线
	wake-gpios = <&gpio2 RK_PD3 GPIO_ACTIVE_LOW>;   // WAKEN 信号：GPIO2_D3
	clkreq-gpios = <&gpio2 RK_PD2 GPIO_ACTIVE_HIGH>; // CLKREQN 信号：GPIO2_D2

	// 新增 WAKEN、CLKREQN 信号映射
	// 这些信号通常用于 PCIe 电源管理（PME#唤醒、时钟请求）
	pinctrl-names = "default";
	pinctrl-0 = <&pcie30x1m1_pins>;

	status = "disabled";
};
```



#### 备份002

```
&pcie30phy {
	rockchip,bifurcation;
	//rockchip,pcie30-phymode = <PHY_MODE_PCIE_NABIBI>;
	rockchip,pipe-grf = <&pipegrf>;

	// rockchip,pipe-grf = <&pcie30_phy_grf>;

	// rockchip,pipe-grf = <&pipegrf>;
	// rockchip,pipe-phy-grf = <&pipe_phy_grf0>;

	status = "disabled";

};

&pcie3x1 {
	rockchip,bifurcation;
	// reset-gpios = <&gpio3 RK_PA1 GPIO_ACTIVE_HIGH>;
	// reset-delay-ms = <100>;
	vpcie3v3-supply = <&vcc3v3_pcie>;

	// prsnt-gpios = <&gpio0 0 GPIO_ACTIVE_HIGH>;

	// PCIe 唤醒、时钟请求的 GPIO 定义（若驱动支持）
	// 注意：部分驱动不直接使用这些信号，而是通过硬件连线
	// wake-gpios = <&gpio2 RK_PD3 GPIO_ACTIVE_LOW>;   // WAKEN 信号：GPIO2_D3
	// clkreq-gpios = <&gpio2 RK_PD2 GPIO_ACTIVE_HIGH>; // CLKREQN 信号：GPIO2_D2

	// 新增 WAKEN、CLKREQN 信号映射
	// 这些信号通常用于 PCIe 电源管理（PME#唤醒、时钟请求）
	//pinctrl-names = "default";
	//pinctrl-0 = <&pcie30x1m1_pins>;

	status = "disabled";
};

&pcie3x2 {

	bus-range = <0x0 0xf>;
	rockchip,bifurcation;
	// reset-gpios = <&gpio2 RK_PD6 GPIO_ACTIVE_HIGH>;
	reset-gpios = <&gpio3 RK_PA1 GPIO_ACTIVE_HIGH>;
	reset-delay-ms = <100>;

	// rockchip,perst-inactive-ms = <500>;

	vpcie3v3-supply = <&vcc3v3_pcie>;

	prsnt-gpios = <&gpio4 RK_PB7 GPIO_ACTIVE_LOW>;

	// PCIe 唤醒、时钟请求的 GPIO 定义（若驱动支持）
	// 注意：部分驱动不直接使用这些信号，而是通过硬件连线
	wake-gpios = <&gpio2 RK_PD3 GPIO_ACTIVE_LOW>;   // WAKEN 信号：GPIO2_D3
	clkreq-gpios = <&gpio2 RK_PD2 GPIO_ACTIVE_HIGH>; // CLKREQN 信号：GPIO2_D2

	// 新增 WAKEN、CLKREQN 信号映射
	// 这些信号通常用于 PCIe 电源管理（PME#唤醒、时钟请求）
	pinctrl-names = "default";
	pinctrl-0 = <&pcie30x2m1_pins>;

	status = "disabled";
};
```



#### 备份001

```
&pcie3x1 {
	rockchip,bifurcation;
	reset-gpios = <&gpio3 RK_PA1 GPIO_ACTIVE_HIGH>;
	vpcie3v3-supply = <&vcc3v3_pcie>;
	status = "okay";

	// 新增 WAKEN、CLKREQN 信号映射
	// 这些信号通常用于 PCIe 电源管理（PME#唤醒、时钟请求）
	pinctrl-names = "default";
	pinctrl-0 = <&pcie30x1_waken_m1
		     &pcie30x1_clkreqn_m1
		     &pcie30x1_perstn_m1>;

	// PCIe 唤醒、时钟请求的 GPIO 定义（若驱动支持）
	// 注意：部分驱动不直接使用这些信号，而是通过硬件连线
	// wake-gpios = <&gpio2 RK_PD3 GPIO_ACTIVE_LOW>;   // WAKEN 信号：GPIO2_D3
	//clkreq-gpios = <&gpio2 RK_PD2 GPIO_ACTIVE_LOW>; // CLKREQN 信号：GPIO2_D2
	//prsnt-gpios = <&gpio3 RK_PA1 GPIO_ACTIVE_HIGH>;

};

&pcie3x2 {
	reset-gpios = <&gpio3 RK_PA1 GPIO_ACTIVE_HIGH>;
	vpcie3v3-supply = <&vcc3v3_pcie>;
	status = "disabled";
};


hp-det-gpio = <&gpio0 RK_PA0 GPIO_ACTIVE_LOW>;
```



#### 引脚冲突报错

```
[ 2.599790] rockchip-pinctrl pinctrl: pin-90 (3c0800000.pcie) status -22 
[ 2.599802] rockchip-pinctrl pinctrl: could not request pin 90 (gpio2-26) from group pcie30x1m1-pins on device rockchip-pinctrl [ 2.599812] rk-pcie 3c0800000.pcie: Error applying setting, reverse things back 
[ 2.599836] rk-pcie: probe of 3c0800000.pcie failed with error -22

```

#### pcie3x2 报错记录



```
[    2.600517] rk-pcie 3c0800000.pcie: invalid prsnt-gpios property in node
[    2.600528] rk-pcie 3c0800000.pcie: ---------------pcie: ret 0 ----------------
[    2.600564] rk-pcie 3c0800000.pcie: no vpcie3v3 regulator found
[    2.609512] snps pcie3phy FW update! size 8192
[    2.623574] rk-pcie 3c0800000.pcie: IRQ msi not found
[    2.623592] rk-pcie 3c0800000.pcie: use outband MSI support
[    2.623602] rk-pcie 3c0800000.pcie: Missing *config* reg space
[    2.623623] rk-pcie 3c0800000.pcie: host bridge /pcie@fe280000 ranges:
[    2.623667] rk-pcie 3c0800000.pcie:      err 0x00f0000000..0x00f00fffff -> 0x00f0000000
[    2.623689] rk-pcie 3c0800000.pcie:       IO 0x00f0100000..0x00f01fffff -> 0x00f0100000
[    2.623709] rk-pcie 3c0800000.pcie:      MEM 0x00f0200000..0x00f1ffffff -> 0x00f0200000
[    2.623729] rk-pcie 3c0800000.pcie:      MEM 0x0380000000..0x03bfffffff -> 0x0380000000
[    2.623770] rk-pcie 3c0800000.pcie: Missing *config* reg space
[    2.623811] rk-pcie 3c0800000.pcie: invalid resource
[    2.829037] rk-pcie 3c0800000.pcie: PCIe Linking... LTSSM is 0x3
[    2.854577] rk-pcie 3c0800000.pcie: PCIe Linking... LTSSM is 0x3
[    2.881250] rk-pcie 3c0800000.pcie: PCIe Linking... LTSSM is 0x3
[    2.907915] rk-pcie 3c0800000.pcie: PCIe Linking... LTSSM is 0x3
[    2.934582] rk-pcie 3c0800000.pcie: PCIe Linking... LTSSM is 0x3
[    2.961245] rk-pcie 3c0800000.pcie: PCIe Linking... LTSSM is 0x3
[    2.987932] rk-pcie 3c0800000.pcie: PCIe Linking... LTSSM is 0x3
[    3.014608] rk-pcie 3c0800000.pcie: PCIe Linking... LTSSM is 0x3
[    3.041274] rk-pcie 3c0800000.pcie: PCIe Linking... LTSSM is 0x3
[    3.067930] rk-pcie 3c0800000.pcie: PCIe Linking... LTSSM is 0x3
[    5.511383] rk-pcie 3c0800000.pcie: PCIe Link Fail, LTSSM is 0x3, hw_retries=0
[    7.854717] rk_pcie_establish_link: 132 callbacks suppressed
[    7.854736] rk-pcie 3c0800000.pcie: PCIe Linking... LTSSM is 0x3
[    7.881449] rk-pcie 3c0800000.pcie: PCIe Linking... LTSSM is 0x3
[    7.908019] rk-pcie 3c0800000.pcie: PCIe Linking... LTSSM is 0x3
[    7.934687] rk-pcie 3c0800000.pcie: PCIe Linking... LTSSM is 0x3
[    7.961296] rk-pcie 3c0800000.pcie: PCIe Linking... LTSSM is 0x3
[    7.988037] rk-pcie 3c0800000.pcie: PCIe Linking... LTSSM is 0x3
[    8.014698] rk-pcie 3c0800000.pcie: PCIe Linking... LTSSM is 0x3
[    8.041329] rk-pcie 3c0800000.pcie: PCIe Linking... LTSSM is 0x3
[    8.067989] rk-pcie 3c0800000.pcie: PCIe Linking... LTSSM is 0x3
[    8.098108] rk-pcie 3c0800000.pcie: PCIe Linking... LTSSM is 0x3
[    9.434641] rk-pcie 3c0800000.pcie: PCIe Link Fail, LTSSM is 0x3, hw_retries=1
[   10.441351] rk-pcie 3c0800000.pcie: failed to initialize host

```



#### 成功日志

```
root@rk3568-buildroot:/# fdisk -l
Disk /dev/nvme0n1: 466 GB, 500107862016 bytes, 976773168 sectors
476940 cylinders, 64 heads, 32 sectors/track
Units: sectors of 1 * 512 = 512 bytes

Disk /dev/nvme0n1 doesn't contain a valid partition table
Found valid GPT with protective MBR; using GPT

Disk /dev/mmcblk0: 15269888 sectors, 3360M
Logical sector size: 512
Disk identifier (GUID): c0170000-0000-4961-8000-610500004121
Partition table holds up to 128 entries
First usable sector is 34, last usable sector is 15269854

Number  Start (sector)    End (sector)  Size Name
     1           16384           24575 4096K uboot
     2           24576           32767 4096K misc
     3           32768          163839 64.0M boot
     4          163840          425983  128M recovery
     5          425984          491519 32.0M backup
     6          491520        13074431 6144M rootfs
     7        13074432        13336575  128M oem
     8        13336576        15269854  943M userdata
root@rk3568-buildroot:/# lspci
0002:20:00.0 PCI bridge: Fuzhou Rockchip Electronics Co., Ltd Device 3566 (rev 01)
0002:21:00.0 Non-Volatile memory controller: Sandisk Corp Device 5019

root@rk3568-buildroot:/# dmesg | grep pci
[    1.951421] pcie30_avdd0v9: supplied by vcc3v3_sys
[    1.951726] pcie30_avdd1v8: supplied by vcc3v3_sys
[    1.952057] vcc3v3_pcie: supplied by dc_12v
[    2.599245] rk-pcie 3c0800000.pcie: ---------------pcie: 11----------------
[    2.599291] rk-pcie 3c0800000.pcie: ---------------pcie: rk_pcie->bifurcation 1 ----------------
[    2.599408] rk-pcie 3c0800000.pcie: ---------------pcie: rk_pcie->rst_gpio 0 ----------------
[    2.599450] rk-pcie 3c0800000.pcie: invalid prsnt-gpios property in node
[    2.599462] rk-pcie 3c0800000.pcie: ---------------pcie: ret 0 ----------------
[    2.608752] snps pcie3phy FW update! size 8192
[    2.622933] rk-pcie 3c0800000.pcie: IRQ msi not found
[    2.622965] rk-pcie 3c0800000.pcie: use outband MSI support
[    2.622976] rk-pcie 3c0800000.pcie: Missing *config* reg space
[    2.623003] rk-pcie 3c0800000.pcie: host bridge /pcie@fe280000 ranges:
[    2.623037] rk-pcie 3c0800000.pcie:      err 0x00f0000000..0x00f00fffff -> 0x00f0000000
[    2.623063] rk-pcie 3c0800000.pcie:       IO 0x00f0100000..0x00f01fffff -> 0x00f0100000
[    2.623088] rk-pcie 3c0800000.pcie:      MEM 0x00f0200000..0x00f1ffffff -> 0x00f0200000
[    2.623106] rk-pcie 3c0800000.pcie:      MEM 0x0380000000..0x03bfffffff -> 0x0380000000
[    2.623151] rk-pcie 3c0800000.pcie: Missing *config* reg space
[    2.623195] rk-pcie 3c0800000.pcie: invalid resource
[    2.814097] ehci-pci: EHCI PCI platform driver
[    2.885110] rk-pcie 3c0800000.pcie: PCIe Link up, LTSSM is 0x30011
[    2.885135] rk-pcie 3c0800000.pcie: ltssm = 0x30011
[    2.885147] rk-pcie 3c0800000.pcie: fifo_status = 0x150000
[    2.885157] rk-pcie 3c0800000.pcie: fifo_status = 0x140001
[    2.885167] rk-pcie 3c0800000.pcie: fifo_status = 0x130002
[    2.885176] rk-pcie 3c0800000.pcie: fifo_status = 0x120004
[    2.885184] rk-pcie 3c0800000.pcie: fifo_status = 0x110007
[    2.885193] rk-pcie 3c0800000.pcie: fifo_status = 0x100008
[    2.885201] rk-pcie 3c0800000.pcie: fifo_status = 0xf0009
[    2.885209] rk-pcie 3c0800000.pcie: fifo_status = 0xe000a
[    2.885218] rk-pcie 3c0800000.pcie: fifo_status = 0xd000b
[    2.885226] rk-pcie 3c0800000.pcie: fifo_status = 0xc000c
[    2.885234] rk-pcie 3c0800000.pcie: fifo_status = 0xb0011
[    2.885242] rk-pcie 3c0800000.pcie: fifo_status = 0xa000d
[    2.885250] rk-pcie 3c0800000.pcie: fifo_status = 0x9000f
[    2.885258] rk-pcie 3c0800000.pcie: fifo_status = 0x8000e
[    2.885266] rk-pcie 3c0800000.pcie: fifo_status = 0x7000d
[    2.885274] rk-pcie 3c0800000.pcie: fifo_status = 0x60021
[    2.885282] rk-pcie 3c0800000.pcie: fifo_status = 0x5000e
[    2.885290] rk-pcie 3c0800000.pcie: fifo_status = 0x4000d
[    2.885297] rk-pcie 3c0800000.pcie: fifo_status = 0x3000f
[    2.885305] rk-pcie 3c0800000.pcie: fifo_status = 0x20010
[    2.885313] rk-pcie 3c0800000.pcie: fifo_status = 0x10011
[    2.885320] rk-pcie 3c0800000.pcie: fifo_status = 0x173e
[    2.885328] rk-pcie 3c0800000.pcie: fifo_status = 0x173e
[    2.885336] rk-pcie 3c0800000.pcie: fifo_status = 0x173e
[    2.885343] rk-pcie 3c0800000.pcie: fifo_status = 0x173e
[    2.885351] rk-pcie 3c0800000.pcie: fifo_status = 0x173e
[    2.885360] rk-pcie 3c0800000.pcie: fifo_status = 0x173e
[    2.885367] rk-pcie 3c0800000.pcie: fifo_status = 0x173e
[    2.885376] rk-pcie 3c0800000.pcie: fifo_status = 0x173e
[    2.885384] rk-pcie 3c0800000.pcie: fifo_status = 0x173e
[    2.885392] rk-pcie 3c0800000.pcie: fifo_status = 0x173e
[    2.885400] rk-pcie 3c0800000.pcie: fifo_status = 0x173e
[    2.885409] rk-pcie 3c0800000.pcie: fifo_status = 0x173e
[    2.885417] rk-pcie 3c0800000.pcie: fifo_status = 0x173e
[    2.885425] rk-pcie 3c0800000.pcie: fifo_status = 0x173e
[    2.885433] rk-pcie 3c0800000.pcie: fifo_status = 0x173e
[    2.885442] rk-pcie 3c0800000.pcie: fifo_status = 0x173e
[    2.885450] rk-pcie 3c0800000.pcie: fifo_status = 0x173e
[    2.885458] rk-pcie 3c0800000.pcie: fifo_status = 0x173e
[    2.885465] rk-pcie 3c0800000.pcie: fifo_status = 0x173e
[    2.885474] rk-pcie 3c0800000.pcie: fifo_status = 0x173e
[    2.885482] rk-pcie 3c0800000.pcie: fifo_status = 0x173e
[    2.885489] rk-pcie 3c0800000.pcie: fifo_status = 0x173e
[    2.885497] rk-pcie 3c0800000.pcie: fifo_status = 0x173e
[    2.885507] rk-pcie 3c0800000.pcie: fifo_status = 0x173e
[    2.885515] rk-pcie 3c0800000.pcie: fifo_status = 0x173e
[    2.885523] rk-pcie 3c0800000.pcie: fifo_status = 0x173e
[    2.885531] rk-pcie 3c0800000.pcie: fifo_status = 0x173e
[    2.885539] rk-pcie 3c0800000.pcie: fifo_status = 0x173e
[    2.885548] rk-pcie 3c0800000.pcie: fifo_status = 0x173e
[    2.885555] rk-pcie 3c0800000.pcie: fifo_status = 0x173e
[    2.885563] rk-pcie 3c0800000.pcie: fifo_status = 0x173e
[    2.885571] rk-pcie 3c0800000.pcie: fifo_status = 0x173e
[    2.885578] rk-pcie 3c0800000.pcie: fifo_status = 0x173e
[    2.885586] rk-pcie 3c0800000.pcie: fifo_status = 0x173e
[    2.885593] rk-pcie 3c0800000.pcie: fifo_status = 0x173e
[    2.885601] rk-pcie 3c0800000.pcie: fifo_status = 0x173e
[    2.885608] rk-pcie 3c0800000.pcie: fifo_status = 0x173e
[    2.885616] rk-pcie 3c0800000.pcie: fifo_status = 0x173e
[    2.885624] rk-pcie 3c0800000.pcie: fifo_status = 0x173e
[    2.885633] rk-pcie 3c0800000.pcie: fifo_status = 0x173e
[    2.885641] rk-pcie 3c0800000.pcie: fifo_status = 0x173e
[    2.885648] rk-pcie 3c0800000.pcie: fifo_status = 0x173e
[    2.885656] rk-pcie 3c0800000.pcie: fifo_status = 0x173e
[    2.885815] rk-pcie 3c0800000.pcie: PCI host bridge to bus 0002:20
[    2.885831] pci_bus 0002:20: root bus resource [bus 20-2f]
[    2.885844] pci_bus 0002:20: root bus resource [??? 0xf0000000-0xf00fffff flags 0x0]
[    2.885856] pci_bus 0002:20: root bus resource [io  0x0000-0xfffff] (bus address [0xf0100000-0xf01fffff])
[    2.885866] pci_bus 0002:20: root bus resource [mem 0xf0200000-0xf1ffffff]
[    2.885876] pci_bus 0002:20: root bus resource [mem 0x380000000-0x3bfffffff pref]
[    2.885920] pci 0002:20:00.0: [1d87:3566] type 01 class 0x060400
[    2.885952] pci 0002:20:00.0: reg 0x38: [mem 0x00000000-0x0000ffff pref]
[    2.886028] pci 0002:20:00.0: supports D1 D2
[    2.886040] pci 0002:20:00.0: PME# supported from D0 D1 D3hot
[    2.897324] pci 0002:20:00.0: Primary bus is hard wired to 0
[    2.897354] pci 0002:20:00.0: bridge configuration invalid ([bus 01-ff]), reconfiguring
[    2.897673] pci 0002:21:00.0: [15b7:5019] type 00 class 0x010802
[    2.897804] pci 0002:21:00.0: reg 0x10: [mem 0x00000000-0x00003fff 64bit]
[    2.897947] pci 0002:21:00.0: reg 0x20: [mem 0x00000000-0x000000ff 64bit]
[    2.898639] pci 0002:21:00.0: 2.000 Gb/s available PCIe bandwidth, limited by 2.5 GT/s PCIe x1 link at 0002:20:00.0 (capable of 31.504 Gb/s with 8.0 GT/s PCIe x4 link)
[    2.939361] pci_bus 0002:21: busn_res: [bus 21-2f] end is updated to 21
[    2.939445] pci 0002:20:00.0: BAR 8: assigned [mem 0xf0200000-0xf02fffff]
[    2.939463] pci 0002:20:00.0: BAR 6: assigned [mem 0xf0300000-0xf030ffff pref]
[    2.939481] pci 0002:21:00.0: BAR 0: assigned [mem 0xf0200000-0xf0203fff 64bit]
[    2.939550] pci 0002:21:00.0: BAR 4: assigned [mem 0xf0204000-0xf02040ff 64bit]
[    2.939610] pci 0002:20:00.0: PCI bridge to [bus 21]
[    2.939624] pci 0002:20:00.0:   bridge window [mem 0xf0200000-0xf02fffff]
[    2.942430] pcieport 0002:20:00.0: PME: Signaling with IRQ 101
[    2.942927] nvme nvme0: pci function 0002:21:00.0

```





https://redmine.rock-chips.com/login
