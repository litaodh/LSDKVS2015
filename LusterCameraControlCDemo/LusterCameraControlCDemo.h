
// LusterCameraControlCDemo.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CLusterCameraControlCDemoApp: 
// �йش����ʵ�֣������ LusterCameraControlCDemo.cpp
//

class CLusterCameraControlCDemoApp : public CWinApp
{
public:
	CLusterCameraControlCDemoApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CLusterCameraControlCDemoApp theApp;