// SavePath.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "LusterCameraControlCDemo.h"
#include "SavePath.h"
#include "afxdialogex.h"
#include <string>

extern std::string m_path;
extern std::string m_fileName;


// SavePath �Ի���

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


// SavePath ��Ϣ�������

BOOL SavePath::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	GetDlgItem(IDC_EDIT_SavePath)->SetWindowText(m_path.c_str());
	GetDlgItem(IDC_EDIT_SaveName)->SetWindowText(m_fileName.c_str());

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}


void SavePath::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString strPath;
	CString strName;
	GetDlgItem(IDC_EDIT_SavePath)->GetWindowText(strPath);
	GetDlgItem(IDC_EDIT_SaveName)->GetWindowText(strName);
	if (!PathFileExists(strPath))
	{
		if (CreateDirectoryA(strPath, NULL))
		{
			::MessageBox(NULL, TEXT("�洢·��������,�Ѵ�����"), NULL, MB_OK);//����ԭ���������ý���
			 //strPath.Replace(_T("\\"),_T("/"));
			m_path = strPath;
			GetDlgItem(IDC_EDIT_SavePath)->SetWindowText(m_path.c_str());
		}
		else
		{
			::MessageBox(NULL, TEXT("�洢·��������,����ʧ�ܣ����������ã�"), NULL, MB_OK);//����ԭ���������ý���
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CDialogEx::OnCancel();
}
