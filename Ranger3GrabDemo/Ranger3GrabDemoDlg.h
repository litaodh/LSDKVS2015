
// Ranger3GrabDemoDlg.h : 头文件
//

#pragma once
//#include "LstXmlManagerCF.h"
//#include "LstCameraControl_C.h"
//#include "LstLfncDef.h"
#define CAMERA_MAX_NUM        32		//支持相机最大数量	

// CRanger3GrabDemoDlg 对话框
class CRanger3GrabDemoDlg : public CDialogEx
{
// 构造
public:
	CRanger3GrabDemoDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_RANGER3GRABDEMO_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


public:

	//int m_iCameraNum;									//相机总数
	//LST_CAMERA_INFO *m_pstCameraInfo[CAMERA_MAX_NUM];	//相机信息结构体
	//LST_BUFFER_INFO *m_pstBufferInfo[CAMERA_MAX_NUM];	//采集缓存信息结构体									
	//void *m_pCameraHandle[CAMERA_MAX_NUM];				//相机句柄

	//unsigned char *m_pBmpInfoPicture;					//界面显示缓冲区
// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCheckBtn1();
};
