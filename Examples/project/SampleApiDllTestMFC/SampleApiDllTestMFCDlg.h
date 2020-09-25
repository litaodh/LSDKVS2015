/** ------------------------------------------------------------------------------------------
*	@ File Name		:	SampleApiDllTestMFCDlg.h
*
*	@ Description	:
*		Demo of showing how to call SICK 3D Camera APIs.
*
*	@ Important Notes:
*
*			This Demo program is only used to show how to call SICK 3D Camera APIs.
*			SICK will not guarantee this file is correct!
*			This file is open to customers. 
*			Any programs based on this demo, please keep it correct by yourself.  
*
*
*	@ Copyright		:	Copyright 2016-2020 SICK AG. All rights reserved.
*	@ Author		:	SICK GCN Vision Lab.
*	@ Version		:	2.0.0.0
*	@ Time			:	2019.10.15
* ------------------------------------------------------------------------------------------ */


// todo : calibration file json support --------------
// todo : add C# API ------------------
// tooo : add trispector supporting --------------


#pragma once

#include "ParametersDlg.h"
#include "../../Sample/ApiDllTest/ImageGabber.h"
#include "../../Sample/ApiDllTest/InterfaceOpenCV.h"
#include <Ranger3.h>
#include <opencv2\opencv.hpp>
#include "afxwin.h"

////////////////////////////////////////////////////////////////////////////////

#define id_Scan		WM_USER + 0x010
#define id_Conn		WM_USER + 0x011
#define id_Para		WM_USER + 0x012
#define id_Impo		WM_USER + 0x013
#define id_Expo		WM_USER + 0x014
#define id_Star		WM_USER + 0x015
#define id_Stop		WM_USER + 0x016
#define id_Disc		WM_USER + 0x017
#define id_FoIP		WM_USER + 0x018


#define id_Dele2			WM_USER + 0x021
#define id_Save2_OpenCV_XML	WM_USER + 0x022
#define id_Save2_PC_TXT		WM_USER + 0x023
#define id_Save2_SICK_DAT	WM_USER + 0x024
#define id_dataImageSetting WM_USER + 0x025


#define id_Save3	WM_USER + 0x031
#define id_Dele3	WM_USER + 0x032


enum DlgStatus
{
	OK = 1,
	NO_CTI,
	NO_CAM
};


////////////////////////////////////////////////////////////////////////////////


template<class T> using PTR = std::shared_ptr<T>;
template<class T> using UPTR = std::unique_ptr<T>;

typedef SickCam::ImgT				SICK_DATA;		///< Unified output image data formate.
typedef SickCam::DataNames			SICK_DN;		///< Names of data points, defined by SICK.
typedef std::vector<SICK_DN>		NAME_VEC;		///< Name list of avaliable data pointers, defined by SICK.
typedef std::map<SICK_DN, cv::Mat>	DATA_MAP;


/** @brief Used inside MFC demo	*/
class MFC_IMG
{
public:
	MFC_IMG();

	/** Get Data from SICK_DATA(ImgT), that can be generated from device or file. */
	MFC_IMG(const SICK_DATA & _data, const CString _path);
	
	////////////////////////////////////////////////////////////////////////////////
	// Self-defined Get Functions 
	////////////////////////////////////////////////////////////////////////////////

	CString			get_path		()	const	{ return m_path;					}
	CString			get_name		()	const	{ return m_name;					}
	CString			get_size		()	const;
	cv::Mat			getMat			(SickCam::DataNames _dn)	const;
	NAME_VEC		get_NameList	()	const;
	/** Get ImgT from offline image. Image info is not completed! */
	SICK_DATA		get_SICK_DATA	();


	////////////////////////////////////////////////////////////////////////////////
	// Call functions of SickCam::ImgT()
	////////////////////////////////////////////////////////////////////////////////

	// ----------------------------------------------------------------------------
	// Get Size info
	size_t getS_cols				() const	{ return m_pImg->getS_cols(); };
	size_t getS_rows				() const	{ return m_pImg->getS_rows(); };
	size_t getR_cols				() const	{ return m_pImg->getR_cols(); };
	size_t getR_rows				() const	{ return m_pImg->getR_rows(); };
	size_t getC_cols				() const	{ return m_pImg->getC_cols(); };
	size_t getC_rows				() const	{ return m_pImg->getC_rows(); };
	
	// ----------------------------------------------------------------------------
	// Get Sensor info
	int64_t		get_SenOffsetX	()	const	{ return m_pImg->getS_OffsetX	();}
	int64_t		get_SenOffsetY	()	const	{ return m_pImg->getS_OffsetY	();}

	// ----------------------------------------------------------------------------
	// Get Range info
	int64_t		get_AoiOffsetX	()	const	{ return m_pImg->getR_aoiOffsetX	();}
	int64_t		get_AoiOffsetY	()	const	{ return m_pImg->getR_aoiOffsetY	();}
	int64_t		get_AoiHeight	()	const	{ return m_pImg->getR_aoiHeight	();}
	int64_t		get_AoiWidth	()	const	{ return m_pImg->getR_aoiWidth	();}
	RA			get_RangeAxis	()	const	{ return m_pImg->getR_RangeAxis	();}
	
	// ----------------------------------------------------------------------------
	// Get Calibration info
	double		get_offsetX	()	const		{ return m_pImg->getC_offsetX		();}
	double		get_offsetY	()	const		{ return m_pImg->getC_offsetY		();}
	double		get_scaleX	()	const		{ return m_pImg->getC_scaleX			();}
	double		get_scaleY	()	const		{ return m_pImg->getC_scaleY			();}
	double		get_LX		()	const		{ return m_pImg->getC_lower_bound_x	();}
	double		get_UX		()	const		{ return m_pImg->getC_upper_bound_x	();}
	double		get_LR		()	const		{ return m_pImg->getC_lower_bound_r	();}
	double		get_UR		()	const		{ return m_pImg->getC_upper_bound_r	();}
	
	// ----------------------------------------------------------------------------
	// Get Extra info
	bool		has_ChunkInfo()	const		{ return m_pImg->has_ChunkInfo();	}
	CString		get_BasicInfo()	const		{ return CString(m_pImg->collectInfo().c_str()); }
	
	// ----------------------------------------------------------------------------
	// Get ImgT status
	bool		isEmpty		()				const	{ return m_pImg->isEmpty();	}
	bool		has			(SICK_DN _dn)	const	{ return m_pImg->has(_dn);	}

	void		set_scaleY	(double _d)		{ m_pImg->setC_scaleY(_d); };
	void		set_path	(CString _d)	{ m_path = _d; };


	// ----------------------------------------------------------------------------
	// static functions
	static const std::string	MFC_IMG::get_StrDN	(SICK_DN _dn)				{ return SickCam::ImgT::DN2Str(_dn);	}
	static SICK_DN				MFC_IMG::get_DN		(const std::string _str)	{ return SickCam::ImgT::Str2DN(_str);	}

private:
	SPtr<SickCam::ImgT>		m_pImg;
	SPtr<InterfaceOpenCV>	m_pCV;


	// ----------------------------------------------------------------------------
	// Common variables.
	CString			m_path;
	CString			m_name;

public:
	// ----------------------------------------------------------------------------
	// Parameters used in image data. Set as CString for easier to show in GUI.
	CString			m_caliPath;
	CString			m_savePath;
	CString			m_aoiHeigh;
	CString			m_scaleZ;
	CString			m_originZ;
	CString			m_RectMeth;
	CString			m_RectWidt;
	CString			m_missData;
	CString			m_doRectfy;

};

// CSampleApiDllTestMFCDlg dialog
class CSampleApiDllTestMFCDlg : public CDialogEx
{
// Construction
public:
	CSampleApiDllTestMFCDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SAMPLEAPIDLLTESTMFC_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;
	

	// Generated message map functions

protected:
	virtual BOOL OnInitDialog();
protected:
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	//afx_msg void OnBnClickedButtonLoadimage();
	afx_msg void OnBnClickedButtonCalibration();
	afx_msg void OnDropFiles(HDROP hDropInfo);

	bool								m_isTest;
	bool								m_isStartOK;


protected:
	// ----------------------------------------------------------------------------
	// Self-defined variables definations
	CTreeCtrl							m_ddTreeCtrl;			///< Device & Data
	HTREEITEM							m_hItemRoot;
	HTREEITEM							m_camRoot;
	HTREEITEM							m_dataRoot;
	HTREEITEM							m_sensorRoot;
	HTREEITEM							m_CurrentSelItem;
	CString								m_CurrentSelItemText;

	DlgStatus							m_Status;
	bool								m_isShowRange;
	bool								m_isShowSensor;
	bool								m_isShowDevice;
	bool								m_requireStopTimer;
	bool								m_isStopTimer;


	// image data
	CString								m_currentImgPath;
	std::map<CString, MFC_IMG>			m_path_ImgData;
	std::map<size_t, CString>			m_ID_ImgPaths;

	// sensor data
	CString								m_currentSensorPath;
	std::map<CString, MFC_IMG>			m_path_SensorData;
	std::map<size_t, CString>			m_ID_SensorPath;

	std::map<CString, ParametersDlg *>	m_Name_Parameters;


public:
	std::map<CString, bool>				m_Name_Parameters_WinsowStatus; ///< true, means open
	std::map<CString, PTR<ImageGabber>>	m_Name_Ranger3s;
	std::map<CString, CString>			m_Name_R3PathSaveTo;

protected:	
	// device
	std::map<CString, std::thread * >	m_Name_Thread;
	std::map<size_t, CString>			m_ID_R3Name;
	CString								m_currentR3Name;
	MFC_IMG								m_currentR3Image;
	SICK_DATA							m_currentR3ImgT;
	SICK_DN								m_currentR3ShowType;	///< For online image
	std::map<int, SICK_DN>				m_currentR3AllType;		///< For online image

	PTR<R3S>							m_pR3S;
	UPTR<InterfaceOpenCV>				m_IFCV;					///< Ptr of calling OpenCV related functions 

public:
	void								saveOnlineImageToPath(const CString path);
	CString&							CheckSuffixIsAvalible(CString& path, const CString& subffix);

protected:
	// ----------------------------------------------------------------------------
	// Self-defined functions definations
	BOOL	_LoadSICKImage			(const CString path);
	/** Range | Range & Reflectance | Range_cal | Range_cal & Reflectance_cal | Sensor */
	BOOL	_LoadOpenCV_XML			(const CString _path, std::vector<SICK_DN> _names = std::vector<SICK_DN>());

	void	_showMatImgToWnd		(CWnd * pictureWnd, cv::Mat disimg);
	void	_updateDeviceList		();
	void	_updateDataList			();
	void	_updateSensorList		();
	void	_MyExpandTree			(HTREEITEM hTreeItem);

	void	_ComboImgTypesInsert	(NAME_VEC _nd);
	void	_ComboImgTypesDelete	(NAME_VEC _nd = NAME_VEC());
	void	_ComboImgTypesUpdate	(NAME_VEC _nd);
	void	_ComboImgTypesFocus		(SICK_DN _nd);
	bool	_ComboImgTypesNeedUpdate(NAME_VEC _nd);

	void	_RefleshImageViever		();

public:
	void	log						(CString str);
	void	OnCalibrateImage(CString pathXML, CString pathSave, const int aoiHeight,
		const float scaleZ, const float originZ, const RectMethod method, const size_t rectificationWidth,
		float missingDataValue, const bool _doRectify, CString currentItemName);

protected:
	BOOL	_SaveAsOpenCVXML		(CString _path, MFC_IMG _imgs);
	BOOL	_SaveAsSICKDAT			(MFC_IMG mfc, CString _path, SICK_DN _nd = SICK_DN::INV);

public:
	// ----------------------------------------------------------------------------
	// Control box callback functions, SYSTEM generated
	void			OnBnClickedButtonViewermode	();
	afx_msg void	OnMouseMove					(UINT nFlags, CPoint point);
	afx_msg void	OnBnClickedButtonEzr		();
	afx_msg void	OnNMRClickTreeList			(NMHDR *pNMHDR, LRESULT *pResult);
	void			OnScanDevice				();
	void			OnParameters				();
	void			OnForceIP					();
	void			OnConnectRanger3			();
	void			OnConnectRanger3			(CString R3Name);
	void			OnStartRanger3				();
	void			OnStopRanger3				();
	void			OnDisconnectRanger3			();
	void			OnSave2OpenCV_XML			();
	void			OnSave2PC_TXT				();
	void			OnSave2SICK_DAT				();
	void			OnDataImageSetting			();
	void			OnDelete2					();
	void			OnSave3						();
	void			OnDelete3					();
	afx_msg void	OnTimer						(UINT_PTR nIDEvent);
	afx_msg void	OnNMClickTreeList			(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void	OnNMDblclkTreeList			(NMHDR *pNMHDR, LRESULT *pResult);

	CEdit m_log;
	CComboBox m_Combo_ImgTypes;
	afx_msg void OnCbnSelchangeComboImgtype();
	afx_msg void OnClose();
	afx_msg void OnBnClickedButtonClearlog();

	afx_msg void OnBnClickedButtonSelftest();
	size_t		 getCurrentSelectItem();
	void		 logTest(CString str);
	bool		 doTest();



};
