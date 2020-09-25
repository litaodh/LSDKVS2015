// SubDialogParameter.cpp : 实现文件
//

#include "stdafx.h"
#include "SampleApiDllTestMFC.h"
#include "SubDialogParameter.h"

#include "afxdialogex.h"


// SubDialogParameter 对话框

IMPLEMENT_DYNAMIC(SubDialogParameter, CDialogEx)

SubDialogParameter::SubDialogParameter(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_PARAMETER, pParent)
{
	//m_initialized = false;
	//pDialog = std::vector<CDialog>(32);
}

SubDialogParameter::~SubDialogParameter()
{
}

#if 0
void SubDialogParameter::InitParameter(RECT rect)
{
	m_TreeFile.Create(/*TVS_EDITLABELS |*/ TVS_HASBUTTONS | WS_CHILD | WS_VISIBLE | TVS_SHOWSELALWAYS, rect, this, IDC_TREE_PARA);
	m_TreeFile.ShowWindow(SW_SHOWNORMAL);
	m_TreeFile.InitTreeFileCSV();
	m_TreeFile.SetTreeRootCSV();

	size_t i = 0;
	std::vector<int> IDDs = {
	IDD_DIALOG_DeviceControl,
	IDD_DIALOG_ImageFormatControl,
	IDD_DIALOG_Scan3dControl,
	IDD_DIALOG_AcquisitionControl,
	IDD_DIALOG_DigitalIOControl,
	IDD_DIALOG_TimerControl,
	IDD_DIALOG_EncoderControl,
	IDD_DIALOG_EventControl,
	IDD_DIALOG_FileAccessControl,
	IDD_DIALOG_ChunkDataControl,
	IDD_DIALOG_TestControl,
	IDD_DIALOG_TransportLayerControl,
	IDD_DIALOG_FirmwareUpdate,

	IDD_DIALOG_DeviceControl_Sensor,
	IDD_DIALOG_ImageFormatControl_Region0,
	IDD_DIALOG_ImageFormatControl_Region1,
	IDD_DIALOG_ImageFormatControl_Scan3dExtraction1,
	IDD_DIALOG_Scan3dControl_Scan3dExtraction1,
	IDD_DIALOG_AcquisitionControl_LineStart,
	IDD_DIALOG_AcquisitionControl_FrameStart,
	IDD_DIALOG_AcquisitionControl_ExposureStart,
	IDD_DIALOG_DigitalIOControl_LaserStrobe1Output,
	IDD_DIALOG_DigitalIOControl_LaserStrobe2Output,
	IDD_DIALOG_TimerControl_LaserStrobe1Timer,
	IDD_DIALOG_TimerControl_LaserStrobe2Timer,
	IDD_DIALOG_EventControl_LogMessage,
	IDD_DIALOG_TransportLayerControl_GigEVision,

	IDD_DIALOG_ImageFormatControl_Region0_Intensity,
	IDD_DIALOG_ImageFormatControl_Region0_Reflectance,
	IDD_DIALOG_ImageFormatControl_Region0_Range,
	//IDD_DIALOG_ImageFormatControl_Region1_Reflectance,
	//IDD_DIALOG_ImageFormatControl_Region1_Range,
	IDD_DIALOG_ImageFormatControl_Scan3dExtraction1_Reflectance,
	IDD_DIALOG_ImageFormatControl_Scan3dExtraction1_Range,

	};

	for (size_t i = 0; i < IDDs.size(); ++i)
	{
		pDialog[i].Create(IDDs[i], &m_tab);
	}


	//设定在Tab内显示的范围
	m_tab.GetWindowRect(m_rc);
	pDialog[0].MoveWindow(&m_rc);
	pDialog[1].MoveWindow(&m_rc);

	//显示初始页面
	pDialog[0].ShowWindow(SW_SHOW);
	pDialog[1].ShowWindow(SW_HIDE);

	//保存当前选择
	m_CurSelTab = 0;
	m_initialized = true;
	m_isClosed = false;

}
#endif // 0


void SubDialogParameter::InitParameter(RECT rect, R3 * p)
{
	//m_TreeFile.Create(/*TVS_EDITLABELS |*/ TVS_HASBUTTONS | WS_CHILD | WS_VISIBLE | TVS_SHOWSELALWAYS, rect, this, IDC_TREE_PARA);
	//m_TreeFile.ShowWindow(SW_SHOWNORMAL);
	//m_TreeFile.InitTreeFileCSV();
	//m_TreeFile.SetTreeRootCSV();

	//size_t i = 0;
	//std::vector<int> IDDs = {
	//	IDD_DIALOG_DeviceControl,
	//	IDD_DIALOG_ImageFormatControl,
	//	IDD_DIALOG_Scan3dControl,
	//	IDD_DIALOG_AcquisitionControl,
	//	IDD_DIALOG_DigitalIOControl,
	//	IDD_DIALOG_TimerControl,
	//	IDD_DIALOG_EncoderControl,
	//	IDD_DIALOG_EventControl,
	//	IDD_DIALOG_FileAccessControl,
	//	IDD_DIALOG_ChunkDataControl,
	//	IDD_DIALOG_TestControl,
	//	IDD_DIALOG_TransportLayerControl,
	//	IDD_DIALOG_FirmwareUpdate,

	//	IDD_DIALOG_DeviceControl_Sensor,
	//	IDD_DIALOG_ImageFormatControl_Region0,
	//	IDD_DIALOG_ImageFormatControl_Region1,
	//	IDD_DIALOG_ImageFormatControl_Scan3dExtraction1,
	//	IDD_DIALOG_Scan3dControl_Scan3dExtraction1,
	//	IDD_DIALOG_AcquisitionControl_LineStart,
	//	IDD_DIALOG_AcquisitionControl_FrameStart,
	//	IDD_DIALOG_AcquisitionControl_ExposureStart,
	//	IDD_DIALOG_DigitalIOControl_LaserStrobe1Output,
	//	IDD_DIALOG_DigitalIOControl_LaserStrobe2Output,
	//	IDD_DIALOG_TimerControl_LaserStrobe1Timer,
	//	IDD_DIALOG_TimerControl_LaserStrobe2Timer,
	//	IDD_DIALOG_EventControl_LogMessage,
	//	IDD_DIALOG_TransportLayerControl_GigEVision,

	//	IDD_DIALOG_ImageFormatControl_Region0_Intensity,
	//	IDD_DIALOG_ImageFormatControl_Region0_Reflectance,
	//	IDD_DIALOG_ImageFormatControl_Region0_Range,
	//	//IDD_DIALOG_ImageFormatControl_Region1_Reflectance,
	//	//IDD_DIALOG_ImageFormatControl_Region1_Range,
	//	IDD_DIALOG_ImageFormatControl_Scan3dExtraction1_Reflectance,
	//	IDD_DIALOG_ImageFormatControl_Scan3dExtraction1_Range,

	//};

	//for (size_t i = 0; i<IDDs.size(); ++i)
	//{
	//	pDialog[i].Create(IDDs[i], &m_tab);
	//}


	////设定在Tab内显示的范围
	//m_tab.GetWindowRect(m_rc);
	//pDialog[0].MoveWindow(&m_rc);
	//pDialog[1].MoveWindow(&m_rc);

	////显示初始页面
	//pDialog[0].ShowWindow(SW_SHOW);
	//pDialog[1].ShowWindow(SW_HIDE);

	//保存当前选择
	m_CurSelTab = 0;
	m_initialized = true;
	m_isClosed = false;

	//// get parameters from device
	//m_para.ExposureTime_RegionSelector_Region0;
	//p->getParameterValue("ExposureTime_RegionSelector_Region0", m_para.ExposureTime_RegionSelector_Region0);
	//CString strValue;
	//strValue.Format(_T("%f"), m_para.ExposureTime_RegionSelector_Region0);
	//SetDlgItemTextW(IDC_EXPOTIME_R0, strValue);

}

bool SubDialogParameter::IsClosed()
{
	return m_isClosed;
}

void SubDialogParameter::DoDataExchange(CDataExchange* pDX)
{
	//DDX_Control(pDX, IDC_TAB, m_tab);
	CDialogEx::DoDataExchange(pDX);
	
}

void SubDialogParameter::UpdateTab(int _CurSelTab, CRect _rc)
{
	for (size_t i=0; i<pDialog.size(); ++i)
	{
		pDialog[i].MoveWindow(_rc);
		pDialog[i].ShowWindow(i == _CurSelTab?SW_SHOW:SW_HIDE);
	}
}


BEGIN_MESSAGE_MAP(SubDialogParameter, CDialogEx)
	//ON_NOTIFY(TCN_SELCHANGE, IDC_TAB, &SubDialogParameter::OnTcnSelchangeTab)
	ON_WM_MOVE()
	ON_WM_CLOSE()
	//ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_PARA, &SubDialogParameter::OnTvnSelchangedTreePara)
	//ON_BN_CLICKED(ID_Start, &SubDialogParameter::OnBnClickedStart)
	//ON_BN_CLICKED(ID_Import, &SubDialogParameter::OnBnClickedImport)
	//ON_BN_CLICKED(ID_Export, &SubDialogParameter::OnBnClickedExport)
	//ON_NOTIFY(NM_CUSTOMDRAW, IDC_TREE_PARA, &SubDialogParameter::OnNMCustomdrawTreePara)
END_MESSAGE_MAP()


// SubDialogParameter 消息处理程序

#if 0

void SubDialogParameter::OnTcnSelchangeTab(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	m_CurSelTab = m_tab.GetCurSel();
	UpdateTab(m_CurSelTab, m_rc);
	*pResult = 0;
}


void SubDialogParameter::OnMove(int x, int y)
{
	CDialogEx::OnMove(x, y);

	// TODO: 在此处添加消息处理程序代码
	//设定在Tab内显示的范围
	if (m_initialized)
	{
		m_tab.GetWindowRect(m_rc);
		UpdateTab(m_CurSelTab, m_rc);
	}
}


void SubDialogParameter::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (m_initialized)
	{
		for (size_t i = 0; i < pDialog.size(); ++i)
			pDialog[i].DestroyWindow();
	}
	m_isClosed = true;
	CDialogEx::OnClose();
}


void SubDialogParameter::OnTvnSelchangedTreePara(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	// 获取当前选中节点深度

	//OnNMCustomdrawTreePara(pNMHDR, pResult);
	//m_CurSelTab = m_TreeFile.GetItemData(m_TreeFile.GetSelectedItem());
	//UpdateTab(m_CurSelTab, m_rc);

	*pResult = 0;
}


void SubDialogParameter::OnBnClickedStart()
{
	CString label;
	GetDlgItem(ID_Start)->GetWindowTextW(label);
	if (label == "Start")
		GetDlgItem(ID_Start)->SetWindowTextW(CString("Stop"));
	else
		GetDlgItem(ID_Start)->SetWindowTextW(CString("Start"));

	// TODO: Start camera here



}


void SubDialogParameter::OnBnClickedImport()
{
	// TODO: 在此添加控件通知处理程序代码


}


void SubDialogParameter::OnBnClickedExport()
{
	// TODO: 在此添加控件通知处理程序代码
}


void SubDialogParameter::OnNMCustomdrawTreePara(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	//NMTVCUSTOMDRAW* plvoid = reinterpret_cast<NMTVCUSTOMDRAW*>(pNMHDR);
	//plvoid->clrTextBk = RGB(0, 0, 255);
	//plvoid->clrText = RGB(0, 255, 0);

	*pResult = 0;
}

#endif // 0
