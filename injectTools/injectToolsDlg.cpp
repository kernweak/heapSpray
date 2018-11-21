
// injectToolsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "injectTools.h"
#include "injectToolsDlg.h"
#include "afxdialogex.h"
//#include ".\injectDll.h"
#include <windows.h>
#include <string>
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//#pragma comment(lib,"injectDll.lib")

// CAboutDlg dialog used for App About

BOOL  EnableDebugPrivilege()
{
	HANDLE hToken;
	BOOL fOk = FALSE;
	if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken)) //Get Token
	{
		TOKEN_PRIVILEGES tp;
		tp.PrivilegeCount = 1;
		if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &tp.Privileges[0].Luid))//Get Luid
			printf("Can't lookup privilege value.\n");
		tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;//这一句很关键，修改其属性为SE_PRIVILEGE_ENABLED
		if (!AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), NULL, NULL))//Adjust Token
			printf("Can't adjust privilege value.\n");
		fOk = (GetLastError() == ERROR_SUCCESS);
		CloseHandle(hToken);
	}
	return fOk;
}





BOOL LoadDll(DWORD dwProcessID, char *szDllPathName) {//第一个参数进程ID，加载dll的完整路径




	EnableDebugPrivilege();
	BOOL bRet;

	HANDLE hProcess;

	HANDLE hThread;

	DWORD dwLength;

	DWORD dwLoadAddr;

	LPVOID lpAllocAddr;

	DWORD dwThreadID;

	HMODULE hModule;

	bRet = 0;

	dwLoadAddr = 0;

	hProcess = 0;

	//1.获取进程句柄
	//PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcessID);

	int a = GetLastError();

	CString STemp;
	STemp.Format(_T("%d"), a);

	if (hProcess == NULL) {

		MessageBox(NULL, STemp, _T("OpenProcess Error"), MB_OK);
		return FALSE;

	}

	//2. 计算DLL路径名字长度，并且要加上0结尾的长度

	dwLength = strlen(szDllPathName) + 1;

	//3. 在目标进程分配内存

	lpAllocAddr = VirtualAllocEx(hProcess, NULL, dwLength, MEM_COMMIT, PAGE_READWRITE);

	if (lpAllocAddr == NULL) {
		MessageBox(NULL, _T("inject explore"), _T("VirtualAllocEx Error!\n"), MB_OK);
		CloseHandle(hProcess);

		return FALSE;

	}

	//4.拷贝DLL路径名字到目标进程的内存

	bRet = WriteProcessMemory(hProcess, lpAllocAddr, szDllPathName, dwLength, NULL);

	if (!bRet) {
		MessageBox(NULL, _T("inject explore"), _T("WriteProcessMemory Error!\n"), MB_OK);
		CloseHandle(hProcess);

		return FALSE;

	}

	//5.因为Kernel32，大家都有，所以从自己进程这获取就行，所以这步是获取模块地址

	hModule = GetModuleHandle("kernel32.dll");

	if (!hModule) {

		MessageBox(NULL, _T("inject explore"), _T("GetModuleHandle Error!\n"), MB_OK);
		CloseHandle(hProcess);

		return FALSE;

	}

	//6.获取LoadLibraryA函数地址

	dwLoadAddr = (DWORD)GetProcAddress(hModule, "LoadLibraryA");

	if (!dwLoadAddr) {
		MessageBox(NULL, _T("inject explore"), _T("GetProcAddress Error!\n"), MB_OK);
		CloseHandle(hProcess);

		return FALSE;

	}

	//7. 创建远程线程，加载Dll

	hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)dwLoadAddr, lpAllocAddr, 0, NULL);
	int a1 = GetLastError();

	CString STemp1;
	STemp1.Format(_T("%d"), a1);


	if (!hThread) {
		MessageBox(NULL, STemp1, _T("CreateRemoteThread Error!\n"), MB_OK);
		CloseHandle(hProcess);

		CloseHandle(hModule);

		return FALSE;

	}

	//8.关闭句柄

	CloseHandle(hProcess);

	return TRUE;

}






class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
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


// CinjectToolsDlg dialog



CinjectToolsDlg::CinjectToolsDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_INJECTTOOLS_DIALOG, pParent)
	, myPID(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CinjectToolsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, myPID);
}

BEGIN_MESSAGE_MAP(CinjectToolsDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CinjectToolsDlg::OnBnClickedOk)
	ON_EN_CHANGE(IDC_EDIT1, &CinjectToolsDlg::OnEnChangeEdit1)
END_MESSAGE_MAP()


// CinjectToolsDlg message handlers

BOOL CinjectToolsDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
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

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CinjectToolsDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CinjectToolsDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CinjectToolsDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


typedef int(*MYFUNC)(void);
void CinjectToolsDlg::OnBnClickedOk()
{
	UpdateData(TRUE);
	int mpid= _ttoi(myPID);
	LoadDll(mpid, "C:\\test\\injectDll.dll");
	// TODO: Add your control notification handler code here

	//HMODULE hMod = LoadLibrary(_T("C:\\test\\injectDll.dll"));//为了方便测试给了相对路径，但是不安全
	//
	//if (hMod == NULL)
	//{
	//	int a1 = GetLastError();
	//	CString STemp1;
	//	STemp1.Format(_T("%d"), a1);
	//	MessageBox(NULL, STemp1, MB_OK);
	//}


	//if (hMod)
	//{
	//	MYFUNC myFunc = (MYFUNC)GetProcAddress(hMod, "funmydll");
	//	if(myFunc)
	//		myFunc();
	//	FreeLibrary(hMod);
	//}
	CDialogEx::OnOK();
}


void CinjectToolsDlg::OnEnChangeEdit1()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}
