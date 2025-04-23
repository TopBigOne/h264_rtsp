

# tftp and minicom

* 启动tftp
```shell

sudo systemctl restart tftpd-hpa

```

* 启动 minicom
```shell
sudo minicom -D /dev/ttyUSB0 -b 115200
```

* 查看端口占用
```shell
sudo lsof /dev/ttyUSB0
```



# 1. rtsp-h264
* get
```shell
tftp -g -r rtsp-h264 192.168.0.193
```


* push
```shell

tftp -p -l stream-chn0-2880x1620.h264 192.168.0.196

```

* copy
```shell
rm -f /home/dev/Desktop/for_tftp/rtsp-h264
rm -f /home/dev/Desktop/for_tftp/stream-chn0-2880x1620.h264
cp -a /home/dev/Documents/Android_work/Hai_si_work/hai_si/h264_rtsp/rtsp-h264/rtsp-h264 /home/dev/Desktop/for_tftp/rtsp-h264
```



* play with ffplay
```shell

/home/dev/Documents/Android_work/main_ffmpeg/FFmpeg/ffplay_g -i /home/dev/Desktop/for_tftp/stream-chn0-2880x1620.h264
```




----------------------------------------------

```c





```









