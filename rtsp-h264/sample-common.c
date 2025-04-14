/*
 * sample-common.c
 *
 * Copyright (C) 2014 Ingenic Semiconductor Co.,Ltd
 */

#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <math.h>

#include <imp/imp_log.h>
#include <imp/imp_common.h>
#include <imp/imp_system.h>
#include <imp/imp_framesource.h>
#include <imp/imp_encoder.h>
#include <imp/imp_isp.h>
#include <imp/imp_osd.h>

#include "sample-common.h"

#define TAG "Sample-Common"

// 码率模式
const IMPEncoderRcMode S_RC_METHOD = IMP_ENC_RC_MODE_VBR;

#define SHOW_FRM_BITRATE

#ifdef  SHOW_FRM_BITRATE
#define FRM_BIT_RATE_TIME 2
#define STREAM_TYPE_NUM 3
// 定义了一个静态整型数组 frmrate_sp，大小为 STREAM_TYPE_NUM
// 初始化所有元素为 0
// 用于存储不同流类型的帧率(frame rate)信息
static int frmrate_sp[STREAM_TYPE_NUM] = {0};
// 用于存储不同流类型的开始时间(start time)或统计时间(statistic time)
static int statime_sp[STREAM_TYPE_NUM] = {0};
// 用于存储不同流类型的比特率(bit rate)信息
static int bitrate_sp[STREAM_TYPE_NUM] = {0};
#endif

int direct_switch = 0;

/**
 *
 * gosd_enable=0：关闭 OSD 功能。
gosd_enable=1：使用 IPU 叠加 OSD（灵活，适合动态内容）。

gosd_enable=2：使用 ISP 叠加 OSD（低延迟，适合固定内容）。

gosd_enable=3：两者同时使用（需硬件支持）。


 */
int gosd_enable = 0; /* 1: ipu osd, 2: isp osd, 3: ipu osd and isp osd */

struct chn_conf chn[FS_CHN_NUM];

/*
IMPSensorInfo Def_Sensor_Info[1] = {
	{
		FIRST_SNESOR_NAME,
		TX_SENSOR_CONTROL_INTERFACE_I2C,
		.i2c = {FIRST_SNESOR_NAME, FIRST_I2C_ADDR, FIRST_I2C_ADAPTER_ID},
		FIRST_RST_GPIO,
		FIRST_PWDN_GPIO,
		FIRST_POWER_GPIO,
		FIRST_SENSOR_ID,
		FIRST_VIDEO_INTERFACE,
		FIRST_MCLK,
		FIRST_DEFAULT_BOOT
	}
};
*/

IMPSensorInfo sensor_info[1];

int sample_system_init() {
    IMP_LOG_DBG(TAG, "sample_system_init start\n");

    memset(&chn, 0, sizeof(chn));
    chn[0].index                             = CH0_INDEX;
    chn[0].enable                            = CHN0_EN;
    chn[0].payloadType                       = IMP_ENC_PROFILE_AVC_MAIN; //IMP_ENC_PROFILE_HEVC_MAIN,
    chn[0].fs_chn_attr.i2dattr.i2d_enable    = 0;
    chn[0].fs_chn_attr.i2dattr.flip_enable   = 0;
    chn[0].fs_chn_attr.i2dattr.mirr_enable   = 0;
    chn[0].fs_chn_attr.i2dattr.rotate_enable = 1;
    chn[0].fs_chn_attr.i2dattr.rotate_angle  = 270;
    chn[0].fs_chn_attr.pixFmt                = PIX_FMT_NV12;
    chn[0].fs_chn_attr.outFrmRateNum         = FIRST_SENSOR_FRAME_RATE_NUM;
    chn[0].fs_chn_attr.outFrmRateDen         = FIRST_SENSOR_FRAME_RATE_DEN;
    chn[0].fs_chn_attr.nrVBs                 = 2;
    chn[0].fs_chn_attr.type                  = FS_PHY_CHANNEL;
    chn[0].fs_chn_attr.crop.enable           = FIRST_CROP_EN;
    chn[0].fs_chn_attr.crop.top              = 0;
    chn[0].fs_chn_attr.crop.left             = 0;
    chn[0].fs_chn_attr.crop.width            = FIRST_SENSOR_WIDTH;
    chn[0].fs_chn_attr.crop.height           = FIRST_SENSOR_HEIGHT;
    chn[0].fs_chn_attr.scaler.enable         = 1;
    chn[0].fs_chn_attr.scaler.outwidth       = FIRST_SENSOR_WIDTH;
    chn[0].fs_chn_attr.scaler.outheight      = FIRST_SENSOR_HEIGHT;
    chn[0].fs_chn_attr.picWidth              = FIRST_SENSOR_WIDTH;
    chn[0].fs_chn_attr.picHeight             = FIRST_SENSOR_HEIGHT;
    chn[0].framesource_chn                   = {DEV_ID_FS, CH0_INDEX, 0};
    chn[0].imp_encoder                       = {DEV_ID_ENC, CH0_INDEX, 0};

    chn[1].index                             = CH1_INDEX;
    chn[1].enable                            = CHN1_EN;
    chn[1].payloadType                       = IMP_ENC_PROFILE_AVC_MAIN; //IMP_ENC_PROFILE_HEVC_MAIN,
    chn[1].fs_chn_attr.i2dattr.i2d_enable    = 0;
    chn[1].fs_chn_attr.i2dattr.flip_enable   = 0;
    chn[1].fs_chn_attr.i2dattr.mirr_enable   = 0;
    chn[1].fs_chn_attr.i2dattr.rotate_enable = 1;
    chn[1].fs_chn_attr.i2dattr.rotate_angle  = 270;
    chn[1].fs_chn_attr.pixFmt                = PIX_FMT_NV12;
    chn[1].fs_chn_attr.outFrmRateNum         = FIRST_SENSOR_FRAME_RATE_NUM;
    chn[1].fs_chn_attr.outFrmRateDen         = FIRST_SENSOR_FRAME_RATE_DEN;
    chn[1].fs_chn_attr.nrVBs                 = 2;
    chn[1].fs_chn_attr.type                  = FS_PHY_CHANNEL;
    chn[1].fs_chn_attr.crop.enable           = 0;
    chn[1].fs_chn_attr.crop.top              = 0;
    chn[1].fs_chn_attr.crop.left             = 0;
    chn[1].fs_chn_attr.crop.width            = FIRST_SENSOR_WIDTH;
    chn[1].fs_chn_attr.crop.height           = FIRST_SENSOR_HEIGHT;
    chn[1].fs_chn_attr.scaler.enable         = 1;
    chn[1].fs_chn_attr.scaler.outwidth       = FIRST_SENSOR_WIDTH_SECOND;
    chn[1].fs_chn_attr.scaler.outheight      = FIRST_SENSOR_HEIGHT_SECOND;
    chn[1].fs_chn_attr.picWidth              = FIRST_SENSOR_WIDTH_SECOND;
    chn[1].fs_chn_attr.picHeight             = FIRST_SENSOR_HEIGHT_SECOND;
    chn[1].framesource_chn                   = {DEV_ID_FS, CH1_INDEX, 0};
    chn[1].imp_encoder                       = {DEV_ID_ENC, CH1_INDEX, 0};

    strcpy(sensor_info[0].name, FIRST_SNESOR_NAME);
    sensor_info[0].cbus_type = TX_SENSOR_CONTROL_INTERFACE_I2C;
    strcpy(sensor_info[0].i2c.type, FIRST_SNESOR_NAME);
    sensor_info[0].i2c.addr           = FIRST_I2C_ADDR;
    sensor_info[0].i2c.i2c_adapter_id = FIRST_I2C_ADAPTER_ID;
    sensor_info[0].rst_gpio           = FIRST_RST_GPIO;
    sensor_info[0].pwdn_gpio          = FIRST_PWDN_GPIO;
    sensor_info[0].power_gpio         = FIRST_POWER_GPIO;
    sensor_info[0].sensor_id          = FIRST_SENSOR_ID;
    sensor_info[0].video_interface    = FIRST_VIDEO_INTERFACE;
    sensor_info[0].mclk               = FIRST_MCLK;
    sensor_info[0].default_boot       = FIRST_DEFAULT_BOOT;
    int ret = 0;

    /* isp osd and ipu osd buffer size set */
    if (1 == gosd_enable) { /* only use ipu osd */
        IMP_OSD_SetPoolSize(512 * 1024);
    } else if (2 == gosd_enable) { /* only use isp osd */
        IMP_ISP_Tuning_SetOsdPoolSize(512 * 1024);
    } else if (3 == gosd_enable) { /* use ipu osd and isp osd */
        IMP_OSD_SetPoolSize(512 * 1024);
        IMP_ISP_Tuning_SetOsdPoolSize(512 * 1024);
    }
    //memset(&sensor_info, 0, sizeof(sensor_info));
    //memcpy(&sensor_info[0], &Def_Sensor_Info[0], sizeof(IMPSensorInfo));

    /* open isp */
    ret = IMP_ISP_Open();
    if (ret < 0) {
        IMP_LOG_ERR(TAG, "failed to open ISP\n");
        return -1;
    }

    /* add sensor */
    ret = IMP_ISP_AddSensor(IMPVI_MAIN, &sensor_info[0]);
    if (ret < 0) {
        IMP_LOG_ERR(TAG, "failed to AddSensor\n");
        return -1;
    }

    /* enable sensor */
    ret = IMP_ISP_EnableSensor(IMPVI_MAIN, &sensor_info[0]);
    if (ret < 0) {
        IMP_LOG_ERR(TAG, "failed to EnableSensor\n");
        return -1;
    }

    /* init imp system */
    // 库文件:libimp.a / libimp.so

    ret = IMP_System_Init();
    if (ret < 0) {
        IMP_LOG_ERR(TAG, "IMP_System_Init failed\n");
        return -1;
    }

    /* enable turning, to debug graphics */
    ret = IMP_ISP_EnableTuning();
    if (ret < 0) {
        IMP_LOG_ERR(TAG, "IMP_ISP_EnableTuning failed\n");
        return -1;
    }

    /* set contrast, sharpness, saturation, brightness */
    unsigned char value = 128;
    IMP_ISP_Tuning_SetContrast(IMPVI_MAIN, &value);
    IMP_ISP_Tuning_SetSharpness(IMPVI_MAIN, &value);
    IMP_ISP_Tuning_SetSaturation(IMPVI_MAIN, &value);
    IMP_ISP_Tuning_SetBrightness(IMPVI_MAIN, &value);

    /* set runningmode */
    IMPISPRunningMode dn = IMPISP_RUNNING_MODE_DAY;
    ret = IMP_ISP_Tuning_SetISPRunningMode(IMPVI_MAIN, &dn);
    if (ret < 0) {
        IMP_LOG_ERR(TAG, "IMP_ISP_Tuning_SetISPRunningMode failed\n");
        return -1;
    }

    /* set fps */
    IMPISPSensorFps fpsAttr;
    fpsAttr.num = FIRST_SENSOR_FRAME_RATE_NUM;
    fpsAttr.den = FIRST_SENSOR_FRAME_RATE_DEN;
    ret = IMP_ISP_Tuning_SetSensorFPS(0, &fpsAttr);
    if (ret < 0) {
        IMP_LOG_ERR(TAG, "IMP_ISP_Tuning_SetSensorFPS failed\n");
        return -1;
    }

    IMP_LOG_DBG(TAG, "sample_system_init success\n");

    return 0;
}

int sample_system_exit() {
    IMP_LOG_DBG(TAG, "sample_system_exit start\n");

    int ret = 0;

    /* exit imp system */
    ret = IMP_System_Exit();
    if (ret < 0) {
        IMP_LOG_ERR(TAG, "IMP_System_Exit failed\n");
        return -1;
    }

    /* disable sensor */
    ret = IMP_ISP_DisableSensor(IMPVI_MAIN);
    if (ret < 0) {
        IMP_LOG_ERR(TAG, "IMP_ISP_DisableSensor failed\n");
        return -1;
    }

    /* delete sensor */
    ret = IMP_ISP_DelSensor(IMPVI_MAIN, &sensor_info[0]);
    if (ret < 0) {
        IMP_LOG_ERR(TAG, "IMP_ISP_DelSensor failed\n");
        return -1;
    }

    /* disable turning */
    ret = IMP_ISP_DisableTuning();
    if (ret < 0) {
        IMP_LOG_ERR(TAG, "IMP_ISP_DisableTuning failed\n");
        return -1;
    }

    /* close isp */
    ret = IMP_ISP_Close();
    if (ret < 0) {
        IMP_LOG_ERR(TAG, "failed to open ISP\n");
        return -1;
    }

    IMP_LOG_DBG(TAG, " sample_system_exit success\n");

    return 0;
}

int sample_framesource_init() {
    IMP_LOG_DBG(TAG, "sample_framesource_init start\n");

    int i, ret;

    for (i = 0; i < FS_CHN_NUM; i++) {
        if (chn[i].enable) {
            // 创建通道
            ret = IMP_FrameSource_CreateChn(chn[i].index, &chn[i].fs_chn_attr);
            if (ret < 0) {
                IMP_LOG_ERR(TAG, "IMP_FrameSource_CreateChn(chn%d) error !\n", chn[i].index);
                return -1;
            }

            ret = IMP_FrameSource_SetChnAttr(chn[i].index, &chn[i].fs_chn_attr);
            if (ret < 0) {
                IMP_LOG_ERR(TAG, "IMP_FrameSource_SetChnAttr(chn%d) error !\n", chn[i].index);
                return -1;
            }
        }
    }

    IMP_LOG_DBG(TAG, "sample_framesource_init success\n");

    return 0;
}

int sample_framesource_exit() {
    IMP_LOG_DBG(TAG, "sample_framesource_exit start\n");

    int i, ret;

    /* destroy framesource channels */
    for (i = 0; i < FS_CHN_NUM; i++) {
        if (chn[i].enable) {
            ret = IMP_FrameSource_DestroyChn(chn[i].index);
            if (ret < 0) {
                IMP_LOG_ERR(TAG, "IMP_FrameSource_DestroyChn(%d) error: %d\n", chn[i].index, ret);
                return -1;
            }
        }
    }

    IMP_LOG_DBG(TAG, "sample_framesource_exit success\n");

    return 0;
}

int sample_encoder_init() {
    // 调试日志：函数开始
    IMP_LOG_DBG(TAG, "sample_encoder_init start\n");

    //  循环变量、返回值、编码通道号
    int               i, ret, chnNum = 0;
    // 图像宽高（可能旋转后）
    int               s32picWidth    = 0, s32picHeight = 0;
    // 帧源通道属性临时指针
    IMPFSChnAttr      *imp_chn_attr_tmp;
    // 编码通道属性结构体
    IMPEncoderChnAttr channel_attr;
    // 图像旋转/镜像属性
    IMPFSI2DAttr      sti2dattr;
    // 遍历所有通道（FS_CHN_NUM 为最大通道数: FS_CHN_NUM = 3
    for (i = 0; i < FS_CHN_NUM; i++) {
        // 检查通道是否启用
        if (chn[i].enable) {
            // 获取当前通道属性
            imp_chn_attr_tmp = &chn[i].fs_chn_attr;
            // 编码通道号（可能不同于输入通道号）
            chnNum           = chn[i].index;
            // 清空结构体
            memset(&channel_attr, 0, sizeof(IMPEncoderChnAttr));
            memset(&sti2dattr, 0, sizeof(IMPFSI2DAttr));

            // 获取当前通道的图像旋转属性
            ret = IMP_FrameSource_GetI2dAttr(chn[i].index, &sti2dattr);
            if (ret < 0) {
                IMP_LOG_ERR(TAG, "IMP_FrameSource_GetI2dAttr(%d) error !\n", chn[i].index);
                return -1;
            }

            /* 处理旋转后的图像宽高 */
            if ((1 == sti2dattr.i2d_enable) &&  // 旋转功能启用
                ((sti2dattr.rotate_enable) &&  // 且旋转角度为90/270度
                (sti2dattr.rotate_angle == 90 || sti2dattr.rotate_angle == 270))) {
                /*this depend on your sensor or channels*/
                //s32picWidth = (chn[i].fs_chn_attr.picHeight +15) & (~15);
                //s32picHeight = (chn[i].fs_chn_attr.picWidth +15) & (~15);
                // 旋转90/270度时，宽高互换（注：实际项目可能需要16对齐）
                s32picWidth  = (chn[i].fs_chn_attr.picHeight);/*this depend on your sensor or channels*/
                s32picHeight = (chn[i].fs_chn_attr.picWidth);
            } else {
                // 无旋转或180度旋转时保持原宽高
                s32picWidth  = chn[i].fs_chn_attr.picWidth;
                s32picHeight = chn[i].fs_chn_attr.picHeight;
            }
            /* 动态计算目标码率（基于分辨率比例） */
            float ratio = 1;
            if (((uint64_t) s32picWidth * s32picHeight) > (1280 * 720)) {
                ratio = log10f(((uint64_t) s32picWidth * s32picHeight) / (1280 * 720.0)) + 1;
            } else {
                ratio = 1.0 / (log10f((1280 * 720.0) / ((uint64_t) s32picWidth * s32picHeight)) + 1);
            }
            ratio       = ratio > 0.1 ? ratio : 0.1;
            unsigned int uTargetBitRate = BITRATE_720P_Kbs * ratio;
            printf("|---------------------------------------------| \n");
            printf("| rcMode:%d.\n", S_RC_METHOD);
            printf("|---------------------------------------------| \n");
            ret = IMP_Encoder_SetDefaultParam(&channel_attr,// 编码属性结构体
                                              chn[i].payloadType,// // 编码类型（H.264/H.265
                                              S_RC_METHOD,  // // 码率控制模式:IMP_ENC_RC_MODE_VBR
                                              s32picWidth, // 图像宽
                                              s32picHeight,// 图像高
                                              imp_chn_attr_tmp->outFrmRateNum,// 帧率分子（如30）
                                              imp_chn_attr_tmp->outFrmRateDen,  // 帧率分母（如1）
                                              imp_chn_attr_tmp->outFrmRateNum * 2 / imp_chn_attr_tmp->outFrmRateDen, //GOP 大小
                                              2,// B帧数（通常为2）
                                              (S_RC_METHOD == IMP_ENC_RC_MODE_FIXQP) ? 35 : -1, // 固定QP模式下的初始QP
                                              uTargetBitRate); // 目标码率
            if (ret < 0) {
                IMP_LOG_ERR(TAG, "IMP_Encoder_SetDefaultParam(%d) error !\n", chnNum);
                return -1;
            }
#ifdef LOW_BITSTREAM // 低码率模式配置（可选）
            /* 低码率优化：降低码率并调整QP范围 */
            IMPEncoderRcAttr *rcAttr = &channel_attr.rcAttr;
            // 码率减半
            uTargetBitRate /= 2;

            switch (rcAttr->attrRcMode.rcMode) {
                case IMP_ENC_RC_MODE_FIXQP: // // 固定QP模式
                    rcAttr->attrRcMode.attrFixQp.iInitialQP = 38; // // 提高QP（降低质量）
                    break;
                case IMP_ENC_RC_MODE_CBR:  // 固定比特率
                    rcAttr->attrRcMode.attrCbr.uTargetBitRate = uTargetBitRate;
                    rcAttr->attrRcMode.attrCbr.iInitialQP = -1;
                    rcAttr->attrRcMode.attrCbr.iMinQP = 34;
                    rcAttr->attrRcMode.attrCbr.iMaxQP = 51;
                    rcAttr->attrRcMode.attrCbr.iIPDelta = -1;
                    rcAttr->attrRcMode.attrCbr.iPBDelta = -1;
                    // 场景切换优化
                    rcAttr->attrRcMode.attrCbr.eRcOptions = IMP_ENC_RC_SCN_CHG_RES | IMP_ENC_RC_OPT_SC_PREVENTION;
                    rcAttr->attrRcMode.attrCbr.uMaxPictureSize = uTargetBitRate * 4 / 3;
                    break;
                case IMP_ENC_RC_MODE_VBR:
                    rcAttr->attrRcMode.attrVbr.uTargetBitRate = uTargetBitRate;
                    rcAttr->attrRcMode.attrVbr.uMaxBitRate = uTargetBitRate * 4 / 3;
                    rcAttr->attrRcMode.attrVbr.iInitialQP = -1;
                    rcAttr->attrRcMode.attrVbr.iMinQP = 34;
                    rcAttr->attrRcMode.attrVbr.iMaxQP = 51;
                    rcAttr->attrRcMode.attrVbr.iIPDelta = -1;
                    rcAttr->attrRcMode.attrVbr.iPBDelta = -1;
                    rcAttr->attrRcMode.attrVbr.eRcOptions = IMP_ENC_RC_SCN_CHG_RES | IMP_ENC_RC_OPT_SC_PREVENTION;
                    rcAttr->attrRcMode.attrVbr.uMaxPictureSize = uTargetBitRate * 4 / 3;
                    break;
                case IMP_ENC_RC_MODE_CAPPED_VBR: // VBR 可变比特率
                    rcAttr->attrRcMode.attrCappedVbr.uTargetBitRate = uTargetBitRate;
                    rcAttr->attrRcMode.attrCappedVbr.uMaxBitRate = uTargetBitRate * 4 / 3;
                    rcAttr->attrRcMode.attrCappedVbr.iInitialQP = -1;
                    rcAttr->attrRcMode.attrCappedVbr.iMinQP = 34;
                    rcAttr->attrRcMode.attrCappedVbr.iMaxQP = 51;
                    rcAttr->attrRcMode.attrCappedVbr.iIPDelta = -1;
                    rcAttr->attrRcMode.attrCappedVbr.iPBDelta = -1;
                    rcAttr->attrRcMode.attrCappedVbr.eRcOptions = IMP_ENC_RC_SCN_CHG_RES | IMP_ENC_RC_OPT_SC_PREVENTION;
                    rcAttr->attrRcMode.attrCappedVbr.uMaxPictureSize = uTargetBitRate * 4 / 3;
                    rcAttr->attrRcMode.attrCappedVbr.uMaxPSNR = 42;
                    break;
                case IMP_ENC_RC_MODE_CAPPED_QUALITY: // 基于 max PSNR 的可变比特率。推荐使用
                    rcAttr->attrRcMode.attrCappedQuality.uTargetBitRate = uTargetBitRate;
                    rcAttr->attrRcMode.attrCappedQuality.uMaxBitRate = uTargetBitRate * 4 / 3;
                    rcAttr->attrRcMode.attrCappedQuality.iInitialQP = -1;
                    rcAttr->attrRcMode.attrCappedQuality.iMinQP = 34;
                    rcAttr->attrRcMode.attrCappedQuality.iMaxQP = 51;
                    rcAttr->attrRcMode.attrCappedQuality.iIPDelta = -1;
                    rcAttr->attrRcMode.attrCappedQuality.iPBDelta = -1;
                    rcAttr->attrRcMode.attrCappedQuality.eRcOptions = IMP_ENC_RC_SCN_CHG_RES | IMP_ENC_RC_OPT_SC_PREVENTION;
                    rcAttr->attrRcMode.attrCappedQuality.uMaxPictureSize = uTargetBitRate * 4 / 3;
                    rcAttr->attrRcMode.attrCappedQuality.uMaxPSNR = 42;
                    break;
                case IMP_ENC_RC_MODE_INVALID:
                    IMP_LOG_ERR(TAG, "unsupported rcmode:%d, we only support fixqp, cbr vbr and capped vbr\n", rcAttr->attrRcMode.rcMode);
                    return -1;
            }
#endif
            /* 特殊配置：启用IVDC（智能视频降噪） */
            if (direct_switch == 1) {
                // 仅对通道0启用
                if (0 == chnNum) channel_attr.bEnableIvdc = true;
            }

            /* 创建编码通道 */
            ret = IMP_Encoder_CreateChn(chnNum, &channel_attr);
            if (ret < 0) {
                IMP_LOG_ERR(TAG, "IMP_Encoder_CreateChn(%d) error !\n", chnNum);
                return -1;
            }


            /* 注册帧源到编码通道 */
            ret = IMP_Encoder_RegisterChn(chn[i].index, chnNum);
            if (ret < 0) {
                IMP_LOG_ERR(TAG, "IMP_Encoder_RegisterChn(%d, %d) error: %d\n", chn[i].index, chnNum, ret);
                return -1;
            }
        }
    }

    IMP_LOG_DBG(TAG, "sample_encoder_init success\n");

    return 0;
}

int sample_encoder_exit(void) {
    IMP_LOG_DBG(TAG, "sample_encoder_exit start\n");

    int               ret = 0, i = 0, chnNum = 0;
    IMPEncoderChnStat chn_stat;

    for (i = 0; i < FS_CHN_NUM; i++) {
        if (chn[i].enable) {
            chnNum = chn[i].index;
            memset(&chn_stat, 0, sizeof(IMPEncoderChnStat));
            ret = IMP_Encoder_Query(chnNum, &chn_stat);
            if (ret < 0) {
                IMP_LOG_ERR(TAG, "IMP_Encoder_Query(%d) error: %d\n", chnNum, ret);
                return -1;
            }

            if (chn_stat.registered) {
                ret = IMP_Encoder_UnRegisterChn(chnNum);
                if (ret < 0) {
                    IMP_LOG_ERR(TAG, "IMP_Encoder_UnRegisterChn(%d) error: %d\n", chnNum, ret);
                    return -1;
                }

                ret = IMP_Encoder_DestroyChn(chnNum);
                if (ret < 0) {
                    IMP_LOG_ERR(TAG, "IMP_Encoder_DestroyChn(%d) error: %d\n", chnNum, ret);
                    return -1;
                }

                ret = IMP_Encoder_DestroyGroup(chnNum);
                if (ret < 0) {
                    IMP_LOG_ERR(TAG, "IMP_Encoder_DestroyGroup(%d) error: %d\n", chnNum, ret);
                    return -1;
                }
            }
        }
    }

    IMP_LOG_DBG(TAG, "sample_encoder_exit success\n");

    return 0;
}

int sample_framesource_streamon() {
    int ret = 0, i = 0;

    /* enable framesource channels */
    for (i = 0; i < FS_CHN_NUM; i++) {
        if (chn[i].enable) {
            ret = IMP_FrameSource_EnableChn(chn[i].index);
            if (ret < 0) {
                IMP_LOG_ERR(TAG, "IMP_FrameSource_EnableChn failed, error chn index: %d\n", chn[i].index);
                return -1;
            }
        }
    }

    return 0;
}

int sample_framesource_streamoff() {
    int ret = 0, i = 0;

    /* disable framesource channels */
    for (i = 0; i < FS_CHN_NUM; i++) {
        if (chn[i].enable) {
            ret = IMP_FrameSource_DisableChn(chn[i].index);
            if (ret < 0) {
                IMP_LOG_ERR(TAG, "IMP_FrameSource_DisableChn failed, error chn index: %d\n", chn[i].index);
                return -1;
            }
        }
    }

    return 0;
}

int sample_jpeg_init() {
    int               i, ret;
    IMPEncoderChnAttr channel_attr;
    IMPFSChnAttr      *imp_chn_attr_tmp;

    for (i = 0; i < FS_CHN_NUM; i++) {
        if (chn[i].enable) {
            imp_chn_attr_tmp = &chn[i].fs_chn_attr;
            memset(&channel_attr, 0, sizeof(IMPEncoderChnAttr));
            ret = IMP_Encoder_SetDefaultParam(&channel_attr, IMP_ENC_PROFILE_JPEG, IMP_ENC_RC_MODE_FIXQP,
                                              imp_chn_attr_tmp->picWidth, imp_chn_attr_tmp->picHeight,
                                              imp_chn_attr_tmp->outFrmRateNum, imp_chn_attr_tmp->outFrmRateDen, 0, 0,
                                              25, 0);

            /* Create Channel */
            ret = IMP_Encoder_CreateChn(4 + chn[i].index, &channel_attr);
            if (ret < 0) {
                IMP_LOG_ERR(TAG, "IMP_Encoder_CreateChn(%d) error: %d\n",
                            chn[i].index, ret);
                return -1;
            }

            /* Resigter Channel */
            ret = IMP_Encoder_RegisterChn(i, 4 + chn[i].index);
            if (ret < 0) {
                IMP_LOG_ERR(TAG, "IMP_Encoder_RegisterChn(0, %d) error: %d\n",
                            chn[i].index, ret);
                return -1;
            }
        }
    }

    return 0;
}

int sample_jpeg_exit() {
    int               ret = 0, i = 0, chnNum = 0;
    IMPEncoderChnStat chn_stat;

    for (i = 0; i < FS_CHN_NUM; i++) {
        if (chn[i].enable) {
            chnNum = 4 + chn[i].index;
            memset(&chn_stat, 0, sizeof(IMPEncoderChnStat));
            ret = IMP_Encoder_Query(chnNum, &chn_stat);
            if (ret < 0) {
                IMP_LOG_ERR(TAG, "IMP_Encoder_Query(%d) error: %d\n", chnNum, ret);
                return -1;
            }

            if (chn_stat.registered) {
                ret = IMP_Encoder_UnRegisterChn(chnNum);
                if (ret < 0) {
                    IMP_LOG_ERR(TAG, "IMP_Encoder_UnRegisterChn(%d) error: %d\n", chnNum, ret);
                    return -1;
                }

                ret = IMP_Encoder_DestroyChn(chnNum);
                if (ret < 0) {
                    IMP_LOG_ERR(TAG, "IMP_Encoder_DestroyChn(%d) error: %d\n", chnNum, ret);
                    return -1;
                }
            }
        }
    }

    return 0;
}

static int save_stream(int fd, IMPEncoderStream *stream) {
    int ret, i, nr_pack = stream->packCount;

    //IMP_LOG_DBG(TAG, "----------packCount=%d, stream->seq=%u start----------\n", stream->packCount, stream->seq);
    for (i = 0; i < nr_pack; i++) {
        //IMP_LOG_DBG(TAG, "[%d]:%10u,%10lld,%10u,%10u,%10u\n", i, stream->pack[i].length, stream->pack[i].timestamp, stream->pack[i].frameEnd, *((uint32_t *)(&stream->pack[i].nalType)), stream->pack[i].sliceType);
        IMPEncoderPack *pack = &stream->pack[i];
        if (pack->length) {
            uint32_t remSize = stream->streamSize - pack->offset;
            if (remSize < pack->length) {
                ret = write(fd, (void *) (stream->virAddr + pack->offset), remSize);
                if (ret != remSize) {
                    IMP_LOG_ERR(TAG, "stream write ret(%d) != pack[%d].remSize(%d) error:%s\n", ret, i, remSize,
                                strerror(errno));
                    return -1;
                }
                ret = write(fd, (void *) stream->virAddr, pack->length - remSize);
                if (ret != (pack->length - remSize)) {
                    IMP_LOG_ERR(TAG,
                                "stream->virAddr:%x stream write ret(%d) != pack[%d].(length-remSize)(%d) error:%s\n",
                                stream->virAddr, ret, i, (pack->length - remSize), strerror(errno));
                    return -1;
                }
            } else {
                ret = write(fd, (void *) (stream->virAddr + pack->offset), pack->length);
                if (ret != pack->length) {
                    IMP_LOG_ERR(TAG, "stream write ret(%d) != pack[%d].length(%d) error:%s\n", ret, i, pack->length,
                                strerror(errno));
                    return -1;
                }
            }
        }
    }
    //IMP_LOG_DBG(TAG, "----------packCount=%d, stream->seq=%u end----------\n", stream->packCount, stream->seq);
    return 0;
}

static int save_stream_by_name(char *stream_prefix, int idx, IMPEncoderStream *stream) {
    int  stream_fd       = -1;
    char stream_path[128];
    int  ret, i, nr_pack = stream->packCount;

    sprintf(stream_path, "%s-%02d.jpg", stream_prefix, idx);

    IMP_LOG_DBG(TAG, "Open Stream file %s ", stream_path);
    stream_fd = open(stream_path, O_RDWR | O_CREAT | O_TRUNC, 0777);
    if (stream_fd < 0) {
        IMP_LOG_ERR(TAG, "failed: %s\n", strerror(errno));
        return -1;
    }
    IMP_LOG_DBG(TAG, "OK\n");

    for (i = 0; i < nr_pack; i++) {
        IMPEncoderPack *pack = &stream->pack[i];
        if (pack->length) {
            uint32_t remSize = stream->streamSize - pack->offset;
            if (remSize < pack->length) {
                ret = write(stream_fd, (void *) (stream->virAddr + pack->offset),
                            remSize);
                if (ret != remSize) {
                    IMP_LOG_ERR(TAG, "stream write ret(%d) != pack[%d].remSize(%d) error:%s\n", ret, i, remSize,
                                strerror(errno));
                    return -1;
                }
                ret = write(stream_fd, (void *) stream->virAddr, pack->length - remSize);
                if (ret != (pack->length - remSize)) {
                    IMP_LOG_ERR(TAG, "stream write ret(%d) != pack[%d].(length-remSize)(%d) error:%s\n", ret, i,
                                (pack->length - remSize), strerror(errno));
                    return -1;
                }
            } else {
                ret = write(stream_fd, (void *) (stream->virAddr + pack->offset), pack->length);
                if (ret != pack->length) {
                    IMP_LOG_ERR(TAG, "stream write ret(%d) != pack[%d].length(%d) error:%s\n", ret, i, pack->length,
                                strerror(errno));
                    return -1;
                }
            }
        }
    }

    close(stream_fd);

    return 0;
}


static void *get_frame_thread(void *args) {
    int          index  = (int) args;
    int          chnNum = chn[index].index;
    int          i      = 0, ret = 0;
    int          fd     = -1;
    char         framefilename[64];
    IMPFrameInfo *frame = NULL;

    if (PIX_FMT_NV12 == chn[index].fs_chn_attr.pixFmt) {
        sprintf(framefilename, "/tmp/frame%dx%d_%d.nv12", chn[index].fs_chn_attr.picWidth,
                chn[index].fs_chn_attr.picHeight, index);
    } else {
        sprintf(framefilename, "frame%dx%d_%d.raw", chn[index].fs_chn_attr.picWidth, chn[index].fs_chn_attr.picHeight,
                index);
    }

    fd = open(framefilename, O_RDWR | O_CREAT, 0x644);
    if (fd < 0) {
        IMP_LOG_ERR(TAG, "open %s failed:%s\n", framefilename, strerror(errno));
        goto err_open_framefilename;
    }

    ret = IMP_FrameSource_SetFrameDepth(chnNum, chn[index].fs_chn_attr.nrVBs * 2);
    if (ret < 0) {
        IMP_LOG_ERR(TAG, "IMP_FrameSource_SetFrameDepth(%d,%d) failed\n", chnNum, chn[index].fs_chn_attr.nrVBs * 2);
        goto err_IMP_FrameSource_SetFrameDepth_1;
    }

    for (i = 0; i < NR_FRAMES_TO_SAVE; i++) {
        printf("IMP_FrameSource_GetFrame(%d) i=%d\n", chnNum, i);
        ret = IMP_FrameSource_GetFrame(chnNum, &frame);
        if (ret < 0) {
            IMP_LOG_ERR(TAG, "IMP_FrameSource_GetFrame(%d) i=%d failed\n", chnNum, i);
            goto err_IMP_FrameSource_GetFrame_i;
        }

        if (NR_FRAMES_TO_SAVE / 2 == i) {
            if (write(fd, (void *) frame->virAddr, frame->size) != frame->size) {
                IMP_LOG_ERR(TAG, "chnNum=%d write frame i=%d failed\n", chnNum, i);
                goto err_write_frame;
            }
        }
        ret = IMP_FrameSource_ReleaseFrame(chnNum, frame);
        if (ret < 0) {
            IMP_LOG_ERR(TAG, "IMP_FrameSource_ReleaseFrame(%d) i=%d failed\n", chnNum, i);
            goto err_IMP_FrameSource_ReleaseFrame_i;
        }
    }

    IMP_FrameSource_SetFrameDepth(chnNum, 0);
    close(fd);

    return (void *) 0;

    err_IMP_FrameSource_ReleaseFrame_i:
    err_write_frame:
    IMP_FrameSource_ReleaseFrame(chnNum, frame);
    err_IMP_FrameSource_GetFrame_i:
    goto err_IMP_FrameSource_SetFrameDepth_1;
    IMP_FrameSource_SetFrameDepth(chnNum, 0);
    err_IMP_FrameSource_SetFrameDepth_1:
    close(fd);
    err_open_framefilename:
    return (void *) -1;
}

int sample_get_frame() {
    int          ret;
    unsigned int i;
    pthread_t    tid[FS_CHN_NUM];

    for (i = 0; i < FS_CHN_NUM; i++) {
        if (chn[i].enable) {
            ret = pthread_create(&tid[i], NULL, get_frame_thread, (void *) i);
            if (ret < 0) {
                IMP_LOG_ERR(TAG, "Create ChnNum%d get_frame_thread failed\n", chn[i].index);
                return -1;
            }
        }
    }

    for (i = 0; i < FS_CHN_NUM; i++) {
        if (chn[i].enable) {
            pthread_join(tid[i], NULL);
        }
    }

    return 0;
}

int HisiPutH264DataToBuffer(IMPEncoderStream *stream);

static void *get_video_stream_thread(void *args) {
    int               val, i, chnNum, ret;
    char              stream_path[64];
    IMPEncoderEncType encType;
    int               stream_fd = -1, totalSaveCnt = 0;

    val     = (int) args;
    chnNum  = val & 0xffff;
    encType = (val >> 16) & 0xffff;

    ret = IMP_Encoder_StartRecvPic(chnNum);
    if (ret < 0) {
        IMP_LOG_ERR(TAG, "IMP_Encoder_StartRecvPic(%d) failed\n", chnNum);
        return ((void *) -1);
    }
    sprintf(stream_path, "%s/stream-chn%d-%dx%d.%s", STREAM_FILE_PATH_PREFIX, chnNum,
            chn[chnNum].fs_chn_attr.picWidth,
            chn[chnNum].fs_chn_attr.picHeight,
            (encType == IMP_ENC_TYPE_AVC) ? "h264" : ((encType == IMP_ENC_TYPE_HEVC) ? "h265" : "jpeg"));

    if (encType == IMP_ENC_TYPE_JPEG) {
        totalSaveCnt = ((NR_FRAMES_TO_SAVE / 10) > 0) ? (NR_FRAMES_TO_SAVE / 10) : 1;
    } else {
        /*
        IMP_LOG_DBG(TAG, "Video ChnNum=%d Open Stream file %s ", chnNum, stream_path);
        stream_fd = open(stream_path, O_RDWR | O_CREAT | O_TRUNC, 0777);
        if (stream_fd < 0) {
            IMP_LOG_ERR(TAG, "failed: %s\n", strerror(errno));
            return ((void *)-1);
        }
        IMP_LOG_DBG(TAG, "OK\n");
        totalSaveCnt = NR_FRAMES_TO_SAVE;*/
    }

    //for (i = 0; i < totalSaveCnt; i++) {
    while (1) {
        ret = IMP_Encoder_PollingStream(chnNum, 1000);
        if (ret < 0) {
            IMP_LOG_ERR(TAG, "IMP_Encoder_PollingStream(%d) timeout\n", chnNum);
            continue;
        }

        IMPEncoderStream stream;
        /* Get H264 or H265 Stream */
        // 获取编码的码流
        ret = IMP_Encoder_GetStream(chnNum, &stream, 1);
#ifdef SHOW_FRM_BITRATE
        int i, len = 0;
        for (i = 0; i < stream.packCount; i++) {
            len += stream.pack[i].length;
        }
        bitrate_sp[chnNum] += len;
        frmrate_sp[chnNum]++;

        // 获得 IMP 系统的时间戳，单位为微秒
        int64_t now = IMP_System_GetTimeStamp() / 1000;
        if (((int) (now - statime_sp[chnNum]) / 1000) >= FRM_BIT_RATE_TIME) {
            double fps = (double) frmrate_sp[chnNum] / ((double) (now - statime_sp[chnNum]) / 1000);
            double kbr = (double) bitrate_sp[chnNum] * 8 / (double) (now - statime_sp[chnNum]);

            printf("streamNum[%d]:FPS: %0.2f,Bitrate: %0.2f(kbps)\n", chnNum, fps, kbr);
            //fflush(stdout);

            frmrate_sp[chnNum] = 0;
            bitrate_sp[chnNum] = 0;
            statime_sp[chnNum] = now;
        }
#endif
        if (ret < 0) {
            IMP_LOG_ERR(TAG, "IMP_Encoder_GetStream(%d) failed\n", chnNum);
            return ((void *) -1);
        }

        if (encType == IMP_ENC_TYPE_JPEG) {
            sprintf(stream_path, "%s/stream-chn%d-%dx%d", STREAM_FILE_PATH_PREFIX, chnNum,
                    chn[chnNum % 4].fs_chn_attr.picWidth,
                    chn[chnNum % 4].fs_chn_attr.picHeight);
            ret = save_stream_by_name(stream_path, i, &stream);
            if (ret < 0) {
                return ((void *) ret);
            }
        } else {
            if (chnNum == 1) {
                HisiPutH264DataToBuffer(&stream);
            }
            //ret = save_stream(stream_fd, &stream);
            //if (ret < 0) {
            //	close(stream_fd);
            //	return ((void *)ret);
            //}
        }
        IMP_Encoder_ReleaseStream(chnNum, &stream);
    }
    close(stream_fd);

    ret = IMP_Encoder_StopRecvPic(chnNum);
    if (ret < 0) {
        IMP_LOG_ERR(TAG, "IMP_Encoder_StopRecvPic(%d) failed\n", chnNum);
        return ((void *) -1);
    }

    return ((void *) 0);
}

int sample_get_video_stream() {
    unsigned int i;
    int          ret;
    pthread_t    tid[FS_CHN_NUM];

    for (i = 0; i < FS_CHN_NUM; i++) {
        if (chn[i].enable) {
            int arg = 0;
            if (chn[i].payloadType == IMP_ENC_PROFILE_JPEG) {
                arg = (((chn[i].payloadType >> 24) << 16) | (4 + chn[i].index));
            } else {
                arg = (((chn[i].payloadType >> 24) << 16) | chn[i].index);
            }
            ret     = pthread_create(&tid[i], NULL, get_video_stream_thread, (void *) arg);
            if (ret < 0) {
                IMP_LOG_ERR(TAG, "Create ChnNum%d get_video_stream_thread failed\n",
                            (chn[i].payloadType == IMP_ENC_PROFILE_JPEG) ? (4 + chn[i].index) : chn[i].index);
            }
        }
    }

    for (i = 0; i < FS_CHN_NUM; i++) {
        if (chn[i].enable) {
            pthread_join(tid[i], NULL);
        }
    }

    return 0;
}

int sample_get_video_stream_byfd() {
    int            streamFd[FS_CHN_NUM], vencFd[FS_CHN_NUM], maxVencFd = 0;
    char           stream_path[FS_CHN_NUM][128];
    fd_set         readfds;
    struct timeval selectTimeout;
    int            saveStreamCnt[FS_CHN_NUM], totalSaveStreamCnt[FS_CHN_NUM];
    int            i                                                   = 0, ret = 0, chnNum = 0;
    memset(streamFd, 0, sizeof(streamFd));
    memset(vencFd, 0, sizeof(vencFd));
    memset(stream_path, 0, sizeof(stream_path));
    memset(saveStreamCnt, 0, sizeof(saveStreamCnt));
    memset(totalSaveStreamCnt, 0, sizeof(totalSaveStreamCnt));

    for (i = 0; i < FS_CHN_NUM; i++) {
        streamFd[i]      = -1;
        vencFd[i]        = -1;
        saveStreamCnt[i] = 0;
        if (chn[i].enable) {
            if (chn[i].payloadType == IMP_ENC_PROFILE_JPEG) {
                chnNum = 4 + chn[i].index;
                totalSaveStreamCnt[i] = (NR_FRAMES_TO_SAVE / 50 > 0) ? NR_FRAMES_TO_SAVE / 50 : NR_FRAMES_TO_SAVE;
            } else {
                chnNum = chn[i].index;
                totalSaveStreamCnt[i] = NR_FRAMES_TO_SAVE;
            }
            sprintf(stream_path[i], "%s/stream-%d.%s", STREAM_FILE_PATH_PREFIX, chnNum,
                    ((chn[i].payloadType >> 24) == IMP_ENC_TYPE_AVC) ? "h264" : (((chn[i].payloadType >> 24) ==
                                                                                  IMP_ENC_TYPE_HEVC) ? "h265"
                                                                                                     : "jpeg"));

            if (chn[i].payloadType != IMP_ENC_PROFILE_JPEG) {
                streamFd[i] = open(stream_path[i], O_RDWR | O_CREAT | O_TRUNC, 0777);
                if (streamFd[i] < 0) {
                    IMP_LOG_ERR(TAG, "open %s failed:%s\n", stream_path[i], strerror(errno));
                    return -1;
                }
            }

            vencFd[i] = IMP_Encoder_GetFd(chnNum);
            if (vencFd[i] < 0) {
                IMP_LOG_ERR(TAG, "IMP_Encoder_GetFd(%d) failed\n", chnNum);
                return -1;
            }

            if (maxVencFd < vencFd[i]) {
                maxVencFd = vencFd[i];
            }

            ret = IMP_Encoder_StartRecvPic(chnNum);
            if (ret < 0) {
                IMP_LOG_ERR(TAG, "IMP_Encoder_StartRecvPic(%d) failed\n", chnNum);
                return -1;
            }
        }
    }

    while (1) {
        int breakFlag = 1;
        for (i = 0; i < FS_CHN_NUM; i++) {
            breakFlag &= (saveStreamCnt[i] >= totalSaveStreamCnt[i]);
        }
        if (breakFlag) {
            break;  // save frame enough
        }

        FD_ZERO(&readfds);
        for (i = 0; i < FS_CHN_NUM; i++) {
            if (chn[i].enable && saveStreamCnt[i] < totalSaveStreamCnt[i]) {
                FD_SET(vencFd[i], &readfds);
            }
        }
        selectTimeout.tv_sec  = 2;
        selectTimeout.tv_usec = 0;

        ret = select(maxVencFd + 1, &readfds, NULL, NULL, &selectTimeout);
        if (ret < 0) {
            IMP_LOG_ERR(TAG, "select failed:%s\n", strerror(errno));
            return -1;
        } else if (ret == 0) {
            continue;
        } else {
            for (i = 0; i < FS_CHN_NUM; i++) {
                if (chn[i].enable && FD_ISSET(vencFd[i], &readfds)) {
                    IMPEncoderStream stream;

                    if (chn[i].payloadType == IMP_ENC_PROFILE_JPEG) {
                        chnNum = 4 + chn[i].index;
                    } else {
                        chnNum = chn[i].index;
                    }
                    /* Get H264 or H265 Stream */
                    ret = IMP_Encoder_GetStream(chnNum, &stream, 1);
                    if (ret < 0) {
                        IMP_LOG_ERR(TAG, "IMP_Encoder_GetStream(%d) failed\n", chnNum);
                        return -1;
                    }

                    if (chn[i].payloadType == IMP_ENC_PROFILE_JPEG) {
                        ret = save_stream_by_name(stream_path[i], saveStreamCnt[i], &stream);
                        if (ret < 0) {
                            return -1;
                        }
                    } else {
                        ret = save_stream(streamFd[i], &stream);
                        if (ret < 0) {
                            close(streamFd[i]);
                            return -1;
                        }
                    }

                    IMP_Encoder_ReleaseStream(chnNum, &stream);
                    saveStreamCnt[i]++;
                }
            }
        }
    }

    for (i = 0; i < FS_CHN_NUM; i++) {
        if (chn[i].enable) {
            if (chn[i].payloadType == IMP_ENC_PROFILE_JPEG) {
                chnNum = 4 + chn[i].index;
            } else {
                chnNum = chn[i].index;
            }
            IMP_Encoder_StopRecvPic(chnNum);
            close(streamFd[i]);
        }
    }

    return 0;
}

static void *get_h265_jpeg_stream_thread(void *args) {
    int               val, i, chnNum, ret;
    char              stream_path[64];
    IMPEncoderEncType encType;
    int               totalSaveCnt = 0;

    val     = (int) args;
    chnNum  = val & 0xffff;
    encType = (val >> 16) & 0xffff;

    totalSaveCnt = NR_FRAMES_TO_SAVE / 10;

    for (i = 0; i < totalSaveCnt; i++) {
        ret = IMP_Encoder_StartRecvPic(chnNum);
        if (ret < 0) {
            IMP_LOG_ERR(TAG, "IMP_Encoder_StartRecvPic(%d) failed\n", chnNum);
            return ((void *) -1);
        }

        ret = IMP_Encoder_PollingStream(chnNum, 1000);
        if (ret < 0) {
            IMP_LOG_ERR(TAG, "IMP_Encoder_PollingStream(%d) timeout\n", chnNum);
            continue;
        }

        IMPEncoderStream stream;
        /* Get H264 or H265 Stream */
        ret = IMP_Encoder_GetStream(chnNum, &stream, 1);
        if (ret < 0) {
            IMP_LOG_ERR(TAG, "IMP_Encoder_GetStream(%d) failed\n", chnNum);
            return ((void *) -1);
        }

        if (encType == IMP_ENC_TYPE_JPEG) {
            sprintf(stream_path, "%s/stream-chn%d-%dx%d", STREAM_FILE_PATH_PREFIX, chnNum,
                    chn[chnNum % 4].fs_chn_attr.picWidth,
                    chn[chnNum % 4].fs_chn_attr.picHeight);
            ret = save_stream_by_name(stream_path, i, &stream);
            if (ret < 0) {
                return ((void *) ret);
            }
        }
        ret = IMP_Encoder_ReleaseStream(chnNum, &stream);
        if (ret < 0) {
            IMP_LOG_ERR(TAG, "IMP_Encoder_ReleaseStream(%d) failed\n", chnNum);
            return ((void *) -1);
        }

        ret = IMP_Encoder_StopRecvPic(chnNum);
        if (ret < 0) {
            IMP_LOG_ERR(TAG, "IMP_Encoder_StopRecvPic(%d) failed\n", chnNum);
            return ((void *) -1);
        }
    }

    return ((void *) 0);
}

int sample_get_h265_jpeg_stream() {
    unsigned int i;
    int          arg = 0;
    int          ret;
    pthread_t    tid[FS_CHN_NUM];
    chn[4].payloadType = chn[5].payloadType = IMP_ENC_PROFILE_JPEG;
    for (i = 0; i < FS_CHN_NUM; i++) {
        if (chn[i].enable) {
            arg = (((chn[i].payloadType >> 24) << 16) | chn[i].index);
            ret = pthread_create(&tid[i], NULL, get_video_stream_thread, (void *) arg);
            if (ret < 0) {
                IMP_LOG_ERR(TAG, "Create ChnNum%d get_video_stream failed\n",
                            (chn[i].payloadType == IMP_ENC_PROFILE_JPEG) ? (4 + chn[i].index) : chn[i].index);
            }

            if (chn[i + 4].payloadType == IMP_ENC_PROFILE_JPEG) {
                arg = (((chn[i + 4].payloadType >> 24) << 16) | (4 + chn[i].index));
                ret = pthread_create(&tid[i + 4], NULL, get_h265_jpeg_stream_thread, (void *) arg);
                if (ret < 0) {
                    IMP_LOG_ERR(TAG, "Create ChnNum%d get_h265_jpeg_stream_thread failed\n",
                                (chn[i + 4].payloadType == IMP_ENC_PROFILE_JPEG) ? (4 + chn[i].index) : chn[i].index);
                }
            }
        }
    }

    for (i = 0; i < FS_CHN_NUM; i++) {
        if (chn[i].enable) {
            pthread_join(tid[i], NULL);
            pthread_join(tid[i + 4], NULL);
        }
    }
    return 0;
}

int sample_jpeg_ivpu_init() {
    int               i, ret;
    IMPEncoderEncAttr *enc_attr;
    IMPEncoderChnAttr channel_attr;
    IMPFSChnAttr      *imp_chn_attr_tmp;

    for (i = 0; i < FS_CHN_NUM; i++) {
        if (chn[i].enable) {
            imp_chn_attr_tmp = &chn[i].fs_chn_attr;
            memset(&channel_attr, 0, sizeof(IMPEncoderChnAttr));
            enc_attr = &channel_attr.encAttr;
            enc_attr->eProfile   = IMP_ENC_PROFILE_JPEG;
            enc_attr->encVputype = IMP_ENC_TPYE_IVPU;
            enc_attr->bufSize    = 0;
            enc_attr->uWidth     = imp_chn_attr_tmp->picWidth;
            enc_attr->uHeight    = imp_chn_attr_tmp->picHeight;

            /* Create Channel */
            ret = IMP_Encoder_CreateChn(4 + chn[i].index, &channel_attr);
            if (ret < 0) {
                IMP_LOG_ERR(TAG, "IMP_Encoder_CreateChn(%d) error: %d\n",
                            chn[i].index, ret);
                return -1;
            }

            /* Resigter Channel */
            ret = IMP_Encoder_RegisterChn(i, 4 + chn[i].index);
            if (ret < 0) {
                IMP_LOG_ERR(TAG, "IMP_Encoder_RegisterChn(0, %d) error: %d\n",
                            chn[i].index, ret);
                return -1;
            }
        }
    }

    return 0;
}

int sample_get_jpeg_snap(int count) {
    int  i, ret;
    int  j;
    char snap_path[64];

    for (i = 0; i < FS_CHN_NUM; i++) {
        if (chn[i].enable) {
            ret = IMP_Encoder_StartRecvPic(4 + chn[i].index);
            if (ret < 0) {
                IMP_LOG_ERR(TAG, "IMP_Encoder_StartRecvPic(%d) failed\n", 4 + chn[i].index);
                return -1;
            }

            for (j = 0; j < count; j++) {
                sprintf(snap_path, "%s/snap-%d-%dx%d-chn%d.jpg",
                        SNAP_FILE_PATH_PREFIX, j, chn[i].fs_chn_attr.picWidth, chn[i].fs_chn_attr.picHeight,
                        4 + chn[i].index);

                IMP_LOG_ERR(TAG, "Open Snap file %s ", snap_path);
                int snap_fd = open(snap_path, O_RDWR | O_CREAT | O_TRUNC, 0777);
                if (snap_fd < 0) {
                    IMP_LOG_ERR(TAG, "failed: %s\n", strerror(errno));
                    return -1;
                }
                IMP_LOG_DBG(TAG, "OK\n");

                /* Polling JPEG Snap, set timeout as 1000msec */
                ret = IMP_Encoder_PollingStream(4 + chn[i].index, 1000);
                if (ret < 0) {
                    IMP_LOG_ERR(TAG, "Polling stream timeout\n");
                    continue;
                }

                IMPEncoderStream stream;
                /* Get JPEG Snap */
                ret = IMP_Encoder_GetStream(chn[i].index + 4, &stream, 1);
                if (ret < 0) {
                    IMP_LOG_ERR(TAG, "IMP_Encoder_GetStream() failed\n");
                    return -1;
                }

                ret = save_stream(snap_fd, &stream);
                if (ret < 0) {
                    close(snap_fd);
                    return ret;
                }

                IMP_Encoder_ReleaseStream(4 + chn[i].index, &stream);

                close(snap_fd);
            }

            ret = IMP_Encoder_StopRecvPic(4 + chn[i].index);
            if (ret < 0) {
                IMP_LOG_ERR(TAG, "IMP_Encoder_StopRecvPic() failed\n");
                return -1;
            }
        }
    }
    return 0;
}

int64_t sample_gettimeus(void) {
    struct timeval sttime;
    gettimeofday(&sttime, NULL);
    return (sttime.tv_sec * 1000000 + (sttime.tv_usec));
}



/**
 * COLLECT_GCC=mips-linux-gnu-gcc
COLLECT_LTO_WRAPPER=/usr/lib/gcc-cross/mips-linux-gnu/9/lto-wrapper
Target: mips-linux-gnu
Configured with: ../src/configure -v --with-pkgversion='Ubuntu 9.4.0-1ubuntu1~20.04' --with-bugurl=file:///usr/share/doc/gcc-9/README.Bugs --enable-languages=c,ada,c++,go,d,fortran,objc,obj-c++,gm2 --prefix=/usr --with-gcc-major-version-only --program-suffix=-9 --enable-shared --enable-linker-build-id --libexecdir=/usr/lib --without-included-gettext --enable-threads=posix --libdir=/usr/lib --enable-nls --with-sysroot=/ --enable-clocale=gnu --enable-libstdcxx-debug --enable-libstdcxx-time=yes --with-default-libstdcxx-abi=new --enable-gnu-unique-object --disable-libitm --disable-libsanitizer --disable-libquadmath --disable-libquadmath-support --enable-plugin --with-system-zlib --without-target-system-zlib --enable-libpth-m2 --enable-multiarch --disable-werror --enable-multilib --with-arch-32=mips32r2 --with-fp-32=xx --with-lxc1-sxc1=no --enable-targets=all --with-arch-64=mips64r2 --enable-checking=release --build=x86_64-linux-gnu --host=x86_64-linux-gnu --target=mips-linux-gnu --program-prefix=mips-linux-gnu- --includedir=/usr/mips-linux-gnu/include
Thread model: posix
gcc version 9.4.0 (Ubuntu 9.4.0-1ubuntu1~20.04) 


 *  ~/Documents/Android_work/Hai_si_work/hai_si/mips_gcc720_glibc229_r5_1_4/bin



 export PATH="/Documents/Android_work/Hai_si_work/hai_si/mips_gcc720_glibc229_r5_1_4/bin:$PATH"
 * 
 */
