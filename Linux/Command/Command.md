node

```
查看所有可用版本：nvm list available
安装指定版本 Node.js：nvm install <版本号>（如 nvm install 18.16.0）
切换使用版本：nvm use <版本号>
查看已安装版本：nvm ls
安装最新版本：nvm install latest
先删掉有问题的版本：nvm uninstall 14.21.3

https://nodejs.org/zh-cn/download/archive/v14.20.0 下载地址

npm install --legacy-peer-deps

https://raw.giteeusercontent.com/mirrors/nvm/raw/v0.40.3/install.sh docker compose install


```





Git

```
git remote set-url origin http://old-domain.com:9090/repo.git
```



GitLab

```
修复冲突：在 /etc/gitlab/gitlab.rb 中找到 puma['port']，将其改成其他空闲端口，比如 8081。

ruby
puma['port'] = 8081
nginx['listen_port']
sudo lsof -i:8080

让配置生效并验证
sudo gitlab-ctl reconfigure
sudo gitlab-ctl restart

如果看到像 puma, sidekiq, nginx 这些关键服务的状态是 run 或 ok，就说明成功了。
sudo gitlab-ctl status

sudo gitlab-ctl tail

配置地址
external_url 'http://git.home.com:800'
```





#### User Passwd

```
sudo passwd pyj
sudo usermod -aG sudo pyj 将当前用户 添加，可以使用 sudo 
```



#### Docker

```
sudo snap install docker          # version 28.4.0, or
sudo apt  install docker-compose  # version 1.29.2-1

# 确保目录存在
sudo mkdir -p /var/snap/docker/current/config/

# 写入配置（使用当前可用的镜像加速器）
sudo tee /var/snap/docker/current/config/daemon.json <<-'EOF'
{
  "registry-mirrors": ["https://docker.1ms.run"]
}
EOF

sudo systemctl restart snap.docker.dockerd


docker ps 

 scp /e/share/index.php pyj@192.168.31.224:/home/pyj
 
 docker system prune -a -f 这个命令不能敲 会把镜像给清除了。
```

#### RTC

```
date 
date -u
hwclock -w
hwclock -s
```



#### GStreamer

```
gst-inspect-1.0 --version

播放视频，把test.mp4换成实际的视频路径, mppvideodec就是尝试硬解
gst-launch-1.0 filesrc location=test.mp4 ! qtdemux ! queue ! h264parse ! mppvideodec ! videoconvert ! autovideosink

gst-inspect-1.0 |grep mpp
```

![image-20260411170509728](E:\Git\learning-record\Linux\Command\assets\image-20260411170509728.png)

