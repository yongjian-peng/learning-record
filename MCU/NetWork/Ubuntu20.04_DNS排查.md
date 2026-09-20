# 操作过程

## 基础信息

```
heli@heli:~$ uname -a
Linux heli 5.15.0-139-generic #149~20.04.1-Ubuntu SMP Wed Apr 16 08:29:56 UTC 2025 x86_64 x86_64 x86_64 GNU/Linux
heli@heli:~$ cat /etc/os-release
NAME="Ubuntu"
VERSION="20.04.6 LTS (Focal Fossa)"
ID=ubuntu
ID_LIKE=debian
PRETTY_NAME="Xubuntu 20.04.6  LTS amd64 (Cubic 2026-08-03 17:19)"
VERSION_ID="20.04"
HOME_URL="https://www.ubuntu.com/"
SUPPORT_URL="https://help.ubuntu.com/"
BUG_REPORT_URL="https://bugs.launchpad.net/ubuntu/"
PRIVACY_POLICY_URL="https://www.ubuntu.com/legal/terms-and-policies/privacy-policy"
VERSION_CODENAME=focal
UBUNTU_CODENAME=focal
heli@heli:~$ uname -r
5.15.0-139-generic
heli@heli:~$ ifconfig
enp1s0: flags=4099<UP,BROADCAST,MULTICAST>  mtu 1500
        ether 50:0a:52:0c:f5:28  txqueuelen 1000  (Ethernet)
        RX packets 0  bytes 0 (0.0 B)
        RX errors 0  dropped 0  overruns 0  frame 0
        TX packets 0  bytes 0 (0.0 B)
        TX errors 0  dropped 0 overruns 0  carrier 0  collisions 0
        device memory 0x50600000-5061ffff

enp2s0: flags=4163<UP,BROADCAST,RUNNING,MULTICAST>  mtu 1500
        inet 192.168.31.85  netmask 255.255.255.0  broadcast 192.168.31.255
        inet6 2408:8220:33:2520:380d:d68d:389b:d9d  prefixlen 64  scopeid 0x0<global>
        inet6 fe80::8bef:cd21:7cca:e6d8  prefixlen 64  scopeid 0x20<link>
        ether 50:0a:52:0c:f5:29  txqueuelen 1000  (Ethernet)
        RX packets 81961  bytes 30400711 (30.4 MB)
        RX errors 0  dropped 757  overruns 0  frame 0
        TX packets 1497  bytes 457806 (457.8 KB)
        TX errors 0  dropped 0 overruns 0  carrier 0  collisions 0
        device memory 0x50500000-5051ffff

lo: flags=73<UP,LOOPBACK,RUNNING>  mtu 65536
        inet 127.0.0.1  netmask 255.0.0.0
        inet6 ::1  prefixlen 128  scopeid 0x10<host>
        loop  txqueuelen 1000  (Local Loopback)
        RX packets 413291987  bytes 37504322076 (37.5 GB)
        RX errors 0  dropped 0  overruns 0  frame 0
        TX packets 413291987  bytes 37504322076 (37.5 GB)
        TX errors 0  dropped 0 overruns 0  carrier 0  collisions 0

heli@heli:~$ ping baidu.cm
ping: baidu.cm: Temporary failure in name resolution

```



##  默认路由问题、DNS 问题，还是 192.168.31.1 上级网络本身没有公网出口

```
heli@heli:~$ echo "===== 1. IP 地址 ====="
===== 1. IP 地址 =====
heli@heli:~$ ip addr show enp2s0

echo "===== 2. 路由表 ====="
ip route
3: enp2s0: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc mq state UP group default qlen 1000
    link/ether 50:0a:52:0c:f5:29 brd ff:ff:ff:ff:ff:ff
    inet 192.168.31.85/24 brd 192.168.31.255 scope global dynamic noprefixroute enp2s0
       valid_lft 3672sec preferred_lft 3672sec
    inet6 2408:8220:33:2520:380d:d68d:389b:d9d/64 scope global dynamic noprefixroute
       valid_lft 202088sec preferred_lft 115688sec
    inet6 fe80::8bef:cd21:7cca:e6d8/64 scope link noprefixroute
       valid_lft forever preferred_lft forever
heli@heli:~$
heli@heli:~$ echo "===== 2. 路由表 ====="
===== 2. 路由表 =====
heli@heli:~$ ip route
default via 192.168.31.1 dev enp2s0 proto dhcp metric 100
169.254.0.0/16 dev enp2s0 scope link metric 1000
192.168.31.0/24 dev enp2s0 proto kernel scope link src 192.168.31.85 metric 100
heli@heli:~$
heli@heli:~$ echo "===== 3. 测试网关 ====="
===== 3. 测试网关 =====
heli@heli:~$ ping -c 4 192.168.31.1
echo "===== 4. 测试公网 IP ====="
ping -c 4 223.5.5.5

echo "===== 5. 测试 DNS ====="
ping -c 4 baidu.com

echo "===== 6. DNS 配置 ====="
cat /etc/resolv.conf

echo "===== 7. systemd DNS 状态 ====="
systemd-resolve --status 2>/dev/null || resolvectl status

echo "===== 8. NetworkManager ====="
nmcli device status
nmcli device show enp2s0 | grep -E 'IP4|GENERAL.CONNECTION'PING 192.168.31.1 (192.168.31.1) 56(84) bytes of data.
64 bytes from 192.168.31.1: icmp_seq=1 ttl=64 time=0.316 ms
64 bytes from 192.168.31.1: icmp_seq=2 ttl=64 time=0.314 ms
64 bytes from 192.168.31.1: icmp_seq=3 ttl=64 time=0.333 ms
64 bytes from 192.168.31.1: icmp_seq=4 ttl=64 time=0.317 ms

--- 192.168.31.1 ping statistics ---
4 packets transmitted, 4 received, 0% packet loss, time 3058ms
rtt min/avg/max/mdev = 0.314/0.320/0.333/0.007 ms
heli@heli:~$
heli@heli:~$ echo "===== 4. 测试公网 IP ====="
===== 4. 测试公网 IP =====
heli@heli:~$ ping -c 4 223.5.5.5
PING 223.5.5.5 (223.5.5.5) 56(84) bytes of data.
64 bytes from 223.5.5.5: icmp_seq=1 ttl=118 time=17.0 ms
64 bytes from 223.5.5.5: icmp_seq=2 ttl=118 time=15.9 ms
64 bytes from 223.5.5.5: icmp_seq=3 ttl=118 time=16.3 ms
64 bytes from 223.5.5.5: icmp_seq=4 ttl=118 time=16.1 ms

--- 223.5.5.5 ping statistics ---
4 packets transmitted, 4 received, 0% packet loss, time 3004ms
rtt min/avg/max/mdev = 15.900/16.330/17.035/0.428 ms
heli@heli:~$
heli@heli:~$ echo "===== 5. 测试 DNS ====="
===== 5. 测试 DNS =====
heli@heli:~$ ping -c 4 baidu.com
ping: baidu.com: Temporary failure in name resolution
heli@heli:~$
heli@heli:~$ echo "===== 6. DNS 配置 ====="
===== 6. DNS 配置 =====
heli@heli:~$ cat /etc/resolv.conf
# This file is managed by man:systemd-resolved(8). Do not edit.
#
# This is a dynamic resolv.conf file for connecting local clients to the
# internal DNS stub resolver of systemd-resolved. This file lists all
# configured search domains.
#
# Run "resolvectl status" to see details about the uplink DNS servers
# currently in use.
#
# Third party programs must not access this file directly, but only through the
# symlink at /etc/resolv.conf. To manage man:resolv.conf(5) in a different way,
# replace this symlink by a static file or a different symlink.
#
# See man:systemd-resolved.service(8) for details about the supported modes of
# operation for /etc/resolv.conf.

nameserver 127.0.0.53
options edns0 trust-ad
heli@heli:~$
heli@heli:~$ echo "===== 7. systemd DNS 状态 ====="
===== 7. systemd DNS 状态 =====
heli@heli:~$ systemd-resolve --status 2>/dev/null || resolvectl status
Global
       LLMNR setting: no
MulticastDNS setting: no
  DNSOverTLS setting: no
      DNSSEC setting: no
    DNSSEC supported: no
          DNSSEC NTA: 10.in-addr.arpa
                      16.172.in-addr.arpa
                      168.192.in-addr.arpa
                      17.172.in-addr.arpa
                      18.172.in-addr.arpa
                      19.172.in-addr.arpa
                      20.172.in-addr.arpa
                      21.172.in-addr.arpa
                      22.172.in-addr.arpa
                      23.172.in-addr.arpa
                      24.172.in-addr.arpa
                      25.172.in-addr.arpa
                      26.172.in-addr.arpa
                      27.172.in-addr.arpa
                      28.172.in-addr.arpa
                      29.172.in-addr.arpa
                      30.172.in-addr.arpa
                      31.172.in-addr.arpa
                      corp
                      d.f.ip6.arpa
                      home
                      internal
                      intranet
                      lan
                      local
                      private
                      test

Link 3 (enp2s0)
      Current Scopes: DNS
DefaultRoute setting: yes
       LLMNR setting: yes
MulticastDNS setting: no
  DNSOverTLS setting: no
      DNSSEC setting: no
    DNSSEC supported: no
         DNS Servers: 222.85.85.85
                      222.88.88.88
                      223.5.5.5
                      114.114.114.114
          DNS Domain: ~.

Link 2 (enp1s0)
      Current Scopes: none
DefaultRoute setting: no
       LLMNR setting: yes
MulticastDNS setting: no
  DNSOverTLS setting: no
      DNSSEC setting: no
    DNSSEC supported: no
heli@heli:~$
heli@heli:~$ echo "===== 8. NetworkManager ====="
===== 8. NetworkManager =====
heli@heli:~$ nmcli device status
DEVICE  TYPE      STATE        CONNECTION
enp2s0  ethernet  connected    Wired connection 2
enp1s0  ethernet  unavailable  --
lo      loopback  unmanaged    --
heli@heli:~$ nmcli device show enp2s0 | grep -E 'IP4|GENERAL.CONNECTION'
GENERAL.CONNECTION:                     Wired connection 2
IP4.ADDRESS[1]:                         192.168.31.85/24
IP4.GATEWAY:                            192.168.31.1
IP4.ROUTE[1]:                           dst = 0.0.0.0/0, nh = 192.168.31.1, mt = 100
IP4.ROUTE[2]:                           dst = 192.168.31.0/24, nh = 0.0.0.0, mt = 100
IP4.ROUTE[3]:                           dst = 169.254.0.0/16, nh = 0.0.0.0, mt = 1000
IP4.DNS[1]:                             222.85.85.85
IP4.DNS[2]:                             222.88.88.88
IP4.DNS[3]:                             223.5.5.5
IP4.DNS[4]:                             114.114.114.114

```

## 本地 DNS Stub，而不是网卡、网关、路由器或公网

```
heli@heli:~$ echo "===== resolved 服务 ====="
===== resolved 服务 =====
heli@heli:~$ systemctl status systemd-resolved --no-pager

echo "===== 53端口 ====="
sudo ss -lunp | grep ':53'

● systemd-resolved.service - Network Name Resolution
     Loaded: loaded (/lib/systemd/system/systemd-resolved.service; enabled; vendor preset: enabled)
     Active: active (running) since Wed 2026-09-16 23:13:42 CST; 23min ago
       Docs: man:systemd-resolved.service(8)
             https://www.freedesktop.org/wiki/Software/systemd/resolved
             https://www.freedesktop.org/wiki/Software/systemd/writing-network-configuration-managers
             https://www.freedesktop.org/wiki/Software/systemd/writing-resolver-clients
   Main PID: 2513 (systemd-resolve)
     Status: "Processing requests..."
      Tasks: 1 (limit: 9102)
     Memory: 4.2M
     CGroup: /system.slice/systemd-resolved.service
             └─2513 /lib/systemd/systemd-resolved

9月 16 23:13:42 heli systemd[1]: Starting Network Name Resolution...
9月 16 23:13:42 heli systemd-resolved[2513]: Positive Trust Anchors:
9月 16 23:13:42 heli systemd-resolved[2513]: . IN DS 20326 8 2 e06d44b80b8f1d39a95c0b0d7c65d08458e880409bbc683457104237c7f8ec8d
9月 16 23:13:42 heli systemd-resolved[2513]: Negative trust anchors: 10.in-addr.arpa 16.172.in-addr.arpa 17.172.in-addr.arpa 18.172.in-…
9月 16 23:13:42 heli systemd-resolved[2513]: Using system hostname 'heli'.
9月 16 23:13:42 heli systemd-resolved[2513]: DNSStubListener= is disabled, but /etc/resolv.conf is a symlink to /run/systemd/r… enabled.
9月 16 23:13:42 heli systemd[1]: Started Network Name Resolution.
9月 16 23:14:54 heli systemd-resolved[2513]: Flushed all caches.
Hint: Some lines were ellipsized, use -l to show in full.
heli@heli:~$
heli@heli:~$ echo "===== 53端口 ====="
===== 53端口 =====
heli@heli:~$ sudo ss -lunp | grep ':53'
echo "===== resolv.conf 链接 ====="
ls -l /etc/resolv.conf

echo "===== systemd直接解析 ====="
systemd-resolve baidu.com

echo "===== 直接问外部DNS ====="
nslookup baidu.com 223.5.5.5[sudo] password for heli:
Sorry, try again.
[sudo] password for heli:
UNCONN 0      0                                 0.0.0.0:53328      0.0.0.0:*     users:(("avahi-daemon",pid=559,fd=14))                  
UNCONN 0      0                                 0.0.0.0:5353       0.0.0.0:*     users:(("avahi-daemon",pid=559,fd=12))                  
UNCONN 159744 0                                 0.0.0.0:53         0.0.0.0:*     users:(("dnsmasq",pid=735,fd=4))                        
UNCONN 0      0                                    [::]:5353          [::]:*     users:(("avahi-daemon",pid=559,fd=13))                  
UNCONN 0      0                                    [::]:53            [::]:*     users:(("dnsmasq",pid=735,fd=6))                        

```



## 本机同时存在 `dnsmasq` 和 `systemd-resolved`，DNS 监听发生冲突

```
heli@heli:~$ ps -fp 735

systemctl status dnsmasq --no-pager -l

ls -l /etc/resolv.conf

grep -RniE 'dnsmasq|dns=' \
UID          PID    PPID  C STIME TTY          TIME CMD
dnsmasq      735       1 99 23:00 ?        00:38:00 /usr/sbin/dnsmasq -x /run/dnsmasq/dnsmasq.pid -u dnsmasq -7 /etc/dnsmasq.d,.dpkg-dist
    /etc/NetworkManager/NetworkManager.conf \
    /etc/NetworkManager/conf.d/ 2>/dev/null

grep -Rni 'DNSStubListener' \
    /etc/systemd/resolved.conf \
    /etc/systemd/resolved.conf.d/ 2>/dev/nullheli@heli:~$
heli@heli:~$ systemctl status dnsmasq --no-pager -l
● dnsmasq.service - dnsmasq - A lightweight DHCP and caching DNS server
     Loaded: loaded (/lib/systemd/system/dnsmasq.service; enabled; vendor preset: enabled)
     Active: active (running) since Wed 2026-09-16 23:00:42 CST; 38min ago
    Process: 680 ExecStartPre=/usr/sbin/dnsmasq --test (code=exited, status=0/SUCCESS)
    Process: 702 ExecStart=/etc/init.d/dnsmasq systemd-exec (code=exited, status=0/SUCCESS)
    Process: 741 ExecStartPost=/etc/init.d/dnsmasq systemd-start-resolvconf (code=exited, status=0/SUCCESS)
   Main PID: 735 (dnsmasq)
      Tasks: 1 (limit: 9102)
     Memory: 1.5M
     CGroup: /system.slice/dnsmasq.service
             └─735 /usr/sbin/dnsmasq -x /run/dnsmasq/dnsmasq.pid -u dnsmasq -7 /etc/dnsmasq.d,.dpkg-dist,.dpkg-old,.dpkg-new --local-service --trust-anchor=.,20326,8,2,e06d44b80b8f1d39a95c0b0d7c65d08458e880409bbc683457104237c7f8ec8d

9月 16 23:00:42 heli dnsmasq[680]: dnsmasq: syntax check OK.
9月 16 23:00:42 heli dnsmasq[735]: started, version 2.80 cachesize 150
9月 16 23:00:42 heli dnsmasq[735]: DNS service limited to local subnets
9月 16 23:00:42 heli dnsmasq[735]: compile time options: IPv6 GNU-getopt DBus i18n IDN DHCP DHCPv6 no-Lua TFTP conntrack ipset auth nettlehash DNSSEC loop-detect inotify dumpfile
9月 16 23:00:42 heli dnsmasq[735]: reading /etc/resolv.conf
9月 16 23:00:42 heli dnsmasq[735]: using nameserver 127.0.0.53#53
9月 16 23:00:42 heli dnsmasq[735]: read /etc/hosts - 7 addresses
9月 16 23:00:42 heli systemd[1]: Starting dnsmasq - A lightweight DHCP and caching DNS server...
9月 16 23:00:42 heli systemd[1]: Started dnsmasq - A lightweight DHCP and caching DNS server.
heli@heli:~$
heli@heli:~$ ls -l /etc/resolv.conf
lrwxrwxrwx 1 root root 39 9月  14 13:48 /etc/resolv.conf -> ../run/systemd/resolve/stub-resolv.conf
heli@heli:~$
heli@heli:~$ grep -RniE 'dnsmasq|dns=' \
>     /etc/NetworkManager/NetworkManager.conf \
>     /etc/NetworkManager/conf.d/ 2>/dev/null
heli@heli:~$
heli@heli:~$ grep -Rni 'DNSStubListener' \
>     /etc/systemd/resolved.conf \
>     /etc/systemd/resolved.conf.d/ 2>/dev/null
/etc/systemd/resolved.conf:23:DNSStubListener=no


heli@heli:~$ sudo systemctl stop dnsmasq
heli@heli:~$ sudo systemctl restart systemd-resolved
heli@heli:~$ sudo ss -lunp | grep ':53'
UNCONN 0      0                                 0.0.0.0:53328      0.0.0.0:*     users:(("avahi-daemon",pid=559,fd=14))                  
UNCONN 0      0                                 0.0.0.0:5353       0.0.0.0:*     users:(("avahi-daemon",pid=559,fd=12))                  
UNCONN 0      0                                    [::]:5353          [::]:*     users:(("avahi-daemon",pid=559,fd=13))                  
heli@heli:~$
heli@heli:~$
heli@heli:~$
heli@heli:~$ sudo systemctl stop dnsmasq
heli@heli:~$ sudo systemctl disable dnsmasq
Synchronizing state of dnsmasq.service with SysV service script with /lib/systemd/systemd-sysv-install.
Executing: /lib/systemd/systemd-sysv-install disable dnsmasq
Removed /etc/systemd/system/multi-user.target.wants/dnsmasq.service.
heli@heli:~$ systemctl is-active dnsmasq
inactive
heli@heli:~$ systemctl is-enabled dnsmasq
disabled
heli@heli:~$ sudo sed -i 's/^DNSStubListener=no/DNSStubListener=yes/' /etc/systemd/resolved.conf
heli@heli:~$
heli@heli:~$
heli@heli:~$ grep DNSStubListener /etc/systemd/resolved.conf
DNSStubListener=yes
heli@heli:~$
heli@heli:~$
heli@heli:~$ sudo systemctl restart systemd-resolved
heli@heli:~$
heli@heli:~$
heli@heli:~$ sudo ss -lunp | grep ':53 '
UNCONN 0      0                           127.0.0.53%lo:53         0.0.0.0:*     users:(("systemd-resolve",pid=3561,fd=12))   

heli@heli:~$ systemd-resolve baidu.com
baidu.com: 124.237.177.164                     -- link: enp2s0
           111.63.65.103                       -- link: enp2s0
           110.242.74.102                      -- link: enp2s0
           111.63.65.247                       -- link: enp2s0

-- Information acquired via protocol DNS in 12.4ms.
-- Data is authenticated: no
heli@heli:~$ getent hosts baidu.com
111.63.65.247   baidu.com
110.242.74.102  baidu.com
111.63.65.103   baidu.com
124.237.177.164 baidu.com




```

