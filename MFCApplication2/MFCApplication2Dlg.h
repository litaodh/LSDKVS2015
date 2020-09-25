
// MFCApplication2Dlg.h : ͷ�ļ�
//

#pragma once
#include "LstLfncDef.h"
#include "LstCameraControl_C.h"
#include "DatXmlWriter.h"
#include "InterfaceOpenCV.h"


static const int CAMERA_MAX_NUM = 8;//����������
static const int INVALID_CAMERA_INDEX = -1;	//������Ч�������ֵ
#define INVALID_CAM_HANDLE ((void*)-1)

void StoreCallBack(int iCallBackType, void* pCameraHandle, void* pData, void* pUserContext);
void DisplayCallBack(int iCallBackType, void* pCameraHandle, void* pData, void* pUserContext);
void MessageCallBack(int iCallBackType, void* pCameraHandle, void* pData, void* pUserContext);

// CMFCApplication2Dlg �Ի���
class CMFCApplication2Dlg : public CDialogEx
{
// ����
public:
	CMFCApplication2Dlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_MFCAPPLICATION2_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��

public:
	int m_iCameraNum;									//�������
	LST_CAMERA_INFO *m_pstCameraInfo[CAMERA_MAX_NUM];	//�����Ϣ�ṹ��
	LST_BUFFER_INFO *m_pstBufferInfo[CAMERA_MAX_NUM];	//�ɼ�������Ϣ�ṹ��									
	void *m_pCameraHandle[CAMERA_MAX_NUM];				//������
	InterfaceOpenCV pIF;

	void ShowMatImgToWnd(CWnd* pictureWnd, cv::Mat _disimg);
	int count;
	unsigned char *m_pBmpInfoPicture;					//������ʾ������
	std::string SaveImagePath;
	bool IsSaveData;
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
