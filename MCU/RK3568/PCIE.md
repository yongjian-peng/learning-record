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


pcie30_phy_grfs

引脚：
PCIE_PWR_EN GPIO1_A4
PCIE30X1_WAKEN_M1 GPIO2_D3
PCIE30X1_CLKREQN_M1 GPIO2_D2
PCIE30X1_PERSTN_M1 GPIO3_A1
```





备份001

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





https://redmine.rock-chips.com/login
