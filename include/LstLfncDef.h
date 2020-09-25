/***********************************************************************
*
* Copyright (c) 2000-2001 Luster LightTech(Beijing) Co.Ltd.
* All Rights Reserved.
*
*
* FILE NAME     :  LusterLfncDef.h
* PROJECT NAME  :  LusterSDK
* ABSTRACT      :  凌云相机参数命名约定（LFNC）定义头文件，用来存放参数名称的定义。
*
*
*
* VERSION       :  0.1.2.0
* DATE 		    :  2017/12/15
* AUTHOR 		:  刘麒
* NOTE 		    :  初稿，对应说明文档《凌云相机参数命名约定(LFNC)_v0.1.2.0》。
***********************************************************************/
#pragma once

#ifndef LUSTER_LFNC_DEF_H_
#define LUSTER_LFNC_DEF_H_


//普通参数
/*—————————————————————————————————————————————————————————————————————————————
|       宏名称                          LFNC名称                    数据类型           单位          参数含义		                                       |
—————————————————————————————————————————————————————————————————————————————*/
#define LFNC_WIDTH                      "Width"                     /*  Int            Pixel         相机图像宽度                                         */
#define LFNC_HEIGHT                     "Height"                    /*  Int            Pixel         相机图像高度                                         */
#define LFNC_OFFSET_X                   "OffsetX"                   /*  Int            Pixel         相机图像水平偏置                                     */
#define LFNC_OFFSET_Y                   "OffsetY"                   /*  Int            Pixel         相机图像垂直偏置                                     */
#define LFNC_PIXEL_FORMAT               "PixelFormat"               /*  Enum           -             相机图像像素格式（参见PFNC）                         */
#define LFNC_ACQ_FRAME_RATE             "AcquisitionFrameRate"      /*  Double         Hz            相机抓取到一帧图像的频率                             */
#define LFNC_ACQ_LINE_RATE              "AcquisitionLineRate"       /*  Double         Hz            相机抓取到一行图像的频率                             */
#define LFNC_EXP_TIME                   "ExposureTime"              /*  Double         us            曝光时间（微秒）                                     */
#define LFNC_GAIN                       "Gain"                      /*  Double         -             增益，单位取决于设备                                 */
#define LFNC_BLACK_LEVEL                "BlackLevel"                /*  Double         -             偏置，单位取决于设备                                 */
#define LFNC_GAMMA                      "Gamma"                     /*  Double         -             伽马矫正                                             */
#define LFNC_TRIG_MODE                  "TriggerMode"               /*  Enum           -             触发模式选择（0->Off, 1->On）                        */
#define LFNC_TRIG_SRC                   "TriggerSource"             /*  Enum           -             触发源选择（0->Line0, 7->Software）                  */
#define LFNC_TRIG_SOFTWARE              "TriggerSoftware"           /*  Int            -             发出一个内部软触发信号(无需发送值)                   */
#define LFNC_ACQ_FRAME_RATE_AUTO        "AcquisitionFrameRateAuto"  /*  Enum           -             帧频率自动（0->Off, 2->Continuous）                  */
#define LFNC_EXP_TIME_AUTO              "ExposureAuto"              /*  Enum           -             曝光时间自动（0->Off, 1->Once, 2->Continuous）       */
#define LFNC_GAIN_AUTO                  "GainAuto"                  /*  Enum           -             增益自动（0->Off, 1->Once, 2->Continuous）           */
#define LFNC_BLACK_LEVEL_AUTO           "BlackLevelAuto"            /*  Enum           -             偏置自动（0->Off, 2->Continuous）                    */

//特有参数
/*———————————————————————————————————————————————————————————————————————————————————
|       宏名称                          LFNC名称                         数据类型           单位          参数含义		                                                |
————————————————————————————————————————————————————————————————————————————————————*/

//用户参数（User Parameters） – 图像控制 – 未在csv上的参数 – 不存于相机 
#define LFNC_SICK_SCALEY               "SICK_User_ScaleY"               /*Double              mm          Y分辨率*/
#define LFNC_SICK_PARAMETPATH          "SICK_User_ParametPath"          /*std::string/char*               参数配置文件路径*/
#define LFNC_SICK_CALIBRAPATH          "SICK_User_CalibraPath"          /*std::string/char*               标定文件路径*/ 
#define LFNC_SICK_CAMMAC               "SICK_Device_CamMac"             /*std::string                     相机mac地址         */
#define LFNC_SICK_DEVICENAME           "SICK_Device_Name"               /*std::string                     设备名字*/


//扩展接口参数
/*———————————————————————————————————————————————————————————————————————
|       宏名称                          LFNC名称                    数据类型           单位          参数含义		                           |
———————————————————————————————————————————————————————————————————————*/
#define LFNC_GET_STORE_IMAGE            "LstGetStoreImage"          /*  Uncertain      -             获取存储类型缓存中的一帧图像数据         */
#define LFNC_RELEASE_STORE_IMAGE        "LstReleaseStoreImage"      /*  Uncertain      -             释放存储类型缓存中的一帧图像数据         */
#define LFNC_SNAP_WAIT_TIME             "LstSnapWaitTime"           /*  Uncertain      ms            获取图像数据的最大等待时间，默认1000ms   */
#define LFNC_GET_NODE_NUM               "LstGetNodeNum"             /*  Uncertain      -             获取xml中描述的相机参数总数              */
#define LFNC_GET_NODE_INFO              "LstGetNodeInfo"            /*  Uncertain      -             获取xml中描述的相机参数信息结构体（所有）*/
#define LFNC_DESTORY_CAMERA             "LstDestoryCamera"          /*  Uncertain      -             析构所有的相机资源                       */
#define LFNC_GET_IMAGE_PROPERTY         "LstGetImageProperty"       /*  Uncertain      -             获取图像属性结构体                       */


#endif   //LUSTER_LFNC_DEF_H_