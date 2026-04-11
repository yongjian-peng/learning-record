Docker

```
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

