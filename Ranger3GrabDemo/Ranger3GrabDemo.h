
// Ranger3GrabDemo.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CRanger3GrabDemoApp: 
// �йش����ʵ�֣������ Ranger3GrabDemo.cpp
//

class CRanger3GrabDemoApp : public CWinApp
{
public:
	CRanger3GrabDemoApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CRanger3GrabDemoApp theApp;