/**	@file	ParametersDlg.h
*
*	@brief	Class ParametersDlg. Parameter setting window.
*
*	@attention
*	Mode 0 - ForceIP window
*			Mode 1 - Parameters window
*
*			Extra Parameter		| Parameters of GUI part, saved in GUI classes.
*			User Parameter		| Caibration and user given parameters, saved in Class Ranger3.
*			Device Parameter	| Vary from firmware version, saved in device
*
*	@copyright	Copyright 2016-2020 SICK AG. All rights reserved.
*	@author		Vision Lab, SICK GCN
*	@version	2.0.0.0
*	@date		2019.10.15
*
*/

#pragma once

#include "afxcmn.h"
#include "CSelfUpdateTreeCtrl.h"
#include "SampleApiDllTestMFCDlg.h"
#include "afxpropertygridctrl.h"

#include <Ranger3.h>
#include <ImageGabber.h>

#include <chrono>


template<class T> using Ptr = std::shared_ptr<T>;

typedef Ptr<SickCam::Parameter>									ParaPtr;
typedef CMFCPropertyGridProperty *								MFCProperty;
typedef std::vector<std::string>								Categories;
typedef std::map<std::string, std::vector<ParaPtr>>				Params;
typedef std::map<std::string, ParaPtr>							AllParams;
typedef std::map<std::string, std::vector<std::string>>			R3_PARAS;
typedef SiCaliWrapper::RectMethod								RectMethod;

class MFC_IMG;

struct Para
{
	std::vector<CString>				m_vName;
	std::vector<CString>				m_vDesc;
	std::vector<CString>				m_vDefV;
	std::vector<std::vector<CString>>	m_vOpts;
	std::vector<bool>					m_vIsRW;
};


class CSampleApiDllTestMFCDlg;

// ParametersDlg
class ParametersDlg : public CDialogEx
{
	DECLARE_DYNAMIC(ParametersDlg)

public:
	ParametersDlg(CWnd* pParent = NULL); 
	virtual ~ParametersDlg();

	void OnTestParameter();
	void OnTestForceIP();


	// ----------------------------------------------------------------------------
	// Extra & User(include online calibration) & Device parameter settings
	void InitParameter(R3 * p, ImageGabber *_pIG);

	// ----------------------------------------------------------------------------
	// Force IP settings
	void InitForceIP(R3 * p, ImageGabber *_pIG);

	// ----------------------------------------------------------------------------
	// Image Data Settings
	void InitImageDataSetting(MFC_IMG * pI);

protected:
	void _InitExtraParameter();
	void _InitUserParameter(bool is_forceIP_GUI=false);
	void _UpdateParameterSettings();

	void _UpdateImageDataSettings();

private:
	Para m_extra;	///< Extra  data
	Para m_user;	///< User   data
	Para m_wind;	///< Device data
	Para m_data;	///< Image  data

	bool m_isTest;


#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_PARAMETER };
#endif

private:
	std::vector<MFCProperty>	m_propertiesUser;
	std::vector<MFCProperty>	m_properties;
	std::vector<MFCProperty>	m_propertiesData;
	MFCProperty					m_proPathSavingImage;
	MFCProperty					m_proSleepTime;
	MFCProperty					m_proGrabRate;
	MFCProperty					m_proCalibrationPath;
	MFCProperty					m_proSaveCalibratedDataPath;

	bool						isParameterWindow;
	bool						isForceIPWindow;
	bool						isNeedReScane;
	bool						isDataWindow;

	CString						m_deviceName;
	time_t						m_tBegin;
	CSampleApiDllTestMFCDlg*	m_pDlg;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV

	DECLARE_MESSAGE_MAP()
public:
	Categories				m_cate;
	Params					m_para;
	AllParams				m_paraNew;
	CMFCPropertyGridCtrl	m_pro;
	R3 *					m_pR3;
	ImageGabber *			m_pIG;
	MFC_IMG *				m_pI;

	afx_msg void OnBnClickedStart();
	afx_msg void OnClose();
	afx_msg void OnBnClickedCalibrate();
	afx_msg void OnBnClickedImport();
	afx_msg void OnBnClickedExport();
	afx_msg void OnBnClickedSaveimgPath();
	/** save online image*/
	afx_msg void OnBnClickedSave();
	afx_msg void OnBnClickedParamfile();






	afx_msg void OnBnClickedTest();
};
