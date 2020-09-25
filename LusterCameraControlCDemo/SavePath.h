#pragma once


// SavePath 对话框

class SavePath : public CDialogEx
{
	DECLARE_DYNAMIC(SavePath)

public:
	SavePath(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~SavePath();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
};
