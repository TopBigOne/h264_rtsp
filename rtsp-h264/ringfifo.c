/*ringbuf .c*/

#include<stdio.h>
#include<ctype.h>
#include <stdlib.h>
#include <string.h>
#include "ringfifo.h"
#include "rtputils.h"

#define NMAX 32

#define SINGLE_BUFFER_SIZE ( 300*1024)


int ring_buffer_index_put = 0; /* 环形缓冲区的当前放入位置 */
int ring_buffer_index_get = 0; /* 缓冲区的当前取出位置 */
int n = 0; /* 环形缓冲区中的元素总数量 */

// buffer 数组
struct ringbuf ringfifo[NMAX];

extern int UpdateSpsOrPps(unsigned char *data, int frame_type, int len);

void UpdateSps(unsigned char *data, int len);

void UpdatePps(unsigned char *data, int len);


/**
* 环形缓冲区的地址编号计算函数，如果到达唤醒缓冲区的尾部，将绕回到头部。
 * 环形缓冲区的有效地址编号为：0到(NMAX-1)
 * @param size 300 字节
 */
void ringmalloc() {
    int i;
    for (i = 0; i < NMAX; i++) {
        ringfifo[i].buffer = malloc(SINGLE_BUFFER_SIZE);
        ringfifo[i].size = 0;
        ringfifo[i].frame_type = 0;
        // printf("FIFO INFO:idx:%d,len:%d,ptr:%x\n",i,ringfifo[i].size,(int)(ringfifo[i].buffer));
    }
    ring_buffer_index_put = 0; /* 环形缓冲区的当前放入位置 */
    ring_buffer_index_get = 0; /* 缓冲区的当前取出位置 */
    n = 0; /* 环形缓冲区中的元素总数量 */
}

/**************************************************************************************************
**
**
**
**************************************************************************************************/
void ringreset() {
    ring_buffer_index_put = 0; /* 环形缓冲区的当前放入位置 */
    ring_buffer_index_get = 0; /* 缓冲区的当前取出位置 */
    n = 0; /* 环形缓冲区中的元素总数量 */
}

/**************************************************************************************************
**
**
**
**************************************************************************************************/
void ringfree(void) {
    int i;
    printf("begin free mem\n");
    for (i = 0; i < NMAX; i++) {
        // printf("FREE FIFO INFO:idx:%d,len:%d,ptr:%x\n",i,ringfifo[i].size,(int)(ringfifo[i].buffer));
        free(ringfifo[i].buffer);
        ringfifo[i].size = 0;
    }
}

/**************************************************************************************************
**
**
**
**************************************************************************************************/
int addring(int i) {
    return (i + 1) == NMAX ? 0 : i + 1;
}

/**************************************************************************************************
**
**
**
**************************************************************************************************/

/* 从环形缓冲区中取一个元素 */

int ringget(struct ringbuf *getinfo) {
    int Pos;
    if (n > 0) {
        Pos = ring_buffer_index_get;
        ring_buffer_index_get = addring(ring_buffer_index_get);
        n--;
        getinfo->buffer = (ringfifo[Pos].buffer);
        getinfo->frame_type = ringfifo[Pos].frame_type;
        getinfo->size = ringfifo[Pos].size;
        //printf("Get FIFO INFO:idx:%d,len:%d,ptr:%x,type:%d\n",Pos,getinfo->size,(int)(getinfo->buffer),getinfo->frame_type);
        return ringfifo[Pos].size;
    } else {
        //printf("Buffer is empty\n");
        return 0;
    }
}

/**************************************************************************************************
**
**
**
**************************************************************************************************/
/* 向环形缓冲区中放入一个元素*/
void ringput(unsigned char *buffer, int size, int encode_type) {
    if (n < NMAX) {
        memcpy(ringfifo[ring_buffer_index_put].buffer, buffer, size);
        ringfifo[ring_buffer_index_put].size = size;
        ringfifo[ring_buffer_index_put].frame_type = encode_type;
        //printf("Put FIFO INFO:idx:%d,len:%d,ptr:%x,type:%d\n",iput,ringfifo[iput].size,(int)(ringfifo[iput].buffer),ringfifo[iput].frame_type);
        ring_buffer_index_put = addring(ring_buffer_index_put);
        n++;
    } else {
        //  printf("Buffer is full\n");
    }
}

/**************************************************************************************************
**
**
**
**************************************************************************************************/
#include <imp/imp_encoder.h>

int hi_si_put_h264_data_to_buffer(IMPEncoderStream *stream) {
    puts("-------------------------------------------------hi_si_put_h264_data_to_buffer()");
    int i;
    int len = 0, off = 0, len2 = 2, uplen = 0;
    unsigned char *pstr;
    int iframe = 0;
    for (i = 0; i < stream->packCount; i++) {
        len += stream->pack[i].length;
    }
    if (len >= SINGLE_BUFFER_SIZE) {
        printf("drop data %d\n", len);
        return 1;
    }

    if (n < NMAX) {
        for (i = 0; i < stream->packCount; i++) {
            memcpy(ringfifo[ring_buffer_index_put].buffer + off, stream->virAddr + stream->pack[i].offset, stream->pack[i].length);

            off += stream->pack[i].length;
            pstr = stream->virAddr + stream->pack[i].offset;

            if (pstr[4] == 0x67) {
                UpdateSps(ringfifo[ring_buffer_index_put].buffer + off, 9);
                iframe = 1;
            }
            if (pstr[4] == 0x68) {
                UpdatePps(ringfifo[ring_buffer_index_put].buffer + off, 4);
            }
            uint32_t p_str_len=    stream->pack->length;
            printf("pstr content: ");
            for (int j = 0; j < p_str_len; ++j) {
                printf("%02X", pstr[j]);
            }
            printf("\n");

        }

        ringfifo[ring_buffer_index_put].size = len;
        ring_buffer_index_put = addring(ring_buffer_index_put);
        n++;
    }

    return 1;
}
