#### User Passwd

```
sudo passwd pyj
sudo usermod -aG sudo pyj 将当前用户 添加，可以使用 sudo 
```



Docker

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

