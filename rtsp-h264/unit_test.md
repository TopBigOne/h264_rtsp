

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



# 1. get rtsp-h264
* get
```shell
tftp -g -r rtsp-h264 192.168.0.193
```

# 1. get gdbserver
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


* debug mips-linux-gnu-gdb rtsp-h264
```shell
mips-linux-gnu-gdb rtsp-h264
```
* /home/dev/Documents/Android_work/Hai_si_work/hai_si/mips_gcc720_glibc229_r5_1_4/bin/mips-linux-gnu-gdb

```shell
target remote 192.168.0.100:6666
```




----------------------------------------------


# 开发板启动 gdbserver

* step 1
> gdbserver [PC 机 IP:端口，与步骤 2 中端口需一致] [应用程序路径]
```shell
gdbserver 192.168.0.193:1234 rtsp-h264
```

* step 2 :链接开发板 : 在PC端执行
> target remote [开发板 IP:端口]
```shell
target remote  192.168.0.102:1234
```


```c





```









