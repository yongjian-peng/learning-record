```swift
arch/arm64/boot/dts/rockchip/rk3568-evb.dtsi

#按键控制
gpio-keys {
        compatible = "gpio-keys";
        pinctrl-names = "default";
        pinctrl-0 = <&otg_switch_key>;

        otg-switch-key {
            label = "OTG Switch Key";
            linux,code = <KEY_F13>; // 使用一个未使用的键值
            gpios = <&gpio0 RK_PA5 GPIO_ACTIVE_LOW>; // 假设低电平有效
        };
    };

	gpio-keys {
        otg_switch_key: otg-switch-key {
            rockchip,pins = <0 RK_PA5 RK_FUNC_GPIO &pcfg_pull_up>;
        };
    };

```



```

vi /usr/bin/usb_otg_switch.sh
chmod +x /usr/bin/usb_otg_switch.sh

#!/bin/sh

# USB控制器路径
USB_DEBUG_PATH="/sys/kernel/debug/usb/fcc00000.dwc3"
MODE_FILE="$USB_DEBUG_PATH/mode"
UDC_PATH="/sys/class/udc/fcc00000.dwc3"

# 当前模式状态文件
STATE_FILE="/tmp/usb_otg_state"

# 日志文件
LOG_FILE="/var/log/usb-otg-switch.log"

# 现有的gadget配置
GADGET_PATH="/sys/kernel/config/usb_gadget/rockchip"

# 动态获取输入设备
find_input_device() {
    # 查找gpio-keys设备
    for dev in /sys/class/input/event*/device/name; do
        if [ -f "$dev" ] && grep -qi "gpio-keys" "$dev"; then
            # 提取event编号
            event_path=$(echo "$dev" | sed 's|/device/name$||')
            event_num=$(echo "$event_path" | grep -o 'event[0-9]*')
            echo "/dev/input/$event_num"
            return 0
        fi
    done
    
    # 备用方法：通过evtest查找
    for dev in /dev/input/event*; do
        if [ -e "$dev" ]; then
            local name=$(evtest --query "$dev" name 2>/dev/null || echo "")
            if echo "$name" | grep -qi "gpio-keys"; then
                echo "$dev"
                return 0
            fi
        fi
    done
    echo ""
}

# 获取当前输入设备
get_current_input_device() {
    local device=$(find_input_device)
    if [ -n "$device" ] && [ -e "$device" ]; then
        echo "$device"
    else
        echo ""
    fi
}

# USB状态检查
check_usb_status() {
    echo "=== USB Status $(date) ===" >> $LOG_FILE
    echo "Stored mode: $(cat $STATE_FILE 2>/dev/null || echo 'unknown')" >> $LOG_FILE
    if [ -f "$MODE_FILE" ]; then
        echo "Actual mode: $(cat $MODE_FILE 2>/dev/null)" >> $LOG_FILE
    else
        echo "Actual mode: Mode file not available" >> $LOG_FILE
    fi
    echo "UDC status: $(test -e $UDC_PATH && echo 'Exists' || echo 'Missing')" >> $LOG_FILE
    echo "Gadget UDC: $(cat $GADGET_PATH/UDC 2>/dev/null || echo 'Not bound')" >> $LOG_FILE
    
    local current_device=$(get_current_input_device)
    echo "Current input device: $current_device" >> $LOG_FILE
    echo "=== End Status ===" >> $LOG_FILE
}

# 启用gadget功能
enable_gadget() {
    echo "$(date): Enabling USB gadget" >> $LOG_FILE
    
    # 确保gadget目录存在
    if [ ! -d "$GADGET_PATH" ]; then
        echo "$(date): Error: Gadget path $GADGET_PATH does not exist" >> $LOG_FILE
        return 1
    fi
    
    # 绑定到UDC控制器
    echo "fcc00000.dwc3" > $GADGET_PATH/UDC 2>/dev/null
    
    if [ $? -eq 0 ]; then
        echo "$(date): USB gadget enabled successfully" >> $LOG_FILE
        return 0
    else
        echo "$(date): Failed to enable USB gadget" >> $LOG_FILE
        return 1
    fi
}

# 禁用gadget功能
disable_gadget() {
    echo "$(date): Disabling USB gadget" >> $LOG_FILE
    
    # 从UDC控制器解绑
    echo "" > $GADGET_PATH/UDC 2>/dev/null
    
    echo "$(date): USB gadget disabled" >> $LOG_FILE
}

# 初始化状态
init_state() {
    echo "$(date): Initializing USB OTG state" >> $LOG_FILE
    
    # 确保必要的内核模块加载
    modprobe libcomposite 2>/dev/null
    modprobe usb_f_ffs 2>/dev/null
    
    if [ -f "$MODE_FILE" ]; then
        # 强制设置为host模式并禁用gadget
        echo "host" > $MODE_FILE 2>/dev/null
        sleep 1
        disable_gadget
        echo "host" > $STATE_FILE
        echo "$(date): Initialized to HOST mode" >> $LOG_FILE
    else
        echo "host" > $STATE_FILE
        echo "$(date): Mode file not found, defaulting to HOST" >> $LOG_FILE
    fi
    
    check_usb_status
}

# 切换USB模式
switch_usb_mode() {
    current_mode=$(cat $STATE_FILE 2>/dev/null || echo "host")
    
    if [ "$current_mode" = "device" ]; then
        # 切换到host模式
        new_mode="host"
        echo "$(date): Switching to HOST mode" >> $LOG_FILE
        
        # 先禁用gadget
        disable_gadget
        
        # 等待一下确保gadget完全禁用
        sleep 1
        
        # 切换到host模式
        echo "host" > $MODE_FILE 2>/dev/null
        
        if [ $? -eq 0 ]; then
            echo "host" > $STATE_FILE
            echo "$(date): Successfully switched to HOST mode" >> $LOG_FILE
            echo "Switched to HOST mode"
        else
            echo "$(date): Failed to switch to HOST mode" >> $LOG_FILE
            echo "Failed to switch to HOST mode"
        fi
        
    else
        # 切换到device模式
        new_mode="device"
        echo "$(date): Switching to DEVICE mode" >> $LOG_FILE
        
        # 先确保在host模式
        echo "host" > $MODE_FILE 2>/dev/null
        sleep 1
        
        # 切换到device模式
        echo "device" > $MODE_FILE 2>/dev/null
        
        if [ $? -eq 0 ]; then
            echo "device" > $STATE_FILE
            echo "$(date): USB controller set to DEVICE mode" >> $LOG_FILE
            
            # 等待控制器稳定
            sleep 2
            
            # 启用gadget
            if enable_gadget; then
                echo "$(date): Successfully switched to DEVICE mode with gadget enabled" >> $LOG_FILE
                echo "Switched to DEVICE mode"
            else
                echo "$(date): Switched to DEVICE mode but failed to enable gadget" >> $LOG_FILE
                echo "Switched to DEVICE mode (gadget activation failed)"
            fi
        else
            echo "$(date): Failed to switch to DEVICE mode" >> $LOG_FILE
            echo "Failed to switch to DEVICE mode"
        fi
    fi
    
    check_usb_status
}

# 监听按键事件
listen_for_key() {
    echo "$(date): Starting USB OTG switch daemon" >> $LOG_FILE
    
    while true; do
        # 每次循环都重新获取输入设备
        INPUT_DEVICE=$(get_current_input_device)
        
        if [ -z "$INPUT_DEVICE" ] || [ ! -e "$INPUT_DEVICE" ]; then
            echo "$(date): Error: No suitable input device found, retrying in 5 seconds..." >> $LOG_FILE
            sleep 5
            continue
        fi
        
        echo "$(date): Using input device: $INPUT_DEVICE" >> $LOG_FILE
        check_usb_status
        
        echo "$(date): Listening for KEY_F13 events on $INPUT_DEVICE" >> $LOG_FILE
        
        # 使用timeout来定期检查设备状态
        timeout 30 evtest --grab "$INPUT_DEVICE" | while read line; do
            if echo "$line" | grep -q "code 183 (KEY_F13).*value 1"; then
                echo "$(date): OTG switch key pressed (KEY_F13)" >> $LOG_FILE
                switch_usb_mode
            fi
        done
        
        # 如果evtest退出，等待一下然后重新开始
        echo "$(date): evtest exited, rechecking input device in 2 seconds..." >> $LOG_FILE
        sleep 2
    done
}

# 手动测试函数
test_switch() {
    echo "$(date): Manual mode switch test" >> $LOG_FILE
    switch_usb_mode
}

# 主程序
case "$1" in
    start)
        init_state
        listen_for_key
        ;;
    stop)
        echo "$(date): Stopping USB OTG switch daemon" >> $LOG_FILE
        pkill -f "usb_otg_switch.sh"
        # 确保回到host模式
        echo "host" > $MODE_FILE 2>/dev/null
        disable_gadget
        ;;
    status)
        check_usb_status
        echo "Stored mode: $(cat $STATE_FILE 2>/dev/null || echo 'unknown')"
        if [ -f "$MODE_FILE" ]; then
            echo "Actual mode: $(cat $MODE_FILE 2>/dev/null)"
        else
            echo "Actual mode: Mode file not available"
        fi
        echo "Gadget UDC: $(cat $GADGET_PATH/UDC 2>/dev/null || echo 'Not bound')"
        echo "Current input device: $(get_current_input_device)"
        ;;
    switch)
        test_switch
        ;;
    enable-gadget)
        enable_gadget
        ;;
    disable-gadget)
        disable_gadget
        ;;
    find-device)
        # 查找输入设备
        echo "Searching for input devices..."
        for dev in /sys/class/input/event*/device/name; do
            if [ -f "$dev" ]; then
                event_path=$(echo "$dev" | sed 's|/device/name$||')
                event_num=$(echo "$event_path" | grep -o 'event[0-9]*')
                name=$(cat "$dev")
                echo "/dev/input/$event_num: $name"
            fi
        done
        found_device=$(get_current_input_device)
        echo "Selected device: $found_device"
        ;;
    *)
        echo "Usage: $0 {start|stop|status|switch|enable-gadget|disable-gadget|find-device}"
        echo "  start           - Start listening for key events"
        echo "  stop            - Stop the daemon"
        echo "  status          - Show current USB mode status"
        echo "  switch          - Manually switch USB mode"
        echo "  enable-gadget   - Enable USB gadget function"
        echo "  disable-gadget  - Disable USB gadget function"
        echo "  find-device     - Find available input devices"
        exit 1
        ;;
esac

exit 0
```



```
root@rk3568-buildroot:/# cat /etc/init.d/S99usbotgswitch
#!/bin/sh
DAEMON="/usr/bin/usb_otg_switch.sh"
NAME="usb-otg-switch"
PIDFILE="/var/run/$NAME.pid"
DESC="USB OTG Mode Switch"

case "$1" in
    start)
        echo "Starting $DESC: $NAME"
        if [ -f $PIDFILE ] && kill -0 $(cat $PIDFILE) 2>/dev/null; then
            echo "$NAME is already running"
            exit 1
        fi
        start-stop-daemon -S -b -m -p $PIDFILE -x $DAEMON -- start
        ;;
    stop)
        echo "Stopping $DESC: $NAME"
        start-stop-daemon -K -p $PIDFILE 2>/dev/null
        rm -f $PIDFILE 2>/dev/null
        ;;
    restart)
        echo "Restarting $DESC: $NAME"
        $0 stop
        sleep 1
        $0 start
        ;;
    status)
        if [ -f $PIDFILE ] && kill -0 $(cat $PIDFILE) 2>/dev/null; then
            echo "$NAME is running"
            $DAEMON status
        else
            echo "$NAME is not running"
        fi
        ;;
    *)
        echo "Usage: $0 {start|stop|restart|status}"
        exit 1
        ;;
esac

exit 0

```

```
chmod +x /etc/init.d/S99usbotgswitch
```



测试脚本

```
# 创建日志目录
mkdir -p /var/log

# 测试脚本功能
/usr/bin/usb_otg_switch.sh status
/usr/bin/usb_otg_switch.sh switch

# 测试启动脚本
/etc/init.d/S99usbotgswitch status
/etc/init.d/S99usbotgswitch start

# 查看日志
cat /var/log/usb-otg-switch.log

# 检查进程
ps aux | grep usb_otg_switch
```



测试按键

```
# 在前台测试
/usr/bin/usb_otg_switch.sh start

# 在另一个终端查看状态
/usr/bin/usb_otg_switch.sh status
```



出现问题

```
# 检查USB模式文件是否存在
ls -la /sys/kernel/debug/usb/fcc00000.dwc3/mode

# 检查输入设备
ls -la /dev/input/event5

# 手动测试模式切换
echo "device" > /sys/kernel/debug/usb/fcc00000.dwc3/mode
echo "host" > /sys/kernel/debug/usb/fcc00000.dwc3/mode

# 检查当前模式
cat /sys/kernel/debug/usb/fcc00000.dwc3/mode
```

