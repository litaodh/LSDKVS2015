/***********************************************************************
*
* Copyright (c) 2000-2001 Luster LightTech(Beijing) Co.Ltd.
* All Rights Reserved.
*
*
* FILE NAME     :  LstCameraControl_C.h
* PROJECT NAME  :  LSDK
* ABSTRACT      :  相机控制模块头文件（标准C语言版本）。
*
*
* VERSION       :  1.0.0
* DATE 		    :  2018/02/14
* AUTHOR 		:  刘麒
* NOTE 		    :  对应接口文档《LSDK相机控制统一接口说明(标准C语言)_v1.0.0》。
***********************************************************************/
#pragma once

#ifndef LST_CAMERA_CONTROL_C_H_
#define LST_CAMERA_CONTROL_C_H_

#include "LstCommonDef.h"


#define MESSAGE_LEN_256 256

/*
*enum
*/

//返回状态类型枚举，包括正常状态和多种错误状态。
//枚举范围0x01000130 ~ 0x0100014F
typedef enum tagEnumLusterStatus
{
	L_OK = 0x01001000,	//正常状态
	L_ERR_CCNOED = 0x01001001,	//相机未打开，没有连接图像采集设备
	L_ERR_CCWC = 0x01001002,	//错误的彩色位深
	L_ERR_CCWD = 0x01001003,	//错误的采集设备
	L_ERR_CCWPF = 0x01001004,	//错误或不支持的像素格式
	L_ERR_CCNV = 0x01001005,	//没有视频信号
	L_ERR_CCU = 0x01001006,	//未检测相机数量和信息
	L_ERR_CCF = 0x01001007,	//图像抓取失败
	L_ERR_CCWR = 0x01001008,	//选择错误的图像分辨率
	L_ERR_CCWP = 0x01001009,	//选择错误的图像部分
	L_ERR_CCWPR = 0x0100100A,	//选择错误的像素比例
	L_ERR_CCWH = 0x0100100B,	//句柄无效
	L_ERR_CCCL = 0x0100100C,	//实例无效（或者已经注销）
	L_ERR_CCNIB = 0x0100100D,	//无法初始化缓存
	L_ERR_CCET = 0x0100100E,	//不支持外触发
	L_ERR_CCLI = 0x0100100F,	//错误的相机输入行（多重）
	L_ERR_CCCS = 0x01001010,	//错误的颜色空间
	L_ERR_CCPT = 0x01001011,	//错误的端口
	L_ERR_CCCT = 0x01001012,	//错误的相机类型
	L_ERR_CCTM = 0x01001013,	//超过了采集设备类的最大数目
	L_ERR_CCDV = 0x01001014,	//设备忙碌
	L_ERR_CCASYNC = 0x01001015,	//不支持异步抓取
	L_ERR_CCPARAM = 0x01001016,	//该参数暂不支持
	L_ERR_CCTIMEOUT = 0x01001017,	//超时
	L_ERR_CCGAIN = 0x01001018,	//无效的增益
	L_ERR_CCFIELD = 0x01001019,	//无效的字段
	L_ERR_CCPART = 0x0100101A,	//无效的参数类型
	L_ERR_CCPARV = 0x0100101B,	//无效的参数值
	L_ERR_CCFNS = 0x0100101C,	//不支持或已被遗弃的功能
	L_ERR_CCIVERS = 0x0100101D,	//版本不兼容的接口
	L_ERR_CCSETPAR = 0x0100101E,	//设置参数值失败
	L_ERR_CCGETPAR = 0x0100101F,	//查询参数值失败
	L_ERR_CCPARNA = 0x01001020,	//当前配置下，该参数不可用
	L_ERR_CCCLOSE = 0x01001021,	//设备不能正常关闭
	L_ERR_CCCAMFILE = 0x01001022,	//相机配置文件不能打开
	L_ERR_CCCALLBACK = 0x01001023,	//不支持的回调类型
	L_ERR_CCDEVLOST = 0x01001024,	//设备丢失
	L_ERR_DNA = 0x01001025,	//设备或操作不可用
	L_ERR_MEM = 0x01001026,	//内存不足
	L_ERR_CCFWIDX = 0x01001027,	//无效的相机索引
	L_ERR_CCFWVI = 0x01001028,	//厂家接口错误
	L_ERR_CCXML = 0x01001029,	//XML配置文件不能打开
	L_ERR_CCFIDFV = 0x0100102A,	//默认参数初始化失败
	L_ERR_CCOPEN = 0x0100102B,	//打开相机失败
	L_ERR_CCRXFVF = 0x0100102C,	//读取XML参数值失败
	L_ERR_CCNRB = 0x0100102D,	//无法释放缓存
	L_ERR_CCWLPI = 0x0100102E,	//无法加载插件，可能名称有误，或缺失依赖文件
	L_ERR_CCWFPI = 0x0100102F,	//无法释放插件
	L_ERR_CCOED = 0x01001030,	//相机已经打开
	L_ERR_CCIED = 0x01001031,	//相机已经初始化
	L_ERR_CCNIED = 0x01001032,	//相机未初始化
	L_ERR_CCGED = 0x01001033,	//相机已经采集
	L_ERR_CCNGED = 0x01001034,	//相机未采集
	L_ERR_UNKNOWN = 0x01001FFF,	//未知错误
} LE_STATUS;

//回调函数类型枚举，包括采集存储回调类型、采集显示回调类型、采集消息回调类型、未知类型Unknow。
//枚举范围0x01000130 ~ 0x0100014F
typedef enum tagEnumCallbackType
{
	//采集存储回调类型，返回完全的数据
	CB_STORE = 0x01000130,
	//采集显示回调类型，内触发下帧频过高会抽帧返回数据，外触发下无法获得帧频因此无法抽帧
	CB_DISPLAY = 0x01000131,
	//采集消息回调类型，这里主要用来进行丢帧问题调试，可以返回帧计数、帧计时等信息辅助判断丢帧原因。
	CB_MESSAGE = 0x01000132,
	//未知类型
	CB_UNKNOW = 0x0100014F
} LE_CALLBACK_TYPE;

//相机接口类型枚举，包括CameraLink接口、GigE Vision网口、USB 2.0接口、USB 3.0接口、IEEE1394接口、CoaXPress接口等。
//枚举范围0x01000170 ~ 0x0100018F
typedef enum tagEnumCameraInterfaceType
{
	//Camera Link接口
	CIT_CAMERALINK = 0x01000170,
	//GigE Vision网口
	CIT_GIGE = 0x01000171,
	//USB 2.0接口
	CIT_USB2 = 0x01000172,
	//USB 3.0接口
	CIT_USB3 = 0x01000173,
	//IEEE1394接口
	CIT_IEEE1394 = 0x01000174,
	//CoaXPress接口
	CIT_COAXPRESS = 0x01000175,
	//未知类型
	CIT_UNKNOWN = 0x0100018F
} LE_CAMERA_INTERFACE_TYPE;

//采集消息类型枚举
//枚举范围0x01000190 ~ 0x010001EF
typedef enum tagEnumMessageType
{
	//厂家获取图像函数失败，一般为硬件丢帧。
	MT_CBT_VGIMF = 0x01000190,
	//相机->缓存线程，上一帧图像和当前帧图像的帧计数不连续（大于1）
	MT_CBT_FCNC = 0x01000191,
	//相机->缓存线程，当前帧图像存入缓存的处理时间过长（超过5ms）
	MT_CBT_TTL = 0x01000192,
	//相机->缓存线程，存储类型缓存队列已满
	MT_CBT_SBF = 0x01000193,
	//相机->缓存线程，显示类型缓存队列已满
	MT_CBT_DBF = 0x01000194,
	//缓存->上层应用线程，上一帧图像和当前帧图像的帧计数不连续（大于1）
	MT_BAT_FCNC = 0x01000195,
	//未知类型
	MT_UNKNOWN = 0x010001EF,
} LE_MESSAGE_TYPE;



/*
*struct
*/

//系统时间结构体，存放记录系统的时间点，包括年、月、日、小时、分钟、秒、毫秒，可以存放每一帧图像存储的时间信息。
typedef struct tagStructSystemTime
{
	//年
	int iYear;
	//月
	int iMonth;
	//日
	int iDay;
	//小时
	int iHour;
	//分钟
	int iMinute;
	//秒
	int iSecond;
	//毫秒
	int iMilliSeconds;
} LST_SYSTEM_TIME;

//标定后的坐标相关信息
typedef struct tagStructCalCoordInfo
{
	//X 的最小值，单位 毫米
	double LowerBoundX;
	// X 的最大值，单位 毫米
	double UpperBoundX;
	//Z 的最小值，单位 毫米
	double LowerBoundR;
	//Z 的最大值，单位 毫米
	double UpperBoundR;
}LST_CALIBRATIONCOORD_INFO;

//未标定的坐标相关信息
typedef struct tagStructCoordInfo
{
	int RegionHeight;
	int RegionOffsetY;
	int RangAixFlat;
}LST_COORD_INFO;
//Ranger3xml相关信息
typedef struct tagStructRanger3XmlInfo
{
	//是否标定
	int tagCalibration;
	//是否包含激光强度图0 不包含 1 包含
	int tagHasReflectance;
	union
	{
		//已标定坐标信息
		LST_CALIBRATIONCOORD_INFO stCalibrationCoordInfo;
		//未标定坐标信息
		LST_COORD_INFO stCoordInfo;
	}CoordinateInfo;

}LST_RANGER3XML_INFO;


//采集图像信息结构体，存放图像数据指针、图像数据大小、图像数据帧号、图像步频、
//缓存已满个数、图像属性结构体、图像数据采集时间结构体、预留字段等信息。
typedef struct tagStructSnapImageInfo
{
	/*图像数据指针*/
	BYTE* pchBuffer;
	/*图像数据大小*/
	int iBufferSize;
	/*图像数据帧号*/
	int iImageFrameIndex;
	/*图像步频*/
	int iImageStrideRate;
	/*缓存已满个数*/
	int iBufferFullNum;
	/*图像属性结构体*/
	LST_IMAGE_PROPERTY stImageProperty;
	/*图像数据采集时间结构体*/
	LST_SYSTEM_TIME stSysTime;
	//附加信息
	void* pstTagInfo;
	//附加信息大小
	int iTagInfoSize;
	/*预留字段*/
	char achReserved[128];
} LST_IMAGE_INFO;

//消息信息结构体，存放消息种类、消息内容。
typedef struct tagStructMessageInfo
{
	/*消息种类*/
	int iMessageType;
	/*消息内容*/
	char achMessage[MESSAGE_LEN_256];
} LST_MESSAGE_INFO;

//相机信息结构体，存放相机索引、相机ID、相机SDK的版本号、相机接口、相机序列号、预留字段。
typedef struct tagStructCameraInfo
{
	/*相机索引*/
	int iCameraIndex;
	/*相机ID*/
	char achCameraID[128];
	/*相机SDK的版本号*/
	char achSdkVersion[128];
	/*相机的接口，参见LE_CAMERA_INTERFACE_TYPE枚举*/
	int  iCameraInterface;
	/*相机序列号*/
	char achSerialNumber[64];
	/*预留字段 Ranger3中启用为相机mac地址*/
	char achReserved[128];
} LST_CAMERA_INFO;

//采集缓存信息结构体，包括缓存数量、整合缓存的个数。
typedef struct tagStructBufferInfo
{
	/*缓存数量*/
	int iBufferNum;
	/*整合缓存的个数，默认是1*/
	int iMergeFrameNum;
	/*预留字段*/
	char achReserved[128];
} LST_BUFFER_INFO;

/*
*Function Ptr
*/

//回调函数指针。按回调类型可以包括
//(1)采集存储回调类型
//(2)采集显示回调类型
//(3)采集消息回调类型
typedef void(*LCC_C_Callback)(int iCallBackType, void* pCameraHandle, void* pData, void* pUserContext);




#ifdef __cplusplus
extern "C" {
#endif

#ifdef LCC_C_EXPORTS
#define LCC_C_API __declspec(dllexport)
#else
#define LCC_C_API __declspec(dllimport)
#endif

	/******************************************************
	@函数说明： 检测相机的数目以及设备信息。
	@参数输入： 无
	@参数输出： pstCameraInfo -- 相机信息结构体指针，指针指向相机信息结构体数组，用来存放多台相机的相机信息。
	iCameraNum -- 检测到的相机数量。
	@返回值  ： 见LE_STATUS枚举
	******************************************************/
	LCC_C_API LE_STATUS __stdcall LCC_C_DetectCameraNum(LST_CAMERA_INFO **pstCameraInfo, int *piCameraNum);

	/******************************************************
	@函数说明： 打开相机。包括读取相机xml参数文件、创建相机相关资源对象、激活上位机系统与相机（或采集卡）
	对象关联的传输数据流、初始化相机参数等。
	@参数输入： iCameraIndex -- 需要连接相机的相机索引号，从0开始。
	pchXmlFileName -- 需要连接相机的xml配置文档的绝对路径。
	@参数输出： pCameraHandle -- 相机句柄指针。如果打开成功则输出大于零的指针，如果打开失败则为默认值NULL（0）。
	@返回值  ： 见LE_STATUS枚举
	******************************************************/
	LCC_C_API LE_STATUS __stdcall LCC_C_OpenCamera(int iCameraIndex, const char *pchXmlFileName, void **pCameraHandle);

	/******************************************************
	@函数说明： 关闭相机。释放相机相关资源对象、消除上位机系统与传输对象关联的传输数据流、相机相关参数。
	@参数输入： pCameraHandle -- 相机句柄指针。
	@参数输出： 无
	@返回值  ： 见LE_STATUS枚举
	******************************************************/
	LCC_C_API LE_STATUS __stdcall LCC_C_CloseCamera(void *pCameraHandle);

	/******************************************************
	@函数说明： 初始化相机。主要根据给入的相机信息参数、缓存信息参数，创建并初始化底层的数据结构体、缓存数据块等。
	注意：此处的数据结构体与缓存数据块主要是Luster SDK在PC系统内存中创建的，而不是在采集卡或相机中创建的。
	@参数输入： pCameraHandle -- 相机句柄指针。
	stBufferInfo -- 缓存信息结构体。
	@参数输出： 无
	@返回值  ： 见LE_STATUS枚举
	******************************************************/
	LCC_C_API LE_STATUS __stdcall LCC_C_InitCamera(void *pCameraHandle, LST_BUFFER_INFO stBufferInfo);

	/******************************************************
	@函数说明： 释放相机。释放底层的数据结构体、缓存数据块等。
	@参数输入： pCameraHandle -- 相机句柄指针。
	@参数输出： 无
	@返回值  ： 见LE_STATUS枚举
	******************************************************/
	LCC_C_API LE_STATUS __stdcall LCC_C_ReleaseCamera(void *pCameraHandle);

	/******************************************************
	@函数说明： 开始相机采集。此命令发送后，相机如果有采集信号（内触发或外触发），则相机开始向采集卡连续传输图像数据。
	注意：开始采集后，如果注册了回调事件，可以从该回调函数中得到采集到的图像数据。
	@参数输入： pCameraHandle -- 相机句柄指针。
	@参数输出： 无
	@返回值  ： 见LE_STATUS枚举
	******************************************************/
	LCC_C_API LE_STATUS __stdcall LCC_C_StartGrab(void *pCameraHandle);

	/******************************************************
	@函数说明： 停止相机采集。此命令发送后，则相机停止向采集卡连续传输图像数据。
	@参数输入： pCameraHandle -- 相机句柄指针。
	@参数输出： 无
	@返回值  ： 见LE_STATUS枚举
	******************************************************/
	LCC_C_API LE_STATUS __stdcall LCC_C_StopGrab(void *pCameraHandle);

	/******************************************************
	@函数说明： 相机单帧采集。LusterSnapImage与LusterStartGrab功能类似，但Snap只抓取一帧图像后就停止抓取，
	抓图过程中回调函数仅调用一次。该函数主要在调试过程中使用。
	@参数输入： pCameraHandle -- 相机句柄指针。
	@参数输出： 无
	@返回值  ： 见LE_STATUS枚举
	******************************************************/
	LCC_C_API LE_STATUS __stdcall LCC_C_SnapImage(void *pCameraHandle);

	/******************************************************
	@函数说明： 注册回调函数。将LusterCameraCallback回调函数指针注册进入软件中，用户可通过此回调函数获得图像数据。
	@参数输入： iCallbackType -- 注册的回调函数的类型，包括：采集存储类型，返回完全的数据；显示回调类型，可能会抽帧抽行返回数据；
	采集消息类型；详见LE_CALLBACK_TYPE枚举，以及LusterCameraCallback说明。
	pCallback     -- 回调函数指针。
	pCameraHandle -- 相机句柄指针。
	pUserContext  -- 用户上下文信息。注意：C++中一般用于给入用户类的指针，C#中可以不关注该值。
	除此之外，可以利用该指针传输其他特殊需求的内容。
	@参数输出： 无
	@返回值  ： 见LE_STATUS枚举
	******************************************************/
	LCC_C_API LE_STATUS __stdcall LCC_C_RegisterCallback(LCC_C_Callback pCallback, int iCallbackType, void *pCameraHandle, void *pUserContext);

	/******************************************************
	@函数说明： 注销回调函数。取消由LusterRegisterCallback注册的回调事件。
	@参数输入： iCallbackType -- 指定需要注册的回调函数的类型。详见LE_CALLBACK_TYPE枚举说明、
	LusterCameraCallback回调函数指针说明、LusterRegisterCallback接口说明。
	pCameraHandle -- 相机句柄指针。
	@参数输出： 无
	@返回值  ： 见LE_STATUS枚举
	******************************************************/
	LCC_C_API LE_STATUS __stdcall LCC_C_UnregisterCallback(int iCallbackType, void *pCameraHandle);

	/******************************************************
	@函数说明： 获取相机参数的数据类型。
	@参数输入： pCameraHandle -- 相机句柄指针。
	pchFeatureName -- 相机参数名称，参见LFNC文档。
	@参数输出： stFeatureInfo -- 参数信息结构体，参见LST_FEATURE_INFO。
	@返回值  ： 见LE_STATUS枚举
	******************************************************/
	LCC_C_API LE_STATUS __stdcall LCC_C_GetFeatureInfo(void *pCameraHandle, const char *pchFeatureName, LST_FEATURE_INFO *pstFeatureInfo);

	/******************************************************
	@函数说明： 获取相机参数（整形类型）。
	@参数输入： pCameraHandle -- 相机句柄指针。
	pchFeatureName -- 相机参数名称，参见LFNC文档。
	@参数输出： stFeatureValue -- 整型数据信息结构体，参见LST_INT_INFO。
	@返回值  ： 见LE_STATUS枚举
	******************************************************/
	LCC_C_API LE_STATUS __stdcall LCC_C_GetFeatureInt(void *pCameraHandle, const char *pchFeatureName, LST_INT_INFO *pstFeatureValue);

	/******************************************************
	@函数说明： 获取相机参数（布尔类型）。
	@参数输入： pCameraHandle -- 相机句柄指针。
	pchFeatureName -- 相机参数名称，参见LFNC文档。
	@参数输出： stFeatureValue -- 布尔型数据信息结构体，参见LST_BOOLEN_INFO。
	@返回值  ： 见LE_STATUS枚举
	******************************************************/
	LCC_C_API LE_STATUS __stdcall LCC_C_GetFeatureBoolean(void *pCameraHandle, const char *pchFeatureName, LST_BOOLEAN_INFO *pstFeatureValue);

	/******************************************************
	@函数说明： 获取相机参数（双精度浮点类型）。
	@参数输入： pCameraHandle -- 相机句柄指针。
	pchFeatureName -- 相机参数名称，参见LFNC文档。
	@参数输出： stFeatureValue -- 浮点型数据信息结构体，参见LST_DOUBLE_INFO。
	@返回值  ： 见LE_STATUS枚举
	******************************************************/
	LCC_C_API LE_STATUS __stdcall LCC_C_GetFeatureDouble(void *pCameraHandle, const char *pchFeatureName, LST_DOUBLE_INFO *pstFeatureValue);

	/******************************************************
	@函数说明： 获取相机参数（字符串类型）。
	@参数输入： pCameraHandle -- 相机句柄指针。
	pchFeatureName -- 相机参数名称，参见LFNC文档。
	@参数输出： stFeatureValue -- 字符串型数据信息结构体，参见LST_STRING_INFO。
	@返回值  ： 见LE_STATUS枚举
	******************************************************/
	LCC_C_API LE_STATUS __stdcall LCC_C_GetFeatureString(void *pCameraHandle, const char *pchFeatureName, LST_STRING_INFO *pstFeatureValue);

	/******************************************************
	@函数说明： 获取相机参数（枚举类型）。
	@参数输入： pCameraHandle -- 相机句柄指针。
	pchFeatureName -- 相机参数名称，参见LFNC文档。
	@参数输出： stFeatureValue -- 枚举型数据信息结构体，参见LST_ENUM_INFO。
	@返回值  ： 见LE_STATUS枚举
	******************************************************/
	LCC_C_API LE_STATUS __stdcall LCC_C_GetFeatureEnum(void *pCameraHandle, const char *pchFeatureName, LST_ENUM_INFO *pstFeatureValue);

	/******************************************************
	@函数说明： 获取相机参数（不确定数据类型）。该接口主要用来兼容不常见的数据类型，因此采用void*的不定参数类型，
	用户可以在这里给入常见变量、指针变量、变量数组、结构体数组等内容，具体的使用注意事项需根据具体的相机进行处理，
	请参考相应型号的相机二次开发使用说明书。
	@参数输入： pCameraHandle -- 相机句柄指针。
	pchFeatureName -- 相机参数名称，参见LFNC文档。
	@参数输出： pFeatureValue -- 相机参数值，不定类型，特殊参数特殊定义。
	@返回值  ： 见LE_STATUS枚举
	******************************************************/
	LCC_C_API LE_STATUS __stdcall LCC_C_GetFeatureUncertain(void *pCameraHandle, const char *pchFeatureName, void **pFeatureValue);

	/******************************************************
	@函数说明： 设置相机参数（整型类型）。
	@参数输入： pCameraHandle -- 相机句柄指针。
	pchFeatureName -- 相机参数名称，参见LFNC文档。
	iFeatureValue -- 相机参数值。
	@参数输出： 无
	@返回值  ： 见LE_STATUS枚举
	******************************************************/
	LCC_C_API LE_STATUS __stdcall LCC_C_SetFeatureInt(void *pCameraHandle, const char *pchFeatureName, int iFeatureValue);

	/******************************************************
	@函数说明： 设置相机参数（布尔类型）。
	@参数输入： pCameraHandle -- 相机句柄指针。
	pchFeatureName -- 相机参数名称，参见LFNC文档。
	bFeatureValue -- 相机参数值。
	@参数输出： 无
	@返回值  ： 见LE_STATUS枚举
	******************************************************/
	LCC_C_API LE_STATUS __stdcall LCC_C_SetFeatureBoolean(void *pCameraHandle, const char *pchFeatureName, bool bFeatureValue);

	/******************************************************
	@函数说明： 设置相机参数（双精度浮点类型）。
	@参数输入： pCameraHandle -- 相机句柄指针。
	pchFeatureName -- 相机参数名称，参见LFNC文档。
	dFeatureValue -- 相机参数值。
	@参数输出： 无
	@返回值  ： 见LE_STATUS枚举
	******************************************************/
	LCC_C_API LE_STATUS __stdcall LCC_C_SetFeatureDouble(void *pCameraHandle, const char *pchFeatureName, double dFeatureValue);

	/******************************************************
	@函数说明： 设置相机参数（字符串类型）。
	@参数输入： pCameraHandle -- 相机句柄指针。
	pchFeatureName -- 相机参数名称，参见LFNC文档。
	pchFeatureValue -- 相机参数值。
	@参数输出： 无
	@返回值  ： 见LE_STATUS枚举
	******************************************************/
	LCC_C_API LE_STATUS __stdcall LCC_C_SetFeatureString(void *pCameraHandle, const char *pchFeatureName, const char *pchFeatureValue);

	/******************************************************
	@函数说明： 设置相机参数（枚举类型）。
	@参数输入： pCameraHandle -- 相机句柄指针。
	pchFeatureName -- 相机参数名称，参见LFNC文档。
	iFeatureValue -- 相机参数值。
	@参数输出： 无
	@返回值  ： 见LE_STATUS枚举
	******************************************************/
	LCC_C_API LE_STATUS __stdcall LCC_C_SetFeatureEnum(void *pCameraHandle, const char *pchFeatureName, int iFeatureValue);

	/******************************************************
	@函数说明： 设置相机参数（不确定数据类型）。该接口主要用来兼容不常见的数据类型，因此采用void*的不定参数类型，
	用户可以在这里给入常见变量、指针变量、变量数组、结构体数组等内容，具体的使用注意事项需根据具体的相机进行处理，
	请参考相应型号的相机二次开发使用说明书。
	@参数输入： pCameraHandle -- 相机句柄指针。
	pFeatureName -- 相机参数名称，参见LFNC文档。
	pFeatureValue -- 相机参数值。
	@参数输出： 无
	@返回值  ： 见LE_STATUS枚举
	******************************************************/
	LCC_C_API LE_STATUS __stdcall LCC_C_SetFeatureUncertain(void *pCameraHandle, const char *pchFeatureName, void *pFeatureValue);

	/******************************************************
	@函数说明： 相机重启。
	@参数输入： pCameraHandle -- 相机句柄指针。
	@参数输出： 无
	@返回值  ： 见LE_STATUS枚举
	******************************************************/
	LCC_C_API LE_STATUS __stdcall LCC_C_RestartCamera(void *pCameraHandle);

	/******************************************************
	@函数说明： 恢复出厂设置。
	@参数输入： pCameraHandle -- 相机句柄指针。
	@参数输出： 无
	@返回值  ： 见LE_STATUS枚举
	******************************************************/
	LCC_C_API LE_STATUS __stdcall LCC_C_RestoreFactory(void *pCameraHandle);
	/******************************************************
	@函数说明： 加载csv配置文件。
	@参数输入： index -- 相机索引。
	pchCSVName--csv名称（带路径）
	@参数输出： 无
	@返回值  ： 见LE_STATUS枚举
	******************************************************/
	//LCC_C_API LE_STATUS __stdcall LCC_C_LoadCSV(int index, const char *pchCSVName);



#ifdef __cplusplus
}
#endif


#endif   //LST_CAMERA_CONTROL_C_H_