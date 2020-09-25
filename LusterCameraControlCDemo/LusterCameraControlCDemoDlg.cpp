//// LusterCameraControlCDemoDlg.cpp : ʵ���ļ�
//
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
* DATE 		    :  2017/12/06
* AUTHOR 		:  �׵�
* NOTE 		    :  �����汾��Ranger3-1.1.3.1396
*				   ������̣�Sick
*				   ����ͺţ�Ranger3
*				   ��Ӧ�ӿ��ĵ������SDK�������ͳһ�ӿ���ϸ���˵��(��׼C����)_v0.1.2.0����
*
*
* VERSION       :  1.0.0
* DATE 		    :  2018/03/15
* AUTHOR 		:  �׵�
* NOTE 		    :  �����汾��Ranger3-1.1.3.1497
*				   ������̣�Sick
*				   ����ͺţ�Ranger3
*                  ��Ӧ�ӿ��ĵ���LSDK�������ͳһ�ӿ�˵��(��׼C����)_v1.0.0����
*                  1.����״̬�ж�m_bDetected��m_bOpened��m_bInited��m_bGrabbed
*                  2.PopImageFromBuffer��������ָ��Ϊ���ж�
*                  3.ͷ�ļ����Ӵ�������L_ERR_CCOED��L_ERR_CCIED��L_ERR_CCNIED��L_ERR_CCGED��L_ERR_CCNGED
*                  4.��ӵ��������ļ��ӿڣ�LCC_C_SetFeatureString
*                  5.���ʵʱ�궨������궨���ͼ��
*                  6.��Ӳ������ù��ܣ�ѡ���ͼΪRange+Reflectance��Rangeģʽ

VERSION       :  1.0.0
* DATE 		    :  2018/05/29
* AUTHOR 		:  �׵�
* NOTE 		    :  �����汾��Ranger3-1.1.3.1497
*				   ������̣�Sick
*				   ����ͺţ�Ranger3
*                  ��Ӧ�ӿ��ĵ���LSDK�������ͳһ�ӿ�˵��(��׼C����)_v1.0.0����
*                  1.����ڴ�й¶���⣨���൱ǰdemo��ʾ���ڴ�й¶ΪMFC�Դ������ÿ���̨������Թ���
*                  2.�޸�����·��λ��
*                  3.���2Dͼ�������ɼ���˸������
*                  4.��ӡ�ShowErrorInfo��
***********************************************************************/


#include "stdafx.h"
#include "afxdialogex.h"
#include <algorithm>
#include <stdint.h>
#include "LusterCameraControlCDemo.h"
#include "LusterCameraControlCDemoDlg.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "DatXmlWriter.h"
#include "SavePath.h"
#include "FeatureSet.h"
#include "LstLfncDef.h"
#include <vector>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CImage mImage;//��ʾͼ��
bool bBrushRectRanger3 = true;//�Ƿ�ˢ����ʾ����-Ranger3

//Region1ģʽ�µ�ͼ������ֵ
int mm_height3D;
int mm_offsetX3D;
int mm_offsetY3D;

int m_iRecord = 0;//�Ƿ��ͼ�������̣���ʼ��Ϊ0��1�����̵����أ�2������ͼ
bool m_Image = false;//ͼ��ģʽ��true->imgae,false->range

//��ͼ·����ͼ������
std::string m_path = "C:\\IMAGE\\ranger3";
std::string m_CalibratePath = "C:\\IMAGE\\Calibrate";
std::string m_RedifyPath = "C:\\IMAGE\\Redify";
std::string m_CalibrateAndRedifyPath = "C:\\IMAGE\\CalibrateAndRedify";
std::string m_fileName = "Image";
std::string m_CalibrateName = "Calibrate";
std::string m_RedifyName = "Redify";
std::string m_CalibratePathAndRedifyName = "CalibrateAndRedify";

//ȫ�ֺ���
void writeSensorRangeTraits(DatXmlWriter& xml, const int64_t bufferWidth,
	const int64_t aoiHeight, const int64_t aoiOffsetX,
	const int64_t aoiOffsetY);
void writeSubComponentRange16(DatXmlWriter& xml, const int64_t bufferWidth);
void writeSubComponentReflectance(DatXmlWriter& xml, const int64_t bufferWidth);

// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// CLusterCameraControlCDemoDlg �Ի���
CLusterCameraControlCDemoDlg::CLusterCameraControlCDemoDlg(CWnd* pParent /*=NULL*/)
: CDialogEx(IDD_LUSTERCAMERACONTROLCDEMO_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CLusterCameraControlCDemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PICTURE_BOX, m_PictureBox);
	DDX_Control(pDX, IDC_RADIO_Image, f_Image);
	DDX_Control(pDX, IDC_RADIO_Range, f_Range);
	DDX_Control(pDX, IDC_EDIT_SavePath, f_SavePath);
}

BEGIN_MESSAGE_MAP(CLusterCameraControlCDemoDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(ID_BTN_CamDetectNum, &CLusterCameraControlCDemoDlg::OnBnClickedBtnCamdetectnum)
	ON_BN_CLICKED(ID_BTN_OpenCamera, &CLusterCameraControlCDemoDlg::OnBnClickedBtnOpencamera)
	ON_BN_CLICKED(ID_BTN_CloseCamera, &CLusterCameraControlCDemoDlg::OnBnClickedBtnClosecamera)
	ON_BN_CLICKED(ID_BTN_StartGrab, &CLusterCameraControlCDemoDlg::OnBnClickedBtnStartgrab)
	ON_BN_CLICKED(ID_BTN_StopGrab, &CLusterCameraControlCDemoDlg::OnBnClickedBtnStopgrab)
	ON_BN_CLICKED(ID_BTN_Snap, &CLusterCameraControlCDemoDlg::OnBnClickedBtnSnap)
	ON_BN_CLICKED(ID_BTN_StartRecord, &CLusterCameraControlCDemoDlg::OnBnClickedBtnStartrecord)
	ON_BN_CLICKED(ID_BTN_StopRecord, &CLusterCameraControlCDemoDlg::OnBnClickedBtnStoprecord)
	ON_BN_CLICKED(IDC_BTN_FeatureSet, &CLusterCameraControlCDemoDlg::OnBnClickedBtnFeatureset)
	ON_BN_CLICKED(IDC_RADIO_Image, &CLusterCameraControlCDemoDlg::OnBnClickedRadioImage)
	ON_BN_CLICKED(IDC_RADIO_Range, &CLusterCameraControlCDemoDlg::OnBnClickedRadioRange)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON_ChangeSavePath, &CLusterCameraControlCDemoDlg::OnBnClickedButtonChangesavepath)
END_MESSAGE_MAP()

// CLusterCameraControlCDemoDlg ��Ϣ�������
BOOL CLusterCameraControlCDemoDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	
	//Ĭ������Ϊ��ʾRangeͼ��
	f_Image.SetCheck(0);
	f_Range.SetCheck(1);

	//��ʼ���洢·��
	GetDlgItem(IDC_EDIT_SavePath)->SetWindowText(m_path.c_str());

	//������ʼ��
	GetDlgItem(ID_BTN_CamDetectNum)->EnableWindow(TRUE);
	GetDlgItem(ID_BTN_OpenCamera)->EnableWindow(FALSE);
	GetDlgItem(ID_BTN_CloseCamera)->EnableWindow(FALSE);
	GetDlgItem(ID_BTN_Snap)->EnableWindow(FALSE);
	GetDlgItem(ID_BTN_StartGrab)->EnableWindow(FALSE);
	GetDlgItem(ID_BTN_StopGrab)->EnableWindow(FALSE);
	GetDlgItem(ID_BTN_StartRecord)->EnableWindow(TRUE);
	GetDlgItem(ID_BTN_StopRecord)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_FeatureSet)->EnableWindow(FALSE);
	
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CLusterCameraControlCDemoDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CLusterCameraControlCDemoDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CLusterCameraControlCDemoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

/******************************************************
@����˵���� ���ڹر��¼�
@�������룺 ��
@��������� ��
@����ֵ  �� ��
******************************************************/
void CLusterCameraControlCDemoDlg::OnClose()
{
	CDialogEx::OnClose();
}


//@����˵���� ������
void CLusterCameraControlCDemoDlg::OnBnClickedBtnCamdetectnum()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	LE_STATUS eResult = LCC_C_DetectCameraNum(m_pstCameraInfo, &m_iCameraNum);
	if (eResult == L_OK)
	{
		CString str;
		str.Format("Ŀǰ������  %d  �����", m_iCameraNum);
		AfxMessageBox(str);
	}
	else
	{
		ShowErrorInfo(eResult, "�������ʧ��");
		return;
	}

	//����ʹ��
	if (m_iCameraNum > 0)
	{
		GetDlgItem(ID_BTN_CamDetectNum)->EnableWindow(FALSE);
		GetDlgItem(ID_BTN_OpenCamera)->EnableWindow(TRUE);
		GetDlgItem(ID_BTN_CloseCamera)->EnableWindow(FALSE);
		GetDlgItem(ID_BTN_Snap)->EnableWindow(FALSE);
		GetDlgItem(ID_BTN_StartGrab)->EnableWindow(FALSE);
		GetDlgItem(ID_BTN_StopGrab)->EnableWindow(FALSE);
		GetDlgItem(IDC_BTN_FeatureSet)->EnableWindow(FALSE);
	}
}

//@����˵���� �����
void CLusterCameraControlCDemoDlg::OnBnClickedBtnOpencamera()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	int iCameraIndex = 0;
	LE_STATUS eResult;

	eResult = LCC_C_OpenCamera(iCameraIndex, "..\\XmlFile\\LCC_C_Sick_Ranger3_1.0.0.xml", &m_pCameraHandle[iCameraIndex]);
	LCC_C_SetFeatureString(m_pCameraHandle[iCameraIndex], LFNC_SICK_PARAMETPATH, "");
	
	//LCC_C_LoadCSV(0, "1.csv");

	return;

	if (eResult == L_OK)
	{
		AfxMessageBox("�����0�ɹ�!");

		m_pstBufferInfo[iCameraIndex] = (LST_BUFFER_INFO*)malloc(sizeof(LST_BUFFER_INFO));
		m_pstBufferInfo[iCameraIndex]->iBufferNum = 5;
		m_pstBufferInfo[iCameraIndex]->iMergeFrameNum = 1;
		m_pstBufferInfo[iCameraIndex]->achReserved[0] = '\0';
		eResult = LCC_C_InitCamera(m_pCameraHandle[iCameraIndex], *(m_pstBufferInfo[iCameraIndex]));
		if (eResult == L_OK)
		{
			AfxMessageBox("��ʼ������ɹ�!");

			LCC_C_RegisterCallback(StoreCallBack, CB_STORE, m_pCameraHandle[iCameraIndex], this);
			LCC_C_RegisterCallback(DisplayCallBack, CB_DISPLAY, m_pCameraHandle[iCameraIndex], this);
			LCC_C_RegisterCallback(MessageCallBack, CB_MESSAGE, m_pCameraHandle[iCameraIndex], this);

			//����ʹ��
			GetDlgItem(ID_BTN_CamDetectNum)->EnableWindow(FALSE);
			GetDlgItem(ID_BTN_OpenCamera)->EnableWindow(FALSE);
			GetDlgItem(ID_BTN_CloseCamera)->EnableWindow(TRUE);
			GetDlgItem(ID_BTN_Snap)->EnableWindow(TRUE);
			GetDlgItem(ID_BTN_StartGrab)->EnableWindow(TRUE);
			GetDlgItem(ID_BTN_StopGrab)->EnableWindow(FALSE);
			GetDlgItem(IDC_BTN_FeatureSet)->EnableWindow(TRUE);
		}
		else
		{
			ShowErrorInfo(eResult, "��ʼ�����ʧ��");
		}

	}
	else
	{
		ShowErrorInfo(eResult, "�����ʧ��");
	}

	//LCC_C_LoadCSV(0, "1.csv");
	LST_BOOLEAN_INFO b;
	b.bValue = false;
	eResult = LCC_C_GetFeatureBoolean(m_pCameraHandle[iCameraIndex], LFNC_SICK_REFLECTANCE_ENABLE, &b);
	//mm_height3D = stiIntInfo.iValue;



	eResult = LCC_C_SetFeatureEnum(m_pCameraHandle[iCameraIndex], LFNC_SICK_REGION_SELECTOR, 1);
	if (eResult != L_OK)
	{
		ShowErrorInfo(eResult, "����ΪRegion1ģʽʧ��");
	}

	//��ȡͼ��߶�;
	LST_INT_INFO stiIntInfo;
	eResult = LCC_C_GetFeatureInt(m_pCameraHandle[iCameraIndex], LFNC_HEIGHT, &stiIntInfo);
	mm_height3D = stiIntInfo.iValue;

	//��ȡͼ��offsety;
	eResult = LCC_C_GetFeatureInt(m_pCameraHandle[iCameraIndex], LFNC_OFFSET_Y, &stiIntInfo);
	mm_offsetY3D = stiIntInfo.iValue;

	//��ȡͼ��offsetx;
	eResult = LCC_C_GetFeatureInt(m_pCameraHandle[iCameraIndex], LFNC_OFFSET_X, &stiIntInfo);
	mm_offsetX3D = stiIntInfo.iValue;



	//iCameraIndex = 1;
	//eResult = LCC_C_OpenCamera(iCameraIndex, "..\\XmlFile\\LCC_C_Sick_Ranger3_1.0.0.xml", &m_pCameraHandle[iCameraIndex]);

	//if (eResult == L_OK)
	//{
	//	AfxMessageBox("�����1�ɹ�!");

	//	m_pstBufferInfo[iCameraIndex] = (LST_BUFFER_INFO*)malloc(sizeof(LST_BUFFER_INFO));
	//	m_pstBufferInfo[iCameraIndex]->iBufferNum = 5;
	//	m_pstBufferInfo[iCameraIndex]->iMergeFrameNum = 1;
	//	m_pstBufferInfo[iCameraIndex]->achReserved[0] = '\0';
	//	eResult = LCC_C_InitCamera(m_pCameraHandle[iCameraIndex], *(m_pstBufferInfo[iCameraIndex]));
	//	if (eResult == L_OK)
	//	{
	//		AfxMessageBox("��ʼ������ɹ�!");

	//		LCC_C_RegisterCallback(StoreCallBack, CB_STORE, m_pCameraHandle[iCameraIndex], this);
	//		LCC_C_RegisterCallback(DisplayCallBack, CB_DISPLAY, m_pCameraHandle[iCameraIndex], this);
	//		LCC_C_RegisterCallback(MessageCallBack, CB_MESSAGE, m_pCameraHandle[iCameraIndex], this);

	//		//����ʹ��
	//		GetDlgItem(ID_BTN_CamDetectNum)->EnableWindow(FALSE);
	//		GetDlgItem(ID_BTN_OpenCamera)->EnableWindow(FALSE);
	//		GetDlgItem(ID_BTN_CloseCamera)->EnableWindow(TRUE);
	//		GetDlgItem(ID_BTN_Snap)->EnableWindow(TRUE);
	//		GetDlgItem(ID_BTN_StartGrab)->EnableWindow(TRUE);
	//		GetDlgItem(ID_BTN_StopGrab)->EnableWindow(FALSE);
	//		GetDlgItem(IDC_BTN_FeatureSet)->EnableWindow(TRUE);
	//	}
	//	else
	//	{
	//		ShowErrorInfo(eResult, "��ʼ�����ʧ��");
	//	}

	//}
	//else
	//{
	//	ShowErrorInfo(eResult, "�����ʧ��");
	//}
	//LCC_C_LoadCSV(1, "1.csv");

	////���������ļ���sick.csv����������Ҫ���������ļ����ɲ����ô˽ӿڣ�
	//char *inputPath = "..\\XmlFile\\true300.csv";//�����������ļ�ʧ�ܣ����ܴ����ļ��𻵵����������sick�ɼ��������������
	//eResult = LCC_C_SetFeatureString(m_pCameraHandle[iCameraIndex], LFNC_SICK_CONFIGURATION, inputPath);
	//if (eResult != L_OK)
	//{
	//	ShowErrorInfo(eResult, "���������ļ�ʧ��");
	//}
	/*eResult = LCC_C_SetFeatureEnum(m_pCameraHandle[iCameraIndex], LFNC_SICK_TRIGGER_MODE_SELECTOR, 1);
	eResult = LCC_C_SetFeatureEnum(m_pCameraHandle[iCameraIndex], LFNC_TRIG_MODE, 1);
	if (eResult != L_OK)
	{
		AfxMessageBox("����Ϊ�ⴥ��ʧ�ܣ�");
	}*/

	//��ȡRegion1ģʽ�µĲ���
	eResult = LCC_C_SetFeatureEnum(m_pCameraHandle[iCameraIndex], LFNC_SICK_REGION_SELECTOR, 1);
	if (eResult != L_OK)
	{
		ShowErrorInfo(eResult, "����ΪRegion1ģʽʧ��");
	}

	//��ȡͼ��߶�;
	//LST_INT_INFO stiIntInfo;
	eResult = LCC_C_GetFeatureInt(m_pCameraHandle[iCameraIndex], LFNC_HEIGHT, &stiIntInfo);
	mm_height3D = stiIntInfo.iValue;

	//��ȡͼ��offsety;
	eResult = LCC_C_GetFeatureInt(m_pCameraHandle[iCameraIndex], LFNC_OFFSET_Y, &stiIntInfo);
	mm_offsetY3D = stiIntInfo.iValue;

	//��ȡͼ��offsetx;
	eResult = LCC_C_GetFeatureInt(m_pCameraHandle[iCameraIndex], LFNC_OFFSET_X, &stiIntInfo);
	mm_offsetX3D = stiIntInfo.iValue;

}

//@����˵���� ��ʼ�����ɼ�
void CLusterCameraControlCDemoDlg::OnBnClickedBtnStartgrab()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	int iCameraIndex = 0;
	bBrushRectRanger3 = true;//ˢ��һ����ʾ����

	if (m_pCameraHandle[iCameraIndex] != NULL)
	{
		/*ֹͣ�ɼ�*/
		LE_STATUS eResult = LCC_C_StartGrab(m_pCameraHandle[iCameraIndex]);

		if (eResult == L_OK)
		{
			//����ʹ��
			GetDlgItem(ID_BTN_CamDetectNum)->EnableWindow(FALSE);
			GetDlgItem(ID_BTN_OpenCamera)->EnableWindow(FALSE);
			GetDlgItem(ID_BTN_CloseCamera)->EnableWindow(FALSE);
			GetDlgItem(ID_BTN_Snap)->EnableWindow(FALSE);
			GetDlgItem(ID_BTN_StartGrab)->EnableWindow(FALSE);
			GetDlgItem(ID_BTN_StopGrab)->EnableWindow(TRUE);
			GetDlgItem(IDC_BTN_FeatureSet)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_Image)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_Range)->EnableWindow(FALSE);
			GetDlgItem(IDC_BUTTON_ChangeSavePath)->EnableWindow(FALSE);
		}
		else
		{
			ShowErrorInfo(eResult, "��ʼ�ɼ�ʧ��");
		}
	}


	//iCameraIndex = 0;
	//bBrushRectRanger3 = true;//ˢ��һ����ʾ����

	//if (m_pCameraHandle[iCameraIndex] != NULL)
	//{
	//	/*ֹͣ�ɼ�*/
	//	LE_STATUS eResult = LCC_C_StartGrab(m_pCameraHandle[iCameraIndex]);

	//	if (eResult == L_OK)
	//	{
	//		//����ʹ��
	//		GetDlgItem(ID_BTN_CamDetectNum)->EnableWindow(FALSE);
	//		GetDlgItem(ID_BTN_OpenCamera)->EnableWindow(FALSE);
	//		GetDlgItem(ID_BTN_CloseCamera)->EnableWindow(FALSE);
	//		GetDlgItem(ID_BTN_Snap)->EnableWindow(FALSE);
	//		GetDlgItem(ID_BTN_StartGrab)->EnableWindow(FALSE);
	//		GetDlgItem(ID_BTN_StopGrab)->EnableWindow(TRUE);
	//		GetDlgItem(IDC_BTN_FeatureSet)->EnableWindow(FALSE);
	//		GetDlgItem(IDC_RADIO_Image)->EnableWindow(FALSE);
	//		GetDlgItem(IDC_RADIO_Range)->EnableWindow(FALSE);
	//		GetDlgItem(IDC_BUTTON_ChangeSavePath)->EnableWindow(FALSE);
	//	}
	//	else
	//	{
	//		ShowErrorInfo(eResult, "��ʼ�ɼ�ʧ��");
	//	}
	//}


}

//@����˵���� ֹͣ�����ɼ�
void CLusterCameraControlCDemoDlg::OnBnClickedBtnStopgrab()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	int iCameraIndex = 0;
	LE_STATUS eResult;

	if (m_pCameraHandle[iCameraIndex] != NULL)
	{
		/*ֹͣ�ɼ�*/
		eResult = LCC_C_StopGrab(m_pCameraHandle[iCameraIndex]);
		if (eResult != L_OK)
		{
			ShowErrorInfo(eResult, "ֹͣ�ɼ�ʧ��");
		}
		else
		{
			//AfxMessageBox("ֹͣ���0�ɹ�!");

			//����ʹ��
			GetDlgItem(ID_BTN_CamDetectNum)->EnableWindow(FALSE);
			GetDlgItem(ID_BTN_OpenCamera)->EnableWindow(FALSE);
			GetDlgItem(ID_BTN_CloseCamera)->EnableWindow(TRUE);
			GetDlgItem(ID_BTN_Snap)->EnableWindow(TRUE);
			GetDlgItem(ID_BTN_StartGrab)->EnableWindow(TRUE);
			GetDlgItem(ID_BTN_StopGrab)->EnableWindow(FALSE);
			GetDlgItem(IDC_BTN_FeatureSet)->EnableWindow(TRUE);
			GetDlgItem(IDC_RADIO_Image)->EnableWindow(TRUE);
			GetDlgItem(IDC_RADIO_Range)->EnableWindow(TRUE);
			GetDlgItem(IDC_BUTTON_ChangeSavePath)->EnableWindow(TRUE);
		}
	}

	//LCC_C_ReleaseCamera(m_pCameraHandle[iCameraIndex]);
	//LCC_C_CloseCamera(m_pCameraHandle[iCameraIndex]);



	//iCameraIndex = 0;

	//if (m_pCameraHandle[iCameraIndex] != NULL)
	//{
	//	/*ֹͣ�ɼ�*/
	//	eResult = LCC_C_StopGrab(m_pCameraHandle[iCameraIndex]);
	//	if (eResult != L_OK)
	//	{
	//		ShowErrorInfo(eResult, "ֹͣ�ɼ�ʧ��");
	//	}
	//	else
	//	{
	//		//AfxMessageBox("ֹͣ���0�ɹ�!");

	//		//����ʹ��
	//		GetDlgItem(ID_BTN_CamDetectNum)->EnableWindow(FALSE);
	//		GetDlgItem(ID_BTN_OpenCamera)->EnableWindow(FALSE);
	//		GetDlgItem(ID_BTN_CloseCamera)->EnableWindow(TRUE);
	//		GetDlgItem(ID_BTN_Snap)->EnableWindow(TRUE);
	//		GetDlgItem(ID_BTN_StartGrab)->EnableWindow(TRUE);
	//		GetDlgItem(ID_BTN_StopGrab)->EnableWindow(FALSE);
	//		GetDlgItem(IDC_BTN_FeatureSet)->EnableWindow(TRUE);
	//		GetDlgItem(IDC_RADIO_Image)->EnableWindow(TRUE);
	//		GetDlgItem(IDC_RADIO_Range)->EnableWindow(TRUE);
	//		GetDlgItem(IDC_BUTTON_ChangeSavePath)->EnableWindow(TRUE);
	//	}

	//	LCC_C_ReleaseCamera(m_pCameraHandle[iCameraIndex]);
	//	LCC_C_CloseCamera(m_pCameraHandle[iCameraIndex]);

	//}

}

//@����˵���� ��֡�ɼ�
void CLusterCameraControlCDemoDlg::OnBnClickedBtnSnap()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	int iCameraIndex = 0;
	bBrushRectRanger3 = true;//ˢ��һ����ʾ����

	if (m_pCameraHandle[iCameraIndex] != INVALID_CAM_HANDLE)
	{
		/*��֡�ɼ�*/
		LE_STATUS eResult = LCC_C_SnapImage(m_pCameraHandle[iCameraIndex]);
		if (eResult == L_OK)
		{
			//AfxMessageBox("��֡�ɼ��ɹ�!");

			//����ʹ��
			GetDlgItem(ID_BTN_CamDetectNum)->EnableWindow(FALSE);
			GetDlgItem(ID_BTN_OpenCamera)->EnableWindow(FALSE);
			GetDlgItem(ID_BTN_CloseCamera)->EnableWindow(TRUE);
			GetDlgItem(ID_BTN_Snap)->EnableWindow(TRUE);
			GetDlgItem(ID_BTN_StartGrab)->EnableWindow(TRUE);
			GetDlgItem(ID_BTN_StopGrab)->EnableWindow(FALSE);
			GetDlgItem(IDC_BTN_FeatureSet)->EnableWindow(TRUE);
		}
		else
		{
			ShowErrorInfo(eResult, "��֡�ɼ�ʧ��");
		}

	}

}

//@����˵���� �ر����
void CLusterCameraControlCDemoDlg::OnBnClickedBtnClosecamera()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	int iCameraIndex = 0;
	LE_STATUS eResult;

	eResult = LCC_C_ReleaseCamera(m_pCameraHandle[iCameraIndex]);
	if (eResult == L_OK)
	{
		AfxMessageBox("�ͷ���Դ���0�ɹ�!");
		if (m_pCameraHandle[iCameraIndex] != INVALID_CAM_HANDLE)
		{
			eResult = LCC_C_CloseCamera(m_pCameraHandle[iCameraIndex]);
			if (eResult == L_OK)
			{
				AfxMessageBox("�ر�����ɹ�!");

				//����ʹ��
				GetDlgItem(ID_BTN_CamDetectNum)->EnableWindow(TRUE);
				GetDlgItem(ID_BTN_OpenCamera)->EnableWindow(FALSE);
				GetDlgItem(ID_BTN_CloseCamera)->EnableWindow(FALSE);
				GetDlgItem(ID_BTN_Snap)->EnableWindow(FALSE);
				GetDlgItem(ID_BTN_StartGrab)->EnableWindow(FALSE);
				GetDlgItem(ID_BTN_StopGrab)->EnableWindow(FALSE);
				GetDlgItem(IDC_BTN_FeatureSet)->EnableWindow(FALSE);
			}
			else
			{
				ShowErrorInfo(eResult, "�ر����ʧ��");
			}
		}
	}

	else
	{
		ShowErrorInfo(eResult, "�ͷ���Դʧ��");
	}

	//�ͷŽṹ��
	if (m_pstBufferInfo[iCameraIndex] != NULL)
	{
		delete m_pstBufferInfo[iCameraIndex];
		m_pstBufferInfo[iCameraIndex] = NULL;
	}


	//iCameraIndex = 1;

	//eResult = LCC_C_ReleaseCamera(m_pCameraHandle[iCameraIndex]);
	//if (eResult == L_OK)
	//{
	//	AfxMessageBox("�ͷ���Դ���0�ɹ�!");
	//	if (m_pCameraHandle[iCameraIndex] != INVALID_CAM_HANDLE)
	//	{
	//		eResult = LCC_C_CloseCamera(m_pCameraHandle[iCameraIndex]);
	//		if (eResult == L_OK)
	//		{
	//			AfxMessageBox("�ر�����ɹ�!");

	//			//����ʹ��
	//			GetDlgItem(ID_BTN_CamDetectNum)->EnableWindow(TRUE);
	//			GetDlgItem(ID_BTN_OpenCamera)->EnableWindow(FALSE);
	//			GetDlgItem(ID_BTN_CloseCamera)->EnableWindow(FALSE);
	//			GetDlgItem(ID_BTN_Snap)->EnableWindow(FALSE);
	//			GetDlgItem(ID_BTN_StartGrab)->EnableWindow(FALSE);
	//			GetDlgItem(ID_BTN_StopGrab)->EnableWindow(FALSE);
	//			GetDlgItem(IDC_BTN_FeatureSet)->EnableWindow(FALSE);
	//		}
	//		else
	//		{
	//			ShowErrorInfo(eResult, "�ر����ʧ��");
	//		}
	//	}
	//}

	//else
	//{
	//	ShowErrorInfo(eResult, "�ͷ���Դʧ��");
	//}

	////�ͷŽṹ��
	//if (m_pstBufferInfo[iCameraIndex] != NULL)
	//{
	//	delete m_pstBufferInfo[iCameraIndex];
	//	m_pstBufferInfo[iCameraIndex] = NULL;
	//}
}

//@����˵���� ��ʼ��ͼ
void CLusterCameraControlCDemoDlg::OnBnClickedBtnStartrecord()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_iRecord = 1;

	CString strPath;
	CString strName;
	GetDlgItem(IDC_EDIT_SavePath)->GetWindowText(strPath);
	if (!PathFileExists(strPath))
	{
		if (CreateDirectoryA(strPath, NULL))
		{
			::MessageBox(NULL, TEXT("�洢·��������,�Ѵ�����"), NULL, MB_OK);//����ԭ���������ý���
			//strPath.Replace(_T("\\"),_T("/"));
			m_path = strPath;
		}
		else
		{
			::MessageBox(NULL, TEXT("�洢·��������,����ʧ�ܣ����������ã�"), NULL, MB_OK);//����ԭ���������ý���
		}
		return;
	}
	else
	{
		m_path = strPath;
	}
	
	//����ʹ��
	GetDlgItem(ID_BTN_StartRecord)->EnableWindow(FALSE);
	GetDlgItem(ID_BTN_StopRecord)->EnableWindow(TRUE);
}

//@����˵���� ֹͣ��ͼ
void CLusterCameraControlCDemoDlg::OnBnClickedBtnStoprecord()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_iRecord = 2;

	//����ʹ��
	GetDlgItem(ID_BTN_StartRecord)->EnableWindow(TRUE);
	GetDlgItem(ID_BTN_StopRecord)->EnableWindow(FALSE);
}

//@����˵���� ��������
void CLusterCameraControlCDemoDlg::OnBnClickedBtnFeatureset()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	FeatureSet newdlg;
	newdlg.DoModal();
}


//@����˵���� �洢�ص�����
void StoreCallBack(int iCallBackType, void* pCameraHandle, void* pData, void* pUserContext)
{
	CLusterCameraControlCDemoDlg *pDlg = static_cast<CLusterCameraControlCDemoDlg*>(pUserContext);
	int iCameraIndex = INVALID_CAMERA_INDEX;
	for (int i = 0; i < pDlg->m_iCameraNum; i++)
	{
		if (pCameraHandle == pDlg->m_pCameraHandle[i])
		{
			iCameraIndex = i;
			break;
		}
	}
	if (iCameraIndex < 0 || iCameraIndex >= CAMERA_MAX_NUM)
	{
		return;
	}

	//TODO�����ݴ���ͼ��洢�Ȳ���
	if (iCameraIndex == 0)
	{
		LST_IMAGE_INFO *pstDisplayImageInfo = (LST_IMAGE_INFO*)pData;

		//bufferPath��ԭʼͼ��洢·����bufferPathCalibrate���궨��ͼ��洢·����
		//bufferPathRedify���޸ĺ�ͼ��洢·����bufferPathCalibrateAndRedify���궨���޸ĺ�ͼ��洢·��
		std::stringstream bufferPath, bufferPathCalibrate, bufferPathRedify, bufferPathCalibrateAndRedify;

		bufferPath << m_path << "\\" << m_fileName << "-" << iCameraIndex << "-" << pstDisplayImageInfo->iImageFrameIndex;

		bufferPathCalibrate << m_CalibratePath << "\\" << m_CalibrateName << "-" << iCameraIndex << "-" << pstDisplayImageInfo->iImageFrameIndex;

		bufferPathRedify << m_RedifyPath << "\\" << m_RedifyName << "-" << iCameraIndex << "-" << pstDisplayImageInfo->iImageFrameIndex;

		bufferPathCalibrateAndRedify << m_CalibrateAndRedifyPath << "\\" << m_CalibratePathAndRedifyName << "-" << iCameraIndex << "-" << pstDisplayImageInfo->iImageFrameIndex;
		
#if 0
		if (m_Image == true)
		{
			if (m_iRecord == 1)
			{
				const uint8_t*buff = (uint8_t*)pstDisplayImageInfo->pchBuffer;//ת��buffer��ʽ����BYTE��unit_8
				uint8_t* buffer8;
				size_t buffer8Size;

				buffer8Size = pstDisplayImageInfo->stImageProperty.iImageWidth*pstDisplayImageInfo->stImageProperty.iImageHeight;
				buffer8 = (uint8_t*)malloc(buffer8Size);
				memcpy(buffer8, buff, buffer8Size);

				pDlg->saveBuffer8(buffer8,
					pstDisplayImageInfo->stImageProperty.iImageWidth,
					pstDisplayImageInfo->stImageProperty.iImageHeight,
					mm_offsetX3D,
					mm_offsetY3D,
					bufferPath.str());

				free(buffer8);
			}
		}
		else if (m_Image == false)
		{
			if (m_iRecord == 1)//�����洢
			{
				const uint8_t*buff = (uint8_t*)pstDisplayImageInfo->pchBuffer;//ת��buffer��ʽ����BYTE��unit_8

				//��ȡ��ǰ��ͼģʽ��true��Range+rReflectance��false��Range
				LST_BOOLEAN_INFO stBooleanInfo;
				LE_STATUS eResult = LCC_C_GetFeatureBoolean(pDlg->m_pCameraHandle[iCameraIndex], LFNC_SICK_REFLECTANCE_ENABLE, &stBooleanInfo);
				if (eResult != L_OK)
				{
					AfxMessageBox("��ȡ��REFLECTANCE_ENABLE��ֵ����");
				}

				//CalibrationError errorlast;
				if (stBooleanInfo.bValue == true)//��ǰ��ͼģʽ��Range+rReflectance
				{
					uint8_t* buffer16;
					size_t buffer16Size;
					uint8_t* buffer8;
					size_t buffer8Size;

					//buffer16:Rangeͼ��buffer8���Ҷ�ͼ
					buffer16Size = pstDisplayImageInfo->stImageProperty.iImageWidth*pstDisplayImageInfo->stImageProperty.iImageHeight * 2;
					buffer16 = (uint8_t*)malloc(buffer16Size);
					buffer8Size = pstDisplayImageInfo->stImageProperty.iImageWidth*pstDisplayImageInfo->stImageProperty.iImageHeight;
					buffer8 = (uint8_t*)malloc(buffer8Size);

					memcpy(buffer16, buff, buffer16Size);
					memcpy(buffer8, buff + buffer16Size, buffer8Size);

					//��ͼ������
					pDlg->saveBufferRangeAndReflectance(buffer16,
						buffer8,
						pstDisplayImageInfo->stImageProperty.iImageWidth,
						pstDisplayImageInfo->stImageProperty.iImageHeight,
						mm_height3D,
						mm_offsetX3D,
						mm_offsetY3D,
						bufferPath.str());

					//�궨���޸ı궨ͼ�񣬱궨���޸�ͼ��������Ҫ�궨���ɲ����ô˽ӿڣ�
					/*errorlast = ApplyCalibration(bufferPath.str().c_str(), "..\\XmlFile\\Calibration.xml", bufferPathCalibrate.str().c_str());
					errorlast = RectifyCalibratedBuffer(bufferPathCalibrate.str().c_str(), bufferPathRedify.str().c_str(), 1536, icon::RectificationFilter::TOP_MOST, false);
					errorlast = CalibrateAndRectifyBuffer(bufferPath.str().c_str(), "..\\XmlFile\\Calibration.xml", bufferPathCalibrateAndRedify.str().c_str(), 1536, icon::RectificationFilter::TOP_MOST, false);*/
					std::string calibFile = "cfg.xml";
					const int64_t rectificationWidth = 2560;
					int bufferWidth = pstDisplayImageInfo->stImageProperty.iImageWidth;
					int bufferHeight = pstDisplayImageInfo->stImageProperty.iImageHeight;
					//Initialize the calibration filter. Best to only do once as it is time-consuming. (Need to make a new filter if the buffer changes size).
					//calibrationAdapter::CalibrationFilter* calFilter = new calibrationAdapter::CalibrationFilter(calibFile, bufferHeight, bufferWidth, rectificationWidth /*Rectification Width*/, aoiHeight, aoiOffsetX, aoiOffsetY, true /*include reflectance*/);
					calibrationAdapter::CalibrationFilter* calFilter =
						new calibrationAdapter::CalibrationFilter(calibFile, bufferHeight,
						bufferWidth, rectificationWidth /*Rectification Width*/,
						bufferHeight, 0, 0, true /*include reflectance*/);
					//Create output variables
					float *rangeRect = new float[rectificationWidth*bufferHeight];
					float *reflectanceRect = new float[rectificationWidth*bufferHeight];

					calibrationAdapter::WorldRangeTraits wTraits{};
					//Perform calibration and rectification 
					//calFilter->apply(buffer16, part1Info.mPartDataPointer/* nullptr*/, rangeRect, reflectanceRect, wTraits, bufferPath.str());
					calFilter->apply(buffer16, NULL, rangeRect, reflectanceRect, wTraits, bufferPathCalibrate.str());

					//You are responsible for the memory management of these variables
					delete rangeRect;
					delete reflectanceRect;//----------�޸ģ���reflectanceʱ��������δ��룩
					delete calFilter;


					free(buffer16);
					free(buffer8);
				}

				else if (stBooleanInfo.bValue == false)//��ǰ��ͼģʽ��Range
				{
					uint8_t* buffer16;//Rangeͼ
					size_t buffer16Size;

					buffer16Size = pstDisplayImageInfo->stImageProperty.iImageWidth*pstDisplayImageInfo->stImageProperty.iImageHeight * 2;
					buffer16 = (uint8_t*)malloc(buffer16Size);

					memcpy(buffer16, buff, buffer16Size);

					//��ͼ������
					pDlg->saveBuffer16(buffer16,
						pstDisplayImageInfo->stImageProperty.iImageWidth,
						pstDisplayImageInfo->stImageProperty.iImageHeight,
						mm_height3D,
						mm_offsetX3D,
						mm_offsetY3D,
						bufferPath.str());

					//�궨���޸ı궨ͼ�񣬱궨���޸�ͼ��������Ҫ�궨���ɲ����ô˽ӿڣ�
					/*errorlast = ApplyCalibration(bufferPath.str().c_str(), "..\\XmlFile\\Calibration.xml", bufferPathCalibrate.str().c_str());
					errorlast = RectifyCalibratedBuffer(bufferPathCalibrate.str().c_str(), bufferPathRedify.str().c_str(), 1536, icon::RectificationFilter::TOP_MOST, false);
					errorlast = CalibrateAndRectifyBuffer(bufferPath.str().c_str(), "..\\XmlFile\\Calibration.xml", bufferPathCalibrateAndRedify.str().c_str(), 1536, icon::RectificationFilter::TOP_MOST, false);*/
					free(buffer16);
				}
			}
		}
#endif // 0


	}
}

//@����˵���� ��ʾ�ص�����
void DisplayCallBack(int iCallBackType, void* pCameraHandle, void* pData, void* pUserContext)
{
	//CLusterCameraControlCDemoDlg *pDlg = (CLusterCameraControlCDemoDlg*)pUserContext;
	CLusterCameraControlCDemoDlg *pDlg = static_cast<CLusterCameraControlCDemoDlg*>(pUserContext);
	int iCameraIndex = INVALID_CAMERA_INDEX;
	for (int i = 0; i < pDlg->m_iCameraNum; i++)
	{
		if (pCameraHandle == pDlg->m_pCameraHandle[i])
		{
			iCameraIndex = i;
			break;
		}
	}
	if (iCameraIndex < 0 || iCameraIndex >= CAMERA_MAX_NUM)
	{
		return;
	}


	if (iCameraIndex == 0)
	{
		if (m_Image == true)
		{
			LST_IMAGE_INFO *pstDisplayImageInfo = (LST_IMAGE_INFO*)pData;
			const uint8_t*buff = (uint8_t*)pstDisplayImageInfo->pchBuffer;
			uint8_t* buffer8;
			size_t buffer8Size;
			CImage image;
			buffer8Size = pstDisplayImageInfo->stImageProperty.iImageWidth*pstDisplayImageInfo->stImageProperty.iImageHeight;
			buffer8 = (uint8_t*)malloc(buffer8Size);

			memcpy(buffer8, buff, buffer8Size);
			pDlg->CreateImage(false, 2560, 832, buffer8, image);

			pDlg->DrawPicture(IDC_PICTURE_BOX, image);
			image.Detach();
			image.Destroy();

			free(buffer8);
		}

		else if (m_Image == false)
		{
			LST_BOOLEAN_INFO stBooleanInfo;
			LE_STATUS eResult = LCC_C_GetFeatureBoolean(pDlg->m_pCameraHandle[iCameraIndex], LFNC_SICK_REFLECTANCE_ENABLE, &stBooleanInfo);
			if (eResult != L_OK)
			{
				AfxMessageBox("��ȡ��REFLECTANCE_ENABLE��ֵ����");
			}
			LST_IMAGE_INFO *pstDisplayImageInfo = (LST_IMAGE_INFO*)pData;
			const uint8_t*buff = (uint8_t*)pstDisplayImageInfo->pchBuffer;

			if (stBooleanInfo.bValue == true)
			{
				uint8_t* buffer16;
				size_t buffer16Size;
				uint8_t* buffer8;
				size_t buffer8Size;

				buffer16Size = pstDisplayImageInfo->stImageProperty.iImageWidth*pstDisplayImageInfo->stImageProperty.iImageHeight * 2;
				buffer16 = (uint8_t*)malloc(buffer16Size);
				buffer8Size = pstDisplayImageInfo->stImageProperty.iImageWidth*pstDisplayImageInfo->stImageProperty.iImageHeight;
				buffer8 = (uint8_t*)malloc(buffer8Size);

				memcpy(buffer16, buff, buffer16Size);
				memcpy(buffer8, buff + buffer16Size, buffer8Size);

				CImage image;

				pDlg->CreateImage(true, pstDisplayImageInfo->stImageProperty.iImageWidth,
					pstDisplayImageInfo->stImageProperty.iImageHeight, buffer16, image);

				pDlg->DrawPicture(IDC_PICTURE_BOX, image);
				image.Detach();
				image.Destroy();

				free(buffer16);
				free(buffer8);
			}
			else if (stBooleanInfo.bValue == false)
			{
				uint8_t* buffer16;
				size_t buffer16Size;

				buffer16Size = pstDisplayImageInfo->stImageProperty.iImageWidth*pstDisplayImageInfo->stImageProperty.iImageHeight * 2;
				buffer16 = (uint8_t*)malloc(buffer16Size);

				memcpy(buffer16, buff, buffer16Size);

				CImage image;

				pDlg->CreateImage(true, pstDisplayImageInfo->stImageProperty.iImageWidth,
					pstDisplayImageInfo->stImageProperty.iImageHeight, buffer16, image);

				pDlg->DrawPicture(IDC_PICTURE_BOX, image);
				image.Detach();
				image.Destroy();

				free(buffer16);

			}


		}


	}

}

//@����˵���� ��Ϣ�ص�����
void MessageCallBack(int iCallBackType, void* pCameraHandle, void* pData, void* pUserContext)
{

}

/***********************************************************************
*
*				       ȫ�ֺ���ʵ��
*
***********************************************************************/

/******************************************************
@����˵���� ��÷���״̬����ϸ������Ϣ��
@�������룺 eResult -- ��������ֵ״̬��
@��������� ��������ֵ״̬��Ӧ��������Ϣ
@����ֵ  �� ��
******************************************************/
void CLusterCameraControlCDemoDlg::GetStatusInfo(IN LE_STATUS eStatus, OUT char* pchInfo)
{
	switch (eStatus)
	{
	case L_ERR_CCNOED:
		strcpy(pchInfo, "L_ERR_CCNFS, û������ͼ��ɼ��豸");
		break;
	case L_ERR_CCWC:
		strcpy(pchInfo, "L_ERR_CCWC, ����Ĳ�ɫλ��");
		break;
	case L_ERR_CCWD:
		strcpy(pchInfo, "L_ERR_CCWD, ����Ĳɼ��豸");
		break;
	case L_ERR_CCWPF:
		strcpy(pchInfo, "L_ERR_CCWPF, �����֧�ֵ����ظ�ʽ");
		break;
	case L_ERR_CCNV:
		strcpy(pchInfo, "L_ERR_CCNV, û����Ƶ�ź�");
		break;
	case L_ERR_CCU:
		strcpy(pchInfo, "L_ERR_CCU, δ֪��ͼ��ɼ��豸");
		break;
	case L_ERR_CCF:
		strcpy(pchInfo, "L_ERR_CCF, ͼ��ץȡʧ��");
		break;
	case L_ERR_CCWR:
		strcpy(pchInfo, "L_ERR_CCWR, ѡ������ͼ��ֱ���");
		break;
	case L_ERR_CCWP:
		strcpy(pchInfo, "L_ERR_CCWP, ѡ������ͼ�񲿷�");
		break;
	case L_ERR_CCWPR:
		strcpy(pchInfo, "L_ERR_CCWPR, ѡ���������ر���");
		break;
	case L_ERR_CCWH:
		strcpy(pchInfo, "L_ERR_CCWH, �����Ч");
		break;
	case L_ERR_CCCL:
		strcpy(pchInfo, "L_ERR_CCCL, ʵ����Ч�������Ѿ�ע����");
		break;
	case L_ERR_CCNIB:
		strcpy(pchInfo, "L_ERR_CCNIB, �޷���ʼ������");
		break;
	case L_ERR_CCET:
		strcpy(pchInfo, "L_ERR_CCET, ��֧���ⴥ��");
		break;
	case L_ERR_CCLI:
		strcpy(pchInfo, "L_ERR_CCLI, �������������У����أ�");
		break;
	case L_ERR_CCCS:
		strcpy(pchInfo, "L_ERR_CCCS, �������ɫ�ռ�");
		break;
	case L_ERR_CCPT:
		strcpy(pchInfo, "L_ERR_CCPT, ����Ķ˿�");
		break;
	case L_ERR_CCCT:
		strcpy(pchInfo, "L_ERR_CCCT, ������������");
		break;
	case L_ERR_CCTM:
		strcpy(pchInfo, "L_ERR_CCTM, �����˲ɼ��豸��������Ŀ");
		break;
	case L_ERR_CCDV:
		strcpy(pchInfo, "L_ERR_CCDV, �豸æµ");
		break;
	case L_ERR_CCASYNC:
		strcpy(pchInfo, "L_ERR_CCASYNC, ��֧���첽ץȡ");
		break;
	case L_ERR_CCPARAM:
		strcpy(pchInfo, "L_ERR_CCPARAM, �ò����ݲ�֧��");
		break;
	case L_ERR_CCTIMEOUT:
		strcpy(pchInfo, "L_ERR_CCTIMEOUT, ��ʱ");
		break;
	case L_ERR_CCGAIN:
		strcpy(pchInfo, "L_ERR_CCGAIN, ��Ч������");
		break;
	case L_ERR_CCFIELD:
		strcpy(pchInfo, "L_ERR_CCFIELD, ��Ч���ֶ�");
		break;
	case L_ERR_CCPART:
		strcpy(pchInfo, "L_ERR_CCPART, ��Ч�Ĳ�������");
		break;
	case L_ERR_CCPARV:
		strcpy(pchInfo, "L_ERR_CCPARV, ��Ч�Ĳ���ֵ");
		break;
	case L_ERR_CCFNS:
		strcpy(pchInfo, "L_ERR_CCFNS, ��֧�ֻ��ѱ������Ĺ���");
		break;
	case L_ERR_CCIVERS:
		strcpy(pchInfo, "L_ERR_CCIVERS, �汾�����ݵĽӿ�");
		break;
	case L_ERR_CCSETPAR:
		strcpy(pchInfo, "L_ERR_CCSETPAR, ���ò���ֵʧ��");
		break;
	case L_ERR_CCGETPAR:
		strcpy(pchInfo, "L_ERR_CCGETPAR, ��ѯ����ֵʧ��");
		break;
	case L_ERR_CCPARNA:
		strcpy(pchInfo, "L_ERR_CCPARNA, ��ǰ�����£��ò���������");
		break;
	case L_ERR_CCCLOSE:
		strcpy(pchInfo, "L_ERR_CCCLOSE, �豸���������ر�");
		break;
	case L_ERR_CCCAMFILE:
		strcpy(pchInfo, "L_ERR_CCCAMFILE, ��������ļ����ܴ�");
		break;
	case L_ERR_CCCALLBACK:
		strcpy(pchInfo, "L_ERR_CCCALLBACK, ��֧�ֵĻص�����");
		break;
	case L_ERR_CCDEVLOST:
		strcpy(pchInfo, "L_ERR_CCDEVLOST, �豸��ʧ");
		break;
	case L_ERR_DNA:
		strcpy(pchInfo, "L_ERR_DNA, �豸�����������");
		break;
	case L_ERR_MEM:
		strcpy(pchInfo, "L_ERR_MEM, �ڴ治��");
		break;
	case L_ERR_CCFWIDX:
		strcpy(pchInfo, "L_ERR_CCFWIDX, ��Ч���������");
		break;
	case L_ERR_CCFWVI:
		strcpy(pchInfo, "L_ERR_CCFWVI, ���ҽӿڴ���");
		break;
	case L_ERR_CCXML:
		strcpy(pchInfo, "L_ERR_CCXML, XML�����ļ����ܴ�");
		break;
	case L_ERR_CCFIDFV:
		strcpy(pchInfo, "L_ERR_CCFIDFV, Ĭ�ϲ�����ʼ��ʧ��");
		break;
	case L_ERR_CCOPEN:
		strcpy(pchInfo, "L_ERR_CCOPEN, �����ʧ��");
		break;
	case L_ERR_CCRXFVF:
		strcpy(pchInfo, "L_ERR_CCRXFVF, ��ȡXML����ֵʧ��");
		break;
	case L_ERR_CCNRB:
		strcpy(pchInfo, "L_ERR_CCNRB, �޷��ͷŻ���");
		break;
	case L_ERR_CCWLPI:
		strcpy(pchInfo, "L_ERR_CCWLPI, �޷����ز���������������󣬻�ȱʧ�����ļ�");
		break;
	case L_ERR_CCWFPI:
		strcpy(pchInfo, "L_ERR_CCWFPI, �޷��ͷŲ��");
		break;
	case L_ERR_UNKNOWN:
		strcpy(pchInfo, "L_ERR_UNKNOWN, δ֪����");
		break;
	case L_OK:
		strcpy(pchInfo, "L_OK, ����");
		break;
	default:
		strcpy(pchInfo, "�°汾�������");
		break;
	}
}

/******************************************************
@����˵���� ��ʾ������Ϣ
@�������룺 eResult -- ����������ƽӿڣ����ص�״̬
pchErrorInfo -- ������Ϣ
@��������� ��
@����ֵ  �� ��
******************************************************/
void CLusterCameraControlCDemoDlg::ShowErrorInfo(LE_STATUS eResult, char *pchErrorInfo)
{
	const int INFO_LEN = 256;
	char achInfo[INFO_LEN];
	memset(achInfo, 0, INFO_LEN);
	CLusterCameraControlCDemoDlg::GetStatusInfo(eResult, achInfo);
	CString strMsg;
	strMsg.Format(_T("%s��������룺0x%x��������Ϣ��%s"), pchErrorInfo, eResult, achInfo);
	AfxMessageBox(strMsg);
}

//����ͼƬ����
void CLusterCameraControlCDemoDlg::CreateImage(bool isRange, int imgWidth, int imgHeight, void *pImageBuf, CImage& pImage)
{
	if (pImageBuf != nullptr)
	{
		if (!pImage.IsNull())
			pImage.Destroy();

		int channels = 1;
		pImage.Create(imgWidth, -imgHeight, 8);
		RGBQUAD* ColorTable;
		int MaxColors = 256;
		ColorTable = new RGBQUAD[MaxColors];
		pImage.GetColorTable(0, MaxColors, ColorTable);
		//#pragma omp parallel for
		for (int i = 0; i<MaxColors; i++)
		{
			ColorTable[i].rgbBlue = (BYTE)i;
			ColorTable[i].rgbGreen = (BYTE)i;
			ColorTable[i].rgbRed = (BYTE)i;
		}

		pImage.SetColorTable(0, MaxColors, ColorTable);
		delete[]ColorTable;

		if (isRange)
		{
			//When using Range after 12 bit to 16 bit conversion, then pImageBuf is a 16 bit pointer

			const uint16_t * pImageBuf16 = static_cast<uint16_t*>(pImageBuf);
			float maxZ = *std::max_element(pImageBuf16, pImageBuf16 + imgWidth*imgHeight);
			float minZ = *std::min_element(pImageBuf16, pImageBuf16 + imgWidth*imgHeight);
			float gSize = maxZ - minZ;
			UCHAR* OriData = (UCHAR*)pImage.GetBits();
			for (int i = 0; i<imgHeight; i++)
			{
				for (int j = 0; j<imgWidth; j++)
				{
					UCHAR a = static_cast<uint8_t>(((*(pImageBuf16 + i*imgWidth + j) - minZ) / gSize) * 255);
					*(OriData + i*imgWidth + j) = a;

				}
			}
		}
		else
		{
			//When using Image the pImageBuf is a 8 bit pointer
			UCHAR* OriData = (UCHAR*)pImage.GetBits();
			for (int i = 0; i<imgHeight; i++)
			{
				for (int j = 0; j<imgWidth; j++)
				{
					//*(OriData + i* (-2560)+ j)=*(reinterpret_cast<uint8_t*>(pImageBuf)+i*imgWidth+j);
					*(OriData + i * 2560 + j) = *(static_cast<uint8_t*>(pImageBuf)+i*imgWidth + j);
				}
			}
		}
	}

}

//����ͼƬ;
bool CLusterCameraControlCDemoDlg::ImageCopy(const CImage &srcImage, CImage &destImage)
{
	int i;//ѭ������  
	if (srcImage.IsNull())
		return false;
	//Դͼ�����  
	BYTE* srcPtr = (BYTE*)srcImage.GetBits();
	int srcBitsCount = srcImage.GetBPP();
	int srcWidth = srcImage.GetWidth();
	int srcHeight = srcImage.GetHeight();
	int srcPitch = srcImage.GetPitch();
	//����ԭ��ͼ��  
	if (!destImage.IsNull())
	{
		destImage.Destroy();
	}
	//������ͼ��  
	if (srcBitsCount == 32)   //֧��alphaͨ��  
	{
		destImage.Create(srcWidth, srcHeight, srcBitsCount, 1);
	}
	else
	{
		destImage.Create(srcWidth, srcHeight, srcBitsCount, 0);
	}
	//���ص�ɫ��  
	if (srcBitsCount <= 8 && srcImage.IsIndexed())//����Ƿ���Ҫ��ɫ��  ;
	{
		RGBQUAD pal[256];
		int nColors = srcImage.GetMaxColorTableEntries();
		if (nColors>0)
		{
			srcImage.GetColorTable(0, nColors, pal);
			destImage.SetColorTable(0, nColors, pal);//���Ƶ�ɫ�����  
		}
	}
	//Ŀ��ͼ�����  
	BYTE *destPtr = (BYTE*)destImage.GetBits();
	int destPitch = destImage.GetPitch();
	//����ͼ������  
	for (i = 0; i<srcHeight; i++)
	{
		memcpy(destPtr + i*destPitch, srcPtr + i*srcPitch, abs(srcPitch));
	}

	return TRUE;
}

//��2Dͼ
void CLusterCameraControlCDemoDlg::DrawPicture(int nID, CImage pImage)
{
	//��ͼƬ��ʾ��������
	//std::_Max_element()
	if (!pImage.IsNull())
	{

		int imgWidth = pImage.GetWidth();//2560
		int imgHeight = pImage.GetHeight();//200
		CWnd *m_Wnd2D = GetDlgItem(nID);
		m_Wnd2D->UpdateWindow();
		CStatic *m_PicCtl = (CStatic*)GetDlgItem(nID);
		CRect rect2D;
		m_Wnd2D->GetClientRect(&rect2D);//��Ϊ����ͼ���С�ľ��ο�
		CDC *pDc = m_Wnd2D->GetDC();

		//ˢ��һ����ʾ����
		if (bBrushRectRanger3 == true)
		{
			CBrush br(0x888888);
			pDc->FillRect(rect2D, &br);
			bBrushRectRanger3 = false;
		}

		int rheight;
		int rwidth;
		rheight = rect2D.bottom - rect2D.top;//776=776-0
		rwidth = rect2D.right - rect2D.left;//798=798-0
		double min_k;
		if ((double(rheight) / double(imgHeight))<(double(rwidth) / double(imgWidth)))
			min_k = double(rheight) / double(imgHeight);
		else
			min_k = double(rwidth) / double(imgWidth);//0.3117
		int img_width = min_k*imgWidth;//798
		int img_height = min_k*imgHeight;//62
		int x0, y0, x1, y1;
		if (abs(img_width - rwidth)<abs(img_height - rheight)) {
			x0 = 0;
			y0 = floor(double((rheight - img_height) / 2));//357
			// y0=0;
			x1 = img_width;//798
			y1 = img_height;//62
		}
		else
		{
			x0 = floor(double((rwidth - img_width) / 2));
			y0 = 0;
			x1 = img_width;
			y1 = img_height;
		}
		SetStretchBltMode(pDc->m_hDC, STRETCH_HALFTONE);			//ͼ���С��Ӧ2D���ڴ�С
		//pImage.StretchBlt(pDc->m_hDC,rect2D,SRCCOPY);
		pImage.StretchBlt(pDc->m_hDC, x0, y0, x1, y1, SRCCOPY);
		m_Wnd2D->ReleaseDC(pDc);
	}

	//delete[] OriData;
}

//����ͼƬ
void CLusterCameraControlCDemoDlg::DrawImage()
{

	if (!mImage.IsNull())
	{
		CImage image;
		ImageCopy(mImage, image);//dstimage:image;
		DrawPicture(IDC_PICTURE_BOX, image);
	}

}

//�洢16+8λͼ
void CLusterCameraControlCDemoDlg::saveBufferRangeAndReflectance(
	const uint8_t* buffer16,
	const uint8_t* buffer8,
	const int64_t bufferWidth,
	const int64_t bufferHeight,
	const int64_t aoiHeight,
	const int64_t aoiOffsetX,
	const int64_t aoiOffsetY,
	const std::string& filePath)
{
	std::string datFilename = filePath + ".dat";
	std::string xmlFilename = filePath + ".xml";
	std::ofstream dataStream(
		datFilename.c_str(), std::ios::binary | std::ios::trunc | std::ios::out);
	dataStream.exceptions(std::ios::failbit | std::ios::badbit);
	dataStream.write(reinterpret_cast<const char*>(buffer16), bufferWidth * bufferHeight * 2);
	dataStream.write(reinterpret_cast<const char*>(buffer8), bufferWidth * bufferHeight);
	dataStream.close();

	DatXmlWriter xml;
	// XML file write use { } brackets to indicate element structure
	xml.open();
	{
		xml.addParameter("size", std::to_string(bufferWidth * 3));
		xml.addParameter("version", "1");
		xml.addParameter("layout", "SUBCOMPONENT");
		xml.openComponent("Hi3D", "Ranger3Range");
		{
			xml.addParameter("size", std::to_string(bufferWidth * 3));
			xml.addParameter("height", "1");
			writeSensorRangeTraits(xml, bufferWidth, aoiHeight, aoiOffsetX,
				aoiOffsetY);
			writeSubComponentRange16(xml, bufferWidth);
			writeSubComponentReflectance(xml, bufferWidth);
		}
		xml.closeComponent();
	}
	xml.close();

	std::ofstream xmlStream(
		xmlFilename.c_str(), std::ios::binary | std::ios::trunc | std::ios::out);
	xmlStream.exceptions(std::ios::failbit | std::ios::badbit);
	xmlStream << xml.toString();
	xmlStream.close();
}

//�洢16λͼ
void CLusterCameraControlCDemoDlg::saveBuffer16(
	const uint8_t* buffer,
	const int64_t bufferWidth,
	const int64_t bufferHeight,
	const int64_t aoiHeight,
	const int64_t aoiOffsetX,
	const int64_t aoiOffsetY,
	const std::string& filePath)
{
	std::string datFilename = filePath + ".dat";
	std::string xmlFilename = filePath + ".xml";
	std::ofstream dataStream(datFilename.c_str(), std::ios::binary | std::ios::trunc | std::ios::out);
	dataStream.exceptions(std::ios::failbit | std::ios::badbit);
	//dataStream.exceptions(std::ofstream::failbit | std::ofstream::badbit);
	dataStream.write((const char*)buffer, bufferWidth * bufferHeight * 2);
	dataStream.close();

	DatXmlWriter xml;
	xml.open();
	{
		xml.addParameter("size", std::to_string(bufferWidth * 2));
		xml.addParameter("version", "1");
		xml.addParameter("layout", "SUBCOMPONENT");
		xml.openComponent("HorThr", "Ranger3Range");
		{
			xml.addParameter("size", std::to_string(bufferWidth * 2));
			xml.addParameter("height", "1");
			xml.openSensorRangeTraits();
			{
				// From where on the sensor the data comes from can be calculated by
				// using the origin and scale factors below

				// Value range of data in X-direction, 0-indexed
				xml.addParameter("fov x0", "0");
				xml.addParameter("fov x1", std::to_string(bufferWidth - 1));
				xml.addParameter("fov x2", "0");
				xml.addParameter("fov x3", std::to_string(bufferWidth - 1));
				// Value range of data in Z-direction (height), 0-indexed. Starts at 1
				// because 0 indicates missing data
				xml.addParameter("fov z0", "1");
				xml.addParameter("fov z1", "1");
				// 16 bits data with sub-pixel accuracy
				xml.addParameter("fov z2", std::to_string(aoiHeight * 16 - 1));
				xml.addParameter("fov z3", std::to_string(aoiHeight * 16 - 1));
				// Where on the sensor is the data extracted from
				xml.addParameter("origin x", std::to_string(aoiOffsetX));
				xml.addParameter("scale x", "1");
				xml.addParameter("origin z", std::to_string(aoiOffsetY));
				// Sub-pixel accuracy
				xml.addParameter("scale z", "0.0625");
			}
			xml.closeSensorRangeTraits();
			xml.openSubComponent("WORD", "Range");
			{
				xml.addParameter("size", std::to_string(bufferWidth * 2));
				xml.addParameter("width", std::to_string(bufferWidth));
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

//�洢8λͼ
void CLusterCameraControlCDemoDlg::saveBuffer8(
	const uint8_t* buffer,
	const int64_t bufferWidth,
	const int64_t bufferHeight,
	const int64_t offsetX,
	const int64_t offsetY,
	const std::string& filePath)
{
	std::string datFilename = filePath + ".dat";
	std::string xmlFilename = filePath + ".xml";
	std::ofstream dataStream(
		datFilename.c_str(), std::ios::binary | std::ios::trunc | std::ios::out);
	dataStream.exceptions(std::ios::failbit | std::ios::badbit);
	dataStream.write(reinterpret_cast<const char*>(buffer), bufferWidth * bufferHeight);
	dataStream.close();

	DatXmlWriter xml;
	// XML file write use { } brackets to indicate element structure
	xml.open("IMAGE", "IMAGE");
	{
		xml.addParameter("size", std::to_string(bufferWidth * bufferHeight));
		xml.addParameter("version", "1");
		xml.addParameter("layout", "SCAN");
		xml.openComponent("image", "Ranger3Image");
		{
			xml.addParameter("size", std::to_string(bufferWidth));
			xml.addParameter("height", std::to_string(bufferHeight));
			xml.openImageTraits();
			{
				xml.addParameter("size x", std::to_string(bufferWidth));
				xml.addParameter("size y", std::to_string(bufferHeight));
				xml.addParameter("offset x", std::to_string(offsetX));
				xml.addParameter("offset y", std::to_string(offsetY));
			}
			xml.closeImageTraits();
			xml.openSubComponent("BYTE", "Image");
			{
				xml.addParameter("size", std::to_string(bufferWidth));
				xml.addParameter("width", std::to_string(bufferWidth));
			}
			xml.closeSubComponent();
		}
		xml.closeComponent();
	}
	xml.close();

	std::ofstream xmlStream(
		xmlFilename.c_str(), std::ios::binary | std::ios::trunc | std::ios::out);
	xmlStream.exceptions(std::ios::failbit | std::ios::badbit);
	xmlStream << xml.toString();
	xmlStream.close();
}

//д��ͼxml�ļ�
void writeSensorRangeTraits(DatXmlWriter& xml, const int64_t bufferWidth,
	const int64_t aoiHeight, const int64_t aoiOffsetX,
	const int64_t aoiOffsetY)
{
	// XML file write use { } brackets to indicate element structure
	xml.openSensorRangeTraits();
	{
		// From where on the sensor the data comes from can be calculated by
		// using the origin and scale factors below

		// Value range of data in X-direction, 0-indexed
		xml.addParameter("fov x0", "0");
		xml.addParameter("fov x1", std::to_string(bufferWidth - 1));
		xml.addParameter("fov x2", "0");
		xml.addParameter("fov x3", std::to_string(bufferWidth - 1));
		// Value range of data in Z-direction (height), 0-indexed. Starts at 1
		// because 0 indicates missing data
		xml.addParameter("fov z0", "1");
		xml.addParameter("fov z1", "1");
		// 16 bits data with sub-pixel accuracy
		xml.addParameter("fov z2", std::to_string(aoiHeight * 16 - 1));
		xml.addParameter("fov z3", std::to_string(aoiHeight * 16 - 1));
		// Where on the sensor is the data extracted from
		xml.addParameter("origin x", std::to_string(aoiOffsetX));
		xml.addParameter("scale x", "1");
		xml.addParameter("origin z", std::to_string(aoiOffsetY));
		// Sub-pixel accuracy
		xml.addParameter("scale z", "0.0625");
	}
	xml.closeSensorRangeTraits();
}

//дRange buffer16��xml�ļ�
void writeSubComponentRange16(DatXmlWriter& xml, const int64_t bufferWidth)
{
	// XML file write use { } brackets to indicate element structure
	xml.openSubComponent("WORD", "Range");
	{
		xml.addParameter("size", std::to_string(bufferWidth * 2));
		xml.addParameter("width", std::to_string(bufferWidth));
	}
	xml.closeSubComponent();
}

//дReflectance buffer��xml�ļ�
void writeSubComponentReflectance(DatXmlWriter& xml, const int64_t bufferWidth)
{
	// XML file write use { } brackets to indicate element structure
	xml.openSubComponent("BYTE", "Reflectance");
	{
		xml.addParameter("size", std::to_string(bufferWidth));
		xml.addParameter("width", std::to_string(bufferWidth));
	}
	xml.closeSubComponent();
}

void CLusterCameraControlCDemoDlg::OnBnClickedRadioImage()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_Image = true;
	int iCameraIndex = 0;
	LE_STATUS eResult = LCC_C_SetFeatureBoolean(m_pCameraHandle[iCameraIndex], LFNC_SICK_IMAGE2D, true);
	f_Image.SetCheck(1);
	f_Range.SetCheck(0);
}


void CLusterCameraControlCDemoDlg::OnBnClickedRadioRange()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_Image = false;
	int iCameraIndex = 0;
	LE_STATUS eResult = LCC_C_SetFeatureBoolean(m_pCameraHandle[iCameraIndex], LFNC_SICK_IMAGE2D, false);
	f_Image.SetCheck(0);
	f_Range.SetCheck(1);
}


void CLusterCameraControlCDemoDlg::OnBnClickedButtonChangesavepath()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	SavePath newdlg;
	newdlg.DoModal();
	f_SavePath.SetWindowText(m_path.c_str());
}
