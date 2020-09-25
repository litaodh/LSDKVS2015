/***********************************************************************
*
* Copyright (c) 2000-2001 Luster LightTech(Beijing) Co.Ltd.
* All Rights Reserved.
*
*
* FILE NAME     :  LstCameraControl_C.h
* PROJECT NAME  :  LSDK
* ABSTRACT      :  �������ģ��ͷ�ļ�����׼C���԰汾����
*
*
* VERSION       :  1.0.0
* DATE 		    :  2018/02/14
* AUTHOR 		:  ����
* NOTE 		    :  ��Ӧ�ӿ��ĵ���LSDK�������ͳһ�ӿ�˵��(��׼C����)_v1.0.0����
***********************************************************************/
#pragma once

#ifndef LST_CAMERA_CONTROL_C_H_
#define LST_CAMERA_CONTROL_C_H_

#include "LstCommonDef.h"


#define MESSAGE_LEN_256 256

/*
*enum
*/

//����״̬����ö�٣���������״̬�Ͷ��ִ���״̬��
//ö�ٷ�Χ0x01000130 ~ 0x0100014F
typedef enum tagEnumLusterStatus
{
	L_OK = 0x01001000,	//����״̬
	L_ERR_CCNOED = 0x01001001,	//���δ�򿪣�û������ͼ��ɼ��豸
	L_ERR_CCWC = 0x01001002,	//����Ĳ�ɫλ��
	L_ERR_CCWD = 0x01001003,	//����Ĳɼ��豸
	L_ERR_CCWPF = 0x01001004,	//�����֧�ֵ����ظ�ʽ
	L_ERR_CCNV = 0x01001005,	//û����Ƶ�ź�
	L_ERR_CCU = 0x01001006,	//δ��������������Ϣ
	L_ERR_CCF = 0x01001007,	//ͼ��ץȡʧ��
	L_ERR_CCWR = 0x01001008,	//ѡ������ͼ��ֱ���
	L_ERR_CCWP = 0x01001009,	//ѡ������ͼ�񲿷�
	L_ERR_CCWPR = 0x0100100A,	//ѡ���������ر���
	L_ERR_CCWH = 0x0100100B,	//�����Ч
	L_ERR_CCCL = 0x0100100C,	//ʵ����Ч�������Ѿ�ע����
	L_ERR_CCNIB = 0x0100100D,	//�޷���ʼ������
	L_ERR_CCET = 0x0100100E,	//��֧���ⴥ��
	L_ERR_CCLI = 0x0100100F,	//�������������У����أ�
	L_ERR_CCCS = 0x01001010,	//�������ɫ�ռ�
	L_ERR_CCPT = 0x01001011,	//����Ķ˿�
	L_ERR_CCCT = 0x01001012,	//������������
	L_ERR_CCTM = 0x01001013,	//�����˲ɼ��豸��������Ŀ
	L_ERR_CCDV = 0x01001014,	//�豸æµ
	L_ERR_CCASYNC = 0x01001015,	//��֧���첽ץȡ
	L_ERR_CCPARAM = 0x01001016,	//�ò����ݲ�֧��
	L_ERR_CCTIMEOUT = 0x01001017,	//��ʱ
	L_ERR_CCGAIN = 0x01001018,	//��Ч������
	L_ERR_CCFIELD = 0x01001019,	//��Ч���ֶ�
	L_ERR_CCPART = 0x0100101A,	//��Ч�Ĳ�������
	L_ERR_CCPARV = 0x0100101B,	//��Ч�Ĳ���ֵ
	L_ERR_CCFNS = 0x0100101C,	//��֧�ֻ��ѱ������Ĺ���
	L_ERR_CCIVERS = 0x0100101D,	//�汾�����ݵĽӿ�
	L_ERR_CCSETPAR = 0x0100101E,	//���ò���ֵʧ��
	L_ERR_CCGETPAR = 0x0100101F,	//��ѯ����ֵʧ��
	L_ERR_CCPARNA = 0x01001020,	//��ǰ�����£��ò���������
	L_ERR_CCCLOSE = 0x01001021,	//�豸���������ر�
	L_ERR_CCCAMFILE = 0x01001022,	//��������ļ����ܴ�
	L_ERR_CCCALLBACK = 0x01001023,	//��֧�ֵĻص�����
	L_ERR_CCDEVLOST = 0x01001024,	//�豸��ʧ
	L_ERR_DNA = 0x01001025,	//�豸�����������
	L_ERR_MEM = 0x01001026,	//�ڴ治��
	L_ERR_CCFWIDX = 0x01001027,	//��Ч���������
	L_ERR_CCFWVI = 0x01001028,	//���ҽӿڴ���
	L_ERR_CCXML = 0x01001029,	//XML�����ļ����ܴ�
	L_ERR_CCFIDFV = 0x0100102A,	//Ĭ�ϲ�����ʼ��ʧ��
	L_ERR_CCOPEN = 0x0100102B,	//�����ʧ��
	L_ERR_CCRXFVF = 0x0100102C,	//��ȡXML����ֵʧ��
	L_ERR_CCNRB = 0x0100102D,	//�޷��ͷŻ���
	L_ERR_CCWLPI = 0x0100102E,	//�޷����ز���������������󣬻�ȱʧ�����ļ�
	L_ERR_CCWFPI = 0x0100102F,	//�޷��ͷŲ��
	L_ERR_CCOED = 0x01001030,	//����Ѿ���
	L_ERR_CCIED = 0x01001031,	//����Ѿ���ʼ��
	L_ERR_CCNIED = 0x01001032,	//���δ��ʼ��
	L_ERR_CCGED = 0x01001033,	//����Ѿ��ɼ�
	L_ERR_CCNGED = 0x01001034,	//���δ�ɼ�
	L_ERR_UNKNOWN = 0x01001FFF,	//δ֪����
} LE_STATUS;

//�ص���������ö�٣������ɼ��洢�ص����͡��ɼ���ʾ�ص����͡��ɼ���Ϣ�ص����͡�δ֪����Unknow��
//ö�ٷ�Χ0x01000130 ~ 0x0100014F
typedef enum tagEnumCallbackType
{
	//�ɼ��洢�ص����ͣ�������ȫ������
	CB_STORE = 0x01000130,
	//�ɼ���ʾ�ص����ͣ��ڴ�����֡Ƶ���߻��֡�������ݣ��ⴥ�����޷����֡Ƶ����޷���֡
	CB_DISPLAY = 0x01000131,
	//�ɼ���Ϣ�ص����ͣ�������Ҫ�������ж�֡������ԣ����Է���֡������֡��ʱ����Ϣ�����ж϶�֡ԭ��
	CB_MESSAGE = 0x01000132,
	//δ֪����
	CB_UNKNOW = 0x0100014F
} LE_CALLBACK_TYPE;

//����ӿ�����ö�٣�����CameraLink�ӿڡ�GigE Vision���ڡ�USB 2.0�ӿڡ�USB 3.0�ӿڡ�IEEE1394�ӿڡ�CoaXPress�ӿڵȡ�
//ö�ٷ�Χ0x01000170 ~ 0x0100018F
typedef enum tagEnumCameraInterfaceType
{
	//Camera Link�ӿ�
	CIT_CAMERALINK = 0x01000170,
	//GigE Vision����
	CIT_GIGE = 0x01000171,
	//USB 2.0�ӿ�
	CIT_USB2 = 0x01000172,
	//USB 3.0�ӿ�
	CIT_USB3 = 0x01000173,
	//IEEE1394�ӿ�
	CIT_IEEE1394 = 0x01000174,
	//CoaXPress�ӿ�
	CIT_COAXPRESS = 0x01000175,
	//δ֪����
	CIT_UNKNOWN = 0x0100018F
} LE_CAMERA_INTERFACE_TYPE;

//�ɼ���Ϣ����ö��
//ö�ٷ�Χ0x01000190 ~ 0x010001EF
typedef enum tagEnumMessageType
{
	//���һ�ȡͼ����ʧ�ܣ�һ��ΪӲ����֡��
	MT_CBT_VGIMF = 0x01000190,
	//���->�����̣߳���һ֡ͼ��͵�ǰ֡ͼ���֡����������������1��
	MT_CBT_FCNC = 0x01000191,
	//���->�����̣߳���ǰ֡ͼ����뻺��Ĵ���ʱ�����������5ms��
	MT_CBT_TTL = 0x01000192,
	//���->�����̣߳��洢���ͻ����������
	MT_CBT_SBF = 0x01000193,
	//���->�����̣߳���ʾ���ͻ����������
	MT_CBT_DBF = 0x01000194,
	//����->�ϲ�Ӧ���̣߳���һ֡ͼ��͵�ǰ֡ͼ���֡����������������1��
	MT_BAT_FCNC = 0x01000195,
	//δ֪����
	MT_UNKNOWN = 0x010001EF,
} LE_MESSAGE_TYPE;



/*
*struct
*/

//ϵͳʱ��ṹ�壬��ż�¼ϵͳ��ʱ��㣬�����ꡢ�¡��ա�Сʱ�����ӡ��롢���룬���Դ��ÿһ֡ͼ��洢��ʱ����Ϣ��
typedef struct tagStructSystemTime
{
	//��
	int iYear;
	//��
	int iMonth;
	//��
	int iDay;
	//Сʱ
	int iHour;
	//����
	int iMinute;
	//��
	int iSecond;
	//����
	int iMilliSeconds;
} LST_SYSTEM_TIME;

//�궨������������Ϣ
typedef struct tagStructCalCoordInfo
{
	//X ����Сֵ����λ ����
	double LowerBoundX;
	// X �����ֵ����λ ����
	double UpperBoundX;
	//Z ����Сֵ����λ ����
	double LowerBoundR;
	//Z �����ֵ����λ ����
	double UpperBoundR;
}LST_CALIBRATIONCOORD_INFO;

//δ�궨�����������Ϣ
typedef struct tagStructCoordInfo
{
	int RegionHeight;
	int RegionOffsetY;
	int RangAixFlat;
}LST_COORD_INFO;
//Ranger3xml�����Ϣ
typedef struct tagStructRanger3XmlInfo
{
	//�Ƿ�궨
	int tagCalibration;
	//�Ƿ��������ǿ��ͼ0 ������ 1 ����
	int tagHasReflectance;
	union
	{
		//�ѱ궨������Ϣ
		LST_CALIBRATIONCOORD_INFO stCalibrationCoordInfo;
		//δ�궨������Ϣ
		LST_COORD_INFO stCoordInfo;
	}CoordinateInfo;

}LST_RANGER3XML_INFO;


//�ɼ�ͼ����Ϣ�ṹ�壬���ͼ������ָ�롢ͼ�����ݴ�С��ͼ������֡�š�ͼ��Ƶ��
//��������������ͼ�����Խṹ�塢ͼ�����ݲɼ�ʱ��ṹ�塢Ԥ���ֶε���Ϣ��
typedef struct tagStructSnapImageInfo
{
	/*ͼ������ָ��*/
	BYTE* pchBuffer;
	/*ͼ�����ݴ�С*/
	int iBufferSize;
	/*ͼ������֡��*/
	int iImageFrameIndex;
	/*ͼ��Ƶ*/
	int iImageStrideRate;
	/*������������*/
	int iBufferFullNum;
	/*ͼ�����Խṹ��*/
	LST_IMAGE_PROPERTY stImageProperty;
	/*ͼ�����ݲɼ�ʱ��ṹ��*/
	LST_SYSTEM_TIME stSysTime;
	//������Ϣ
	void* pstTagInfo;
	//������Ϣ��С
	int iTagInfoSize;
	/*Ԥ���ֶ�*/
	char achReserved[128];
} LST_IMAGE_INFO;

//��Ϣ��Ϣ�ṹ�壬�����Ϣ���ࡢ��Ϣ���ݡ�
typedef struct tagStructMessageInfo
{
	/*��Ϣ����*/
	int iMessageType;
	/*��Ϣ����*/
	char achMessage[MESSAGE_LEN_256];
} LST_MESSAGE_INFO;

//�����Ϣ�ṹ�壬���������������ID�����SDK�İ汾�š�����ӿڡ�������кš�Ԥ���ֶΡ�
typedef struct tagStructCameraInfo
{
	/*�������*/
	int iCameraIndex;
	/*���ID*/
	char achCameraID[128];
	/*���SDK�İ汾��*/
	char achSdkVersion[128];
	/*����Ľӿڣ��μ�LE_CAMERA_INTERFACE_TYPEö��*/
	int  iCameraInterface;
	/*������к�*/
	char achSerialNumber[64];
	/*Ԥ���ֶ� Ranger3������Ϊ���mac��ַ*/
	char achReserved[128];
} LST_CAMERA_INFO;

//�ɼ�������Ϣ�ṹ�壬�����������������ϻ���ĸ�����
typedef struct tagStructBufferInfo
{
	/*��������*/
	int iBufferNum;
	/*���ϻ���ĸ�����Ĭ����1*/
	int iMergeFrameNum;
	/*Ԥ���ֶ�*/
	char achReserved[128];
} LST_BUFFER_INFO;

/*
*Function Ptr
*/

//�ص�����ָ�롣���ص����Ϳ��԰���
//(1)�ɼ��洢�ص�����
//(2)�ɼ���ʾ�ص�����
//(3)�ɼ���Ϣ�ص�����
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
	@����˵���� ����������Ŀ�Լ��豸��Ϣ��
	@�������룺 ��
	@��������� pstCameraInfo -- �����Ϣ�ṹ��ָ�룬ָ��ָ�������Ϣ�ṹ�����飬������Ŷ�̨����������Ϣ��
	iCameraNum -- ��⵽�����������
	@����ֵ  �� ��LE_STATUSö��
	******************************************************/
	LCC_C_API LE_STATUS __stdcall LCC_C_DetectCameraNum(LST_CAMERA_INFO **pstCameraInfo, int *piCameraNum);

	/******************************************************
	@����˵���� �������������ȡ���xml�����ļ���������������Դ���󡢼�����λ��ϵͳ���������ɼ�����
	��������Ĵ�������������ʼ����������ȡ�
	@�������룺 iCameraIndex -- ��Ҫ�����������������ţ���0��ʼ��
	pchXmlFileName -- ��Ҫ���������xml�����ĵ��ľ���·����
	@��������� pCameraHandle -- ������ָ�롣����򿪳ɹ�������������ָ�룬�����ʧ����ΪĬ��ֵNULL��0����
	@����ֵ  �� ��LE_STATUSö��
	******************************************************/
	LCC_C_API LE_STATUS __stdcall LCC_C_OpenCamera(int iCameraIndex, const char *pchXmlFileName, void **pCameraHandle);

	/******************************************************
	@����˵���� �ر�������ͷ���������Դ����������λ��ϵͳ�봫���������Ĵ����������������ز�����
	@�������룺 pCameraHandle -- ������ָ�롣
	@��������� ��
	@����ֵ  �� ��LE_STATUSö��
	******************************************************/
	LCC_C_API LE_STATUS __stdcall LCC_C_CloseCamera(void *pCameraHandle);

	/******************************************************
	@����˵���� ��ʼ���������Ҫ���ݸ���������Ϣ������������Ϣ��������������ʼ���ײ�����ݽṹ�塢�������ݿ�ȡ�
	ע�⣺�˴������ݽṹ���뻺�����ݿ���Ҫ��Luster SDK��PCϵͳ�ڴ��д����ģ��������ڲɼ���������д����ġ�
	@�������룺 pCameraHandle -- ������ָ�롣
	stBufferInfo -- ������Ϣ�ṹ�塣
	@��������� ��
	@����ֵ  �� ��LE_STATUSö��
	******************************************************/
	LCC_C_API LE_STATUS __stdcall LCC_C_InitCamera(void *pCameraHandle, LST_BUFFER_INFO stBufferInfo);

	/******************************************************
	@����˵���� �ͷ�������ͷŵײ�����ݽṹ�塢�������ݿ�ȡ�
	@�������룺 pCameraHandle -- ������ָ�롣
	@��������� ��
	@����ֵ  �� ��LE_STATUSö��
	******************************************************/
	LCC_C_API LE_STATUS __stdcall LCC_C_ReleaseCamera(void *pCameraHandle);

	/******************************************************
	@����˵���� ��ʼ����ɼ���������ͺ��������вɼ��źţ��ڴ������ⴥ�������������ʼ��ɼ�����������ͼ�����ݡ�
	ע�⣺��ʼ�ɼ������ע���˻ص��¼������ԴӸûص������еõ��ɼ�����ͼ�����ݡ�
	@�������룺 pCameraHandle -- ������ָ�롣
	@��������� ��
	@����ֵ  �� ��LE_STATUSö��
	******************************************************/
	LCC_C_API LE_STATUS __stdcall LCC_C_StartGrab(void *pCameraHandle);

	/******************************************************
	@����˵���� ֹͣ����ɼ���������ͺ������ֹͣ��ɼ�����������ͼ�����ݡ�
	@�������룺 pCameraHandle -- ������ָ�롣
	@��������� ��
	@����ֵ  �� ��LE_STATUSö��
	******************************************************/
	LCC_C_API LE_STATUS __stdcall LCC_C_StopGrab(void *pCameraHandle);

	/******************************************************
	@����˵���� �����֡�ɼ���LusterSnapImage��LusterStartGrab�������ƣ���Snapֻץȡһ֡ͼ����ֹͣץȡ��
	ץͼ�����лص�����������һ�Ρ��ú�����Ҫ�ڵ��Թ�����ʹ�á�
	@�������룺 pCameraHandle -- ������ָ�롣
	@��������� ��
	@����ֵ  �� ��LE_STATUSö��
	******************************************************/
	LCC_C_API LE_STATUS __stdcall LCC_C_SnapImage(void *pCameraHandle);

	/******************************************************
	@����˵���� ע��ص���������LusterCameraCallback�ص�����ָ��ע���������У��û���ͨ���˻ص��������ͼ�����ݡ�
	@�������룺 iCallbackType -- ע��Ļص����������ͣ��������ɼ��洢���ͣ�������ȫ�����ݣ���ʾ�ص����ͣ����ܻ��֡���з������ݣ�
	�ɼ���Ϣ���ͣ����LE_CALLBACK_TYPEö�٣��Լ�LusterCameraCallback˵����
	pCallback     -- �ص�����ָ�롣
	pCameraHandle -- ������ָ�롣
	pUserContext  -- �û���������Ϣ��ע�⣺C++��һ�����ڸ����û����ָ�룬C#�п��Բ���ע��ֵ��
	����֮�⣬�������ø�ָ�봫������������������ݡ�
	@��������� ��
	@����ֵ  �� ��LE_STATUSö��
	******************************************************/
	LCC_C_API LE_STATUS __stdcall LCC_C_RegisterCallback(LCC_C_Callback pCallback, int iCallbackType, void *pCameraHandle, void *pUserContext);

	/******************************************************
	@����˵���� ע���ص�������ȡ����LusterRegisterCallbackע��Ļص��¼���
	@�������룺 iCallbackType -- ָ����Ҫע��Ļص����������͡����LE_CALLBACK_TYPEö��˵����
	LusterCameraCallback�ص�����ָ��˵����LusterRegisterCallback�ӿ�˵����
	pCameraHandle -- ������ָ�롣
	@��������� ��
	@����ֵ  �� ��LE_STATUSö��
	******************************************************/
	LCC_C_API LE_STATUS __stdcall LCC_C_UnregisterCallback(int iCallbackType, void *pCameraHandle);

	/******************************************************
	@����˵���� ��ȡ����������������͡�
	@�������룺 pCameraHandle -- ������ָ�롣
	pchFeatureName -- ����������ƣ��μ�LFNC�ĵ���
	@��������� stFeatureInfo -- ������Ϣ�ṹ�壬�μ�LST_FEATURE_INFO��
	@����ֵ  �� ��LE_STATUSö��
	******************************************************/
	LCC_C_API LE_STATUS __stdcall LCC_C_GetFeatureInfo(void *pCameraHandle, const char *pchFeatureName, LST_FEATURE_INFO *pstFeatureInfo);

	/******************************************************
	@����˵���� ��ȡ����������������ͣ���
	@�������룺 pCameraHandle -- ������ָ�롣
	pchFeatureName -- ����������ƣ��μ�LFNC�ĵ���
	@��������� stFeatureValue -- ����������Ϣ�ṹ�壬�μ�LST_INT_INFO��
	@����ֵ  �� ��LE_STATUSö��
	******************************************************/
	LCC_C_API LE_STATUS __stdcall LCC_C_GetFeatureInt(void *pCameraHandle, const char *pchFeatureName, LST_INT_INFO *pstFeatureValue);

	/******************************************************
	@����˵���� ��ȡ����������������ͣ���
	@�������룺 pCameraHandle -- ������ָ�롣
	pchFeatureName -- ����������ƣ��μ�LFNC�ĵ���
	@��������� stFeatureValue -- ������������Ϣ�ṹ�壬�μ�LST_BOOLEN_INFO��
	@����ֵ  �� ��LE_STATUSö��
	******************************************************/
	LCC_C_API LE_STATUS __stdcall LCC_C_GetFeatureBoolean(void *pCameraHandle, const char *pchFeatureName, LST_BOOLEAN_INFO *pstFeatureValue);

	/******************************************************
	@����˵���� ��ȡ���������˫���ȸ������ͣ���
	@�������룺 pCameraHandle -- ������ָ�롣
	pchFeatureName -- ����������ƣ��μ�LFNC�ĵ���
	@��������� stFeatureValue -- ������������Ϣ�ṹ�壬�μ�LST_DOUBLE_INFO��
	@����ֵ  �� ��LE_STATUSö��
	******************************************************/
	LCC_C_API LE_STATUS __stdcall LCC_C_GetFeatureDouble(void *pCameraHandle, const char *pchFeatureName, LST_DOUBLE_INFO *pstFeatureValue);

	/******************************************************
	@����˵���� ��ȡ����������ַ������ͣ���
	@�������룺 pCameraHandle -- ������ָ�롣
	pchFeatureName -- ����������ƣ��μ�LFNC�ĵ���
	@��������� stFeatureValue -- �ַ�����������Ϣ�ṹ�壬�μ�LST_STRING_INFO��
	@����ֵ  �� ��LE_STATUSö��
	******************************************************/
	LCC_C_API LE_STATUS __stdcall LCC_C_GetFeatureString(void *pCameraHandle, const char *pchFeatureName, LST_STRING_INFO *pstFeatureValue);

	/******************************************************
	@����˵���� ��ȡ���������ö�����ͣ���
	@�������룺 pCameraHandle -- ������ָ�롣
	pchFeatureName -- ����������ƣ��μ�LFNC�ĵ���
	@��������� stFeatureValue -- ö����������Ϣ�ṹ�壬�μ�LST_ENUM_INFO��
	@����ֵ  �� ��LE_STATUSö��
	******************************************************/
	LCC_C_API LE_STATUS __stdcall LCC_C_GetFeatureEnum(void *pCameraHandle, const char *pchFeatureName, LST_ENUM_INFO *pstFeatureValue);

	/******************************************************
	@����˵���� ��ȡ�����������ȷ���������ͣ����ýӿ���Ҫ�������ݲ��������������ͣ���˲���void*�Ĳ����������ͣ�
	�û�������������볣��������ָ��������������顢�ṹ����������ݣ������ʹ��ע����������ݾ����������д���
	��ο���Ӧ�ͺŵ�������ο���ʹ��˵���顣
	@�������룺 pCameraHandle -- ������ָ�롣
	pchFeatureName -- ����������ƣ��μ�LFNC�ĵ���
	@��������� pFeatureValue -- �������ֵ���������ͣ�����������ⶨ�塣
	@����ֵ  �� ��LE_STATUSö��
	******************************************************/
	LCC_C_API LE_STATUS __stdcall LCC_C_GetFeatureUncertain(void *pCameraHandle, const char *pchFeatureName, void **pFeatureValue);

	/******************************************************
	@����˵���� ��������������������ͣ���
	@�������룺 pCameraHandle -- ������ָ�롣
	pchFeatureName -- ����������ƣ��μ�LFNC�ĵ���
	iFeatureValue -- �������ֵ��
	@��������� ��
	@����ֵ  �� ��LE_STATUSö��
	******************************************************/
	LCC_C_API LE_STATUS __stdcall LCC_C_SetFeatureInt(void *pCameraHandle, const char *pchFeatureName, int iFeatureValue);

	/******************************************************
	@����˵���� ��������������������ͣ���
	@�������룺 pCameraHandle -- ������ָ�롣
	pchFeatureName -- ����������ƣ��μ�LFNC�ĵ���
	bFeatureValue -- �������ֵ��
	@��������� ��
	@����ֵ  �� ��LE_STATUSö��
	******************************************************/
	LCC_C_API LE_STATUS __stdcall LCC_C_SetFeatureBoolean(void *pCameraHandle, const char *pchFeatureName, bool bFeatureValue);

	/******************************************************
	@����˵���� �������������˫���ȸ������ͣ���
	@�������룺 pCameraHandle -- ������ָ�롣
	pchFeatureName -- ����������ƣ��μ�LFNC�ĵ���
	dFeatureValue -- �������ֵ��
	@��������� ��
	@����ֵ  �� ��LE_STATUSö��
	******************************************************/
	LCC_C_API LE_STATUS __stdcall LCC_C_SetFeatureDouble(void *pCameraHandle, const char *pchFeatureName, double dFeatureValue);

	/******************************************************
	@����˵���� ��������������ַ������ͣ���
	@�������룺 pCameraHandle -- ������ָ�롣
	pchFeatureName -- ����������ƣ��μ�LFNC�ĵ���
	pchFeatureValue -- �������ֵ��
	@��������� ��
	@����ֵ  �� ��LE_STATUSö��
	******************************************************/
	LCC_C_API LE_STATUS __stdcall LCC_C_SetFeatureString(void *pCameraHandle, const char *pchFeatureName, const char *pchFeatureValue);

	/******************************************************
	@����˵���� �������������ö�����ͣ���
	@�������룺 pCameraHandle -- ������ָ�롣
	pchFeatureName -- ����������ƣ��μ�LFNC�ĵ���
	iFeatureValue -- �������ֵ��
	@��������� ��
	@����ֵ  �� ��LE_STATUSö��
	******************************************************/
	LCC_C_API LE_STATUS __stdcall LCC_C_SetFeatureEnum(void *pCameraHandle, const char *pchFeatureName, int iFeatureValue);

	/******************************************************
	@����˵���� ���������������ȷ���������ͣ����ýӿ���Ҫ�������ݲ��������������ͣ���˲���void*�Ĳ����������ͣ�
	�û�������������볣��������ָ��������������顢�ṹ����������ݣ������ʹ��ע����������ݾ����������д���
	��ο���Ӧ�ͺŵ�������ο���ʹ��˵���顣
	@�������룺 pCameraHandle -- ������ָ�롣
	pFeatureName -- ����������ƣ��μ�LFNC�ĵ���
	pFeatureValue -- �������ֵ��
	@��������� ��
	@����ֵ  �� ��LE_STATUSö��
	******************************************************/
	LCC_C_API LE_STATUS __stdcall LCC_C_SetFeatureUncertain(void *pCameraHandle, const char *pchFeatureName, void *pFeatureValue);

	/******************************************************
	@����˵���� ���������
	@�������룺 pCameraHandle -- ������ָ�롣
	@��������� ��
	@����ֵ  �� ��LE_STATUSö��
	******************************************************/
	LCC_C_API LE_STATUS __stdcall LCC_C_RestartCamera(void *pCameraHandle);

	/******************************************************
	@����˵���� �ָ��������á�
	@�������룺 pCameraHandle -- ������ָ�롣
	@��������� ��
	@����ֵ  �� ��LE_STATUSö��
	******************************************************/
	LCC_C_API LE_STATUS __stdcall LCC_C_RestoreFactory(void *pCameraHandle);
	/******************************************************
	@����˵���� ����csv�����ļ���
	@�������룺 index -- ���������
	pchCSVName--csv���ƣ���·����
	@��������� ��
	@����ֵ  �� ��LE_STATUSö��
	******************************************************/
	//LCC_C_API LE_STATUS __stdcall LCC_C_LoadCSV(int index, const char *pchCSVName);



#ifdef __cplusplus
}
#endif


#endif   //LST_CAMERA_CONTROL_C_H_