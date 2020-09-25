
// LusterCameraControlCDemoDlg.h : ͷ�ļ�
//

#pragma once
#include "LstLfncDef.h"
#include "LstCameraControl_C.h"
#include <iostream>
#include <stdint.h>

static const int CAMERA_MAX_NUM = 8;//����������
static const int INVALID_CAMERA_INDEX = -1;	//������Ч�������ֵ
#define INVALID_CAM_HANDLE ((void*)-1)

void StoreCallBack(int iCallBackType, void* pCameraHandle, void* pData, void* pUserContext);
void DisplayCallBack(int iCallBackType, void* pCameraHandle, void* pData, void* pUserContext);
void MessageCallBack(int iCallBackType, void* pCameraHandle, void* pData, void* pUserContext);

// CLusterCameraControlCDemoDlg �Ի���
class CLusterCameraControlCDemoDlg : public CDialogEx
{
// ����
public:
	CLusterCameraControlCDemoDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_LUSTERCAMERACONTROLCDEMO_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
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


	int m_iCameraNum;									//�������
	LST_CAMERA_INFO *m_pstCameraInfo[CAMERA_MAX_NUM];	//�����Ϣ�ṹ��
	LST_BUFFER_INFO *m_pstBufferInfo[CAMERA_MAX_NUM];	//�ɼ�������Ϣ�ṹ��									
	void *m_pCameraHandle[CAMERA_MAX_NUM];				//������

	unsigned char *m_pBmpInfoPicture;					//������ʾ������
	//ZLBImageStruct *m_pDisplayedImage;					//��ʾͼ��ṹ��
	CDC *m_pDC;											//��ʾ���
	CStatic m_PictureBox;								//��ʾ�ؼ�
	afx_msg void OnBnClickedBtnFeatureset();

	CButton f_Image, f_Range;
	CEdit f_SavePath;

	afx_msg void OnBnClickedRadioImage();
	afx_msg void OnBnClickedRadioRange();
	afx_msg void OnBnClickedButtonChangesavepath();
};
