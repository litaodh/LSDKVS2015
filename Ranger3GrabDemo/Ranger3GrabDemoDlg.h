
// Ranger3GrabDemoDlg.h : ͷ�ļ�
//

#pragma once
//#include "LstXmlManagerCF.h"
//#include "LstCameraControl_C.h"
//#include "LstLfncDef.h"
#define CAMERA_MAX_NUM        32		//֧������������	

// CRanger3GrabDemoDlg �Ի���
class CRanger3GrabDemoDlg : public CDialogEx
{
// ����
public:
	CRanger3GrabDemoDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_RANGER3GRABDEMO_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


public:

	//int m_iCameraNum;									//�������
	//LST_CAMERA_INFO *m_pstCameraInfo[CAMERA_MAX_NUM];	//�����Ϣ�ṹ��
	//LST_BUFFER_INFO *m_pstBufferInfo[CAMERA_MAX_NUM];	//�ɼ�������Ϣ�ṹ��									
	//void *m_pCameraHandle[CAMERA_MAX_NUM];				//������

	//unsigned char *m_pBmpInfoPicture;					//������ʾ������
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
};
