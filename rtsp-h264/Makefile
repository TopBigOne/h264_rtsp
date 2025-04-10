INCLUDE=-I./ -I../include -I../magik-toolkit/InferenceKit/nna2/mips720-glibc229/T41/include
LIBS= ../lib/glibc/libimp.a ../lib/glibc/libalog.a  -L../magik-toolkit/InferenceKit/nna2/mips720-glibc229/T41/lib/glibc -lvenus -laip -ldrivers -lpthread -lrt -ldl -lm
all:rtsp-h264
rtsp-h264:
	mips-linux-gnu-g++ -mabi=32 -mhard-float -o rtsp-h264 -std=c++11 -mfp64 -mnan=2008 -mabs=2008 -Wall -EL -O3 -march=mips32r2 -flax-vector-conversions  -w -fpermissive  main.c ringfifo.c rtputils.c rtspservice.c rtsputils.c  sample-common.c sample-Encoder-video.c inference_nv12.cpp  $(INCLUDE) $(LIBS)
clean:
	rm -rfv rtsp-h264
