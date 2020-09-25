
// LusterCameraControlCDemoDlg.h : 头文件
//

#pragma once
#include "LstLfncDef.h"
#include "LstCameraControl_C.h"
#include <iostream>
#include <stdint.h>

static const int CAMERA_MAX_NUM = 8;//最大相机数量
static const int INVALID_CAMERA_INDEX = -1;	//定义无效相机索引值
#define INVALID_CAM_HANDLE ((void*)-1)

void StoreCallBack(int iCallBackType, void* pCameraHandle, void* pData, void* pUserContext);
void DisplayCallBack(int iCallBackType, void* pCameraHandle, void* pData, void* pUserContext);
void MessageCallBack(int iCallBackType, void* pCameraHandle, void* pData, void* pUserContext);

// CLusterCameraControlCDemoDlg 对话框
class CLusterCameraControlCDemoDlg : public CDialogEx
{
// 构造
public:
	CLusterCameraControlCDemoDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_LUSTERCAMERACONTROLCDEMO_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


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
	afx_msg void OnClose();
	afx_msg void OnBnClickedBtnCamdetectnum();
	afx_msg void OnBnClickedBtnOpencamera();
	afx_msg void OnBnClickedBtnClosecamera();
	afx_msg void OnBnClickedBtnStartgrab();
	afx_msg void OnBnClickedBtnStopgrab();
	afx_msg void OnBnClickedBtnSnap();
	afx_msg void OnBnClickedBtnStartrecord();
	afx_msg void OnBnClickedBtnStoprecord();

	void DrawImage();
	void CreateImage(bool isRange, int imgWidth, int imgHeight, void *pImageBuf, CImage& pImage);
	bool ImageCopy(const CImage &srcImage, CImage &destImage);
	void DrawPicture(int nID, CImage pImage);
	void saveBuffer16(
		const uint8_t* buffer,
		const int64_t bufferWidth,
		const int64_t bufferHeight,
		const int64_t aoiHeight,
		const int64_t aoiOffsetX,
		const int64_t aoiOffsetY,
		const std::string& filePath);

	void saveBufferRangeAndReflectance(
		const uint8_t* buffer16,
		const uint8_t* buffer8,
		const int64_t bufferWidth,
		const int64_t bufferHeight,
		const int64_t aoiHeight,
		const int64_t aoiOffsetX,
		const int64_t aoiOffsetY,
		const std::string& filePath);

	void saveBuffer8(
		const uint8_t* buffer,
		const int64_t bufferWidth,
		const int64_t bufferHeight,
		const int64_t offsetX,
		const int64_t offsetY,
		const std::string& filePath);

	static void GetStatusInfo(IN LE_STATUS eStatus, OUT char* pchInfo);
	static void ShowErrorInfo(LE_STATUS eStatus, char *pchErrorInfo);


	int m_iCameraNum;									//相机总数
	LST_CAMERA_INFO *m_pstCameraInfo[CAMERA_MAX_NUM];	//相机信息结构体
	LST_BUFFER_INFO *m_pstBufferInfo[CAMERA_MAX_NUM];	//采集缓存信息结构体									
	void *m_pCameraHandle[CAMERA_MAX_NUM];				//相机句柄

	unsigned char *m_pBmpInfoPicture;					//界面显示缓冲区
	//ZLBImageStruct *m_pDisplayedImage;					//显示图像结构体
	CDC *m_pDC;											//显示句柄
	CStatic m_PictureBox;								//显示控件
	afx_msg void OnBnClickedBtnFeatureset();

	CButton f_Image, f_Range;
	CEdit f_SavePath;

	afx_msg void OnBnClickedRadioImage();
	afx_msg void OnBnClickedRadioRange();
	afx_msg void OnBnClickedButtonChangesavepath();
};
