
// MFCApplication2Dlg.h : 头文件
//

#pragma once
#include "LstLfncDef.h"
#include "LstCameraControl_C.h"
#include "DatXmlWriter.h"
#include "InterfaceOpenCV.h"


static const int CAMERA_MAX_NUM = 8;//最大相机数量
static const int INVALID_CAMERA_INDEX = -1;	//定义无效相机索引值
#define INVALID_CAM_HANDLE ((void*)-1)

void StoreCallBack(int iCallBackType, void* pCameraHandle, void* pData, void* pUserContext);
void DisplayCallBack(int iCallBackType, void* pCameraHandle, void* pData, void* pUserContext);
void MessageCallBack(int iCallBackType, void* pCameraHandle, void* pData, void* pUserContext);

// CMFCApplication2Dlg 对话框
class CMFCApplication2Dlg : public CDialogEx
{
// 构造
public:
	CMFCApplication2Dlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_MFCAPPLICATION2_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

public:
	int m_iCameraNum;									//相机总数
	LST_CAMERA_INFO *m_pstCameraInfo[CAMERA_MAX_NUM];	//相机信息结构体
	LST_BUFFER_INFO *m_pstBufferInfo[CAMERA_MAX_NUM];	//采集缓存信息结构体									
	void *m_pCameraHandle[CAMERA_MAX_NUM];				//相机句柄
	InterfaceOpenCV pIF;

	void ShowMatImgToWnd(CWnd* pictureWnd, cv::Mat _disimg);
	int count;
	unsigned char *m_pBmpInfoPicture;					//界面显示缓冲区
	std::string SaveImagePath;
	bool IsSaveData;
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
	afx_msg void OnBnClickedOpencamBtn1();
	afx_msg void OnBnClickedClosecamBtn1();
	afx_msg void OnBnClickedStartgrabBtn1();
	afx_msg void OnBnClickedStopgrabButton1();
	afx_msg void OnBnClickedParamconfigfileBtn1();
	afx_msg void OnBnClickedCalfileBtn1();
	int m_GrabImageCount;

	afx_msg void OnBnClickedSavepathBtn1();
	afx_msg void OnBnClickedCheck1();
};
