#pragma once


// FeatureSet 对话框

class FeatureSet : public CDialogEx
{
	DECLARE_DYNAMIC(FeatureSet)

public:
	FeatureSet(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~FeatureSet();

	CComboBox f_TemSel, f_RegionSel, f_ComponentEnable, f_TriggerModeSelector, f_TriggerMode, f_TriggerActivation;
	CComboBox f_ExtractionScan3d, f_WamSize, f_SearchMode3D, f_RangeAxis, f_SearchDirection, f_PixelFormat;
	CComboBox f_MultiSlopeMode, f_EventNotification, f_TestPattern;
	CComboBox f_ReflectanceFilter, f_DeviceScanType, f_AcquisitionMode, f_LineSelector, f_LineFormat;
	CEdit f_Temperature, f_RegionDes, f_ExposureTime, f_Width, f_Height, f_OffsetX, f_OffsetY;
	CEdit f_LineRate, f_FrameRate, f_ThresHold;
	CEdit f_EncoderDivider, f_GevSCPD, f_MultiSlopeKneePointCount;

	void ReadTriggerMode();
	void ReadTemperature();
	void ReadExposureTime();
	void ReadWidth();
	void ReadHeight();
	void ReadOffsetX();
	void ReadOffsetY();
	void ReadLineRate();
	void ReadFrameRate();
	void ReadComponentEnable();
	void ReadWamSize();
	void ReadSearchMode3D();
	void ReadRangeAxis();
	void ReadSearchDirection();
	void ReadThresHold();
	void ReadPixelFormat();
	void ReadTriggerModeSelector();
	void ReadTriggerActivation();
	void ReadMultiSlopeMode();
	void ReadEventNotification();
	void ReadEncoderDivider();
	void ReadGevSCPD();
	void ReadTestPattern();
	void ReadReflectanceFilter();
	void ReadMultiSlopeKneePointCount();
	void ReadLineFormat();
	void ReadDeviceScanType();
	void ReadAcquisitionMode();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FeatureSet};
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnCbnSelchangeTemselector();
	afx_msg void OnCbnSelchangeRegionselector();
	afx_msg void OnEnKillfocusExposuretime();
	afx_msg void OnEnKillfocusWidth();
	afx_msg void OnEnKillfocusHeight();
	afx_msg void OnEnKillfocusOffsety();
	afx_msg void OnEnKillfocusLinerate();
	afx_msg void OnEnKillfocusFramerate();
	afx_msg void OnCbnSelchangeTriggermode();
	
	afx_msg void OnCbnSelchangeComboComponentenable();
	afx_msg void OnCbnSelchangeComboWamsize();
	afx_msg void OnEnKillfocusThreshold();
	afx_msg void OnCbnSelchangeSearchmode3d();
	afx_msg void OnCbnSelchangeRangeaxis();
	afx_msg void OnCbnSelchangeSearchdirection();
	afx_msg void OnCbnSelchangePixelformat();
	afx_msg void OnCbnSelchangeTriggermodeselector();
	afx_msg void OnCbnSelchangeTriggeractivation();
	afx_msg void OnCbnSelchangeTestpattern();
	afx_msg void OnCbnSelchangeMultislopemode();
	afx_msg void OnEnKillfocusEncoderdivider();
	afx_msg void OnCbnSelchangeEventnotification();
	afx_msg void OnEnKillfocusGevscpd();
	afx_msg void OnCbnSelchangeReflectancefilter();
	afx_msg void OnEnKillfocusMultislopekneepointcount();
	afx_msg void OnCbnSelchangeLineselector();
	afx_msg void OnCbnSelchangeLineformat();
	afx_msg void OnCbnSelchangeDevicescantype();
	afx_msg void OnCbnSelchangeAcquisitionmode();
	afx_msg void OnEnKillfocusOffsetx();
	afx_msg void OnBnClickedOk();
};
