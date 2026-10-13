



# 设置代理

```
export http_proxy=http://127.0.0.1:7897
export https_proxy=http://127.0.0.1:7897
export all_proxy=http://127.0.0.1:7897

export HTTP_PROXY=$http_proxy
export HTTPS_PROXY=$https_proxy
export ALL_PROXY=$all_proxy
```



# 调整网络优先级

```
打开图形界面
nm-connection-editor
sudo apt install network-manager-gnome


```

# 命令

```
# 查看是否插入网线 1 是 0 否
watch -n 1 cat /sys/class/net/eth1/carrier

# 查看 网卡的 mac 地址
 cat /sys/class/net/eth0/address
```



# Modbus 协议

```
MODBUS-TCP、OPCUA 协议 使用
```

