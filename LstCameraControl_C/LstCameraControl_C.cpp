// LstCameraControl_C.cpp : 定义 DLL 应用程序的导出函数。
//



// LusterCameraControlC.cpp : 定义 DLL 应用程序的导出函数。
/***********************************************************************
*
* Copyright (c) 2000-2001 Luster LightTech(Beijing) Co.Ltd.
* All Rights Reserved.
*
*
* FILE NAME     :  LusterCameraControlC.cpp
* PROJECT NAME  :  LusterSDK
* ABSTRACT      :  相机控制模块头文件（标准C语言版本）。
*				   详细参见最新的软件方案与接口设计文档《组件SDK与相机采集软件方案_v1.10》和
*					《组件SDK相机控制统一接口详细设计说明_v0.1.1.0》
*
*
* VERSION       :  0.1.2.0
* DATE 		    :  2017/12/6
* AUTHOR 		:  雷丹
* NOTE 		    :  驱动：Ranger3-1.1.3.1497
*				   相机厂商：Sick
*				   相机型号：Ranger3
*				   对应接口文档《组件SDK相机控制统一接口详细设计说明(标准C语言)_v0.1.2.0》。
*
*
* VERSION       :  1.0
* DATE 		    :  2018/3/12
* AUTHOR 		:  雷丹
* NOTE 		    :  驱动：Ranger3-1.1.3.1497
*				   相机厂商：Sick
*				   相机型号：Ranger3
*				   对应接口文档《LSDK相机控制统一接口说明(标准C语言)_v1.0.0》。
* VERSION       :  2.0
* DATE 		    :  2019/1/24
* AUTHOR 		:  李萍
* NOTE 		    :  驱动：Ranger3-2.0.3.1866
*				   相机厂商：Sick
*				   相机型号：Ranger3
*				   对应接口文档《LSDK相机控制统一接口说明(标准C语言)_v1.0.0》。

* VERSION       :  3.0
* DATE 		    :  2020/4/20
* AUTHOR 		:  李涛
* NOTE 		    :  驱动：Ranger3-2.1.0.3050
*				   相机厂商：Sick
*				   相机型号：Ranger3
*				   对应接口文档《LSDK相机控制统一接口说明(标准C语言)_v1.0.0》。

***********************************************************************/

//#include "afxdialogex.h"
#include "stdafx.h"
#include <time.h>
#include <string>
#include <string.h>
#include "LstXmlManagerCF.h"
#include "LstCameraControl_C.h"
#include "LstLfncDef.h"
#include <algorithm>
#include <sstream>
#include <iostream>
#include <vector>
#include "DatXmlWriter.h"


//using namespace SickCam;

/***********************************************************************
*
*				       全局变量声明
*
***********************************************************************/

#define IS_USE_CLOCK 0
#define IS_USE_FAST 0
#define IS_USE_INDEX    1

#define SNAP_BUF_NUM    1
#define SNAP_BUF_INDEX  0

#define CAMERA_MAX_NUM        32		//支持相机最大数量		
#define DISPLAY_MAX_BUFF      5			//显示缓存数目
#define EMPTY_STRING          '\0'		//定义空字符
#define INVALID_CAMERA_INDEX  -1        //定义无效相机索引值
#define INDEX_SEMAPHORE       100		//定义信号量存放在LST_IMAGE_INFO结构体的char achReserved[128]属性的字符索引
#define LEN_128               128		//字符串长度128

static int m_iCameraNum = 0;								//检测到的相机总数量
static int m_iCameraNumOpen = 0;							//打开的相机总数量
static int m_iShowFps[CAMERA_MAX_NUM];					//显示帧频
static int m_iRecordMode[CAMERA_MAX_NUM];				//存储开关，0为初始值，1代表开启，2代表关闭
static double m_dCurFps[CAMERA_MAX_NUM];				//当前的显示帧频，主要用于抽帧显示时计算用
static void *m_pCamHandle[CAMERA_MAX_NUM];				//用于存放相机句柄
static int m_iCamIndex[CAMERA_MAX_NUM];					//用于存放相机索引  ={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31}

static std::shared_ptr<R3S> m_pRanger3Share;// = std::make_shared<R3S>("");           //Ranger3基础类
static SickCam::Ranger3 *m_paoRanger3s[CAMERA_MAX_NUM];   //图像采集类;
static LST_CAMERA_INFO	*m_pastCameraInfo[CAMERA_MAX_NUM];	//相机信息结构体

static LCC_C_Callback m_pStoreCallBack[CAMERA_MAX_NUM];		//存储类型的回调指针
static LCC_C_Callback m_pMessageCallBack[CAMERA_MAX_NUM];	//消息类型的回调指针
static LCC_C_Callback m_pDisplayCallBack[CAMERA_MAX_NUM];	//显示类型的回调指针
static void	*m_pStoreUserContext[CAMERA_MAX_NUM];			//存储类型的用户上下文
static void	*m_pDisplayUserContext[CAMERA_MAX_NUM];			//消息类型的用户上下文
static void	*m_pMessageUserContext[CAMERA_MAX_NUM];			//显示类型的用户上下文

static HANDLE m_ThreadPushHandle[CAMERA_MAX_NUM];		//线程句柄，读取图像线程，读取相机端的Buffer到计算机端的Buffer。
static HANDLE m_ThreadPopStoreHandle[CAMERA_MAX_NUM];	//线程句柄，存储回调线程，读取计算机端的Buffer通过存储类型回调传给上层调用者，该图像数据用于存储，该图像数据需要保障不丢帧。
static HANDLE m_ThreadPopDisplayHandle[CAMERA_MAX_NUM];	//线程句柄，显示回调线程，读取计算机端的Buffer通过显示类型回调传给上层调用者，该图像数据用于显示，因此存在抽帧抽行功能。
static int    m_iThreadID[CAMERA_MAX_NUM];				//线程ID，此处存放了相机的索引CameraIndex
static bool   m_bIsStopReceive[CAMERA_MAX_NUM];			//状态变量，是否停止获取图像，ThreadReceiveImage
static bool   m_bIsStopStore[CAMERA_MAX_NUM];			//状态变量，是否停止存储图像，ThreadStoreImage
static bool   m_bIsStopDisplay[CAMERA_MAX_NUM];			//状态变量，是否停止显示图像，ThreadDisplayImage

static XmlTreeNode *m_poTreeNodeRoot[CAMERA_MAX_NUM];	//XML配置文件导入参数*/
static int m_iSnapWaitTime[CAMERA_MAX_NUM];				//单帧抓图等待时间
static int m_iImageCount[CAMERA_MAX_NUM];                //图像帧数

/***********************************************************************
*				      缓存模块
***********************************************************************/
static LST_BUFFER_INFO	  *m_pastBufferInfo[CAMERA_MAX_NUM];		//采集缓存信息结构体
static LST_IMAGE_PROPERTY *m_pastImageProperty[CAMERA_MAX_NUM];		//图像属性结构体

static LST_IMAGE_INFO	  *m_pastStoreImageInfo[CAMERA_MAX_NUM];	//采集图像信息结构体（存储类型）
static LST_IMAGE_INFO	  *m_pastDisplayImageInfo[CAMERA_MAX_NUM];	//采集图像信息结构体（显示类型）
static LST_IMAGE_INFO     *m_pastSnapImageInfo[SNAP_BUF_NUM];		//Snap单帧图像临时存图结构体，TODO:这里不应该SNAP_BUF_NUM，应该CAMERA_MAX_NUM
static LST_RANGER3XML_INFO *m_pastRanger3XMLInfo[CAMERA_MAX_NUM];   //Ranger3 xml文件信息结构体

static int m_iPushStoreBufferIndex[CAMERA_MAX_NUM];		//当前Buffer索引，存储Buffer，Camera→Buffer
static int m_iPopStoreBufferIndex[CAMERA_MAX_NUM];		//当前Buffer索引，存储Buffer，Buffer→CallBack
static int m_iPushDisplayBufferIndex[CAMERA_MAX_NUM];	//当前Buffer索引，显示Buffer，Camera→Buffer
static int m_iPopDisplayBufferIndex[CAMERA_MAX_NUM];	//当前Buffer索引，显示Buffer，Buffer→CallBack

/***********************************************************************
*				      状态模块
***********************************************************************/
static BOOL m_bDetected = FALSE;									//是否已经查询相机
static BOOL m_bOpened[MAX_CAMERA_NUM] = { FALSE };					//是否已经打开相机
static BOOL m_bInited[MAX_CAMERA_NUM] = { FALSE };					//是否已经初始化相机
static BOOL m_bGrabbed[MAX_CAMERA_NUM] = { FALSE };					//是否已经开始采集



static BOOL m_bIsSensor[MAX_CAMERA_NUM] = { FALSE };                //是否是sensor模式 sensor模式这个暂时没有启用，如果后期有客户需要可以启动
static BOOL m_bIsCalibration[MAX_CAMERA_NUM] = { FALSE };           //是否是标定后数据
static BOOL m_bHasReflectance[MAX_CAMERA_NUM] = { FALSE };          //数据是否含灰度值


static unsigned long m_RunRate[MAX_CAMERA_NUM];                     //轮询取图时间间隔
//ranger3变量;
//std::string ctiFile = Sample::getPathToProducer();
//Sample::Consumer consumer(ctiFile);

//static Sample::InterfaceList interfaces;//接口列表
//static Sample::DeviceList devices;//相机设备列表
//static GenTL::TL_HANDLE tlHandle;//传输句柄
//static GenTLApi* tl;
//static Sample::DeviceId deviceId;//设备ID
//std::vector<std::pair<GenTL::IF_HANDLE, std::string>> link_id;//接口句柄及设备名称
//std::set<GenTL::IF_HANDLE> openInterfaces;//接口句柄
//std::vector<GenApi::CNodeMapRef> interfaceNodeVec;

bool b_image2D = false;

int64_t BufferSize[CAMERA_MAX_NUM];//缓存buffer大小
char* pBufferOfRange3[CAMERA_MAX_NUM];//缓存buffer

//全局函数声明
bool GetFeatureInfoFromXML(int iCameraIndex, const char *pchfeatureName, LST_FEATURE_INFO &stFeatureInfo);
bool IsFeatureTypeRight(int iCameraIndex, const char* pchfeatureName, LE_FEATURE_TYPE eType);
bool GetEnumInfoFromXML(int iCameraIndex, char* pfeatureName, LST_ENUM_INFO& stFeatureValue);
bool ConvertLusterEnumIntToCameraEnumString(int iCameraIndex, char* pfeatureName, int pFeatureValue, char** pchCameraEnumString);
int GetIndexFromHandle(void *pCameraHandle);
int GetAllNodeNum(XmlTreeNode* pNode);
int GetAllNodeInfo(XmlTreeNode* pNode, LST_NODE_INFO *pstFni, int &iIndex);
bool CreateNodeBuffer(int iCameraIndex, LST_IMAGE_INFO **pstImageInfo, LST_IMAGE_PROPERTY *pstImageProperty, LST_BUFFER_INFO *pstBufferInfo);
bool FreeNodeBuffer(LST_IMAGE_INFO *pstImageInfo, int iBufferNum);
bool LoadCameraXml(int iCameraIndex, const char *pXmlFileName);
bool ConvertLfncToVendor(int iCameraIndex, char *pXmlChar, char *pCameraChar);
//PvGenParameter *GetVendorParameter(void *pCameraHandle, char *pValueName);
bool PushImageToBuffer(LST_IMAGE_INFO *pstImageInfo, int iBufferNum, int iCameraIndex, int &iCurBufferIndex,
	char *pchCameraBuffer, int iCameraBufferSize, int iImageWidth, int iImageHeight, int iImagePixelFormat,
	int iImageCount, int iImageChannles, int iCameraStride, int iImageDepth, LST_SYSTEM_TIME *sysTime);
bool IsThisFrameMiss(int iFrameCount, int iCurFrameRate, int iDestFrameRate);
bool PopImageFromBuffer(LST_IMAGE_INFO *pstImageInfo, int iBufferNum, int iCameraIndex,
	LST_IMAGE_INFO **pstOutImageInfo, int &iCurBufferIndex);
DWORD WINAPI  ThreadReceiveImage(LPVOID lpParam);
DWORD WINAPI  ThreadStoreImage(LPVOID lpParam);
DWORD WINAPI  ThreadDisplayImage(LPVOID lpParam);
bool ConvertPixelFormatVendorToLfnc(char* pPixelFormat_EBUS, LST_IMAGE_PROPERTY& stImageProperty);
bool GetImagePropertyFromEnumInt(int iPixelFormat, LST_IMAGE_PROPERTY *stImageProperty);

LE_STATUS InitLusterBuffer(int iCameraIndex, LST_BUFFER_INFO *stBufferInfo);
LE_STATUS ReleaseLusterBuffer(int iCameraIndex);

unsigned long  _runRate(int iCameraIndex);

/**********************************************
//ranger3全局函数;
**********************************************/
//void setThreadPriority(GenApi::CNodeMapRef &dataStreamNodeMap);
//void getRangeBufferFromCamera(GenTLApi* tl,
//	GenTL::DS_HANDLE dataStreamHandle,
//	GenTL::BUFFER_HANDLE bufferHandle,
//	uint8_t* bufferData,
//	int64_t bufferSize);
//void getRangeAndReflectanceBufferFromCamera(int iCameraIndex, GenTLApi* tl,
//	GenTL::DS_HANDLE dataStreamHandle,
//	GenTL::BUFFER_HANDLE bufferHandle,
//	int iImageWidth,
//	int iImageHeight,
//	uint8_t* bufferData,
//	int64_t bufferSize);
void GetKeyValue(std::string line, std::string& key, std::string& value);
LE_STATUS ReadCSVFlie(void *pCameraHandle, std::ifstream &input);
void SaveBuffer(
	const float* xBuffer,
	const float* zBuffer,
	const uint8_t* iBuffer,
	const int64_t bufferWidth,
	const int64_t bufferHeight,
	const int64_t lowerBoundX,
	const int64_t lowerBoundR,
	const int64_t upperBoundX,
	const int64_t upperBoundR,
	const std::string& filePath);

/***********************************************************************
*
*				       组件SDK相机控制统一接口实现
*
***********************************************************************/

LE_STATUS __stdcall LCC_C_DetectCameraNum(LST_CAMERA_INFO **pstCameraInfo, int *piCameraNum)
{
	if ((pstCameraInfo == NULL) || (piCameraNum == NULL)) return L_ERR_CCPARV;	//无效的参数值
	*piCameraNum = 0;//全部相机List的总数量
	
	if (m_pRanger3Share == nullptr)
	{
		m_pRanger3Share = std::make_shared<R3S>("");
	}
	
	//检查“SICKGigEVisionTL.cti”文件是否存在
	if (!m_pRanger3Share->isCtiFound())
	{
		return L_ERR_CCWLPI;//无法加载插件，可能名称有误，或缺失依赖文件
	}

	//检索相机
	m_pRanger3Share->scanDevice();
	
	//取得连接相机列表
	auto devices = m_pRanger3Share->getConDevList();
		

	//清空结构体（所有）
	for (int i = 0; i < CAMERA_MAX_NUM; i++)
	{
		m_iCamIndex[i] = i;
		if (m_pastCameraInfo[i] != nullptr)
		{
			delete m_pastCameraInfo[i];
			m_pastCameraInfo[i] = nullptr;
		}

		if (m_paoRanger3s[i] != nullptr)
		{
			delete m_paoRanger3s[i];
			m_paoRanger3s[i] = nullptr;
		}
	}

	std::string firstName = "SICKGigEVisionTL_DEV_";
	std::string endName = "_Ranger3";
	int camIndex = 0;
	try
	{
		//检索已连接Ranger3相机设备并初始化相关实例
		for (auto device : devices)
		{
			std::string deviceName = device.first;
			if ((deviceName.find(firstName) == deviceName.npos) ||
				(deviceName.find(endName) == deviceName.npos))
			{
				continue;
			}

			//初始化采集类
			std::string camMac = device.second->getMac();
			camMac.erase(std::remove(camMac.begin(), camMac.end(), ':'), camMac.end());		

			//初始化相机信息结构
			m_pastCameraInfo[camIndex] = new LST_CAMERA_INFO();
			//设置相机索引
			m_pastCameraInfo[camIndex]->iCameraIndex = camIndex;

			std::string deviceid = std::to_string(device.second->mId);
			strcpy_s(m_pastCameraInfo[camIndex]->achCameraID, deviceid.c_str());
			//写入相机ID
			m_pastCameraInfo[camIndex]->achSdkVersion[0] = EMPTY_STRING;
			//写入相机接口：Ranger3相机为网口
			m_pastCameraInfo[camIndex]->iCameraInterface = CIT_GIGE;
			m_pastCameraInfo[camIndex]->achSerialNumber[0] = EMPTY_STRING;			
			//写入相机mac地址
			//std::string camMac = device.second->getMac();
			strcpy_s(m_pastCameraInfo[camIndex]->achReserved, camMac.c_str());
						
			//写入相机信息;序列号
			//device.second->mInterfaceNodeMap._GetNode("DeviceSerialNumber");
			/*std::string number = device.second->getMac();
			strcpy_s(m_pastCameraInfo[i]->achSerialNumber, number.c_str());*/
			pstCameraInfo[camIndex] = m_pastCameraInfo[camIndex];
			++camIndex;

			//判断输出相机数量
			if (camIndex > MAX_CAMERA_NUM)
			{
				*piCameraNum = -1;
				return L_ERR_CCTM;   //超过了采集设备类的最大数目
			}
		}

		*piCameraNum = camIndex;//相机数量
	}
	catch (const std::exception& e)
	{
		return L_ERR_UNKNOWN;
	}
	
	//全局变量赋值-相机数量
	m_iCameraNum = *piCameraNum;

	if (m_iCameraNum == 0)
	{
		return L_ERR_CCU;//未检测相机数量和信息
	}

	//已检测状态
	m_bDetected = TRUE;

	return L_OK;

}
#if 1

LE_STATUS __stdcall LCC_C_OpenCamera(int iCameraIndex, const char *pchXmlFileName, void **pCameraHandle)
{
	//参数合理性判断
	if (iCameraIndex < 0 || iCameraIndex >= CAMERA_MAX_NUM || iCameraIndex >= m_iCameraNum)
	{
		return L_ERR_CCFWIDX;	//无效的相机索引
	}
	
	if (!m_bDetected) return L_ERR_CCU;	//未检测相机数量和信息
	if (m_bOpened[iCameraIndex]) return L_ERR_CCOED;//相机已经打开

	//对参数控制节点清空
	if (m_poTreeNodeRoot[iCameraIndex] != NULL)
	{
		delete m_poTreeNodeRoot[iCameraIndex];
		m_poTreeNodeRoot[iCameraIndex] = NULL;
	}

	//导入xml相机参数配置文件
	if (!LoadCameraXml(iCameraIndex, pchXmlFileName))
	{
		return L_ERR_CCXML;	 //XML配置文件不能打开
	}

	m_paoRanger3s[iCameraIndex] = new SickCam::Ranger3(m_pRanger3Share, m_pastCameraInfo[iCameraIndex]->achReserved, false);

	//连接相机
	// check
	if (m_paoRanger3s[iCameraIndex]->getDeviceName() == "ERROR")
	{
		return L_ERR_UNKNOWN;
	}

	//  --- Set one Ranger3.
	//m_paoRanger3s[iCameraIndex]->setTimeOut(m_paoRanger3s[iCameraIndex]->getTimeOut());
	Ecode ec;
	try
	{
		ec = m_paoRanger3s[iCameraIndex]->connectCamera();

		if (ec != Ecode::All_OK)
		{
			return L_ERR_CCOPEN;
		}
		m_paoRanger3s[iCameraIndex]->setScaleY(0.02);//设置Y精度 	
		/*m_paoRanger3s[iCameraIndex]->setParametPath(para_csv_path);
		m_paoRanger3s[iCameraIndex]->setCalibraPath(calib_xml_path);*/
		//m_paoRanger3s[iCameraIndex]->setRectMethos(SiCaliWrapper::RectMethod::Top);
		m_paoRanger3s[iCameraIndex]->setDoRectify(false);
		//m_paoRanger3s[iCameraIndex]->setTimeOut(10);
		m_paoRanger3s[iCameraIndex]->setMissingData(0);
		//m_paoRanger3s[iCameraIndex]->setBufferCount(20);

		std::string hasReflectance = m_paoRanger3s[iCameraIndex]->getParameterValue("ComponentEnable_ComponentSelector_Reflectance_RegionSelector_Scan3dExtraction1");//判断是否含灰度值
		m_bHasReflectance[iCameraIndex] = std::stoi(hasReflectance);
		std::string isCalibration = m_paoRanger3s[iCameraIndex]->getCalibraPath();//判断是否是标定数据
		m_bIsCalibration[iCameraIndex] = isCalibration == "" ? false : true;
	}
	catch (const char * msg)
	{
		return L_ERR_CCOPEN;
	}
	catch (...)
	{
		return L_ERR_CCOPEN;
	}

	//启动采集线程
	//if (isOk && m_paoImageGrabers[iCameraIndex]->isImageGraberThreadRunning==false)
	//{
	//	//m_paoImageGrabers[iCameraIndex]->getR3()->setTimeOut(1000);

	//	std::thread* thread_pImgGabber = new std::thread(&ImageGabber::run, m_paoImageGrabers[iCameraIndex]);
	//	thread_pImgGabber->detach();
	//	delete thread_pImgGabber;
	//	thread_pImgGabber = nullptr;
	//}
	//		

	*pCameraHandle = &(m_iCamIndex[iCameraIndex]);//待测m_paoImageGrabers[iCameraIndex]->getR3()->getDeviceHandle();
	m_pCamHandle[iCameraIndex] = &(m_iCamIndex[iCameraIndex]);


	//已打开状态
	m_bOpened[iCameraIndex] = TRUE;
	m_iCameraNumOpen++;

	return L_OK;
}

LE_STATUS __stdcall LCC_C_InitCamera(void *pCameraHandle, LST_BUFFER_INFO stBufferInfo)
{
	//由相机句柄得到相机索引
	int iCameraIndex = GetIndexFromHandle(pCameraHandle);
	if (iCameraIndex < 0 || iCameraIndex >= CAMERA_MAX_NUM)
	{
		return L_ERR_CCWH;	//句柄无效
	}

	if (!m_bDetected) return L_ERR_CCU;	//未检测相机数量和信息
	if (!m_bOpened[iCameraIndex]) return L_ERR_CCNOED;	//相机未打开，没有连接图像采集设备
	if (m_bInited[iCameraIndex]) return L_ERR_CCIED;	//相机已经初始化

	/*******************************************
	初始化
	*******************************************/

	//首先清空操作，存储buffer，LST_IMAGE_INFO
	if (m_pastStoreImageInfo[iCameraIndex] != NULL)
	{
		FreeNodeBuffer(m_pastStoreImageInfo[iCameraIndex], stBufferInfo.iBufferNum);
		m_pastStoreImageInfo[iCameraIndex] = NULL;
	}

	//首先清空操作，显示buffer, LST_IMAGE_INFO
	if (m_pastDisplayImageInfo[iCameraIndex] != NULL)
	{
		FreeNodeBuffer(m_pastDisplayImageInfo[iCameraIndex], DISPLAY_MAX_BUFF);
		m_pastDisplayImageInfo[iCameraIndex] = NULL;
	}

	//首先清空操作，单帧buffer, LST_IMAGE_INFO
	if (m_pastSnapImageInfo[0] != NULL)
	{
		FreeNodeBuffer(m_pastSnapImageInfo[0], 1);//只要1个buffer
		m_pastSnapImageInfo[0] = NULL;
	}
	//首先清空操作，Ranger3XML文件信息
	if (m_pastRanger3XMLInfo[iCameraIndex] != NULL)
	{
		delete m_pastRanger3XMLInfo[iCameraIndex];
		m_pastRanger3XMLInfo[iCameraIndex] = NULL;
	}
	//首先清空操作，图像属性结构体
	if (m_pastImageProperty[iCameraIndex] != NULL)
	{
		delete m_pastImageProperty[iCameraIndex];
		m_pastImageProperty[iCameraIndex] = NULL;
	}

	//首先清空操作，缓存信息结构体
	if (m_pastBufferInfo[iCameraIndex] != NULL)
	{
		delete m_pastBufferInfo[iCameraIndex];
		m_pastBufferInfo[iCameraIndex] = NULL;
	}

	//线程句柄初始化
	m_ThreadPushHandle[iCameraIndex] = NULL;
	m_ThreadPopStoreHandle[iCameraIndex] = NULL;
	m_ThreadPopDisplayHandle[iCameraIndex] = NULL;

	//缓存索引初始化
	m_iPushStoreBufferIndex[iCameraIndex] = -1;
	m_iPopStoreBufferIndex[iCameraIndex] = -1;
	m_iPushDisplayBufferIndex[iCameraIndex] = -1;
	m_iPopDisplayBufferIndex[iCameraIndex] = -1;

	//CallBack指针初始化
	m_pStoreCallBack[iCameraIndex] = NULL;			//存储类型的回调指针
	m_pMessageCallBack[iCameraIndex] = NULL;		//消息类型的回调指针
	m_pDisplayCallBack[iCameraIndex] = NULL;		//显示类型的回调指针

	//CallBack用户上下文初始化
	m_pStoreUserContext[iCameraIndex] = NULL;		//存储类型的用户上下文
	m_pDisplayUserContext[iCameraIndex] = NULL;		//消息类型的用户上下文
	m_pMessageUserContext[iCameraIndex] = NULL;		//显示类型的用户上下文

	//其他初始化
	m_iThreadID[iCameraIndex] = -1;		//线程ID初始化
	m_iShowFps[iCameraIndex] = 15;		//显示帧频初始化
	m_dCurFps[iCameraIndex] = 0;		//当前帧频初始化
	m_bIsStopReceive[iCameraIndex] = true;			//状态变量，是否停止获取图像，ThreadReceiveImage
	m_bIsStopStore[iCameraIndex] = true;			//状态变量，是否停止存储图像，ThreadStoreImage
	m_bIsStopDisplay[iCameraIndex] = true;			//状态变量，是否停止显示图像，ThreadDisplayImage

	m_iSnapWaitTime[iCameraIndex] = 1000;           //单帧存图等待时间初始值1000

	/*******************************************
	读取图像属性
	*******************************************/
	LE_STATUS eResult;
	LST_BOOLEAN_INFO stBooleanInfo;

	////获取相机buffer模式，true：Range+Reflectance；false：Range
	//eResult = LCC_C_GetFeatureBoolean(m_paoConnection[iCameraIndex]->mDeviceHandle, LFNC_SICK_REFLECTANCE_ENABLE, &stBooleanInfo);
	//
	//存储buffer信息结构体
	m_pastBufferInfo[iCameraIndex] = new LST_BUFFER_INFO();
	m_pastBufferInfo[iCameraIndex]->iBufferNum = stBufferInfo.iBufferNum;
	if (stBufferInfo.iMergeFrameNum < 1 || stBufferInfo.iMergeFrameNum > 100000)	//判断超界
	{
		m_pastBufferInfo[iCameraIndex]->iMergeFrameNum = 1;
	}
	else
	{
		m_pastBufferInfo[iCameraIndex]->iMergeFrameNum = stBufferInfo.iMergeFrameNum;
	}
	strcpy_s(m_pastBufferInfo[iCameraIndex]->achReserved, stBufferInfo.achReserved);

	//初始化Buffer
	eResult = InitLusterBuffer(iCameraIndex, &stBufferInfo);

	//已初始化状态
	m_bInited[iCameraIndex] = TRUE;

	return L_OK;
}

LE_STATUS __stdcall LCC_C_CloseCamera(void *pCameraHandle)
{
	//由相机句柄得到相机索引
	int iCameraIndex = GetIndexFromHandle(pCameraHandle);
	if (iCameraIndex < 0 || iCameraIndex >= CAMERA_MAX_NUM)
	{
		return L_ERR_CCWH;	//句柄无效
	}

	if (!m_bOpened[iCameraIndex]) return L_ERR_CCNOED;	//相机未打开，没有连接图像采集设备
	if (m_bGrabbed[iCameraIndex]) return L_ERR_CCGED;	//相机已经采集

	//如果已初始化状态，则释放状态
	if (m_bInited[iCameraIndex])
	{
		LE_STATUS eStatus = LCC_C_ReleaseCamera(pCameraHandle);
		if (eStatus != L_OK) return eStatus;
	}

	//释放XmlTreeNode
	if (m_poTreeNodeRoot[iCameraIndex] != nullptr)
	{
		delete m_poTreeNodeRoot[iCameraIndex];
		m_poTreeNodeRoot[iCameraIndex] = nullptr;
	}

	//清空相机Handle
	if (m_pCamHandle[iCameraIndex] != nullptr)
	{
		m_pCamHandle[iCameraIndex] = nullptr;
	}


	//释放相机连接类
	if (m_paoRanger3s[iCameraIndex] != nullptr)
	{
		m_paoRanger3s[iCameraIndex]->disconnectCamera();
		delete m_paoRanger3s[iCameraIndex];
		m_paoRanger3s[iCameraIndex] = nullptr;
	}


	//未打开状态
	m_bOpened[iCameraIndex] = FALSE;

	/*if (m_bDetected == true)
	{
	if (iCameraIndex == m_iCameraNum - 1)
	{
	for (int i = 0; i < CAMERA_MAX_NUM; i++)
	{
	delete m_pastCameraInfo[i];
	m_pastCameraInfo[i] = NULL;
	}
	}
	}*/

	m_iCameraNumOpen--;

	return L_OK;
}

LE_STATUS __stdcall LCC_C_StartGrab(void *pCameraHandle)
{
	//由相机句柄得到相机索引
	int iCameraIndex = GetIndexFromHandle(pCameraHandle);
	if (iCameraIndex < 0 || iCameraIndex >= CAMERA_MAX_NUM)
	{
		return L_ERR_CCWH;	//句柄无效
	}

	if (!m_bDetected) return L_ERR_CCU;	//未检测相机数量和信息
	if (!m_bOpened[iCameraIndex]) return L_ERR_CCNOED;	//相机未打开，没有连接图像采集设备
	if (!m_bInited[iCameraIndex]) return L_ERR_CCNIED;	//相机未初始化
	if (m_bGrabbed[iCameraIndex]) return L_ERR_CCGED;	//相机已经采集

	//状态更新
	m_bIsStopReceive[iCameraIndex] = false;
	m_bIsStopStore[iCameraIndex] = false;
	m_bIsStopDisplay[iCameraIndex] = false;

	//开始采集
	if (m_paoRanger3s[iCameraIndex] != nullptr)
	{
		m_RunRate[iCameraIndex] = 5;// _runRate(iCameraIndex);
		Ecode ec = m_paoRanger3s[iCameraIndex]->startCamera();

		if (ec != Ecode::All_OK)
		{
			return L_ERR_UNKNOWN;
		}

	}
	//创建接受图像线程
	m_iThreadID[iCameraIndex] = iCameraIndex;
	m_ThreadPushHandle[iCameraIndex] = ::CreateThread(NULL, NULL, ThreadReceiveImage, &m_iThreadID[iCameraIndex], NULL, NULL);

	//创建抽取图像线程（存储缓存）
	if (m_pStoreCallBack[iCameraIndex] != NULL)
	{
		m_ThreadPopStoreHandle[iCameraIndex] = ::CreateThread(NULL, NULL, ThreadStoreImage, &m_iThreadID[iCameraIndex], NULL, NULL);
	}

	//创建抽取图像线程（显示缓存）
	if (m_pDisplayCallBack[iCameraIndex] != NULL)
	{
		m_ThreadPopDisplayHandle[iCameraIndex] = ::CreateThread(NULL, NULL, ThreadDisplayImage, &m_iThreadID[iCameraIndex], NULL, NULL);
	}

	//已采集状态
	m_bGrabbed[iCameraIndex] = TRUE;

	Sleep(100);//多相机采集时，两个相机采集功能实现时须有延时  待测

	return L_OK;
}

LE_STATUS __stdcall LCC_C_StopGrab(void *pCameraHandle)
{
	//由相机句柄得到相机索引
	int iCameraIndex = GetIndexFromHandle(pCameraHandle);
	if (iCameraIndex < 0 || iCameraIndex >= CAMERA_MAX_NUM)
	{
		return L_ERR_CCWH;	//句柄无效
	}

	if (!m_bGrabbed[iCameraIndex]) return L_ERR_CCNGED;	//相机未采集

	

	//更新状态
	m_bIsStopReceive[iCameraIndex] = true;
	m_bIsStopStore[iCameraIndex] = true;
	m_bIsStopDisplay[iCameraIndex] = true;

	//停止采集
	if (m_paoRanger3s[iCameraIndex] != nullptr)
	{
		Ecode ec = m_paoRanger3s[iCameraIndex]->stopCamera();
		if (ec = SickCam::All_OK)
		{
			return L_ERR_UNKNOWN;
		}
	}

	//缓存索引初始化
	m_iPushStoreBufferIndex[iCameraIndex] = -1;
	m_iPopStoreBufferIndex[iCameraIndex] = -1;
	m_iPushDisplayBufferIndex[iCameraIndex] = -1;
	m_iPopDisplayBufferIndex[iCameraIndex] = -1;

	//释放所有的Buffer的标识信号
	for (int i = 0; i < m_pastBufferInfo[iCameraIndex]->iBufferNum; i++)
	{
		m_pastStoreImageInfo[iCameraIndex][i].achReserved[INDEX_SEMAPHORE] = FALSE;
	}
	for (int i = 0; i < DISPLAY_MAX_BUFF; i++)
	{
		m_pastDisplayImageInfo[iCameraIndex][i].achReserved[INDEX_SEMAPHORE] = FALSE;
	}

	//未采集状态
	m_bGrabbed[iCameraIndex] = FALSE;

	return L_OK;
}

LE_STATUS __stdcall LCC_C_ReleaseCamera(void *pCameraHandle)
{
	//由相机句柄得到相机索引
	int iCameraIndex = GetIndexFromHandle(pCameraHandle);
	if (iCameraIndex < 0 || iCameraIndex >= CAMERA_MAX_NUM)
	{
		return L_ERR_CCWH;	//句柄无效
	}

	if (m_bGrabbed[iCameraIndex]) return L_ERR_CCGED;	//相机已经采集
	if (!m_bInited[iCameraIndex]) return L_ERR_CCNIED;	//相机未初始化

	//清空buffer
	LE_STATUS eResult = ReleaseLusterBuffer(iCameraIndex);

	//首先清空操作，缓存信息结构体
	if (m_pastBufferInfo[iCameraIndex] != NULL)
	{
		delete m_pastBufferInfo[iCameraIndex];
		m_pastBufferInfo[iCameraIndex] = NULL;
	}

	//线程句柄初始化
	m_ThreadPushHandle[iCameraIndex] = NULL;
	m_ThreadPopStoreHandle[iCameraIndex] = NULL;
	m_ThreadPopDisplayHandle[iCameraIndex] = NULL;

	//CallBack指针初始化
	m_pStoreCallBack[iCameraIndex] = NULL;			//存储类型的回调指针
	m_pMessageCallBack[iCameraIndex] = NULL;		//消息类型的回调指针
	m_pDisplayCallBack[iCameraIndex] = NULL;		//显示类型的回调指针

	//CallBack用户上下文初始化
	m_pStoreUserContext[iCameraIndex] = NULL;		//存储类型的用户上下文
	m_pDisplayUserContext[iCameraIndex] = NULL;		//消息类型的用户上下文
	m_pMessageUserContext[iCameraIndex] = NULL;		//显示类型的用户上下文

	//其他初始化
	m_iThreadID[iCameraIndex] = -1;		//线程ID初始化
	//m_iRecordMode[iCameraIndex] = 0;	//存储控制初始化
	m_iShowFps[iCameraIndex] = 15;		//显示帧频初始化
	m_dCurFps[iCameraIndex] = 0;		//当前帧频初始化
	m_bIsStopReceive[iCameraIndex] = true;			//状态变量，是否停止获取图像，ThreadReceiveImage
	m_bIsStopStore[iCameraIndex] = true;			//状态变量，是否停止存储图像，ThreadStoreImage
	m_bIsStopDisplay[iCameraIndex] = true;			//状态变量，是否停止显示图像，ThreadDisplayImage

	//未初始化状态
	m_bInited[iCameraIndex] = FALSE;

	return L_OK;
}

LE_STATUS __stdcall LCC_C_SnapImage(void *pCameraHandle)
{
	//由相机句柄得到相机索引
	int iCameraIndex = GetIndexFromHandle(pCameraHandle);
	if (iCameraIndex < 0 || iCameraIndex >= CAMERA_MAX_NUM)
	{
		return L_ERR_CCWH;	//句柄无效
	}

	if (!m_bDetected) return L_ERR_CCU;	//未检测相机数量和信息
	if (!m_bOpened[iCameraIndex]) return L_ERR_CCNOED;	//相机未打开，没有连接图像采集设备
	if (!m_bInited[iCameraIndex]) return L_ERR_CCNIED;	//相机未初始化
	if (m_bGrabbed[iCameraIndex]) return L_ERR_CCGED;	//相机已经采集

	//已采集状态
	m_bGrabbed[iCameraIndex] = TRUE;



	return L_OK;
}

LE_STATUS __stdcall LCC_C_RegisterCallback(LCC_C_Callback pCallback, int iCallbackType, void *pCameraHandle, void *pUserContext)
{
	//由相机句柄得到相机索引
	int iCameraIndex = GetIndexFromHandle(pCameraHandle);
	if (iCameraIndex < 0 || iCameraIndex >= CAMERA_MAX_NUM)
	{
		return L_ERR_CCWH;	//句柄无效
	}

	switch (iCallbackType)
	{
	case CB_STORE:
		m_pStoreCallBack[iCameraIndex] = pCallback;
		m_pStoreUserContext[iCameraIndex] = pUserContext;
		break;
	case CB_DISPLAY:
		m_pDisplayCallBack[iCameraIndex] = pCallback;
		m_pDisplayUserContext[iCameraIndex] = pUserContext;
		//m_iShowFps = iShowFps;//这个值建议并到相机参数设置里面去
		break;
	case CB_MESSAGE:
		m_pMessageCallBack[iCameraIndex] = pCallback;
		m_pMessageUserContext[iCameraIndex] = pUserContext;
		break;
	default:
		return L_ERR_CCCALLBACK;	//不支持的回调类型
	}
	return L_OK;
}

LE_STATUS __stdcall LCC_C_UnregisterCallback(int iCallbackType, void *pCameraHandle)
{
	//由相机句柄得到相机索引
	int iCameraIndex = GetIndexFromHandle(pCameraHandle);
	if (iCameraIndex < 0 || iCameraIndex >= CAMERA_MAX_NUM)
	{
		return L_ERR_CCWH;	//句柄无效
	}

	switch (iCallbackType)
	{
	case CB_STORE:
		m_pStoreCallBack[iCameraIndex] = nullptr;
		m_pStoreUserContext[iCameraIndex] = nullptr;
		break;
	case CB_DISPLAY:
		m_pDisplayCallBack[iCameraIndex] = nullptr;
		m_pDisplayUserContext[iCameraIndex] = nullptr;
		//m_iShowFps[iCameraIndex] = 15; //这个值建议并到相机参数设置里面去
		break;
	case CB_MESSAGE:
		m_pMessageCallBack[iCameraIndex] = nullptr;
		m_pMessageUserContext[iCameraIndex] = nullptr;
		break;
	default:
		return L_ERR_CCCALLBACK;	//不支持的回调类型
	}
	return L_OK;
}

LE_STATUS __stdcall LCC_C_GetFeatureInfo(void *pCameraHandle, const char *pchfeatureName, LST_FEATURE_INFO *stFeatureInfo)
{
	//由相机句柄得到相机索引
	int iCameraIndex = GetIndexFromHandle(pCameraHandle);
	if (iCameraIndex < 0 || iCameraIndex >= CAMERA_MAX_NUM)
	{
		return L_ERR_CCWH;	//句柄无效
	}

	//从XML相机参数管理文档中，读取相机参数信息结构体
	if (!GetFeatureInfoFromXML(iCameraIndex, pchfeatureName, *stFeatureInfo))
	{
		stFeatureInfo->eType = FEATURE_TYPE_UNKNOW;
		return L_ERR_CCPARAM;		//不支持的参数
	}

	return L_OK;
}

LE_STATUS __stdcall LCC_C_GetFeatureInt(void *pCameraHandle, const char *pchfeatureName, LST_INT_INFO *stFeatureValue)
{
	//由相机句柄得到相机索引
	int iCameraIndex = GetIndexFromHandle(pCameraHandle);
	if (iCameraIndex < 0 || iCameraIndex >= CAMERA_MAX_NUM)
	{
		return L_ERR_CCWH;	//句柄无效
	}

	//if (pchfeatureName == LFNC_SICK_BUFFERCOUNT)
	//{
	//	stFeatureValue->iValue = m_paoRanger3s[iCameraIndex]->getBufferCount();
	//	return L_OK;
	//}

	//判断参数类型是否正确
	if (!IsFeatureTypeRight(iCameraIndex, pchfeatureName, TYPE_INT))
	{
		return L_ERR_CCPART;	//无效的参数类型
	}

	//输入的凌云相机参数名称（LFNC）
	char pchfeatureName_Lfnc[255];
	strcpy_s(pchfeatureName_Lfnc, pchfeatureName);

	//待转换的相机厂商的参数名称（EBUS）
	char achfeatureName_Vendor[255];
	memset(achfeatureName_Vendor, 0x00, 255);

	//转换名称，由LFNC转换为EBUS
	if (!ConvertLfncToVendor(iCameraIndex, pchfeatureName_Lfnc, achfeatureName_Vendor))
	{
		return L_ERR_CCRXFVF;//读取XML参数值失败
	}

	//获得参数值
	std::string value = m_paoRanger3s[iCameraIndex]->getParameterValue(achfeatureName_Vendor);

	stFeatureValue->iValue = std::stoi(value);

	std::string minValue, maxValue, incValue;
	if (m_paoRanger3s[iCameraIndex]->getParameterValueConstraint(achfeatureName_Vendor,
		minValue, maxValue, incValue))
	{
		//获得参数最大值
		stFeatureValue->iMax = std::stoi(minValue);

		//获得参数最小值
		stFeatureValue->iMin = std::stoi(maxValue);

		//获得参数递增值
		stFeatureValue->iInc = std::stoi(incValue);
	}


	return L_OK;
}

LE_STATUS __stdcall LCC_C_GetFeatureBoolean(void *pCameraHandle, const char *pchfeatureName, LST_BOOLEAN_INFO *stFeatureValue)
{
	//由相机句柄得到相机索引
	int iCameraIndex = GetIndexFromHandle(pCameraHandle);
	if (iCameraIndex < 0 || iCameraIndex >= CAMERA_MAX_NUM)
	{
		return L_ERR_CCWH;	//句柄无效
	}

	return L_ERR_CCPARNA;

	/*if (pchfeatureName == LFNC_SICK_DORECTIFY)
	{
		stFeatureValue->bValue = m_paoRanger3s[iCameraIndex]->getDoRectify();
		return L_OK;
	}*/

	//判断参数类型是否正确
	if (!IsFeatureTypeRight(iCameraIndex, pchfeatureName, TYPE_BOOLEAN))
	{
		return L_ERR_CCPART;  //无效的参数类型
	}

	//输入的凌云相机参数名称（LFNC）
	char pchfeatureName_Lfnc[255];
	strcpy_s(pchfeatureName_Lfnc, pchfeatureName);

	//待转换的相机厂商的参数名称（EBUS）
	char achfeatureName_Vendor[255];
	memset(achfeatureName_Vendor, 0x00, 255);

	//转换名称，由LFNC转换为EBUS
	if (!ConvertLfncToVendor(iCameraIndex, pchfeatureName_Lfnc, achfeatureName_Vendor))
	{
		return L_ERR_CCRXFVF;//读取XML参数值失败
	}

	std::string value = m_paoRanger3s[iCameraIndex]->getParameterValue(achfeatureName_Vendor);

	stFeatureValue->bValue = std::stoi(value);

#if 0
	//存图模式
	if (strcmp(pchfeatureName, LFNC_SICK_REFLECTANCE_ENABLE) == 0)
	{
		GenApi::CNodeMapRef device = m_paoConnection[iCameraIndex]->mDeviceNodeMap;
		GenApi::CEnumerationPtr regionSelector = device._GetNode("RegionSelector");
		*regionSelector = "Scan3dExtraction1";
		GenApi::CEnumerationPtr componentSelector = device._GetNode("ComponentSelector");
		*componentSelector = "Reflectance";
		GenApi::CBooleanPtr reflectanceEnable = device._GetNode("ComponentEnable");
		//*reflectanceEnable = true;

		stFeatureValue->bValue = reflectanceEnable->GetValue();

	}

	//ReflectanceFilter
	else if (strcmp(pchfeatureName, LFNC_SICK_REFLECTANCE_FILTER) == 0)
	{
		LE_STATUS eResult = LCC_C_SetFeatureEnum(pCameraHandle, LFNC_SICK_REGION_SELECTOR, 2);
		GenApi::CBooleanPtr featureName = m_paoConnection[iCameraIndex]->mDeviceNodeMap._GetNode("ReflectanceFilter");

		stFeatureValue->bValue = featureName->GetValue();
	}
#endif // 0

	return L_OK;
}

LE_STATUS __stdcall LCC_C_GetFeatureDouble(void *pCameraHandle, const char *pchfeatureName, LST_DOUBLE_INFO *stFeatureValue)
{
	//由相机句柄得到相机索引
	int iCameraIndex = GetIndexFromHandle(pCameraHandle);
	if (iCameraIndex < 0 || iCameraIndex >= CAMERA_MAX_NUM)
	{
		return L_ERR_UNKNOWN;
	}

	/*if (pchfeatureName == LFNC_SICK_MISSINGDATA)
	{
		stFeatureValue->dValue = m_paoRanger3s[iCameraIndex]->getMissingData();
		return L_OK;
	}

	if (pchfeatureName == LFNC_SICK_SCALEY)
	{
		stFeatureValue->dValue = m_paoRanger3s[iCameraIndex]->getScaleY();
		return L_OK;
	}*/

	if (strcmp(pchfeatureName, LFNC_SICK_SCALEY) == 0)
	{
		stFeatureValue->dValue = m_paoRanger3s[iCameraIndex]->getScaleY();
		return L_OK;
	}

	return L_ERR_CCPARNA;
	//判断参数类型是否正确
	if (!IsFeatureTypeRight(iCameraIndex, pchfeatureName, TYPE_DOUBLE))
	{
		return L_ERR_CCPART;	//无效的参数类型
	}

	//输入的凌云相机参数名称（LFNC）
	char pchfeatureName_Lfnc[255];
	strcpy_s(pchfeatureName_Lfnc, pchfeatureName);

	//待转换的相机厂商的参数名称（EBUS）
	char achfeatureName_Vendor[255];
	memset(achfeatureName_Vendor, 0x00, 255);

	//转换名称，由LFNC转换为EBUS
	if (!ConvertLfncToVendor(iCameraIndex, pchfeatureName_Lfnc, achfeatureName_Vendor))
	{
		return L_ERR_UNKNOWN;
	}

	//获得参数值
	std::string value = m_paoRanger3s[iCameraIndex]->getParameterValue(achfeatureName_Vendor);

	stFeatureValue->dValue = std::stod(value);

	std::string minV, maxV, incV;
	if (m_paoRanger3s[iCameraIndex]->getParameterValueConstraint(achfeatureName_Vendor,
		minV,
		maxV,
		incV))
	{
		//获得参数最大值
		stFeatureValue->dMax = std::stod(minV);

		//获得参数最小值
		stFeatureValue->dMin = std::stod(maxV);
	}

	return L_OK;
}

LE_STATUS __stdcall LCC_C_GetFeatureString(void *pCameraHandle, const char *pchfeatureName, LST_STRING_INFO *stFeatureValue)
{
	//由相机句柄得到相机索引
	int iCameraIndex = GetIndexFromHandle(pCameraHandle);
	if (iCameraIndex < 0 || iCameraIndex >= CAMERA_MAX_NUM)
	{
		return L_ERR_CCWH;	//句柄无效
	}
	//初始化采集类
	
	if (strcmp(pchfeatureName, LFNC_SICK_CAMMAC) == 0)
	{
		std::string camMac = m_paoRanger3s[iCameraIndex]->getMac();
		if (!camMac.empty())
			camMac.erase(std::remove(camMac.begin(), camMac.end(), ':'), camMac.end());

		strcpy(stFeatureValue->achValue, camMac.c_str());
		stFeatureValue->iLength = camMac.length();
	}
	if (strcmp(pchfeatureName, LFNC_SICK_DEVICENAME) == 0)
	{
		std::string deviceName= m_paoRanger3s[iCameraIndex]->getDeviceName();
		if (!deviceName.empty())
		{
			strcpy(stFeatureValue->achValue, deviceName.c_str());
			stFeatureValue->iLength = deviceName.length();
		}
	}
	return L_ERR_CCPARNA;

	//判断参数类型是否正确
	if (!IsFeatureTypeRight(iCameraIndex, pchfeatureName, TYPE_STRING))
	{
		return L_ERR_CCPART;  //无效的参数类型
	}

	//if (pchfeatureName == LFNC_SICK_MAC)
	//{
	//	strcpy_s(stFeatureValue->achValue, m_paoRanger3s[iCameraIndex]->getMac().c_str());
	//	return L_OK;
	//}
	//else if (pchfeatureName == LFNC_SICK_PERSISITENTIP)
	//{
	//	strcpy_s(stFeatureValue->achValue, m_paoRanger3s[iCameraIndex]->getIp(true).c_str());
	//	return L_OK;
	//}
	//else if (pchfeatureName == LFNC_SICK_CURRENTIP)
	//{
	//	strcpy_s(stFeatureValue->achValue, m_paoRanger3s[iCameraIndex]->getIp().c_str());
	//	return L_OK;
	//}
	//else if (pchfeatureName == LFNC_SICK_PERSISITENTSUBNETMASK)
	//{
	//	strcpy_s(stFeatureValue->achValue, m_paoRanger3s[iCameraIndex]->getSubNet(true).c_str());
	//	return L_OK;
	//}
	//else if (pchfeatureName == LFNC_SICK_CURRENTSUBNETMASK)
	//{
	//	strcpy_s(stFeatureValue->achValue, m_paoRanger3s[iCameraIndex]->getSubNet().c_str());
	//	return L_OK;
	//}
	//else if (pchfeatureName == LFNC_SICK_PARAMETPATH)
	//{
	//	strcpy_s(stFeatureValue->achValue, m_paoRanger3s[iCameraIndex]->getParametPath().c_str());
	//	return L_OK;
	//}
	//else if (pchfeatureName == LFNC_SICK_CALIBRAPATH)
	//{
	//	strcpy_s(stFeatureValue->achValue, m_paoRanger3s[iCameraIndex]->getCalibraPath().c_str());
	//	return L_OK;
	//}

	return L_ERR_CCPART;
}

LE_STATUS __stdcall LCC_C_GetFeatureEnum(void *pCameraHandle, const char *pchfeatureName, LST_ENUM_INFO *stFeatureValue)
{
	//由相机句柄得到相机索引
	int iCameraIndex = GetIndexFromHandle(pCameraHandle);
	if (iCameraIndex < 0 || iCameraIndex >= CAMERA_MAX_NUM)
	{
		return L_ERR_CCWH;	//句柄无效
	}

	return L_ERR_CCPARNA;
#if 0
	//触发模式获取
	if (strcmp(pchfeatureName, LFNC_TRIG_MODE) == 0)
	{
		GenApi::CEnumerationPtr triggerMode = m_paoConnection[iCameraIndex]->mDeviceNodeMap._GetNode("TriggerMode");

		int trigMode = triggerMode->GetIntValue();

		if (trigMode == 1)
		{
			stFeatureValue->iValue = 1;//on
		}
		else
		{
			stFeatureValue->iValue = 0;//off
		}
	}

	//窗口大小
	else if (strcmp(pchfeatureName, LFNC_SICK_WAM_SIZE) == 0)
	{
		GenApi::CNodeMapRef device = m_paoConnection[iCameraIndex]->mDeviceNodeMap;
		GenApi::CEnumerationPtr wamsizeselector = device._GetNode("WamSize");
		int wamsize = wamsizeselector->GetIntValue();

		if (wamsize == 15)
		{
			stFeatureValue->iValue = 0;//Normal
		}
		else if (wamsize == 7)
		{
			stFeatureValue->iValue = 1;//Small
		}
		else if (wamsize == 31)
		{
			stFeatureValue->iValue = 2;//Large
		}
	}

	//SearchMode3D
	else if (strcmp(pchfeatureName, LFNC_SICK_SEARCH_MODE_3D) == 0)
	{
		GenApi::CEnumerationPtr searchMode3D = m_paoConnection[iCameraIndex]->mDeviceNodeMap._GetNode("SearchMode3D");
		int SearchMode = searchMode3D->GetIntValue();

		if (SearchMode == 1)
		{
			stFeatureValue->iValue = 1;
		}
		else
		{
			stFeatureValue->iValue = 0;
		}
	}

	//RangeAxis
	else if (strcmp(pchfeatureName, LFNC_SICK_RANGE_AXIS) == 0)
	{
		GenApi::CEnumerationPtr rangeAxis = m_paoConnection[iCameraIndex]->mDeviceNodeMap._GetNode("RangeAxis");
		int RangeAxis = rangeAxis->GetIntValue();

		if (RangeAxis == 1)
		{
			stFeatureValue->iValue = 1;
		}
		else
		{
			stFeatureValue->iValue = 0;
		}
	}

	//SearchDirection
	else if (strcmp(pchfeatureName, LFNC_SICK_SEARCH_DIRECTION) == 0)
	{
		GenApi::CEnumerationPtr searchDirection = m_paoConnection[iCameraIndex]->mDeviceNodeMap._GetNode("SearchDirection");
		int SearchDirection = searchDirection->GetIntValue();

		if (SearchDirection == 1)
		{
			stFeatureValue->iValue = 1;
		}
		else
		{
			stFeatureValue->iValue = 0;
		}
	}

	//PixelFormat
	else if (strcmp(pchfeatureName, LFNC_PIXEL_FORMAT) == 0)
	{
		LE_STATUS eResult = LCC_C_SetFeatureEnum(pCameraHandle, LFNC_SICK_REGION_SELECTOR, 2);
		GenApi::CNodeMapRef device = m_paoConnection[iCameraIndex]->mDeviceNodeMap;
		GenApi::CEnumerationPtr componentSelector = device._GetNode("ComponentSelector");
		*componentSelector = "Range";
		GenApi::CEnumerationPtr pixelFormat = device._GetNode("PixelFormat");
		int PixelFormat = pixelFormat->GetIntValue();

		if (PixelFormat == 17563719)
		{
			stFeatureValue->iValue = 0;
		}
		else if (PixelFormat == 17563866)
		{
			stFeatureValue->iValue = 1;
		}
		else if (PixelFormat == 17825799)
		{
			stFeatureValue->iValue = 2;
		}
		else if (PixelFormat == 17825976)
		{
			stFeatureValue->iValue = 3;
		}
	}

	//TriggerSelector
	else if (strcmp(pchfeatureName, LFNC_SICK_TRIGGER_MODE_SELECTOR) == 0)
	{
		GenApi::CEnumerationPtr triggerSelector = m_paoConnection[iCameraIndex]->mDeviceNodeMap._GetNode("TriggerSelector");
		int TriggerSelector = triggerSelector->GetIntValue();

		if (TriggerSelector == 1)
		{
			stFeatureValue->iValue = 1;
		}
		else
		{
			stFeatureValue->iValue = 0;
		}
	}

	//TriggerActivation
	else if (strcmp(pchfeatureName, LFNC_SICK_TRIGGER_ACTIVATION) == 0)
	{
		GenApi::CEnumerationPtr triggerActivation = m_paoConnection[iCameraIndex]->mDeviceNodeMap._GetNode("TriggerActivation");
		int TriggerActivation = triggerActivation->GetIntValue();

		if (TriggerActivation == 1)
		{
			stFeatureValue->iValue = 1;
		}
		else if (TriggerActivation == 0)
		{
			stFeatureValue->iValue = 0;
		}
	}

	//TestPattern
	else if (strcmp(pchfeatureName, LFNC_SICK_TEST_PATTERN) == 0)
	{
		GenApi::CEnumerationPtr testPattern = m_paoConnection[iCameraIndex]->mDeviceNodeMap._GetNode("TestPattern");
		int TestPattern = testPattern->GetIntValue();

		if (TestPattern == 1)
		{
			stFeatureValue->iValue = 1;
		}
		else if (TestPattern == 0)
		{
			stFeatureValue->iValue = 0;
		}
	}

	//MultiSlopeMode
	else if (strcmp(pchfeatureName, LFNC_SICK_MULTISLOPE_MODE) == 0)
	{
		GenApi::CEnumerationPtr multiSlopeMode = m_paoConnection[iCameraIndex]->mDeviceNodeMap._GetNode("MultiSlopeMode");
		int MultiSlopeMode = multiSlopeMode->GetIntValue();

		if (MultiSlopeMode == 0)
		{
			stFeatureValue->iValue = 0;
		}
		else if (MultiSlopeMode == 1)
		{
			stFeatureValue->iValue = 1;
		}
		else if (MultiSlopeMode == 2)
		{
			stFeatureValue->iValue = 2;
		}
		else if (MultiSlopeMode == 3)
		{
			stFeatureValue->iValue = 3;
		}
	}

	//EventNotification
	else if (strcmp(pchfeatureName, LFNC_SICK_EVENT_NOTIFICATION) == 0)
	{
		GenApi::CEnumerationPtr eventNotification = m_paoConnection[iCameraIndex]->mDeviceNodeMap._GetNode("EventNotification");
		int EventNotification = eventNotification->GetIntValue();

		if (EventNotification == 0)
		{
			stFeatureValue->iValue = 0;
		}
		else if (EventNotification == 1)
		{
			stFeatureValue->iValue = 1;
		}
	}

	//DeviceScanType
	else if (strcmp(pchfeatureName, LFNC_SICK_DEVICE_SCAN_TYPE) == 0)
	{
		GenApi::CEnumerationPtr deviceScanType = m_paoConnection[iCameraIndex]->mDeviceNodeMap._GetNode("DeviceScanType");
		int DeviceScanType = deviceScanType->GetIntValue();

		if (DeviceScanType == 0)
		{
			stFeatureValue->iValue = 0;
		}
		else if (DeviceScanType == 1)
		{
			stFeatureValue->iValue = 1;
		}
	}

	//AcquisitionMode
	else if (strcmp(pchfeatureName, LFNC_SICK_ACQUISITION_MODE) == 0)
	{
		GenApi::CEnumerationPtr acquisitionMode = m_paoConnection[iCameraIndex]->mDeviceNodeMap._GetNode("AcquisitionMode");
		int AcquisitionMode = acquisitionMode->GetIntValue();

		if (AcquisitionMode == 0)
		{
			stFeatureValue->iValue = 0;
		}
		else if (AcquisitionMode == 1)
		{
			stFeatureValue->iValue = 1;
		}
	}

	//LineSelector
	else if (strcmp(pchfeatureName, LFNC_SICK_LINE_SELECTOR) == 0)
	{
		GenApi::CEnumerationPtr lineSelector = m_paoConnection[iCameraIndex]->mDeviceNodeMap._GetNode("LineSelector");
		int LineSelector = lineSelector->GetIntValue();

		if (LineSelector == 0)
		{
			stFeatureValue->iValue = 0;
		}
		else if (LineSelector == 1)
		{
			stFeatureValue->iValue = 1;
		}
		else if (LineSelector == 2)
		{
			stFeatureValue->iValue = 2;
		}
	}
#endif // 0
	//判断参数类型是否正确
	if (!IsFeatureTypeRight(iCameraIndex, pchfeatureName, TYPE_ENUM))
	{
		return L_ERR_CCPART;	//无效的参数类型
	}

	//输入的凌云相机参数名称（LFNC）
	char pchfeatureName_Lfnc[255];
	strcpy_s(pchfeatureName_Lfnc, pchfeatureName);

	//待转换的相机厂商的参数名称（EBUS）
	char achfeatureName_Vendor[255];
	memset(achfeatureName_Vendor, 0x00, 255);

	//转换名称，由LFNC转换为EBUS
	if (!ConvertLfncToVendor(iCameraIndex, pchfeatureName_Lfnc, achfeatureName_Vendor))
	{
		return L_ERR_UNKNOWN;
	}

	//获得参数值
	std::string value = m_paoRanger3s[iCameraIndex]->getParameterValue(achfeatureName_Vendor);

	stFeatureValue->iValue = std::stoi(value);

	return L_OK;
}

LE_STATUS __stdcall LCC_C_GetFeatureUncertain(void *pCameraHandle, const char *pchfeatureName, void **pFeatureValue)
{
	//由相机句柄得到相机索引
	int iCameraIndex = GetIndexFromHandle(pCameraHandle);
	if (iCameraIndex < 0 || iCameraIndex >= CAMERA_MAX_NUM)
	{
		return L_ERR_CCWH;	//句柄无效
	}

	return L_ERR_CCPARNA;

	//判断名称
	if (strcmp(pchfeatureName, LFNC_GET_STORE_IMAGE) == 0)//获取缓存中的一帧图像
	{
		LST_IMAGE_INFO **stImageInfo = (LST_IMAGE_INFO**)pFeatureValue;

		int iLoopCount = m_iSnapWaitTime[iCameraIndex] / 10;
		for (int i = 0; i < iLoopCount; i++)
		{
			if (m_pastStoreImageInfo[iCameraIndex] == NULL)
			{
				return L_ERR_CCCL;	//实例无效（或者已经注销）
			}

			if (m_pastStoreImageInfo[iCameraIndex][m_iPushStoreBufferIndex[iCameraIndex]].achReserved[INDEX_SEMAPHORE] == TRUE)
			{
				*stImageInfo = &(m_pastStoreImageInfo[iCameraIndex][m_iPushStoreBufferIndex[iCameraIndex]]);
				return L_OK;
			}
			Sleep(10);
		}
		return L_ERR_CCTIMEOUT;  //超时
	}
	else if (strcmp(pchfeatureName, LFNC_GET_NODE_NUM) == 0)//相机参数总数
	{
		int **pNum = (int**)(pFeatureValue);
		int iFeatureNum = 0;

		//获取相机参数数量
		iFeatureNum = GetAllNodeNum(m_poTreeNodeRoot[iCameraIndex]);

		//赋值
		**pNum = iFeatureNum;
	}
	else if (strcmp(pchfeatureName, LFNC_GET_NODE_INFO) == 0)
	{
		//定义
		LST_NODE_INFO **pastFni = (LST_NODE_INFO **)pFeatureValue;
		//LST_NODE_INFO ptempNoInfo;
		int iFeatureIndex = 0;		//全局相机参数索引，从0开始

		//获取所有节点信息
		GetAllNodeInfo(m_poTreeNodeRoot[iCameraIndex], *pastFni, iFeatureIndex);
	}
	else if (strcmp(pchfeatureName, LFNC_GET_IMAGE_PROPERTY) == 0)
	{
		LST_IMAGE_PROPERTY**pastPrp = (LST_IMAGE_PROPERTY**)pFeatureValue;
		if (m_pastImageProperty[iCameraIndex] == NULL)
		{
			return L_ERR_CCCL;	//实例无效（或者已经注销）
		}
		*pastPrp = m_pastImageProperty[iCameraIndex];
	}
	else
	{
		return L_ERR_CCPARAM;//该参数暂不支持
	}
	return L_OK;
}

LE_STATUS __stdcall LCC_C_SetFeatureInt(void *pCameraHandle, const char *pchfeatureName, int iFeatureValue)
{
	//由相机句柄得到相机索引
	int iCameraIndex = GetIndexFromHandle(pCameraHandle);
	if (iCameraIndex < 0 || iCameraIndex >= CAMERA_MAX_NUM)
	{
		return L_ERR_CCWH;	//句柄无效
	}



	SickCam::CAM_STATUS status = SickCam::All_OK;

	//判断参数类型是否正确
	if (!IsFeatureTypeRight(iCameraIndex, pchfeatureName, TYPE_INT))
	{
		return L_ERR_CCPART;	//无效的参数类型
	}

	//输入的凌云相机参数名称（LFNC）
	char pchfeatureName_Lfnc[255];
	strcpy_s(pchfeatureName_Lfnc, pchfeatureName);

	//待转换的相机厂商的参数名称（EBUS）
	char achfeatureName_Vendor[255];
	memset(achfeatureName_Vendor, 0x00, 255);

	//转换名称，由LFNC转换为EBUS
	if (!ConvertLfncToVendor(iCameraIndex, pchfeatureName_Lfnc, achfeatureName_Vendor))
	{
		return L_ERR_UNKNOWN;
	}

	/*if (pchfeatureName == LFNC_SICK_TIMEOUT)
	{
		status = m_paoRanger3s[iCameraIndex]->setTimeOut(iFeatureValue);
		if (status == SickCam::All_OK)
		{
			return L_OK;
		}
		else
		{
			return L_ERR_UNKNOWN;
		}
	}
	else if (pchfeatureName == LFNC_SICK_BUFFERCOUNT)
	{
		status = m_paoRanger3s[iCameraIndex]->setBufferCount(iFeatureValue);
		if (status == SickCam::All_OK)
		{
			return L_OK;
		}
		else
		{
			return L_ERR_UNKNOWN;
		}
	}*/

	std::string minV, maxV, incV;
	status = m_paoRanger3s[iCameraIndex]->getParameterValueConstraint(achfeatureName_Vendor, minV, maxV, incV);
	if (status == SickCam::All_OK && (iFeatureValue<std::stoi(minV) || iFeatureValue>std::stoi(maxV)))
	{
		return L_ERR_CCPARV;
	}

	if (strcmp(pchfeatureName, LFNC_HEIGHT) == 0)
	{
		status = m_paoRanger3s[iCameraIndex]->setParameterValue(achfeatureName_Vendor, std::to_string(iFeatureValue));

		if (status == SickCam::All_OK)
		{
			m_pastImageProperty[iCameraIndex]->iImageHeight = iFeatureValue;
			LE_STATUS eResult = ReleaseLusterBuffer(iCameraIndex);//释放buffer
			if (eResult != L_OK)
			{
				return L_ERR_CCNRB;	//无法初始化缓存
			}
			eResult = InitLusterBuffer(iCameraIndex, m_pastBufferInfo[iCameraIndex]);//初始化buffer
			if (eResult != L_OK)
			{
				return L_ERR_CCNIB;	//无法初始化缓存
			}
			return L_OK;
		}
		else
		{
			return L_ERR_UNKNOWN;
		}
	}

	if (strcmp(pchfeatureName, LFNC_WIDTH) == 0)
	{
		status = m_paoRanger3s[iCameraIndex]->setParameterValue(achfeatureName_Vendor, std::to_string(iFeatureValue));

		if (status == SickCam::All_OK)
		{
			m_pastImageProperty[iCameraIndex]->iImageWidth = iFeatureValue;
			LE_STATUS eResult = ReleaseLusterBuffer(iCameraIndex);//释放buffer
			if (eResult != L_OK)
			{
				return L_ERR_CCNRB;	//无法初始化缓存
			}
			eResult = InitLusterBuffer(iCameraIndex, m_pastBufferInfo[iCameraIndex]);//初始化buffer
			if (eResult != L_OK)
			{
				return L_ERR_CCNIB;	//无法初始化缓存
			}
			return L_OK;
		}
		else
		{
			return L_ERR_UNKNOWN;
		}
	}
	
	return L_ERR_CCPARNA;
}

LE_STATUS __stdcall LCC_C_SetFeatureBoolean(void *pCameraHandle, const char *pchfeatureName, bool bFeatureValue)
{
	//由相机句柄得到相机索引
	int iCameraIndex = GetIndexFromHandle(pCameraHandle);
	if (iCameraIndex < 0 || iCameraIndex >= CAMERA_MAX_NUM)
	{
		return L_ERR_CCWH;	//句柄无效
	}

	return L_ERR_CCPARNA;
	/*if (pchfeatureName == LFNC_SICK_DORECTIFY)
	{
		SickCam::CAM_STATUS status = m_paoRanger3s[iCameraIndex]->setDoRectify(bFeatureValue);
		if (status == SickCam::All_OK)
		{
			return L_OK;
		}
		else
		{
			return L_ERR_UNKNOWN;
		}
	}
*/
	//判断参数类型是否正确
	//if (!IsFeatureTypeRight(iCameraIndex, pchfeatureName, TYPE_BOOLEAN))
	//{
	//	return L_ERR_CCPART;	//无效的参数类型
	//}


	//判断参数类型是否正确
	if (!IsFeatureTypeRight(iCameraIndex, pchfeatureName, TYPE_BOOLEAN))
	{
		return L_ERR_CCPART;	//无效的参数类型
	}

	//输入的凌云相机参数名称（LFNC）
	char pchfeatureName_Lfnc[255];
	strcpy_s(pchfeatureName_Lfnc, pchfeatureName);

	//待转换的相机厂商的参数名称（EBUS）
	char achfeatureName_Vendor[255];
	memset(achfeatureName_Vendor, 0x00, 255);

	//转换名称，由LFNC转换为EBUS
	if (!ConvertLfncToVendor(iCameraIndex, pchfeatureName_Lfnc, achfeatureName_Vendor))
	{
		return L_ERR_UNKNOWN;
	}

	SickCam::CAM_STATUS status = m_paoRanger3s[iCameraIndex]->setParameterValue(achfeatureName_Vendor, std::to_string(bFeatureValue));

	if (status == SickCam::All_OK)
	{
		return L_OK;
	}
	else
	{
		return L_ERR_UNKNOWN;
	}
}

LE_STATUS __stdcall LCC_C_SetFeatureDouble(void *pCameraHandle, const char *pchfeatureName, double dFeatureValue)
{
	//由相机句柄得到相机索引
	int iCameraIndex = GetIndexFromHandle(pCameraHandle);
	if (iCameraIndex < 0 || iCameraIndex >= CAMERA_MAX_NUM)
	{
		return L_ERR_CCWH;	//句柄无效
	}

	SickCam::CAM_STATUS status;
	/*if (pchfeatureName == LFNC_SICK_MISSINGDATA)
	{
		status = m_paoRanger3s[iCameraIndex]->setMissingData(dFeatureValue);
		if (status == SickCam::All_OK)
		{
			return L_OK;
		}
		else
		{
			return L_ERR_UNKNOWN;
		}
	}*/

	if (strcmp(pchfeatureName,LFNC_SICK_SCALEY)==0)
	{
		status = m_paoRanger3s[iCameraIndex]->setScaleY(dFeatureValue);
		if (status == SickCam::All_OK)
		{
			return L_OK;
		}
		else
		{
			return L_ERR_UNKNOWN;
		}
	}

	return L_ERR_CCPARNA;


	//判断参数类型是否正确
	if (!IsFeatureTypeRight(iCameraIndex, pchfeatureName, TYPE_DOUBLE))
	{
		return L_ERR_CCPART;	//无效的参数类型
	}

	//输入的凌云相机参数名称（LFNC）
	char pchfeatureName_Lfnc[255];
	strcpy_s(pchfeatureName_Lfnc, pchfeatureName);

	//待转换的相机厂商的参数名称（EBUS）
	char achfeatureName_Vendor[255];
	memset(achfeatureName_Vendor, 0x00, 255);

	//转换名称，由LFNC转换为EBUS
	if (!ConvertLfncToVendor(iCameraIndex, pchfeatureName_Lfnc, achfeatureName_Vendor))
	{
		return L_ERR_UNKNOWN;
	}

	std::string minV, maxV, incV;
	status = m_paoRanger3s[iCameraIndex]->getParameterValueConstraint(achfeatureName_Vendor, minV, maxV, incV);
	if (status == SickCam::All_OK && (dFeatureValue<std::stoi(minV) || dFeatureValue>std::stoi(maxV)))
	{
		return L_ERR_CCPARV;
	}

	status = m_paoRanger3s[iCameraIndex]->setParameterValue(achfeatureName_Vendor, std::to_string(dFeatureValue));

	if (status == SickCam::All_OK)
	{
		return L_OK;
	}
	else
	{
		return L_ERR_UNKNOWN;
	}


}

LE_STATUS __stdcall LCC_C_SetFeatureString(void *pCameraHandle, const char *pchfeatureName, const char *pchFeatureValue)
{
	//由相机句柄得到相机索引
	int iCameraIndex = GetIndexFromHandle(pCameraHandle);
	if (iCameraIndex < 0 || iCameraIndex >= CAMERA_MAX_NUM)
	{
		return L_ERR_CCWH;	//句柄无效
	}

	//else if (pchfeatureName == LFNC_SICK_PERSISITENTIP)
	//{
	//	std::string paramValue = pchFeatureValue;
	//	SickCam::CAM_STATUS status = m_paoRanger3s[iCameraIndex]->setIp(paramValue, true);
	//	if (status == SickCam::All_OK)
	//	{
	//		return L_OK;
	//	}
	//	else
	//	{
	//		return L_ERR_CCPARV;
	//	}
	//}
	//else if (pchfeatureName == LFNC_SICK_CURRENTIP)
	//{
	//	std::string paramValue = pchFeatureValue;
	//	SickCam::CAM_STATUS status = m_paoRanger3s[iCameraIndex]->setIp(paramValue);
	//	if (status == SickCam::All_OK)
	//	{
	//		return L_OK;
	//	}
	//	else
	//	{
	//		return L_ERR_CCPARV;
	//	}
	//}
	//else if (pchfeatureName == LFNC_SICK_PERSISITENTSUBNETMASK)
	//{
	//	std::string paramValue = pchFeatureValue;
	//	SickCam::CAM_STATUS status = m_paoRanger3s[iCameraIndex]->setSubnet(paramValue, true);
	//	if (status == SickCam::All_OK)
	//	{
	//		return L_OK;
	//	}
	//	else
	//	{
	//		return L_ERR_CCPARV;
	//	}
	//}
	//else if (pchfeatureName == LFNC_SICK_CURRENTSUBNETMASK)
	//{
	//	std::string paramValue = pchFeatureValue;
	//	SickCam::CAM_STATUS status = m_paoRanger3s[iCameraIndex]->setSubnet(paramValue);
	//	if (status == SickCam::All_OK)
	//	{
	//		return L_OK;
	//	}
	//	else
	//	{
	//		return L_ERR_CCPARV;
	//	}
	//}
	if (strcmp(pchfeatureName, LFNC_SICK_PARAMETPATH) == 0)
	{	
		SickCam::CAM_STATUS status = m_paoRanger3s[iCameraIndex]->setParametPath(pchFeatureValue);		
		if (status == SickCam::All_OK)
		{
			std::string hasReflectance = m_paoRanger3s[iCameraIndex]->getParameterValue("ComponentEnable_ComponentSelector_Reflectance_RegionSelector_Scan3dExtraction1");//判断是否含灰度值
			m_bHasReflectance[iCameraIndex] = std::stoi(hasReflectance);
			
			LE_STATUS eResult = ReleaseLusterBuffer(iCameraIndex);//释放buffer
			if (eResult != L_OK)
			{
				return L_ERR_CCNRB;	//无法初始化缓存
			}
			eResult = InitLusterBuffer(iCameraIndex, m_pastBufferInfo[iCameraIndex]);//初始化buffer
			if (eResult != L_OK)
			{
				return L_ERR_CCNIB;	//无法初始化缓存
			}

			if (!m_bIsCalibration[iCameraIndex])
			{
				//写入未标定ranger3xml文件所需信息
				m_pastRanger3XMLInfo[iCameraIndex]->tagCalibration = 0;

				std::string value = m_paoRanger3s[iCameraIndex]->getParameterValue("Height_RegionSelector_Region1");

				if (!value.empty())
				{
					m_pastRanger3XMLInfo[iCameraIndex]->CoordinateInfo.stCoordInfo.RegionHeight = std::stoi(value);
				}

				value = m_paoRanger3s[iCameraIndex]->getParameterValue("OffsetY_RegionSelector_Region1");

				if (!value.empty())
				{
					m_pastRanger3XMLInfo[iCameraIndex]->CoordinateInfo.stCoordInfo.RegionOffsetY = std::stoi(value);
				}

				int rangAixFlat = m_paoRanger3s[iCameraIndex]->getParameterValue("RangeAxis_Scan3dExtractionSelector_Scan3dExtraction1") == "Standard" ? 1 : -1;

				m_pastRanger3XMLInfo[iCameraIndex]->CoordinateInfo.stCoordInfo.RangAixFlat = rangAixFlat;

			}
			
			return L_OK;
		}
		else
		{
			return L_ERR_CCPARV;
		}
	}
	else if (strcmp(pchfeatureName, LFNC_SICK_CALIBRAPATH) == 0)
	{
		std::string paramValue = pchFeatureValue;
		SickCam::CAM_STATUS status = m_paoRanger3s[iCameraIndex]->setCalibraPath(paramValue);
		
		if (status == SickCam::All_OK)
		{
			m_bIsCalibration[iCameraIndex] = true;
			LE_STATUS eResult = ReleaseLusterBuffer(iCameraIndex);//释放buffer
			if (eResult != L_OK)
			{
				return L_ERR_CCNRB;	//无法初始化缓存
			}
			eResult = InitLusterBuffer(iCameraIndex, m_pastBufferInfo[iCameraIndex]);//初始化buffer
			if (eResult != L_OK)
			{
				return L_ERR_CCNIB;	//无法初始化缓存
			}
			return L_OK;
		}
		else
		{
			m_bIsCalibration[iCameraIndex] = false;
			return L_ERR_CCPARV;
		}
	}

	return L_ERR_CCPARNA;
	//导入配置文件
	//if (strcmp(pchfeatureName, LFNC_SICK_CONFIGURATION) == 0)
	//{
	//	std::string filePath = pchFeatureValue;
	//	std::ifstream inputStream(filePath);

	//	//ReadCSVFlie(pCameraHandle, inputStream);//读入配置文件
	//}

	return L_OK;
}

LE_STATUS __stdcall LCC_C_SetFeatureEnum(void *pCameraHandle, const char *pchfeatureName, int iFeatureValue)
{
	//由相机句柄得到相机索引
	int iCameraIndex = GetIndexFromHandle(pCameraHandle);
	if (iCameraIndex < 0 || iCameraIndex >= CAMERA_MAX_NUM)
	{
		return L_ERR_CCWH;	//句柄无效
	}

	return L_ERR_CCPARNA;


	/*if (pchfeatureName == LFNC_SICK_RECTIFICATIONMETHOD)
	{
		if (SickCam::RectMethod::Top == iFeatureValue)
			m_paoRanger3s[iCameraIndex]->setRectMethos(SickCam::RectMethod::Top);
		else if (SickCam::RectMethod::Bottom == iFeatureValue)
			m_paoRanger3s[iCameraIndex]->setRectMethos(SickCam::RectMethod::Bottom);
		else if (SickCam::RectMethod::Mean == iFeatureValue)
			m_paoRanger3s[iCameraIndex]->setRectMethos(SickCam::RectMethod::Mean);
	}*/
	//else if (strcmp(pchfeatureName, LFNC_SICK_LINE_SELECTOR) == 0)
	//{
	//	GenApi::CNodeMapRef device = m_paoConnection[iCameraIndex]->mDeviceNodeMap;
	//	GenApi::CEnumerationPtr lineSelector = device._GetNode("LineSelector");
	//	if (iFeatureValue == 0)
	//	{
	//		*lineSelector = "RS422A";
	//	}
	//	else if (iFeatureValue == 1)
	//	{
	//		*lineSelector = "RS422B";
	//	}
	//	/*else if (iFeatureValue == 2)
	//	{
	//	*lineSelector = "FrameTrig";
	//	}*/
	//}

	//else if (strcmp(pchfeatureName, LFNC_SICK_LINE_FORMAT) == 0)
	//{
	//	GenApi::CNodeMapRef device = m_paoConnection[iCameraIndex]->mDeviceNodeMap;
	//	GenApi::CEnumerationPtr lineFormat = device._GetNode("LineFormat");
	//	if (iFeatureValue == 0)
	//	{
	//		*lineFormat = "RS422";
	//	}
	//	/*else if (iFeatureValue == 1)
	//	{
	//	*lineFormat = "Line24V";
	//	}*/
	//}

	return L_OK;
}

LE_STATUS __stdcall LCC_C_SetFeatureUncertain(void *pCameraHandle, const char *pchfeatureName, void *pFeatureValue)
{
	int iCameraIndex = GetIndexFromHandle(pCameraHandle);
	if (iCameraIndex < 0 || iCameraIndex >= CAMERA_MAX_NUM)
	{
		return L_ERR_CCWH;//句柄无效
	}

	return L_ERR_CCPARNA;
	//if (strcmp(pchfeatureName, LFNC_SNAP_WAIT_TIME) == 0)
	//{
	//	int iSnapWaitTime = *((int*)pFeatureValue);
	//	m_iSnapWaitTime[iCameraIndex] = iSnapWaitTime;
	//}

	//else if (strcmp(pchfeatureName, LFNC_RELEASE_STORE_IMAGE) == 0)
	//{
	//	if (m_pastStoreImageInfo[iCameraIndex] == NULL)
	//	{
	//		return L_ERR_CCCL;//实例无效（或者已经注销）
	//	}
	//	m_pastStoreImageInfo[iCameraIndex][m_iPushStoreBufferIndex[iCameraIndex]].achReserved[INDEX_SEMAPHORE] = FALSE;
	//}

	//else if (strcmp(pchfeatureName, LFNC_DESTORY_CAMERA) == 0)
	//{

	//	//设备参数结构体申请内存
	//	for (int i = 0; i < CAMERA_MAX_NUM; i++)
	//	{
	//		if (m_pastCameraInfo[i] != NULL)
	//		{
	//			delete m_pastCameraInfo[i];
	//			m_pastCameraInfo[i] = NULL;
	//		}

	//	}
	//	////关闭数据流和设备
	//	//GenTL::DS_HANDLE dataStreamHandle = m_paoConnection[iCameraIndex]->mDataStreamHandle;
	//	//consumer.closeDataStream(dataStreamHandle);
	//	//consumer.closeDevice(m_paoConnection[iCameraIndex]->mDeviceHandle);

	//	//释放相机buffer缓存
	//	/*free(pBufferRangeAndReflectance[iCameraIndex]);
	//	pBufferRangeAndReflectance[iCameraIndex] = NULL;*/

	//	//释放相机连接类
	//	/*if (m_paoConnection[iCameraIndex] != NULL)
	//	{
	//	delete m_paoConnection[iCameraIndex];
	//	m_paoConnection[iCameraIndex] = NULL;
	//	}*/

	//	//若为连接的最后一个相机，则关闭所有相机接口
	//	/*if (iCameraIndex == m_iCameraNum - 1)
	//	{
	//	for (std::set<GenTL::IF_HANDLE>::iterator it = openInterfaces.begin(); it != openInterfaces.end(); ++it)
	//	{
	//	consumer.closeInterface(*it);
	//	}
	//	consumer.close();
	//	openInterfaces.clear();
	//	link_id.clear();
	//	}*/
	//	m_iCameraNum = 0;
	//}
	//else
	//{
	//	return L_ERR_CCPARAM;
	//}
	return L_OK;
}

LE_STATUS __stdcall LCC_C_RestartCamera(void *pCameraHandle)
{
	return L_OK;
}

LE_STATUS __stdcall LCC_C_RestoreFactory(void *pCameraHandle)
{
	return L_OK;
}


/***********************************************************************
*
*				       公共函数实现
*
***********************************************************************/

/******************************************************
@函数说明： 由相机句柄获得相机索引。
@参数输入： pCameraHandle -- 相机句柄。
@参数输出： 无
@返回值  ： 相机索引值，从0开始。成功则返回大于等于0整数，失败则返回小于0的整数。
******************************************************/
int GetIndexFromHandle(void *pCameraHandle)
{
#if IS_USE_INDEX
	int iCameraIndex = INVALID_CAMERA_INDEX;
	for (int i = 0; i < m_iCameraNum; i++)
	{
		if (pCameraHandle == m_pCamHandle[i])
		{
			iCameraIndex = i;
			break;
		}
	}
#else
	int iCameraIndex = INVALID_CAMERA_INDEX;
	for (int i = 0; i < m_iCameraNum; i++)
	{
		if (m_paoConnection[i] == NULL)
		{
			continue;
		}
		else
		{
			if (m_paoConnection[i]->mDeviceHandle == pCameraHandle)
			{
				iCameraIndex = i;
				break;
			}
		}
	}
#endif

	return iCameraIndex;
}

/******************************************************
@函数说明： 释放采集图像信息结构体，其中对采集图像数据内存进行管理。
@参数输入： pstImageInfo -- 需要释放的采集图像结构体。
iBufferNum -- Buffer数量
@参数输出： 无
@返回值  ： 成功返回true，反之返回false。
******************************************************/
bool FreeNodeBuffer(LST_IMAGE_INFO *pstImageInfo, int iBufferNum)
{
	//合理性判断
	if (pstImageInfo == NULL)
	{
		return false;
	}

	//循环释放所有的Buffer
	for (int i = 0; i < iBufferNum; i++)
	{
		if (pstImageInfo[i].pchBuffer == NULL && pstImageInfo[i].pstTagInfo == NULL)
		{
			free(pstImageInfo);
			//pstImageInfo = NULL;
			return false;
		}
		if (pstImageInfo[i].pstTagInfo != NULL)
		{
			free(pstImageInfo[i].pstTagInfo);
			pstImageInfo[i].pstTagInfo = NULL;
		}
		if (pstImageInfo[i].pchBuffer != NULL)
		{
			free(pstImageInfo[i].pchBuffer);
			pstImageInfo[i].pchBuffer = NULL;
		}
	}

	//释放结构体
	free(pstImageInfo);
	//pstImageInfo = NULL;

	return true;
}

/******************************************************
@函数说明： 创建采集图像信息结构体，其中对采集图像数据内存进行管理。
@参数输入： iCameraIndex -- 相机索引
pstImageInfo -- 需要创建的采集图像信息结构体。
pstImageProperty -- 图像属性结构体
pBufferInfo -- 缓存信息结构体
@参数输出： 无
@返回值  ： 成功返回true，反之返回false。
******************************************************/
bool CreateNodeBuffer(int iCameraIndex, LST_IMAGE_INFO **pstImageInfo, LST_IMAGE_PROPERTY *pstImageProperty, LST_BUFFER_INFO *pstBufferInfo)
{
	pstImageInfo[iCameraIndex] = (LST_IMAGE_INFO*)malloc(sizeof(LST_IMAGE_INFO)*(pstBufferInfo->iBufferNum));
	if (pstImageInfo[iCameraIndex] == NULL)
	{
		return false;
	}

	//判断iMergeFrameNum的超界情况
	int iMergeNum = pstBufferInfo->iMergeFrameNum;
	if (iMergeNum < 1 || iMergeNum > 100000)
	{
		iMergeNum = 1;
	}
	
	int bitCount = m_bIsCalibration[iCameraIndex] ? 8 : 2;
	int refSize = m_bHasReflectance[iCameraIndex] ? 1 : 0;

	int iWidth = pstImageProperty->iImageWidth;
	int iHeight = pstImageProperty->iImageHeight;
	int iFormat = pstImageProperty->iImagePixelFormat;
	int iBufferSize = iWidth * iHeight * iMergeNum;

	//循环创建多个Buffer
	for (int i = 0; i < pstBufferInfo->iBufferNum; i++)
	{
		switch (iFormat)
		{
		case Mono8:
			pstImageInfo[iCameraIndex][i].pchBuffer = (BYTE*)malloc(iBufferSize);
			if (pstImageInfo[iCameraIndex][i].pchBuffer == NULL)
				return false;
			memset(pstImageInfo[iCameraIndex][i].pchBuffer, 0x00, iBufferSize);
			pstImageInfo[iCameraIndex][i].iBufferSize = iBufferSize;
			break;
		case Mono12:
			pstImageInfo[iCameraIndex][i].pchBuffer = (BYTE*)malloc(iBufferSize * 2);
			if (pstImageInfo[iCameraIndex][i].pchBuffer == NULL)
				return false;
			memset(pstImageInfo[iCameraIndex][i].pchBuffer, 0x00, iBufferSize * 2);
			pstImageInfo[iCameraIndex][i].iBufferSize = iBufferSize * 2;
			break;
		case Mono12Packed:
			pstImageInfo[iCameraIndex][i].pchBuffer = (BYTE*)malloc(iBufferSize * 12 / 8);
			if (pstImageInfo[iCameraIndex][i].pchBuffer == NULL)
				return false;
			memset(pstImageInfo[iCameraIndex][i].pchBuffer, 0x00, iBufferSize * 12 / 8);
			pstImageInfo[iCameraIndex][i].iBufferSize = iBufferSize * 12 / 8;
			break;
		case Mono16:
			pstImageInfo[iCameraIndex][i].pchBuffer = (BYTE*)malloc(iBufferSize * (2 + 1));
			if (pstImageInfo[iCameraIndex][i].pchBuffer == NULL)
				return false;
			memset(pstImageInfo[iCameraIndex][i].pchBuffer, 0x00, iBufferSize * (2 + 1));
			pstImageInfo[iCameraIndex][i].iBufferSize = iBufferSize * (2 + 1);
			break;
		case Coord3D_C12p:
			pstImageInfo[iCameraIndex][i].pchBuffer = (BYTE*)malloc(iBufferSize * (bitCount + refSize));
			if (pstImageInfo[iCameraIndex][i].pchBuffer == NULL)
				return false;
			memset(pstImageInfo[iCameraIndex][i].pchBuffer, 0x00, iBufferSize * (bitCount + refSize));
			pstImageInfo[iCameraIndex][i].iBufferSize = iBufferSize * (bitCount + refSize);

			break;
		case Coord3D_C16:
			pstImageInfo[iCameraIndex][i].pchBuffer = (BYTE*)malloc(iBufferSize * (bitCount + refSize));
			if (pstImageInfo[iCameraIndex][i].pchBuffer == NULL)
				return false;
			memset(pstImageInfo[iCameraIndex][i].pchBuffer, 0x00, iBufferSize * (bitCount + refSize));
			pstImageInfo[iCameraIndex][i].iBufferSize = iBufferSize * (bitCount + refSize);

			break;
		case InvalidPixelFormat:
			return false;
		default:
			return false;
		}

		pstImageInfo[iCameraIndex][i].pstTagInfo = (LST_RANGER3XML_INFO*)malloc(sizeof(LST_RANGER3XML_INFO));
		memset(pstImageInfo[iCameraIndex][i].pstTagInfo, 0x00, sizeof(LST_RANGER3XML_INFO));
		//pstImageInfo[iCameraIndex][i].pstTagInfo = new LST_RANGER3XML_INFO();
		pstImageInfo[iCameraIndex][i].iTagInfoSize = sizeof(LST_RANGER3XML_INFO);

		pstImageInfo[iCameraIndex][i].iImageFrameIndex = 0;//帧号索引从0开始。
		pstImageInfo[iCameraIndex][i].achReserved[INDEX_SEMAPHORE] = FALSE;//用于Buffer处理  //TODO：这里的100应该在全局上定义一个常量代替

		pstImageInfo[iCameraIndex][i].stImageProperty.iImageWidth = pstImageProperty->iImageWidth;
		pstImageInfo[iCameraIndex][i].stImageProperty.iImageHeight = pstImageProperty->iImageHeight;
		pstImageInfo[iCameraIndex][i].stImageProperty.iImageDepth = pstImageProperty->iImageDepth;
		pstImageInfo[iCameraIndex][i].stImageProperty.iImagePixelFormat = pstImageProperty->iImagePixelFormat;
		pstImageInfo[iCameraIndex][i].stImageProperty.iImageChannels = pstImageProperty->iImageChannels;
		pstImageInfo[iCameraIndex][i].stImageProperty.iEndianess = pstImageProperty->iEndianess;
		strcpy_s(pstImageInfo[iCameraIndex][i].stImageProperty.achReserved, pstImageProperty->achReserved);

	}

	return true;
}

/******************************************************
@函数说明： 获取全部节点的数量
@参数输入： pNode -- 参数xml节点
@参数输出： 无
@返回值  ： 成功返回节点数量，失败返回0
******************************************************/
int GetAllNodeNum(XmlTreeNode* pNode)
{
	//判断合理性
	if (pNode == NULL)
	{
		return 0;
	}
	if (pNode->NodeType() == NODE_TYPE_UNKNOW)
	{
		return 0;
	}

	//定义
	int iNum = 0;

	//获取所有的子节点
	std::vector<XmlTreeNode*> ChildList = pNode->GetChildrenNodeList();
	if (ChildList.size() == 0)
	{
		return 0;
	}

	//加上子节点数量
	iNum += (int)ChildList.size();

	//迭代计算子节点的所有子节点
	for (int i = 0; i < ChildList.size(); i++)
	{
		iNum += GetAllNodeNum(ChildList[i]);
	}

	return iNum;
}

/******************************************************
@函数说明： 获取全部节点的相机参数信息结构体
@参数输入： pNode -- 参数xml节点
pstFni -- 参数名字信息结构体
iIndex -- 全局参数索引
@参数输出： 无
@返回值  ： 成功返回true，失败返回false
******************************************************/
int GetAllNodeInfo(XmlTreeNode* pNode, LST_NODE_INFO *pstFni, int &iIndex)
{
	//判断合理性
	if (pNode == NULL)
	{
		return 0;
	}
	if (pNode->NodeType() == NODE_TYPE_UNKNOW)
	{
		return 0;
	}

	//首先读取当前节点的子节点列表
	std::vector<XmlTreeNode*> ChildList = pNode->GetChildrenNodeList();

	//判断子节点的数量，如果等于0，则表示已经迭代到最底层，可以退出当前迭代
	if (ChildList.size() == 0)
	{
		return 1;
	}
	int ii = ChildList.size();
	//循环处理所有子节点
	for (int i = 0; i < ChildList.size(); i++)
	{
		//向结构体中赋值
		pstFni[iIndex].iNodeIndex = iIndex;
		memcpy(pstFni[iIndex].achNodeName, ChildList[i]->NodeName(), LEN_128);
		pstFni[iIndex].iNodeType = ChildList[i]->NodeType();
		pstFni[iIndex].achReserved[0] = '\0';

		//全局索引增加
		iIndex++;

		//迭代所有子节点的子节点
		GetAllNodeInfo(ChildList[i], pstFni, iIndex);
	}
	return 0;
}

/******************************************************
@函数说明： 已知凌云参数命名约定LFNC，从XML相机参数管理文档中，读取相机参数信息结构体
@参数输入： iCameraIndex -- 相机索引
pchfeatureName -- 凌云参数命名约定LFNC
@参数输出： stFeatureInfo -- 参数信息结构体，参见LST_FEATURE_INFO。
@返回值  ： 成功返回true，反之返回false。
******************************************************/
bool GetFeatureInfoFromXML(int iCameraIndex, const char *pchfeatureName, LST_FEATURE_INFO &stFeatureInfo)
{
	//合理性判断
	if (pchfeatureName == NULL)
	{
		stFeatureInfo.eType = FEATURE_TYPE_UNKNOW;
		return false;
	}

	//读取相机参数的节点类
	XmlTreeNode *poChildTreeNode = m_poTreeNodeRoot[iCameraIndex]->GetChildNode(pchfeatureName);
	if (poChildTreeNode == NULL)
	{
		stFeatureInfo.eType = FEATURE_TYPE_UNKNOW;
		return false;
	}
	if (poChildTreeNode->NodeType() == NODE_TYPE_UNKNOW)
	{
		stFeatureInfo.eType = FEATURE_TYPE_UNKNOW;
		return false;
	}
	stFeatureInfo = poChildTreeNode->GetFeatureInfo();
	return true;
}

/******************************************************
@函数说明： 已知凌云参数命名约定LFNC，得到对应的厂家参数命名（Vendor，此处为SaperaLT）
@参数输入： iCameraIndex -- 相机索引
pchfeatureName_Lfnc -- 凌云参数命名约定LFNC
@参数输出： pchfeatureName_Vendor -- 厂家参数命名（Vendor，此处为ranger3）
@返回值  ： 成功返回true，反之返回false。
******************************************************/
bool ConvertLfncToVendor(int iCameraIndex, char *pchfeatureName_Lfnc, char *pchfeatureName_Vendor)
{
	//合理性判断
	if (pchfeatureName_Lfnc == NULL)
	{
		return false;
	}
	if (pchfeatureName_Vendor == NULL)
	{
		return false;
	}

	//读取相机参数的节点类
	XmlTreeNode *poChildTreeNode = m_poTreeNodeRoot[iCameraIndex]->GetChildNode(pchfeatureName_Lfnc);
	if (poChildTreeNode->NodeType() == NODE_TYPE_UNKNOW)
	{
		return false;
	}
	strcpy_s(pchfeatureName_Vendor, 255, poChildTreeNode->GetFeatureInfo().achVenderFeatureName);

	return true;
}

/******************************************************
@函数说明： 设置线程优先级
@参数输入： dataStreamNodeMap -- 数据流节点图
@参数输出： 无
@返回值  ： 无
******************************************************/
#if 0
void setThreadPriority(GenApi::CNodeMapRef &dataStreamNodeMap)
{
	GenApi::CIntegerPtr threadPriority = dataStreamNodeMap._GetNode("StreamThreadPriority");
	GenApi::CCommandPtr setStreamPrio = dataStreamNodeMap._GetNode("StreamThreadApplyPriority");
	if (threadPriority.IsValid())
	{
		// Default priority is "HIGHEST" = 2, set to TIME CRITICAL = 15
		threadPriority->SetValue(15);
	}
	if (setStreamPrio.IsValid())
	{
		setStreamPrio->Execute();
	}
}
#endif // 0


unsigned long  _runRate(int iCameraIndex)
{
	m_bIsSensor[iCameraIndex] = m_paoRanger3s[iCameraIndex]->getParameterValue("DeviceScanType") == "Areascan";

	if (m_bIsSensor[iCameraIndex])
	{
		float linerate = atof(m_paoRanger3s[iCameraIndex]->getParameterValue("AcquisitionFrameRate").c_str());
		return static_cast<unsigned long>(1000 / linerate);
	}
	else
	{
		float linerate = atof(m_paoRanger3s[iCameraIndex]->getParameterValue("AcquisitionLineRate").c_str());
		int height = atoi(m_paoRanger3s[iCameraIndex]->getParameterValue("Height_RegionSelector_Region1").c_str());
		return static_cast<unsigned long>(height / linerate * 1000);
	}
}

/******************************************************
@函数说明： 读取图像线程，读取相机端的Buffer到计算机端的Buffer。
@参数输入： lpParam -- 线程默认参数，这里用来传输了相机句柄
@参数输出： 无
@返回值  ： 正常结束返回true，反之为false
******************************************************/

DWORD WINAPI  ThreadReceiveImage(LPVOID lpParam)
{
	int iImageWidth = 0;
	int	iImageHeight = 0;
	int iImagePixelFormat = 0;
	int iImageChannels = 0;
	int iImageDepth = 0;
	
	int iImageSize = 0;
	int iImageCount = 0;
	int iCameraStride = 0;
	//char *pImageData = NULL;
	LST_SYSTEM_TIME sysTime;
	SYSTEMTIME nowTime;
	LST_IMAGE_PROPERTY stImageProperty;
	LST_MESSAGE_INFO messageInfo;

	//合理性判断
	int iCameraIndex = *(int*)lpParam;

	if (iCameraIndex < 0 || iCameraIndex >= CAMERA_MAX_NUM)
	{
		return false;
	}

	iImageHeight =  m_pastImageProperty[iCameraIndex]->iImageHeight;

	iImageWidth =  m_pastImageProperty[iCameraIndex]->iImageWidth;

	int rangDataBitSize = m_bIsCalibration[iCameraIndex] ? 4 : 2;
	
	//OutputDebugString("start grab!\r\n");
	while (!m_bIsStopReceive[iCameraIndex])
	{		
		SickCam::ImgT imgTable;
		try
		{
			Ecode ec = m_paoRanger3s[iCameraIndex]->getImageData(imgTable);
			if (ec == Ecode::TIME_OUT)
			{
				Sleep(100);
				if (!m_bIsStopReceive[iCameraIndex])
				{
					continue;
				}
				else
				{
					break;
				}
			}

			if (ec != Ecode::All_OK)
			{
				if (!m_bIsStopReceive[iCameraIndex])
				{
					continue;
				}
				else
				{
					break;
				}
			}

		}
		catch (const char * msg)
		{
			m_bIsStopReceive[iCameraIndex] = true;
			break;
		}
		catch (...)
		{
			m_bIsStopReceive[iCameraIndex] = true;
			break;
		}

		m_iImageCount[iCameraIndex]++;


		/*SaveBuffer(imgTable.RangX, static_cast<float*>(imgTable.getData(DN::RAN_CAL)),
			nullptr, imgTable.get_cols(DN::RAN_CAL), imgTable.get_rows(DN::RAN_CAL),
			imgTable.getC_lower_bound_x(), imgTable.getC_lower_bound_r(),
			imgTable.getC_upper_bound_x(), imgTable.getC_upper_bound_r(), savePath);*/

		//获取系统时间
		::GetLocalTime(&nowTime);
		sysTime.iYear = nowTime.wYear;
		sysTime.iMonth = nowTime.wMonth;
		sysTime.iDay = nowTime.wDay;
		sysTime.iHour = nowTime.wHour;
		sysTime.iMinute = nowTime.wMinute;
		sysTime.iSecond = nowTime.wSecond;
		sysTime.iMilliSeconds = nowTime.wMilliseconds;

		if (m_bIsCalibration[iCameraIndex])
		{
			//X值
			memcpy(pBufferOfRange3[iCameraIndex], static_cast<float*>(imgTable.RangX), iImageHeight*iImageWidth * rangDataBitSize);
			//R值
			memcpy(pBufferOfRange3[iCameraIndex] + iImageHeight*iImageWidth * rangDataBitSize, static_cast<float*>(imgTable.getData(SickCam::DataNames::RAN_CAL)),
				iImageHeight*iImageWidth * rangDataBitSize);
		}
		else
		{
			memcpy(pBufferOfRange3[iCameraIndex], static_cast<float*>(imgTable.getData(SickCam::DataNames::RAN)),
				iImageHeight*iImageWidth * rangDataBitSize);

		}

		//灰度值
		if (m_bHasReflectance[iCameraIndex])
		{
			int offset = (m_bIsCalibration[iCameraIndex] ? rangDataBitSize * 2 : rangDataBitSize)*iImageHeight*iImageWidth;
			SickCam::DataNames dataName = m_bIsCalibration[iCameraIndex] ? SickCam::DataNames::REF_CAL : SickCam::DataNames::REF;
			memcpy(pBufferOfRange3[iCameraIndex] + offset, (uint8_t *)imgTable.getData(dataName),
				iImageHeight*iImageWidth);
			m_pastRanger3XMLInfo[iCameraIndex]->tagHasReflectance = 1;
			OutputDebugString("m_bHasReflectance \r\n");
		}
		else
		{
			m_pastRanger3XMLInfo[iCameraIndex]->tagHasReflectance = 0;
		}

		//这个只能在这里取的
		if (m_bIsCalibration[iCameraIndex])
		{
			m_pastRanger3XMLInfo[iCameraIndex]->tagCalibration = 1;
			m_pastRanger3XMLInfo[iCameraIndex]->CoordinateInfo.stCalibrationCoordInfo.LowerBoundX = imgTable.getC_lower_bound_x();
			m_pastRanger3XMLInfo[iCameraIndex]->CoordinateInfo.stCalibrationCoordInfo.LowerBoundR = imgTable.getC_lower_bound_r();
			m_pastRanger3XMLInfo[iCameraIndex]->CoordinateInfo.stCalibrationCoordInfo.UpperBoundX = imgTable.getC_upper_bound_x();
			m_pastRanger3XMLInfo[iCameraIndex]->CoordinateInfo.stCalibrationCoordInfo.UpperBoundR = imgTable.getC_upper_bound_r();
		}
		/*else
		{
			stRanger3XmlInfo->tagCalibration = 0;
			stRanger3XmlInfo->CoordinateInfo.stCoordInfo.RegionHeight = 1;
			stRanger3XmlInfo->CoordinateInfo.stCoordInfo.RegionOffsetY = 1;
			stRanger3XmlInfo->CoordinateInfo.stCoordInfo.RangAixFlat = 1;
		}*/

		//m_pastStoreImageInfo[iCameraIndex]->pstTagInfo = stRanger3XmlInfo;
						
		//pImageData = (char*)pBufferOfRange3[iCameraIndex];

		//存储Buffer，判断是否需要存图，
		PushImageToBuffer(m_pastStoreImageInfo[iCameraIndex], m_pastBufferInfo[iCameraIndex]->iBufferNum, iCameraIndex,
			m_iPushStoreBufferIndex[iCameraIndex], pBufferOfRange3[iCameraIndex], BufferSize[iCameraIndex],
			iImageWidth, iImageHeight, iImagePixelFormat, m_iImageCount[iCameraIndex], iImageChannels,
			iCameraStride, iImageDepth, &sysTime);

		memcpy(m_pastStoreImageInfo[iCameraIndex][m_iPushStoreBufferIndex[iCameraIndex]].pstTagInfo, m_pastRanger3XMLInfo[iCameraIndex],
			m_pastStoreImageInfo[iCameraIndex][m_iPushStoreBufferIndex[iCameraIndex]].iTagInfoSize);

		m_pastStoreImageInfo[iCameraIndex][m_iPushStoreBufferIndex[iCameraIndex]].achReserved[INDEX_SEMAPHORE] = TRUE;
	
		//显示Buffer，判断该帧是否需要被Miss
		if (m_pDisplayCallBack[iCameraIndex] != NULL)
		{
			if (!IsThisFrameMiss(m_iImageCount[iCameraIndex], (int)m_dCurFps[iCameraIndex], m_iShowFps[iCameraIndex]))
			{
				PushImageToBuffer(m_pastDisplayImageInfo[iCameraIndex], DISPLAY_MAX_BUFF, iCameraIndex,
					m_iPushDisplayBufferIndex[iCameraIndex], pBufferOfRange3[iCameraIndex], BufferSize[iCameraIndex], iImageWidth,
					iImageHeight, iImagePixelFormat, m_iImageCount[iCameraIndex], iImageChannels, iCameraStride, iImageDepth, &sysTime);

				memcpy(m_pastDisplayImageInfo[iCameraIndex][m_iPushDisplayBufferIndex[iCameraIndex]].pstTagInfo, m_pastRanger3XMLInfo[iCameraIndex],
					m_pastDisplayImageInfo[iCameraIndex][m_iPushDisplayBufferIndex[iCameraIndex]].iTagInfoSize);

				m_pastDisplayImageInfo[iCameraIndex][m_iPushDisplayBufferIndex[iCameraIndex]].achReserved[INDEX_SEMAPHORE] = TRUE;
			}
		}
		
		Sleep(m_RunRate[iCameraIndex]);

	}

	return TRUE;
}

/******************************************************
@函数说明： 显示回调线程，读取计算机端的Buffer通过显示类型回调传给上层调用者，
该图像数据用于显示，因此存在抽帧抽行功能。
@参数输入： lpParam -- 线程默认参数，这里用来传输了相机句柄
@参数输出： 无
@返回值  ： 正常结束返回true，反之为false
******************************************************/
DWORD WINAPI  ThreadDisplayImage(LPVOID lpParam)
{
	//合理性判断
	int iCameraIndex = *(int*)lpParam;
	if (iCameraIndex < 0 || iCameraIndex >= CAMERA_MAX_NUM)
	{
		return false;
	}

	//创建临时Buffer，存放每帧数据
	LST_IMAGE_INFO *pstImageInfoTemp[SNAP_BUF_NUM];
	pstImageInfoTemp[SNAP_BUF_INDEX] = NULL;

	//临时BufferInfo只取一个
	LST_BUFFER_INFO stBufferInfoTemp;
	stBufferInfoTemp.iBufferNum = SNAP_BUF_NUM;
	stBufferInfoTemp.iMergeFrameNum = 1;
	stBufferInfoTemp.achReserved[0] = EMPTY_STRING;

	bool bCreate = CreateNodeBuffer(SNAP_BUF_INDEX, pstImageInfoTemp, m_pastImageProperty[iCameraIndex], &stBufferInfoTemp);
	if (!bCreate)
	{
		return false;
	}
	
	//循环等待数据，一直等待，判断m_bIsStopShow退出
	while (1)
	{
		//取出缓存里的Buffer数据
		if (!PopImageFromBuffer(m_pastDisplayImageInfo[iCameraIndex], DISPLAY_MAX_BUFF, iCameraIndex, &pstImageInfoTemp[SNAP_BUF_INDEX],
			m_iPopDisplayBufferIndex[iCameraIndex]))
		{
			if (m_bIsStopDisplay[iCameraIndex])
				break;
			::Sleep(10);
		}
		else
		{
			//调用显示回调函数
			m_pDisplayCallBack[iCameraIndex](CB_DISPLAY, m_pCamHandle[iCameraIndex], pstImageInfoTemp[SNAP_BUF_INDEX], m_pDisplayUserContext[iCameraIndex]);


			//数据取出完毕，解锁信号量
			m_pastDisplayImageInfo[iCameraIndex][m_iPopDisplayBufferIndex[iCameraIndex]].achReserved[INDEX_SEMAPHORE] = FALSE;			
		}
	}

	//线程结束之前，释放临时Buffer资源
	if (pstImageInfoTemp[SNAP_BUF_INDEX] != NULL)
	{
		FreeNodeBuffer(pstImageInfoTemp[SNAP_BUF_INDEX], stBufferInfoTemp.iBufferNum);
		pstImageInfoTemp[SNAP_BUF_INDEX] = NULL;
	}

	return true;
}

/******************************************************
@函数说明： 存储回调线程，读取计算机端的Buffer通过存储类型回调传给上层调用者，
该图像数据用于存储，该图像数据需要保障不丢帧。
@参数输入： lpParam -- 线程默认参数，这里用来传输了相机句柄
@参数输出： 无
@返回值  ： 正常结束返回true，反之为false
******************************************************/
DWORD WINAPI  ThreadStoreImage(LPVOID lpParam)
{
	LST_MESSAGE_INFO messageInfo;
	int iLastImageCount = 0;
	int iImageCount = 0;

	//合理性判断
	int iCameraIndex = *((int*)lpParam);
	if (iCameraIndex < 0 || iCameraIndex >= CAMERA_MAX_NUM)
	{
		return false;
	}

	//创建临时Buffer，存放每帧数据
	LST_IMAGE_INFO *pstCurImageInfo[SNAP_BUF_NUM];
	pstCurImageInfo[SNAP_BUF_INDEX] = NULL;

	//Snap的buffer只取一个
	LST_BUFFER_INFO stSnapBufferInfo;
	stSnapBufferInfo.iBufferNum = SNAP_BUF_NUM;
	stSnapBufferInfo.iMergeFrameNum = 1;
	stSnapBufferInfo.achReserved[0] = EMPTY_STRING;

	bool bCreate = CreateNodeBuffer(SNAP_BUF_INDEX, pstCurImageInfo, m_pastImageProperty[iCameraIndex], &stSnapBufferInfo);
	if (!bCreate)
	{
		return false;
	}

	//循环等待数据，一直等待，判断m_bIsStopShow退出
	while (1)
	{
		//取出缓存里的Buffer数据

	/*	for (int i = 0; i < 5; i++)
		{
			if (m_pastStoreImageInfo[iCameraIndex][i].achReserved[INDEX_SEMAPHORE] == TRUE)
			{
				char ss[255] = { 0 };
				sprintf(ss, "m_pastStoreImageInfo is:%d %d\r\n", ((LST_RANGER3XML_INFO*)m_pastStoreImageInfo[iCameraIndex][i].pstTagInfo)->tagCalibration, i);
				OutputDebugString(ss);
			}
		}*/
		if (!PopImageFromBuffer(m_pastStoreImageInfo[iCameraIndex], m_pastBufferInfo[iCameraIndex]->iBufferNum, iCameraIndex, &pstCurImageInfo[SNAP_BUF_INDEX], m_iPopStoreBufferIndex[iCameraIndex]))
		{
			if (m_bIsStopStore[iCameraIndex])
				break;

			::Sleep(10);
		}
		else
		{

			if (m_pStoreCallBack[iCameraIndex] != NULL)
			{
				//调用存储回调函数
				m_pStoreCallBack[iCameraIndex](CB_STORE, m_pCamHandle[iCameraIndex], pstCurImageInfo[SNAP_BUF_INDEX], m_pStoreUserContext[iCameraIndex]);
			/*	std::string info = "当前buffer count is:"+std::to_string(pstCurImageInfo[SNAP_BUF_INDEX]->iBufferFullNum)+"\r\n";
				OutputDebugString(info.c_str());*/
				//存图模式结束，且当前已满Buffer数量为1。	//为什么是1？  因为此时这个正在被占用的Buffer还没有解锁释放，因此iBufferFullNum等于1即可。	
				//if(m_iRecordMode[iCameraIndex] == 2 && pstCurImageInfo[SNAP_BUF_INDEX]->iBufferFullNum == 1)
				//{
				//	if (m_pMessageCallBack[iCameraIndex] != NULL)
				//	{
				//		memset(messageInfo.achMessage, 0x00, 256);
				//		messageInfo.iMessageType = 0x01;//TODO:信息类型需后期详细定义。

				//		//调用消息回调函数
				//		m_pMessageCallBack[iCameraIndex](CB_MESSAGE, m_pCamHandle[iCameraIndex], &messageInfo, m_pMessageUserContext[iCameraIndex]);
				//		m_iRecordMode[iCameraIndex] = 0;
				//	}
				//}

				//数据取出完毕，解锁信号量
				m_pastStoreImageInfo[iCameraIndex][m_iPopStoreBufferIndex[iCameraIndex]].achReserved[INDEX_SEMAPHORE] = FALSE;			
			}
		}
	}

	//线程结束之前，释放临时Buffer资源
	if (pstCurImageInfo[SNAP_BUF_INDEX] != NULL)
	{
		FreeNodeBuffer(pstCurImageInfo[SNAP_BUF_INDEX], stSnapBufferInfo.iBufferNum);
		pstCurImageInfo[SNAP_BUF_INDEX] = NULL;
	}

	return true;
}
/******************************************************
@函数说明： 将内存数据保存进入缓冲区
@参数输入： pstImageInfo -- 采集图像信息结构体，计算机内存Buffer。
iBufferNum -- 计算机内存Buffer缓存数量
iCameraIndex -- 相机索引
iCurBufferIndex -- 当前Buffer索引
pchCameraBuffer -- 相机端得到的Buffer数据
iCameraBufferSize -- 相机Buffer大小
iImageWidth -- 相机Buffer宽度
iImageHeight -- 相机Buffer高度
iImagePixelFormat -- 相机Buffer像素格式，LFNC
iImageCount -- 帧计数值，该值为总计数，从1开始，一直增加到65536（ebus的说明）。
iImageChannles -- 相机Buffer通道数
iCameraStride -- 相机图像步频
iImageDepth	-- 相机Buffer位深
sysTime -- 系统时间结构体，当前帧时间信息
@参数输出： 无
@返回值  ： 成功操作Buffer返回true，失败或无Buffer可操作返回false。
******************************************************/
bool PushImageToBuffer(LST_IMAGE_INFO *pstImageInfo, int iBufferNum, int iCameraIndex, int &iCurBufferIndex,
	char *pchCameraBuffer, int iCameraBufferSize, int iImageWidth, int iImageHeight, int iImagePixelFormat,
	int iImageCount, int iImageChannles, int iCameraStride, int iImageDepth, LST_SYSTEM_TIME *sysTime)
{
	int iNextBufferIndex = 0;	//下一个Buffer索引
	int iBreakFlag = 0;			//寻找结果Flag

#if IS_USE_CLOCK
	clock_t start, end;
	double time;
#endif

	//参数合理性判断
	if (pstImageInfo == NULL || iBufferNum == 0)
		return false;

	//判断当前Buffer索引，得到下一buffer索引。
	if (iCurBufferIndex == iBufferNum - 1)
	{
		iNextBufferIndex = 0;//如果当前Buffer索引为最后一个buffer，则跳回到最开始的0。
	}
	else
	{
		iNextBufferIndex = iCurBufferIndex + 1;//正常情况下，索引加1
	}

	//按顺序遍历所有的Buffer，寻找第一个信号量为false的Buffer，并得到该Buffer的索引
	for (int index = iNextBufferIndex; index < iNextBufferIndex + iBufferNum; index++)
	{
		if (index > iBufferNum - 1) //如果索引超过buffer大小，则跳回
		{
			if (pstImageInfo[index - iBufferNum].achReserved[INDEX_SEMAPHORE] == FALSE)
			{
				iCurBufferIndex = index - iBufferNum;
				iBreakFlag = 1;
				break;
			}
		}
		else  //如果索引不超过buffer大小，不需要特别处理
		{
			if (pstImageInfo[index].achReserved[INDEX_SEMAPHORE] == FALSE)
			{
				iCurBufferIndex = index;
				iBreakFlag = 1;
				break;
			}
		}
	}

	//判断是否发现信号量为true的Buffer，没有发现则返回false
	if (iBreakFlag == 0)
	{
		return false;
	}

#if IS_USE_CLOCK
	start = clock();
#endif

#if IS_USE_FAST
	memcpy_fast(pstImageInfo[iCurBufferIndex].pchBuffer, pchCameraBuffer, iCameraBufferSize);

#else
	memcpy(pstImageInfo[iCurBufferIndex].pchBuffer, pchCameraBuffer, iCameraBufferSize);

#endif
	pstImageInfo[iCurBufferIndex].iBufferSize = iCameraBufferSize;
	pstImageInfo[iCurBufferIndex].stImageProperty.iImageWidth = iImageWidth;
	pstImageInfo[iCurBufferIndex].stImageProperty.iImageHeight = iImageHeight;
	pstImageInfo[iCurBufferIndex].stImageProperty.iImagePixelFormat = iImagePixelFormat;
	pstImageInfo[iCurBufferIndex].iImageFrameIndex = iImageCount;
	pstImageInfo[iCurBufferIndex].stImageProperty.iImageChannels = iImageChannles;
	pstImageInfo[iCurBufferIndex].iImageStrideRate = iCameraStride;
	pstImageInfo[iCurBufferIndex].stImageProperty.iImageDepth = iImageDepth;
	pstImageInfo[iCurBufferIndex].stImageProperty.iEndianess = m_pastImageProperty[iCameraIndex]->iEndianess;
	strcpy_s(pstImageInfo[iCurBufferIndex].stImageProperty.achReserved, m_pastImageProperty[iCameraIndex]->achReserved);
	memcpy(&pstImageInfo[iCurBufferIndex].stSysTime, sysTime, sizeof(LST_SYSTEM_TIME));
	//pstImageInfo[iCurBufferIndex].achReserved[INDEX_SEMAPHORE] = TRUE;

#if IS_USE_CLOCK
	end = clock();
	time = end - start;
#endif

	return true;
}

/******************************************************
@函数说明： 从计算机内存缓冲区中取出图像数据，最后通过CallBack给到上层。该计算机内存缓存区为多个Buffer的队列。
@参数输入： pstImageInfo -- 采集图像信息结构体，计算机内存Buffer。
iBufferNum -- 计算机内存Buffer缓存数量
iCameraIndex -- 相机索引
pstOutImageInfo -- 采集图像信息结构体，CallBack输出Buffer
iCurBufferIndex -- 当前Buffer索引
@参数输出： 无
@返回值  ： 成功操作Buffer返回true，失败或无Buffer可操作返回false。
******************************************************/
bool PopImageFromBuffer(LST_IMAGE_INFO *pstImageInfo, int iBufferNum, int iCameraIndex, LST_IMAGE_INFO **pstOutImageInfo, int &iCurBufferIndex)
{
	int iNextBufferIndex = 0;	//下一个Buffer索引
	int iBreakFlag = 0;			//寻找结果Flag

	//判断当前Buffer索引，得到下一buffer索引。
	if (iCurBufferIndex == iBufferNum - 1)
	{
		iNextBufferIndex = 0;//如果当前Buffer索引为最后一个buffer，则跳回到最开始的0。
	}
	else
	{
		iNextBufferIndex = iCurBufferIndex + 1;//正常情况下，索引加1
	}

	//按顺序遍历所有的Buffer，寻找第一个信号量为true的Buffer，并得到该Buffer的索引
	for (int index = iNextBufferIndex; index < iNextBufferIndex + iBufferNum; index++)
	{
		if (index > iBufferNum - 1)  //如果索引超过buffer大小，则跳回
		{
			if (pstImageInfo[index - iBufferNum].achReserved[INDEX_SEMAPHORE] == TRUE)
			{
				iCurBufferIndex = index - iBufferNum;
				iBreakFlag = 1;
				break;
			}
		}
		else  //如果索引不超过buffer大小，不需要特别处理
		{
			if (pstImageInfo[index].achReserved[INDEX_SEMAPHORE] == TRUE)
			{
				iCurBufferIndex = index;
				iBreakFlag = 1;
				break;
			}
		}
	}

	//判断是否发现信号量为true的Buffer，没有发现则返回false
	if (iBreakFlag == 0)
	{
		return false;
	}

	//拷贝图像数据

	memcpy(((LST_IMAGE_INFO *)*pstOutImageInfo)->pchBuffer, pstImageInfo[iCurBufferIndex].pchBuffer, pstImageInfo[iCurBufferIndex].iBufferSize);

	((LST_IMAGE_INFO *)*pstOutImageInfo)->iBufferSize = pstImageInfo[iCurBufferIndex].iBufferSize;
	((LST_IMAGE_INFO *)*pstOutImageInfo)->stImageProperty.iImageWidth = pstImageInfo[iCurBufferIndex].stImageProperty.iImageWidth;
	((LST_IMAGE_INFO *)*pstOutImageInfo)->stImageProperty.iImageHeight = pstImageInfo[iCurBufferIndex].stImageProperty.iImageHeight;
	((LST_IMAGE_INFO *)*pstOutImageInfo)->stImageProperty.iImagePixelFormat = pstImageInfo[iCurBufferIndex].stImageProperty.iImagePixelFormat;
	((LST_IMAGE_INFO *)*pstOutImageInfo)->iImageFrameIndex = pstImageInfo[iCurBufferIndex].iImageFrameIndex;
	((LST_IMAGE_INFO *)*pstOutImageInfo)->stImageProperty.iImageChannels = pstImageInfo[iCurBufferIndex].stImageProperty.iImageChannels;
	((LST_IMAGE_INFO *)*pstOutImageInfo)->iImageStrideRate = pstImageInfo[iCurBufferIndex].iImageStrideRate;
	((LST_IMAGE_INFO *)*pstOutImageInfo)->stImageProperty.iImageDepth = pstImageInfo[iCurBufferIndex].stImageProperty.iImageDepth;
	((LST_IMAGE_INFO *)*pstOutImageInfo)->stImageProperty.iEndianess = pstImageInfo[iCurBufferIndex].stImageProperty.iEndianess;
	strcpy_s(((LST_IMAGE_INFO *)*pstOutImageInfo)->stImageProperty.achReserved, pstImageInfo[iCurBufferIndex].stImageProperty.achReserved);
	memcpy(&((LST_IMAGE_INFO *)*pstOutImageInfo)->stSysTime, &pstImageInfo[iCurBufferIndex].stSysTime, sizeof(LST_SYSTEM_TIME));
	//char ss[255] = { 0 };
	//sprintf(ss, "popbuffer xml info is:%d %d\r\n", ((LST_RANGER3XML_INFO*)pstImageInfo[iCurBufferIndex].pstTagInfo)->tagCalibration, iCurBufferIndex);
	//OutputDebugString(ss);
	if (pstImageInfo[iCurBufferIndex].iTagInfoSize > 0)
	{
		memcpy(((LST_IMAGE_INFO *)*pstOutImageInfo)->pstTagInfo, pstImageInfo[iCurBufferIndex].pstTagInfo, pstImageInfo[iCurBufferIndex].iTagInfoSize);
	}
	
	//搜索当前缓存区内有多少个信号量为true的Buffer
	int iBufferCount = 0;
	for (int i = 0; i < iBufferNum; i++)
	{
		if (pstImageInfo[i].achReserved[INDEX_SEMAPHORE] == TRUE)
		{
			iBufferCount++;
		}
	}
	((LST_IMAGE_INFO *)*pstOutImageInfo)->iBufferFullNum = iBufferCount;

	return true;
}

/******************************************************
@函数说明： 根据当前的帧率进行丢帧处理,减少系统负担
@参数输入： iFrameCount -- 帧计数值，该值为总计数，从1开始，一直增加到65536（ebus的说明）。
iCurFrameRate -- 当前帧频
iDestFrameRate -- 目标帧频
@参数输出： 无
@返回值  ： 需要丢帧返回true，不需要丢帧(这帧数据显示)返回false
******************************************************/
bool  IsThisFrameMiss(int iFrameCount, int iCurFrameRate, int iDestFrameRate)
{
	//TODO:该抽帧函数的数学原理，请给出说明
	//TODO:这里的定义 int iCurFrameRate, int iDestFrameRate，如果给入的iCurFrameRate是小于1的小数，会被截取为0，
	//然后会被判断为该帧需要丢弃，则一直不再触发回调。建议改为double型。 
	//qiliu,2017-08-30 10-56
	int iPer = -1;
	int iDis = -1;
	int iFlag = -1;

	//参数合理性判断
	if (iCurFrameRate == 0 || iDestFrameRate == 0)
	{
		return true;
	}
	if (iDestFrameRate >= iCurFrameRate)
	{
		return false;
	}

	iPer = iCurFrameRate / iDestFrameRate + 1;
	iDis = iFrameCount % iDestFrameRate;
	iFlag = iDestFrameRate - iCurFrameRate / iPer;

	if (iFrameCount % iPer == 0)
	{
		return false;
	}
	else
	{
		int iTemp = 0;
		iTemp = iFrameCount % iCurFrameRate;
		if (iDis < (iFlag + iFlag / iPer) &&
			iTemp < iDestFrameRate)
		{
			return false;
		}
		else
		{
			return true;
		}
	}
}

/******************************************************
@函数说明： 加载相机参数管理XML文件。
@参数输入： iCameraIndex -- 相机索引
pchXmlFileName -- 文件绝对路径。
@参数输出： 无
@返回值  ： 成功返回true，反之返回false。
******************************************************/
bool LoadCameraXml(int iCameraIndex, const char *pchXmlFileName)
{
	//打开文件，并判断文件是否存在
	//FILE *pf = fopen(pchXmlFileName, "rb");
	FILE *pf;
	fopen_s(&pf, pchXmlFileName, "rb");

	if (pf == NULL)
	{
		return false;
	}
	fclose(pf);

	//初始化全局变量
	if (m_poTreeNodeRoot[iCameraIndex] != NULL)
	{
		delete m_poTreeNodeRoot[iCameraIndex];
		m_poTreeNodeRoot[iCameraIndex] = NULL;
	}

	//实例化类，并调用类函数加载文件
	m_poTreeNodeRoot[iCameraIndex] = new XmlTreeNode("", NODE_TYPE_UNKNOW);
	m_poTreeNodeRoot[iCameraIndex]->LoadXml(pchXmlFileName);

	return true;
}

/******************************************************
@函数说明： 根据输入的相机参数名称pchfeatureName，从相机参数管理文档XML中
读取该参数的类型，判断其与输入的参数类型eType是否相同。
@参数输入： iCameraIndex -- 相机索引
pchfeatureName -- 相机参数名称，此名称为LFNC名称。
eType -- 输入的参数类型。
@参数输出： 无
@返回值  ： 类型相同返回true，反之返回false
******************************************************/
bool IsFeatureTypeRight(int iCameraIndex, const char* pchfeatureName, LE_FEATURE_TYPE eType)
{
	//合理性判断
	if (m_poTreeNodeRoot == NULL)
	{
		return NULL;
	}
	if (pchfeatureName == NULL)
	{
		return NULL;
	}

	//读取相机参数的节点类
	XmlTreeNode *poChildTreeNode = m_poTreeNodeRoot[iCameraIndex]->GetChildNode(pchfeatureName);
	if (poChildTreeNode->NodeType() == NODE_TYPE_UNKNOW)
	{
		return NULL;
	}

	//根据节点类返回该节点的相机参数信息
	LST_FEATURE_INFO stFeatureInfo = poChildTreeNode->GetFeatureInfo();

	//判断
	if (stFeatureInfo.eType == eType)
	{
		return true;
	}
	else
	{
		return false;
	}
}

/******************************************************
@函数说明： 根据LFNC的枚举（int形式），得到相应的LST_IMAGE_PROPERTY结构体
@参数输入： iPixelFormat -- 像素类型
@参数输出： stImageProperty -- 图像属性结构体
@返回值  ： 成功返回true，反之返回false。
******************************************************/
bool GetImagePropertyFromEnumInt(int iPixelFormat, LST_IMAGE_PROPERTY *stImageProperty)
{
	//参数合理性判断
	if (iPixelFormat < 0)
	{
		return false;
	}

	//判断类型，返回对应的位数和通道数
	switch (iPixelFormat)
	{
	case Mono8:
		stImageProperty->iImageDepth = 8;
		stImageProperty->iImageChannels = 1;
		return true;
	case Mono16:
		stImageProperty->iImageDepth = 16;
		stImageProperty->iImageChannels = 1;
		return true;
	case Coord3D_C12p:
		stImageProperty->iImageDepth = 16;
		stImageProperty->iImageChannels = 1;
		return true;
	case Coord3D_C16:
		stImageProperty->iImageDepth = 16;
		stImageProperty->iImageChannels = 1;
		return true;
	case InvalidPixelFormat:
		return false;
	default:
		return false;
	}
}

void SaveBuffer(
	const float* xBuffer,
	const float* zBuffer,
	const uint8_t* iBuffer,
	int64_t bufferWidth,
	int64_t bufferHeight,
	int64_t lowerBoundX,
	int64_t lowerBoundR,
	int64_t upperBoundX,
	int64_t upperBoundR,
	const std::string& filePath)
{
	std::string datFilename = filePath + ".dat";
	std::string xmlFilename = filePath + ".xml";

	std::ofstream dataStream(datFilename.c_str(), std::ios::binary | std::ios::trunc | std::ios::out);
	dataStream.exceptions(std::ios::failbit | std::ios::badbit);


	dataStream.write((const char*)xBuffer, bufferWidth*bufferHeight*sizeof(float));
	dataStream.write((const char*)zBuffer, bufferWidth*bufferHeight*sizeof(float));

	bool isSaveInstensity = false;
	if (iBuffer != nullptr)
	{
		dataStream.write((const char*)iBuffer, bufferWidth*bufferHeight);
		isSaveInstensity = true;
	}

	dataStream.close();

	DatXmlWriter xml;
	xml.open();
	{
		xml.addParameter("size", std::to_string(isSaveInstensity ? bufferWidth * (sizeof(float)* 2 + 1) : bufferWidth*sizeof(float)* 2));
		xml.addParameter("version", "1");
		xml.addParameter("layout", "SUBCOMPONENT");
		xml.openComponent("", "Ranger3Range");
		{
			xml.addParameter("size", std::to_string(isSaveInstensity ? bufferWidth * (sizeof(float)* 2 + 1) : bufferWidth*sizeof(float)* 2));
			xml.addParameter("height", "1");
			xml.openWorldRangeTraits();
			{
				// From where on the sensor the data comes from can be calculated by
				// using the origin and scale factors below

				xml.addParameter("lower bound x", std::to_string(lowerBoundX));
				xml.addParameter("lower bound r", std::to_string(lowerBoundR));
				xml.addParameter("upper bound x", std::to_string(upperBoundX));
				xml.addParameter("upper bound r", std::to_string(upperBoundR));
				xml.addParameter("coordinate unit", "");
			}
			xml.closeWorldRangeTraits();
			xml.openSubComponent("FLOAT", "Range X");
			{
				xml.addParameter("size", std::to_string(bufferWidth * sizeof(float)));
				xml.addParameter("width", std::to_string(bufferWidth));
			}
			xml.closeSubComponent();
			xml.openSubComponent("FLOAT", "Range R");
			{
				xml.addParameter("size", std::to_string(bufferWidth * sizeof(float)));
				xml.addParameter("width", std::to_string(bufferWidth));
			}
			if (isSaveInstensity == true)
			{
				xml.openSubComponent("BYTE", "Intensity");
				{
					xml.addParameter("size", std::to_string(bufferWidth));
					xml.addParameter("width", std::to_string(bufferWidth));
				}
				xml.closeSubComponent();
			}
			xml.closeSubComponent();
		}
		xml.closeComponent();
	}
	xml.close();

	std::ofstream xmlStream(xmlFilename.c_str(), std::ios::binary | std::ios::trunc | std::ios::out);
	xmlStream.exceptions(std::ios::failbit | std::ios::badbit);
	xmlStream << xml.toString();
	xmlStream.close();

}

/******************************************************
@函数说明： 获取相机端buffer（Range）
@参数输入： tl -- 传输句柄
dataStreamHandle -- 数据流句柄
bufferHandle -- buffer句柄
@参数输出： bufferData -- mono16 buffer
bufferSize -- mono16 buffer的大小
@返回值  ： 无
******************************************************/



/******************************************************
@函数说明： 获取相机参数配置文件（csv）中的参数名称和参数值
@参数输入： line -- 配置文件的行
@参数输出： key -- 参数名称
value -- 参数值
@返回值  ： 无
******************************************************/
void GetKeyValue(std::string line, std::string& key, std::string& value)
{
	std::string element;
	std::stringstream stream(line);
	std::vector<std::string> values;

	while (std::getline(stream, element, ','))
	{
		element.erase(std::remove(element.begin(), element.end(), '\n'), element.end());
		values.push_back(element);
	}
	if (values.size() == 2)
	{
		key = values.at(0);
		value = values.at(1);
	}
	else
	{
		std::string error("No <key>,<value> pair found");
	}
}

/******************************************************
@函数说明： 初始化自定义缓存模块。存储缓存、显示缓存、单帧缓存、缓存索引
@参数输入： iCameraIndex -- 相机索引
@参数输出： 无
@返回值  ： 成功返回true，失败返回false
******************************************************/
LE_STATUS InitLusterBuffer(int iCameraIndex, LST_BUFFER_INFO *stBufferInfo)
{
	//相机属性结构体
	m_pastImageProperty[iCameraIndex] = new LST_IMAGE_PROPERTY();
	//Ranger3 XML信息结构体
	m_pastRanger3XMLInfo[iCameraIndex] = new LST_RANGER3XML_INFO();

	/*******************************************
	读取图像属性
	*******************************************/

	//读取ImageProperty结构体
	//读取图像宽度
	LE_STATUS eResult;
	LST_INT_INFO stiIntInfo;
	LST_BOOLEAN_INFO stBooleanInfo;

	std::string width = m_paoRanger3s[iCameraIndex]->getParameterValue("Width_RegionSelector_Region1");

	m_pastImageProperty[iCameraIndex]->iImageWidth = std::stoi(width);

	std::string height = m_paoRanger3s[iCameraIndex]->getParameterValue("Height_RegionSelector_Scan3dExtraction1");
	m_pastImageProperty[iCameraIndex]->iImageHeight = std::stoi(height);

	std::string pixFormat = m_paoRanger3s[iCameraIndex]->getParameterValue("PixelFormat_ComponentSelector_Range_RegionSelector_Scan3dExtraction1");
	if (pixFormat == "Coord3D_C16")
	{
		m_pastImageProperty[iCameraIndex]->iImagePixelFormat = Coord3D_C16;
	}
	else
	{
		m_pastImageProperty[iCameraIndex]->iImagePixelFormat = Coord3D_C12p;
	}

    
	/*if (b_image2D == true)
	{
		eResult = LCC_C_SetFeatureEnum(m_pCamHandle[iCameraIndex], LFNC_SICK_REGION_SELECTOR, 0);
		eResult = LCC_C_GetFeatureInt(m_pCamHandle[iCameraIndex], LFNC_WIDTH, &stiIntInfo);
		m_pastImageProperty[iCameraIndex]->iImageWidth = stiIntInfo.iValue;
		eResult = LCC_C_GetFeatureInt(m_pCamHandle[iCameraIndex], LFNC_HEIGHT, &stiIntInfo);
		m_pastImageProperty[iCameraIndex]->iImageHeight = stiIntInfo.iValue;
		m_pastImageProperty[iCameraIndex]->iImagePixelFormat = Mono8;

		pBufferRangeAndReflectanceSize[iCameraIndex] = m_pastImageProperty[iCameraIndex]->iImageWidth*m_pastImageProperty[iCameraIndex]->iImageHeight;
		pBufferRangeAndReflectance[iCameraIndex] = (uint8_t*)malloc(static_cast<size_t>(pBufferRangeAndReflectanceSize[iCameraIndex]));
	}*/
	
	//eResult = LCC_C_SetFeatureEnum(m_pCamHandle[iCameraIndex], LFNC_SICK_REGION_SELECTOR, 2);
	//	if (eResult != L_OK)
	//	{
	//		return eResult;
	//	}

	//	//读取图像高度
	//	eResult = LCC_C_GetFeatureInt(m_pCamHandle[iCameraIndex], LFNC_HEIGHT, &stiIntInfo);
	//	if (eResult != L_OK)
	//	{
	//		return eResult;
	//	}
	//	m_pastImageProperty[iCameraIndex]->iImageHeight = stiIntInfo.iValue;

	//	eResult = LCC_C_SetFeatureEnum(m_pCamHandle[iCameraIndex], LFNC_SICK_REGION_SELECTOR, 1);
	//	if (eResult != L_OK)
	//	{
	//		return eResult;
	//	}

	//	eResult = LCC_C_GetFeatureInt(m_pCamHandle[iCameraIndex], LFNC_WIDTH, &stiIntInfo);
	//	if (eResult != L_OK)
	//	{
	//		return eResult;
	//	}
		
		//eResult = LCC_C_GetFeatureBoolean(m_pCamHandle[iCameraIndex], LFNC_SICK_REFLECTANCE_ENABLE, &stBooleanInfo);
		
	    /*std::string calPath=m_paoRanger3s[iCameraIndex]->getCalibraPath();
		m_bIsCalibration[iCameraIndex] = calPath == "" ? 0 : 1;*/
		int r = m_bHasReflectance[iCameraIndex] ? 1 : 0;
		//m_bIsCalibration[iCameraIndex] = 1;
		if (m_bIsCalibration[iCameraIndex])
		{	
			BufferSize[iCameraIndex] = m_pastImageProperty[iCameraIndex]->iImageWidth*m_pastImageProperty[iCameraIndex]->iImageHeight*(8+r);
			pBufferOfRange3[iCameraIndex] = (char*)malloc(static_cast<size_t>(BufferSize[iCameraIndex]));
		}
		else
		{
			BufferSize[iCameraIndex] = m_pastImageProperty[iCameraIndex]->iImageWidth*m_pastImageProperty[iCameraIndex]->iImageHeight*(2+r);
			pBufferOfRange3[iCameraIndex] = (char*)malloc(static_cast<size_t>(BufferSize[iCameraIndex]));

			//写入未标定ranger3xml文件所需信息
			m_pastRanger3XMLInfo[iCameraIndex]->tagCalibration = 0;	

			std::string value = m_paoRanger3s[iCameraIndex]->getParameterValue("Height_RegionSelector_Region1");

			if (!value.empty())
			{
				m_pastRanger3XMLInfo[iCameraIndex]->CoordinateInfo.stCoordInfo.RegionHeight = std::stoi(value);
			}

			value = m_paoRanger3s[iCameraIndex]->getParameterValue("OffsetY_RegionSelector_Region1");

			if (!value.empty())
			{
				m_pastRanger3XMLInfo[iCameraIndex]->CoordinateInfo.stCoordInfo.RegionOffsetY = std::stoi(value);
			}

			int rangAixFlat = m_paoRanger3s[iCameraIndex]->getParameterValue("RangeAxis_Scan3dExtractionSelector_Scan3dExtraction1") == "Standard" ? 1 : -1;

			m_pastRanger3XMLInfo[iCameraIndex]->CoordinateInfo.stCoordInfo.RangAixFlat = rangAixFlat;
		}
	

	//eResult = LCC_C_SetFeatureEnum(m_pCamHandle[iCameraIndex], LFNC_SICK_REGION_SELECTOR, 1);
	//GenApi::CIntegerPtr offsetY3D = m_paoConnection[iCameraIndex]->mDeviceNodeMap._GetNode("OffsetY");
	//int aoi_offsety = offsetY3D->GetValue();
	//GenApi::CIntegerPtr offsetX3D = m_paoConnection[iCameraIndex]->mDeviceNodeMap._GetNode("OffsetX");
	//int aoi_offsetx = offsetX3D->GetValue();
	//GenApi::CIntegerPtr width3d = m_paoConnection[iCameraIndex]->mDeviceNodeMap._GetNode("Width");
	//int aoi_width = width3d->GetValue();
	//GenApi::CIntegerPtr height3d = m_paoConnection[iCameraIndex]->mDeviceNodeMap._GetNode("Height");
	//int aoi_height = height3d->GetValue();

	//m_paoConnection[iCameraIndex]->mAoi = Aoi(aoi_offsetx, aoi_offsety, aoi_width, aoi_height);

	//读取图像像素格式
	if (!GetImagePropertyFromEnumInt(m_pastImageProperty[iCameraIndex]->iImagePixelFormat, m_pastImageProperty[iCameraIndex]))
	{
		return L_ERR_UNKNOWN;
	}
	m_pastImageProperty[iCameraIndex]->iEndianess = 0;//以后可根据相机具体判断。
	m_pastImageProperty[iCameraIndex]->achReserved[0] = EMPTY_STRING;

	//读取当前的帧频，如果是线阵相机则由行频推算帧频

	/*GenApi::CFloatPtr acquisitionFrameRate = m_paoConnection[iCameraIndex]->mDeviceNodeMap._GetNode("AcquisitionFrameRate");
	m_dCurFps[iCameraIndex] = acquisitionFrameRate->GetValue();*/

	std::string curFps = m_paoRanger3s[iCameraIndex]->getParameterValue("AcquisitionFrameRate");
	if (!curFps.empty())
	{
		m_dCurFps[iCameraIndex] = std::stoi(curFps);
	}
	else
	{
		m_dCurFps[iCameraIndex] = 20;
	}

	//创建存储Buffer
	bool bResult = CreateNodeBuffer(iCameraIndex, m_pastStoreImageInfo, m_pastImageProperty[iCameraIndex], stBufferInfo);
	if (bResult == false)
	{
		return L_ERR_CCNIB;	//图像采集设备无法初始化
	}

	//显示Buffer信息结构体
	LST_BUFFER_INFO stShowBufferInfo;
	stShowBufferInfo.iBufferNum = DISPLAY_MAX_BUFF;
	stShowBufferInfo.iMergeFrameNum = m_pastBufferInfo[iCameraIndex]->iMergeFrameNum;
	stShowBufferInfo.achReserved[0] = EMPTY_STRING;

	//创建显示Buffer
	bResult = CreateNodeBuffer(iCameraIndex, m_pastDisplayImageInfo, m_pastImageProperty[iCameraIndex], &stShowBufferInfo);
	if (bResult == false)
	{
		return L_ERR_CCNIB;	//图像采集设备无法初始化
	}


	return L_OK;
}

/******************************************************
@函数说明： 释放自定义缓存模块。存储缓存、显示缓存、单帧缓存、缓存索引
@参数输入： iCameraIndex -- 相机索引
@参数输出： 无
@返回值  ： 成功返回true，失败返回false
******************************************************/
LE_STATUS ReleaseLusterBuffer(int iCameraIndex)
{
	//释放存储缓存
	if (m_pastStoreImageInfo[iCameraIndex] != NULL)
	{
		FreeNodeBuffer(m_pastStoreImageInfo[iCameraIndex], m_pastBufferInfo[iCameraIndex]->iBufferNum);
		m_pastStoreImageInfo[iCameraIndex] = NULL;
	}
	//释放显示缓存
	if (m_pastDisplayImageInfo[iCameraIndex] != NULL)
	{
		FreeNodeBuffer(m_pastDisplayImageInfo[iCameraIndex], DISPLAY_MAX_BUFF);
		m_pastDisplayImageInfo[iCameraIndex] = NULL;
	}
	//首先清空操作，单帧buffer, LST_IMAGE_INFO
	if (m_pastSnapImageInfo[SNAP_BUF_INDEX] != NULL)
	{
		FreeNodeBuffer(m_pastSnapImageInfo[SNAP_BUF_INDEX], SNAP_BUF_NUM);//只要1个buffer
		m_pastSnapImageInfo[SNAP_BUF_INDEX] = NULL;
	}

	//清空buffer缓存
	if (pBufferOfRange3[iCameraIndex] != NULL)
	{
		free(pBufferOfRange3[iCameraIndex]);
		pBufferOfRange3[iCameraIndex] = NULL;
	}

	//首先清空操作，图像属性结构体
	if (m_pastImageProperty[iCameraIndex] != NULL)
	{
		delete m_pastImageProperty[iCameraIndex];
		m_pastImageProperty[iCameraIndex] = NULL;
	}

	//首先清空操作，清空Ranger3xml信息
	if (m_pastRanger3XMLInfo[iCameraIndex] != NULL)
	{
		delete m_pastRanger3XMLInfo[iCameraIndex];
		m_pastRanger3XMLInfo[iCameraIndex] = NULL;
	}
	//缓存索引初始化
	m_iPushStoreBufferIndex[iCameraIndex] = -1;
	m_iPopStoreBufferIndex[iCameraIndex] = -1;
	m_iPushDisplayBufferIndex[iCameraIndex] = -1;
	m_iPopDisplayBufferIndex[iCameraIndex] = -1;

	return L_OK;
}

#endif // 0


