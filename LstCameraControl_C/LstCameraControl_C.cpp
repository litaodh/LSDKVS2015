// LstCameraControl_C.cpp : ���� DLL Ӧ�ó���ĵ���������
//



// LusterCameraControlC.cpp : ���� DLL Ӧ�ó���ĵ���������
/***********************************************************************
*
* Copyright (c) 2000-2001 Luster LightTech(Beijing) Co.Ltd.
* All Rights Reserved.
*
*
* FILE NAME     :  LusterCameraControlC.cpp
* PROJECT NAME  :  LusterSDK
* ABSTRACT      :  �������ģ��ͷ�ļ�����׼C���԰汾����
*				   ��ϸ�μ����µ����������ӿ�����ĵ������SDK������ɼ��������_v1.10����
*					�����SDK�������ͳһ�ӿ���ϸ���˵��_v0.1.1.0��
*
*
* VERSION       :  0.1.2.0
* DATE 		    :  2017/12/6
* AUTHOR 		:  �׵�
* NOTE 		    :  ������Ranger3-1.1.3.1497
*				   ������̣�Sick
*				   ����ͺţ�Ranger3
*				   ��Ӧ�ӿ��ĵ������SDK�������ͳһ�ӿ���ϸ���˵��(��׼C����)_v0.1.2.0����
*
*
* VERSION       :  1.0
* DATE 		    :  2018/3/12
* AUTHOR 		:  �׵�
* NOTE 		    :  ������Ranger3-1.1.3.1497
*				   ������̣�Sick
*				   ����ͺţ�Ranger3
*				   ��Ӧ�ӿ��ĵ���LSDK�������ͳһ�ӿ�˵��(��׼C����)_v1.0.0����
* VERSION       :  2.0
* DATE 		    :  2019/1/24
* AUTHOR 		:  ��Ƽ
* NOTE 		    :  ������Ranger3-2.0.3.1866
*				   ������̣�Sick
*				   ����ͺţ�Ranger3
*				   ��Ӧ�ӿ��ĵ���LSDK�������ͳһ�ӿ�˵��(��׼C����)_v1.0.0����

* VERSION       :  3.0
* DATE 		    :  2020/4/20
* AUTHOR 		:  ����
* NOTE 		    :  ������Ranger3-2.1.0.3050
*				   ������̣�Sick
*				   ����ͺţ�Ranger3
*				   ��Ӧ�ӿ��ĵ���LSDK�������ͳһ�ӿ�˵��(��׼C����)_v1.0.0����

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
*				       ȫ�ֱ�������
*
***********************************************************************/

#define IS_USE_CLOCK 0
#define IS_USE_FAST 0
#define IS_USE_INDEX    1

#define SNAP_BUF_NUM    1
#define SNAP_BUF_INDEX  0

#define CAMERA_MAX_NUM        32		//֧������������		
#define DISPLAY_MAX_BUFF      5			//��ʾ������Ŀ
#define EMPTY_STRING          '\0'		//������ַ�
#define INVALID_CAMERA_INDEX  -1        //������Ч�������ֵ
#define INDEX_SEMAPHORE       100		//�����ź��������LST_IMAGE_INFO�ṹ���char achReserved[128]���Ե��ַ�����
#define LEN_128               128		//�ַ�������128

static int m_iCameraNum = 0;								//��⵽�����������
static int m_iCameraNumOpen = 0;							//�򿪵����������
static int m_iShowFps[CAMERA_MAX_NUM];					//��ʾ֡Ƶ
static int m_iRecordMode[CAMERA_MAX_NUM];				//�洢���أ�0Ϊ��ʼֵ��1��������2����ر�
static double m_dCurFps[CAMERA_MAX_NUM];				//��ǰ����ʾ֡Ƶ����Ҫ���ڳ�֡��ʾʱ������
static void *m_pCamHandle[CAMERA_MAX_NUM];				//���ڴ��������
static int m_iCamIndex[CAMERA_MAX_NUM];					//���ڴ���������  ={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31}

static std::shared_ptr<R3S> m_pRanger3Share;// = std::make_shared<R3S>("");           //Ranger3������
static SickCam::Ranger3 *m_paoRanger3s[CAMERA_MAX_NUM];   //ͼ��ɼ���;
static LST_CAMERA_INFO	*m_pastCameraInfo[CAMERA_MAX_NUM];	//�����Ϣ�ṹ��

static LCC_C_Callback m_pStoreCallBack[CAMERA_MAX_NUM];		//�洢���͵Ļص�ָ��
static LCC_C_Callback m_pMessageCallBack[CAMERA_MAX_NUM];	//��Ϣ���͵Ļص�ָ��
static LCC_C_Callback m_pDisplayCallBack[CAMERA_MAX_NUM];	//��ʾ���͵Ļص�ָ��
static void	*m_pStoreUserContext[CAMERA_MAX_NUM];			//�洢���͵��û�������
static void	*m_pDisplayUserContext[CAMERA_MAX_NUM];			//��Ϣ���͵��û�������
static void	*m_pMessageUserContext[CAMERA_MAX_NUM];			//��ʾ���͵��û�������

static HANDLE m_ThreadPushHandle[CAMERA_MAX_NUM];		//�߳̾������ȡͼ���̣߳���ȡ����˵�Buffer��������˵�Buffer��
static HANDLE m_ThreadPopStoreHandle[CAMERA_MAX_NUM];	//�߳̾�����洢�ص��̣߳���ȡ������˵�Bufferͨ���洢���ͻص������ϲ�����ߣ���ͼ���������ڴ洢����ͼ��������Ҫ���ϲ���֡��
static HANDLE m_ThreadPopDisplayHandle[CAMERA_MAX_NUM];	//�߳̾������ʾ�ص��̣߳���ȡ������˵�Bufferͨ����ʾ���ͻص������ϲ�����ߣ���ͼ������������ʾ����˴��ڳ�֡���й��ܡ�
static int    m_iThreadID[CAMERA_MAX_NUM];				//�߳�ID���˴���������������CameraIndex
static bool   m_bIsStopReceive[CAMERA_MAX_NUM];			//״̬�������Ƿ�ֹͣ��ȡͼ��ThreadReceiveImage
static bool   m_bIsStopStore[CAMERA_MAX_NUM];			//״̬�������Ƿ�ֹͣ�洢ͼ��ThreadStoreImage
static bool   m_bIsStopDisplay[CAMERA_MAX_NUM];			//״̬�������Ƿ�ֹͣ��ʾͼ��ThreadDisplayImage

static XmlTreeNode *m_poTreeNodeRoot[CAMERA_MAX_NUM];	//XML�����ļ��������*/
static int m_iSnapWaitTime[CAMERA_MAX_NUM];				//��֡ץͼ�ȴ�ʱ��
static int m_iImageCount[CAMERA_MAX_NUM];                //ͼ��֡��

/***********************************************************************
*				      ����ģ��
***********************************************************************/
static LST_BUFFER_INFO	  *m_pastBufferInfo[CAMERA_MAX_NUM];		//�ɼ�������Ϣ�ṹ��
static LST_IMAGE_PROPERTY *m_pastImageProperty[CAMERA_MAX_NUM];		//ͼ�����Խṹ��

static LST_IMAGE_INFO	  *m_pastStoreImageInfo[CAMERA_MAX_NUM];	//�ɼ�ͼ����Ϣ�ṹ�壨�洢���ͣ�
static LST_IMAGE_INFO	  *m_pastDisplayImageInfo[CAMERA_MAX_NUM];	//�ɼ�ͼ����Ϣ�ṹ�壨��ʾ���ͣ�
static LST_IMAGE_INFO     *m_pastSnapImageInfo[SNAP_BUF_NUM];		//Snap��֡ͼ����ʱ��ͼ�ṹ�壬TODO:���ﲻӦ��SNAP_BUF_NUM��Ӧ��CAMERA_MAX_NUM
static LST_RANGER3XML_INFO *m_pastRanger3XMLInfo[CAMERA_MAX_NUM];   //Ranger3 xml�ļ���Ϣ�ṹ��

static int m_iPushStoreBufferIndex[CAMERA_MAX_NUM];		//��ǰBuffer�������洢Buffer��Camera��Buffer
static int m_iPopStoreBufferIndex[CAMERA_MAX_NUM];		//��ǰBuffer�������洢Buffer��Buffer��CallBack
static int m_iPushDisplayBufferIndex[CAMERA_MAX_NUM];	//��ǰBuffer��������ʾBuffer��Camera��Buffer
static int m_iPopDisplayBufferIndex[CAMERA_MAX_NUM];	//��ǰBuffer��������ʾBuffer��Buffer��CallBack

/***********************************************************************
*				      ״̬ģ��
***********************************************************************/
static BOOL m_bDetected = FALSE;									//�Ƿ��Ѿ���ѯ���
static BOOL m_bOpened[MAX_CAMERA_NUM] = { FALSE };					//�Ƿ��Ѿ������
static BOOL m_bInited[MAX_CAMERA_NUM] = { FALSE };					//�Ƿ��Ѿ���ʼ�����
static BOOL m_bGrabbed[MAX_CAMERA_NUM] = { FALSE };					//�Ƿ��Ѿ���ʼ�ɼ�



static BOOL m_bIsSensor[MAX_CAMERA_NUM] = { FALSE };                //�Ƿ���sensorģʽ sensorģʽ�����ʱû�����ã���������пͻ���Ҫ��������
static BOOL m_bIsCalibration[MAX_CAMERA_NUM] = { FALSE };           //�Ƿ��Ǳ궨������
static BOOL m_bHasReflectance[MAX_CAMERA_NUM] = { FALSE };          //�����Ƿ񺬻Ҷ�ֵ


static unsigned long m_RunRate[MAX_CAMERA_NUM];                     //��ѯȡͼʱ����
//ranger3����;
//std::string ctiFile = Sample::getPathToProducer();
//Sample::Consumer consumer(ctiFile);

//static Sample::InterfaceList interfaces;//�ӿ��б�
//static Sample::DeviceList devices;//����豸�б�
//static GenTL::TL_HANDLE tlHandle;//������
//static GenTLApi* tl;
//static Sample::DeviceId deviceId;//�豸ID
//std::vector<std::pair<GenTL::IF_HANDLE, std::string>> link_id;//�ӿھ�����豸����
//std::set<GenTL::IF_HANDLE> openInterfaces;//�ӿھ��
//std::vector<GenApi::CNodeMapRef> interfaceNodeVec;

bool b_image2D = false;

int64_t BufferSize[CAMERA_MAX_NUM];//����buffer��С
char* pBufferOfRange3[CAMERA_MAX_NUM];//����buffer

//ȫ�ֺ�������
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
//ranger3ȫ�ֺ���;
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
*				       ���SDK�������ͳһ�ӿ�ʵ��
*
***********************************************************************/

LE_STATUS __stdcall LCC_C_DetectCameraNum(LST_CAMERA_INFO **pstCameraInfo, int *piCameraNum)
{
	if ((pstCameraInfo == NULL) || (piCameraNum == NULL)) return L_ERR_CCPARV;	//��Ч�Ĳ���ֵ
	*piCameraNum = 0;//ȫ�����List��������
	
	if (m_pRanger3Share == nullptr)
	{
		m_pRanger3Share = std::make_shared<R3S>("");
	}
	
	//��顰SICKGigEVisionTL.cti���ļ��Ƿ����
	if (!m_pRanger3Share->isCtiFound())
	{
		return L_ERR_CCWLPI;//�޷����ز���������������󣬻�ȱʧ�����ļ�
	}

	//�������
	m_pRanger3Share->scanDevice();
	
	//ȡ����������б�
	auto devices = m_pRanger3Share->getConDevList();
		

	//��սṹ�壨���У�
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
		//����������Ranger3����豸����ʼ�����ʵ��
		for (auto device : devices)
		{
			std::string deviceName = device.first;
			if ((deviceName.find(firstName) == deviceName.npos) ||
				(deviceName.find(endName) == deviceName.npos))
			{
				continue;
			}

			//��ʼ���ɼ���
			std::string camMac = device.second->getMac();
			camMac.erase(std::remove(camMac.begin(), camMac.end(), ':'), camMac.end());		

			//��ʼ�������Ϣ�ṹ
			m_pastCameraInfo[camIndex] = new LST_CAMERA_INFO();
			//�����������
			m_pastCameraInfo[camIndex]->iCameraIndex = camIndex;

			std::string deviceid = std::to_string(device.second->mId);
			strcpy_s(m_pastCameraInfo[camIndex]->achCameraID, deviceid.c_str());
			//д�����ID
			m_pastCameraInfo[camIndex]->achSdkVersion[0] = EMPTY_STRING;
			//д������ӿڣ�Ranger3���Ϊ����
			m_pastCameraInfo[camIndex]->iCameraInterface = CIT_GIGE;
			m_pastCameraInfo[camIndex]->achSerialNumber[0] = EMPTY_STRING;			
			//д�����mac��ַ
			//std::string camMac = device.second->getMac();
			strcpy_s(m_pastCameraInfo[camIndex]->achReserved, camMac.c_str());
						
			//д�������Ϣ;���к�
			//device.second->mInterfaceNodeMap._GetNode("DeviceSerialNumber");
			/*std::string number = device.second->getMac();
			strcpy_s(m_pastCameraInfo[i]->achSerialNumber, number.c_str());*/
			pstCameraInfo[camIndex] = m_pastCameraInfo[camIndex];
			++camIndex;

			//�ж�����������
			if (camIndex > MAX_CAMERA_NUM)
			{
				*piCameraNum = -1;
				return L_ERR_CCTM;   //�����˲ɼ��豸��������Ŀ
			}
		}

		*piCameraNum = camIndex;//�������
	}
	catch (const std::exception& e)
	{
		return L_ERR_UNKNOWN;
	}
	
	//ȫ�ֱ�����ֵ-�������
	m_iCameraNum = *piCameraNum;

	if (m_iCameraNum == 0)
	{
		return L_ERR_CCU;//δ��������������Ϣ
	}

	//�Ѽ��״̬
	m_bDetected = TRUE;

	return L_OK;

}
#if 1

LE_STATUS __stdcall LCC_C_OpenCamera(int iCameraIndex, const char *pchXmlFileName, void **pCameraHandle)
{
	//�����������ж�
	if (iCameraIndex < 0 || iCameraIndex >= CAMERA_MAX_NUM || iCameraIndex >= m_iCameraNum)
	{
		return L_ERR_CCFWIDX;	//��Ч���������
	}
	
	if (!m_bDetected) return L_ERR_CCU;	//δ��������������Ϣ
	if (m_bOpened[iCameraIndex]) return L_ERR_CCOED;//����Ѿ���

	//�Բ������ƽڵ����
	if (m_poTreeNodeRoot[iCameraIndex] != NULL)
	{
		delete m_poTreeNodeRoot[iCameraIndex];
		m_poTreeNodeRoot[iCameraIndex] = NULL;
	}

	//����xml������������ļ�
	if (!LoadCameraXml(iCameraIndex, pchXmlFileName))
	{
		return L_ERR_CCXML;	 //XML�����ļ����ܴ�
	}

	m_paoRanger3s[iCameraIndex] = new SickCam::Ranger3(m_pRanger3Share, m_pastCameraInfo[iCameraIndex]->achReserved, false);

	//�������
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
		m_paoRanger3s[iCameraIndex]->setScaleY(0.02);//����Y���� 	
		/*m_paoRanger3s[iCameraIndex]->setParametPath(para_csv_path);
		m_paoRanger3s[iCameraIndex]->setCalibraPath(calib_xml_path);*/
		//m_paoRanger3s[iCameraIndex]->setRectMethos(SiCaliWrapper::RectMethod::Top);
		m_paoRanger3s[iCameraIndex]->setDoRectify(false);
		//m_paoRanger3s[iCameraIndex]->setTimeOut(10);
		m_paoRanger3s[iCameraIndex]->setMissingData(0);
		//m_paoRanger3s[iCameraIndex]->setBufferCount(20);

		std::string hasReflectance = m_paoRanger3s[iCameraIndex]->getParameterValue("ComponentEnable_ComponentSelector_Reflectance_RegionSelector_Scan3dExtraction1");//�ж��Ƿ񺬻Ҷ�ֵ
		m_bHasReflectance[iCameraIndex] = std::stoi(hasReflectance);
		std::string isCalibration = m_paoRanger3s[iCameraIndex]->getCalibraPath();//�ж��Ƿ��Ǳ궨����
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

	//�����ɼ��߳�
	//if (isOk && m_paoImageGrabers[iCameraIndex]->isImageGraberThreadRunning==false)
	//{
	//	//m_paoImageGrabers[iCameraIndex]->getR3()->setTimeOut(1000);

	//	std::thread* thread_pImgGabber = new std::thread(&ImageGabber::run, m_paoImageGrabers[iCameraIndex]);
	//	thread_pImgGabber->detach();
	//	delete thread_pImgGabber;
	//	thread_pImgGabber = nullptr;
	//}
	//		

	*pCameraHandle = &(m_iCamIndex[iCameraIndex]);//����m_paoImageGrabers[iCameraIndex]->getR3()->getDeviceHandle();
	m_pCamHandle[iCameraIndex] = &(m_iCamIndex[iCameraIndex]);


	//�Ѵ�״̬
	m_bOpened[iCameraIndex] = TRUE;
	m_iCameraNumOpen++;

	return L_OK;
}

LE_STATUS __stdcall LCC_C_InitCamera(void *pCameraHandle, LST_BUFFER_INFO stBufferInfo)
{
	//���������õ��������
	int iCameraIndex = GetIndexFromHandle(pCameraHandle);
	if (iCameraIndex < 0 || iCameraIndex >= CAMERA_MAX_NUM)
	{
		return L_ERR_CCWH;	//�����Ч
	}

	if (!m_bDetected) return L_ERR_CCU;	//δ��������������Ϣ
	if (!m_bOpened[iCameraIndex]) return L_ERR_CCNOED;	//���δ�򿪣�û������ͼ��ɼ��豸
	if (m_bInited[iCameraIndex]) return L_ERR_CCIED;	//����Ѿ���ʼ��

	/*******************************************
	��ʼ��
	*******************************************/

	//������ղ������洢buffer��LST_IMAGE_INFO
	if (m_pastStoreImageInfo[iCameraIndex] != NULL)
	{
		FreeNodeBuffer(m_pastStoreImageInfo[iCameraIndex], stBufferInfo.iBufferNum);
		m_pastStoreImageInfo[iCameraIndex] = NULL;
	}

	//������ղ�������ʾbuffer, LST_IMAGE_INFO
	if (m_pastDisplayImageInfo[iCameraIndex] != NULL)
	{
		FreeNodeBuffer(m_pastDisplayImageInfo[iCameraIndex], DISPLAY_MAX_BUFF);
		m_pastDisplayImageInfo[iCameraIndex] = NULL;
	}

	//������ղ�������֡buffer, LST_IMAGE_INFO
	if (m_pastSnapImageInfo[0] != NULL)
	{
		FreeNodeBuffer(m_pastSnapImageInfo[0], 1);//ֻҪ1��buffer
		m_pastSnapImageInfo[0] = NULL;
	}
	//������ղ�����Ranger3XML�ļ���Ϣ
	if (m_pastRanger3XMLInfo[iCameraIndex] != NULL)
	{
		delete m_pastRanger3XMLInfo[iCameraIndex];
		m_pastRanger3XMLInfo[iCameraIndex] = NULL;
	}
	//������ղ�����ͼ�����Խṹ��
	if (m_pastImageProperty[iCameraIndex] != NULL)
	{
		delete m_pastImageProperty[iCameraIndex];
		m_pastImageProperty[iCameraIndex] = NULL;
	}

	//������ղ�����������Ϣ�ṹ��
	if (m_pastBufferInfo[iCameraIndex] != NULL)
	{
		delete m_pastBufferInfo[iCameraIndex];
		m_pastBufferInfo[iCameraIndex] = NULL;
	}

	//�߳̾����ʼ��
	m_ThreadPushHandle[iCameraIndex] = NULL;
	m_ThreadPopStoreHandle[iCameraIndex] = NULL;
	m_ThreadPopDisplayHandle[iCameraIndex] = NULL;

	//����������ʼ��
	m_iPushStoreBufferIndex[iCameraIndex] = -1;
	m_iPopStoreBufferIndex[iCameraIndex] = -1;
	m_iPushDisplayBufferIndex[iCameraIndex] = -1;
	m_iPopDisplayBufferIndex[iCameraIndex] = -1;

	//CallBackָ���ʼ��
	m_pStoreCallBack[iCameraIndex] = NULL;			//�洢���͵Ļص�ָ��
	m_pMessageCallBack[iCameraIndex] = NULL;		//��Ϣ���͵Ļص�ָ��
	m_pDisplayCallBack[iCameraIndex] = NULL;		//��ʾ���͵Ļص�ָ��

	//CallBack�û������ĳ�ʼ��
	m_pStoreUserContext[iCameraIndex] = NULL;		//�洢���͵��û�������
	m_pDisplayUserContext[iCameraIndex] = NULL;		//��Ϣ���͵��û�������
	m_pMessageUserContext[iCameraIndex] = NULL;		//��ʾ���͵��û�������

	//������ʼ��
	m_iThreadID[iCameraIndex] = -1;		//�߳�ID��ʼ��
	m_iShowFps[iCameraIndex] = 15;		//��ʾ֡Ƶ��ʼ��
	m_dCurFps[iCameraIndex] = 0;		//��ǰ֡Ƶ��ʼ��
	m_bIsStopReceive[iCameraIndex] = true;			//״̬�������Ƿ�ֹͣ��ȡͼ��ThreadReceiveImage
	m_bIsStopStore[iCameraIndex] = true;			//״̬�������Ƿ�ֹͣ�洢ͼ��ThreadStoreImage
	m_bIsStopDisplay[iCameraIndex] = true;			//״̬�������Ƿ�ֹͣ��ʾͼ��ThreadDisplayImage

	m_iSnapWaitTime[iCameraIndex] = 1000;           //��֡��ͼ�ȴ�ʱ���ʼֵ1000

	/*******************************************
	��ȡͼ������
	*******************************************/
	LE_STATUS eResult;
	LST_BOOLEAN_INFO stBooleanInfo;

	////��ȡ���bufferģʽ��true��Range+Reflectance��false��Range
	//eResult = LCC_C_GetFeatureBoolean(m_paoConnection[iCameraIndex]->mDeviceHandle, LFNC_SICK_REFLECTANCE_ENABLE, &stBooleanInfo);
	//
	//�洢buffer��Ϣ�ṹ��
	m_pastBufferInfo[iCameraIndex] = new LST_BUFFER_INFO();
	m_pastBufferInfo[iCameraIndex]->iBufferNum = stBufferInfo.iBufferNum;
	if (stBufferInfo.iMergeFrameNum < 1 || stBufferInfo.iMergeFrameNum > 100000)	//�жϳ���
	{
		m_pastBufferInfo[iCameraIndex]->iMergeFrameNum = 1;
	}
	else
	{
		m_pastBufferInfo[iCameraIndex]->iMergeFrameNum = stBufferInfo.iMergeFrameNum;
	}
	strcpy_s(m_pastBufferInfo[iCameraIndex]->achReserved, stBufferInfo.achReserved);

	//��ʼ��Buffer
	eResult = InitLusterBuffer(iCameraIndex, &stBufferInfo);

	//�ѳ�ʼ��״̬
	m_bInited[iCameraIndex] = TRUE;

	return L_OK;
}

LE_STATUS __stdcall LCC_C_CloseCamera(void *pCameraHandle)
{
	//���������õ��������
	int iCameraIndex = GetIndexFromHandle(pCameraHandle);
	if (iCameraIndex < 0 || iCameraIndex >= CAMERA_MAX_NUM)
	{
		return L_ERR_CCWH;	//�����Ч
	}

	if (!m_bOpened[iCameraIndex]) return L_ERR_CCNOED;	//���δ�򿪣�û������ͼ��ɼ��豸
	if (m_bGrabbed[iCameraIndex]) return L_ERR_CCGED;	//����Ѿ��ɼ�

	//����ѳ�ʼ��״̬�����ͷ�״̬
	if (m_bInited[iCameraIndex])
	{
		LE_STATUS eStatus = LCC_C_ReleaseCamera(pCameraHandle);
		if (eStatus != L_OK) return eStatus;
	}

	//�ͷ�XmlTreeNode
	if (m_poTreeNodeRoot[iCameraIndex] != nullptr)
	{
		delete m_poTreeNodeRoot[iCameraIndex];
		m_poTreeNodeRoot[iCameraIndex] = nullptr;
	}

	//������Handle
	if (m_pCamHandle[iCameraIndex] != nullptr)
	{
		m_pCamHandle[iCameraIndex] = nullptr;
	}


	//�ͷ����������
	if (m_paoRanger3s[iCameraIndex] != nullptr)
	{
		m_paoRanger3s[iCameraIndex]->disconnectCamera();
		delete m_paoRanger3s[iCameraIndex];
		m_paoRanger3s[iCameraIndex] = nullptr;
	}


	//δ��״̬
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
	//���������õ��������
	int iCameraIndex = GetIndexFromHandle(pCameraHandle);
	if (iCameraIndex < 0 || iCameraIndex >= CAMERA_MAX_NUM)
	{
		return L_ERR_CCWH;	//�����Ч
	}

	if (!m_bDetected) return L_ERR_CCU;	//δ��������������Ϣ
	if (!m_bOpened[iCameraIndex]) return L_ERR_CCNOED;	//���δ�򿪣�û������ͼ��ɼ��豸
	if (!m_bInited[iCameraIndex]) return L_ERR_CCNIED;	//���δ��ʼ��
	if (m_bGrabbed[iCameraIndex]) return L_ERR_CCGED;	//����Ѿ��ɼ�

	//״̬����
	m_bIsStopReceive[iCameraIndex] = false;
	m_bIsStopStore[iCameraIndex] = false;
	m_bIsStopDisplay[iCameraIndex] = false;

	//��ʼ�ɼ�
	if (m_paoRanger3s[iCameraIndex] != nullptr)
	{
		m_RunRate[iCameraIndex] = 5;// _runRate(iCameraIndex);
		Ecode ec = m_paoRanger3s[iCameraIndex]->startCamera();

		if (ec != Ecode::All_OK)
		{
			return L_ERR_UNKNOWN;
		}

	}
	//��������ͼ���߳�
	m_iThreadID[iCameraIndex] = iCameraIndex;
	m_ThreadPushHandle[iCameraIndex] = ::CreateThread(NULL, NULL, ThreadReceiveImage, &m_iThreadID[iCameraIndex], NULL, NULL);

	//������ȡͼ���̣߳��洢���棩
	if (m_pStoreCallBack[iCameraIndex] != NULL)
	{
		m_ThreadPopStoreHandle[iCameraIndex] = ::CreateThread(NULL, NULL, ThreadStoreImage, &m_iThreadID[iCameraIndex], NULL, NULL);
	}

	//������ȡͼ���̣߳���ʾ���棩
	if (m_pDisplayCallBack[iCameraIndex] != NULL)
	{
		m_ThreadPopDisplayHandle[iCameraIndex] = ::CreateThread(NULL, NULL, ThreadDisplayImage, &m_iThreadID[iCameraIndex], NULL, NULL);
	}

	//�Ѳɼ�״̬
	m_bGrabbed[iCameraIndex] = TRUE;

	Sleep(100);//������ɼ�ʱ����������ɼ�����ʵ��ʱ������ʱ  ����

	return L_OK;
}

LE_STATUS __stdcall LCC_C_StopGrab(void *pCameraHandle)
{
	//���������õ��������
	int iCameraIndex = GetIndexFromHandle(pCameraHandle);
	if (iCameraIndex < 0 || iCameraIndex >= CAMERA_MAX_NUM)
	{
		return L_ERR_CCWH;	//�����Ч
	}

	if (!m_bGrabbed[iCameraIndex]) return L_ERR_CCNGED;	//���δ�ɼ�

	

	//����״̬
	m_bIsStopReceive[iCameraIndex] = true;
	m_bIsStopStore[iCameraIndex] = true;
	m_bIsStopDisplay[iCameraIndex] = true;

	//ֹͣ�ɼ�
	if (m_paoRanger3s[iCameraIndex] != nullptr)
	{
		Ecode ec = m_paoRanger3s[iCameraIndex]->stopCamera();
		if (ec = SickCam::All_OK)
		{
			return L_ERR_UNKNOWN;
		}
	}

	//����������ʼ��
	m_iPushStoreBufferIndex[iCameraIndex] = -1;
	m_iPopStoreBufferIndex[iCameraIndex] = -1;
	m_iPushDisplayBufferIndex[iCameraIndex] = -1;
	m_iPopDisplayBufferIndex[iCameraIndex] = -1;

	//�ͷ����е�Buffer�ı�ʶ�ź�
	for (int i = 0; i < m_pastBufferInfo[iCameraIndex]->iBufferNum; i++)
	{
		m_pastStoreImageInfo[iCameraIndex][i].achReserved[INDEX_SEMAPHORE] = FALSE;
	}
	for (int i = 0; i < DISPLAY_MAX_BUFF; i++)
	{
		m_pastDisplayImageInfo[iCameraIndex][i].achReserved[INDEX_SEMAPHORE] = FALSE;
	}

	//δ�ɼ�״̬
	m_bGrabbed[iCameraIndex] = FALSE;

	return L_OK;
}

LE_STATUS __stdcall LCC_C_ReleaseCamera(void *pCameraHandle)
{
	//���������õ��������
	int iCameraIndex = GetIndexFromHandle(pCameraHandle);
	if (iCameraIndex < 0 || iCameraIndex >= CAMERA_MAX_NUM)
	{
		return L_ERR_CCWH;	//�����Ч
	}

	if (m_bGrabbed[iCameraIndex]) return L_ERR_CCGED;	//����Ѿ��ɼ�
	if (!m_bInited[iCameraIndex]) return L_ERR_CCNIED;	//���δ��ʼ��

	//���buffer
	LE_STATUS eResult = ReleaseLusterBuffer(iCameraIndex);

	//������ղ�����������Ϣ�ṹ��
	if (m_pastBufferInfo[iCameraIndex] != NULL)
	{
		delete m_pastBufferInfo[iCameraIndex];
		m_pastBufferInfo[iCameraIndex] = NULL;
	}

	//�߳̾����ʼ��
	m_ThreadPushHandle[iCameraIndex] = NULL;
	m_ThreadPopStoreHandle[iCameraIndex] = NULL;
	m_ThreadPopDisplayHandle[iCameraIndex] = NULL;

	//CallBackָ���ʼ��
	m_pStoreCallBack[iCameraIndex] = NULL;			//�洢���͵Ļص�ָ��
	m_pMessageCallBack[iCameraIndex] = NULL;		//��Ϣ���͵Ļص�ָ��
	m_pDisplayCallBack[iCameraIndex] = NULL;		//��ʾ���͵Ļص�ָ��

	//CallBack�û������ĳ�ʼ��
	m_pStoreUserContext[iCameraIndex] = NULL;		//�洢���͵��û�������
	m_pDisplayUserContext[iCameraIndex] = NULL;		//��Ϣ���͵��û�������
	m_pMessageUserContext[iCameraIndex] = NULL;		//��ʾ���͵��û�������

	//������ʼ��
	m_iThreadID[iCameraIndex] = -1;		//�߳�ID��ʼ��
	//m_iRecordMode[iCameraIndex] = 0;	//�洢���Ƴ�ʼ��
	m_iShowFps[iCameraIndex] = 15;		//��ʾ֡Ƶ��ʼ��
	m_dCurFps[iCameraIndex] = 0;		//��ǰ֡Ƶ��ʼ��
	m_bIsStopReceive[iCameraIndex] = true;			//״̬�������Ƿ�ֹͣ��ȡͼ��ThreadReceiveImage
	m_bIsStopStore[iCameraIndex] = true;			//״̬�������Ƿ�ֹͣ�洢ͼ��ThreadStoreImage
	m_bIsStopDisplay[iCameraIndex] = true;			//״̬�������Ƿ�ֹͣ��ʾͼ��ThreadDisplayImage

	//δ��ʼ��״̬
	m_bInited[iCameraIndex] = FALSE;

	return L_OK;
}

LE_STATUS __stdcall LCC_C_SnapImage(void *pCameraHandle)
{
	//���������õ��������
	int iCameraIndex = GetIndexFromHandle(pCameraHandle);
	if (iCameraIndex < 0 || iCameraIndex >= CAMERA_MAX_NUM)
	{
		return L_ERR_CCWH;	//�����Ч
	}

	if (!m_bDetected) return L_ERR_CCU;	//δ��������������Ϣ
	if (!m_bOpened[iCameraIndex]) return L_ERR_CCNOED;	//���δ�򿪣�û������ͼ��ɼ��豸
	if (!m_bInited[iCameraIndex]) return L_ERR_CCNIED;	//���δ��ʼ��
	if (m_bGrabbed[iCameraIndex]) return L_ERR_CCGED;	//����Ѿ��ɼ�

	//�Ѳɼ�״̬
	m_bGrabbed[iCameraIndex] = TRUE;



	return L_OK;
}

LE_STATUS __stdcall LCC_C_RegisterCallback(LCC_C_Callback pCallback, int iCallbackType, void *pCameraHandle, void *pUserContext)
{
	//���������õ��������
	int iCameraIndex = GetIndexFromHandle(pCameraHandle);
	if (iCameraIndex < 0 || iCameraIndex >= CAMERA_MAX_NUM)
	{
		return L_ERR_CCWH;	//�����Ч
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
		//m_iShowFps = iShowFps;//���ֵ���鲢�����������������ȥ
		break;
	case CB_MESSAGE:
		m_pMessageCallBack[iCameraIndex] = pCallback;
		m_pMessageUserContext[iCameraIndex] = pUserContext;
		break;
	default:
		return L_ERR_CCCALLBACK;	//��֧�ֵĻص�����
	}
	return L_OK;
}

LE_STATUS __stdcall LCC_C_UnregisterCallback(int iCallbackType, void *pCameraHandle)
{
	//���������õ��������
	int iCameraIndex = GetIndexFromHandle(pCameraHandle);
	if (iCameraIndex < 0 || iCameraIndex >= CAMERA_MAX_NUM)
	{
		return L_ERR_CCWH;	//�����Ч
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
		//m_iShowFps[iCameraIndex] = 15; //���ֵ���鲢�����������������ȥ
		break;
	case CB_MESSAGE:
		m_pMessageCallBack[iCameraIndex] = nullptr;
		m_pMessageUserContext[iCameraIndex] = nullptr;
		break;
	default:
		return L_ERR_CCCALLBACK;	//��֧�ֵĻص�����
	}
	return L_OK;
}

LE_STATUS __stdcall LCC_C_GetFeatureInfo(void *pCameraHandle, const char *pchfeatureName, LST_FEATURE_INFO *stFeatureInfo)
{
	//���������õ��������
	int iCameraIndex = GetIndexFromHandle(pCameraHandle);
	if (iCameraIndex < 0 || iCameraIndex >= CAMERA_MAX_NUM)
	{
		return L_ERR_CCWH;	//�����Ч
	}

	//��XML������������ĵ��У���ȡ���������Ϣ�ṹ��
	if (!GetFeatureInfoFromXML(iCameraIndex, pchfeatureName, *stFeatureInfo))
	{
		stFeatureInfo->eType = FEATURE_TYPE_UNKNOW;
		return L_ERR_CCPARAM;		//��֧�ֵĲ���
	}

	return L_OK;
}

LE_STATUS __stdcall LCC_C_GetFeatureInt(void *pCameraHandle, const char *pchfeatureName, LST_INT_INFO *stFeatureValue)
{
	//���������õ��������
	int iCameraIndex = GetIndexFromHandle(pCameraHandle);
	if (iCameraIndex < 0 || iCameraIndex >= CAMERA_MAX_NUM)
	{
		return L_ERR_CCWH;	//�����Ч
	}

	//if (pchfeatureName == LFNC_SICK_BUFFERCOUNT)
	//{
	//	stFeatureValue->iValue = m_paoRanger3s[iCameraIndex]->getBufferCount();
	//	return L_OK;
	//}

	//�жϲ��������Ƿ���ȷ
	if (!IsFeatureTypeRight(iCameraIndex, pchfeatureName, TYPE_INT))
	{
		return L_ERR_CCPART;	//��Ч�Ĳ�������
	}

	//�������������������ƣ�LFNC��
	char pchfeatureName_Lfnc[255];
	strcpy_s(pchfeatureName_Lfnc, pchfeatureName);

	//��ת����������̵Ĳ������ƣ�EBUS��
	char achfeatureName_Vendor[255];
	memset(achfeatureName_Vendor, 0x00, 255);

	//ת�����ƣ���LFNCת��ΪEBUS
	if (!ConvertLfncToVendor(iCameraIndex, pchfeatureName_Lfnc, achfeatureName_Vendor))
	{
		return L_ERR_CCRXFVF;//��ȡXML����ֵʧ��
	}

	//��ò���ֵ
	std::string value = m_paoRanger3s[iCameraIndex]->getParameterValue(achfeatureName_Vendor);

	stFeatureValue->iValue = std::stoi(value);

	std::string minValue, maxValue, incValue;
	if (m_paoRanger3s[iCameraIndex]->getParameterValueConstraint(achfeatureName_Vendor,
		minValue, maxValue, incValue))
	{
		//��ò������ֵ
		stFeatureValue->iMax = std::stoi(minValue);

		//��ò�����Сֵ
		stFeatureValue->iMin = std::stoi(maxValue);

		//��ò�������ֵ
		stFeatureValue->iInc = std::stoi(incValue);
	}


	return L_OK;
}

LE_STATUS __stdcall LCC_C_GetFeatureBoolean(void *pCameraHandle, const char *pchfeatureName, LST_BOOLEAN_INFO *stFeatureValue)
{
	//���������õ��������
	int iCameraIndex = GetIndexFromHandle(pCameraHandle);
	if (iCameraIndex < 0 || iCameraIndex >= CAMERA_MAX_NUM)
	{
		return L_ERR_CCWH;	//�����Ч
	}

	return L_ERR_CCPARNA;

	/*if (pchfeatureName == LFNC_SICK_DORECTIFY)
	{
		stFeatureValue->bValue = m_paoRanger3s[iCameraIndex]->getDoRectify();
		return L_OK;
	}*/

	//�жϲ��������Ƿ���ȷ
	if (!IsFeatureTypeRight(iCameraIndex, pchfeatureName, TYPE_BOOLEAN))
	{
		return L_ERR_CCPART;  //��Ч�Ĳ�������
	}

	//�������������������ƣ�LFNC��
	char pchfeatureName_Lfnc[255];
	strcpy_s(pchfeatureName_Lfnc, pchfeatureName);

	//��ת����������̵Ĳ������ƣ�EBUS��
	char achfeatureName_Vendor[255];
	memset(achfeatureName_Vendor, 0x00, 255);

	//ת�����ƣ���LFNCת��ΪEBUS
	if (!ConvertLfncToVendor(iCameraIndex, pchfeatureName_Lfnc, achfeatureName_Vendor))
	{
		return L_ERR_CCRXFVF;//��ȡXML����ֵʧ��
	}

	std::string value = m_paoRanger3s[iCameraIndex]->getParameterValue(achfeatureName_Vendor);

	stFeatureValue->bValue = std::stoi(value);

#if 0
	//��ͼģʽ
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
	//���������õ��������
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
	//�жϲ��������Ƿ���ȷ
	if (!IsFeatureTypeRight(iCameraIndex, pchfeatureName, TYPE_DOUBLE))
	{
		return L_ERR_CCPART;	//��Ч�Ĳ�������
	}

	//�������������������ƣ�LFNC��
	char pchfeatureName_Lfnc[255];
	strcpy_s(pchfeatureName_Lfnc, pchfeatureName);

	//��ת����������̵Ĳ������ƣ�EBUS��
	char achfeatureName_Vendor[255];
	memset(achfeatureName_Vendor, 0x00, 255);

	//ת�����ƣ���LFNCת��ΪEBUS
	if (!ConvertLfncToVendor(iCameraIndex, pchfeatureName_Lfnc, achfeatureName_Vendor))
	{
		return L_ERR_UNKNOWN;
	}

	//��ò���ֵ
	std::string value = m_paoRanger3s[iCameraIndex]->getParameterValue(achfeatureName_Vendor);

	stFeatureValue->dValue = std::stod(value);

	std::string minV, maxV, incV;
	if (m_paoRanger3s[iCameraIndex]->getParameterValueConstraint(achfeatureName_Vendor,
		minV,
		maxV,
		incV))
	{
		//��ò������ֵ
		stFeatureValue->dMax = std::stod(minV);

		//��ò�����Сֵ
		stFeatureValue->dMin = std::stod(maxV);
	}

	return L_OK;
}

LE_STATUS __stdcall LCC_C_GetFeatureString(void *pCameraHandle, const char *pchfeatureName, LST_STRING_INFO *stFeatureValue)
{
	//���������õ��������
	int iCameraIndex = GetIndexFromHandle(pCameraHandle);
	if (iCameraIndex < 0 || iCameraIndex >= CAMERA_MAX_NUM)
	{
		return L_ERR_CCWH;	//�����Ч
	}
	//��ʼ���ɼ���
	
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

	//�жϲ��������Ƿ���ȷ
	if (!IsFeatureTypeRight(iCameraIndex, pchfeatureName, TYPE_STRING))
	{
		return L_ERR_CCPART;  //��Ч�Ĳ�������
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
	//���������õ��������
	int iCameraIndex = GetIndexFromHandle(pCameraHandle);
	if (iCameraIndex < 0 || iCameraIndex >= CAMERA_MAX_NUM)
	{
		return L_ERR_CCWH;	//�����Ч
	}

	return L_ERR_CCPARNA;
#if 0
	//����ģʽ��ȡ
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

	//���ڴ�С
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
	//�жϲ��������Ƿ���ȷ
	if (!IsFeatureTypeRight(iCameraIndex, pchfeatureName, TYPE_ENUM))
	{
		return L_ERR_CCPART;	//��Ч�Ĳ�������
	}

	//�������������������ƣ�LFNC��
	char pchfeatureName_Lfnc[255];
	strcpy_s(pchfeatureName_Lfnc, pchfeatureName);

	//��ת����������̵Ĳ������ƣ�EBUS��
	char achfeatureName_Vendor[255];
	memset(achfeatureName_Vendor, 0x00, 255);

	//ת�����ƣ���LFNCת��ΪEBUS
	if (!ConvertLfncToVendor(iCameraIndex, pchfeatureName_Lfnc, achfeatureName_Vendor))
	{
		return L_ERR_UNKNOWN;
	}

	//��ò���ֵ
	std::string value = m_paoRanger3s[iCameraIndex]->getParameterValue(achfeatureName_Vendor);

	stFeatureValue->iValue = std::stoi(value);

	return L_OK;
}

LE_STATUS __stdcall LCC_C_GetFeatureUncertain(void *pCameraHandle, const char *pchfeatureName, void **pFeatureValue)
{
	//���������õ��������
	int iCameraIndex = GetIndexFromHandle(pCameraHandle);
	if (iCameraIndex < 0 || iCameraIndex >= CAMERA_MAX_NUM)
	{
		return L_ERR_CCWH;	//�����Ч
	}

	return L_ERR_CCPARNA;

	//�ж�����
	if (strcmp(pchfeatureName, LFNC_GET_STORE_IMAGE) == 0)//��ȡ�����е�һ֡ͼ��
	{
		LST_IMAGE_INFO **stImageInfo = (LST_IMAGE_INFO**)pFeatureValue;

		int iLoopCount = m_iSnapWaitTime[iCameraIndex] / 10;
		for (int i = 0; i < iLoopCount; i++)
		{
			if (m_pastStoreImageInfo[iCameraIndex] == NULL)
			{
				return L_ERR_CCCL;	//ʵ����Ч�������Ѿ�ע����
			}

			if (m_pastStoreImageInfo[iCameraIndex][m_iPushStoreBufferIndex[iCameraIndex]].achReserved[INDEX_SEMAPHORE] == TRUE)
			{
				*stImageInfo = &(m_pastStoreImageInfo[iCameraIndex][m_iPushStoreBufferIndex[iCameraIndex]]);
				return L_OK;
			}
			Sleep(10);
		}
		return L_ERR_CCTIMEOUT;  //��ʱ
	}
	else if (strcmp(pchfeatureName, LFNC_GET_NODE_NUM) == 0)//�����������
	{
		int **pNum = (int**)(pFeatureValue);
		int iFeatureNum = 0;

		//��ȡ�����������
		iFeatureNum = GetAllNodeNum(m_poTreeNodeRoot[iCameraIndex]);

		//��ֵ
		**pNum = iFeatureNum;
	}
	else if (strcmp(pchfeatureName, LFNC_GET_NODE_INFO) == 0)
	{
		//����
		LST_NODE_INFO **pastFni = (LST_NODE_INFO **)pFeatureValue;
		//LST_NODE_INFO ptempNoInfo;
		int iFeatureIndex = 0;		//ȫ�����������������0��ʼ

		//��ȡ���нڵ���Ϣ
		GetAllNodeInfo(m_poTreeNodeRoot[iCameraIndex], *pastFni, iFeatureIndex);
	}
	else if (strcmp(pchfeatureName, LFNC_GET_IMAGE_PROPERTY) == 0)
	{
		LST_IMAGE_PROPERTY**pastPrp = (LST_IMAGE_PROPERTY**)pFeatureValue;
		if (m_pastImageProperty[iCameraIndex] == NULL)
		{
			return L_ERR_CCCL;	//ʵ����Ч�������Ѿ�ע����
		}
		*pastPrp = m_pastImageProperty[iCameraIndex];
	}
	else
	{
		return L_ERR_CCPARAM;//�ò����ݲ�֧��
	}
	return L_OK;
}

LE_STATUS __stdcall LCC_C_SetFeatureInt(void *pCameraHandle, const char *pchfeatureName, int iFeatureValue)
{
	//���������õ��������
	int iCameraIndex = GetIndexFromHandle(pCameraHandle);
	if (iCameraIndex < 0 || iCameraIndex >= CAMERA_MAX_NUM)
	{
		return L_ERR_CCWH;	//�����Ч
	}



	SickCam::CAM_STATUS status = SickCam::All_OK;

	//�жϲ��������Ƿ���ȷ
	if (!IsFeatureTypeRight(iCameraIndex, pchfeatureName, TYPE_INT))
	{
		return L_ERR_CCPART;	//��Ч�Ĳ�������
	}

	//�������������������ƣ�LFNC��
	char pchfeatureName_Lfnc[255];
	strcpy_s(pchfeatureName_Lfnc, pchfeatureName);

	//��ת����������̵Ĳ������ƣ�EBUS��
	char achfeatureName_Vendor[255];
	memset(achfeatureName_Vendor, 0x00, 255);

	//ת�����ƣ���LFNCת��ΪEBUS
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
			LE_STATUS eResult = ReleaseLusterBuffer(iCameraIndex);//�ͷ�buffer
			if (eResult != L_OK)
			{
				return L_ERR_CCNRB;	//�޷���ʼ������
			}
			eResult = InitLusterBuffer(iCameraIndex, m_pastBufferInfo[iCameraIndex]);//��ʼ��buffer
			if (eResult != L_OK)
			{
				return L_ERR_CCNIB;	//�޷���ʼ������
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
			LE_STATUS eResult = ReleaseLusterBuffer(iCameraIndex);//�ͷ�buffer
			if (eResult != L_OK)
			{
				return L_ERR_CCNRB;	//�޷���ʼ������
			}
			eResult = InitLusterBuffer(iCameraIndex, m_pastBufferInfo[iCameraIndex]);//��ʼ��buffer
			if (eResult != L_OK)
			{
				return L_ERR_CCNIB;	//�޷���ʼ������
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
	//���������õ��������
	int iCameraIndex = GetIndexFromHandle(pCameraHandle);
	if (iCameraIndex < 0 || iCameraIndex >= CAMERA_MAX_NUM)
	{
		return L_ERR_CCWH;	//�����Ч
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
	//�жϲ��������Ƿ���ȷ
	//if (!IsFeatureTypeRight(iCameraIndex, pchfeatureName, TYPE_BOOLEAN))
	//{
	//	return L_ERR_CCPART;	//��Ч�Ĳ�������
	//}


	//�жϲ��������Ƿ���ȷ
	if (!IsFeatureTypeRight(iCameraIndex, pchfeatureName, TYPE_BOOLEAN))
	{
		return L_ERR_CCPART;	//��Ч�Ĳ�������
	}

	//�������������������ƣ�LFNC��
	char pchfeatureName_Lfnc[255];
	strcpy_s(pchfeatureName_Lfnc, pchfeatureName);

	//��ת����������̵Ĳ������ƣ�EBUS��
	char achfeatureName_Vendor[255];
	memset(achfeatureName_Vendor, 0x00, 255);

	//ת�����ƣ���LFNCת��ΪEBUS
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
	//���������õ��������
	int iCameraIndex = GetIndexFromHandle(pCameraHandle);
	if (iCameraIndex < 0 || iCameraIndex >= CAMERA_MAX_NUM)
	{
		return L_ERR_CCWH;	//�����Ч
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


	//�жϲ��������Ƿ���ȷ
	if (!IsFeatureTypeRight(iCameraIndex, pchfeatureName, TYPE_DOUBLE))
	{
		return L_ERR_CCPART;	//��Ч�Ĳ�������
	}

	//�������������������ƣ�LFNC��
	char pchfeatureName_Lfnc[255];
	strcpy_s(pchfeatureName_Lfnc, pchfeatureName);

	//��ת����������̵Ĳ������ƣ�EBUS��
	char achfeatureName_Vendor[255];
	memset(achfeatureName_Vendor, 0x00, 255);

	//ת�����ƣ���LFNCת��ΪEBUS
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
	//���������õ��������
	int iCameraIndex = GetIndexFromHandle(pCameraHandle);
	if (iCameraIndex < 0 || iCameraIndex >= CAMERA_MAX_NUM)
	{
		return L_ERR_CCWH;	//�����Ч
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
			std::string hasReflectance = m_paoRanger3s[iCameraIndex]->getParameterValue("ComponentEnable_ComponentSelector_Reflectance_RegionSelector_Scan3dExtraction1");//�ж��Ƿ񺬻Ҷ�ֵ
			m_bHasReflectance[iCameraIndex] = std::stoi(hasReflectance);
			
			LE_STATUS eResult = ReleaseLusterBuffer(iCameraIndex);//�ͷ�buffer
			if (eResult != L_OK)
			{
				return L_ERR_CCNRB;	//�޷���ʼ������
			}
			eResult = InitLusterBuffer(iCameraIndex, m_pastBufferInfo[iCameraIndex]);//��ʼ��buffer
			if (eResult != L_OK)
			{
				return L_ERR_CCNIB;	//�޷���ʼ������
			}

			if (!m_bIsCalibration[iCameraIndex])
			{
				//д��δ�궨ranger3xml�ļ�������Ϣ
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
			LE_STATUS eResult = ReleaseLusterBuffer(iCameraIndex);//�ͷ�buffer
			if (eResult != L_OK)
			{
				return L_ERR_CCNRB;	//�޷���ʼ������
			}
			eResult = InitLusterBuffer(iCameraIndex, m_pastBufferInfo[iCameraIndex]);//��ʼ��buffer
			if (eResult != L_OK)
			{
				return L_ERR_CCNIB;	//�޷���ʼ������
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
	//���������ļ�
	//if (strcmp(pchfeatureName, LFNC_SICK_CONFIGURATION) == 0)
	//{
	//	std::string filePath = pchFeatureValue;
	//	std::ifstream inputStream(filePath);

	//	//ReadCSVFlie(pCameraHandle, inputStream);//���������ļ�
	//}

	return L_OK;
}

LE_STATUS __stdcall LCC_C_SetFeatureEnum(void *pCameraHandle, const char *pchfeatureName, int iFeatureValue)
{
	//���������õ��������
	int iCameraIndex = GetIndexFromHandle(pCameraHandle);
	if (iCameraIndex < 0 || iCameraIndex >= CAMERA_MAX_NUM)
	{
		return L_ERR_CCWH;	//�����Ч
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
		return L_ERR_CCWH;//�����Ч
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
	//		return L_ERR_CCCL;//ʵ����Ч�������Ѿ�ע����
	//	}
	//	m_pastStoreImageInfo[iCameraIndex][m_iPushStoreBufferIndex[iCameraIndex]].achReserved[INDEX_SEMAPHORE] = FALSE;
	//}

	//else if (strcmp(pchfeatureName, LFNC_DESTORY_CAMERA) == 0)
	//{

	//	//�豸�����ṹ�������ڴ�
	//	for (int i = 0; i < CAMERA_MAX_NUM; i++)
	//	{
	//		if (m_pastCameraInfo[i] != NULL)
	//		{
	//			delete m_pastCameraInfo[i];
	//			m_pastCameraInfo[i] = NULL;
	//		}

	//	}
	//	////�ر����������豸
	//	//GenTL::DS_HANDLE dataStreamHandle = m_paoConnection[iCameraIndex]->mDataStreamHandle;
	//	//consumer.closeDataStream(dataStreamHandle);
	//	//consumer.closeDevice(m_paoConnection[iCameraIndex]->mDeviceHandle);

	//	//�ͷ����buffer����
	//	/*free(pBufferRangeAndReflectance[iCameraIndex]);
	//	pBufferRangeAndReflectance[iCameraIndex] = NULL;*/

	//	//�ͷ����������
	//	/*if (m_paoConnection[iCameraIndex] != NULL)
	//	{
	//	delete m_paoConnection[iCameraIndex];
	//	m_paoConnection[iCameraIndex] = NULL;
	//	}*/

	//	//��Ϊ���ӵ����һ���������ر���������ӿ�
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
*				       ��������ʵ��
*
***********************************************************************/

/******************************************************
@����˵���� ��������������������
@�������룺 pCameraHandle -- ��������
@��������� ��
@����ֵ  �� �������ֵ����0��ʼ���ɹ��򷵻ش��ڵ���0������ʧ���򷵻�С��0��������
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
@����˵���� �ͷŲɼ�ͼ����Ϣ�ṹ�壬���жԲɼ�ͼ�������ڴ���й���
@�������룺 pstImageInfo -- ��Ҫ�ͷŵĲɼ�ͼ��ṹ�塣
iBufferNum -- Buffer����
@��������� ��
@����ֵ  �� �ɹ�����true����֮����false��
******************************************************/
bool FreeNodeBuffer(LST_IMAGE_INFO *pstImageInfo, int iBufferNum)
{
	//�������ж�
	if (pstImageInfo == NULL)
	{
		return false;
	}

	//ѭ���ͷ����е�Buffer
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

	//�ͷŽṹ��
	free(pstImageInfo);
	//pstImageInfo = NULL;

	return true;
}

/******************************************************
@����˵���� �����ɼ�ͼ����Ϣ�ṹ�壬���жԲɼ�ͼ�������ڴ���й���
@�������룺 iCameraIndex -- �������
pstImageInfo -- ��Ҫ�����Ĳɼ�ͼ����Ϣ�ṹ�塣
pstImageProperty -- ͼ�����Խṹ��
pBufferInfo -- ������Ϣ�ṹ��
@��������� ��
@����ֵ  �� �ɹ�����true����֮����false��
******************************************************/
bool CreateNodeBuffer(int iCameraIndex, LST_IMAGE_INFO **pstImageInfo, LST_IMAGE_PROPERTY *pstImageProperty, LST_BUFFER_INFO *pstBufferInfo)
{
	pstImageInfo[iCameraIndex] = (LST_IMAGE_INFO*)malloc(sizeof(LST_IMAGE_INFO)*(pstBufferInfo->iBufferNum));
	if (pstImageInfo[iCameraIndex] == NULL)
	{
		return false;
	}

	//�ж�iMergeFrameNum�ĳ������
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

	//ѭ���������Buffer
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

		pstImageInfo[iCameraIndex][i].iImageFrameIndex = 0;//֡��������0��ʼ��
		pstImageInfo[iCameraIndex][i].achReserved[INDEX_SEMAPHORE] = FALSE;//����Buffer����  //TODO�������100Ӧ����ȫ���϶���һ����������

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
@����˵���� ��ȡȫ���ڵ������
@�������룺 pNode -- ����xml�ڵ�
@��������� ��
@����ֵ  �� �ɹ����ؽڵ�������ʧ�ܷ���0
******************************************************/
int GetAllNodeNum(XmlTreeNode* pNode)
{
	//�жϺ�����
	if (pNode == NULL)
	{
		return 0;
	}
	if (pNode->NodeType() == NODE_TYPE_UNKNOW)
	{
		return 0;
	}

	//����
	int iNum = 0;

	//��ȡ���е��ӽڵ�
	std::vector<XmlTreeNode*> ChildList = pNode->GetChildrenNodeList();
	if (ChildList.size() == 0)
	{
		return 0;
	}

	//�����ӽڵ�����
	iNum += (int)ChildList.size();

	//���������ӽڵ�������ӽڵ�
	for (int i = 0; i < ChildList.size(); i++)
	{
		iNum += GetAllNodeNum(ChildList[i]);
	}

	return iNum;
}

/******************************************************
@����˵���� ��ȡȫ���ڵ�����������Ϣ�ṹ��
@�������룺 pNode -- ����xml�ڵ�
pstFni -- ����������Ϣ�ṹ��
iIndex -- ȫ�ֲ�������
@��������� ��
@����ֵ  �� �ɹ�����true��ʧ�ܷ���false
******************************************************/
int GetAllNodeInfo(XmlTreeNode* pNode, LST_NODE_INFO *pstFni, int &iIndex)
{
	//�жϺ�����
	if (pNode == NULL)
	{
		return 0;
	}
	if (pNode->NodeType() == NODE_TYPE_UNKNOW)
	{
		return 0;
	}

	//���ȶ�ȡ��ǰ�ڵ���ӽڵ��б�
	std::vector<XmlTreeNode*> ChildList = pNode->GetChildrenNodeList();

	//�ж��ӽڵ���������������0�����ʾ�Ѿ���������ײ㣬�����˳���ǰ����
	if (ChildList.size() == 0)
	{
		return 1;
	}
	int ii = ChildList.size();
	//ѭ�����������ӽڵ�
	for (int i = 0; i < ChildList.size(); i++)
	{
		//��ṹ���и�ֵ
		pstFni[iIndex].iNodeIndex = iIndex;
		memcpy(pstFni[iIndex].achNodeName, ChildList[i]->NodeName(), LEN_128);
		pstFni[iIndex].iNodeType = ChildList[i]->NodeType();
		pstFni[iIndex].achReserved[0] = '\0';

		//ȫ����������
		iIndex++;

		//���������ӽڵ���ӽڵ�
		GetAllNodeInfo(ChildList[i], pstFni, iIndex);
	}
	return 0;
}

/******************************************************
@����˵���� ��֪���Ʋ�������Լ��LFNC����XML������������ĵ��У���ȡ���������Ϣ�ṹ��
@�������룺 iCameraIndex -- �������
pchfeatureName -- ���Ʋ�������Լ��LFNC
@��������� stFeatureInfo -- ������Ϣ�ṹ�壬�μ�LST_FEATURE_INFO��
@����ֵ  �� �ɹ�����true����֮����false��
******************************************************/
bool GetFeatureInfoFromXML(int iCameraIndex, const char *pchfeatureName, LST_FEATURE_INFO &stFeatureInfo)
{
	//�������ж�
	if (pchfeatureName == NULL)
	{
		stFeatureInfo.eType = FEATURE_TYPE_UNKNOW;
		return false;
	}

	//��ȡ��������Ľڵ���
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
@����˵���� ��֪���Ʋ�������Լ��LFNC���õ���Ӧ�ĳ��Ҳ���������Vendor���˴�ΪSaperaLT��
@�������룺 iCameraIndex -- �������
pchfeatureName_Lfnc -- ���Ʋ�������Լ��LFNC
@��������� pchfeatureName_Vendor -- ���Ҳ���������Vendor���˴�Ϊranger3��
@����ֵ  �� �ɹ�����true����֮����false��
******************************************************/
bool ConvertLfncToVendor(int iCameraIndex, char *pchfeatureName_Lfnc, char *pchfeatureName_Vendor)
{
	//�������ж�
	if (pchfeatureName_Lfnc == NULL)
	{
		return false;
	}
	if (pchfeatureName_Vendor == NULL)
	{
		return false;
	}

	//��ȡ��������Ľڵ���
	XmlTreeNode *poChildTreeNode = m_poTreeNodeRoot[iCameraIndex]->GetChildNode(pchfeatureName_Lfnc);
	if (poChildTreeNode->NodeType() == NODE_TYPE_UNKNOW)
	{
		return false;
	}
	strcpy_s(pchfeatureName_Vendor, 255, poChildTreeNode->GetFeatureInfo().achVenderFeatureName);

	return true;
}

/******************************************************
@����˵���� �����߳����ȼ�
@�������룺 dataStreamNodeMap -- �������ڵ�ͼ
@��������� ��
@����ֵ  �� ��
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
@����˵���� ��ȡͼ���̣߳���ȡ����˵�Buffer��������˵�Buffer��
@�������룺 lpParam -- �߳�Ĭ�ϲ�������������������������
@��������� ��
@����ֵ  �� ������������true����֮Ϊfalse
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

	//�������ж�
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

		//��ȡϵͳʱ��
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
			//Xֵ
			memcpy(pBufferOfRange3[iCameraIndex], static_cast<float*>(imgTable.RangX), iImageHeight*iImageWidth * rangDataBitSize);
			//Rֵ
			memcpy(pBufferOfRange3[iCameraIndex] + iImageHeight*iImageWidth * rangDataBitSize, static_cast<float*>(imgTable.getData(SickCam::DataNames::RAN_CAL)),
				iImageHeight*iImageWidth * rangDataBitSize);
		}
		else
		{
			memcpy(pBufferOfRange3[iCameraIndex], static_cast<float*>(imgTable.getData(SickCam::DataNames::RAN)),
				iImageHeight*iImageWidth * rangDataBitSize);

		}

		//�Ҷ�ֵ
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

		//���ֻ��������ȡ��
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

		//�洢Buffer���ж��Ƿ���Ҫ��ͼ��
		PushImageToBuffer(m_pastStoreImageInfo[iCameraIndex], m_pastBufferInfo[iCameraIndex]->iBufferNum, iCameraIndex,
			m_iPushStoreBufferIndex[iCameraIndex], pBufferOfRange3[iCameraIndex], BufferSize[iCameraIndex],
			iImageWidth, iImageHeight, iImagePixelFormat, m_iImageCount[iCameraIndex], iImageChannels,
			iCameraStride, iImageDepth, &sysTime);

		memcpy(m_pastStoreImageInfo[iCameraIndex][m_iPushStoreBufferIndex[iCameraIndex]].pstTagInfo, m_pastRanger3XMLInfo[iCameraIndex],
			m_pastStoreImageInfo[iCameraIndex][m_iPushStoreBufferIndex[iCameraIndex]].iTagInfoSize);

		m_pastStoreImageInfo[iCameraIndex][m_iPushStoreBufferIndex[iCameraIndex]].achReserved[INDEX_SEMAPHORE] = TRUE;
	
		//��ʾBuffer���жϸ�֡�Ƿ���Ҫ��Miss
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
@����˵���� ��ʾ�ص��̣߳���ȡ������˵�Bufferͨ����ʾ���ͻص������ϲ�����ߣ�
��ͼ������������ʾ����˴��ڳ�֡���й��ܡ�
@�������룺 lpParam -- �߳�Ĭ�ϲ�������������������������
@��������� ��
@����ֵ  �� ������������true����֮Ϊfalse
******************************************************/
DWORD WINAPI  ThreadDisplayImage(LPVOID lpParam)
{
	//�������ж�
	int iCameraIndex = *(int*)lpParam;
	if (iCameraIndex < 0 || iCameraIndex >= CAMERA_MAX_NUM)
	{
		return false;
	}

	//������ʱBuffer�����ÿ֡����
	LST_IMAGE_INFO *pstImageInfoTemp[SNAP_BUF_NUM];
	pstImageInfoTemp[SNAP_BUF_INDEX] = NULL;

	//��ʱBufferInfoֻȡһ��
	LST_BUFFER_INFO stBufferInfoTemp;
	stBufferInfoTemp.iBufferNum = SNAP_BUF_NUM;
	stBufferInfoTemp.iMergeFrameNum = 1;
	stBufferInfoTemp.achReserved[0] = EMPTY_STRING;

	bool bCreate = CreateNodeBuffer(SNAP_BUF_INDEX, pstImageInfoTemp, m_pastImageProperty[iCameraIndex], &stBufferInfoTemp);
	if (!bCreate)
	{
		return false;
	}
	
	//ѭ���ȴ����ݣ�һֱ�ȴ����ж�m_bIsStopShow�˳�
	while (1)
	{
		//ȡ���������Buffer����
		if (!PopImageFromBuffer(m_pastDisplayImageInfo[iCameraIndex], DISPLAY_MAX_BUFF, iCameraIndex, &pstImageInfoTemp[SNAP_BUF_INDEX],
			m_iPopDisplayBufferIndex[iCameraIndex]))
		{
			if (m_bIsStopDisplay[iCameraIndex])
				break;
			::Sleep(10);
		}
		else
		{
			//������ʾ�ص�����
			m_pDisplayCallBack[iCameraIndex](CB_DISPLAY, m_pCamHandle[iCameraIndex], pstImageInfoTemp[SNAP_BUF_INDEX], m_pDisplayUserContext[iCameraIndex]);


			//����ȡ����ϣ������ź���
			m_pastDisplayImageInfo[iCameraIndex][m_iPopDisplayBufferIndex[iCameraIndex]].achReserved[INDEX_SEMAPHORE] = FALSE;			
		}
	}

	//�߳̽���֮ǰ���ͷ���ʱBuffer��Դ
	if (pstImageInfoTemp[SNAP_BUF_INDEX] != NULL)
	{
		FreeNodeBuffer(pstImageInfoTemp[SNAP_BUF_INDEX], stBufferInfoTemp.iBufferNum);
		pstImageInfoTemp[SNAP_BUF_INDEX] = NULL;
	}

	return true;
}

/******************************************************
@����˵���� �洢�ص��̣߳���ȡ������˵�Bufferͨ���洢���ͻص������ϲ�����ߣ�
��ͼ���������ڴ洢����ͼ��������Ҫ���ϲ���֡��
@�������룺 lpParam -- �߳�Ĭ�ϲ�������������������������
@��������� ��
@����ֵ  �� ������������true����֮Ϊfalse
******************************************************/
DWORD WINAPI  ThreadStoreImage(LPVOID lpParam)
{
	LST_MESSAGE_INFO messageInfo;
	int iLastImageCount = 0;
	int iImageCount = 0;

	//�������ж�
	int iCameraIndex = *((int*)lpParam);
	if (iCameraIndex < 0 || iCameraIndex >= CAMERA_MAX_NUM)
	{
		return false;
	}

	//������ʱBuffer�����ÿ֡����
	LST_IMAGE_INFO *pstCurImageInfo[SNAP_BUF_NUM];
	pstCurImageInfo[SNAP_BUF_INDEX] = NULL;

	//Snap��bufferֻȡһ��
	LST_BUFFER_INFO stSnapBufferInfo;
	stSnapBufferInfo.iBufferNum = SNAP_BUF_NUM;
	stSnapBufferInfo.iMergeFrameNum = 1;
	stSnapBufferInfo.achReserved[0] = EMPTY_STRING;

	bool bCreate = CreateNodeBuffer(SNAP_BUF_INDEX, pstCurImageInfo, m_pastImageProperty[iCameraIndex], &stSnapBufferInfo);
	if (!bCreate)
	{
		return false;
	}

	//ѭ���ȴ����ݣ�һֱ�ȴ����ж�m_bIsStopShow�˳�
	while (1)
	{
		//ȡ���������Buffer����

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
				//���ô洢�ص�����
				m_pStoreCallBack[iCameraIndex](CB_STORE, m_pCamHandle[iCameraIndex], pstCurImageInfo[SNAP_BUF_INDEX], m_pStoreUserContext[iCameraIndex]);
			/*	std::string info = "��ǰbuffer count is:"+std::to_string(pstCurImageInfo[SNAP_BUF_INDEX]->iBufferFullNum)+"\r\n";
				OutputDebugString(info.c_str());*/
				//��ͼģʽ�������ҵ�ǰ����Buffer����Ϊ1��	//Ϊʲô��1��  ��Ϊ��ʱ������ڱ�ռ�õ�Buffer��û�н����ͷţ����iBufferFullNum����1���ɡ�	
				//if(m_iRecordMode[iCameraIndex] == 2 && pstCurImageInfo[SNAP_BUF_INDEX]->iBufferFullNum == 1)
				//{
				//	if (m_pMessageCallBack[iCameraIndex] != NULL)
				//	{
				//		memset(messageInfo.achMessage, 0x00, 256);
				//		messageInfo.iMessageType = 0x01;//TODO:��Ϣ�����������ϸ���塣

				//		//������Ϣ�ص�����
				//		m_pMessageCallBack[iCameraIndex](CB_MESSAGE, m_pCamHandle[iCameraIndex], &messageInfo, m_pMessageUserContext[iCameraIndex]);
				//		m_iRecordMode[iCameraIndex] = 0;
				//	}
				//}

				//����ȡ����ϣ������ź���
				m_pastStoreImageInfo[iCameraIndex][m_iPopStoreBufferIndex[iCameraIndex]].achReserved[INDEX_SEMAPHORE] = FALSE;			
			}
		}
	}

	//�߳̽���֮ǰ���ͷ���ʱBuffer��Դ
	if (pstCurImageInfo[SNAP_BUF_INDEX] != NULL)
	{
		FreeNodeBuffer(pstCurImageInfo[SNAP_BUF_INDEX], stSnapBufferInfo.iBufferNum);
		pstCurImageInfo[SNAP_BUF_INDEX] = NULL;
	}

	return true;
}
/******************************************************
@����˵���� ���ڴ����ݱ�����뻺����
@�������룺 pstImageInfo -- �ɼ�ͼ����Ϣ�ṹ�壬������ڴ�Buffer��
iBufferNum -- ������ڴ�Buffer��������
iCameraIndex -- �������
iCurBufferIndex -- ��ǰBuffer����
pchCameraBuffer -- ����˵õ���Buffer����
iCameraBufferSize -- ���Buffer��С
iImageWidth -- ���Buffer���
iImageHeight -- ���Buffer�߶�
iImagePixelFormat -- ���Buffer���ظ�ʽ��LFNC
iImageCount -- ֡����ֵ����ֵΪ�ܼ�������1��ʼ��һֱ���ӵ�65536��ebus��˵������
iImageChannles -- ���Bufferͨ����
iCameraStride -- ���ͼ��Ƶ
iImageDepth	-- ���Bufferλ��
sysTime -- ϵͳʱ��ṹ�壬��ǰ֡ʱ����Ϣ
@��������� ��
@����ֵ  �� �ɹ�����Buffer����true��ʧ�ܻ���Buffer�ɲ�������false��
******************************************************/
bool PushImageToBuffer(LST_IMAGE_INFO *pstImageInfo, int iBufferNum, int iCameraIndex, int &iCurBufferIndex,
	char *pchCameraBuffer, int iCameraBufferSize, int iImageWidth, int iImageHeight, int iImagePixelFormat,
	int iImageCount, int iImageChannles, int iCameraStride, int iImageDepth, LST_SYSTEM_TIME *sysTime)
{
	int iNextBufferIndex = 0;	//��һ��Buffer����
	int iBreakFlag = 0;			//Ѱ�ҽ��Flag

#if IS_USE_CLOCK
	clock_t start, end;
	double time;
#endif

	//�����������ж�
	if (pstImageInfo == NULL || iBufferNum == 0)
		return false;

	//�жϵ�ǰBuffer�������õ���һbuffer������
	if (iCurBufferIndex == iBufferNum - 1)
	{
		iNextBufferIndex = 0;//�����ǰBuffer����Ϊ���һ��buffer�������ص��ʼ��0��
	}
	else
	{
		iNextBufferIndex = iCurBufferIndex + 1;//��������£�������1
	}

	//��˳��������е�Buffer��Ѱ�ҵ�һ���ź���Ϊfalse��Buffer�����õ���Buffer������
	for (int index = iNextBufferIndex; index < iNextBufferIndex + iBufferNum; index++)
	{
		if (index > iBufferNum - 1) //�����������buffer��С��������
		{
			if (pstImageInfo[index - iBufferNum].achReserved[INDEX_SEMAPHORE] == FALSE)
			{
				iCurBufferIndex = index - iBufferNum;
				iBreakFlag = 1;
				break;
			}
		}
		else  //�������������buffer��С������Ҫ�ر���
		{
			if (pstImageInfo[index].achReserved[INDEX_SEMAPHORE] == FALSE)
			{
				iCurBufferIndex = index;
				iBreakFlag = 1;
				break;
			}
		}
	}

	//�ж��Ƿ����ź���Ϊtrue��Buffer��û�з����򷵻�false
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
@����˵���� �Ӽ�����ڴ滺������ȡ��ͼ�����ݣ����ͨ��CallBack�����ϲ㡣�ü�����ڴ滺����Ϊ���Buffer�Ķ��С�
@�������룺 pstImageInfo -- �ɼ�ͼ����Ϣ�ṹ�壬������ڴ�Buffer��
iBufferNum -- ������ڴ�Buffer��������
iCameraIndex -- �������
pstOutImageInfo -- �ɼ�ͼ����Ϣ�ṹ�壬CallBack���Buffer
iCurBufferIndex -- ��ǰBuffer����
@��������� ��
@����ֵ  �� �ɹ�����Buffer����true��ʧ�ܻ���Buffer�ɲ�������false��
******************************************************/
bool PopImageFromBuffer(LST_IMAGE_INFO *pstImageInfo, int iBufferNum, int iCameraIndex, LST_IMAGE_INFO **pstOutImageInfo, int &iCurBufferIndex)
{
	int iNextBufferIndex = 0;	//��һ��Buffer����
	int iBreakFlag = 0;			//Ѱ�ҽ��Flag

	//�жϵ�ǰBuffer�������õ���һbuffer������
	if (iCurBufferIndex == iBufferNum - 1)
	{
		iNextBufferIndex = 0;//�����ǰBuffer����Ϊ���һ��buffer�������ص��ʼ��0��
	}
	else
	{
		iNextBufferIndex = iCurBufferIndex + 1;//��������£�������1
	}

	//��˳��������е�Buffer��Ѱ�ҵ�һ���ź���Ϊtrue��Buffer�����õ���Buffer������
	for (int index = iNextBufferIndex; index < iNextBufferIndex + iBufferNum; index++)
	{
		if (index > iBufferNum - 1)  //�����������buffer��С��������
		{
			if (pstImageInfo[index - iBufferNum].achReserved[INDEX_SEMAPHORE] == TRUE)
			{
				iCurBufferIndex = index - iBufferNum;
				iBreakFlag = 1;
				break;
			}
		}
		else  //�������������buffer��С������Ҫ�ر���
		{
			if (pstImageInfo[index].achReserved[INDEX_SEMAPHORE] == TRUE)
			{
				iCurBufferIndex = index;
				iBreakFlag = 1;
				break;
			}
		}
	}

	//�ж��Ƿ����ź���Ϊtrue��Buffer��û�з����򷵻�false
	if (iBreakFlag == 0)
	{
		return false;
	}

	//����ͼ������

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
	
	//������ǰ���������ж��ٸ��ź���Ϊtrue��Buffer
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
@����˵���� ���ݵ�ǰ��֡�ʽ��ж�֡����,����ϵͳ����
@�������룺 iFrameCount -- ֡����ֵ����ֵΪ�ܼ�������1��ʼ��һֱ���ӵ�65536��ebus��˵������
iCurFrameRate -- ��ǰ֡Ƶ
iDestFrameRate -- Ŀ��֡Ƶ
@��������� ��
@����ֵ  �� ��Ҫ��֡����true������Ҫ��֡(��֡������ʾ)����false
******************************************************/
bool  IsThisFrameMiss(int iFrameCount, int iCurFrameRate, int iDestFrameRate)
{
	//TODO:�ó�֡��������ѧԭ�������˵��
	//TODO:����Ķ��� int iCurFrameRate, int iDestFrameRate����������iCurFrameRate��С��1��С�����ᱻ��ȡΪ0��
	//Ȼ��ᱻ�ж�Ϊ��֡��Ҫ��������һֱ���ٴ����ص��������Ϊdouble�͡� 
	//qiliu,2017-08-30 10-56
	int iPer = -1;
	int iDis = -1;
	int iFlag = -1;

	//�����������ж�
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
@����˵���� ���������������XML�ļ���
@�������룺 iCameraIndex -- �������
pchXmlFileName -- �ļ�����·����
@��������� ��
@����ֵ  �� �ɹ�����true����֮����false��
******************************************************/
bool LoadCameraXml(int iCameraIndex, const char *pchXmlFileName)
{
	//���ļ������ж��ļ��Ƿ����
	//FILE *pf = fopen(pchXmlFileName, "rb");
	FILE *pf;
	fopen_s(&pf, pchXmlFileName, "rb");

	if (pf == NULL)
	{
		return false;
	}
	fclose(pf);

	//��ʼ��ȫ�ֱ���
	if (m_poTreeNodeRoot[iCameraIndex] != NULL)
	{
		delete m_poTreeNodeRoot[iCameraIndex];
		m_poTreeNodeRoot[iCameraIndex] = NULL;
	}

	//ʵ�����࣬�������ຯ�������ļ�
	m_poTreeNodeRoot[iCameraIndex] = new XmlTreeNode("", NODE_TYPE_UNKNOW);
	m_poTreeNodeRoot[iCameraIndex]->LoadXml(pchXmlFileName);

	return true;
}

/******************************************************
@����˵���� ��������������������pchfeatureName����������������ĵ�XML��
��ȡ�ò��������ͣ��ж���������Ĳ�������eType�Ƿ���ͬ��
@�������룺 iCameraIndex -- �������
pchfeatureName -- ����������ƣ�������ΪLFNC���ơ�
eType -- ����Ĳ������͡�
@��������� ��
@����ֵ  �� ������ͬ����true����֮����false
******************************************************/
bool IsFeatureTypeRight(int iCameraIndex, const char* pchfeatureName, LE_FEATURE_TYPE eType)
{
	//�������ж�
	if (m_poTreeNodeRoot == NULL)
	{
		return NULL;
	}
	if (pchfeatureName == NULL)
	{
		return NULL;
	}

	//��ȡ��������Ľڵ���
	XmlTreeNode *poChildTreeNode = m_poTreeNodeRoot[iCameraIndex]->GetChildNode(pchfeatureName);
	if (poChildTreeNode->NodeType() == NODE_TYPE_UNKNOW)
	{
		return NULL;
	}

	//���ݽڵ��෵�ظýڵ�����������Ϣ
	LST_FEATURE_INFO stFeatureInfo = poChildTreeNode->GetFeatureInfo();

	//�ж�
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
@����˵���� ����LFNC��ö�٣�int��ʽ�����õ���Ӧ��LST_IMAGE_PROPERTY�ṹ��
@�������룺 iPixelFormat -- ��������
@��������� stImageProperty -- ͼ�����Խṹ��
@����ֵ  �� �ɹ�����true����֮����false��
******************************************************/
bool GetImagePropertyFromEnumInt(int iPixelFormat, LST_IMAGE_PROPERTY *stImageProperty)
{
	//�����������ж�
	if (iPixelFormat < 0)
	{
		return false;
	}

	//�ж����ͣ����ض�Ӧ��λ����ͨ����
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
@����˵���� ��ȡ�����buffer��Range��
@�������룺 tl -- ������
dataStreamHandle -- ���������
bufferHandle -- buffer���
@��������� bufferData -- mono16 buffer
bufferSize -- mono16 buffer�Ĵ�С
@����ֵ  �� ��
******************************************************/



/******************************************************
@����˵���� ��ȡ������������ļ���csv���еĲ������ƺͲ���ֵ
@�������룺 line -- �����ļ�����
@��������� key -- ��������
value -- ����ֵ
@����ֵ  �� ��
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
@����˵���� ��ʼ���Զ��建��ģ�顣�洢���桢��ʾ���桢��֡���桢��������
@�������룺 iCameraIndex -- �������
@��������� ��
@����ֵ  �� �ɹ�����true��ʧ�ܷ���false
******************************************************/
LE_STATUS InitLusterBuffer(int iCameraIndex, LST_BUFFER_INFO *stBufferInfo)
{
	//������Խṹ��
	m_pastImageProperty[iCameraIndex] = new LST_IMAGE_PROPERTY();
	//Ranger3 XML��Ϣ�ṹ��
	m_pastRanger3XMLInfo[iCameraIndex] = new LST_RANGER3XML_INFO();

	/*******************************************
	��ȡͼ������
	*******************************************/

	//��ȡImageProperty�ṹ��
	//��ȡͼ����
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

	//	//��ȡͼ��߶�
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

			//д��δ�궨ranger3xml�ļ�������Ϣ
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

	//��ȡͼ�����ظ�ʽ
	if (!GetImagePropertyFromEnumInt(m_pastImageProperty[iCameraIndex]->iImagePixelFormat, m_pastImageProperty[iCameraIndex]))
	{
		return L_ERR_UNKNOWN;
	}
	m_pastImageProperty[iCameraIndex]->iEndianess = 0;//�Ժ�ɸ�����������жϡ�
	m_pastImageProperty[iCameraIndex]->achReserved[0] = EMPTY_STRING;

	//��ȡ��ǰ��֡Ƶ��������������������Ƶ����֡Ƶ

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

	//�����洢Buffer
	bool bResult = CreateNodeBuffer(iCameraIndex, m_pastStoreImageInfo, m_pastImageProperty[iCameraIndex], stBufferInfo);
	if (bResult == false)
	{
		return L_ERR_CCNIB;	//ͼ��ɼ��豸�޷���ʼ��
	}

	//��ʾBuffer��Ϣ�ṹ��
	LST_BUFFER_INFO stShowBufferInfo;
	stShowBufferInfo.iBufferNum = DISPLAY_MAX_BUFF;
	stShowBufferInfo.iMergeFrameNum = m_pastBufferInfo[iCameraIndex]->iMergeFrameNum;
	stShowBufferInfo.achReserved[0] = EMPTY_STRING;

	//������ʾBuffer
	bResult = CreateNodeBuffer(iCameraIndex, m_pastDisplayImageInfo, m_pastImageProperty[iCameraIndex], &stShowBufferInfo);
	if (bResult == false)
	{
		return L_ERR_CCNIB;	//ͼ��ɼ��豸�޷���ʼ��
	}


	return L_OK;
}

/******************************************************
@����˵���� �ͷ��Զ��建��ģ�顣�洢���桢��ʾ���桢��֡���桢��������
@�������룺 iCameraIndex -- �������
@��������� ��
@����ֵ  �� �ɹ�����true��ʧ�ܷ���false
******************************************************/
LE_STATUS ReleaseLusterBuffer(int iCameraIndex)
{
	//�ͷŴ洢����
	if (m_pastStoreImageInfo[iCameraIndex] != NULL)
	{
		FreeNodeBuffer(m_pastStoreImageInfo[iCameraIndex], m_pastBufferInfo[iCameraIndex]->iBufferNum);
		m_pastStoreImageInfo[iCameraIndex] = NULL;
	}
	//�ͷ���ʾ����
	if (m_pastDisplayImageInfo[iCameraIndex] != NULL)
	{
		FreeNodeBuffer(m_pastDisplayImageInfo[iCameraIndex], DISPLAY_MAX_BUFF);
		m_pastDisplayImageInfo[iCameraIndex] = NULL;
	}
	//������ղ�������֡buffer, LST_IMAGE_INFO
	if (m_pastSnapImageInfo[SNAP_BUF_INDEX] != NULL)
	{
		FreeNodeBuffer(m_pastSnapImageInfo[SNAP_BUF_INDEX], SNAP_BUF_NUM);//ֻҪ1��buffer
		m_pastSnapImageInfo[SNAP_BUF_INDEX] = NULL;
	}

	//���buffer����
	if (pBufferOfRange3[iCameraIndex] != NULL)
	{
		free(pBufferOfRange3[iCameraIndex]);
		pBufferOfRange3[iCameraIndex] = NULL;
	}

	//������ղ�����ͼ�����Խṹ��
	if (m_pastImageProperty[iCameraIndex] != NULL)
	{
		delete m_pastImageProperty[iCameraIndex];
		m_pastImageProperty[iCameraIndex] = NULL;
	}

	//������ղ��������Ranger3xml��Ϣ
	if (m_pastRanger3XMLInfo[iCameraIndex] != NULL)
	{
		delete m_pastRanger3XMLInfo[iCameraIndex];
		m_pastRanger3XMLInfo[iCameraIndex] = NULL;
	}
	//����������ʼ��
	m_iPushStoreBufferIndex[iCameraIndex] = -1;
	m_iPopStoreBufferIndex[iCameraIndex] = -1;
	m_iPushDisplayBufferIndex[iCameraIndex] = -1;
	m_iPopDisplayBufferIndex[iCameraIndex] = -1;

	return L_OK;
}

#endif // 0


