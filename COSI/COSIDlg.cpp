// COSIDlg.cpp : implementation file
//

#include "stdafx.h"
#include "COSI.h"
#include "../COSIDLL/COSIDLLinclude.h"
#include "COSIDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

size_t CALLBACK MAIN_FileRead(void *memory, unsigned long size1, unsigned long size2, FILE *infile) {
	return fread(memory, size1, size2, infile);
}

unsigned long FindEndOfParameter(CString intext, const unsigned long start = 0) {
	unsigned long rtrn;

	if (intext.GetAt(start) == '"') {
		rtrn = intext.Find('"', start+1);
		if (rtrn == -1) rtrn = intext.GetLength();
	} else {
		rtrn = intext.Find(' ', start);
		if (rtrn == -1) rtrn = intext.GetLength();
	}

	return rtrn;
}

unsigned char to_BCD(unsigned char x) {
	return (((x / 10) << 4) | (x % 10));
}

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCOSIDlg dialog

CCOSIDlg::CCOSIDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCOSIDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCOSIDlg)
	m_filename = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CCOSIDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCOSIDlg)
	DDX_Control(pDX, IDC_COMPOPTION1, m_compoption1);
	DDX_Control(pDX, IDC_STATUS, m_status);
	DDX_Control(pDX, IDC_PROGRESS, m_progress);
	DDX_Control(pDX, IDC_DECOMPRESS, m_decompressb);
	DDX_Control(pDX, IDC_COMPRESS, m_compressb);
	DDX_Text(pDX, IDC_FILENAME, m_filename);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CCOSIDlg, CDialog)
	//{{AFX_MSG_MAP(CCOSIDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_COMPRESS, OnCompress)
	ON_BN_CLICKED(IDC_DECOMPRESS, OnDecompress)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	ON_WM_TIMER()
	ON_WM_SHOWWINDOW()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCOSIDlg message handlers

BOOL CCOSIDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
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
	//buff = GetCommandLine();

	Lib = LoadLibrary("COSIDLL.dll");
	if (Lib) {
		LoadDLLFunction(Lib, _Name);
		LoadDLLFunction(Lib, _Version);
		LoadDLLFunction(Lib, _DecompressSector);
		LoadDLLFunction(Lib, _CompressSector);
		LoadDLLFunction(Lib, CloseFiles);
		LoadDLLFunction(Lib, SetFiles);
		LoadDLLFunction(Lib, GetModeFileName);
		LoadDLLFunction(Lib, GetDataFileName);
		LoadDLLFunction(Lib, ReadFileSector);
		LoadDLLFunction(Lib, SeekFileSector);
		LoadDLLFunction(Lib, SeekFilePosition);
		LoadDLLFunction(Lib, GetFileCurrentPosition);
		LoadDLLFunction(Lib, GetFileCurrentSector);
		LoadDLLFunction(Lib, InitCOSI);
		LoadDLLFunction(Lib, DeInitCOSI);
		LoadDLLFunction(Lib, Set_FileRead);

		if (DLL__Version)
			DLLversion = DLL__Version();
		else
			DLLversion = -1;

		buff.Format("DLL Version: %i.%i.%i", (DLLversion >> 16) & 0xFF, (DLLversion >> 8) & 0xFF, DLLversion & 0xFF);
		GetDlgItem(IDC_DLLVERSION)->SetWindowText(buff);

		if (DLL_InitCOSI) DLL_InitCOSI();
		if (DLL_Set_FileRead) DLL_Set_FileRead(MAIN_FileRead);
	} else {
		MessageBox("Couldn't find the COSI DLL. Please place it in the same directory as the COSI exe.", "DLL Error");
		
		OnOK();
		return false;
	}
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

BOOL CCOSIDlg::DestroyWindow() 
{
	if (Lib) {
		if (DLL_DeInitCOSI) DLL_DeInitCOSI();
	}

	return CDialog::DestroyWindow();
}

void CCOSIDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CCOSIDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

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
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CCOSIDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CCOSIDlg::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
	
	if (bShow) {
		buff = GetCommandLine();
		currentfile = numfiles = 0;

		a = FindEndOfParameter(buff);
		if (buff.GetAt(a) == '"') {
			a += 2;
		} else a++;

		while (a < (unsigned)buff.GetLength()) {
			b = FindEndOfParameter(buff, a);
			if (buff.GetAt(a) == '"') {
				files[numfiles] = buff.Mid(a+1, b-a-1);
				a = b+2;
			} else {
				files[numfiles] = buff.Mid(a, b-a);
				a = b+1;
			}

			if (GetFileAttributes(files[numfiles]) != -1) {
				numfiles++;
				if (numfiles >= 20) break;
			}
		}

		if (numfiles) {
			m_compoption1.SetCheck(BST_CHECKED);
			RunFiles();
		}
	}
}

void CCOSIDlg::RunFiles() {
	if (!numfiles) return;

	if (currentfile >= numfiles) {
		currentfile = numfiles = 0;

		m_filename = "";
		//GetDlgItem(IDC_FILENAME)->SetWindowText(m_filename);
		m_progress.ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATUS)->ShowWindow(SW_HIDE);
		MessageBox("Done!", "Done");

		DestroyWindow();

		return;
	}

	m_filename = files[currentfile];
	GetDlgItem(IDC_FILENAME)->SetWindowText(m_filename);

	currentfile++;

	if (m_filename.Right(5) == ".mode") {
		OnDecompress();
	} else if ((m_filename.Right(4) == ".img") || (m_filename.Right(4) == ".bin")) {
		OnCompress();
	} else {
		RunFiles();
		return;
	}

	if (!timer)
		RunFiles();
}

void CCOSIDlg::OnBrowse() 
{
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY , "All Acceptable Files|*.img;*.bin;*.mode;*.cosi||\0");
	if (dlg.DoModal() != IDOK) return;

	m_filename = dlg.GetPathName();
	GetDlgItem(IDC_FILENAME)->SetWindowText(m_filename);

	if ((dlg.GetFileExt() == "cosi") || (dlg.GetFileExt() == "mode")) {
		m_compressb.EnableWindow(false);
		m_decompressb.EnableWindow(true);
	} else {
		m_compressb.EnableWindow(true);
		m_decompressb.EnableWindow(false);
	}
}

void CCOSIDlg::OnCompress() 
{
	timer = 0;

	input = fopen(m_filename, "rb");
	if (!input) {
		MessageBox("Couldn't open input file for compressing!", "Error");
		return;
	}

	buff = m_filename;
	
	output1 = fopen(buff + ".data", "wb");
	if (!output1) {
		fclose(input);
		MessageBox("Couldn't open output file 1 (data) for compressing!", "Error");
		return;
	}

	output2 = fopen(buff + ".mode", "wb");
	if (!output2) {
		fclose(input);
		fclose(output1);
		MessageBox("Couldn't open output file 2 (mode) for compressing!", "Error");
		return;
	}

	m_compressb.EnableWindow(false);
	m_decompressb.EnableWindow(false);

	compress = true;
	end = false;
	pos = 0;
	stage = 0;
	m_progress.SetPos(0);

	fseek(input, 0, SEEK_END);
	inputlength = ftell(input);

	m_progress.SetRange32(0, inputlength);

	GetDlgItem(IDC_STATUS)->SetWindowText("");
	m_progress.ShowWindow(SW_SHOW);
	GetDlgItem(IDC_STATUS)->ShowWindow(SW_SHOW);

	fseek(input, 0, SEEK_SET);

	timer = SetTimer(timer, 1, NULL);
}

void CCOSIDlg::OnDecompress() 
{
	timer = 0;

	if (m_filename.Find("cosi") != -1) {
		
	} else {
		output2 = fopen(m_filename, "rb");
		if (!output2) {
			MessageBox("Couldn't open output file 1 for decompressing!", "Error");
			return;
		}

		buff = m_filename;
		buff.GetBufferSetLength(buff.GetLength() - 4);

		output1 = fopen(buff + "data", "rb");
		if (!output1) {
			fclose(output2);
			MessageBox("Couldn't open output file 2 for decompressing!", "Error");
			return;
		}

		buff.GetBufferSetLength(buff.GetLength() - 1);

		input = fopen(buff, "wb");
		if (!input) {
			fclose(output1);
			fclose(output2);
			MessageBox("Couldn't open cd image file for decompressing!", "Error");
			return;
		}

		stage = 1;
	}

	m_compressb.EnableWindow(false);
	m_decompressb.EnableWindow(false);

	end = compress = false;
	posb = pos = 0;
	sectorpos = 75*2;

	m_progress.SetPos(0);

	fseek(output2, 0, SEEK_END);
	inputlength = ftell(output2);

	m_progress.SetRange32(0, inputlength);

	GetDlgItem(IDC_STATUS)->SetWindowText("");
	m_progress.ShowWindow(SW_SHOW);
	GetDlgItem(IDC_STATUS)->ShowWindow(SW_SHOW);

	fseek(output1, 0, SEEK_SET);
	fseek(output2, 0, SEEK_SET);

	timer = SetTimer(timer, 1, NULL);
}

void CCOSIDlg::OnTimer(UINT nIDEvent) 
{
	if (end) return;

	if (compress)	_Compress();
	else			_DeCompress();

	if (end) {
		KillTimer(timer);

		if (numfiles) {
			RunFiles();
		} else {
			m_progress.ShowWindow(SW_HIDE);
			GetDlgItem(IDC_STATUS)->ShowWindow(SW_HIDE);
			MessageBox("Done!", "Done");
		}
		return;
	} else {
		m_progress.SetPos(pos);
		GetDlgItem(IDC_STATUS)->SetWindowText(buff);
	}
	
	CDialog::OnTimer(nIDEvent);
}

void CCOSIDlg::_Compress() {
	for (int i = 0; (i < 17) && (!end); i++) {

	if (stage == 0) {
		//do work here
		
		fread(sector, 1, 2352, input);

		DLL__CompressSector(sector, modedata, sectordata, modesize, datasize, (m_compoption1.GetCheck() > 0));

		fwrite(sectordata, 1, datasize, output1);
		fwrite(modedata, 1, modesize, output2);

		pos += 2352;

		if (pos >= inputlength) {
			stage++;

			//prepare for next stage
		}
	} else if (stage == 1) {
		//combine into 1 file

		//if (done) {
			end = true;

			fclose(input);
			fclose(output1);
			fclose(output2);
		//}
	}

	}//end for

	if (!end) {
		if (stage == 0) buff.Format("Splitting:\r\n%i out of %i", pos, inputlength);
	}
}

void CCOSIDlg::_DeCompress() {
	for (int i = 0; (i < 17) && (!end); i++) {

	if (stage == 0) {
		//decompress the file and split into 2
	} else if (stage == 1) {
		//rejoin the 2 files

		returnsector = DLL__DecompressSector(output2, output1, sectorpos, true);
		if (returnsector) {
			fwrite(returnsector, 1, 2352, input);
		}


		sectorpos++;
		pos = ftell(output2);

		if (pos >= inputlength) {
			end = true;

			fclose(output1);
			fclose(output2);
			fclose(input);
		}
	}

	}//end for

	if (!end) {
		if (stage == 0) buff.Format("DeCompression:\r\n%i out of %i", pos, inputlength);
		else if (stage == 1) buff.Format("ReJoining:\r\n%i out of %i", pos, inputlength);
	}
}
