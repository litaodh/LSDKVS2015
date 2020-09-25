// FeatureSet.cpp : 实现文件
//

#include "stdafx.h"
#include "LusterCameraControlCDemo.h"
#include "FeatureSet.h"
#include "afxdialogex.h"
#include "LstLfncDef.h"
#include "LusterCameraControlCDemoDlg.h"

CLusterCameraControlCDemoDlg *CameraControl;

//Region1模式下的图像属性值
extern int mm_height3D;
extern int mm_offsetX3D;
extern int mm_offsetY3D;


// FeatureSet 对话框

IMPLEMENT_DYNAMIC(FeatureSet, CDialogEx)

FeatureSet::FeatureSet(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_FeatureSet, pParent)
{

}

FeatureSet::~FeatureSet()
{
}

void FeatureSet::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_TemSelector, f_TemSel);
	DDX_Control(pDX, IDC_RegionSelector, f_RegionSel);
	DDX_Control(pDX, IDC_COMBO_ComponentEnable, f_ComponentEnable);
	DDX_Control(pDX, IDC_TriggerModeSelector, f_TriggerModeSelector);
	DDX_Control(pDX, IDC_TriggerMode, f_TriggerMode);
	DDX_Control(pDX, IDC_TriggerActivation, f_TriggerActivation);
	DDX_Control(pDX, IDC_TestPattern, f_TestPattern);

	DDX_Control(pDX, IDC_Temperature, f_Temperature);
	DDX_Control(pDX, IDC_RegionDescription, f_RegionDes);
	DDX_Control(pDX, IDC_ExposureTime, f_ExposureTime);
	DDX_Control(pDX, IDC_Width, f_Width);
	DDX_Control(pDX, IDC_Height, f_Height);
	DDX_Control(pDX, IDC_OffsetX, f_OffsetX);
	DDX_Control(pDX, IDC_OffsetY, f_OffsetY);

	DDX_Control(pDX, IDC_FrameRate, f_FrameRate);
	DDX_Control(pDX, IDC_LineRate, f_LineRate);

	DDX_Control(pDX, IDC_COMBO_Scan3D, f_ExtractionScan3d);
	DDX_Control(pDX, IDC_COMBO_WamSize, f_WamSize);
	DDX_Control(pDX, IDC_SearchMode3D, f_SearchMode3D);
	DDX_Control(pDX, IDC_RangeAxis, f_RangeAxis);
	DDX_Control(pDX, IDC_ThresHold,f_ThresHold);
	DDX_Control(pDX, IDC_SearchDirection, f_SearchDirection);
	DDX_Control(pDX, IDC_PixelFormat, f_PixelFormat);
	DDX_Control(pDX, IDC_ReflectanceFilter, f_ReflectanceFilter);
	DDX_Control(pDX, IDC_MultiSlopeKneePointCount, f_MultiSlopeKneePointCount);

	DDX_Control(pDX, IDC_MultiSlopeMode, f_MultiSlopeMode);
	DDX_Control(pDX, IDC_EncoderDivider, f_EncoderDivider);
	DDX_Control(pDX, IDC_EventNotification, f_EventNotification);
	DDX_Control(pDX, IDC_GevSCPD, f_GevSCPD);
	DDX_Control(pDX, IDC_LineSelector, f_LineSelector);
	DDX_Control(pDX, IDC_LineFormat, f_LineFormat);
	DDX_Control(pDX, IDC_DeviceScanType, f_DeviceScanType);
	DDX_Control(pDX, IDC_AcquisitionMode, f_AcquisitionMode);
}


BEGIN_MESSAGE_MAP(FeatureSet, CDialogEx)
	ON_CBN_SELCHANGE(IDC_TemSelector, &FeatureSet::OnCbnSelchangeTemselector)
	ON_CBN_SELCHANGE(IDC_RegionSelector, &FeatureSet::OnCbnSelchangeRegionselector)
	ON_EN_KILLFOCUS(IDC_ExposureTime, &FeatureSet::OnEnKillfocusExposuretime)
	ON_EN_KILLFOCUS(IDC_Width, &FeatureSet::OnEnKillfocusWidth)
	ON_EN_KILLFOCUS(IDC_Height, &FeatureSet::OnEnKillfocusHeight)
	ON_EN_KILLFOCUS(IDC_OffsetY, &FeatureSet::OnEnKillfocusOffsety)
	ON_EN_KILLFOCUS(IDC_LineRate, &FeatureSet::OnEnKillfocusLinerate)
	ON_EN_KILLFOCUS(IDC_FrameRate, &FeatureSet::OnEnKillfocusFramerate)
	ON_CBN_SELCHANGE(IDC_TriggerMode, &FeatureSet::OnCbnSelchangeTriggermode)
	
	ON_CBN_SELCHANGE(IDC_COMBO_ComponentEnable, &FeatureSet::OnCbnSelchangeComboComponentenable)
	ON_CBN_SELCHANGE(IDC_COMBO_WamSize, &FeatureSet::OnCbnSelchangeComboWamsize)
	ON_EN_KILLFOCUS(IDC_ThresHold, &FeatureSet::OnEnKillfocusThreshold)
	ON_CBN_SELCHANGE(IDC_SearchMode3D, &FeatureSet::OnCbnSelchangeSearchmode3d)
	ON_CBN_SELCHANGE(IDC_RangeAxis, &FeatureSet::OnCbnSelchangeRangeaxis)
	ON_CBN_SELCHANGE(IDC_SearchDirection, &FeatureSet::OnCbnSelchangeSearchdirection)
	ON_CBN_SELCHANGE(IDC_PixelFormat, &FeatureSet::OnCbnSelchangePixelformat)
	ON_CBN_SELCHANGE(IDC_TriggerModeSelector, &FeatureSet::OnCbnSelchangeTriggermodeselector)
	ON_CBN_SELCHANGE(IDC_TriggerActivation, &FeatureSet::OnCbnSelchangeTriggeractivation)
	ON_CBN_SELCHANGE(IDC_TestPattern, &FeatureSet::OnCbnSelchangeTestpattern)
	ON_CBN_SELCHANGE(IDC_MultiSlopeMode, &FeatureSet::OnCbnSelchangeMultislopemode)
	ON_EN_KILLFOCUS(IDC_EncoderDivider, &FeatureSet::OnEnKillfocusEncoderdivider)
	ON_CBN_SELCHANGE(IDC_EventNotification, &FeatureSet::OnCbnSelchangeEventnotification)
	ON_EN_KILLFOCUS(IDC_GevSCPD, &FeatureSet::OnEnKillfocusGevscpd)
	ON_CBN_SELCHANGE(IDC_ReflectanceFilter, &FeatureSet::OnCbnSelchangeReflectancefilter)
	ON_EN_KILLFOCUS(IDC_MultiSlopeKneePointCount, &FeatureSet::OnEnKillfocusMultislopekneepointcount)
	ON_CBN_SELCHANGE(IDC_LineSelector, &FeatureSet::OnCbnSelchangeLineselector)
	ON_CBN_SELCHANGE(IDC_LineFormat, &FeatureSet::OnCbnSelchangeLineformat)
	ON_CBN_SELCHANGE(IDC_DeviceScanType, &FeatureSet::OnCbnSelchangeDevicescantype)
	ON_CBN_SELCHANGE(IDC_AcquisitionMode, &FeatureSet::OnCbnSelchangeAcquisitionmode)
	ON_EN_KILLFOCUS(IDC_OffsetX, &FeatureSet::OnEnKillfocusOffsetx)
	ON_BN_CLICKED(IDOK, &FeatureSet::OnBnClickedOk)
END_MESSAGE_MAP()


// FeatureSet 消息处理程序

BOOL FeatureSet::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CameraControl = (CLusterCameraControlCDemoDlg*)::AfxGetMainWnd();
	UpdateData(TRUE);

	//下拉窗口初始化
	f_TemSel.InsertString(0, _T("Sensor"));
	f_TemSel.InsertString(1, _T("Mainboard"));
	f_TemSel.InsertString(2, _T("SensorBoard"));
	f_TemSel.InsertString(3, _T("ZpmSoc"));
	f_TemSel.InsertString(4, _T("Zpm1"));
	f_TemSel.InsertString(5, _T("Zpm2"));
	f_TemSel.InsertString(6, _T("Zpm3"));
	f_TemSel.InsertString(7, _T("Zpm4"));
	f_TemSel.InsertString(8, _T("Filter"));

	f_RegionSel.InsertString(0, _T("Region0"));
	f_RegionSel.InsertString(1, _T("Region1"));
	f_RegionSel.InsertString(2, _T("Scan3dExtraction1"));

	//f_TriggerModeSelector.InsertString(0, _T("FrameStart"));
	f_TriggerModeSelector.InsertString(0, _T("LineStart"));

	f_TriggerMode.InsertString(0, _T("Off"));
	f_TriggerMode.InsertString(1, _T("On"));

	f_TriggerActivation.InsertString(0, _T("RisingEdge"));
	f_TriggerActivation.InsertString(1, _T("LevelHigh"));

	f_ComponentEnable.InsertString(0, _T("True"));
	f_ComponentEnable.InsertString(1, _T("False"));

	f_TestPattern.InsertString(0, _T("Off"));
	f_TestPattern.InsertString(1, _T("GreyHorizontalRampMoving"));

	f_ExtractionScan3d.InsertString(0, _T("Scan3dExtraction1"));

	f_WamSize.InsertString(0, _T("Nomal"));
	f_WamSize.InsertString(1, _T("Small"));
	f_WamSize.InsertString(2, _T("Large"));

	f_SearchMode3D.InsertString(0, _T("GlobalMax"));
	f_SearchMode3D.InsertString(1, _T("FirstLocalMax"));

	f_RangeAxis.InsertString(0, _T("Standard"));
	f_RangeAxis.InsertString(1, _T("Reverse"));

	f_SearchDirection.InsertString(0, _T("Standard"));
	f_SearchDirection.InsertString(1, _T("Reverse"));

	f_PixelFormat.InsertString(0, _T("Mono12p"));
	f_PixelFormat.InsertString(1, _T("Coord3D_C12p"));
	f_PixelFormat.InsertString(2, _T("Mono16"));
	f_PixelFormat.InsertString(3, _T("Coord3D_C16"));

	f_MultiSlopeMode.InsertString(0, _T("Off"));
	f_MultiSlopeMode.InsertString(1, _T("PresetSoft"));
	f_MultiSlopeMode.InsertString(2, _T("PresetMedium"));
	f_MultiSlopeMode.InsertString(3, _T("PresetAggressive"));

	f_EventNotification.InsertString(0, _T("On"));
	f_EventNotification.InsertString(1, _T("Off"));

	f_ReflectanceFilter.InsertString(0, _T("true"));
	f_ReflectanceFilter.InsertString(1, _T("false"));

	f_LineSelector.InsertString(0, _T("RS422A"));
	f_LineSelector.InsertString(1, _T("RS422B"));

	f_LineFormat.InsertString(0, _T("RS422"));

	f_DeviceScanType.InsertString(0, _T("Linescan3D"));
	f_DeviceScanType.InsertString(1, _T("AreaScan"));

	f_AcquisitionMode.InsertString(0, _T("SingleFrame"));
	f_AcquisitionMode.InsertString(0, _T("Continuous"));

	//温度显示初始化
	f_TemSel.SetCurSel(0);
	int iCameraIndex = 0;
	LE_STATUS eResult = LCC_C_SetFeatureEnum(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_SICK_TEM_SELECTOR, 0);
	if (eResult != L_OK)
	{
		AfxMessageBox("设置为温度Sensor失败！");
	}
	ReadTemperature();

	//图像属性初始化
	f_RegionSel.SetCurSel(0);
	eResult = LCC_C_SetFeatureEnum(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_SICK_REGION_SELECTOR, 0);
	if (eResult != L_OK)
	{
		AfxMessageBox("设置为Region0模式失败！");
	}
	GetDlgItem(IDC_RegionDescription)->SetWindowText("SensorImage");

	GetDlgItem(IDC_RegionDescription)->EnableWindow(FALSE);
	GetDlgItem(IDC_ExposureTime)->EnableWindow(TRUE);
	GetDlgItem(IDC_Width)->EnableWindow(FALSE);
	GetDlgItem(IDC_Height)->EnableWindow(FALSE);
	GetDlgItem(IDC_OffsetY)->EnableWindow(FALSE);
	GetDlgItem(IDC_OffsetX)->EnableWindow(FALSE);

	GetDlgItem(IDC_Width)->SetWindowText("2560");
	GetDlgItem(IDC_Height)->SetWindowText("832");
	GetDlgItem(IDC_OffsetX)->SetWindowText("0");
	GetDlgItem(IDC_OffsetY)->SetWindowText("0");

	ReadExposureTime();
	ReadFrameRate();
	ReadLineRate();

	eResult = LCC_C_SetFeatureEnum(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_SICK_COMPONENT_SELECTOR, 0);
	ReadTestPattern();

	//触发模式初始化
	//f_TriggerModeSelector.SetCurSel(1);
	//eResult = LCC_C_SetFeatureEnum(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_SICK_TRIGGER_MODE_SELECTOR, 1);
	/*if (eResult != L_OK)
	{
		AfxMessageBox("设置为LineStart触发模式失败！");
	}*/
	ReadTriggerMode();
	ReadTriggerActivation();


	//存图模式初始化
	ReadComponentEnable();

	//ExtractionScan3d初始化
	f_ExtractionScan3d.SetCurSel(0);
	ReadWamSize();
	ReadSearchMode3D();
	ReadRangeAxis();
	ReadThresHold();
	ReadSearchDirection();
	ReadPixelFormat();
	ReadReflectanceFilter();
	ReadMultiSlopeKneePointCount();

	//初始化
	ReadMultiSlopeMode();
	ReadEncoderDivider();
	ReadEventNotification();
	ReadGevSCPD();

	f_LineSelector.SetCurSel(0);
	eResult = LCC_C_SetFeatureEnum(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_SICK_LINE_SELECTOR, 0);
	ReadLineFormat();

	ReadDeviceScanType();
	ReadAcquisitionMode();

	return TRUE;
}


void FeatureSet::OnCbnSelchangeTemselector()
{
	// TODO: 在此添加控件通知处理程序代码
	int sel = f_TemSel.GetCurSel();

	int iCameraIndex = 0;
	LE_STATUS eResult;

#if 0
	if (sel == 0)
	{
		eResult = LCC_C_SetFeatureEnum(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_SICK_TEM_SELECTOR, 0);
		if (eResult != L_OK)
		{
			AfxMessageBox("设置为温度Sensor失败！");
		}
		ReadTemperature();
	}

	else if (sel == 1)
	{
		eResult = LCC_C_SetFeatureEnum(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_SICK_TEM_SELECTOR, 1);
		if (eResult != L_OK)
		{
			AfxMessageBox("设置为温度Mainboard失败！");
		}
		ReadTemperature();
	}

	else if (sel == 2)
	{
		eResult = LCC_C_SetFeatureEnum(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_SICK_TEM_SELECTOR, 2);
		if (eResult != L_OK)
		{
			AfxMessageBox("设置为温度Sensorboard失败！");
		}
		ReadTemperature();
	}

	else if (sel == 3)
	{
		eResult = LCC_C_SetFeatureEnum(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_SICK_TEM_SELECTOR, 3);
		if (eResult != L_OK)
		{
			AfxMessageBox("设置为温度Zpmsco失败！");
		}
		ReadTemperature();
	}

	else if (sel == 4)
	{
		eResult = LCC_C_SetFeatureEnum(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_SICK_TEM_SELECTOR, 4);
		if (eResult != L_OK)
		{
			AfxMessageBox("设置为温度Zpm1失败！");
		}
		ReadTemperature();
	}

	else if (sel == 5)
	{
		eResult = LCC_C_SetFeatureEnum(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_SICK_TEM_SELECTOR, 5);
		if (eResult != L_OK)
		{
			AfxMessageBox("设置为温度Zpm2失败！");
		}
		ReadTemperature();
	}

	else if (sel == 6)
	{
		eResult = LCC_C_SetFeatureEnum(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_SICK_TEM_SELECTOR, 6);
		if (eResult != L_OK)
		{
			AfxMessageBox("设置为温度Zpm3失败！");
		}
		ReadTemperature();
	}

	else if (sel == 7)
	{
		eResult = LCC_C_SetFeatureEnum(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_SICK_TEM_SELECTOR, 7);
		if (eResult != L_OK)
		{
			AfxMessageBox("设置为温度Zpm4失败！");
		}
		ReadTemperature();
	}

	else if (sel == 8)
	{
		eResult = LCC_C_SetFeatureEnum(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_SICK_TEM_SELECTOR, 8);
		if (eResult != L_OK)
		{
			AfxMessageBox("设置为温度Filter失败！");
		}
		ReadTemperature();
	}
#endif // 0


}


void FeatureSet::OnCbnSelchangeRegionselector()
{
	// TODO: 在此添加控件通知处理程序代码
	int sel = f_RegionSel.GetCurSel();

	int iCameraIndex = 0;
	LE_STATUS eResult;

	if (sel == 0)
	{
		eResult = LCC_C_SetFeatureEnum(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_SICK_REGION_SELECTOR, 0);
		if (eResult != L_OK)
		{
			AfxMessageBox("设置为Region0模式失败！");
		}

		GetDlgItem(IDC_RegionDescription)->SetWindowText("SensorImage");

		GetDlgItem(IDC_RegionDescription)->EnableWindow(FALSE);
		GetDlgItem(IDC_ExposureTime)->EnableWindow(TRUE);
		GetDlgItem(IDC_Width)->EnableWindow(FALSE);
		GetDlgItem(IDC_Height)->EnableWindow(FALSE);
		GetDlgItem(IDC_OffsetY)->EnableWindow(FALSE);
		GetDlgItem(IDC_OffsetX)->EnableWindow(FALSE);

		ReadExposureTime();
		GetDlgItem(IDC_Width)->SetWindowText("2560");
		GetDlgItem(IDC_Height)->SetWindowText("832");
		GetDlgItem(IDC_OffsetX)->SetWindowText("0");
		GetDlgItem(IDC_OffsetY)->SetWindowText("0");

	}
	if (sel == 1)
	{
		eResult = LCC_C_SetFeatureEnum(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_SICK_REGION_SELECTOR, 1);
		if (eResult != L_OK)
		{
			AfxMessageBox("设置为Region1模式失败！");
		}

		GetDlgItem(IDC_RegionDescription)->SetWindowText("3D 1 ROI");

		GetDlgItem(IDC_RegionDescription)->EnableWindow(FALSE);
		GetDlgItem(IDC_ExposureTime)->EnableWindow(TRUE);
		GetDlgItem(IDC_Width)->EnableWindow(TRUE);
		GetDlgItem(IDC_Height)->EnableWindow(TRUE);
		GetDlgItem(IDC_OffsetY)->EnableWindow(TRUE);
		GetDlgItem(IDC_OffsetX)->EnableWindow(TRUE);

		ReadExposureTime();
		ReadWidth();
		ReadHeight();
		ReadOffsetX();
		ReadOffsetY();
	}

	if (sel == 2)
	{
		eResult = LCC_C_SetFeatureEnum(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_SICK_REGION_SELECTOR, 2);
		if (eResult != L_OK)
		{
			AfxMessageBox("设置为Scan3dExtraction1模式失败！");
		}

		GetDlgItem(IDC_RegionDescription)->SetWindowText("3D 1 out");

		GetDlgItem(IDC_RegionDescription)->EnableWindow(FALSE);
		GetDlgItem(IDC_ExposureTime)->EnableWindow(FALSE);
		GetDlgItem(IDC_Width)->EnableWindow(TRUE);
		GetDlgItem(IDC_Height)->EnableWindow(TRUE);
		GetDlgItem(IDC_OffsetY)->EnableWindow(FALSE);
		GetDlgItem(IDC_OffsetX)->EnableWindow(TRUE);

		GetDlgItem(IDC_ExposureTime)->SetWindowText("100");
		ReadWidth();
		ReadHeight();
		ReadOffsetX();
		GetDlgItem(IDC_OffsetY)->SetWindowText("0");

	}

}


void FeatureSet::OnEnKillfocusExposuretime()
{
	// TODO: 在此添加控件通知处理程序代码
	int sel = f_RegionSel.GetCurSel();
	int iCameraIndex = 0;
	LE_STATUS eResult;
	CString str_feature;

	if (sel == 0)
	{
		f_ExposureTime.GetWindowText(str_feature);
		double d_ExposureTime = _ttof(str_feature);

		eResult = LCC_C_SetFeatureEnum(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_SICK_REGION_SELECTOR, 0);
		if (eResult != L_OK)
		{
			AfxMessageBox("设置为Region0模式失败！");
		}
		eResult = LCC_C_SetFeatureDouble(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_EXP_TIME, d_ExposureTime);
		if (eResult != L_OK)
		{
			AfxMessageBox("设置曝光时间失败！");
		}
	}

	if (sel == 1)
	{
		f_ExposureTime.GetWindowText(str_feature);
		double d_ExposureTime = _ttof(str_feature);

		eResult = LCC_C_SetFeatureEnum(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_SICK_REGION_SELECTOR, 1);
		if (eResult != L_OK)
		{
			AfxMessageBox("设置为Region1模式失败！");
		}
		eResult = LCC_C_SetFeatureDouble(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_EXP_TIME, d_ExposureTime);
		if (eResult != L_OK)
		{
			AfxMessageBox("设置曝光时间失败！");
		}
	}


}


void FeatureSet::OnEnKillfocusWidth()
{
	// TODO: 在此添加控件通知处理程序代码
	int sel = f_RegionSel.GetCurSel();
	int iCameraIndex = 0;
	LE_STATUS eResult;
	CString str_feature;

	if (sel == 0)
	{

		GetDlgItem(IDC_Width)->SetWindowText("2560");
		GetDlgItem(IDC_Width)->EnableWindow(FALSE);
	}

	if (sel == 1)
	{
		f_Width.GetWindowText(str_feature);
		int i_width = _ttoi(str_feature);

		eResult = LCC_C_SetFeatureEnum(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_SICK_REGION_SELECTOR, 1);
		if (eResult != L_OK)
		{
			AfxMessageBox("设置为Region1模式失败！");
		}
		eResult = LCC_C_SetFeatureInt(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_WIDTH, i_width);
		if (eResult != L_OK)
		{
			AfxMessageBox("设置width失败！");
		}

		sel = 2;
		GetDlgItem(IDC_Width)->SetWindowText(str_feature);

	}

	if (sel == 2)
	{
		f_Width.GetWindowText(str_feature);
		int i_width = _ttoi(str_feature);

		eResult = LCC_C_SetFeatureEnum(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_SICK_REGION_SELECTOR, 1);

		if (eResult != L_OK)
		{
			AfxMessageBox("设置为Region1模式失败！");
		}
		eResult = LCC_C_SetFeatureInt(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_WIDTH, i_width);
		if (eResult != L_OK)
		{
			AfxMessageBox("设置width失败！");
		}
		sel = 1;
		GetDlgItem(IDC_Width)->SetWindowText(str_feature);
	}

}


void FeatureSet::OnEnKillfocusHeight()
{
	// TODO: 在此添加控件通知处理程序代码
	int sel = f_RegionSel.GetCurSel();
	int iCameraIndex = 0;
	LE_STATUS eResult;
	CString str_feature;

	if (sel == 0)
	{
		GetDlgItem(IDC_Height)->SetWindowText("832");
		GetDlgItem(IDC_Height)->EnableWindow(FALSE);
	}

	if (sel == 1)
	{
		f_Height.GetWindowText(str_feature);
		int i_height = _ttoi(str_feature);

		eResult = LCC_C_SetFeatureEnum(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_SICK_REGION_SELECTOR, 1);
		if (eResult != L_OK)
		{
			AfxMessageBox("设置为Region1模式失败！");
		}
		eResult = LCC_C_SetFeatureInt(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_HEIGHT, i_height);
		if (eResult != L_OK)
		{
			AfxMessageBox("设置height失败！");
		}

		mm_height3D = i_height;//全局变量赋值

	}

	if (sel == 2)
	{
		f_Height.GetWindowText(str_feature);
		int i_height = _ttoi(str_feature);

		eResult = LCC_C_SetFeatureEnum(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_SICK_REGION_SELECTOR, 2);

		if (eResult != L_OK)
		{
			AfxMessageBox("设置为Scan3D模式失败！");
		}
		eResult = LCC_C_SetFeatureInt(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_HEIGHT, i_height);
		if (eResult != L_OK)
		{
			AfxMessageBox("设置height失败！");
		}

	}
}


void FeatureSet::OnEnKillfocusOffsetx()
{
	// TODO: 在此添加控件通知处理程序代码
	int sel = f_RegionSel.GetCurSel();
	int iCameraIndex = 0;
	LE_STATUS eResult;
	CString str_feature;

	if (sel == 0)
	{
		GetDlgItem(IDC_Width)->SetWindowText("0");
		GetDlgItem(IDC_Width)->EnableWindow(FALSE);
	}

	if (sel == 1)
	{
		GetDlgItem(IDC_OffsetX)->EnableWindow(TRUE);
		f_OffsetX.GetWindowText(str_feature);
		int i_offsetX = _ttoi(str_feature);

		eResult = LCC_C_SetFeatureEnum(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_SICK_REGION_SELECTOR, 1);
		if (eResult != L_OK)
		{
			AfxMessageBox("设置为Region1模式失败！");
			return;
		}
		eResult = LCC_C_SetFeatureInt(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_OFFSET_X, i_offsetX);
		if (eResult != L_OK)
		{
			AfxMessageBox("设置offsetX失败！");
			return;
		}
		mm_offsetX3D = i_offsetX;//全局变量赋值

		sel = 2;
		GetDlgItem(IDC_OffsetX)->SetWindowText(str_feature);

	}

	if (sel == 2)
	{
		GetDlgItem(IDC_OffsetX)->EnableWindow(TRUE);
		f_OffsetX.GetWindowText(str_feature);
		int i_offsetX = _ttoi(str_feature);

		eResult = LCC_C_SetFeatureEnum(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_SICK_REGION_SELECTOR, 1);

		if (eResult != L_OK)
		{
			AfxMessageBox("设置为Region1模式失败！");
		}
		eResult = LCC_C_SetFeatureInt(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_OFFSET_X, i_offsetX);
		if (eResult != L_OK)
		{
			AfxMessageBox("设置offsetX失败！");
		}

		mm_offsetX3D = i_offsetX;//全局变量赋值

		sel = 1;
		GetDlgItem(IDC_OffsetX)->SetWindowText(str_feature);
	}
}


void FeatureSet::OnEnKillfocusOffsety()
{
	// TODO: 在此添加控件通知处理程序代码
	int sel = f_RegionSel.GetCurSel();
	int iCameraIndex = 0;
	LE_STATUS eResult;
	CString str_feature;

	if (sel == 0)
	{
		GetDlgItem(IDC_OffsetY)->SetWindowText("0");
		GetDlgItem(IDC_OffsetY)->EnableWindow(FALSE);
	}

	if (sel == 1)
	{
		eResult = LCC_C_SetFeatureEnum(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_SICK_REGION_SELECTOR, 1);
		if (eResult != L_OK)
		{
			AfxMessageBox("设置为Region1模式失败！");
		}

		GetDlgItem(IDC_OffsetY)->EnableWindow(TRUE);

		f_OffsetY.GetWindowText(str_feature);
		int i_OffsetY = _ttoi(str_feature);

		eResult = LCC_C_SetFeatureInt(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_OFFSET_Y, i_OffsetY);
		if (eResult != L_OK)
		{
			AfxMessageBox("设置OffsetY失败！");
			return;
		}
		mm_offsetY3D = i_OffsetY;//全局变量赋值
	}

	if (sel == 2)
	{

		GetDlgItem(IDC_OffsetY)->SetWindowText("0");
		GetDlgItem(IDC_OffsetY)->EnableWindow(FALSE);
	}
}


void FeatureSet::OnEnKillfocusLinerate()
{
	// TODO: 在此添加控件通知处理程序代码
	int iCameraIndex = 0;
	LE_STATUS eResult;
	CString str_feature;

	f_LineRate.GetWindowText(str_feature);
	double d_LineRate = _ttof(str_feature);

	eResult = LCC_C_SetFeatureDouble(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_ACQ_LINE_RATE, d_LineRate);
	if (eResult != L_OK)
	{
		AfxMessageBox("设置LineRate失败！");
	}
}


void FeatureSet::OnEnKillfocusFramerate()
{
	// TODO: 在此添加控件通知处理程序代码
	int iCameraIndex = 0;
	LE_STATUS eResult;
	CString str_feature;

	f_FrameRate.GetWindowText(str_feature);
	double d_FrameRate = _ttof(str_feature);

	eResult = LCC_C_SetFeatureDouble(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_ACQ_FRAME_RATE, d_FrameRate);
	if (eResult != L_OK)
	{
		AfxMessageBox("设置FrameRate失败！");
	}
}


void FeatureSet::OnCbnSelchangeTriggermodeselector()
{
	// TODO: 在此添加控件通知处理程序代码
	int iCameraIndex = 0;
	LE_STATUS eResult;

	int sel = f_TriggerModeSelector.GetCurSel();

	if (sel == 1)
	{
		eResult = LCC_C_SetFeatureEnum(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_SICK_TRIGGER_MODE_SELECTOR, 1);
		if (eResult != L_OK)
		{
			AfxMessageBox("设置为FrameStart触发失败！");
		}
	}
	else if (sel == 0)
	{
		eResult = LCC_C_SetFeatureEnum(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_SICK_TRIGGER_MODE_SELECTOR, 0);
		if (eResult != L_OK)
		{
			AfxMessageBox("设置为LineStart触发失败！");
		}
	}
}


void FeatureSet::OnCbnSelchangeTriggermode()
{
	// TODO: 在此添加控件通知处理程序代码
	int iCameraIndex = 0;
	LE_STATUS eResult;
	int sel = f_TriggerMode.GetCurSel();

	if (sel == 0)
	{
		eResult = LCC_C_SetFeatureEnum(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_SICK_TRIGGER_MODE_SELECTOR, 1);
		eResult = LCC_C_SetFeatureEnum(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_TRIG_MODE, 0);
		if (eResult != L_OK)
		{
			AfxMessageBox("设置为外触发失败！");
		}
	}
	else if (sel == 1)
	{
		eResult = LCC_C_SetFeatureEnum(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_SICK_TRIGGER_MODE_SELECTOR, 1);
		eResult = LCC_C_SetFeatureEnum(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_TRIG_MODE, 1);
		if (eResult != L_OK)
		{
			AfxMessageBox("设置为外触发失败！");
		}
	}

	/*eResult = LCC_C_SetFeatureEnum(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_SICK_TRIGGER_MODE_SELECTOR, 1);
	eResult = LCC_C_SetFeatureEnum(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_TRIG_MODE, 1);
	if (eResult != L_OK)
	{
		AfxMessageBox("设置为外触发失败！");
	}*/

	/*int selSelector = f_TriggerModeSelector.GetCurSel();
	int sel = f_TriggerMode.GetCurSel();

	if (selSelector == 0)
	{
		eResult = LCC_C_SetFeatureEnum(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_SICK_TRIGGER_MODE_SELECTOR, 0);
		if (sel == 1)
		{
			eResult = LCC_C_SetFeatureEnum(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_TRIG_MODE, 1);
			if (eResult != L_OK)
			{
				AfxMessageBox("设置为外触发失败！");
			}
		}
		else if (sel == 0)
		{
			eResult = LCC_C_SetFeatureEnum(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_TRIG_MODE, 0);
			if (eResult != L_OK)
			{
				AfxMessageBox("设置为软触发失败！");
			}
		}
	}
	else if (selSelector == 1)
	{
		eResult = LCC_C_SetFeatureEnum(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_SICK_TRIGGER_MODE_SELECTOR, 1);
		if (sel == 1)
		{
			eResult = LCC_C_SetFeatureEnum(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_TRIG_MODE, 1);
			if (eResult != L_OK)
			{
				AfxMessageBox("设置为外触发失败！");
			}
		}
		else if (sel == 0)
		{
			eResult = LCC_C_SetFeatureEnum(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_TRIG_MODE, 0);
			if (eResult != L_OK)
			{
				AfxMessageBox("设置为软触发失败！");
			}
		}
	}*/

	
}


void FeatureSet::OnCbnSelchangeTriggeractivation()
{
	// TODO: 在此添加控件通知处理程序代码
	int iCameraIndex = 0;
	LE_STATUS eResult;

	int selSelector = f_TriggerModeSelector.GetCurSel();
	int sel = f_TriggerActivation.GetCurSel();

	if (selSelector == 1)
	{
		GetDlgItem(IDC_TriggerActivation)->EnableWindow(FALSE);
	}
	else if (selSelector == 0)
	{
		eResult = LCC_C_SetFeatureEnum(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_SICK_TRIGGER_MODE_SELECTOR, 1);
		if (sel == 1)
		{
			eResult = LCC_C_SetFeatureEnum(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_SICK_TRIGGER_ACTIVATION, 1);
			if (eResult != L_OK)
			{
				AfxMessageBox("设置为LevelHigh失败！");
			}
		}
		else if (sel == 0)
		{
			eResult = LCC_C_SetFeatureEnum(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_SICK_TRIGGER_ACTIVATION, 0);
			if (eResult != L_OK)
			{
				AfxMessageBox("设置为RisingEde失败！");
			}
		}
	}

}



void FeatureSet::OnCbnSelchangeComboComponentenable()
{
	// TODO: 在此添加控件通知处理程序代码
	int iCameraIndex = 0;
	LE_STATUS eResult;
	int sel = f_ComponentEnable.GetCurSel();

	if (sel == 0)
	{
		eResult = LCC_C_SetFeatureBoolean(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_SICK_REFLECTANCE_ENABLE, true);
		if (eResult != L_OK)
		{
			AfxMessageBox("设置为Range+Reflectance模式失败！");
		}
	}
	else if (sel == 1)
	{
		eResult = LCC_C_SetFeatureBoolean(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_SICK_REFLECTANCE_ENABLE, false);
		if (eResult != L_OK)
		{
			AfxMessageBox("设置为Range模式失败！");
		}
	}
}


void FeatureSet::OnCbnSelchangeComboWamsize()
{
	// TODO: 在此添加控件通知处理程序代码
	int iCameraIndex = 0;
	LE_STATUS eResult;
	int sel = f_WamSize.GetCurSel();

	if (sel == 0)
	{
		eResult = LCC_C_SetFeatureEnum(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_SICK_WAM_SIZE, 0);
		if (eResult != L_OK)
		{
			AfxMessageBox("设置为Normal失败！");
		}
	}
	else if (sel == 1)
	{
		eResult = LCC_C_SetFeatureEnum(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_SICK_WAM_SIZE, 1);
		if (eResult != L_OK)
		{
			AfxMessageBox("设置为Small失败！");
		}
	}
	else if (sel == 2)
	{
		eResult = LCC_C_SetFeatureEnum(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_SICK_WAM_SIZE, 2);
		if (eResult != L_OK)
		{
			AfxMessageBox("设置为Large失败！");
		}
	}
	
}


void FeatureSet::OnEnKillfocusThreshold()
{
	// TODO: 在此添加控件通知处理程序代码
	int iCameraIndex = 0;
	LE_STATUS eResult;
	CString str_feature;

	f_ThresHold.GetWindowText(str_feature);
	int i_ThresHold = _ttoi(str_feature);

	eResult = LCC_C_SetFeatureInt(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_SICK_DETECTION_THRESHOLD, i_ThresHold);
	if (eResult != L_OK)
	{
		AfxMessageBox("设置ThresHold失败！");
	}
}


void FeatureSet::OnCbnSelchangeSearchmode3d()
{
	// TODO: 在此添加控件通知处理程序代码
	int iCameraIndex = 0;
	LE_STATUS eResult;
	int sel = f_SearchMode3D.GetCurSel();

	if (sel == 0)
	{
		eResult = LCC_C_SetFeatureEnum(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_SICK_SEARCH_MODE_3D, 0);
		if (eResult != L_OK)
		{
			AfxMessageBox("设置为GlobalMax失败！");
		}
	}
	else if (sel == 1)
	{
		eResult = LCC_C_SetFeatureEnum(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_SICK_SEARCH_MODE_3D, 1);
		if (eResult != L_OK)
		{
			AfxMessageBox("设置为FirstLocalMax失败！");
		}
	}
}


void FeatureSet::OnCbnSelchangeRangeaxis()
{
	// TODO: 在此添加控件通知处理程序代码
	int iCameraIndex = 0;
	LE_STATUS eResult;
	int sel = f_RangeAxis.GetCurSel();

	if (sel == 0)
	{
		eResult = LCC_C_SetFeatureEnum(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_SICK_RANGE_AXIS, 0);
		if (eResult != L_OK)
		{
			AfxMessageBox("设置为Standard失败！");
		}
	}
	else if (sel == 1)
	{
		eResult = LCC_C_SetFeatureEnum(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_SICK_RANGE_AXIS, 1);
		if (eResult != L_OK)
		{
			AfxMessageBox("设置为Reverse失败！");
		}
	}
}


void FeatureSet::OnCbnSelchangeSearchdirection()
{
	// TODO: 在此添加控件通知处理程序代码
	int iCameraIndex = 0;
	LE_STATUS eResult;
	int sel = f_SearchDirection.GetCurSel();

	if (sel == 0)
	{
		eResult = LCC_C_SetFeatureEnum(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_SICK_SEARCH_DIRECTION, 0);
		if (eResult != L_OK)
		{
			AfxMessageBox("设置为Standard失败！");
		}
	}
	else if (sel == 1)
	{
		eResult = LCC_C_SetFeatureEnum(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_SICK_SEARCH_DIRECTION, 1);
		if (eResult != L_OK)
		{
			AfxMessageBox("设置为Reverse失败！");
		}
	}
}


void FeatureSet::OnCbnSelchangePixelformat()
{
	// TODO: 在此添加控件通知处理程序代码
	int iCameraIndex = 0;
	LE_STATUS eResult;
	int sel = f_PixelFormat.GetCurSel();

	if (sel == 0)
	{
		eResult = LCC_C_SetFeatureEnum(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_PIXEL_FORMAT, 0);
		if (eResult != L_OK)
		{
			AfxMessageBox("设置为Standard失败！");
		}
	}
	else if (sel == 1)
	{
		eResult = LCC_C_SetFeatureEnum(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_PIXEL_FORMAT, 1);
		if (eResult != L_OK)
		{
			AfxMessageBox("设置为Reverse失败！");
		}
	}
	else if (sel == 2)
	{
		eResult = LCC_C_SetFeatureEnum(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_PIXEL_FORMAT, 2);
		if (eResult != L_OK)
		{
			AfxMessageBox("设置为Reverse失败！");
		}
	}
	else if (sel == 3)
	{
		eResult = LCC_C_SetFeatureEnum(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_PIXEL_FORMAT, 3);
		if (eResult != L_OK)
		{
			AfxMessageBox("设置为Reverse失败！");
		}
	}
}


void FeatureSet::OnCbnSelchangeTestpattern()
{
	// TODO: 在此添加控件通知处理程序代码
	int selRegion = f_RegionSel.GetCurSel();
	int sel = f_TestPattern.GetCurSel();
	int iCameraIndex = 0;
	LE_STATUS eResult;
	CString str_feature;

	if (selRegion == 0)
	{
		eResult = LCC_C_SetFeatureEnum(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_SICK_REGION_SELECTOR, 0);
		eResult = LCC_C_SetFeatureEnum(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_SICK_COMPONENT_SELECTOR, 0);
		if (sel == 0)
		{
			eResult = LCC_C_SetFeatureEnum(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_SICK_TEST_PATTERN, 0);
		}
		else if (sel == 1)
		{
			eResult = LCC_C_SetFeatureEnum(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_SICK_TEST_PATTERN, 1);
		}
	}

	else if (selRegion == 1)
	{
		GetDlgItem(IDC_TestPattern)->EnableWindow(FALSE);
	}
	else if (selRegion == 2)
	{
		eResult = LCC_C_SetFeatureEnum(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_SICK_REGION_SELECTOR, 2);
		eResult = LCC_C_SetFeatureEnum(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_SICK_COMPONENT_SELECTOR, 1);
		if (sel == 0)
		{
			eResult = LCC_C_SetFeatureEnum(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_SICK_TEST_PATTERN, 0);
		}
		else if (sel == 1)
		{
			eResult = LCC_C_SetFeatureEnum(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_SICK_TEST_PATTERN, 1);
		}
	}
}


void FeatureSet::OnCbnSelchangeMultislopemode()
{
	// TODO: 在此添加控件通知处理程序代码
	int iCameraIndex = 0;
	LE_STATUS eResult;
	int sel = f_MultiSlopeMode.GetCurSel();

	if (sel == 0)
	{
		eResult = LCC_C_SetFeatureEnum(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_SICK_MULTISLOPE_MODE, 0);
		if (eResult != L_OK)
		{
			AfxMessageBox("设置为Off失败！");
		}
	}
	else if (sel == 1)
	{
		eResult = LCC_C_SetFeatureEnum(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_SICK_MULTISLOPE_MODE, 1);
		if (eResult != L_OK)
		{
			AfxMessageBox("设置为PresetSoft失败！");
		}
	}
	else if (sel == 2)
	{
		eResult = LCC_C_SetFeatureEnum(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_SICK_MULTISLOPE_MODE, 2);
		if (eResult != L_OK)
		{
			AfxMessageBox("设置为PresetMedium失败！");
		}
	}
	else if (sel == 3)
	{
		eResult = LCC_C_SetFeatureEnum(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_SICK_MULTISLOPE_MODE, 3);
		if (eResult != L_OK)
		{
			AfxMessageBox("设置为PresetAggressive失败！");
		}
	}
}


void FeatureSet::OnEnKillfocusEncoderdivider()
{
	// TODO: 在此添加控件通知处理程序代码
	int iCameraIndex = 0;
	LE_STATUS eResult;
	CString str_feature;

	f_EncoderDivider.GetWindowText(str_feature);
	int i_feature = _ttoi(str_feature);
	eResult = LCC_C_SetFeatureInt(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_SICK_ENCODER_DIVIDER, i_feature);
	
}


void FeatureSet::OnCbnSelchangeEventnotification()
{
	// TODO: 在此添加控件通知处理程序代码
	int iCameraIndex = 0;
	LE_STATUS eResult;
	int sel = f_EventNotification.GetCurSel();

	if (sel == 0)
	{
		eResult = LCC_C_SetFeatureEnum(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_SICK_EVENT_NOTIFICATION, 0);
		if (eResult != L_OK)
		{
			AfxMessageBox("设置为On失败！");
		}
	}
	else if (sel == 1)
	{
		eResult = LCC_C_SetFeatureEnum(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_SICK_EVENT_NOTIFICATION, 1);
		if (eResult != L_OK)
		{
			AfxMessageBox("设置为Off失败！");
		}
	}
}


void FeatureSet::OnEnKillfocusGevscpd()
{
	// TODO: 在此添加控件通知处理程序代码
	int iCameraIndex = 0;
	LE_STATUS eResult;
	CString str_feature;

	f_GevSCPD.GetWindowText(str_feature);
	int i_feature = _ttoi(str_feature);
	eResult = LCC_C_SetFeatureInt(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_SICK_GEV_SCPD, i_feature);
}


void FeatureSet::OnCbnSelchangeReflectancefilter()
{
	// TODO: 在此添加控件通知处理程序代码
	int iCameraIndex = 0;
	LE_STATUS eResult;
	int sel = f_ReflectanceFilter.GetCurSel();

	if (sel == 0)
	{
		eResult = LCC_C_SetFeatureBoolean(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_SICK_REFLECTANCE_FILTER, true);
		if (eResult != L_OK)
		{
			AfxMessageBox("设置为Reflectancefilter-true模式失败！");
		}
	}
	else if (sel == 1)
	{
		eResult = LCC_C_SetFeatureBoolean(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_SICK_REFLECTANCE_FILTER, false);
		if (eResult != L_OK)
		{
			AfxMessageBox("设置为Reflectancefilter-false模式失败！");
		}
	}
}


void FeatureSet::OnEnKillfocusMultislopekneepointcount()
{
	// TODO: 在此添加控件通知处理程序代码
	int iCameraIndex = 0;
	LE_STATUS eResult;
	CString str_feature;

	f_MultiSlopeKneePointCount.GetWindowText(str_feature);
	int i_feature = _ttoi(str_feature);
	eResult = LCC_C_SetFeatureInt(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_SICK_POINT_COUNT, i_feature);
}


void FeatureSet::OnCbnSelchangeLineselector()
{
	// TODO: 在此添加控件通知处理程序代码
	int iCameraIndex = 0;
	LE_STATUS eResult;
	int sel = f_LineSelector.GetCurSel();

	if (sel == 0)
	{
		eResult = LCC_C_SetFeatureEnum(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_SICK_LINE_SELECTOR, 0);
		if (eResult != L_OK)
		{
			AfxMessageBox("设置为RS422A失败！");
		}
	}
	else if (sel == 1)
	{
		eResult = LCC_C_SetFeatureEnum(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_SICK_LINE_SELECTOR, 1);
		if (eResult != L_OK)
		{
			AfxMessageBox("设置为RS422B失败！");
		}
	}
	else if (sel == 2)
	{
		eResult = LCC_C_SetFeatureEnum(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_SICK_LINE_SELECTOR, 2);
		if (eResult != L_OK)
		{
			AfxMessageBox("设置为FrameTrig失败！");
		}
	}
}


void FeatureSet::OnCbnSelchangeLineformat()
{
	// TODO: 在此添加控件通知处理程序代码
	int iCameraIndex = 0;
	LE_STATUS eResult;
	int selselector = f_LineSelector.GetCurSel();;
	int sel = f_LineFormat.GetCurSel();

	if (selselector == 0)
	{
		eResult = LCC_C_SetFeatureEnum(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_SICK_LINE_SELECTOR, 0);
		if (eResult != L_OK)
		{
			AfxMessageBox("设置为RS422A失败！");
		}
		if (sel == 0)
		{
			eResult = LCC_C_SetFeatureEnum(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_SICK_LINE_FORMAT, 0);
		}
		else if (sel == 1)
		{
			eResult = LCC_C_SetFeatureEnum(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_SICK_LINE_FORMAT, 1);
		}
		
	}
	else if (selselector == 1)
	{
		eResult = LCC_C_SetFeatureEnum(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_SICK_LINE_SELECTOR, 1);
		if (eResult != L_OK)
		{
			AfxMessageBox("设置为RS422B失败！");
		}
		if (sel == 0)
		{
			eResult = LCC_C_SetFeatureEnum(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_SICK_LINE_FORMAT, 0);
		}
		else if (sel == 1)
		{
			eResult = LCC_C_SetFeatureEnum(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_SICK_LINE_FORMAT, 1);
		}
	}
	else if (selselector == 2)
	{
		eResult = LCC_C_SetFeatureEnum(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_SICK_LINE_SELECTOR, 2);
		if (eResult != L_OK)
		{
			AfxMessageBox("设置为FrameTrig失败！");
		}
		if (sel == 0)
		{
			eResult = LCC_C_SetFeatureEnum(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_SICK_LINE_FORMAT, 0);
		}
		else if (sel == 1)
		{
			eResult = LCC_C_SetFeatureEnum(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_SICK_LINE_FORMAT, 1);
		}
	}
}


void FeatureSet::OnCbnSelchangeDevicescantype()
{
	// TODO: 在此添加控件通知处理程序代码
	int iCameraIndex = 0;
	LE_STATUS eResult;
	int sel = f_DeviceScanType.GetCurSel();

	if (sel == 0)
	{
		eResult = LCC_C_SetFeatureEnum(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_SICK_DEVICE_SCAN_TYPE, 0);
		if (eResult != L_OK)
		{
			AfxMessageBox("设置为Linescan3D失败！");
		}
	}
	else if (sel == 1)
	{
		eResult = LCC_C_SetFeatureEnum(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_SICK_DEVICE_SCAN_TYPE, 1);
		if (eResult != L_OK)
		{
			AfxMessageBox("设置为AreaScan失败！");
		}
	}
}


void FeatureSet::OnCbnSelchangeAcquisitionmode()
{
	// TODO: 在此添加控件通知处理程序代码
	int iCameraIndex = 0;
	LE_STATUS eResult;
	int sel = f_AcquisitionMode.GetCurSel();

	if (sel == 0)
	{
		eResult = LCC_C_SetFeatureEnum(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_SICK_ACQUISITION_MODE, 0);
		if (eResult != L_OK)
		{
			AfxMessageBox("设置为Continuous失败！");
		}
	}
	else if (sel == 1)
	{
		eResult = LCC_C_SetFeatureEnum(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_SICK_ACQUISITION_MODE, 1);
		if (eResult != L_OK)
		{
			AfxMessageBox("设置为SingleFrame失败！");
		}
	}
}


//读取图像爆光
void FeatureSet::ReadExposureTime()
{
	int iCameraIndex = 0;
	LST_DOUBLE_INFO stDouble;
	LE_STATUS eResult = LCC_C_GetFeatureDouble(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_EXP_TIME, &stDouble);
	if (eResult != L_OK)
	{
		AfxMessageBox("获取图像爆光失败！");
	}

	CString c_ExposureTime;
	c_ExposureTime.Format("%f", stDouble.dValue);
	f_ExposureTime.SetWindowText(c_ExposureTime);
}

//读取图像帧频
void FeatureSet::ReadFrameRate()
{
	int iCameraIndex = 0;
	LST_DOUBLE_INFO stDouble;
	LE_STATUS eResult = LCC_C_GetFeatureDouble(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_ACQ_FRAME_RATE, &stDouble);
	if (eResult != L_OK)
	{
		AfxMessageBox("获取图像帧频失败！");
	}

	CString c_FrameRate;
	c_FrameRate.Format("%f", stDouble.dValue);
	f_FrameRate.SetWindowText(c_FrameRate);
}

//读取图像行频
void FeatureSet::ReadLineRate()
{
	int iCameraIndex = 0;
	LST_DOUBLE_INFO stDouble;
	LE_STATUS eResult = LCC_C_GetFeatureDouble(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_ACQ_LINE_RATE, &stDouble);
	if (eResult != L_OK)
	{
		AfxMessageBox("获取图像行频失败！");
	}

	CString c_LineRate;
	c_LineRate.Format("%f", stDouble.dValue);
	f_LineRate.SetWindowText(c_LineRate);
}

//读取图像高度
void FeatureSet::ReadHeight()
{
	int iCameraIndex = 0;
	LST_INT_INFO stIntInfo;
	LE_STATUS eResult = LCC_C_GetFeatureInt(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_HEIGHT, &stIntInfo);
	if (eResult != L_OK)
	{
		AfxMessageBox("获取图像高度失败！");
	}

	CString c_Height;
	c_Height.Format("%d", stIntInfo.iValue);
	f_Height.SetWindowText(c_Height);
}

//读取图像宽度
void FeatureSet::ReadWidth()
{
	int iCameraIndex = 0;
	LST_INT_INFO stIntInfo;

	LE_STATUS eResult = LCC_C_GetFeatureInt(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_WIDTH, &stIntInfo);
	if (eResult != L_OK)
	{
		AfxMessageBox("获取图像宽度失败！");
	}

	CString c_Width;
	c_Width.Format("%d", stIntInfo.iValue);
	f_Width.SetWindowText(c_Width);
}

//读取图像OffsetX
void FeatureSet::ReadOffsetX()
{
	int iCameraIndex = 0;
	LST_INT_INFO stIntInfo;
	LE_STATUS eResult = LCC_C_GetFeatureInt(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_OFFSET_X, &stIntInfo);
	if (eResult != L_OK)
	{
		AfxMessageBox("获取图像OffsetX失败！");
	}

	CString c_OffsetX;
	c_OffsetX.Format("%d", stIntInfo.iValue);
	f_OffsetX.SetWindowText(c_OffsetX);
}

//读取图像OffsetY
void FeatureSet::ReadOffsetY()
{
	int iCameraIndex = 0;
	LST_INT_INFO stIntInfo;
	LE_STATUS eResult = LCC_C_GetFeatureInt(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_OFFSET_Y, &stIntInfo);
	if (eResult != L_OK)
	{
		AfxMessageBox("获取图像OffsetY失败！");
	}

	CString c_OffsetY;
	c_OffsetY.Format("%d", stIntInfo.iValue);
	f_OffsetY.SetWindowText(c_OffsetY);
}

//读取温度
void FeatureSet::ReadTemperature()
{
	int iCameraIndex = 0;
	LST_DOUBLE_INFO stDouble;
	LE_STATUS eResult = LCC_C_GetFeatureDouble(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_SICK_DEVICE_TEMPERATURE, &stDouble);
	if (eResult != L_OK)
	{
		AfxMessageBox("获取设备温度失败！");
	}

	CString c_Temperature;
	c_Temperature.Format("%f", stDouble.dValue);
	f_Temperature.SetWindowText(c_Temperature);

}

//获取是否外触发
void FeatureSet::ReadTriggerMode()
{
	/*int sel = f_TriggerMode.GetCurSel();*/
	int iCameraIndex = 0;
	LE_STATUS eResult;
	LST_ENUM_INFO stEnumInfo;

	eResult = LCC_C_SetFeatureEnum(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_SICK_TRIGGER_MODE_SELECTOR, 1);
	eResult = LCC_C_GetFeatureEnum(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_TRIG_MODE, &stEnumInfo);
	if (eResult != L_OK)
	{
		AfxMessageBox("获取触发模式失败！");
	}

	if (stEnumInfo.iValue == 1)//
	{
		f_TriggerMode.SetCurSel(1);
	}
	else if (stEnumInfo.iValue == 0)
	{
		f_TriggerMode.SetCurSel(0);
	}
}

//获取存图模式
void FeatureSet::ReadComponentEnable()
{
	int sel = f_ComponentEnable.GetCurSel();
	int iCameraIndex = 0;
	LE_STATUS eResult;
	LST_BOOLEAN_INFO stBooleanInfo;

	eResult = LCC_C_GetFeatureBoolean(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_SICK_REFLECTANCE_ENABLE, &stBooleanInfo);
	if (eResult != L_OK)
	{
		AfxMessageBox("获取ComponentEnable失败！");
	}

	if (stBooleanInfo.bValue == true)
	{
		f_ComponentEnable.SetCurSel(0);
	}
	else if (stBooleanInfo.bValue == false)
	{
		f_ComponentEnable.SetCurSel(1);
	}

}

//获取Wamsize
void FeatureSet::ReadWamSize()
{
	int sel = f_WamSize.GetCurSel();
	int iCameraIndex = 0;
	LE_STATUS eResult;
	LST_ENUM_INFO stEnumInfo;

	eResult = LCC_C_GetFeatureEnum(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_SICK_WAM_SIZE, &stEnumInfo);
	if (eResult != L_OK)
	{
		AfxMessageBox("获取WamSize失败！");
	}

	if (stEnumInfo.iValue == 0)
	{
		f_WamSize.SetCurSel(0);
	}
	else if (stEnumInfo.iValue == 1)
	{
		f_WamSize.SetCurSel(1);
	}
	else if (stEnumInfo.iValue == 2)
	{
		f_WamSize.SetCurSel(2);
	}
}

//获取SearchMode3D
void FeatureSet::ReadSearchMode3D()
{
	int sel = f_SearchMode3D.GetCurSel();
	int iCameraIndex = 0;
	LE_STATUS eResult;
	LST_ENUM_INFO stEnumInfo;

	eResult = LCC_C_GetFeatureEnum(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_SICK_SEARCH_MODE_3D, &stEnumInfo);
	if (eResult != L_OK)
	{
		AfxMessageBox("获取SearchMode3D失败！");
	}

	if (stEnumInfo.iValue == 0)
	{
		f_SearchMode3D.SetCurSel(0);
	}
	else if (stEnumInfo.iValue == 1)
	{
		f_SearchMode3D.SetCurSel(1);
	}
}

//获取RangeAxis
void FeatureSet::ReadRangeAxis()
{
	int sel = f_RangeAxis.GetCurSel();
	int iCameraIndex = 0;
	LE_STATUS eResult;
	LST_ENUM_INFO stEnumInfo;

	eResult = LCC_C_GetFeatureEnum(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_SICK_RANGE_AXIS, &stEnumInfo);
	if (eResult != L_OK)
	{
		AfxMessageBox("获取RangeAxis失败！");
	}

	if (stEnumInfo.iValue == 0)
	{
		f_RangeAxis.SetCurSel(0);
	}
	else if (stEnumInfo.iValue == 1)
	{
		f_RangeAxis.SetCurSel(1);
	}
}

//读取图像ThresHold
void FeatureSet::ReadThresHold()
{
	int iCameraIndex = 0;
	LST_INT_INFO stIntInfo;
	LE_STATUS eResult = LCC_C_GetFeatureInt(CameraControl->m_pCameraHandle[iCameraIndex],LFNC_SICK_DETECTION_THRESHOLD , &stIntInfo);
	if (eResult != L_OK)
	{
		AfxMessageBox("获取图像ThresHold失败！");
	}

	CString c_ThresHold;
	c_ThresHold.Format("%d", stIntInfo.iValue);
	f_ThresHold.SetWindowText(c_ThresHold);
}

//获取SearchDirection
void FeatureSet::ReadSearchDirection()
{
	int sel = f_SearchDirection.GetCurSel();
	int iCameraIndex = 0;
	LE_STATUS eResult;
	LST_ENUM_INFO stEnumInfo;

	eResult = LCC_C_GetFeatureEnum(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_SICK_SEARCH_DIRECTION, &stEnumInfo);
	if (eResult != L_OK)
	{
		AfxMessageBox("获取SearchDirection失败！");
	}

	if (stEnumInfo.iValue == 0)
	{
		f_SearchDirection.SetCurSel(0);
	}
	else if (stEnumInfo.iValue == 1)
	{
		f_SearchDirection.SetCurSel(1);
	}
}

//获取PixelFormat
void FeatureSet::ReadPixelFormat()
{
	int sel = f_PixelFormat.GetCurSel();
	int iCameraIndex = 0;
	LE_STATUS eResult;
	LST_ENUM_INFO stEnumInfo;

	eResult = LCC_C_GetFeatureEnum(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_PIXEL_FORMAT, &stEnumInfo);
	if (eResult != L_OK)
	{
		AfxMessageBox("获取PixelFormat失败！");
	}

	if (stEnumInfo.iValue == 0)
	{
		f_PixelFormat.SetCurSel(0);
	}
	else if (stEnumInfo.iValue == 1)
	{
		f_PixelFormat.SetCurSel(1);
	}
	else if (stEnumInfo.iValue == 2)
	{
		f_PixelFormat.SetCurSel(2);
	}
	else if (stEnumInfo.iValue == 3)
	{
		f_PixelFormat.SetCurSel(3);
	}
}

//获取TriggerModeSelector
void FeatureSet::ReadTriggerModeSelector()
{
	int sel = f_TriggerModeSelector.GetCurSel();
	int iCameraIndex = 0;
	LE_STATUS eResult;
	LST_ENUM_INFO stEnumInfo;

	eResult = LCC_C_GetFeatureEnum(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_SICK_TRIGGER_MODE_SELECTOR, &stEnumInfo);
	if (eResult != L_OK)
	{
		AfxMessageBox("获取TriggerModeSelector失败！");
	}

	if (stEnumInfo.iValue == 0)
	{
		f_TriggerModeSelector.SetCurSel(0);
	}
	else if (stEnumInfo.iValue == 1)
	{
		f_TriggerModeSelector.SetCurSel(1);
	}
}

//获取TriggerActivation
void FeatureSet::ReadTriggerActivation()
{
	int sel = f_TriggerActivation.GetCurSel();
	int iCameraIndex = 0;
	LE_STATUS eResult;
	LST_ENUM_INFO stEnumInfo;

	eResult = LCC_C_GetFeatureEnum(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_SICK_TRIGGER_ACTIVATION, &stEnumInfo);
	if (eResult != L_OK)
	{
		AfxMessageBox("获取TriggerActivation失败！");
	}

	if (stEnumInfo.iValue == 1)
	{
		f_TriggerActivation.SetCurSel(1);
	}
	else if (stEnumInfo.iValue == 0)
	{
		f_TriggerActivation.SetCurSel(0);
	}
}

//获取TestPattern
void FeatureSet::ReadTestPattern()
{
	int sel = f_TestPattern.GetCurSel();
	int iCameraIndex = 0;
	LE_STATUS eResult;
	LST_ENUM_INFO stEnumInfo;

	eResult = LCC_C_GetFeatureEnum(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_SICK_TEST_PATTERN, &stEnumInfo);
	if (eResult != L_OK)
	{
		AfxMessageBox("获取TestPattern失败！");
	}

	if (stEnumInfo.iValue == 1)
	{
		f_TestPattern.SetCurSel(1);
	}
	else if (stEnumInfo.iValue == 0)
	{
		f_TestPattern.SetCurSel(0);
	}
}

//获取MultiSlopeMode
void FeatureSet::ReadMultiSlopeMode()
{
	int sel = f_MultiSlopeMode.GetCurSel();
	int iCameraIndex = 0;
	LE_STATUS eResult;
	LST_ENUM_INFO stEnumInfo;

	eResult = LCC_C_GetFeatureEnum(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_SICK_MULTISLOPE_MODE, &stEnumInfo);
	
	if (stEnumInfo.iValue == 0)
	{
		f_MultiSlopeMode.SetCurSel(0);
	}
	else if (stEnumInfo.iValue == 1)
	{
		f_MultiSlopeMode.SetCurSel(1);
	}
	else if (stEnumInfo.iValue == 2)
	{
		f_MultiSlopeMode.SetCurSel(2);
	}
	else if (stEnumInfo.iValue == 3)
	{
		f_MultiSlopeMode.SetCurSel(3);
	}
}

//读取图像EncoderDivider
void FeatureSet::ReadEncoderDivider()
{
	int iCameraIndex = 0;
	LST_INT_INFO stIntInfo;
	LE_STATUS eResult = LCC_C_GetFeatureInt(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_SICK_ENCODER_DIVIDER, &stIntInfo);
	if (eResult != L_OK)
	{
		AfxMessageBox("获取EncoderDivider失败！");
	}

	CString c_str;
	c_str.Format("%d", stIntInfo.iValue);
	f_EncoderDivider.SetWindowText(c_str);
}

//获取EventNotification
void FeatureSet::ReadEventNotification()
{
	int sel = f_EventNotification.GetCurSel();
	int iCameraIndex = 0;
	LE_STATUS eResult;
	LST_ENUM_INFO stEnumInfo;

	eResult = LCC_C_GetFeatureEnum(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_SICK_EVENT_NOTIFICATION, &stEnumInfo);

	if (stEnumInfo.iValue == 0)
	{
		f_EventNotification.SetCurSel(0);
	}
	else if (stEnumInfo.iValue == 1)
	{
		f_EventNotification.SetCurSel(1);
	}
	
}

//读取图像GevSCPD
void FeatureSet::ReadGevSCPD()
{
	int iCameraIndex = 0;
	LST_INT_INFO stIntInfo;
	LE_STATUS eResult = LCC_C_GetFeatureInt(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_SICK_GEV_SCPD, &stIntInfo);
	if (eResult != L_OK)
	{
		AfxMessageBox("获取GevSCPD失败！");
	}

	CString c_str;
	c_str.Format("%d", stIntInfo.iValue);
	f_GevSCPD.SetWindowText(c_str);
}

//获取ReflectanceFilter
void FeatureSet::ReadReflectanceFilter()
{
	int sel = f_ReflectanceFilter.GetCurSel();
	int iCameraIndex = 0;
	LE_STATUS eResult;
	LST_BOOLEAN_INFO stBooleanInfo;

	eResult = LCC_C_GetFeatureBoolean(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_SICK_REFLECTANCE_FILTER, &stBooleanInfo);
	if (eResult != L_OK)
	{
		AfxMessageBox("获取ReflectanceFilter失败！");
	}

	if (stBooleanInfo.bValue == true)
	{
		f_ReflectanceFilter.SetCurSel(0);
	}
	else if (stBooleanInfo.bValue == false)
	{
		f_ReflectanceFilter.SetCurSel(1);
	}

}

//获取LineFormat
void FeatureSet::ReadLineFormat()
{
	int sel = f_LineFormat.GetCurSel();
	int iCameraIndex = 0;
	LE_STATUS eResult;
	LST_ENUM_INFO stEnumInfo;

	eResult = LCC_C_GetFeatureEnum(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_SICK_LINE_FORMAT, &stEnumInfo);

	if (stEnumInfo.iValue == 0)
	{
		f_LineFormat.SetCurSel(0);
	}
	else if (stEnumInfo.iValue == 1)
	{
		f_LineFormat.SetCurSel(1);
	}
	else if (stEnumInfo.iValue == 2)
	{
		f_LineFormat.SetCurSel(2);
	}
}

//获取DeviceScanType
void FeatureSet::ReadDeviceScanType()
{
	int sel = f_DeviceScanType.GetCurSel();
	int iCameraIndex = 0;
	LE_STATUS eResult;
	LST_ENUM_INFO stEnumInfo;

	eResult = LCC_C_GetFeatureEnum(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_SICK_DEVICE_SCAN_TYPE, &stEnumInfo);

	if (stEnumInfo.iValue == 0)
	{
		f_DeviceScanType.SetCurSel(0);
	}
	else if (stEnumInfo.iValue == 1)
	{
		f_DeviceScanType.SetCurSel(1);
	}
}

//获取AcquisitionMode
void FeatureSet::ReadAcquisitionMode()
{
	int sel = f_AcquisitionMode.GetCurSel();
	int iCameraIndex = 0;
	LE_STATUS eResult;
	LST_ENUM_INFO stEnumInfo;

	eResult = LCC_C_GetFeatureEnum(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_SICK_ACQUISITION_MODE, &stEnumInfo);

	if (stEnumInfo.iValue == 0)
	{
		f_AcquisitionMode.SetCurSel(0);
	}
	else if (stEnumInfo.iValue == 1)
	{
		f_AcquisitionMode.SetCurSel(1);
	}
}

//读取图像MultiSlopeKneePointCount
void FeatureSet::ReadMultiSlopeKneePointCount()
{
	int iCameraIndex = 0;
	LST_INT_INFO stIntInfo;
	LE_STATUS eResult = LCC_C_GetFeatureInt(CameraControl->m_pCameraHandle[iCameraIndex], LFNC_SICK_POINT_COUNT, &stIntInfo);
	if (eResult != L_OK)
	{
		AfxMessageBox("获取MultiSlopeKneePointCount失败！");
	}

	CString c_str;
	c_str.Format("%d", stIntInfo.iValue);
	f_MultiSlopeKneePointCount.SetWindowText(c_str);
}



void FeatureSet::OnBnClickedOk()
{
	// TODO:  在此添加控件通知处理程序代码
	CDialogEx::OnOK();
}
