#pragma once


// SavePath �Ի���

class SavePath : public CDialogEx
{
	DECLARE_DYNAMIC(SavePath)

public:
	SavePath(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~SavePath();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
};
