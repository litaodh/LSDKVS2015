#pragma once
#include "CSelfUpdateTreeCtrl.h"
#include <Ranger3.h>


// SubDialogParameter �Ի���

class SubDialogParameter : public CDialogEx
{
	DECLARE_DYNAMIC(SubDialogParameter)

public:
	SubDialogParameter(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~SubDialogParameter();

	//void InitParameter(RECT rect);
	void InitParameter(RECT rect, R3 * p);
	bool IsClosed();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_PARAMETER };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	void UpdateTab(int _CurSelTab, CRect _rc);

	DECLARE_MESSAGE_MAP()

private:
	CSelfUpdateTreeCtrl m_TreeFile;
	//CTabCtrl m_tab;
	CRect m_rc;
	//CDialog m_page1;
	//CDialog m_page2;
	std::vector<CDialog> pDialog;  //��������Ի������ָ��

	int m_CurSelTab;
	bool m_initialized;
	bool m_isClosed;
	R3SDK::Paramters m_para;


public:
	//afx_msg void OnTcnSelchangeTab(NMHDR *pNMHDR, LRESULT *pResult);
	//afx_msg void OnMove(int x, int y);
	//afx_msg void OnClose();
	//afx_msg void OnTvnSelchangedTreePara(NMHDR *pNMHDR, LRESULT *pResult);
	//afx_msg void OnBnClickedStart();
	//afx_msg void OnBnClickedImport();
	//afx_msg void OnBnClickedExport();
	//afx_msg void OnNMCustomdrawTreePara(NMHDR *pNMHDR, LRESULT *pResult);
};
