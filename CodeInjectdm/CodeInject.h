
// CodeInject.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CCodeInjectApp: 
// �йش����ʵ�֣������ CodeInject.cpp
//

class CCodeInjectApp : public CWinApp
{
public:
	CCodeInjectApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CCodeInjectApp theApp;