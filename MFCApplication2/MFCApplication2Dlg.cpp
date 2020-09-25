
// MFCApplication2Dlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MFCApplication2.h"
#include "MFCApplication2Dlg.h"
#include "afxdialogex.h"
#include <string>
#include <iostream>
#include <fstream>
#include <stdint.h>
#include <algorithm>
#include <sstream>
#include <string>



#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void SaveCalBuffer(
	const char*   dataBuffer,
	int dataSize,
	bool  hasReflectance,
	int64_t bufferWidth,
	int64_t bufferHeight,
	int64_t lowerBoundX,
	int64_t lowerBoundR,
	int64_t upperBoundX,
	int64_t upperBoundR,
	const std::string& filePath);

void SaveBufferRaw(
	const char* rangeBuffer,
	int dataSize,
	bool  hasReflectance,
	const int64_t bufferWidth,
	const int64_t bufferHeight,
	const int r1Height,
	const int r1OffsetY,
	const int rangeAxisFlat,
	const std::string& filePath);

//将wstring转换成string  
std::string wstring2string(std::wstring wstr)
{
	std::string result;
	//获取缓冲区大小，并申请空间，缓冲区大小事按字节计算的  
	int len = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.size(), NULL, 0, NULL, NULL);
	char* buffer = new char[len + 1];
	//宽字节编码转换成多字节编码  
	WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.size(), buffer, len, NULL, NULL);
	buffer[len] = '\0';
	//删除缓冲区并返回值  
	result.append(buffer);
	delete[] buffer;
	return result;
}
// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMFCApplication2Dlg 对话框



CMFCApplication2Dlg::CMFCApplication2Dlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMFCApplication2Dlg::IDD, pParent)
	, m_GrabImageCount(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	count = 0;
}

void CMFCApplication2Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_GRABIMAGECOUNT_EDIT1, m_GrabImageCount);
}

BEGIN_MESSAGE_MAP(CMFCApplication2Dlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_CHECK_BTN1, &CMFCApplication2Dlg::OnBnClickedCheckBtn1)
	ON_BN_CLICKED(IDC_OPENCAM_BTN1, &CMFCApplication2Dlg::OnBnClickedOpencamBtn1)
	ON_BN_CLICKED(IDC_CLOSECAM_BTN1, &CMFCApplication2Dlg::OnBnClickedClosecamBtn1)
	ON_BN_CLICKED(IDC_STARTGRAB_BTN1, &CMFCApplication2Dlg::OnBnClickedStartgrabBtn1)
	ON_BN_CLICKED(IDC_STOPGRAB_BUTTON1, &CMFCApplication2Dlg::OnBnClickedStopgrabButton1)
	ON_BN_CLICKED(IDC_PARAMCONFIGFILE_BTN1, &CMFCApplication2Dlg::OnBnClickedParamconfigfileBtn1)
	ON_BN_CLICKED(IDC_CALFILE_BTN1, &CMFCApplication2Dlg::OnBnClickedCalfileBtn1)
	ON_BN_CLICKED(IDC_SAVEPATH_BTN1, &CMFCApplication2Dlg::OnBnClickedSavepathBtn1)
	ON_BN_CLICKED(IDC_CHECK1, &CMFCApplication2Dlg::OnBnClickedCheck1)
END_MESSAGE_MAP()


// CMFCApplication2Dlg 消息处理程序

BOOL CMFCApplication2Dlg::OnInitDialog()
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

	// TODO:  在此添加额外的初始化代码
	IsSaveData = false;
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

//@函数说明： 存储回调函数
void StoreCallBack(int iCallBackType, void* pCameraHandle, void* pData, void* pUserContext)
{
	CMFCApplication2Dlg *pDlg = static_cast<CMFCApplication2Dlg*>(pUserContext);
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

	
	if (iCameraIndex == 0 && pDlg->IsSaveData)
	{
		LST_IMAGE_INFO *pstStoreImageInfo = (LST_IMAGE_INFO*)pData;
		int imgWidth = pstStoreImageInfo->stImageProperty.iImageWidth;
		int imgHeight = pstStoreImageInfo->stImageProperty.iImageHeight;
		LST_RANGER3XML_INFO* pstStoreRanger3XmlInfo = (LST_RANGER3XML_INFO*)pstStoreImageInfo->pstTagInfo;
		int dataSize = pstStoreImageInfo->iBufferSize;

		
		char fileName[255] = { 0 };
		sprintf_s(fileName, "%s/%d%d%d%d_%d", pDlg->SaveImagePath.c_str(), pstStoreImageInfo->stSysTime.iHour, pstStoreImageInfo->stSysTime.iMinute,
			pstStoreImageInfo->stSysTime.iSecond, pstStoreImageInfo->stSysTime.iMilliSeconds, pDlg->count++);
			
		/*char* buffer = new char[dataSize];
		memcpy(buffer, pstStoreImageInfo->pchBuffer, dataSize);

		SaveCalBuffer((char*)buffer,
			dataSize,
			pstStoreRanger3XmlInfo->tagHasReflectance,
			imgWidth,
			imgHeight,
			pstStoreRanger3XmlInfo->CoordinateInfo.stCalibrationCoordInfo.LowerBoundX,
			pstStoreRanger3XmlInfo->CoordinateInfo.stCalibrationCoordInfo.LowerBoundR,
			pstStoreRanger3XmlInfo->CoordinateInfo.stCalibrationCoordInfo.UpperBoundX,
			pstStoreRanger3XmlInfo->CoordinateInfo.stCalibrationCoordInfo.UpperBoundR,
			fileName);

		if (buffer != nullptr)
		{
			delete[] buffer;
			buffer = nullptr;
		}*/

		//判断是已标定数据还是未标定数据
		if (pstStoreRanger3XmlInfo->tagCalibration)
		{
			char* buffer = new char[dataSize];
			memcpy(buffer, pstStoreImageInfo->pchBuffer, dataSize);

			SaveCalBuffer(buffer,
				dataSize,
				pstStoreRanger3XmlInfo->tagHasReflectance,
				imgWidth,
				imgHeight,
				pstStoreRanger3XmlInfo->CoordinateInfo.stCalibrationCoordInfo.LowerBoundX,
				pstStoreRanger3XmlInfo->CoordinateInfo.stCalibrationCoordInfo.LowerBoundR,
				pstStoreRanger3XmlInfo->CoordinateInfo.stCalibrationCoordInfo.UpperBoundX,
				pstStoreRanger3XmlInfo->CoordinateInfo.stCalibrationCoordInfo.UpperBoundR,
				fileName);

			if (buffer != nullptr)
			{
				delete[] buffer;
				buffer = nullptr;
			}
		}
		else
		{
			char* buffer = new char[dataSize];
			memcpy(buffer, pstStoreImageInfo->pchBuffer, dataSize);

			SaveBufferRaw(buffer,
				dataSize,
				pstStoreRanger3XmlInfo->tagHasReflectance,
				imgWidth,
				imgHeight,
				pstStoreRanger3XmlInfo->CoordinateInfo.stCoordInfo.RegionHeight,
				pstStoreRanger3XmlInfo->CoordinateInfo.stCoordInfo.RegionOffsetY,
				pstStoreRanger3XmlInfo->CoordinateInfo.stCoordInfo.RangAixFlat,
				fileName);

			if (buffer != nullptr)
			{
				delete[] buffer;
				buffer = nullptr;
			}
		}

		
		CString str;
		str.Format(_T("%d"), ++pDlg->m_GrabImageCount);
		pDlg->SetDlgItemText(IDC_GRABIMAGECOUNT_EDIT1, str);
	}
}

//@函数说明： 显示回调函数
void DisplayCallBack(int iCallBackType, void* pCameraHandle, void* pData, void* pUserContext)
{
	CMFCApplication2Dlg *pDlg = (CMFCApplication2Dlg*)pUserContext;
	LST_IMAGE_INFO *pstDisplayImageInfo = (LST_IMAGE_INFO*)pData;

	int iWidth = pstDisplayImageInfo->stImageProperty.iImageWidth;
	int iHeight = pstDisplayImageInfo->stImageProperty.iImageHeight;

	LST_RANGER3XML_INFO* pstStoreRanger3XmlInfo = (LST_RANGER3XML_INFO*)pstDisplayImageInfo->pstTagInfo;

	//判断是已标定数据还是未标定数据
	if (pstStoreRanger3XmlInfo->tagCalibration)
	{
		float* rangerBuffer = new float[iWidth*iHeight];
		//memset(rangerBuffer, 0x00, iWidth*iHeight*sizeof(float));

		int offset = iWidth*iHeight*sizeof(float);
		memcpy(rangerBuffer, pstDisplayImageInfo->pchBuffer + offset, iWidth*iHeight*sizeof(float));

		cv::Mat src = pDlg->pIF.floatPtrToMat(rangerBuffer, iHeight, iWidth).clone();

		pDlg->ShowMatImgToWnd(pDlg->GetDlgItem(IDC_PICTURE_BOX), src);

		if (rangerBuffer != nullptr)
		{
			delete[] rangerBuffer;
			rangerBuffer = nullptr;
		}
	}
	else
	{
		uint16_t* rangerBuffer = new uint16_t[iWidth*iHeight];
		memcpy(rangerBuffer, pstDisplayImageInfo->pchBuffer, iWidth*iHeight*sizeof(uint16_t));

		cv::Mat src = pDlg->pIF.uint16_tPtrToMat(rangerBuffer, iHeight, iWidth).clone();

		pDlg->ShowMatImgToWnd(pDlg->GetDlgItem(IDC_PICTURE_BOX), src);

		if (rangerBuffer != nullptr)
		{
			delete[] rangerBuffer;
			rangerBuffer = nullptr;
		}
	}
}

//@函数说明： 信息回调函数
void MessageCallBack(int iCallBackType, void* pCameraHandle, void* pData, void* pUserContext)
{

}
void CMFCApplication2Dlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CMFCApplication2Dlg::OnPaint()
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
HCURSOR CMFCApplication2Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CMFCApplication2Dlg::OnBnClickedCheckBtn1()
{
	// TODO:  在此添加控件通知处理程序代码

	LE_STATUS eResult = LCC_C_DetectCameraNum(m_pstCameraInfo, &m_iCameraNum);
	if (eResult == L_OK)
	{
		CString str;
		str.Format(_T("目前检索到  %d  个相机"), m_iCameraNum);
		AfxMessageBox(str);
	}
}


void CMFCApplication2Dlg::OnBnClickedOpencamBtn1()
{
	// TODO:  在此添加控件通知处理程序代码
	int iCameraIndex = 0;
	LE_STATUS eResult = eResult = LCC_C_OpenCamera(iCameraIndex, "..\\XmlFile\\LCC_C_Sick_Ranger3_3.0.0.xml", &m_pCameraHandle[iCameraIndex]);
	if (eResult == L_OK)
	{
		
		m_pstBufferInfo[iCameraIndex] = (LST_BUFFER_INFO*)malloc(sizeof(LST_BUFFER_INFO));
		m_pstBufferInfo[iCameraIndex]->iBufferNum = 5;
		m_pstBufferInfo[iCameraIndex]->iMergeFrameNum = 1;
		m_pstBufferInfo[iCameraIndex]->achReserved[0] = '\0';
		eResult = LCC_C_InitCamera(m_pCameraHandle[iCameraIndex], *(m_pstBufferInfo[iCameraIndex]));
		if (eResult == L_OK)
		{
			AfxMessageBox(_T("初始化相机成功!"));

			LCC_C_RegisterCallback(StoreCallBack, CB_STORE, m_pCameraHandle[iCameraIndex], this);
			LCC_C_RegisterCallback(DisplayCallBack, CB_DISPLAY, m_pCameraHandle[iCameraIndex], this);
			LCC_C_RegisterCallback(MessageCallBack, CB_MESSAGE, m_pCameraHandle[iCameraIndex], this);

			CString str;
			str.Format(_T("第  %d  号索引相机打开成功!"), m_iCameraNum);
			AfxMessageBox(str);
			SetDlgItemText(IDC_EDIT4, _T("相机打开"));
			LST_STRING_INFO lstStringInfo;
			LCC_C_GetFeatureString(m_pCameraHandle[iCameraIndex], LFNC_SICK_CAMMAC, &lstStringInfo);
			CString strCamMac(lstStringInfo.achValue);
			SetDlgItemText(IDC_CAMMAC_EDIT5, strCamMac);
			LCC_C_GetFeatureString(m_pCameraHandle[iCameraIndex], LFNC_SICK_DEVICENAME, &lstStringInfo);
			CString strDeviceName(lstStringInfo.achValue);
			SetDlgItemText(IDC_DEVICENAME_EDIT6, strDeviceName);
		}	
		else
		{
			AfxMessageBox(_T("初始化相机失败!"));
		}
	}
	else
	{
		AfxMessageBox(_T("打开相机失败!"));
	}
}


void CMFCApplication2Dlg::OnBnClickedClosecamBtn1()
{
	// TODO:  在此添加控件通知处理程序代码
	int iCameraIndex = 0;
	LE_STATUS eResult = eResult =LCC_C_CloseCamera(m_pCameraHandle[iCameraIndex]);
	if (eResult != L_OK)
	{
		AfxMessageBox(_T("关闭相机失败!"));
	}
	else
	{
		SetDlgItemText(IDC_EDIT4, _T("相机关闭"));
	}
}


void CMFCApplication2Dlg::OnBnClickedStartgrabBtn1()
{
	// TODO:  在此添加控件通知处理程序代码
	UpdateData();
	m_GrabImageCount = 0;
	int iCameraIndex = 0;
	
	if (m_pCameraHandle[iCameraIndex] != NULL)
	{
		/*停止采集*/
		LE_STATUS eResult = LCC_C_StartGrab(m_pCameraHandle[iCameraIndex]);

		if (eResult != L_OK)
		{
			AfxMessageBox(_T("开始采集失败!"));
		}
		else
		{
			SetDlgItemText(IDC_EDIT4, _T("相机采集"));
		}
	}
	UpdateData(false);
}


void CMFCApplication2Dlg::OnBnClickedStopgrabButton1()
{
	// TODO:  在此添加控件通知处理程序代码
	int iCameraIndex = 0;

	if (m_pCameraHandle[iCameraIndex] != NULL)
	{
		/*停止采集*/
		LE_STATUS eResult = LCC_C_StopGrab(m_pCameraHandle[iCameraIndex]);

		if (eResult != L_OK)
		{
			AfxMessageBox(_T("停止采集失败!"));
		}
		else
		{
			SetDlgItemText(IDC_EDIT4, _T("停止采集"));
		}
	}
}

void SaveCalBuffer(
	const char*   dataBuffer,
	int dataSize,
	bool  hasReflectance,
	const int64_t bufferWidth,
	const int64_t bufferHeight,
	const int64_t lowerBoundX,
	const int64_t lowerBoundR,
	const int64_t upperBoundX,
	const int64_t upperBoundR,
	const std::string& filePath)
{
	std::string datFilename = filePath + ".dat";
	std::string xmlFilename = filePath + ".xml";

	std::ofstream dataStream(datFilename.c_str(), std::ios::binary | std::ios::trunc | std::ios::out);
	dataStream.exceptions(std::ios::failbit | std::ios::badbit);
		
	dataStream.write((const char*)dataBuffer, dataSize);
	dataStream.close();

	DatXmlWriter xml;
	xml.open();
	{
		xml.addParameter("size", std::to_string(hasReflectance ? bufferWidth * (sizeof(float)* 2 + 1) : bufferWidth*sizeof(float)* 2));
		xml.addParameter("version", "1");
		xml.addParameter("layout", "SUBCOMPONENT");
		xml.openComponent("", "Ranger3Range");
		{
			xml.addParameter("size", std::to_string(hasReflectance ? bufferWidth * (sizeof(float)* 2 + 1) : bufferWidth*sizeof(float)* 2));
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
			xml.closeSubComponent();
			if (hasReflectance == true)
			{
				xml.openSubComponent("BYTE", "Intensity");
				{
					xml.addParameter("size", std::to_string(bufferWidth));
					xml.addParameter("width", std::to_string(bufferWidth));
				}
				xml.closeSubComponent();
			}
		}
		xml.closeComponent();
	}
	xml.close();

	std::ofstream xmlStream(xmlFilename.c_str(), std::ios::binary | std::ios::trunc | std::ios::out);
	xmlStream.exceptions(std::ios::failbit | std::ios::badbit);
	xmlStream << xml.toString();
	xmlStream.close();

}

void SaveBufferRaw(
	const char* rangeBuffer,
	int dataSize,
	bool  hasReflectance,
	const int64_t bufferWidth,
	const int64_t bufferHeight,
	const int r1Height,
	const int r1OffsetY,
	const int rangeAxisFlat,
	const std::string& filePath)
{
	std::string datFilename = filePath + ".dat";
	std::string xmlFilename = filePath + ".xml";

	std::ofstream dataStream(datFilename.c_str(), std::ios::binary | std::ios::trunc | std::ios::out);
	dataStream.exceptions(std::ios::failbit | std::ios::badbit);

	dataStream.write((const char*)rangeBuffer, dataSize);	
	dataStream.close();

	DatXmlWriter xml;
	xml.open();
	{
		xml.addParameter("size", std::to_string(hasReflectance ? bufferWidth * (sizeof(uint16_t)+1) : bufferWidth * sizeof(uint16_t)));
		xml.addParameter("version", "1");
		xml.addParameter("layout", "SUBCOMPONENT");
		xml.openComponent("Hi3D", "Ranger3Range");
		{
			xml.addParameter("size", std::to_string(hasReflectance ? bufferWidth * (sizeof(uint16_t)+1) : bufferWidth * sizeof(uint16_t)));
			xml.addParameter("height", "1");
			xml.openSensorRangeTraits();
			{
				// From where on the sensor the data comes from can be calculated by
				// using the origin and scale factors below

				xml.addParameter("fov x0", "0");
				xml.addParameter("fov x1", std::to_string(bufferWidth - 1));
				xml.addParameter("fov x2", "0");
				xml.addParameter("fov x3", std::to_string(bufferWidth - 1));
				xml.addParameter("fov z0", "1");
				xml.addParameter("fov z1", "1");
				xml.addParameter("fov z2", std::to_string(r1Height * 16 - 1));
				xml.addParameter("fov z3", std::to_string(r1Height * 16 - 1));
				xml.addParameter("origin x", "0");
				xml.addParameter("scale x", "1");
				xml.addParameter("origin z", std::to_string(r1OffsetY));
				xml.addParameter("scale z", std::to_string(1.0 / 16.0* rangeAxisFlat));
			}
			xml.closeSensorRangeTraits();
			xml.openSubComponent("WORD", "Range");
			{
				xml.addParameter("size", std::to_string(bufferWidth * sizeof(uint16_t)));
				xml.addParameter("width", std::to_string(bufferWidth));
			}
			xml.closeSubComponent();
			if (hasReflectance == true)
			{
				xml.openSubComponent("BYTE", "Intensity");
				{
					xml.addParameter("size", std::to_string(bufferWidth));
					xml.addParameter("width", std::to_string(bufferWidth));
				}
				xml.closeSubComponent();
			}
		}
		xml.closeComponent();
	}
	xml.close();

	std::ofstream xmlStream(xmlFilename.c_str(), std::ios::binary | std::ios::trunc | std::ios::out);
	xmlStream.exceptions(std::ios::failbit | std::ios::badbit);
	xmlStream << xml.toString();
	xmlStream.close();

}

void CMFCApplication2Dlg::OnBnClickedParamconfigfileBtn1()
{
	// TODO:  在此添加控件通知处理程序代码
	int iCameraIndex = 0;
	TCHAR szFilter[] = _T("文本文件(*.csv)|*.csv||");
	// 构造打开文件对话框   
	CFileDialog fileDlg(TRUE, _T("csv"), NULL, 0, szFilter, this);
	CString strFilePath;

	// 显示打开文件对话框   
	if (IDOK == fileDlg.DoModal())
	{
		strFilePath = fileDlg.GetPathName();
		std::string feautreConfigFile1 = wstring2string(strFilePath.GetBuffer(0));
		strFilePath.ReleaseBuffer();
		SetDlgItemText(IDC_EDIT1, strFilePath);
		//设置参数配置文件到相机
		LCC_C_SetFeatureString(m_pCameraHandle[iCameraIndex], LFNC_SICK_PARAMETPATH, feautreConfigFile1.c_str());
	}
}


void CMFCApplication2Dlg::OnBnClickedCalfileBtn1()
{
	// TODO:  在此添加控件通知处理程序代码
	int iCameraIndex = 0;
	TCHAR szFilter[] = _T("文本文件(*.xml)|*.xml||");
	// 构造打开文件对话框   
	CFileDialog fileDlg(TRUE, _T("xml"), NULL, 0, szFilter, this);
	CString strFilePath;

	// 显示打开文件对话框   
	if (IDOK == fileDlg.DoModal())
	{  
		strFilePath = fileDlg.GetPathName();
		std::string  calbritionFile1 = wstring2string(strFilePath.GetBuffer(0));
		strFilePath.ReleaseBuffer();
		SetDlgItemText(IDC_EDIT2, strFilePath);
		//设置标定文件到相机
		LCC_C_SetFeatureString(m_pCameraHandle[iCameraIndex], LFNC_SICK_CALIBRAPATH, calbritionFile1.c_str());
	}
}


void CMFCApplication2Dlg::ShowMatImgToWnd(CWnd* pictureWnd, cv::Mat _disimg)
{
	if (_disimg.empty())
		_disimg = cv::Mat::ones(480, 640, CV_8UC1) * 126;

	static BITMAPINFO *bitMapinfo = NULL;
	static bool First = TRUE;

	cv::Mat disimg = _disimg.clone();
	pIF.removeNAN(disimg);
	cv::normalize(disimg, disimg, 0.0, 255.0, cv::NORM_MINMAX);
	disimg.convertTo(disimg, CV_8UC1);

	if (First)
	{
		BYTE *bitBuffer = new BYTE[40 + 4 * 256];
		if (bitBuffer == NULL)
		{
			return;
		}
		First = FALSE;
		memset(bitBuffer, 0, 40 + 4 * 256);
		bitMapinfo = (BITMAPINFO *)bitBuffer;
		bitMapinfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bitMapinfo->bmiHeader.biPlanes = 1;
		for (int i = 0; i<256; i++)
		{
			bitMapinfo->bmiColors[i].rgbBlue = bitMapinfo->bmiColors[i].rgbGreen = bitMapinfo->bmiColors[i].rgbRed = (BYTE)i;
		}
	}

	bitMapinfo->bmiHeader.biHeight = -disimg.rows;
	bitMapinfo->bmiHeader.biWidth = disimg.cols;
	bitMapinfo->bmiHeader.biBitCount = disimg.channels() * 8;


	CRect drect;
	pictureWnd->GetClientRect(drect);
	CClientDC dc(pictureWnd);
	HDC hDC = dc.GetSafeHdc();
	SetStretchBltMode(hDC, COLORONCOLOR);
	StretchDIBits(hDC,
		0,
		0,
		drect.right,
		drect.bottom,
		0,
		0,
		disimg.cols,
		disimg.rows,
		disimg.data,
		bitMapinfo,
		DIB_RGB_COLORS,
		SRCCOPY);
}

void CMFCApplication2Dlg::OnBnClickedSavepathBtn1()
{
	// TODO:  在此添加控件通知处理程序代码
	TCHAR szPath[MAX_PATH];

	BROWSEINFO bi;
	bi.hwndOwner = m_hWnd;
	bi.pidlRoot = NULL;
	bi.pszDisplayName = szPath;
	bi.lpszTitle = _T("");
	bi.ulFlags = 0;
	bi.lpfn = NULL;
	bi.lParam = 0;
	bi.iImage = 0;

	LPITEMIDLIST lp = SHBrowseForFolder(&bi);

	if (lp && SHGetPathFromIDList(lp, szPath))
	{
		CString ss= szPath;
		SaveImagePath = wstring2string(ss.GetBuffer(0));	
		SetDlgItemText(IDC_EDIT3, szPath);
	}
}


void CMFCApplication2Dlg::OnBnClickedCheck1()
{
	// TODO:  在此添加控件通知处理程序代码
	IsSaveData = ((CButton*)GetDlgItem(IDC_CHECK1))->GetCheck();
}
