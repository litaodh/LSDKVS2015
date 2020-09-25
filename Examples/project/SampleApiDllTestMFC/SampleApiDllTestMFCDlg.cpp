/**	@file	SampleApiDllTestMFCDlg.cpp
*
*	@brief	Demo of showing how to call SICK 3D Camera APIs.
*
*	@attention
*	This Demo program is only used to show how to call SICK 3D Camera APIs.
*			SICK will not guarantee this file is correct!
*			This file is open to customers.
*			Any programs based on this demo, please keep it correct by yourself.
*
*	@copyright	Copyright 2016-2020 SICK AG. All rights reserved.
*	@author		Vision Lab, SICK GCN
*	@version	2.0.0.0
*	@date		2019.10.15
*
*/


#include "stdafx.h"
#include "SampleApiDllTestMFC.h"
#include "SampleApiDllTestMFCDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ----------------------------------------------------------------------------
// CAboutDlg function definitions


class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
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


// ----------------------------------------------------------------------------
// MFC_IMG function definitions


MFC_IMG::MFC_IMG()
{
	m_pImg = std::make_shared<SICK_DATA>();
	m_pCV = std::make_shared<InterfaceOpenCV>();
}

MFC_IMG::MFC_IMG(const SICK_DATA & _data, const CString _path=CString(""))
{
	m_pImg = std::make_shared<SICK_DATA>(_data);
	m_pCV = std::make_shared<InterfaceOpenCV>();

	CString name;
	if (_path.IsEmpty())
		name = CString("");
	else
		name = _path.Right(_path.GetLength() - _path.ReverseFind('\\') - 1);

	if (!m_pImg->isEmpty())
	{
		m_path = _path;
		m_name = name;

		// MRC_IMG unique variables:
		m_caliPath = CString("");
		m_savePath = CString("");
		m_aoiHeigh = CString("200");
		m_scaleZ   = CString("0.0625");
		m_originZ  = CString("500");
		m_RectMeth = CString("0 | Top");
		m_RectWidt = CString("2560");
		m_missData = CString("NAN");
		m_doRectfy = CString("1");
	}
}

CString MFC_IMG::get_size() const
{
	CString strSzie;
	if(m_pImg->has(SICK_DN::RAN))
		strSzie.Format(_T("Ran size: %d and %d"), m_pImg->getR_cols(), m_pImg->getR_rows());
	else if (m_pImg->has(SICK_DN::SEN))
		strSzie.Format(_T("\r\n\r\nSen size: %d and %d"), m_pImg->getS_cols(), m_pImg->getS_rows());
	else if (m_pImg->has(SICK_DN::RAN_CAL))
		strSzie.Format(_T("\r\n\r\nCal size: %d and %d"), m_pImg->getC_cols(), m_pImg->getC_rows());

	return strSzie;
}

cv::Mat MFC_IMG::getMat(SickCam::DataNames _dn) const
{
	return m_pCV->getMat(*(m_pImg.get()), _dn);
}

NAME_VEC MFC_IMG::get_NameList() const
{
	NAME_VEC vec;
	for (auto sub : m_pImg->getData())		vec.push_back(sub.first);
	return vec;
}

SICK_DATA MFC_IMG::get_SICK_DATA()
{
	return *m_pImg;
}


// ----------------------------------------------------------------------------
// CSampleApiDllTestMFCDlg function definitions


CSampleApiDllTestMFCDlg::CSampleApiDllTestMFCDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_SAMPLEAPIDLLTESTMFC_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSampleApiDllTestMFCDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_LOG, m_log);
	DDX_Control(pDX, IDC_COMBO_IMGTYPE, m_Combo_ImgTypes);
}

BEGIN_MESSAGE_MAP(CSampleApiDllTestMFCDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_Calibration, &CSampleApiDllTestMFCDlg::OnBnClickedButtonCalibration)
	ON_WM_DROPFILES()
	ON_BN_CLICKED(IDC_BUTTON_ViewerMode, &CSampleApiDllTestMFCDlg::OnBnClickedButtonViewermode)
	ON_WM_MOUSEMOVE()
	ON_BN_CLICKED(IDC_BUTTON_EzR, &CSampleApiDllTestMFCDlg::OnBnClickedButtonEzr)
	ON_NOTIFY(NM_RCLICK, IDC_TREE_LIST, &CSampleApiDllTestMFCDlg::OnNMRClickTreeList)
	ON_COMMAND(id_Scan, &CSampleApiDllTestMFCDlg::OnScanDevice)
	ON_COMMAND(id_Conn, &CSampleApiDllTestMFCDlg::OnConnectRanger3)
	ON_COMMAND(id_Para, &CSampleApiDllTestMFCDlg::OnParameters)
	ON_COMMAND(id_FoIP, &CSampleApiDllTestMFCDlg::OnForceIP)
	ON_COMMAND(id_Star, &CSampleApiDllTestMFCDlg::OnStartRanger3)
	ON_COMMAND(id_Stop, &CSampleApiDllTestMFCDlg::OnStopRanger3)
	ON_COMMAND(id_Disc, &CSampleApiDllTestMFCDlg::OnDisconnectRanger3)
	ON_COMMAND(id_Save2_OpenCV_XML, &CSampleApiDllTestMFCDlg::OnSave2OpenCV_XML)
	ON_COMMAND(id_Save2_PC_TXT, &CSampleApiDllTestMFCDlg::OnSave2PC_TXT)
	ON_COMMAND(id_Save2_SICK_DAT, &CSampleApiDllTestMFCDlg::OnSave2SICK_DAT)
	ON_COMMAND(id_dataImageSetting, &CSampleApiDllTestMFCDlg::OnDataImageSetting)
	ON_COMMAND(id_Dele2, &CSampleApiDllTestMFCDlg::OnDelete2)
	ON_COMMAND(id_Save3, &CSampleApiDllTestMFCDlg::OnSave3)
	ON_COMMAND(id_Dele3, &CSampleApiDllTestMFCDlg::OnDelete3)

ON_WM_TIMER()
ON_NOTIFY(NM_CLICK, IDC_TREE_LIST, &CSampleApiDllTestMFCDlg::OnNMClickTreeList)
ON_NOTIFY(NM_DBLCLK, IDC_TREE_LIST, &CSampleApiDllTestMFCDlg::OnNMDblclkTreeList)
ON_CBN_SELCHANGE(IDC_COMBO_IMGTYPE, &CSampleApiDllTestMFCDlg::OnCbnSelchangeComboImgtype)
ON_WM_CLOSE()
ON_BN_CLICKED(IDC_BUTTON_ClearLog, &CSampleApiDllTestMFCDlg::OnBnClickedButtonClearlog)
ON_BN_CLICKED(IDC_BUTTON_SelfTest, &CSampleApiDllTestMFCDlg::OnBnClickedButtonSelftest)
END_MESSAGE_MAP()


// CSampleApiDllTestMFCDlg message handlers

BOOL CSampleApiDllTestMFCDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
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

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	::DragAcceptFiles(m_hWnd, TRUE);
	ChangeWindowMessageFilter(WM_DROPFILES, MSGFLT_ADD);
	ChangeWindowMessageFilter(0x0049, MSGFLT_ADD);

	m_isShowRange = true;
	m_isShowSensor = false;

	CRect rect;
	GetDlgItem(IDC_TREE_LIST)->GetWindowRect(rect);
	rect.top -= 31; rect.bottom -= 30;
	rect.right -= 8; rect.left -= 8;
	m_ddTreeCtrl.Create(WS_CHILD | WS_VISIBLE | TVS_HASBUTTONS | TVS_SHOWSELALWAYS, rect, this, IDC_TREE_LIST);
	m_hItemRoot = m_ddTreeCtrl.InsertItem(CString("SICK"), 0, 0, NULL);
	m_ddTreeCtrl.SetItemData(m_hItemRoot, 0);
	m_camRoot = m_ddTreeCtrl.InsertItem(CString("Devices"), m_hItemRoot, TVI_LAST); 
	m_ddTreeCtrl.SetItemData(m_camRoot, 1);
	m_dataRoot = m_ddTreeCtrl.InsertItem(CString("DataImages"), m_hItemRoot, TVI_LAST);
	m_ddTreeCtrl.SetItemData(m_dataRoot, 2);
	m_sensorRoot = m_ddTreeCtrl.InsertItem(CString("SensorImages"), m_hItemRoot, TVI_LAST);
	m_ddTreeCtrl.SetItemData(m_sensorRoot, 3);
	m_ddTreeCtrl.ShowWindow(SW_SHOWNORMAL);
	_MyExpandTree(m_hItemRoot);
	m_CurrentSelItem = m_hItemRoot;
	m_CurrentSelItemText = "SICK";
	m_isShowDevice = false;
	m_IFCV = std::make_unique<InterfaceOpenCV>();

	_ComboImgTypesInsert({ SickCam::DN::INV});

	// --- Init GenICam environment for all ranger3.
	std::string logPath("");
	m_pR3S = std::make_shared<R3S>(logPath);
	if (!m_pR3S->isAvaliable())
	{
		m_Status = DlgStatus::NO_CTI;
		SetWindowTextW(CString("SampleApiDllTestMFCDlg - NO_CTI_FOUND!"));
		log(CString("NO_CTI_FOUND!"));
	}

	OnScanDevice();
	m_requireStopTimer = false;
	m_isStopTimer = true;

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CSampleApiDllTestMFCDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CSampleApiDllTestMFCDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CSampleApiDllTestMFCDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CSampleApiDllTestMFCDlg::OnBnClickedButtonCalibration()
{
	log(CString("Try to open Ranger3Setup, please wait ..."));
	system("RunCAL.bat");
	log(CString("Open Ranger3Setup done!"));
}

void CSampleApiDllTestMFCDlg::OnDropFiles(HDROP hDropInfo)
{
	int DropCount = DragQueryFile(hDropInfo, -1, NULL, 0);
	std::set<CString> fileNames;
	for(int i=0;i< DropCount;i++)	
	{		
		WCHAR wcStr[MAX_PATH];		
		DragQueryFile(hDropInfo,i,wcStr,MAX_PATH);	
		CString str = CString(wcStr);
		if(str.GetLength() > 4)
			fileNames.insert(str.Mid(0, str.GetLength() - 4));
	} 	
	DragFinish(hDropInfo); 

	if(fileNames.size() == 1)
		_LoadSICKImage(*fileNames.begin());
	else
	{
		for (auto sub : fileNames)
		{
			_LoadSICKImage(sub);
		}
	}
	
	_updateDataList();
	_updateSensorList();

	CDialogEx::OnDropFiles(hDropInfo);
}

void CSampleApiDllTestMFCDlg::saveOnlineImageToPath(const CString path)
{
	// get current online image from OnTimer() & save to path
	std::string spath = CT2A(path);

	std::stringstream ss;
	ss << "Save as SICK dat|xml < " << spath.c_str() << " >";

	if (!m_currentR3Image.isEmpty())
	{
		SickCam::ImgT img = m_currentR3Image.get_SICK_DATA();
		auto e = img.SaveToIconFile(spath);
		ss << ". \r\nExecution Code (0 is ok) = " << e;
	}
	else
		ss << ". \r\nFalid!! Current Online Image is Invalid!! Please Star Camera!";

	log(CString(ss.str().c_str()));

	_LoadSICKImage(path);
	_LoadSICKImage(path + CString("_SEN"));
	_LoadSICKImage(path + CString("_CAL"));


	auto H = m_CurrentSelItem;
	auto Hstr = m_CurrentSelItemText;
	
	_updateDataList();
	_updateSensorList();

	m_CurrentSelItem = H;
	m_CurrentSelItemText = Hstr;
	m_ddTreeCtrl.SetItemText(m_CurrentSelItem, CString("* ") + m_CurrentSelItemText);
}

CString& CSampleApiDllTestMFCDlg::CheckSuffixIsAvalible(CString& _path, const CString& _subffix)
{
	CString subffix(_subffix);
	if (_path.Mid(_path.GetLength() - 3, 3) != subffix.MakeLower() && _path.Mid(_path.GetLength() - 3, 3) != subffix.MakeUpper())
		_path += (CString(".") + subffix.MakeLower());
	return _path;
}

BOOL CSampleApiDllTestMFCDlg::_LoadSICKImage(const CString path)
{
	SICK_DATA img;
	if (img.loadFromIconFile(std::string(CT2A(path))))
	{
		auto im = MFC_IMG(img, path);
		if (im.has(SICK_DN::SEN))
		{
			m_path_SensorData.insert({ path, im });
			m_currentSensorPath = path;

			log(CString("Load new sensor image") +
				CString("\r\nimage name: ") + m_path_SensorData[m_currentSensorPath].get_name() +
				CString("\r\nLoad image path: ") + m_currentSensorPath +
				im.get_BasicInfo()
			);

			_showMatImgToWnd(GetDlgItem(IDC_STATIC_CAM1_RANGE), m_path_SensorData[m_currentSensorPath].getMat(SICK_DN::SEN));
		}
		else
		{
			m_path_ImgData.insert({ path,  im });
			m_currentImgPath = path;

			log(CString("Load new image") +
				CString("\r\nimage name: ") + m_path_ImgData[m_currentImgPath].get_name() +
				CString("\r\nLoad image path: ") + m_currentImgPath +
				im.get_BasicInfo()
			);

			_RefleshImageViever();
		}

		return TRUE;
	}

	return FALSE;
}

BOOL CSampleApiDllTestMFCDlg::_LoadOpenCV_XML(const CString _path, std::vector<SICK_DN> _names)
{
	if (_path.IsEmpty()) return FALSE;

	std::string path = CT2A(_path);
	if (_path.Mid(_path.GetLength() - 3, 3) != CString("xml") && _path.Mid(_path.GetLength() - 3, 3) != CString("XML"))
		path += ".xml";

	std::stringstream ss; ss << "Read from cv::FileStorage ( OpenCV ) < " << path << " >\r\n\r\n";
	log(CString("Start loading, please wait ..."));
	std::vector<SICK_DN> names;
	if (_names.empty())
	{
		log(CString(ss.str().c_str()) + CString("Input name list is empty! Using default name list to continue ...\r\n\r\n"));
		for (auto sub : ImgT::DN_StrName)
			names.push_back(sub.first);
	}

	// Load from file
	MFC_IMG img = MFC_IMG(m_IFCV->LoadImgTFromOpenCVXML(path, names), _path);
	if (img.isEmpty())
	{
		log(CString(ss.str().c_str()) + CString("Load MFC_IMG empty!\r\n\r\n"));
		return FALSE;
	}

	// show in GUI
	if (img.has(SICK_DN::SEN))
	{
		m_currentSensorPath = _path;
		m_path_SensorData.insert({ m_currentSensorPath, img });
		_showMatImgToWnd(GetDlgItem(IDC_STATIC_CAM1_RANGE), m_path_SensorData[m_currentSensorPath].getMat(SICK_DN::SEN));
	}
	else
	{
		m_currentImgPath = _path;
		m_path_ImgData.insert({ m_currentImgPath, img});
		_RefleshImageViever();
	}

	ss << "\r\n\r\nOK!";
	log(CString(ss.str().c_str()));

	return TRUE;
}

void CSampleApiDllTestMFCDlg::_showMatImgToWnd(CWnd* pictureWnd, cv::Mat _disimg)
{
	if (_disimg.empty())
		_disimg = cv::Mat::ones(480, 640, CV_8UC1)*126;

	static BITMAPINFO *bitMapinfo = NULL;
	static bool First = TRUE;

	cv::Mat disimg = _disimg.clone();
	m_IFCV->removeNAN(disimg);
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

void CSampleApiDllTestMFCDlg::_updateDeviceList()
{
	// delete all children
	HTREEITEM hNextItem = m_ddTreeCtrl.GetChildItem(m_camRoot);
	while (hNextItem != NULL)
	{
		HTREEITEM h2 = hNextItem;
		hNextItem = m_ddTreeCtrl.GetNextItem(hNextItem, TVGN_NEXT);
		m_ddTreeCtrl.DeleteItem(h2);
	}

	if (!m_ddTreeCtrl.ItemHasChildren(m_camRoot))
	{
		// update current
		m_CurrentSelItem = m_camRoot;
		m_CurrentSelItemText = CString("Devices");


		// renew
		size_t i = 1000;
		m_ID_R3Name.clear();
		for (auto sub : m_Name_Ranger3s)
		{
			HTREEITEM h = m_ddTreeCtrl.InsertItem(CString(sub.second->m_gCamStatus.c_str()) + sub.first, m_camRoot, TVI_LAST);
			m_ddTreeCtrl.SetItemData(h, i);
			m_ID_R3Name.insert({ i, sub.first });
			++i;

			if (sub.first.IsEmpty())
				AfxMessageBox(_T("Big Problem! m_ID_R3Name have Empty item!"));

		}

		_MyExpandTree(m_camRoot);
	}

}

void CSampleApiDllTestMFCDlg::_updateDataList()
{
	m_ddTreeCtrl.DeleteItem(m_dataRoot);
	m_dataRoot = m_ddTreeCtrl.InsertItem(CString("DataImages"), m_hItemRoot, TVI_LAST);
	m_ddTreeCtrl.SetItemData(m_dataRoot, 2);

	size_t i = 2000;
	m_ID_ImgPaths.clear();
	for (auto sub : m_path_ImgData)
	{
		HTREEITEM h = m_ddTreeCtrl.InsertItem(sub.second.get_name(), m_dataRoot, TVI_LAST);
		m_ddTreeCtrl.SetItemData(h, i);
		m_ID_ImgPaths.insert({ i, sub.second.get_path() });
		++i;

		if (sub.first.IsEmpty())
			AfxMessageBox(_T("Big Problem! m_path_ImgData have Empty item!"));
	}

	// update current
	m_CurrentSelItem = m_dataRoot;
	m_CurrentSelItemText = CString("DataImages");
	m_currentImgPath = CString("DataImages");

	_MyExpandTree(m_dataRoot);
}

void CSampleApiDllTestMFCDlg::_updateSensorList()
{
	m_ddTreeCtrl.DeleteItem(m_sensorRoot);
	m_sensorRoot = m_ddTreeCtrl.InsertItem(CString("SensorImages"), m_hItemRoot, TVI_LAST);
	m_ddTreeCtrl.SetItemData(m_sensorRoot, 3);

	size_t i = 3000;
	m_ID_SensorPath.clear();
	for (auto sub : m_path_SensorData)
	{
		HTREEITEM h = m_ddTreeCtrl.InsertItem(sub.second.get_name(), m_sensorRoot, TVI_LAST);
		m_ddTreeCtrl.SetItemData(h, i);
		m_ID_SensorPath.insert({ i, sub.second.get_path() });
		++i;
		if (sub.first.IsEmpty())
			AfxMessageBox(_T("Big Problem! m_path_SensorData have Empty item!"));
	}

	// update current
	m_CurrentSelItem = m_sensorRoot;
	m_CurrentSelItemText = CString("SensorImages");
	m_currentImgPath = CString("SensorImages");

	_MyExpandTree(m_sensorRoot);
}

void CSampleApiDllTestMFCDlg::_MyExpandTree(HTREEITEM hTreeItem)
{
	if (!m_ddTreeCtrl.ItemHasChildren(hTreeItem))
	{       
		return;    
	}    
	HTREEITEM hNextItem = m_ddTreeCtrl.GetChildItem(hTreeItem);
	while (hNextItem != NULL)
	{        
		_MyExpandTree(hNextItem);        
		hNextItem = m_ddTreeCtrl.GetNextItem(hNextItem, TVGN_NEXT);   
	}    
	m_ddTreeCtrl.Expand(hTreeItem,TVE_EXPAND);
}

void CSampleApiDllTestMFCDlg::_ComboImgTypesInsert(NAME_VEC _nd)
{
	if (_nd.empty()) return;

	for (auto sub : _nd)
	{
		int id = static_cast<int>(m_currentR3AllType.size());
		m_Combo_ImgTypes.InsertString(id, CString(m_currentR3ImgT.DN2Str(sub).c_str()));
		m_currentR3AllType.insert({ id, sub });
	}
	_ComboImgTypesFocus(_nd[0]);
}

void CSampleApiDllTestMFCDlg::_ComboImgTypesDelete(NAME_VEC _nd)
{
	if (_nd.empty())
	{		
		m_Combo_ImgTypes.ResetContent();
		m_currentR3AllType.swap(std::map<int, SickCam::DN>());
		return;
	}

	for (auto sub : _nd)
	{
		int id = m_Combo_ImgTypes.FindString(0, CString(m_currentR3ImgT.DN2Str(sub).c_str()));
		m_Combo_ImgTypes.DeleteString(id);
		auto it = m_currentR3AllType.find(id);
		m_currentR3AllType.erase(it);
	}
}

void CSampleApiDllTestMFCDlg::_ComboImgTypesUpdate(NAME_VEC _nd)
{
	_ComboImgTypesDelete();
	_ComboImgTypesInsert(_nd);
}

void CSampleApiDllTestMFCDlg::_ComboImgTypesFocus(SICK_DN _nd)
{
	int id = m_Combo_ImgTypes.FindString(0, CString(m_currentR3ImgT.DN2Str(_nd).c_str()));
	if (0 <= id && id <= 20)
	{
		m_Combo_ImgTypes.SetCurSel(id);
		m_currentR3ShowType = _nd;
	}
}

bool CSampleApiDllTestMFCDlg::_ComboImgTypesNeedUpdate(NAME_VEC _nd)
{
	NAME_VEC newNames(_nd);
	NAME_VEC oldNames;
	for (auto it : m_currentR3AllType)
	{
		oldNames.push_back(it.second);
	}

	std::sort(newNames.begin(), newNames.end(), [&](SICK_DN A, SICK_DN B) { return A < B; });
	std::sort(oldNames.begin(), oldNames.end(), [&](SICK_DN A, SICK_DN B) { return A < B; });

	return newNames != oldNames;
}

void CSampleApiDllTestMFCDlg::_RefleshImageViever()
{
	if (m_path_ImgData.count(m_currentImgPath) == 1)
	{
		auto img = m_path_ImgData[m_currentImgPath];
		SICK_DN dn;

		if (img.has(SICK_DN::RAN))
			dn = m_isShowRange ? SICK_DN::RAN : SICK_DN::REF;
		else
		{
			dn = m_isShowRange ? 
				SICK_DN::RAN_CAL : 
				img.has(SICK_DN::REF_CAL) ? 
					SICK_DN::REF_CAL : 
					img.has(SICK_DN::REF) ? SICK_DN::REF : SICK_DN::INV;
		}

		_showMatImgToWnd(GetDlgItem(IDC_STATIC_CAM1_RANGE), img.getMat(dn));
	}
}



void CSampleApiDllTestMFCDlg::log(CString str)
{
	CString strOld;
	GetDlgItemTextW(IDC_EDIT_LOG, strOld);

	CTime tm; tm = CTime::GetCurrentTime();
	CString strTime = tm.Format("%Y-%m-%d- %X");

	CString strAll = strOld + "--------" + strTime + "---------\r\n" + str + "\r\n\r\n\r\n";
	SetDlgItemTextW(IDC_EDIT_LOG, strAll);

	m_log.LineScroll(m_log.GetLineCount());
}

void CSampleApiDllTestMFCDlg::OnCalibrateImage(CString pathXML, CString pathSave,
	const int aoiHeight,	const float scaleZ,	const float originZ,	const RectMethod method,
	const size_t rectificationWidth,	float missingDataValue,	const bool _doRectify, CString currentItemName)
{
	if (currentItemName != CString("") && currentItemName != m_currentImgPath )
	{
		CString slog = CString("Error! \r\n\r\nImage Data \r\n") + currentItemName + CString("\r\n does not be selected.\r\n\r\nPlease re-select an image!");
		log(slog);
		AfxMessageBox(slog);
		
		return;
	}

	if (m_path_ImgData.count(m_currentImgPath) == 0)
	{
		log(CString("Current Selected Image Invalid!"));
		return;
	}

	if (pathXML.IsEmpty())
	{
		log(CString("Calibration File is Empty!"));
		return;
	}

	if (pathSave.IsEmpty())
	{
		log(CString("Save Path of Calibrated Image is Empty!"));
		return;
	}

	MFC_IMG img = m_path_ImgData[m_currentImgPath];

	// init calibration ptr
	UPTR<SiCaliWrapper::CalibrationWrapper> pCalibrationWrapper = std::make_unique<SiCaliWrapper::CalibrationWrapper>();
	if (false == pCalibrationWrapper->initCalibrationFilter(
		std::string(CT2A(pathXML)), img.getR_cols(), img.getR_rows(),
		aoiHeight, scaleZ, originZ, method, rectificationWidth,
		missingDataValue, _doRectify))
	{
		log(CString("initCalibrationFilter() Failed!"));
		return;
	}

	// convert data format
	size_t sizeRange(img.getR_rows()* img.getR_cols() * sizeof(uint16_t));

	uint8_t* p1 = new uint8_t[sizeRange];
	uint8_t* p2 = nullptr;

	if (img.has(SICK_DN::RAN))
		memcpy(p1, (uint8_t*)img.get_SICK_DATA().getData(SICK_DN::RAN), sizeRange);
	else
	{
		log(CString("SICK_DN::RAN is empty! Return error!"));
		return;
	}

	if (img.has(SICK_DN::REF))
	{
		size_t sizeRefle(img.getR_rows()* img.getR_cols() * sizeof(uint8_t));
		p2 = new uint8_t[sizeRefle];
		memcpy(p2, (uint8_t*)img.get_SICK_DATA().getData(SICK_DN::REF), sizeRefle);
	}
	else
	{
		log(CString("SICK_DN::REF is empty! Continue with range only ..."));
	}

	// calibrate
	pCalibrationWrapper->calibrate(p1, p2);
	auto cpData = pCalibrationWrapper->getData();

	// set current
	SICK_DATA timgT = img.get_SICK_DATA();
	size_t pixelSize = img.getR_cols() * img.getR_rows();

	timgT.setCaliInfo(
		img.getR_cols(), img.getR_rows(),
		cpData->offsetX,0.0, cpData->scaleX, img.get_scaleY(),
		cpData->lower_bound_x, cpData->upper_bound_x, cpData->lower_bound_r, cpData->upper_bound_r);
	
	timgT.insertDataCopy(SICK_DN::RAN_CAL, (uint8_t*)cpData->getData(PN::RAN_C), 0);
	timgT.insertDataCopy(SICK_DN::REF_CAL, (uint8_t*)cpData->getData(PN::REF_C), 0);

	MFC_IMG timg(timgT, pathSave);

	delete[] p1;
	delete[] p2;

	_SaveAsSICKDAT(timgT, pathSave, SICK_DN::RAN_CAL);
	_LoadSICKImage(pathSave);

	_updateDataList();
	_updateSensorList();
}

BOOL CSampleApiDllTestMFCDlg::_SaveAsOpenCVXML(CString _path, MFC_IMG _imgs)
{
	if (_path.IsEmpty() || _imgs.isEmpty()) return FALSE;

	std::string path = CT2A(CheckSuffixIsAvalible(_path, CString("xml")));
	//if (_path.Mid(_path.GetLength() - 3, 3) != CString("xml") && _path.Mid(_path.GetLength() - 3, 3) != CString("XML"))
	//	path += ".xml";

	std::stringstream ss; ss << "Save as cv::FileStorage ( OpenCV ) < " << path << " >\r\n\r\n";
	if (m_IFCV->SaveImgT2OpenCVXML(_imgs.get_SICK_DATA(), path))
	{
		ss << "OK!";	log(CString(ss.str().c_str()));
		return TRUE;
	}
	ss << "Failed!";	log(CString(ss.str().c_str()));
	return FALSE;
}

BOOL CSampleApiDllTestMFCDlg::_SaveAsSICKDAT(MFC_IMG mfc, CString _path, SICK_DN _nd)
{
	if (_path.IsEmpty()) return FALSE;

	std::string path = CT2A(_path);
	std::stringstream ss2;	ss2 << "Save as SICK dat|xml < " << path << " >\r\n\r\nNot finish, please wait ...\r\n\r\n ";
	log(CString(ss2.str().c_str()));

	std::stringstream ss; ss << "OK!";

	//MFC_IMG mfc;
	if(mfc.isEmpty())
	{
		ss << ". Invalid image";
		log(CString(ss.str().c_str()) + CString("\r\n"));
		return FALSE;
	}

	SickCam::ImgT img = mfc.get_SICK_DATA();
	if (img.SaveToIconFile(path, _nd))
	{
		ss << "OK!";	log(CString(ss.str().c_str()));
		return TRUE;
	}
	ss << "Failed!";	log(CString(ss.str().c_str()));
	return FALSE;

}



void CSampleApiDllTestMFCDlg::OnBnClickedButtonViewermode()
{
	m_isShowRange = !m_isShowRange;
	GetDlgItem(IDC_BUTTON_ViewerMode)->SetWindowTextW(m_isShowRange ? CString("Show Range now | Click to show Reflectance") : CString("Show Reflectance now | Click to show Range"));
	log(CString("Change Viewer Mode: ") + (m_isShowRange ? CString("Range") : CString("Reflectance")));

	if (m_isShowSensor && !m_isShowDevice)
	{
		if (m_path_SensorData.count(m_currentSensorPath) == 1)
			_showMatImgToWnd(GetDlgItem(IDC_STATIC_CAM1_RANGE), m_path_SensorData[m_currentSensorPath].getMat(SICK_DN::SEN));
	}
	else 
	{
		_RefleshImageViever();
	}
}

void CSampleApiDllTestMFCDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_isShowDevice || m_isShowSensor) return;

	ClientToScreen(&point);
	CRect rect;
	GetDlgItem(IDC_STATIC_CAM1_RANGE)->GetClientRect(rect);
	GetDlgItem(IDC_STATIC_CAM1_RANGE)->ClientToScreen(rect);
	if (rect.PtInRect(point) && !m_currentImgPath.IsEmpty())
	{
		CPoint pt;
		pt.x = point.x - rect.left;
		pt.y = point.y - rect.top;

		if (m_path_ImgData.count(m_currentImgPath) == 1)
		{
			MFC_IMG img = m_path_ImgData[m_currentImgPath];
			size_t iw(0), ih(0);
			if (img.has(SICK_DN::SEN))
			{
				iw = img.getS_cols();
				ih = img.getS_rows();
			}
			else if (img.has(SICK_DN::RAN))
			{
				iw = img.getR_cols();
				ih = img.getR_rows();
			}
			else if (img.has(SICK_DN::RAN_CAL))
			{
				iw = img.getC_cols();
				ih = img.getC_rows();
			}
			else
				return;

			int rw = rect.Width();
			int rh = rect.Height();

			CString str;
			if (img.has(SICK_DN::RAN))
			{
				int x(pt.x*iw / rw), y(pt.y*ih / rh);
				{
					CString substr;
					substr.Format(
						_T("(px = %d, py = %d, pz = %d)"),
						x, y, img.getMat(SICK_DN::RAN).at<uint16_t>(y, x));
					str += substr;
				}
				if (img.has(SICK_DN::REF))
				{
					int x(pt.x*iw / rw), y(pt.y*ih / rh);
					CString substr;
					substr.Format(
						_T(", Reflectance = %d, Mark = Not Set"),
						img.getMat(SICK_DN::REF).at<uint8_t>(y, x));
					str += substr;
				}
			}
			else if (img.has(SICK_DN::RAN_CAL))
			{
				int x(pt.x*iw / rw), y(pt.y*ih / rh);
				{
					CString substr;
					substr.Format(
						_T("(px = %d, py = %d) = (x = %f, y = %f, z = %f)"),
						x, y,
						x * img.get_scaleX() + img.get_offsetX(),
						y * img.get_scaleY(),
						img.getMat(SICK_DN::RAN_CAL).at<float>(y, x));
					str += substr;
				}
				if (img.has(SICK_DN::REF_CAL))
				{
					int x(pt.x*iw / rw), y(pt.y*ih / rh);
					CString substr;
					substr.Format(
						_T(", Reflectance = %d, Mark = Not Set"),
						img.getMat(SICK_DN::REF_CAL).at<uint8_t>(y, x));
					str += substr;
				}
			}

			SetDlgItemTextW(IDC_EDIT_POSITION, str);

		}

	}

	CDialogEx::OnMouseMove(nFlags, point);
}

void CSampleApiDllTestMFCDlg::OnBnClickedButtonEzr()
{
	log(CString("Try to open EasyRanger, please wait ..."));
	system("RunEZR.bat");
	log(CString("Open EasyRanger done!"));
}

void CSampleApiDllTestMFCDlg::OnNMRClickTreeList(NMHDR *pNMHDR, LRESULT *pResult)
{
	CRect rect;
	m_ddTreeCtrl.GetWindowRect(&rect);
	CPoint p;
	GetCursorPos(&p);
	
	if (rect.PtInRect(p))
	{
		CMenu menu;
		menu.CreatePopupMenu();

		// device
		size_t curSelTab = static_cast<size_t>(m_ddTreeCtrl.GetItemData(m_ddTreeCtrl.GetSelectedItem()));
		if (1 == curSelTab)
		{
			menu.AppendMenu(MF_STRING, id_Scan, _T("Scan Device"));
		}
		else if (1000 <= curSelTab && curSelTab < 2000)
		{
			menu.AppendMenu(MF_STRING, id_Para, _T("Parameter"));
			menu.AppendMenu(MF_STRING, id_FoIP, _T("ForceIP"));
			menu.AppendMenu(MF_STRING, id_Conn, _T("Connect"));
			menu.AppendMenu(MF_STRING, id_Star, _T("Start"));
			menu.AppendMenu(MF_STRING, id_Stop, _T("Stop"));
			menu.AppendMenu(MF_STRING, id_Disc, _T("Disconnet"));
		}

		// data
		else if (2 == curSelTab)
		{
		}
		else if (2000 <= curSelTab && curSelTab < 3000)
		{
			menu.AppendMenu(MF_STRING, id_Save2_OpenCV_XML, _T("Save as OpenCV xml"));
			menu.AppendMenu(MF_STRING, id_Save2_PC_TXT, _T("Save as Point Cloud txt"));
			menu.AppendMenu(MF_STRING, id_Save2_SICK_DAT, _T("Save as SICK dat|xml"));
			menu.AppendMenu(MF_STRING, id_dataImageSetting, _T("Settings"));
			menu.AppendMenu(MF_STRING, id_Dele2, _T("Delete"));
		}

		// sensor
		else if (3 == curSelTab)
		{
		}
		else if (3000 <= curSelTab && curSelTab < 4000)
		{
			menu.AppendMenu(MF_STRING, id_Save3, _T("Save to"));
			menu.AppendMenu(MF_STRING, id_Dele3, _T("Delete"));
		}
		else
		{
			menu.AppendMenu(MF_STRING, id_Conn, _T("Undefined"));
		}
		
		menu.TrackPopupMenu(TPM_LEFTALIGN, p.x, p.y, this);

	}

	*pResult = 0;
}


// ----------------------------------------------------------------------------
// Ranger3 Operation


void CSampleApiDllTestMFCDlg::OnScanDevice()
{
	// Disconnect and clear all if not empty;
	if (!m_Name_Ranger3s.empty())
	{
		for (auto sub : m_Name_Ranger3s)
		{
			if (sub.second->isStartAcquition())
			{
				sub.second->requireStopAcquition();
				while (sub.second->isStartAcquition());
			}

			if (sub.second->isConnected())
			{
				sub.second->requireDisconnect();
				while (sub.second->isConnected());
			}
		}

		m_Name_Ranger3s.clear(); m_Name_R3PathSaveTo.clear();
	}

	// --- check 
	auto e = m_pR3S->scanDevice();
	std::stringstream ss;
	ss << "Scan device done! Execution Code (0 is ok) = " << e;

	auto devices = m_pR3S->getConDevList();
	if (devices.empty())
	{
		m_Status = DlgStatus::NO_CAM;
		SetWindowTextW(CString("SampleApiDllTestMFCDlg - NO_CAMERA_CONNECTED!"));
		log(CString("NO_CAMERA_CONNECTED!"));

		for (auto sub : m_Name_Ranger3s)
		{
			sub.second->requireDisconnect();
			while (!sub.second->isDisconnected());
			sub.second->clearData();
		}

		m_Name_Ranger3s.clear();
		m_Name_Thread.clear();
		m_Name_Parameters.clear();
		m_Name_Parameters_WinsowStatus.clear();
		m_ID_R3Name.clear(); 
		m_currentR3AllType.clear();
	}

	// init ranger3 thread.
	for (auto sub : devices)
	{
		auto pIG = std::make_shared<ImageGabber>(m_pR3S, sub.second->getIp(), true);
		if (!sub.first.empty())
		{
			m_Name_Ranger3s.insert({ CString(sub.first.c_str()),  pIG }); m_Name_R3PathSaveTo.insert({ CString(sub.first.c_str()),  CString("")});
			ss << "\r\n\r\nFind: " << sub.first.c_str() << "\r\nIP: " << sub.second->getIp().c_str();
		}
	}

	log(CString(ss.str().c_str()));
	_updateDeviceList();
}

void CSampleApiDllTestMFCDlg::OnParameters()
{
	// which one
	size_t curSelTab = static_cast<size_t>(m_ddTreeCtrl.GetItemData(m_CurrentSelItem));
	if (m_ID_R3Name.count(curSelTab) == 1)
	{
		if (m_Name_Ranger3s.count(m_ID_R3Name[curSelTab]) == 1)
		{
			if (!m_Name_Ranger3s[m_ID_R3Name[curSelTab]]->isConnected() && !m_Name_Ranger3s[m_ID_R3Name[curSelTab]]->isStartAcquition())
			{
				OnConnectRanger3();
			}
			else if (m_Name_Ranger3s[m_ID_R3Name[curSelTab]]->isConnected() && m_Name_Ranger3s[m_ID_R3Name[curSelTab]]->isStartAcquition())
			{
				OnStopRanger3();
			}
		}
		else
			return;


		// Delete a closed window, new one.
		if (m_Name_Parameters.count(m_ID_R3Name[curSelTab]) == 1 && !m_Name_Parameters_WinsowStatus[m_ID_R3Name[curSelTab]])
		{
			m_Name_Parameters.erase(m_ID_R3Name[curSelTab]);
			m_Name_Parameters_WinsowStatus.erase(m_ID_R3Name[curSelTab]);

			ParametersDlg* p = new ParametersDlg(this);
			p->Create(IDD_DIALOG_PARAMETER);
			p->InitParameter(m_Name_Ranger3s[m_ID_R3Name[curSelTab]]->getR3().get(),
				m_Name_Ranger3s[m_ID_R3Name[curSelTab]].get());
			m_Name_Parameters.insert({ m_ID_R3Name[curSelTab], p });
			m_Name_Parameters_WinsowStatus.insert({ m_ID_R3Name[curSelTab], true });
			m_Name_Parameters[m_ID_R3Name[curSelTab]]->SetWindowTextW(CString("Parameters -- ") + m_ID_R3Name[curSelTab]);
			m_Name_Parameters[m_ID_R3Name[curSelTab]]->ShowWindow(SW_SHOW);

			log(CString("open parameter window of ") + m_ID_R3Name[curSelTab]);
		}
		else if (m_Name_Parameters.count(m_ID_R3Name[curSelTab]) == 0)
		{
			// add a new window
			ParametersDlg* p = new ParametersDlg(this);
			p->Create(IDD_DIALOG_PARAMETER);
			p->InitParameter(m_Name_Ranger3s[m_ID_R3Name[curSelTab]]->getR3().get(),
				m_Name_Ranger3s[m_ID_R3Name[curSelTab]].get());
			m_Name_Parameters.insert({ m_ID_R3Name[curSelTab], p });
			m_Name_Parameters_WinsowStatus.insert({ m_ID_R3Name[curSelTab], true });
			m_Name_Parameters[m_ID_R3Name[curSelTab]]->SetWindowTextW(CString("Parameters -- ") + m_ID_R3Name[curSelTab]);
			m_Name_Parameters[m_ID_R3Name[curSelTab]]->ShowWindow(SW_SHOW);

			log(CString("open parameter window of ") + m_ID_R3Name[curSelTab]);
		}
		else if (m_Name_Parameters.count(m_ID_R3Name[curSelTab]) == 1 && m_Name_Parameters_WinsowStatus[m_ID_R3Name[curSelTab]])
		{
			log(CString("Already opened! Parameter window of ") + m_ID_R3Name[curSelTab]);
		}


	}

}

void CSampleApiDllTestMFCDlg::OnForceIP()
{
	// which one
	size_t curSelTab = static_cast<size_t>(m_ddTreeCtrl.GetItemData(m_CurrentSelItem));
	if (m_ID_R3Name.count(curSelTab) == 1)
	{
		OnStopRanger3();
		OnDisconnectRanger3();

		// Delete a closed window, new one.
		if (m_Name_Parameters.count(m_ID_R3Name[curSelTab]) == 1 && !m_Name_Parameters_WinsowStatus[m_ID_R3Name[curSelTab]])
		{
			m_Name_Parameters.erase(m_ID_R3Name[curSelTab]);
			m_Name_Parameters_WinsowStatus.erase(m_ID_R3Name[curSelTab]);

			ParametersDlg* p = new ParametersDlg(this);
			p->Create(IDD_DIALOG_PARAMETER);
			p->InitForceIP(m_Name_Ranger3s[m_ID_R3Name[curSelTab]]->getR3().get(),
				m_Name_Ranger3s[m_ID_R3Name[curSelTab]].get());
			m_Name_Parameters.insert({ m_ID_R3Name[curSelTab], p });
			m_Name_Parameters_WinsowStatus.insert({ m_ID_R3Name[curSelTab], true });
			m_Name_Parameters[m_ID_R3Name[curSelTab]]->SetWindowTextW(CString("Force IP -- ") + m_ID_R3Name[curSelTab]);
			m_Name_Parameters[m_ID_R3Name[curSelTab]]->ShowWindow(SW_SHOW);

			log(CString("open Force IP window of ") + m_ID_R3Name[curSelTab]);
		}
		else if (m_Name_Parameters.count(m_ID_R3Name[curSelTab]) == 0)
		{
			// add a new window
			ParametersDlg* p = new ParametersDlg(this);
			p->Create(IDD_DIALOG_PARAMETER);
			p->InitForceIP(m_Name_Ranger3s[m_ID_R3Name[curSelTab]]->getR3().get(),
				m_Name_Ranger3s[m_ID_R3Name[curSelTab]].get());
			m_Name_Parameters.insert({ m_ID_R3Name[curSelTab], p });
			m_Name_Parameters_WinsowStatus.insert({ m_ID_R3Name[curSelTab], true });
			m_Name_Parameters[m_ID_R3Name[curSelTab]]->SetWindowTextW(CString("Force IP -- ") + m_ID_R3Name[curSelTab]);
			m_Name_Parameters[m_ID_R3Name[curSelTab]]->ShowWindow(SW_SHOW);

			log(CString("open Force IP window of ") + m_ID_R3Name[curSelTab]);
		}
		else if (m_Name_Parameters.count(m_ID_R3Name[curSelTab]) == 1 && m_Name_Parameters_WinsowStatus[m_ID_R3Name[curSelTab]])
		{
			log(CString("Already opened! Force IP window of ") + m_ID_R3Name[curSelTab]);
		}

	}
}

void CSampleApiDllTestMFCDlg::OnConnectRanger3()
{
	// which one
	size_t curSelTab = static_cast<size_t>(m_ddTreeCtrl.GetItemData(m_CurrentSelItem));
	if ((1000 <= curSelTab && curSelTab < 2000) | m_isTest) // device items: 1000-2000
	{
		if(m_ID_R3Name.count(curSelTab) == 1)
			OnConnectRanger3(m_ID_R3Name[curSelTab]);
	}
}

void CSampleApiDllTestMFCDlg::OnConnectRanger3(CString R3Name)
{
	if (m_Name_Ranger3s.count(R3Name) == 1)
	{
		auto p = m_Name_Ranger3s[R3Name];
		if (p->isConnected())
		{
			log(CString("** Warning ** ") + CString("\r\nDevice already connected!"));
			return;
		}

		bool ok = p->connect();
		ok = ok & p->isConnected();

		if (ok)
		{
			p->getR3()->setTimeOut(1000);

			std::thread* thread_pImgGabber = new std::thread(&ImageGabber::run, p);
			thread_pImgGabber->detach();

			if (m_Name_Thread.count(R3Name) == 1)
				m_Name_Thread.erase(R3Name);

			m_Name_Thread.insert({ R3Name, thread_pImgGabber });

			// update status to item text
			CString itemName = CString("* ") + CString(p->m_gCamStatus.c_str()) + m_CurrentSelItemText;
			m_ddTreeCtrl.SetItemText(m_CurrentSelItem, itemName);

		}

		CString strlog;
		strlog.Format(_T("\r\nMissing Data: %f\r\nTimeOut: %d"), p->getR3()->getMissingData(), p->getR3()->getTimeOut());

		log((!ok ? CString("## Failed ## ") : CString("")) +
			CString("\r\nconnect: ") + R3Name +
			CString("\r\nIP: ") + (ok ? CString(p->getR3()->getIp().c_str()) : CString("")) +
			CString("\r\nMac: ") + (ok ? CString(p->getR3()->getMac().c_str()) : CString("")) +
			CString("\r\nPatameter Path:") + CString(p->getR3()->getParametPath().c_str()) +
			CString("\r\nCalibration Path: ") + CString(p->getR3()->getCalibraPath().c_str()) +
			CString("\r\nRectification Method: ") + (p->getR3()->getRectMethos() == SiCaliWrapper::RectMethod::Top ? CString("Top") : (p->getR3()->getRectMethos() == SiCaliWrapper::RectMethod::Mean ? CString("Mean") : CString("Bottom"))) +
			strlog
		);

	}
}

void CSampleApiDllTestMFCDlg::OnStartRanger3()
{
	// which one
	size_t curSelTab = static_cast<size_t>(m_ddTreeCtrl.GetItemData(m_CurrentSelItem));
	if ((1000 <= curSelTab && curSelTab < 2000) | m_isTest) // device items: 1000-2000
	{
		if (m_Name_Ranger3s.count(m_ID_R3Name[curSelTab]) == 1)
		{
			auto p = m_Name_Ranger3s[m_ID_R3Name[curSelTab]];
			
			if (!p->isConnected())
			{
				log(CString("** Warning ** ") +	CString("\r\nDevice is disconnected! Please click connect first!"));
				AfxMessageBox(CString("** Warning ** ") + CString("\r\nDevice is disconnected! Please click connect first!"));
				return;
			}

			if (p->isStartAcquition())
			{
				log(CString("** Warning ** ") + CString("\r\nDevice already started! "));
				return;
			}

			if (p->isConnected() && !p->isStartAcquition())
			{
				p->clearData();
				p->requireStartAcquition();
				while (!p->isStartAcquition()) Sleep(10);
				bool ok = p->isStartAcquition();

				if (ok)
				{
					// update status to item text
					CString itemName = CString("* ") + CString(p->m_gCamStatus.c_str()) + m_CurrentSelItemText;
					m_ddTreeCtrl.SetItemText(m_CurrentSelItem, itemName);
				}
				

				CString strlog;
				strlog.Format(_T("\r\nMissing Data: %f\r\nTimeOut: %d" ), p->getR3()->getMissingData(), p->getR3()->getTimeOut());

				log((!ok ? CString("## Failed ## ") : CString("")) +
					CString("\r\nstart: ") + m_ID_R3Name[curSelTab] +
					CString("\r\nIP: ") + CString(p->getR3()->getIp().c_str()) +
					CString("\r\nMac: ") + CString(p->getR3()->getMac().c_str()) +
					CString("\r\nPatameter Path:") + CString(p->getR3()->getParametPath().c_str()) +
					CString("\r\nCalibration Path: ") + CString(p->getR3()->getCalibraPath().c_str()) +
					CString("\r\nRectification Method: ") + (p->getR3()->getRectMethos() == SiCaliWrapper::RectMethod::Top ? CString("Top") : (p->getR3()->getRectMethos() == SiCaliWrapper::RectMethod::Mean ? CString("Mean") : CString("Bottom"))) +
					strlog
				);
			}

			// start a timer to inquiry if the grabbed.
			SetTimer(1, 10, NULL);
			log(CString("Timer start!"));
			m_requireStopTimer = false;
		}
	}

}

void CSampleApiDllTestMFCDlg::OnStopRanger3()
{
	// which one
	size_t curSelTab = static_cast<size_t>(m_ddTreeCtrl.GetItemData(m_CurrentSelItem));
	if ((1000 <= curSelTab && curSelTab < 2000) | m_isTest) // device items: 1000-2000
	{
		if (m_Name_Ranger3s.count(m_ID_R3Name[curSelTab]) == 1)
		{
			auto p = m_Name_Ranger3s[m_ID_R3Name[curSelTab]];

			if (!p->isConnected())
			{
				log(CString("** Warning ** ") + CString("\r\nDevice is disconnected! Device already stopped!"));
				return;
			}

			if (!p->isStartAcquition())
			{
				log(CString("** Warning ** ") + CString("\r\nDevice already stopped! "));
				return;
			}

			if (p->isConnected() && p->isStartAcquition())
			{
				m_requireStopTimer = true;
				while (m_isStopTimer) Sleep(10);;

				p->requireStopAcquition();
				while (p->isStartAcquition()) Sleep(10);;
				bool ok = !p->isStartAcquition();

				if (ok)
				{
					// update status to item text
					CString itemName = CString("* ") + CString(p->m_gCamStatus.c_str()) + m_CurrentSelItemText;
					m_ddTreeCtrl.SetItemText(m_CurrentSelItem, itemName);
				}

				CString strlog;
				strlog.Format(_T("\r\nMissing Data: %f\r\nTimeOut: %d"), p->getR3()->getMissingData(), p->getR3()->getTimeOut());

				log((!ok ? CString("## Failed ## ") : CString("")) +
					CString("\r\nstop: ") + m_ID_R3Name[curSelTab] +
					CString("\r\nIP: ") + CString(p->getR3()->getIp().c_str()) +
					CString("\r\nMac: ") + CString(p->getR3()->getMac().c_str()) +
					CString("\r\nPatameter Path:") + CString(p->getR3()->getParametPath().c_str()) +
					CString("\r\nCalibration Path: ") + CString(p->getR3()->getCalibraPath().c_str()) +
					CString("\r\nRectification Method: ") + (p->getR3()->getRectMethos() == SiCaliWrapper::RectMethod::Top ? CString("Top") : (p->getR3()->getRectMethos() == SiCaliWrapper::RectMethod::Mean ? CString("Mean") : CString("Bottom"))) +
					strlog
				);
			}

		}
	}
}

void CSampleApiDllTestMFCDlg::OnDisconnectRanger3()
{
	// which one
	size_t curSelTab = static_cast<size_t>(m_ddTreeCtrl.GetItemData(m_CurrentSelItem));
	if ((1000 <= curSelTab && curSelTab < 2000) | m_isTest) // device items: 1000-2000
	{
		if (m_Name_Ranger3s.count(m_ID_R3Name[curSelTab]) == 1)
		{
			auto p = m_Name_Ranger3s[m_ID_R3Name[curSelTab]];

			if (!p->isConnected())
			{
				log(CString("** Warning ** ") + CString("\r\nDevice already disconnected!"));
				return;
			}

			if (p->isStartAcquition())
			{
				log(CString("** Warning ** ") + CString("\r\nDevice is started! Please click stop first!"));
				AfxMessageBox(CString("** Warning ** ") + CString("\r\nDevice is started! Please click stop first!"));
				return;
			}

			if (p->isConnected() && !p->isStartAcquition())
			{
				p->requireDisconnect();
				while (p->isConnected()) Sleep(10);

				bool ok = !p->isConnected();

				if (ok)
				{
					// update status to item text
					CString itemName = CString("* ") + CString(p->m_gCamStatus.c_str()) + m_CurrentSelItemText;
					m_ddTreeCtrl.SetItemText(m_CurrentSelItem, itemName);
				}

				CString strlog;
				strlog.Format(_T("\r\nMissing Data: %f\r\nTimeOut: %d"), p->getR3()->getMissingData(), p->getR3()->getTimeOut());

				log((!ok ? CString("## Failed ## ") : CString("")) +
					CString("\r\ndisconnect: ") + m_ID_R3Name[curSelTab] +
					CString("\r\nIP: ") + CString(p->getR3()->getIp().c_str()) +
					CString("\r\nMac: ") + CString(p->getR3()->getMac().c_str()) +
					CString("\r\nPatameter Path:") + CString(p->getR3()->getParametPath().c_str()) +
					CString("\r\nCalibration Path: ") + CString(p->getR3()->getCalibraPath().c_str()) +
					CString("\r\nRectification Method: ") + (p->getR3()->getRectMethos() == SiCaliWrapper::RectMethod::Top ? CString("Top") : (p->getR3()->getRectMethos() == SiCaliWrapper::RectMethod::Mean ? CString("Mean") : CString("Bottom"))) +
					strlog
				);
			}

		}
	}
}


// ----------------------------------------------------------------------------
// Image Data Operation


void CSampleApiDllTestMFCDlg::OnSave2OpenCV_XML()
{
	size_t curSelTab = static_cast<size_t>(m_ddTreeCtrl.GetItemData(m_CurrentSelItem));
	if (2000 <= curSelTab && curSelTab < 3000) // DataImages items: 2000-3000
	{
		CFileDialog dlg(FALSE, NULL, NULL, OFN_HIDEREADONLY, _T("XML Files(*.xml)|*.xml||"));
		if (IDOK != dlg.DoModal())		return;

		CString strPath = dlg.GetPathName();
		if (strPath.IsEmpty())	return;

		if (strPath.Mid(strPath.GetLength() - 3, 3) != CString("xml") && strPath.Mid(strPath.GetLength() - 3, 3) != CString("XML"))
			strPath += ".xml";

		MFC_IMG img = m_path_ImgData[m_currentImgPath];
		Vec<Str> vecNames;
		Vec<cv::Mat> vecMats;
		for (auto sub : img.get_NameList())
		{
			vecNames.push_back(img.get_StrDN(sub));
			vecMats.push_back(img.getMat(sub));
		}

		_SaveAsOpenCVXML(strPath, img);

		log(CString("save image as OpenCV xml ok! Path: ") + strPath);
	}
}

void CSampleApiDllTestMFCDlg::OnSave2PC_TXT()
{
	size_t curSelTab = static_cast<size_t>(m_ddTreeCtrl.GetItemData(m_CurrentSelItem));
	if (2000 <= curSelTab && curSelTab < 3000) // DataImages items: 2000-3000
	{
		if (m_path_ImgData.count(m_currentImgPath) == 1)
		{
			// calibrated data only
			SICK_DATA img = m_path_ImgData[m_currentImgPath].get_SICK_DATA();
			if (!img.has(SICK_DN::RAN_CAL))
			{
				AfxMessageBox(CString("Support Calibrated Data only!"));
				return;
			}

			// show window to select saving path
			CFileDialog dlg(FALSE, NULL, NULL, OFN_HIDEREADONLY, _T("TXT Files(*.txt)|*.txt||"));
			if (IDOK != dlg.DoModal())		return;

			CString strPath = dlg.GetPathName();
			if (strPath.IsEmpty())	return;

			std::string path = CT2A(strPath);
			if (strPath.Mid(strPath.GetLength() - 3, 3) != CString("txt") && strPath.Mid(strPath.GetLength() - 3, 3) != CString("TXT"))
				path += ".txt";

			std::stringstream ss; ss << "Save as Point Cloud txt < " << path << " >\r\n\r\n";
			if (img.SaveCalibratedDataToPointCloud(path))
				ss << "OK!";
			else
				ss << "Failed!";

			log(CString(ss.str().c_str()) + CString("\r\n"));
		}
		else
		{
			AfxMessageBox(CString("Please select a calibrated image first!"));
			return;
		}
	}
}

void CSampleApiDllTestMFCDlg::OnSave2SICK_DAT()
{
	size_t curSelTab = static_cast<size_t>(m_ddTreeCtrl.GetItemData(m_CurrentSelItem));
	if (2000 <= curSelTab && curSelTab < 3000) // DataImages items: 2000-3000
	{
		// save to sick dat
		CFileDialog dlg(FALSE, NULL, NULL, OFN_HIDEREADONLY, _T("All Files(*.dat)|*.xml||"));
		if (IDOK != dlg.DoModal())		return;

		CString strPath = dlg.GetPathName();
		if (strPath.IsEmpty())		return;

		if (m_path_ImgData.count(m_currentImgPath) == 1)
		{
			_SaveAsSICKDAT(m_path_ImgData.at(m_currentImgPath), strPath);
			log(CString("save image as ICON ok! Path: ") + strPath + CString(".dat|xml"));
		}
		else
			log(CString("save image as ICON failed! Please select a range image!"));
	}
}

void CSampleApiDllTestMFCDlg::OnDataImageSetting()
{
	size_t curSelTab = static_cast<size_t>(m_ddTreeCtrl.GetItemData(m_CurrentSelItem));
	if (2000 <= curSelTab && curSelTab < 3000) // DataImages items: 2000-3000
	{
		if (m_path_ImgData.count(m_currentImgPath) == 1)
		{
			// Delete a closed window, new one.
			ParametersDlg* p = new ParametersDlg(this);
			p->Create(IDD_DIALOG_PARAMETER);
			p->InitImageDataSetting(&(m_path_ImgData[m_currentImgPath]));
			p->SetWindowTextW(CString("Settings : ") + m_currentImgPath);
			p->ShowWindow(SW_SHOW);
		}
	}
}

void CSampleApiDllTestMFCDlg::OnDelete2()
{
	// which one
	size_t curSelTab = static_cast<size_t>(m_ddTreeCtrl.GetItemData(m_CurrentSelItem));
	if (2000 <= curSelTab && curSelTab < 3000) // DataImages items: 2000-3000
	{
		// delete image info
		if (m_ID_ImgPaths.count(curSelTab) == 1 && m_path_ImgData.count(m_currentImgPath) == 1)
		{
			log(CString("delete image") +
				CString("\r\nimage name: ") + m_path_ImgData[m_currentImgPath].get_name() +
				CString("\r\nLoad image path: ") + m_currentImgPath +
				CString("\r\nImage width and Heihgt : ") + m_path_ImgData[m_currentImgPath].get_size()
			);

			CString path = m_ID_ImgPaths[curSelTab];
			m_path_ImgData.erase(path);
			m_ID_ImgPaths.erase(curSelTab);
			m_CurrentSelItem = m_hItemRoot;
			m_CurrentSelItemText = "SICK";

			_updateDataList();
			_updateSensorList();
			_showMatImgToWnd(GetDlgItem(IDC_STATIC_CAM1_RANGE), cv::Mat());
		}
	}
}


// ----------------------------------------------------------------------------
// Sensor Data Operation


void CSampleApiDllTestMFCDlg::OnSave3()
{
	// which one
	size_t curSelTab = static_cast<size_t>(m_ddTreeCtrl.GetItemData(m_CurrentSelItem));
	if (3000 <= curSelTab && curSelTab < 4000) // SensorImages items: 3000-4000
	{
		// save to sick dat
		CFileDialog dlg(FALSE, NULL, NULL, OFN_HIDEREADONLY, _T("All Files(*.dat)|*.xml||"));
		if (IDOK != dlg.DoModal())		return;

		CString strPath = dlg.GetPathName();
		if (strPath.IsEmpty())		return;

		if (m_path_SensorData.count(m_currentSensorPath) == 1)
		{
			_SaveAsSICKDAT(m_path_SensorData.at(m_currentSensorPath), strPath);
			log(CString("save sensor image as ICON ok! Path: ") + strPath + CString(".dat|xml"));
		}
		else
			log(CString("save sensor image as ICON failed! Please select a sensor image!"));
	}
}

void CSampleApiDllTestMFCDlg::OnDelete3()
{
	// which one
	size_t curSelTab = static_cast<size_t>(m_ddTreeCtrl.GetItemData(m_CurrentSelItem));
	if (3000 <= curSelTab && curSelTab < 4000) // SensorImages items: 3000-4000
	{
		// delete image info
		if (m_ID_SensorPath.count(curSelTab) == 1)
		{
			log(CString("delete sensor image") +
				CString("\r\nimage name: ") + m_path_SensorData[m_currentSensorPath].get_name() +
				CString("\r\nLoad image path: ") + m_currentSensorPath
			);

			CString path = m_ID_SensorPath[curSelTab];
			m_path_SensorData.erase(path);
			m_ID_SensorPath.erase(curSelTab);
			m_CurrentSelItem = m_hItemRoot;
			m_CurrentSelItemText = "SICK";

			_updateDataList();
			_updateSensorList();
			_showMatImgToWnd(GetDlgItem(IDC_STATIC_CAM1_RANGE), cv::Mat());
		}
	}
}

void CSampleApiDllTestMFCDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (m_requireStopTimer)
	{
		m_isStopTimer = true;
		KillTimer(1);
		log(CString("Timer stop!"));
		return;
	}
	else
		m_isStopTimer = false;

	if(!m_isShowSensor && m_isShowDevice)
	{
		if (m_Name_Ranger3s.count(m_currentR3Name) == 1)
		{
			auto pIG = m_Name_Ranger3s[m_currentR3Name];
			if (pIG->isGrabbed())
			{
				// --- Get Image.
				m_currentR3ImgT = pIG->getImgT();
				std::stringstream  ss; ss << m_currentR3ImgT.get_ID();
				SetDlgItemText(IDC_EDIT_IMGID, CString("ID: ") + CString(ss.str().c_str()));
				m_currentR3Image = MFC_IMG(m_currentR3ImgT);

				NAME_VEC names = m_currentR3Image.get_NameList();
				if(_ComboImgTypesNeedUpdate(names))
					_ComboImgTypesUpdate(names);

				if (!m_currentR3Image.isEmpty())
				{
					cv::Mat src = m_currentR3Image.getMat(m_currentR3ShowType);
					_showMatImgToWnd(GetDlgItem(IDC_STATIC_CAM1_RANGE), src);
					m_isStartOK = true;
				}
			}
		}
	}

	CDialogEx::OnTimer(nIDEvent);
}

void CSampleApiDllTestMFCDlg::OnNMClickTreeList(NMHDR *pNMHDR, LRESULT *pResult)
{
	if (m_CurrentSelItem == NULL || m_CurrentSelItemText == CString(""))
		return;

	// get Current clicked Item
	CPoint ppp;
	GetCursorPos(&ppp);
	m_ddTreeCtrl.ScreenToClient(&ppp);

	HTREEITEM H = m_ddTreeCtrl.HitTest(ppp);
	CString Hstr = m_ddTreeCtrl.GetItemText(H);
	
	if (H == NULL || Hstr == CString(""))
		return;

	// already choose
	if (Hstr.Mid(0, 1) == CString("*"))
		return; 

	// recover old item text
	size_t lastSelTab = static_cast<size_t>(m_ddTreeCtrl.GetItemData(m_CurrentSelItem));
	if (1000 <= lastSelTab && lastSelTab < 2000)
	{
		m_ddTreeCtrl.SetItemText(m_CurrentSelItem,
			CString(m_Name_Ranger3s[m_ID_R3Name[lastSelTab]]->m_gCamStatus.c_str()) + m_CurrentSelItemText);
	}
	else
		m_ddTreeCtrl.SetItemText(m_CurrentSelItem, m_CurrentSelItemText);

	// update current item ptr
	m_CurrentSelItem = H;
	size_t curSelTab = static_cast<size_t>(m_ddTreeCtrl.GetItemData(m_CurrentSelItem));

	// Devices
	if (1000 <= curSelTab && curSelTab < 2000)
	{
		m_isShowDevice = true;
		m_isShowSensor = false;

		if (m_ID_R3Name.count(curSelTab) == 1 && Hstr.Mid(0, 1) == CString("<"))
		{
			m_currentR3Name = m_ID_R3Name[curSelTab];

			// update currentText according to item type
			m_CurrentSelItemText = Hstr.Mid(14, Hstr.GetLength() - 14);
			m_ddTreeCtrl.SetItemText(m_CurrentSelItem, CString("* ") +
				CString(m_Name_Ranger3s[m_ID_R3Name[curSelTab]]->m_gCamStatus.c_str()) + m_CurrentSelItemText);
		}
		return;
	}

	// update current item Text
	m_CurrentSelItem = H;
	m_CurrentSelItemText = Hstr;
	m_ddTreeCtrl.SetItemText(m_CurrentSelItem, CString("* ") + m_CurrentSelItemText);

	// DataImages
	if (2000 <= curSelTab && curSelTab < 3000)
	{
		m_isShowDevice = false;
		m_isShowSensor = false;

		m_currentImgPath = m_ID_ImgPaths[curSelTab];
		if (m_path_ImgData.count(m_currentImgPath) == 1)
		{
			log(CString("change to show image: ") + m_path_ImgData[m_currentImgPath].get_name() +
				CString("\r\nimage name: ") + m_path_ImgData[m_currentImgPath].get_name() +
				CString("\r\nLoad image path: ") + m_currentImgPath +
				m_path_ImgData[m_currentImgPath].get_BasicInfo()
			);

			_RefleshImageViever();
		}
	}

	// SensorImages
	if (3000 <= curSelTab && curSelTab < 4000)
	{
		m_isShowSensor = true;
		m_isShowDevice = false;
		m_currentSensorPath = m_path_SensorData[m_ID_SensorPath[curSelTab]].get_path();
		if (m_path_SensorData.count(m_currentSensorPath) == 1)
		{
			log(CString("change to show sensor image: ") + m_path_SensorData[m_currentSensorPath].get_name() +
				CString("\r\n image name: ") + m_path_SensorData[m_currentSensorPath].get_name() +
				CString("\r\nLoad image path: ") + m_currentSensorPath +
				m_path_SensorData[m_currentSensorPath].get_BasicInfo()
			);
			_showMatImgToWnd(GetDlgItem(IDC_STATIC_CAM1_RANGE), m_path_SensorData[m_currentSensorPath].getMat(SICK_DN::SEN));
		}
	}

	*pResult = 0;
}

void CSampleApiDllTestMFCDlg::OnNMDblclkTreeList(NMHDR *pNMHDR, LRESULT *pResult)
{
	// get Current clicked Item
	CPoint ppp;
	GetCursorPos(&ppp);
	m_ddTreeCtrl.ScreenToClient(&ppp);

	HTREEITEM H = m_ddTreeCtrl.HitTest(ppp);
	CString Hstr = m_ddTreeCtrl.GetItemText(H);

	if (H == NULL || Hstr == CString(""))
		return;

	if (Hstr.Mid(0,1) == CString("*"))
	{
		m_isShowSensor = false;
		size_t curSelTab = static_cast<size_t>(m_ddTreeCtrl.GetItemData(m_CurrentSelItem));

		// Devices
		if (1000 <= curSelTab && curSelTab < 2000)
		{
			m_isShowDevice = true;
			m_isShowSensor = false;

			// connect start stop
			if (m_Name_Ranger3s.count(m_currentR3Name) == 1)
			{
				if (m_Name_Ranger3s[m_currentR3Name]->m_gCamStatus == "<Disconnected>")
					OnConnectRanger3();
				else if (m_Name_Ranger3s[m_currentR3Name]->m_gCamStatus == "<Connected___>")
					OnStartRanger3();
				else if (m_Name_Ranger3s[m_currentR3Name]->m_gCamStatus == "<Started_____>")
					OnStopRanger3();
				else if (m_Name_Ranger3s[m_currentR3Name]->m_gCamStatus == "<Stopped_____>")
					OnDisconnectRanger3();
			}
		}
	}
	
	*pResult = 0;
}

void CSampleApiDllTestMFCDlg::OnCbnSelchangeComboImgtype()
{
	int id = m_Combo_ImgTypes.GetCurSel();
	m_currentR3ShowType = m_currentR3AllType[id];
}

void CSampleApiDllTestMFCDlg::OnClose()
{
	OnStopRanger3();
	OnDisconnectRanger3();

	CDialogEx::OnClose();
}

void CSampleApiDllTestMFCDlg::OnBnClickedButtonClearlog()
{
	SetDlgItemTextW(IDC_EDIT_LOG, CString(""));
}


// ----------------------------------------------------------------------------
// Test

UINT test(LPVOID lpParam)
{
	auto dlg = (CSampleApiDllTestMFCDlg*)lpParam;
	dlg->doTest();
	return 1;
}


void CSampleApiDllTestMFCDlg::OnBnClickedButtonSelftest()
{	
	CWinThread * pThread = AfxBeginThread(test, this);
}

size_t CSampleApiDllTestMFCDlg::getCurrentSelectItem()
{
	return static_cast<size_t>(m_ddTreeCtrl.GetItemData(m_CurrentSelItem));
}

void CSampleApiDllTestMFCDlg::logTest(CString str)
{
	SetDlgItemTextW(IDC_EDIT_POSITION, str);
}

bool CSampleApiDllTestMFCDlg::doTest()
{
	// test all function of MFC demo
	m_isTest = true;

	size_t curSelTab = getCurrentSelectItem();
	if ((1000 <= curSelTab && curSelTab < 2000)) // device items: 1000-2000
	{
		logTest(CString("1/9 Tesing... OnDisconnectRanger3	")); OnDisconnectRanger3(); Sleep(1000);   (CString("\r\n\r\r\n\r\n* * * OnDisconnectRanger3	done!"));
		logTest(CString("2/9 Tesing... OnConnectRanger3		")); OnConnectRanger3();	Sleep(1000);   (CString("\r\n\r\r\n\r\n* * * OnConnectRanger3		done!"));
		
		/// start test
		{
			m_isStartOK = false;
			logTest(CString("3/9 Tesing... OnStartRanger3		")); OnStartRanger3();		Sleep(10000);  (CString("\r\n\r\r\n\r\n* * * OnStartRanger3			done!"));
			if (!m_isStartOK)
				AfxMessageBox(CString("3/9 Tesing... OnStartRanger3	FAILED!!!!!!!!!"));
		}
		
		logTest(CString("4/9 Tesing... OnStopRanger3		")); OnStopRanger3();		Sleep(1000);   log(CString("\r\n\r\r\n\r\n* * * OnStopRanger3		done!"));
		logTest(CString("5/9 Tesing... OnDisconnectRanger3	")); OnDisconnectRanger3(); Sleep(1000);   log(CString("\r\n\r\r\n\r\n* * * OnDisconnectRanger3	done!"));
		logTest(CString("6/9 Tesing... OnConnectRanger3		")); OnConnectRanger3();	Sleep(1000);   log(CString("\r\n\r\r\n\r\n* * * OnConnectRanger3	done!"));
		
		/// start test
		{
			m_isStartOK = false;
			logTest(CString("6/9 Tesing... OnStartRanger3		")); OnStartRanger3();		Sleep(10000);  (CString("\r\n\r\r\n\r\n* * * OnStartRanger3			done!"));
			if (!m_isStartOK)
				AfxMessageBox(CString("6/9 Tesing... OnStartRanger3	FAILED!!!!!!!!!"));
		}


		logTest(CString("8/9 Tesing... OnStopRanger3		")); OnStopRanger3();		Sleep(1000);   log(CString("\r\n\r\r\n\r\n* * * OnStopRanger3		one!"));
		logTest(CString("9/9 Tesing... OnDisconnectRanger3	")); OnDisconnectRanger3(); Sleep(1000);   log(CString("\r\n\r\r\n\r\n* * * OnDisconnectRanger3	done!"));
	}
	else if ((2000 <= curSelTab && curSelTab < 3000))
	{

	}
	else if ((3000 <= curSelTab && curSelTab < 4000))
	{

	}
	else
	{
		logTest(CString("Please select a item to test!!!!"));
		AfxMessageBox(CString("Please select a item to test!!!!"));
		m_isTest = false;
		return FALSE;
	}

	logTest(CString("Tesing... DONE!!!!!!!!!!!!!!"));
	AfxMessageBox(CString("test done!"));
	m_isTest = false;
	return TRUE;
}


