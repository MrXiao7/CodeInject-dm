
// CodeInjectDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "CodeInject.h"
#include "CodeInjectDlg.h"
#include "afxdialogex.h"
#include "ShowProcessDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

Idmsoft* GetDmObject()
{
    typedef  HRESULT(__stdcall * PDllGetClassObject)(REFCLSID, REFIID, LPVOID*);
    PDllGetClassObject  pDllGetClassObject = NULL;
    HMODULE  h = ::LoadLibrary(_T("dm.dll"));
    if (h == 0)
    {
        AfxMessageBox(_T("Error"));
        return  NULL;
    }
    pDllGetClassObject = (PDllGetClassObject)GetProcAddress(h, "DllGetClassObject");
    if (pDllGetClassObject == NULL)
    {
        return  NULL;
    }
    IClassFactory* pcf = NULL;
    HRESULT hr = pDllGetClassObject(__uuidof(dmsoft), IID_IClassFactory, (void**)&pcf);
    if (SUCCEEDED(hr) && pcf != NULL)
    {
        DWORD* PGetRes = NULL;
        hr = pcf->CreateInstance(NULL, IID_IUnknown, (void**)&PGetRes);
        if (SUCCEEDED(hr) && PGetRes != NULL)
        {
            pcf->Release();
            return (Idmsoft*)PGetRes;
        }
        if (pcf != NULL)
        {
            pcf->Release();
        }
    }
    if (pcf != NULL)
    {
        pcf->Release();
    }
    return  NULL;
}

// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CCodeInjectDlg �Ի���



CCodeInjectDlg::CCodeInjectDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_CODEINJECT_DIALOG, pParent)
    , m_pdm(nullptr)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CCodeInjectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_WINDOWTITLE, m_editWindowTitle);
	DDX_Control(pDX, IDC_EDIT_PID, m_editPid);
	DDX_Control(pDX, IDC_EDIT_ASM, m_editAsm);
	DDX_Control(pDX, IDC_DRAG, m_Pic);
	DDX_Control(pDX, IDC_EDIT_LOG, m_editLog);
}

BEGIN_MESSAGE_MAP(CCodeInjectDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTNINJECTCODE, &CCodeInjectDlg::OnBnClickedBtninjectcode)
	ON_BN_CLICKED(IDC_BTNINJECTDLL, &CCodeInjectDlg::OnBnClickedBtninjectdll)
    ON_BN_CLICKED(IDC_BTNVIEWPROCESS, &CCodeInjectDlg::OnBnClickedBtnviewprocess)
END_MESSAGE_MAP()


// CCodeInjectDlg ��Ϣ�������

BOOL CCodeInjectDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	m_editWindowTitle.SetWindowText(L"�Ϸ�ͼ�굽Ŀ�괰��,��ֱ������PID");
    m_pdm = GetDmObject();
    if (m_pdm->Reg(_T("mrxiao72f7e669e045abc341b148e28fc7dd094"), _T("777")) != 1)
    {
        MessageBox(_T("��Į���ע��ʧ��"));
        exit(0);
    }
    m_pdm->SetShowAsmErrorMsg(1);
    if (!AdjustPr())
    {
        return TRUE;
    }

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CCodeInjectDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CCodeInjectDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CCodeInjectDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CCodeInjectDlg::DebugLog(CString str)
{
	m_editLog.SetSel(-1, -1);
	m_editLog.ReplaceSel(str + "\r\n", 1);
}

void CCodeInjectDlg::DebugErr(CString str)
{
	DebugLog(str);
	MessageBox(str);
}

void CCodeInjectDlg::OnBnClickedBtninjectcode()
{
	//ȡ�ñ༭������
	CString strAsm;
	m_editAsm.GetWindowText(strAsm);
	if (strAsm.IsEmpty())
	{
		DebugLog(L"�����������");
		return;
	}
	m_editLog.SetWindowText(L"");

	int pid = GetDlgItemInt(IDC_EDIT_PID);
	if (pid)
	{
		InjectAsm(pid);
		return;
	}
	DebugErr(L"û��pid");
}


void CCodeInjectDlg::OnBnClickedBtninjectdll()
{
	int pid = GetDlgItemInt(IDC_EDIT_PID);
	if (pid)
	{
		InjectDll(pid);
		return;
	}
	DebugErr(L"û��pid");
}

BOOL CCodeInjectDlg::AdjustPr()
{
	HANDLE hToken;
	LUID sedebugnameValue;
	TOKEN_PRIVILEGES tkp;

	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ALL_ACCESS, &hToken))
	{
		TRACE("OpenProcessTokenִ��ʧ��");
		return FALSE;
	}

	if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &sedebugnameValue))
	{
		CloseHandle(hToken);
		TRACE("LookupPrivilegeValueִ��ʧ��");
		return FALSE;
	}
	tkp.PrivilegeCount = 1;
	tkp.Privileges[0].Luid = sedebugnameValue;
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	if (!AdjustTokenPrivileges(hToken, FALSE, &tkp, sizeof tkp, NULL, NULL))
	{
		TRACE("AdjustTokenPrivilegesִ��ʧ��");
		CloseHandle(hToken);
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

void CCodeInjectDlg::InjectAsm(DWORD pid)
{
	if (m_pdm == nullptr)
	{
        MessageBox(_T("û�д�Į����"));
		return;
	}

    int nAsmCodeLine = m_editAsm.GetLineCount();
    
    for (int i = 0; i < nAsmCodeLine; i++)
    {
        TCHAR szLineAsm[BYTE_MAX] = { 0 };
        int len = m_editAsm.GetLine(i, szLineAsm, BYTE_MAX);
        if (len != 0)
        {
            szLineAsm[len] = 0;
            CString strTmp(szLineAsm);
            strTmp.Replace(_T("0x"),_T(""));
            strTmp.Replace(_T("0X"), _T(""));
            m_pdm->AsmAdd(strTmp.GetBuffer());
        }
        
    }
    CString strTitle;
    m_editWindowTitle.GetWindowText(strTitle);
    HWND hwnd = ::FindWindow(NULL,strTitle.GetBuffer());
    if (hwnd != NULL)//�жϴ��ھ���Ƿ���Ч
    {
        m_pdm->AsmCall((long)hwnd,1);
    }
    m_pdm->AsmClear();
}

void CCodeInjectDlg::TcharToChar(const TCHAR * tchar, char * _char)
{
	int iLength;
	//��ȡ�ֽڳ���   
	iLength = WideCharToMultiByte(CP_ACP, 0, tchar, -1, NULL, 0, NULL, NULL);
	//��tcharֵ����_char    
	WideCharToMultiByte(CP_ACP, 0, tchar, -1, _char, iLength, NULL, NULL);
}

void CCodeInjectDlg::InjectDll(DWORD pid)
{
	TCHAR szFileName[MAX_PATH] = { 0 };

	OPENFILENAME openFileName = { 0 };
	openFileName.lStructSize = sizeof(OPENFILENAME);
	openFileName.lpstrInitialDir = NULL;
	openFileName.nMaxFile = MAX_PATH;
	openFileName.lpstrFilter = L"DLL(*.dll)\0*.dll\0";
	openFileName.lpstrFile = szFileName;
	openFileName.nFilterIndex = 1;
	openFileName.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	if (!GetOpenFileName(&openFileName))
	{
		DebugLog(L"��DLL�ļ�ʧ��");
		return;
	}


	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	if (hProcess == NULL)
	{
		DebugLog(L"�򿪽���ʧ��");
		return;
	}

	LPVOID lpAdd = NULL;
	lpAdd = VirtualAllocEx(hProcess, NULL, MAX_PATH, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (lpAdd == NULL)
	{
		DebugLog(L"�����̷����ڴ�ʧ��");
		return;
	}
	char szPath[MAX_PATH] = { 0 };
	TcharToChar(szFileName, szPath);
	if (!WriteProcessMemory(hProcess, lpAdd, szPath, strlen(szPath)+1, NULL))
	{
		VirtualFreeEx(hProcess, lpAdd, MAX_PATH, MEM_RELEASE);
		DebugLog(L"д��DLL·��ʧ��");
		return;
	}

	HMODULE hModule = GetModuleHandle(L"kernel32.dll");
	
	HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)GetProcAddress(hModule, "LoadLibraryA"), lpAdd, 0, NULL);
	CloseHandle(hThread);
	VirtualFreeEx(hProcess, lpAdd, MAX_PATH, MEM_RELEASE);
}

void CCodeInjectDlg::OnBnClickedBtnviewprocess()
{
    CShowProcessDlg dlg;
    if (dlg.DoModal() == IDOK)
    {
        m_editPid.SetWindowText(dlg.GetSelectPid());
    }
}
