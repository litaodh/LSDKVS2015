// SavePath.cpp : 实现文件
//

#include "stdafx.h"
#include "LusterCameraControlCDemo.h"
#include "SavePath.h"
#include "afxdialogex.h"
#include <string>

extern std::string m_path;
extern std::string m_fileName;


// SavePath 对话框

IMPLEMENT_DYNAMIC(SavePath, CDialogEx)

SavePath::SavePath(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_SavePath, pParent)
{

}

SavePath::~SavePath()
{
}

void SavePath::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	
}


BEGIN_MESSAGE_MAP(SavePath, CDialogEx)
	ON_BN_CLICKED(IDOK, &SavePath::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &SavePath::OnBnClickedCancel)
END_MESSAGE_MAP()


// SavePath 消息处理程序

BOOL SavePath::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	GetDlgItem(IDC_EDIT_SavePath)->SetWindowText(m_path.c_str());
	GetDlgItem(IDC_EDIT_SaveName)->SetWindowText(m_fileName.c_str());

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void SavePath::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strPath;
	CString strName;
	GetDlgItem(IDC_EDIT_SavePath)->GetWindowText(strPath);
	GetDlgItem(IDC_EDIT_SaveName)->GetWindowText(strName);
	if (!PathFileExists(strPath))
	{
		if (CreateDirectoryA(strPath, NULL))
		{
			::MessageBox(NULL, TEXT("存储路径不存在,已创建！"), NULL, MB_OK);//返回原来参数设置界面
			 //strPath.Replace(_T("\\"),_T("/"));
			m_path = strPath;
			GetDlgItem(IDC_EDIT_SavePath)->SetWindowText(m_path.c_str());
		}
		else
		{
			::MessageBox(NULL, TEXT("存储路径不存在,创建失败，请重新设置！"), NULL, MB_OK);//返回原来参数设置界面
			GetDlgItem(IDC_EDIT_SavePath)->SetWindowText(m_path.c_str());
		}
		return;
	}
	else
	{
		//strPath.Replace(_T("\\"),_T("/"));
		m_path = strPath;
	}
	m_fileName = strName;

	CDialogEx::OnOK();
}


void SavePath::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	CDialogEx::OnCancel();
}
