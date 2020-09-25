/**	@file	ParametersDlg.cpp
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



#include "stdafx.h"
#include "SampleApiDllTestMFCDlg.h"
#include "SampleApiDllTestMFC.h"
#include "ParametersDlg.h"
#include "afxdialogex.h"


// ParametersDlg

IMPLEMENT_DYNAMIC(ParametersDlg, CDialogEx)

ParametersDlg::ParametersDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_PARAMETER, pParent)
{
	isForceIPWindow = false;
	isDataWindow = false;

	m_pDlg = (CSampleApiDllTestMFCDlg*)pParent;
}

ParametersDlg::~ParametersDlg()
{
}

void ParametersDlg::OnTestParameter()
{
	m_isTest = true;
	//CSampleApiDllTestMFCDlg * pDlg = (CSampleApiDllTestMFCDlg*)this->GetParent();

	// start / stop
	m_pDlg->logTest(CString("1/9 Tesing... OnBnClickedStart")); OnBnClickedStart();	Sleep(2000);
	m_pDlg->logTest(CString("2/9 Tesing... OnBnClickedStart")); OnBnClickedStart();	Sleep(100);

	// parameter setting;
	m_pDlg->logTest(CString("3/9 Tesing... OnBnClickedExport")); OnBnClickedExport();Sleep(1000);

	/// ExtraParameter


	/// UserParameter


	m_pDlg->logTest(CString("4/9 Tesing... OnBnClickedImport")); OnBnClickedImport();Sleep(1000);

	// 

	m_isTest = false;
	AfxMessageBox(CString("ok"));
}

void ParametersDlg::OnTestForceIP()
{
	CSampleApiDllTestMFCDlg * pDlg = (CSampleApiDllTestMFCDlg*)this->GetParent();



	// parameter setting;

	// parameter setting;

	// parameter setting;

	// parameter setting;

	OnBnClickedStart();



	AfxMessageBox(CString("ok"));
}


// ----------------------------------------------------------------------------
// public function


void ParametersDlg::InitParameter(R3 * p, ImageGabber *_pIG)
{
	m_pR3 = p;
	if (m_pR3 == nullptr)	return;
	
	m_pIG = _pIG;
	if (m_pIG == nullptr)	return;

	isParameterWindow = true;
	isForceIPWindow = false;
	isNeedReScane = false;
	isDataWindow = false;

	m_deviceName = CString(m_pR3->getDeviceName().c_str());

	// clear
	int iCount = m_pro.GetPropertyCount();
	CMFCPropertyGridProperty *pDelete;
	for (int i = iCount - 1; i >= 0; i--)
	{
		pDelete = m_pro.GetProperty(i);
		m_pro.DeleteProperty(pDelete);
	}
	m_properties.clear();
	m_propertiesData.clear();
	m_propertiesUser.clear();

	// set buttons
	SetDlgItemText(ID_Start, CString("Start"));

	m_pR3->getAllParameterInfo(m_cate, m_para);
	m_pR3->getAllParameterInfo(m_paraNew);

	HDITEM item;
	item.cxy = 200;
	item.mask = HDI_WIDTH;
	m_pro.GetHeaderCtrl().SetItem(0, new HDITEM(item));
	
	_InitExtraParameter();
	_InitUserParameter();

	for (int i=0; i<m_cate.size(); ++i)
	{
		if (!m_cate[i].empty())
		{
			MFCProperty group = new CMFCPropertyGridProperty(CString(m_cate[i].c_str()));
			m_properties.push_back(group);

			if (m_para.count(m_cate[i]) == 1)
			{
				std::vector<ParaPtr> vParas = m_para.at(m_cate[i]);
				for (auto sub : vParas)
				{
					std::string defaultOpt = sub->getStrValue();
					MFCProperty proper = new CMFCPropertyGridProperty(CString(sub->getName().c_str()), CString(defaultOpt.c_str()));

					proper->SetDescription(CString(sub->getCsvName().c_str()));
					m_properties.push_back(proper);
					group->AddSubItem(proper);
					
					std::vector<Str> vOps = sub->getOptions();
					for (auto opt : vOps)
						proper->AddOption(CString(opt.c_str()));

					proper->AllowEdit(vOps.empty());
				}

				m_pro.AddProperty(group);
			}
		}		
	}

	// edit controls
	SetDlgItemTextW(IDC_EDIT_CALI_PATH, CString(m_pR3->getCalibraPath().c_str()));

	ShowWindow(SW_SHOW);
}

void ParametersDlg::InitForceIP(R3 * p, ImageGabber *_pIG)
{
	m_pR3 = p;
	if (m_pR3 == nullptr) return;

	m_pIG = _pIG;
	if (m_pIG == nullptr) return;

	// set buttons
	SetDlgItemText(ID_Start, CString("Force IP"));
	SetDlgItemText(ID_Save, CString("Invalid"));
	SetDlgItemText(ID_Import, CString("Invalid"));
	SetDlgItemText(ID_Export, CString("Invalid"));
	SetDlgItemText(ID_Calibrate, CString("Invalid"));
	SetDlgItemText(ID_ParamFile, CString("Invalid"));
	SetDlgItemText(ID_SaveImg, CString("Invalid"));

	isParameterWindow = false;
	isForceIPWindow = true;
	isNeedReScane = false;
	isDataWindow = false;

	m_deviceName = CString(m_pR3->getDeviceName().c_str());

	HDITEM item;
	item.cxy = 200;
	item.mask = HDI_WIDTH;
	m_pro.GetHeaderCtrl().SetItem(0, new HDITEM(item));

	_InitUserParameter(true);

	ShowWindow(SW_SHOW);
}

void ParametersDlg::InitImageDataSetting(MFC_IMG * pI)
{
	m_pI = pI;
	if (m_pI == nullptr)		return;

	isParameterWindow = false;
	isForceIPWindow = false;
	isNeedReScane = false;
	isDataWindow = true;

	// set buttons
	SetDlgItemText(ID_Start, CString("OK"));
	SetDlgItemText(ID_Save, CString("Invalid"));
	SetDlgItemText(ID_Import, CString("Invalid"));
	SetDlgItemText(ID_Export, CString("Invalid"));
	SetDlgItemText(ID_Calibrate, CString("Calibration File"));
	SetDlgItemText(ID_ParamFile, CString("Invalid"));
	SetDlgItemText(ID_SaveImg, CString("Save Calibration to"));

	HDITEM item;
	item.cxy = 200;
	item.mask = HDI_WIDTH;
	m_pro.GetHeaderCtrl().SetItem(0, new HDITEM(item));

	m_data.m_vName = {
		CString("Width / Cols"), 
		CString("Height / Rows"), 
		CString("ScaleX"), 
		CString("ScaleY"),
		CString("Lower_bound_x"),
		CString("Upper_bound_x"),
		CString("Calibration File Path"), 
		CString("Save to"),
		CString("aoiHeight"),
		CString("scaleZ"),
		CString("originZ"),
		CString("RectificationMethod"),
		CString("RectificationWidth"),
		CString("missingDataValue"),
		CString("doRectify"),
	};

	m_data.m_vDesc = {
		CString("[Read Only] Image width, unit : pixel."),
		CString("[Read Only] Image height, unit : pixel."),
		CString("[Read Only] Resolution of X, unit : millimeter / pixel."),
		CString("[Read & Write] Resolution of Y, unit : millimeter / pixel."),
		CString("[Read Only] Minimum FOV in X axis, unit : millimeter."),
		CString("[Read Only] Maximum FOV in X axis, unit : millimeter."),
		CString("[Read & Write] Format xml, contain calibration parameters."),
		CString("[Read & Write] The path to save calibrated image. Do not need suffix. Default format is dat|xml."),
		CString("[Read & Write] The height of AOI in sensor image. Unit pixel."),
		CString("[Read & Write] The scale of z in sensor. Unit pixel. If 16p, Default value is 0.0625(1/16)."),
		CString("[Read & Write] The origin point of Z in sensor. Unit pixel. Defalut value is 0"),
		CString("[Read & Write] RectificationMethod."),
		CString("[Read & Write] RectificationWidth. Default is 2560."),
		CString("[Read & Write] The value filled in the empty pixels of the Region image."),
		CString("[Read & Write] Temporary parameter. Control to excute Rectification."),
	};

	m_data.m_vOpts = {
		{ CString("") },{ CString("") },{ CString("") },
		{ CString("") },{ CString("") },{ CString("") },
		{ CString("") }, 
		{ CString("") },
		{ CString("") },
		{ CString("") },
		{ CString("") },
		{ CString("0 | Top"), CString("1 | Mean"), CString("2 | Bottom") },
		{ CString("") },
		{ CString("NAN"), CString("-1.0e6"), CString("0.0"), CString("-1.0") },
		{ CString("0"), CString("1") }
	};

	{
		std::stringstream sss;	sss << m_pI->getR_cols();
		m_data.m_vDefV.push_back(CString(sss.str().c_str()));
	}
	{
		std::stringstream sss;	sss << m_pI->getR_rows();
		m_data.m_vDefV.push_back(CString(sss.str().c_str()));
	}
	{
		std::stringstream sss;	sss << m_pI->get_scaleX();
		m_data.m_vDefV.push_back(CString(sss.str().c_str()));
	}
	{
		std::stringstream sss;	sss << m_pI->get_scaleY();
		m_data.m_vDefV.push_back(CString(sss.str().c_str()));
	}
	{
		std::stringstream sss;	sss << m_pI->get_LX();
		m_data.m_vDefV.push_back(CString(sss.str().c_str()));
	} 
	{
		std::stringstream sss;	sss << m_pI->get_UX();
		m_data.m_vDefV.push_back(CString(sss.str().c_str()));
	}
	m_data.m_vDefV.push_back(m_pI->m_caliPath);
	m_data.m_vDefV.push_back(m_pI->m_savePath);
	m_data.m_vDefV.push_back(m_pI->m_aoiHeigh);
	m_data.m_vDefV.push_back(m_pI->m_scaleZ);
	m_data.m_vDefV.push_back(m_pI->m_originZ);
	m_data.m_vDefV.push_back(m_pI->m_RectMeth);
	m_data.m_vDefV.push_back(m_pI->m_RectWidt);
	m_data.m_vDefV.push_back(m_pI->m_missData);
	m_data.m_vDefV.push_back(m_pI->m_doRectfy);

	
	m_data.m_vIsRW = { 0,0,0,1,0,0, 
		!m_pI->has(SICK_DN::RAN_CAL), !m_pI->has(SICK_DN::RAN_CAL), !m_pI->has(SICK_DN::RAN_CAL),
		!m_pI->has(SICK_DN::RAN_CAL), !m_pI->has(SICK_DN::RAN_CAL), !m_pI->has(SICK_DN::RAN_CAL),
		!m_pI->has(SICK_DN::RAN_CAL), !m_pI->has(SICK_DN::RAN_CAL), !m_pI->has(SICK_DN::RAN_CAL)
	};

	// draw
	MFCProperty group = new CMFCPropertyGridProperty(CString("DataParameters"));
	m_propertiesData.push_back(group);
	for (size_t i = 0; i<m_data.m_vName.size(); ++i)
	{
		MFCProperty proper = new CMFCPropertyGridProperty(m_data.m_vName[i], m_data.m_vDefV[i]);

		proper->SetDescription(m_data.m_vDesc[i]);
		m_propertiesData.push_back(proper);

		if (i == 6) m_proCalibrationPath = proper;
		if (i == 7) m_proSaveCalibratedDataPath = proper;


		group->AddSubItem(proper);

		if (!(m_data.m_vOpts[i].size() == 1 && m_data.m_vOpts[i][0] == CString("")))
		{
			for (auto opt : m_data.m_vOpts[i])
				proper->AddOption(opt);
		}
		proper->AllowEdit(m_data.m_vIsRW[i]);
		proper->Enable(m_data.m_vIsRW[i]);
	}

	m_pro.AddProperty(group);
	isDataWindow = 1;
}


// ----------------------------------------------------------------------------
// protected


void ParametersDlg::_InitExtraParameter()
{
	// parameter names
	m_extra.m_vName = {
		CString("Path of saving image"),
		CString("Sleep time"),
		CString("Grab rate")
	};

	// parameter descriptions
	m_extra.m_vDesc = {
		CString("Path of saving image."),
		CString("Sleep time (us) in grabbing thread."),
		CString("Purly grabbing speed (Hz), without converting and showing.")
	};

	// parameter defalut values
	///CSampleApiDllTestMFCDlg * pDlg = (CSampleApiDllTestMFCDlg*)this->GetParent();

	m_extra.m_vDefV = std::vector<CString>(m_extra.m_vName.size());
	{
		if (m_pDlg->m_Name_R3PathSaveTo.count(m_deviceName) == 1)
		{
			m_extra.m_vDefV[0] = m_pDlg->m_Name_R3PathSaveTo.at(m_deviceName);
			SetDlgItemTextW(IDC_EDIT_SAVE_NAME, m_extra.m_vDefV[0].IsEmpty() ? + CString("\\img-0"): m_extra.m_vDefV[0]);
		}
		m_extra.m_vDefV[1] = "";
		m_extra.m_vDefV[2] = "";
	}

	// parameter optional values
	m_extra.m_vOpts = {
		{ CString("") },{ CString("") },{ CString("") }
	};

	m_extra.m_vIsRW = {
		0,0,0
	};

	// draw
	MFCProperty group = new CMFCPropertyGridProperty(CString("ExtraParameters"));
	m_properties.push_back(group);
	for (size_t i = 0; i<m_extra.m_vName.size(); ++i)
	{
		MFCProperty proper = new CMFCPropertyGridProperty(m_extra.m_vName[i], m_extra.m_vDefV[i]);
		proper->SetDescription(m_extra.m_vDesc[i]);
		
		if (i == 0) m_proPathSavingImage = proper;
		if (i == 1) m_proSleepTime = proper;
		if (i == 2) m_proGrabRate = proper;

		group->AddSubItem(proper);

		if (!(m_extra.m_vOpts[i].size() == 1 && m_extra.m_vOpts[i][0] == CString("")))
		{
			for (auto opt : m_extra.m_vOpts[i])
				proper->AddOption(opt);
		}
		proper->AllowEdit(m_extra.m_vIsRW[i]);
		proper->Enable(m_extra.m_vIsRW[i]);
	}

	m_pro.AddProperty(group);
}

void ParametersDlg::_InitUserParameter(bool is_forceIP_GUI)
{
	// parameter names
	m_user.m_vName = {
		CString("00.RectificationMethod"), 
		CString("01.MissingData"),	
		CString("02.TimeOut"),	
		CString("03.ScaleY"), 
		CString("04.Mac"),
		CString("05.PersisitentIP"),
		CString("06.PersisitentSubnetMask"), 
		CString("07.CurrentIP"), 
		CString("08.CurrentSubnetMask"), 
		CString("09.doRectify"),
		CString("10.BufferCount")
	};

	// parameter descriptions
	m_user.m_vDesc = {
		CString("00.RectificationMethod"),
		CString("01.The value filled in the empty pixels of the Region image."),
		CString("02.Maximun waiting time for one buffer."),
		CString("03.Or resolution of Y. "),
		CString("04.Mac address, fixed. "),
		CString("05.Ip address, editable only when connected. It will be kept when power off."),
		CString("06.Subne mask, editable only when connected. It will be kept when power off."),
		CString("07.Current Ip address. Fixed in parameter windows for showing current IP. Editable in Force_IP window for setting a temporary IP which will be lost when power off. Note: Persistent IP can only be set when connected!"),
		CString("08.Current Subne mask. Fixed in parameter windows for showing current Subnet. Editable in Force_IP window for setting a temporary Subnet which will be lost when power off. Note: Persistent Subnet can only be set when connected!"),
		CString("09.Temporary parameter. Control to excute Rectification."),
		CString("10.Buffer number to initialize the camera. The default is 20, it means that the camera can only hold no more than 20 images. If 21st comes and 1st still does not be grabbed, 1st will be replaced by 21st.")
	};

	// parameter defalut values
	m_user.m_vDefV = std::vector<CString>(m_user.m_vName.size());
	{
		RectMethod m = m_pR3->getRectMethos();
		switch (m)
		{
		case SiCaliWrapper::Top:
			m_user.m_vDefV[0] = CString("0 | Top");
			break;
		case SiCaliWrapper::Mean:
			m_user.m_vDefV[0] = CString("1 | Mean");
			break;
		case SiCaliWrapper::Bottom:
			m_user.m_vDefV[0] = CString("2 | Bottom");
			break;
		}

		float missing = m_pR3->getMissingData();
		if (isnan(missing))
			m_user.m_vDefV[1] = CString("NAN");
		else if (missing == -1.0e6)
			m_user.m_vDefV[1] = CString("-1.0e6");
		else if (missing == 0.0)
			m_user.m_vDefV[1] = CString("0.0");
		else if (missing == -1.0)
			m_user.m_vDefV[1] = CString("-1.0");
		else
			m_user.m_vDefV[1] = CString("ERROR");

		m_user.m_vDefV[2] = CString(std::to_string(m_pR3->getTimeOut()).c_str());
		m_user.m_vDefV[3] = CString(std::to_string(m_pR3->getScaleY()).c_str());
		m_user.m_vDefV[4] = CString(m_pR3->getMac().c_str());
		m_user.m_vDefV[5] = is_forceIP_GUI ? CString("") : CString(m_pR3->getIp(true).c_str());
		m_user.m_vDefV[6] = is_forceIP_GUI ? CString("") : CString(m_pR3->getSubNet(true).c_str());
		m_user.m_vDefV[7] = CString(m_pR3->getIp(false).c_str());
		m_user.m_vDefV[8] = CString(m_pR3->getSubNet(false).c_str());
		m_user.m_vDefV[9] = CString(std::to_string(m_pR3->getDoRectify()).c_str());
		m_user.m_vDefV[10] = CString(std::to_string(m_pR3->getBufferCount()).c_str());
	}

	// parameter optional values
	m_user.m_vOpts = {
		{ CString("0 | Top"), CString("1 | Mean"), CString("2 | Bottom") },
		{ CString("NAN"), CString("-1.0e6"), CString("0.0"), CString("-1.0") },
		{ CString("") },
		{ CString("") },
		{ m_user.m_vDefV[4] },
		{ (is_forceIP_GUI ? m_user.m_vDefV[5] : CString("")) },
		{ (is_forceIP_GUI ? m_user.m_vDefV[6] : CString("")) },
		{ (!is_forceIP_GUI ? m_user.m_vDefV[7] : CString("")) },
		{ (!is_forceIP_GUI ? m_user.m_vDefV[8] : CString("")) },
		{ CString("0"), CString("1") },
		{ CString("") }
	};

	m_user.m_vIsRW = {
		1,1,1,1,0,
		!is_forceIP_GUI,!is_forceIP_GUI,is_forceIP_GUI,is_forceIP_GUI,
		1,1
	};

	// draw
	MFCProperty group = new CMFCPropertyGridProperty(CString("UserParameters"));
	m_properties.push_back(group);
	size_t begin = !is_forceIP_GUI ? 0 : 7;
	for (size_t i = begin; i<m_user.m_vName.size(); ++i)
	{
		MFCProperty proper = new CMFCPropertyGridProperty(m_user.m_vName[i], m_user.m_vDefV[i]);

		proper->SetDescription(m_user.m_vDesc[i]);
		m_propertiesUser.push_back(proper);

		group->AddSubItem(proper);

		if (!(m_user.m_vOpts[i].size() == 1 && m_user.m_vOpts[i][0] == CString("")))
		{
			for (auto opt : m_user.m_vOpts[i])
				proper->AddOption(opt);
		}
		proper->AllowEdit(m_user.m_vIsRW[i]);
		proper->Enable(m_user.m_vIsRW[i]);
	}

	m_pro.AddProperty(group);
}

void ParametersDlg::_UpdateParameterSettings()
{
	if (m_pR3 == nullptr) return;

	///CSampleApiDllTestMFCDlg * pDlg = (CSampleApiDllTestMFCDlg*)this->GetParent();
	CString strLog;

	// editor control
	{
		CString CaliPath;
		GetDlgItemTextW(IDC_EDIT_CALI_PATH, CaliPath);
		std::string path = CT2A(CaliPath);
		std::stringstream ss; ss << "Set calibration path to : " << path.c_str();
		m_pR3->setCalibraPath(path);
		m_pDlg->log(CString(ss.str().c_str()));
	}

	// update user parameters, get / set
	for (int i = 0; i<m_propertiesUser.size(); ++i)
	{
		MFCProperty pPro = m_propertiesUser[i];

		CString proName = (pPro->GetName());
		CString newValue(pPro->GetValue().bstrVal);
		CString oriValue(pPro->GetOriginalValue().bstrVal);

		bool is_updated = newValue != oriValue;
		if (is_updated)
		{
			strLog += (CString("Set ") + proName + CString(" as ") + newValue);
			Ecode e = Ecode::All_OK;

			// extra parameters

			// device parameters
			if (proName == m_user.m_vName[0])
			{
				if (newValue == m_user.m_vOpts[0][1])			m_pR3->setRectMethos(RectMethod::Mean);
				else if (newValue == m_user.m_vOpts[0][2])		m_pR3->setRectMethos(RectMethod::Bottom);
				else											
					m_pR3->setRectMethos(RectMethod::Top);
			}
			else if (proName == m_user.m_vName[1])
			{
				if (newValue == m_user.m_vOpts[1][1])					m_pR3->setMissingData(-1.0e6);
				else if (newValue == m_user.m_vOpts[1][2])				m_pR3->setMissingData(0.0);
				else if (newValue == m_user.m_vOpts[1][3])				m_pR3->setMissingData(-1.0);
				else					
					m_pR3->setMissingData(NAN);
			}
			else if (proName == m_user.m_vName[2])
			{
				m_pR3->setTimeOut(atoi(CT2A(newValue)));
			}
			else if (proName == m_user.m_vName[3])
			{
				m_pR3->setScaleY(atof(CT2A(newValue)));
			}
			else if (proName == m_user.m_vName[4])
			{
				// mac can not be set.
			}
			else if (proName == m_user.m_vName[5])
			{
				e = m_pR3->setIp(std::string(CT2A(newValue)), true);
				isNeedReScane = true;
			}
			else if (proName == m_user.m_vName[6])
			{
				e = m_pR3->setSubnet(std::string(CT2A(newValue)), true);
				isNeedReScane = true;
			}
			else if (proName == m_user.m_vName[7])
			{
				e = m_pR3->setIp(std::string(CT2A(newValue)), false);
			}
			else if (proName == m_user.m_vName[8])
			{
				e = m_pR3->setSubnet(std::string(CT2A(newValue)), false);
			}
			else if (proName == m_user.m_vName[9])
			{
				m_pR3->setDoRectify(std::string(CT2A(newValue)) == "1");
			}
			else if (proName == m_user.m_vName[10])
			{
				m_pR3->setBufferCount(atoi(CT2A(newValue)));
			}

			std::stringstream ss; ss << ", Execution Code (0 is ok) = " << e;
			strLog += CString(ss.str().c_str()) + CString("\r\n");
		}
	}

	if (isForceIPWindow)
	{
		m_pDlg->log(strLog);
		return;
	}

	// update device parameters
	for (int i = 0; i<m_properties.size(); ++i)
	{
		MFCProperty pPro = m_properties[i];

		std::string csvName = CT2A(pPro->GetDescription());
		std::string newValue = CT2A(CString(pPro->GetValue().bstrVal));
		std::string oriValue = CT2A(CString(pPro->GetOriginalValue().bstrVal));

		bool is_updated = newValue != oriValue;

		if (is_updated && m_paraNew.count(csvName) == 1)
		{
			Ecode e = m_pR3->setParameterValue(csvName, newValue);

			// log updates
			strLog += (CString("Set ") + CString(csvName.c_str()) + CString(" as ") + CString(newValue.c_str()));
			if (e == Ecode::All_OK)
			{
				pPro->SetOriginalValue(CString(newValue.c_str()));
				strLog += CString(" done!\n\r");
			}
			else
			{
				CString excep;
				excep.Format(_T(" failed! Error Code: %d\n\r"), e);
				strLog += excep;
			}
		}
	}

	m_pDlg->log(strLog);
}

void ParametersDlg::_UpdateImageDataSettings()
{
	if (!isDataWindow) return;

	CSampleApiDllTestMFCDlg * pDlg = (CSampleApiDllTestMFCDlg*)this->GetParent();
	CString strLog;

	for (int i = 0; i < m_propertiesData.size(); ++i)
	{
		MFCProperty pPro = m_propertiesData[i];

		CString proName = (pPro->GetName());
		CString newValue(pPro->GetValue().bstrVal);
		CString oriValue(pPro->GetOriginalValue().bstrVal);

		// set 
		bool is_updated = newValue != oriValue;
		if (is_updated)
		{
			strLog += (CString("\r\nSet ") + proName + CString(" as ") + newValue);
			if (proName == m_data.m_vName[3])
				m_pI->set_scaleY(atof(CT2A(newValue)));
			else if (proName == m_data.m_vName[6])			m_pI->m_caliPath	= newValue;
			else if (proName == m_data.m_vName[7])			m_pI->m_savePath	= newValue;
			else if (proName == m_data.m_vName[8])			m_pI->m_aoiHeigh	= newValue;
			else if (proName == m_data.m_vName[9])			m_pI->m_scaleZ		= newValue;
			else if (proName == m_data.m_vName[10])			m_pI->m_originZ		= newValue;
			else if (proName == m_data.m_vName[11])			m_pI->m_RectMeth	= newValue;
			else if (proName == m_data.m_vName[12])			m_pI->m_RectWidt	= newValue;
			else if (proName == m_data.m_vName[13])			m_pI->m_missData	= newValue;
			else if (proName == m_data.m_vName[14])			m_pI->m_doRectfy	= newValue;
		}
	}

	pDlg->log(strLog);

	// calibrate image
	if (!m_pI->has(SICK_DN::RAN_CAL))
	{
		pDlg->log(CString("Start calibrating image, please wait ..."));

		RectMethod rm(RectMethod::Top);
		if (m_pI->m_RectMeth == m_data.m_vOpts[11][1])			rm = (RectMethod::Mean);
		else if (m_pI->m_RectMeth == m_data.m_vOpts[11][2])		rm = (RectMethod::Bottom);
		
		float missingD(NAN);
		if (m_pI->m_missData == m_data.m_vOpts[13][1])			missingD = (-1.0e6);
		else if (m_pI->m_missData == m_data.m_vOpts[13][2])		missingD = (0.0);
		else if (m_pI->m_missData == m_data.m_vOpts[13][3])		missingD = (-1.0);


		pDlg->OnCalibrateImage(
			m_pI->m_caliPath,
			m_pI->m_savePath,
			atoi(CT2A(m_pI->m_aoiHeigh)),
			atof(CT2A(m_pI->m_scaleZ)),
			atof(CT2A(m_pI->m_originZ)),
			rm,
			atoi(CT2A(m_pI->m_RectWidt)),
			missingD,
			m_pI->m_doRectfy != CString("0"),
			m_pI->get_path()
		);
	}

}

void ParametersDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PRO, m_pro);
}


BEGIN_MESSAGE_MAP(ParametersDlg, CDialogEx)
	ON_BN_CLICKED(ID_Start, &ParametersDlg::OnBnClickedStart)
	ON_WM_CLOSE()
	ON_BN_CLICKED(ID_Calibrate, &ParametersDlg::OnBnClickedCalibrate)
	ON_BN_CLICKED(ID_Import, &ParametersDlg::OnBnClickedImport)
	ON_BN_CLICKED(ID_Export, &ParametersDlg::OnBnClickedExport)
	ON_BN_CLICKED(ID_SaveImg, &ParametersDlg::OnBnClickedSaveimgPath)
	ON_BN_CLICKED(ID_Save, &ParametersDlg::OnBnClickedSave)
	ON_BN_CLICKED(ID_ParamFile, &ParametersDlg::OnBnClickedParamfile)
	ON_BN_CLICKED(ID_Test, &ParametersDlg::OnBnClickedTest)
END_MESSAGE_MAP()


// ParametersDlg 消息处理程序


void ParametersDlg::OnBnClickedStart()
{
	///CSampleApiDllTestMFCDlg * pDlg = (CSampleApiDllTestMFCDlg*)this->GetParent();
	CString currentStr;
	GetDlgItem(ID_Start)->GetWindowTextW(currentStr);
	
	if (isForceIPWindow)
	{
		if (currentStr == CString("Force IP"))
			_UpdateParameterSettings();
		return;
	}

	if (isDataWindow)
	{
		if (currentStr == CString("OK"))
			_UpdateImageDataSettings();
		return;
	}
	
	bool is_start = currentStr != CString("Start");
	if (is_start)
	{
		m_pDlg->OnStopRanger3();
		GetDlgItem(ID_Start)->SetWindowTextW(CString("Start"));

		double nImg = static_cast<double>(m_pR3->getImgID());
		time_t tNow = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		std::stringstream ss;	ss << (nImg / (tNow - m_tBegin));
		m_proGrabRate->SetValue(CString(ss.str().c_str()));
	}
	else
	{
		// get curent settings and set parameters
		_UpdateParameterSettings();

		// If persisent IP setting is enabled, disconnect it and re-scan environment.
		if (isNeedReScane)
		{
			m_pDlg->OnScanDevice();
			m_pDlg->log(CString("Persisent IP setting is enabled, system require to close parameter window!"));
			AfxMessageBox(CString("Persisent IP setting is enabled, system require to close parameter window!"));
			OnClose();
			this->DestroyWindow();
			return;
		}

		// Start
		m_pDlg->OnStartRanger3();
		GetDlgItem(ID_Start)->SetWindowTextW(CString("Stop"));

		// Get grab rate
		m_proSleepTime->SetValue(CString(std::to_string(std::max(0.1*m_pIG->getRunRate(), 1.0)).c_str()));
	
		std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
		m_tBegin = std::chrono::system_clock::to_time_t(now);
	}
}


void ParametersDlg::OnClose()
{
	///CSampleApiDllTestMFCDlg * pDlg = (CSampleApiDllTestMFCDlg*)this->GetParent();
	if (m_pDlg->m_Name_Parameters_WinsowStatus.count(m_deviceName) == 1)
		m_pDlg->m_Name_Parameters_WinsowStatus[m_deviceName] = false;
	
	CDialogEx::OnClose();
}


void ParametersDlg::OnBnClickedCalibrate()
{
	if (isParameterWindow || isDataWindow)
	{
		CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, _T("XML Files(*.xml)|*.xml||"));
		if (IDOK != dlg.DoModal())
			return;

		CString strPath = dlg.GetPathName();
		if (strPath.GetLength() <= 4)
			return;

		///CSampleApiDllTestMFCDlg * pDlg = (CSampleApiDllTestMFCDlg*)this->GetParent();
		std::stringstream ss;
		if (strPath.Mid(strPath.GetLength() - 3, 3) == CString("xml"))
		{
			
			if (isParameterWindow)
				SetDlgItemTextW(IDC_EDIT_CALI_PATH, strPath);
			else
				m_proCalibrationPath->SetValue(strPath);

			std::string path = CT2A(strPath);
			ss << "Load calibration file from : " << path;
		}
		else
			ss << "Load calibration file : path invalid!";

		m_pDlg->log(CString(ss.str().c_str()));
	}
	
}


void ParametersDlg::OnBnClickedImport()
{
	if (!isParameterWindow) return;

	CString strPath;
	if (!m_isTest)
	{
		CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, _T("CSV Files(*.csv)|*.csv||"));
		if (IDOK != dlg.DoModal())
			return;
		strPath = dlg.GetPathName();
	}
	else
		strPath = CString("C:\\paraTest.csv");

	if (strPath.GetLength() <= 4)
		return;
	
	std::stringstream ss;
	///CSampleApiDllTestMFCDlg * pDlg = (CSampleApiDllTestMFCDlg*)this->GetParent();
	std::string path = CT2A(strPath);
	if (strPath.Mid(strPath.GetLength() - 3, 3) == CString("csv"))
	{
		m_pDlg->OnStopRanger3();

		Ecode e = m_pR3->loadParameterFrCSV(path);
		ss << "Import (Load from < " << path.c_str() << " > ) parameter file to device. Execution Code (0 is ok) = " << e;
		// update window
		InitParameter(m_pR3, m_pIG);
		SetDlgItemTextW(IDC_EDIT_PARA_PATH, strPath);
	}
	else
		ss << "Fail to export (Save to file) parameter file. Invalid path : " << path.c_str();

	m_pDlg->log(CString(ss.str().c_str()) + CString("\r\n"));

}


void ParametersDlg::OnBnClickedExport()
{
	if (!isParameterWindow) return;

	CString strPath;
	if (!m_isTest)
	{
		CFileDialog dlg(FALSE, NULL, NULL, OFN_HIDEREADONLY, _T("CSV Files(*.csv)|*.csv||"));
		if (IDOK != dlg.DoModal())
			return;
		strPath = dlg.GetPathName();
	}
	else
		strPath = CString("C:\\paraTest.csv");

	if (strPath.GetLength() <= 4)	return;

	///CSampleApiDllTestMFCDlg * pDlg = (CSampleApiDllTestMFCDlg*)this->GetParent();
	std::string path = CT2A(m_pDlg->CheckSuffixIsAvalible(strPath, CString("csv")));
	Ecode e = m_pR3->saveParameterToCSV(path);
	
	std::stringstream ss;
	ss << "Export (Save to < "<< path.c_str() << " >) parameter file from device. Execution Code (0 is ok) = " << e;
	
	m_pDlg->log(CString(ss.str().c_str()) + CString("\r\n"));
}


void ParametersDlg::OnBnClickedSaveimgPath()
{
	if (isParameterWindow || isDataWindow)
	{
		CFolderPickerDialog dlg(NULL, 0, this, 0);

		if (IDOK != dlg.DoModal())
			return;

		CString strPath = dlg.GetPathName();
		if (strPath.GetLength() <= 4)
			return;

		strPath += CString("\\img-0");

		CSampleApiDllTestMFCDlg * pDlg = (CSampleApiDllTestMFCDlg*)this->GetParent();
		std::stringstream ss;

		if (pDlg->m_Name_R3PathSaveTo.count(m_deviceName) == 1)
			pDlg->m_Name_R3PathSaveTo.at(m_deviceName) = strPath;

		std::string path = CT2A(strPath);
		ss << "Set image saving path to : " << path.c_str();
		pDlg->log(CString(ss.str().c_str()));

		if (isParameterWindow)
		{
			m_proPathSavingImage->SetValue(strPath);
			SetDlgItemTextW(IDC_EDIT_SAVE_NAME, strPath);
		}
		else
			m_proSaveCalibratedDataPath->SetValue(strPath);
	}
}

void ParametersDlg::OnBnClickedSave()
{
	if (!isParameterWindow) return;
	
	// set to main window
	///CSampleApiDllTestMFCDlg * pDlg = (CSampleApiDllTestMFCDlg*)this->GetParent();
	std::stringstream ss;
	ss << "Save online image.";

	CString strName;
	GetDlgItemTextW(IDC_EDIT_SAVE_NAME, strName);
	if (strName.IsEmpty()) 
	{ 
		ss << "\r\n\r\nImage Path is empty!!"; 
		m_pDlg->log(CString(ss.str().c_str()));
		AfxMessageBox(CString(ss.str().c_str()));
		return;
	}

	CString strFullPath = strName;
	ss << "\r\nFull Path is :" << CT2A(strFullPath);
	m_pDlg->log(CString(ss.str().c_str()));
	m_pDlg->saveOnlineImageToPath(strFullPath);
	m_proPathSavingImage->SetValue(strFullPath);

	if (m_pDlg->m_Name_R3PathSaveTo.count(m_deviceName) == 1)
		m_pDlg->m_Name_R3PathSaveTo.at(m_deviceName) = strFullPath;
}

void ParametersDlg::OnBnClickedParamfile()
{
	OnBnClickedImport();
}


// ----------------------------------------------------------------------------
// Test

UINT testParametersDlg(LPVOID lpParam)
{
	auto dlg = (ParametersDlg*)lpParam;
	
	dlg->OnTestParameter();
	return 1;
}

void ParametersDlg::OnBnClickedTest()
{
	//CWinThread * pThread = AfxBeginThread(testParametersDlg, this);
}
