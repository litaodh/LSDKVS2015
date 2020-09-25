//// LusterCameraControlCDemoDlg.cpp : 实现文件
//
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
* DATE 		    :  2017/12/06
* AUTHOR 		:  雷丹
* NOTE 		    :  驱动版本：Ranger3-1.1.3.1396
*				   相机厂商：Sick
*				   相机型号：Ranger3
*				   对应接口文档《组件SDK相机控制统一接口详细设计说明(标准C语言)_v0.1.2.0》。
*
*
* VERSION       :  1.0.0
* DATE 		    :  2018/03/15
* AUTHOR 		:  雷丹
* NOTE 		    :  驱动版本：Ranger3-1.1.3.1497
*				   相机厂商：Sick
*				   相机型号：Ranger3
*                  对应接口文档《LSDK相机控制统一接口说明(标准C语言)_v1.0.0》。
*                  1.增加状态判断m_bDetected、m_bOpened、m_bInited、m_bGrabbed
*                  2.PopImageFromBuffer函数增加指针为空判断
*                  3.头文件增加错误类型L_ERR_CCOED、L_ERR_CCIED、L_ERR_CCNIED、L_ERR_CCGED、L_ERR_CCNGED
*                  4.添加导入配置文件接口：LCC_C_SetFeatureString
*                  5.添加实时标定，输出标定后的图像
*                  6.添加参数设置功能：选择存图为Range+Reflectance或Range模式

VERSION       :  1.0.0
* DATE 		    :  2018/05/29
* AUTHOR 		:  雷丹
* NOTE 		    :  驱动版本：Ranger3-1.1.3.1497
*				   相机厂商：Sick
*				   相机型号：Ranger3
*                  对应接口文档《LSDK相机控制统一接口说明(标准C语言)_v1.0.0》。
*                  1.解决内存泄露问题（其余当前demo显示的内存泄露为MFC自带，已用控制台程序测试过）
*                  2.修改设置路径位置
*                  3.解决2D图像连续采集闪烁的问题
*                  4.添加“ShowErrorInfo”
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

CImage mImage;//显示图像
bool bBrushRectRanger3 = true;//是否刷新显示界面-Ranger3

//Region1模式下的图像属性值
int mm_height3D;
int mm_offsetX3D;
int mm_offsetY3D;

int m_iRecord = 0;//是否存图到本地盘，初始化为0，1：存盘到本地；2：不存图
bool m_Image = false;//图像模式，true->imgae,false->range

//存图路径及图像名称
std::string m_path = "C:\\IMAGE\\ranger3";
std::string m_CalibratePath = "C:\\IMAGE\\Calibrate";
std::string m_RedifyPath = "C:\\IMAGE\\Redify";
std::string m_CalibrateAndRedifyPath = "C:\\IMAGE\\CalibrateAndRedify";
std::string m_fileName = "Image";
std::string m_CalibrateName = "Calibrate";
std::string m_RedifyName = "Redify";
std::string m_CalibratePathAndRedifyName = "CalibrateAndRedify";

//全局函数
void writeSensorRangeTraits(DatXmlWriter& xml, const int64_t bufferWidth,
	const int64_t aoiHeight, const int64_t aoiOffsetX,
	const int64_t aoiOffsetY);
void writeSubComponentRange16(DatXmlWriter& xml, const int64_t bufferWidth);
void writeSubComponentReflectance(DatXmlWriter& xml, const int64_t bufferWidth);

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	// 实现
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

// CLusterCameraControlCDemoDlg 对话框
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

// CLusterCameraControlCDemoDlg 消息处理程序
BOOL CLusterCameraControlCDemoDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	
	//默认设置为显示Range图像
	f_Image.SetCheck(0);
	f_Range.SetCheck(1);

	//初始化存储路径
	GetDlgItem(IDC_EDIT_SavePath)->SetWindowText(m_path.c_str());

	//按键初始化
	GetDlgItem(ID_BTN_CamDetectNum)->EnableWindow(TRUE);
	GetDlgItem(ID_BTN_OpenCamera)->EnableWindow(FALSE);
	GetDlgItem(ID_BTN_CloseCamera)->EnableWindow(FALSE);
	GetDlgItem(ID_BTN_Snap)->EnableWindow(FALSE);
	GetDlgItem(ID_BTN_StartGrab)->EnableWindow(FALSE);
	GetDlgItem(ID_BTN_StopGrab)->EnableWindow(FALSE);
	GetDlgItem(ID_BTN_StartRecord)->EnableWindow(TRUE);
	GetDlgItem(ID_BTN_StopRecord)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_FeatureSet)->EnableWindow(FALSE);
	
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CLusterCameraControlCDemoDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CLusterCameraControlCDemoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

/******************************************************
@函数说明： 窗口关闭事件
@参数输入： 无
@参数输出： 无
@返回值  ： 无
******************************************************/
void CLusterCameraControlCDemoDlg::OnClose()
{
	CDialogEx::OnClose();
}


//@函数说明： 相机检测
void CLusterCameraControlCDemoDlg::OnBnClickedBtnCamdetectnum()
{
	// TODO: 在此添加控件通知处理程序代码
	LE_STATUS eResult = LCC_C_DetectCameraNum(m_pstCameraInfo, &m_iCameraNum);
	if (eResult == L_OK)
	{
		CString str;
		str.Format("目前检索到  %d  个相机", m_iCameraNum);
		AfxMessageBox(str);
	}
	else
	{
		ShowErrorInfo(eResult, "检索相机失败");
		return;
	}

	//按键使能
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

//@函数说明： 打开相机
void CLusterCameraControlCDemoDlg::OnBnClickedBtnOpencamera()
{
	// TODO: 在此添加控件通知处理程序代码
	int iCameraIndex = 0;
	LE_STATUS eResult;

	eResult = LCC_C_OpenCamera(iCameraIndex, "..\\XmlFile\\LCC_C_Sick_Ranger3_1.0.0.xml", &m_pCameraHandle[iCameraIndex]);
	LCC_C_SetFeatureString(m_pCameraHandle[iCameraIndex], LFNC_SICK_PARAMETPATH, "");
	
	//LCC_C_LoadCSV(0, "1.csv");

	return;

	if (eResult == L_OK)
	{
		AfxMessageBox("打开相机0成功!");

		m_pstBufferInfo[iCameraIndex] = (LST_BUFFER_INFO*)malloc(sizeof(LST_BUFFER_INFO));
		m_pstBufferInfo[iCameraIndex]->iBufferNum = 5;
		m_pstBufferInfo[iCameraIndex]->iMergeFrameNum = 1;
		m_pstBufferInfo[iCameraIndex]->achReserved[0] = '\0';
		eResult = LCC_C_InitCamera(m_pCameraHandle[iCameraIndex], *(m_pstBufferInfo[iCameraIndex]));
		if (eResult == L_OK)
		{
			AfxMessageBox("初始化相机成功!");

			LCC_C_RegisterCallback(StoreCallBack, CB_STORE, m_pCameraHandle[iCameraIndex], this);
			LCC_C_RegisterCallback(DisplayCallBack, CB_DISPLAY, m_pCameraHandle[iCameraIndex], this);
			LCC_C_RegisterCallback(MessageCallBack, CB_MESSAGE, m_pCameraHandle[iCameraIndex], this);

			//按键使能
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
			ShowErrorInfo(eResult, "初始化相机失败");
		}

	}
	else
	{
		ShowErrorInfo(eResult, "打开相机失败");
	}

	//LCC_C_LoadCSV(0, "1.csv");
	LST_BOOLEAN_INFO b;
	b.bValue = false;
	eResult = LCC_C_GetFeatureBoolean(m_pCameraHandle[iCameraIndex], LFNC_SICK_REFLECTANCE_ENABLE, &b);
	//mm_height3D = stiIntInfo.iValue;



	eResult = LCC_C_SetFeatureEnum(m_pCameraHandle[iCameraIndex], LFNC_SICK_REGION_SELECTOR, 1);
	if (eResult != L_OK)
	{
		ShowErrorInfo(eResult, "设置为Region1模式失败");
	}

	//读取图像高度;
	LST_INT_INFO stiIntInfo;
	eResult = LCC_C_GetFeatureInt(m_pCameraHandle[iCameraIndex], LFNC_HEIGHT, &stiIntInfo);
	mm_height3D = stiIntInfo.iValue;

	//读取图像offsety;
	eResult = LCC_C_GetFeatureInt(m_pCameraHandle[iCameraIndex], LFNC_OFFSET_Y, &stiIntInfo);
	mm_offsetY3D = stiIntInfo.iValue;

	//读取图像offsetx;
	eResult = LCC_C_GetFeatureInt(m_pCameraHandle[iCameraIndex], LFNC_OFFSET_X, &stiIntInfo);
	mm_offsetX3D = stiIntInfo.iValue;



	//iCameraIndex = 1;
	//eResult = LCC_C_OpenCamera(iCameraIndex, "..\\XmlFile\\LCC_C_Sick_Ranger3_1.0.0.xml", &m_pCameraHandle[iCameraIndex]);

	//if (eResult == L_OK)
	//{
	//	AfxMessageBox("打开相机1成功!");

	//	m_pstBufferInfo[iCameraIndex] = (LST_BUFFER_INFO*)malloc(sizeof(LST_BUFFER_INFO));
	//	m_pstBufferInfo[iCameraIndex]->iBufferNum = 5;
	//	m_pstBufferInfo[iCameraIndex]->iMergeFrameNum = 1;
	//	m_pstBufferInfo[iCameraIndex]->achReserved[0] = '\0';
	//	eResult = LCC_C_InitCamera(m_pCameraHandle[iCameraIndex], *(m_pstBufferInfo[iCameraIndex]));
	//	if (eResult == L_OK)
	//	{
	//		AfxMessageBox("初始化相机成功!");

	//		LCC_C_RegisterCallback(StoreCallBack, CB_STORE, m_pCameraHandle[iCameraIndex], this);
	//		LCC_C_RegisterCallback(DisplayCallBack, CB_DISPLAY, m_pCameraHandle[iCameraIndex], this);
	//		LCC_C_RegisterCallback(MessageCallBack, CB_MESSAGE, m_pCameraHandle[iCameraIndex], this);

	//		//按键使能
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
	//		ShowErrorInfo(eResult, "初始化相机失败");
	//	}

	//}
	//else
	//{
	//	ShowErrorInfo(eResult, "打开相机失败");
	//}
	//LCC_C_LoadCSV(1, "1.csv");

	////导入配置文件（sick.csv）（若不需要导入配置文件，可不调用此接口）
	//char *inputPath = "..\\XmlFile\\true300.csv";//若导入配置文件失败，可能存在文件损坏的情况，可在sick采集软件中重新生成
	//eResult = LCC_C_SetFeatureString(m_pCameraHandle[iCameraIndex], LFNC_SICK_CONFIGURATION, inputPath);
	//if (eResult != L_OK)
	//{
	//	ShowErrorInfo(eResult, "导入配置文件失败");
	//}
	/*eResult = LCC_C_SetFeatureEnum(m_pCameraHandle[iCameraIndex], LFNC_SICK_TRIGGER_MODE_SELECTOR, 1);
	eResult = LCC_C_SetFeatureEnum(m_pCameraHandle[iCameraIndex], LFNC_TRIG_MODE, 1);
	if (eResult != L_OK)
	{
		AfxMessageBox("设置为外触发失败！");
	}*/

	//读取Region1模式下的参数
	eResult = LCC_C_SetFeatureEnum(m_pCameraHandle[iCameraIndex], LFNC_SICK_REGION_SELECTOR, 1);
	if (eResult != L_OK)
	{
		ShowErrorInfo(eResult, "设置为Region1模式失败");
	}

	//读取图像高度;
	//LST_INT_INFO stiIntInfo;
	eResult = LCC_C_GetFeatureInt(m_pCameraHandle[iCameraIndex], LFNC_HEIGHT, &stiIntInfo);
	mm_height3D = stiIntInfo.iValue;

	//读取图像offsety;
	eResult = LCC_C_GetFeatureInt(m_pCameraHandle[iCameraIndex], LFNC_OFFSET_Y, &stiIntInfo);
	mm_offsetY3D = stiIntInfo.iValue;

	//读取图像offsetx;
	eResult = LCC_C_GetFeatureInt(m_pCameraHandle[iCameraIndex], LFNC_OFFSET_X, &stiIntInfo);
	mm_offsetX3D = stiIntInfo.iValue;

}

//@函数说明： 开始连续采集
void CLusterCameraControlCDemoDlg::OnBnClickedBtnStartgrab()
{
	// TODO: 在此添加控件通知处理程序代码
	int iCameraIndex = 0;
	bBrushRectRanger3 = true;//刷新一次显示界面

	if (m_pCameraHandle[iCameraIndex] != NULL)
	{
		/*停止采集*/
		LE_STATUS eResult = LCC_C_StartGrab(m_pCameraHandle[iCameraIndex]);

		if (eResult == L_OK)
		{
			//按键使能
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
			ShowErrorInfo(eResult, "开始采集失败");
		}
	}


	//iCameraIndex = 0;
	//bBrushRectRanger3 = true;//刷新一次显示界面

	//if (m_pCameraHandle[iCameraIndex] != NULL)
	//{
	//	/*停止采集*/
	//	LE_STATUS eResult = LCC_C_StartGrab(m_pCameraHandle[iCameraIndex]);

	//	if (eResult == L_OK)
	//	{
	//		//按键使能
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
	//		ShowErrorInfo(eResult, "开始采集失败");
	//	}
	//}


}

//@函数说明： 停止连续采集
void CLusterCameraControlCDemoDlg::OnBnClickedBtnStopgrab()
{
	// TODO: 在此添加控件通知处理程序代码
	int iCameraIndex = 0;
	LE_STATUS eResult;

	if (m_pCameraHandle[iCameraIndex] != NULL)
	{
		/*停止采集*/
		eResult = LCC_C_StopGrab(m_pCameraHandle[iCameraIndex]);
		if (eResult != L_OK)
		{
			ShowErrorInfo(eResult, "停止采集失败");
		}
		else
		{
			//AfxMessageBox("停止相机0成功!");

			//按键使能
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
	//	/*停止采集*/
	//	eResult = LCC_C_StopGrab(m_pCameraHandle[iCameraIndex]);
	//	if (eResult != L_OK)
	//	{
	//		ShowErrorInfo(eResult, "停止采集失败");
	//	}
	//	else
	//	{
	//		//AfxMessageBox("停止相机0成功!");

	//		//按键使能
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

//@函数说明： 单帧采集
void CLusterCameraControlCDemoDlg::OnBnClickedBtnSnap()
{
	// TODO: 在此添加控件通知处理程序代码
	int iCameraIndex = 0;
	bBrushRectRanger3 = true;//刷新一次显示界面

	if (m_pCameraHandle[iCameraIndex] != INVALID_CAM_HANDLE)
	{
		/*单帧采集*/
		LE_STATUS eResult = LCC_C_SnapImage(m_pCameraHandle[iCameraIndex]);
		if (eResult == L_OK)
		{
			//AfxMessageBox("单帧采集成功!");

			//按键使能
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
			ShowErrorInfo(eResult, "单帧采集失败");
		}

	}

}

//@函数说明： 关闭相机
void CLusterCameraControlCDemoDlg::OnBnClickedBtnClosecamera()
{
	// TODO: 在此添加控件通知处理程序代码
	int iCameraIndex = 0;
	LE_STATUS eResult;

	eResult = LCC_C_ReleaseCamera(m_pCameraHandle[iCameraIndex]);
	if (eResult == L_OK)
	{
		AfxMessageBox("释放资源相机0成功!");
		if (m_pCameraHandle[iCameraIndex] != INVALID_CAM_HANDLE)
		{
			eResult = LCC_C_CloseCamera(m_pCameraHandle[iCameraIndex]);
			if (eResult == L_OK)
			{
				AfxMessageBox("关闭相机成功!");

				//按键使能
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
				ShowErrorInfo(eResult, "关闭相机失败");
			}
		}
	}

	else
	{
		ShowErrorInfo(eResult, "释放资源失败");
	}

	//释放结构体
	if (m_pstBufferInfo[iCameraIndex] != NULL)
	{
		delete m_pstBufferInfo[iCameraIndex];
		m_pstBufferInfo[iCameraIndex] = NULL;
	}


	//iCameraIndex = 1;

	//eResult = LCC_C_ReleaseCamera(m_pCameraHandle[iCameraIndex]);
	//if (eResult == L_OK)
	//{
	//	AfxMessageBox("释放资源相机0成功!");
	//	if (m_pCameraHandle[iCameraIndex] != INVALID_CAM_HANDLE)
	//	{
	//		eResult = LCC_C_CloseCamera(m_pCameraHandle[iCameraIndex]);
	//		if (eResult == L_OK)
	//		{
	//			AfxMessageBox("关闭相机成功!");

	//			//按键使能
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
	//			ShowErrorInfo(eResult, "关闭相机失败");
	//		}
	//	}
	//}

	//else
	//{
	//	ShowErrorInfo(eResult, "释放资源失败");
	//}

	////释放结构体
	//if (m_pstBufferInfo[iCameraIndex] != NULL)
	//{
	//	delete m_pstBufferInfo[iCameraIndex];
	//	m_pstBufferInfo[iCameraIndex] = NULL;
	//}
}

//@函数说明： 开始存图
void CLusterCameraControlCDemoDlg::OnBnClickedBtnStartrecord()
{
	// TODO: 在此添加控件通知处理程序代码
	m_iRecord = 1;

	CString strPath;
	CString strName;
	GetDlgItem(IDC_EDIT_SavePath)->GetWindowText(strPath);
	if (!PathFileExists(strPath))
	{
		if (CreateDirectoryA(strPath, NULL))
		{
			::MessageBox(NULL, TEXT("存储路径不存在,已创建！"), NULL, MB_OK);//返回原来参数设置界面
			//strPath.Replace(_T("\\"),_T("/"));
			m_path = strPath;
		}
		else
		{
			::MessageBox(NULL, TEXT("存储路径不存在,创建失败，请重新设置！"), NULL, MB_OK);//返回原来参数设置界面
		}
		return;
	}
	else
	{
		m_path = strPath;
	}
	
	//按键使能
	GetDlgItem(ID_BTN_StartRecord)->EnableWindow(FALSE);
	GetDlgItem(ID_BTN_StopRecord)->EnableWindow(TRUE);
}

//@函数说明： 停止存图
void CLusterCameraControlCDemoDlg::OnBnClickedBtnStoprecord()
{
	// TODO: 在此添加控件通知处理程序代码
	m_iRecord = 2;

	//按键使能
	GetDlgItem(ID_BTN_StartRecord)->EnableWindow(TRUE);
	GetDlgItem(ID_BTN_StopRecord)->EnableWindow(FALSE);
}

//@函数说明： 参数设置
void CLusterCameraControlCDemoDlg::OnBnClickedBtnFeatureset()
{
	// TODO: 在此添加控件通知处理程序代码
	FeatureSet newdlg;
	newdlg.DoModal();
}


//@函数说明： 存储回调函数
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

	//TODO：数据处理、图像存储等操作
	if (iCameraIndex == 0)
	{
		LST_IMAGE_INFO *pstDisplayImageInfo = (LST_IMAGE_INFO*)pData;

		//bufferPath：原始图像存储路径；bufferPathCalibrate：标定后图像存储路径；
		//bufferPathRedify：修改后图像存储路径；bufferPathCalibrateAndRedify：标定并修改后图像存储路径
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
				const uint8_t*buff = (uint8_t*)pstDisplayImageInfo->pchBuffer;//转换buffer格式，从BYTE到unit_8
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
			if (m_iRecord == 1)//开启存储
			{
				const uint8_t*buff = (uint8_t*)pstDisplayImageInfo->pchBuffer;//转换buffer格式，从BYTE到unit_8

				//获取当前存图模式：true：Range+rReflectance；false：Range
				LST_BOOLEAN_INFO stBooleanInfo;
				LE_STATUS eResult = LCC_C_GetFeatureBoolean(pDlg->m_pCameraHandle[iCameraIndex], LFNC_SICK_REFLECTANCE_ENABLE, &stBooleanInfo);
				if (eResult != L_OK)
				{
					AfxMessageBox("读取“REFLECTANCE_ENABLE”值错误！");
				}

				//CalibrationError errorlast;
				if (stBooleanInfo.bValue == true)//当前存图模式：Range+rReflectance
				{
					uint8_t* buffer16;
					size_t buffer16Size;
					uint8_t* buffer8;
					size_t buffer8Size;

					//buffer16:Range图；buffer8：灰度图
					buffer16Size = pstDisplayImageInfo->stImageProperty.iImageWidth*pstDisplayImageInfo->stImageProperty.iImageHeight * 2;
					buffer16 = (uint8_t*)malloc(buffer16Size);
					buffer8Size = pstDisplayImageInfo->stImageProperty.iImageWidth*pstDisplayImageInfo->stImageProperty.iImageHeight;
					buffer8 = (uint8_t*)malloc(buffer8Size);

					memcpy(buffer16, buff, buffer16Size);
					memcpy(buffer8, buff + buffer16Size, buffer8Size);

					//存图到本地
					pDlg->saveBufferRangeAndReflectance(buffer16,
						buffer8,
						pstDisplayImageInfo->stImageProperty.iImageWidth,
						pstDisplayImageInfo->stImageProperty.iImageHeight,
						mm_height3D,
						mm_offsetX3D,
						mm_offsetY3D,
						bufferPath.str());

					//标定，修改标定图像，标定并修改图像（若不需要标定，可不调用此接口）
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
					delete reflectanceRect;//----------修改（有reflectance时，启用这段代码）
					delete calFilter;


					free(buffer16);
					free(buffer8);
				}

				else if (stBooleanInfo.bValue == false)//当前存图模式：Range
				{
					uint8_t* buffer16;//Range图
					size_t buffer16Size;

					buffer16Size = pstDisplayImageInfo->stImageProperty.iImageWidth*pstDisplayImageInfo->stImageProperty.iImageHeight * 2;
					buffer16 = (uint8_t*)malloc(buffer16Size);

					memcpy(buffer16, buff, buffer16Size);

					//存图到本地
					pDlg->saveBuffer16(buffer16,
						pstDisplayImageInfo->stImageProperty.iImageWidth,
						pstDisplayImageInfo->stImageProperty.iImageHeight,
						mm_height3D,
						mm_offsetX3D,
						mm_offsetY3D,
						bufferPath.str());

					//标定，修改标定图像，标定并修改图像（若不需要标定，可不调用此接口）
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

//@函数说明： 显示回调函数
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
				AfxMessageBox("读取“REFLECTANCE_ENABLE”值错误！");
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

//@函数说明： 信息回调函数
void MessageCallBack(int iCallBackType, void* pCameraHandle, void* pData, void* pUserContext)
{

}

/***********************************************************************
*
*				       全局函数实现
*
***********************************************************************/

/******************************************************
@函数说明： 获得返回状态的详细描述信息。
@参数输入： eResult -- 函数返回值状态。
@参数输出： 函数返回值状态对应的描述信息
@返回值  ： 无
******************************************************/
void CLusterCameraControlCDemoDlg::GetStatusInfo(IN LE_STATUS eStatus, OUT char* pchInfo)
{
	switch (eStatus)
	{
	case L_ERR_CCNOED:
		strcpy(pchInfo, "L_ERR_CCNFS, 没有连接图像采集设备");
		break;
	case L_ERR_CCWC:
		strcpy(pchInfo, "L_ERR_CCWC, 错误的彩色位深");
		break;
	case L_ERR_CCWD:
		strcpy(pchInfo, "L_ERR_CCWD, 错误的采集设备");
		break;
	case L_ERR_CCWPF:
		strcpy(pchInfo, "L_ERR_CCWPF, 错误或不支持的像素格式");
		break;
	case L_ERR_CCNV:
		strcpy(pchInfo, "L_ERR_CCNV, 没有视频信号");
		break;
	case L_ERR_CCU:
		strcpy(pchInfo, "L_ERR_CCU, 未知的图像采集设备");
		break;
	case L_ERR_CCF:
		strcpy(pchInfo, "L_ERR_CCF, 图像抓取失败");
		break;
	case L_ERR_CCWR:
		strcpy(pchInfo, "L_ERR_CCWR, 选择错误的图像分辨率");
		break;
	case L_ERR_CCWP:
		strcpy(pchInfo, "L_ERR_CCWP, 选择错误的图像部分");
		break;
	case L_ERR_CCWPR:
		strcpy(pchInfo, "L_ERR_CCWPR, 选择错误的像素比例");
		break;
	case L_ERR_CCWH:
		strcpy(pchInfo, "L_ERR_CCWH, 句柄无效");
		break;
	case L_ERR_CCCL:
		strcpy(pchInfo, "L_ERR_CCCL, 实例无效（或者已经注销）");
		break;
	case L_ERR_CCNIB:
		strcpy(pchInfo, "L_ERR_CCNIB, 无法初始化缓存");
		break;
	case L_ERR_CCET:
		strcpy(pchInfo, "L_ERR_CCET, 不支持外触发");
		break;
	case L_ERR_CCLI:
		strcpy(pchInfo, "L_ERR_CCLI, 错误的相机输入行（多重）");
		break;
	case L_ERR_CCCS:
		strcpy(pchInfo, "L_ERR_CCCS, 错误的颜色空间");
		break;
	case L_ERR_CCPT:
		strcpy(pchInfo, "L_ERR_CCPT, 错误的端口");
		break;
	case L_ERR_CCCT:
		strcpy(pchInfo, "L_ERR_CCCT, 错误的相机类型");
		break;
	case L_ERR_CCTM:
		strcpy(pchInfo, "L_ERR_CCTM, 超过了采集设备类的最大数目");
		break;
	case L_ERR_CCDV:
		strcpy(pchInfo, "L_ERR_CCDV, 设备忙碌");
		break;
	case L_ERR_CCASYNC:
		strcpy(pchInfo, "L_ERR_CCASYNC, 不支持异步抓取");
		break;
	case L_ERR_CCPARAM:
		strcpy(pchInfo, "L_ERR_CCPARAM, 该参数暂不支持");
		break;
	case L_ERR_CCTIMEOUT:
		strcpy(pchInfo, "L_ERR_CCTIMEOUT, 超时");
		break;
	case L_ERR_CCGAIN:
		strcpy(pchInfo, "L_ERR_CCGAIN, 无效的增益");
		break;
	case L_ERR_CCFIELD:
		strcpy(pchInfo, "L_ERR_CCFIELD, 无效的字段");
		break;
	case L_ERR_CCPART:
		strcpy(pchInfo, "L_ERR_CCPART, 无效的参数类型");
		break;
	case L_ERR_CCPARV:
		strcpy(pchInfo, "L_ERR_CCPARV, 无效的参数值");
		break;
	case L_ERR_CCFNS:
		strcpy(pchInfo, "L_ERR_CCFNS, 不支持或已被遗弃的功能");
		break;
	case L_ERR_CCIVERS:
		strcpy(pchInfo, "L_ERR_CCIVERS, 版本不兼容的接口");
		break;
	case L_ERR_CCSETPAR:
		strcpy(pchInfo, "L_ERR_CCSETPAR, 设置参数值失败");
		break;
	case L_ERR_CCGETPAR:
		strcpy(pchInfo, "L_ERR_CCGETPAR, 查询参数值失败");
		break;
	case L_ERR_CCPARNA:
		strcpy(pchInfo, "L_ERR_CCPARNA, 当前配置下，该参数不可用");
		break;
	case L_ERR_CCCLOSE:
		strcpy(pchInfo, "L_ERR_CCCLOSE, 设备不能正常关闭");
		break;
	case L_ERR_CCCAMFILE:
		strcpy(pchInfo, "L_ERR_CCCAMFILE, 相机配置文件不能打开");
		break;
	case L_ERR_CCCALLBACK:
		strcpy(pchInfo, "L_ERR_CCCALLBACK, 不支持的回调类型");
		break;
	case L_ERR_CCDEVLOST:
		strcpy(pchInfo, "L_ERR_CCDEVLOST, 设备丢失");
		break;
	case L_ERR_DNA:
		strcpy(pchInfo, "L_ERR_DNA, 设备或操作不可用");
		break;
	case L_ERR_MEM:
		strcpy(pchInfo, "L_ERR_MEM, 内存不足");
		break;
	case L_ERR_CCFWIDX:
		strcpy(pchInfo, "L_ERR_CCFWIDX, 无效的相机索引");
		break;
	case L_ERR_CCFWVI:
		strcpy(pchInfo, "L_ERR_CCFWVI, 厂家接口错误");
		break;
	case L_ERR_CCXML:
		strcpy(pchInfo, "L_ERR_CCXML, XML配置文件不能打开");
		break;
	case L_ERR_CCFIDFV:
		strcpy(pchInfo, "L_ERR_CCFIDFV, 默认参数初始化失败");
		break;
	case L_ERR_CCOPEN:
		strcpy(pchInfo, "L_ERR_CCOPEN, 打开相机失败");
		break;
	case L_ERR_CCRXFVF:
		strcpy(pchInfo, "L_ERR_CCRXFVF, 读取XML参数值失败");
		break;
	case L_ERR_CCNRB:
		strcpy(pchInfo, "L_ERR_CCNRB, 无法释放缓存");
		break;
	case L_ERR_CCWLPI:
		strcpy(pchInfo, "L_ERR_CCWLPI, 无法加载插件，可能名称有误，或缺失依赖文件");
		break;
	case L_ERR_CCWFPI:
		strcpy(pchInfo, "L_ERR_CCWFPI, 无法释放插件");
		break;
	case L_ERR_UNKNOWN:
		strcpy(pchInfo, "L_ERR_UNKNOWN, 未知错误");
		break;
	case L_OK:
		strcpy(pchInfo, "L_OK, 正常");
		break;
	default:
		strcpy(pchInfo, "新版本错误代码");
		break;
	}
}

/******************************************************
@函数说明： 显示错误信息
@参数输入： eResult -- 调用相机控制接口，返回的状态
pchErrorInfo -- 错误信息
@参数输出： 无
@返回值  ： 无
******************************************************/
void CLusterCameraControlCDemoDlg::ShowErrorInfo(LE_STATUS eResult, char *pchErrorInfo)
{
	const int INFO_LEN = 256;
	char achInfo[INFO_LEN];
	memset(achInfo, 0, INFO_LEN);
	CLusterCameraControlCDemoDlg::GetStatusInfo(eResult, achInfo);
	CString strMsg;
	strMsg.Format(_T("%s。错误代码：0x%x，错误信息：%s"), pchErrorInfo, eResult, achInfo);
	AfxMessageBox(strMsg);
}

//创建图片对象
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

//复制图片;
bool CLusterCameraControlCDemoDlg::ImageCopy(const CImage &srcImage, CImage &destImage)
{
	int i;//循环变量  
	if (srcImage.IsNull())
		return false;
	//源图像参数  
	BYTE* srcPtr = (BYTE*)srcImage.GetBits();
	int srcBitsCount = srcImage.GetBPP();
	int srcWidth = srcImage.GetWidth();
	int srcHeight = srcImage.GetHeight();
	int srcPitch = srcImage.GetPitch();
	//销毁原有图像  
	if (!destImage.IsNull())
	{
		destImage.Destroy();
	}
	//创建新图像  
	if (srcBitsCount == 32)   //支持alpha通道  
	{
		destImage.Create(srcWidth, srcHeight, srcBitsCount, 1);
	}
	else
	{
		destImage.Create(srcWidth, srcHeight, srcBitsCount, 0);
	}
	//加载调色板  
	if (srcBitsCount <= 8 && srcImage.IsIndexed())//检测是否需要调色板  ;
	{
		RGBQUAD pal[256];
		int nColors = srcImage.GetMaxColorTableEntries();
		if (nColors>0)
		{
			srcImage.GetColorTable(0, nColors, pal);
			destImage.SetColorTable(0, nColors, pal);//复制调色板程序  
		}
	}
	//目标图像参数  
	BYTE *destPtr = (BYTE*)destImage.GetBits();
	int destPitch = destImage.GetPitch();
	//复制图像数据  
	for (i = 0; i<srcHeight; i++)
	{
		memcpy(destPtr + i*destPitch, srcPtr + i*srcPitch, abs(srcPitch));
	}

	return TRUE;
}

//画2D图
void CLusterCameraControlCDemoDlg::DrawPicture(int nID, CImage pImage)
{
	//将图片显示在主界面
	//std::_Max_element()
	if (!pImage.IsNull())
	{

		int imgWidth = pImage.GetWidth();//2560
		int imgHeight = pImage.GetHeight();//200
		CWnd *m_Wnd2D = GetDlgItem(nID);
		m_Wnd2D->UpdateWindow();
		CStatic *m_PicCtl = (CStatic*)GetDlgItem(nID);
		CRect rect2D;
		m_Wnd2D->GetClientRect(&rect2D);//改为符合图像大小的矩形框
		CDC *pDc = m_Wnd2D->GetDC();

		//刷新一次显示界面
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
		SetStretchBltMode(pDc->m_hDC, STRETCH_HALFTONE);			//图像大小适应2D窗口大小
		//pImage.StretchBlt(pDc->m_hDC,rect2D,SRCCOPY);
		pImage.StretchBlt(pDc->m_hDC, x0, y0, x1, y1, SRCCOPY);
		m_Wnd2D->ReleaseDC(pDc);
	}

	//delete[] OriData;
}

//绘制图片
void CLusterCameraControlCDemoDlg::DrawImage()
{

	if (!mImage.IsNull())
	{
		CImage image;
		ImageCopy(mImage, image);//dstimage:image;
		DrawPicture(IDC_PICTURE_BOX, image);
	}

}

//存储16+8位图
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

//存储16位图
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

//存储8位图
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

//写存图xml文件
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

//写Range buffer16到xml文件
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

//写Reflectance buffer到xml文件
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
	// TODO: 在此添加控件通知处理程序代码
	m_Image = true;
	int iCameraIndex = 0;
	LE_STATUS eResult = LCC_C_SetFeatureBoolean(m_pCameraHandle[iCameraIndex], LFNC_SICK_IMAGE2D, true);
	f_Image.SetCheck(1);
	f_Range.SetCheck(0);
}


void CLusterCameraControlCDemoDlg::OnBnClickedRadioRange()
{
	// TODO: 在此添加控件通知处理程序代码
	m_Image = false;
	int iCameraIndex = 0;
	LE_STATUS eResult = LCC_C_SetFeatureBoolean(m_pCameraHandle[iCameraIndex], LFNC_SICK_IMAGE2D, false);
	f_Image.SetCheck(0);
	f_Range.SetCheck(1);
}


void CLusterCameraControlCDemoDlg::OnBnClickedButtonChangesavepath()
{
	// TODO:  在此添加控件通知处理程序代码
	SavePath newdlg;
	newdlg.DoModal();
	f_SavePath.SetWindowText(m_path.c_str());
}
