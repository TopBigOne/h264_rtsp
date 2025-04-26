

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

# 1. gdbserver
* get
```shell
tftp -g -r gdbserver 192.168.0.193
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

* 开发板
```shell
./gdbserver :6666 ./rtsp-h264 
```


* debug mips-linux-gnu-gdb rtsp-h264
```shell
mips-linux-gnu-gdb rtsp-h264
```
* /home/dev/Documents/Android_work/Hai_si_work/hai_si/mips_gcc720_glibc229_r5_1_4/bin/mips-linux-gnu-gdb

```shell
target remote 192.168.0.100:6666
```




----------------------------------------------

```c





```









