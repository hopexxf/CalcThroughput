// CalcThroughput.cpp : ����Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "CalcThroughput.h"
#include "calc_throughput_alg.h"

#define MAX_LOADSTRING 100

#define MAINWINDOW_WIDTH    400      // �����ڿ��
#define MAINWINDOW_HEIGHT   400      // �����ڸ߶�
#define TEXTBOX_WIDTH       50       // �ı�����
#define TEXTBOX_HEIGHT      20       // �ı���߶�
#define TEXTBOX_MAXLENGTH   32       // �ı������ı�����󳤶�
#define OUTPUT_POS_TOP      210
#define OUTPUT_POS_LEFT     70
#define MAX_INPUT_LEN       5

#define MAIN_NAME_POS        110
#define MAIN_TEXT_LEFT       130
#define MAIN_TEXT_ROW_DELTA  180
#define MAIN_TEXT_ROW_N(n)   (MAIN_TEXT_LEFT+MAIN_TEXT_ROW_DELTA*n)
#define MAIN_TEXT_TOP        40
#define MAIN_TEXT_LINE_DELTA 40
#define MAIN_TEXT_LINE_N(n)  (MAIN_TEXT_TOP+MAIN_TEXT_LINE_DELTA*n)

#define PRO_NAME_POS         140
#define PRO_TEXT_LEFT        150
#define PRO_TEXT_ROW_DELTA   200
#define PRO_TEXT_ROW_N(n)    (PRO_TEXT_LEFT+PRO_TEXT_ROW_DELTA*n)
#define PRO_TEXT_TOP         10
#define PRO_TEXT_LINE_DELTA  30
#define PRO_TEXT_LINE_N(n)   (PRO_TEXT_TOP+PRO_TEXT_LINE_DELTA*n)

typedef struct TextInfo
{
	HWND   hWnd;
	TCHAR  text[TEXTBOX_MAXLENGTH + 1]; // �ı�
	int    cursorPos; // ����������ڵ�λ��
	TCHAR  textBoxName[MAX_LOADSTRING+1]; // �ı��������
	int    leftPos;
	int    topPos;
    int    *value;
    int    min;
    int    max;
}TextInfo;

typedef struct Throughput
{
    TCHAR ulThroughput[100];
    TCHAR dlThroughput[100];

    TCHAR ulTbSizeWithoutCodeRateCtrl[100];
    TCHAR dlTbSizeWithoutCodeRateCtrl[100];

    TCHAR ulTbSizeWithCodeRateCtrl[100];
    TCHAR dlTbSizeWithCodeRateCtrl[100];
}Throughput;

// ȫ�ֱ���:
int g_isProTextProc = 0;
int g_isProEnable = 0;

CfgInfo g_cfg;

TextInfo g_mainTextInfo[] = {
							    {0, _T("272"), ::_tcslen(g_mainTextInfo[0].text), _T("rbNum"),  MAIN_TEXT_ROW_N(0), MAIN_TEXT_LINE_N(0), &g_cfg.rbNum, 0, 272},
							    {0, _T("28"), ::_tcslen(g_mainTextInfo[1].text), _T("mcs"), MAIN_TEXT_ROW_N(1), MAIN_TEXT_LINE_N(0), &g_cfg.mcs, 0, 28},
							    {0, _T("1"), ::_tcslen(g_mainTextInfo[2].text), _T("ueMaxLayer"), MAIN_TEXT_ROW_N(0), MAIN_TEXT_LINE_N(1), &g_cfg.ueMaxLayer, 1, 8},
							    {0, _T("1"), ::_tcslen(g_mainTextInfo[3].text), _T("totalLayer"), MAIN_TEXT_ROW_N(1), MAIN_TEXT_LINE_N(1), &g_cfg.totalLayer, 1, 24},
							    {0, _T("0"), ::_tcslen(g_mainTextInfo[4].text), _T("256QamEnable"), MAIN_TEXT_ROW_N(0), MAIN_TEXT_LINE_N(2), &g_cfg.is256Qam, 0, 1},
							    {0, _T("1"), ::_tcslen(g_mainTextInfo[5].text), _T("waveform"), MAIN_TEXT_ROW_N(1), MAIN_TEXT_LINE_N(2), &g_cfg.waveform, 0, 1}
						    };

TextInfo g_proTextInfo[] = {
							    {0, _T("10"), ::_tcslen(g_proTextInfo[0].text), _T("csiPeriod"), PRO_TEXT_ROW_N(0), PRO_TEXT_LINE_N(0), &g_cfg.csiPeriod, 0, 320},
							    {0, _T("1"), ::_tcslen(g_proTextInfo[1].text), _T("csiSym"), PRO_TEXT_ROW_N(1), PRO_TEXT_LINE_N(0), &g_cfg.csiSym, 0, 4},
                                {0, _T("80"), ::_tcslen(g_proTextInfo[2].text), _T("sibPeriod"), PRO_TEXT_ROW_N(0), PRO_TEXT_LINE_N(1), &g_cfg.sibPeriod, 0, 320},
							    {0, _T("24"), ::_tcslen(g_proTextInfo[3].text), _T("sibRbNum"), PRO_TEXT_ROW_N(1), PRO_TEXT_LINE_N(1), &g_cfg.sibRbNum, 0, 96},
                                {0, _T("20"), ::_tcslen(g_proTextInfo[4].text), _T("ssbPeriod"), PRO_TEXT_ROW_N(0), PRO_TEXT_LINE_N(2), &g_cfg.ssbPeriod, 0, 320},
							    {0, _T("30"), ::_tcslen(g_proTextInfo[5].text), _T("ssbRbStart"), PRO_TEXT_ROW_N(1), PRO_TEXT_LINE_N(2), &g_cfg.ssbRbStart, 0, 100},
                                {0, _T("1"), ::_tcslen(g_proTextInfo[6].text), _T("dmrsTypeDl"), PRO_TEXT_ROW_N(0), PRO_TEXT_LINE_N(3), &g_cfg.dmrsTypeDl, 0, 1},
							    {0, _T("0"), ::_tcslen(g_proTextInfo[7].text), _T("dmrsDurationDl"), PRO_TEXT_ROW_N(1), PRO_TEXT_LINE_N(3), &g_cfg.dmrsDurationDl, 0, 1},
                                {0, _T("0"), ::_tcslen(g_proTextInfo[8].text), _T("dmrsAddPosDl"), PRO_TEXT_ROW_N(0), PRO_TEXT_LINE_N(4), &g_cfg.dmrsAddPosDl, 0, 3},
							    {0, _T("1"), ::_tcslen(g_proTextInfo[9].text), _T("pdcchSymInD"), PRO_TEXT_ROW_N(1), PRO_TEXT_LINE_N(4), &g_cfg.pdcchSymInD, 0, 4},

                                {0, _T("1"), ::_tcslen(g_proTextInfo[10].text), _T("dmrsTypeUl"), PRO_TEXT_ROW_N(0), PRO_TEXT_LINE_N(5), &g_cfg.dmrsTypeUl, 0, 1},
							    {0, _T("1"), ::_tcslen(g_proTextInfo[11].text), _T("dmrsDurationUl"), PRO_TEXT_ROW_N(1), PRO_TEXT_LINE_N(5), &g_cfg.dmrsDurationUl, 0, 1},
                                {0, _T("0"), ::_tcslen(g_proTextInfo[12].text), _T("dmrsAddPosUl"), PRO_TEXT_ROW_N(0), PRO_TEXT_LINE_N(6), &g_cfg.dmrsAddPosUl, 0, 3},
							    {0, _T("0"), ::_tcslen(g_proTextInfo[13].text), _T("srsSymInU"), PRO_TEXT_ROW_N(1), PRO_TEXT_LINE_N(6), &g_cfg.srsSymInU, 0, 4},
                                {0, _T("0"), ::_tcslen(g_proTextInfo[14].text), _T("shortPucchSymInU"), PRO_TEXT_ROW_N(0), PRO_TEXT_LINE_N(7), &g_cfg.shortPucchSymInU, 0, 4},
							    {0, _T("90"), ::_tcslen(g_proTextInfo[15].text), _T("longPucchRbNum"), PRO_TEXT_ROW_N(1), PRO_TEXT_LINE_N(7), &g_cfg.longPucchRbNum, 0, 200},
                                {0, _T("20"), ::_tcslen(g_proTextInfo[16].text), _T("prachPeriod"), PRO_TEXT_ROW_N(0), PRO_TEXT_LINE_N(8), &g_cfg.prachPeriod, 0, 320},
							    {0, _T("12"), ::_tcslen(g_proTextInfo[17].text), _T("prachRbNum"), PRO_TEXT_ROW_N(1), PRO_TEXT_LINE_N(8), &g_cfg.prachRbNum, 0, 60},

                                {0, _T("0"), ::_tcslen(g_proTextInfo[18].text), _T("shortPucchSymInS"), PRO_TEXT_ROW_N(0), PRO_TEXT_LINE_N(9), &g_cfg.shortPucchSymInS, 0, 4},
							    {0, _T("1"), ::_tcslen(g_proTextInfo[19].text), _T("srsSymInS"), PRO_TEXT_ROW_N(1), PRO_TEXT_LINE_N(9), &g_cfg.srsSymInS, 0, 4},
                                {0, _T("1"), ::_tcslen(g_proTextInfo[20].text), _T("pdcchSymInS"), PRO_TEXT_ROW_N(0), PRO_TEXT_LINE_N(10), &g_cfg.pdcchSymInS, 0, 4},
							    {0, _T("0"), ::_tcslen(g_proTextInfo[21].text), _T("ulSymInS"), PRO_TEXT_ROW_N(1), PRO_TEXT_LINE_N(10), &g_cfg.ulSymInS, 0, 14},
                                {0, _T("11"), ::_tcslen(g_proTextInfo[22].text), _T("dlSymInS"), PRO_TEXT_ROW_N(0), PRO_TEXT_LINE_N(11), &g_cfg.dlSymInS, 0, 14}
						   };

Throughput g_throughput = {_T("0"), _T("0"), _T("0"), _T("0"), _T("0"), _T("0")};

HINSTANCE hInst;								// ��ǰʵ��
TCHAR szTitle[MAX_LOADSTRING];					// �������ı�
TCHAR szWindowClass[MAX_LOADSTRING];			// ����������

// �˴���ģ���а����ĺ�����ǰ������:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    Profession(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    ProfessionEnable(HWND, UINT, WPARAM, LPARAM);

ATOM _RegisterTextBoxClass(TCHAR textBoxName[]); // ע���ı������
HWND _CreateTextBoxWindow(HWND hParentWnd, TCHAR textBoxName[], int left, int top); // �����ı���
LRESULT CALLBACK _TextBoxWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam); // �ı��򴰿���Ϣ������
void _DrawText(HDC hDC, TCHAR text[]);                           // �����ı�
void _SetCaretPos(HWND hWnd, TCHAR text[], int cursorPos);       // ���ù��λ��
void _UpdateWindow(HWND hWnd);                     // ���´���

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: �ڴ˷��ô��롣
	MSG msg;
	HACCEL hAccelTable;

	// ��ʼ��ȫ���ַ���
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_CALCTHROUGHPUT, szWindowClass, MAX_LOADSTRING);

	if(!MyRegisterClass(hInstance))
    {
        MessageBox(NULL, TEXT("��ѽ��ԭ�����������Ҫ .NET 4.0 ��"), szWindowClass, MB_ICONERROR);
        return FALSE;
    }

	// ִ��Ӧ�ó����ʼ��:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CALCTHROUGHPUT));

	// ����Ϣѭ��:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}

//
//  ����: MyRegisterClass()
//
//  Ŀ��: ע�ᴰ���ࡣ
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CALCTHROUGHPUT));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_CALCTHROUGHPUT);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   ����: InitInstance(HINSTANCE, int)
//
//   Ŀ��: ����ʵ�����������������
//
//   ע��:
//
//        �ڴ˺����У�������ȫ�ֱ����б���ʵ�������
//        ��������ʾ�����򴰿ڡ�
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // ��ʵ������洢��ȫ�ֱ�����

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX & ~WS_THICKFRAME,
      CW_USEDEFAULT, 0, MAINWINDOW_WIDTH, MAINWINDOW_HEIGHT, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

static void SaveTextToInt(TextInfo *textInfo, int len)
{
    int i;
    for(i = 0; i < len; i++)
    {
        *textInfo[i].value = _wtoi(textInfo[i].text);
    }
}

static void GetIntToText(TextInfo *textInfo, int len)
{
    int i;
    for(i = 0; i < len; i++)
    {
         _itow_s(*textInfo[i].value, textInfo[i].text, 10);
    }
}

static bool RangeCheck(TextInfo *textInfo, int len)
{
    int i, value;
    for(i = 0; i < len; i++)
    {
        if(wcslen(textInfo[i].text) < 1) return false;
        value = _wtoi(textInfo[i].text);
        if(value < textInfo[i].min || value > textInfo[i].max) return false;
    }
    return true;
}

static bool MainSpecialCheck(TextInfo *textInfo, int len)
{
    int ueLayer = _wtoi(textInfo[2].text);
    int totalLayer = _wtoi(textInfo[3].text);
    int waveform = _wtoi(textInfo[5].text);
    int mcs = _wtoi(textInfo[1].text);
    int is256QamEnable = _wtoi(textInfo[4].text);

    if(ueLayer > totalLayer) return false;
    if(waveform == 0 && (mcs > 27 || is256QamEnable == 1)) return false;
    if(is256QamEnable == 1 && mcs > 27) return false;

    return true;
}

static bool ProSpecialCheck(TextInfo *textInfo, int len)
{
    int shortPucchSymInS = _wtoi(textInfo[18].text);
    int pdcchSymInS = _wtoi(textInfo[20].text);
    int dlSymInS = _wtoi(textInfo[22].text);
    int srsSymInS = _wtoi(textInfo[19].text);
    int ulSymInS = _wtoi(textInfo[21].text);

    int dmrsDurationDl = _wtoi(textInfo[7].text);
    int dmrsAddPosDl = _wtoi(textInfo[8].text);
    int dmrsDurationUl = _wtoi(textInfo[11].text);
    int dmrsAddPosUl = _wtoi(textInfo[12].text);

    if(shortPucchSymInS + dlSymInS + ulSymInS + srsSymInS > SYMBOL_NUM - 1) return false;
    if(shortPucchSymInS + dlSymInS + ulSymInS + srsSymInS < SYMBOL_NUM - 4) return false;
    if(pdcchSymInS > dlSymInS) return false;
    if(dmrsDurationDl == 1 && dmrsAddPosDl > 1) return false;
    if(dmrsDurationUl == 1 && dmrsAddPosUl > 1) return false;

    return true;
}

static bool CheckInput(TextInfo *textInfo, int len, bool (*SpecialCheck)(TextInfo *textInfo, int len))
{
    if(!RangeCheck(textInfo, len) || !SpecialCheck(textInfo, len)) return false;

    return true;
}

//
//  ����: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  Ŀ��: ���������ڵ���Ϣ��
//
//  WM_COMMAND	- ����Ӧ�ó���˵�
//  WM_PAINT	- ����������
//  WM_DESTROY	- �����˳���Ϣ������
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	int i = 0;
	int len = sizeof(g_mainTextInfo)/sizeof(TextInfo);
    static HWND hwndButton[2];   //��ť���
    RECT rect;

	switch (message)
	{
	case WM_CREATE:
		{
            SaveTextToInt(g_mainTextInfo, sizeof(g_mainTextInfo)/sizeof(TextInfo));
            SaveTextToInt(g_proTextInfo, sizeof(g_proTextInfo)/sizeof(TextInfo));

			for(i = 0; i < len; i++)
			{
				_RegisterTextBoxClass(g_mainTextInfo[i].textBoxName);
				g_mainTextInfo[i].hWnd = _CreateTextBoxWindow(hWnd, g_mainTextInfo[i].textBoxName, g_mainTextInfo[i].leftPos, g_mainTextInfo[i].topPos);
			}

            //��ť1
            hwndButton[0] = CreateWindow(TEXT("BUTTON"),TEXT("����"),      
                WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  
                80,160,50,30,hWnd,NULL,
                (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE), NULL);
            //��ť3
            hwndButton[1] = CreateWindow(TEXT("BUTTON"),TEXT("�߼�"),      
                WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  
                240,160,50,30,hWnd,NULL,
                (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE), NULL);
			break;
		}
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);

        if((HWND)lParam == hwndButton[0])
        {
            int len = sizeof(g_mainTextInfo)/sizeof(TextInfo);

            if(!CheckInput(g_mainTextInfo, len, MainSpecialCheck))
            {
                MessageBox(hWnd, TEXT("���֣�����������"), TEXT("@#$%^&*...."), MB_OK);
                break;
            }

            SaveTextToInt(g_mainTextInfo, len);

            _i64tow_s(CalcUlThroughput(), g_throughput.ulThroughput, sizeof(g_throughput.ulThroughput)/sizeof(TCHAR), 10);
            _i64tow_s(CalcDlThroughput(), g_throughput.dlThroughput, sizeof(g_throughput.dlThroughput)/sizeof(TCHAR), 10);

            _i64tow_s(CalcUlTbSizeWithCodeRateCtrl(), g_throughput.ulTbSizeWithCodeRateCtrl, sizeof(g_throughput.ulTbSizeWithCodeRateCtrl)/sizeof(TCHAR), 10);
            _i64tow_s(CalcDlTbSizeWithCodeRateCtrl(), g_throughput.dlTbSizeWithCodeRateCtrl, sizeof(g_throughput.dlTbSizeWithCodeRateCtrl)/sizeof(TCHAR), 10);

            GetClientRect(hWnd, &rect);
            rect.top = OUTPUT_POS_TOP;
            InvalidateRect(hWnd, &rect, TRUE);
        }
        else if((HWND)lParam == hwndButton[1])
        {
            DialogBox(hInst, MAKEINTRESOURCE(IDD_PRO), hWnd, Profession);
        }

		// �����˵�ѡ��:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
        case IDM_PROSWITCH:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_PROENABLE), hWnd, ProfessionEnable);
            break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
        {
		    hdc = BeginPaint(hWnd, &ps);
            SetBkMode(hdc, TRANSPARENT);

		    for(i = 0; i < len; i++)
		    {
			    TextOut(hdc,g_mainTextInfo[i].leftPos - MAIN_NAME_POS, g_mainTextInfo[i].topPos, g_mainTextInfo[i].textBoxName, wcslen(g_mainTextInfo[i].textBoxName));
		    }

            TextOut(hdc, OUTPUT_POS_LEFT, OUTPUT_POS_TOP, _T("����������"), 5);
            TextOut(hdc, OUTPUT_POS_LEFT+80, OUTPUT_POS_TOP, g_throughput.ulThroughput, _tcslen(g_throughput.ulThroughput));
            TextOut(hdc, OUTPUT_POS_LEFT, OUTPUT_POS_TOP+30, _T("����������"), 5);
            TextOut(hdc, OUTPUT_POS_LEFT+80, OUTPUT_POS_TOP+30, g_throughput.dlThroughput, _tcslen(g_throughput.dlThroughput));

            TextOut(hdc, OUTPUT_POS_LEFT, OUTPUT_POS_TOP+60, _T("���е�slot TbSize��"), 15);
            TextOut(hdc, OUTPUT_POS_LEFT+140, OUTPUT_POS_TOP+60, g_throughput.ulTbSizeWithCodeRateCtrl, _tcslen(g_throughput.ulTbSizeWithCodeRateCtrl));
            TextOut(hdc, OUTPUT_POS_LEFT, OUTPUT_POS_TOP+90, _T("���е�slot TbSize��"), 15);
            TextOut(hdc, OUTPUT_POS_LEFT+140, OUTPUT_POS_TOP+90, g_throughput.dlTbSizeWithCodeRateCtrl, _tcslen(g_throughput.dlTbSizeWithCodeRateCtrl));

		    EndPaint(hWnd, &ps);
		    break;
        }
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

ATOM _RegisterTextBoxClass(TCHAR textBoxName[])
{
     WNDCLASSEX wc;
     ::ZeroMemory(&wc, sizeof(wc));

     wc.cbSize     = sizeof(wc);
     wc.style      = CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS;   // ָ�������ڳߴ緢���仯ʱ�ػ洰�ڣ�������Ӧ���˫���¼�
     wc.hInstance  = hInst;
     wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1); // ָ�����ڱ�����ɫΪϵͳ��ɫ�����ڱ�����
     wc.lpszClassName = textBoxName;                  // ָ��Ҫע��Ĵ�����������������ʱҪ�Դ�����Ϊ��ʶ��
     wc.lpfnWndProc   = _TextBoxWndProc;               // ��������Ϣ�ĺ���

     return ::RegisterClassEx(&wc);                     // ����API����ע���ı��򴰿�
}

// �����ı���
HWND _CreateTextBoxWindow(HWND hParentWnd, TCHAR textBoxName[], int left, int top)
{
     // �����ı���
     HWND hWnd = ::CreateWindow(textBoxName, NULL, WS_CHILDWINDOW | WS_VISIBLE,
         left, top, TEXTBOX_WIDTH, TEXTBOX_HEIGHT,
         hParentWnd, NULL, hInst, NULL);

     return hWnd;
}

static int GetTextInfo(HWND hWnd, TCHAR **text, int **cursorPos)
{
	int i = 0;
	TCHAR name[MAX_LOADSTRING];
	int len = 0;
    TextInfo *textInfo = NULL;

    if(g_isProTextProc == 1)
    {
        len = sizeof(g_proTextInfo)/sizeof(TextInfo);
        textInfo = g_proTextInfo;
    }
    else
    {
        len = sizeof(g_mainTextInfo)/sizeof(TextInfo);
        textInfo = g_mainTextInfo;
    }

	GetClassName(hWnd, name, MAX_LOADSTRING);

	for(i = 0; i < len; i++)
	{
		if(0 != wcscmp(name, textInfo[i].textBoxName)) continue;
		*text = textInfo[i].text;
		*cursorPos = &textInfo[i].cursorPos;
		break;
	}

	if(i >= len)
		return 1;
	else
		return 0;
}

// �ı�����Ϣ�Ĵ������
LRESULT CALLBACK _TextBoxWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	TCHAR *text = NULL;     // �ı�
	int   *org = NULL;        // ����������ڵ�λ��
	int   cursorPos;

	if(GetTextInfo(hWnd, &text, &org)) 
		return ::DefWindowProc(hWnd, message, wParam, lParam);

	cursorPos = *org;

     switch (message)
     {
     case WM_PAINT: {  // ��������֮���Լ�һ�Դ����ţ���Ϊ����֮�¶���ı����ֲ���

         static PAINTSTRUCT ps;
         static RECT rect;
         HDC hDC = ::BeginPaint(hWnd, &ps);  // ��ʼ���Ʋ���

         ::GetClientRect(hWnd, &rect);        // ��ȡ�ͻ����ĳߴ�
         ::DrawEdge(hDC, &rect, EDGE_SUNKEN, BF_RECT);  // ���Ʊ߿�EDGE_SUNKEN��ʾ������ʽΪ��Ƕ��ʽ��BF_RECT��ʾ���ƾ��α߿�
         _DrawText(hDC, text);                      // �����ı�
         ::EndPaint(hWnd, &ps);               // �������Ʋ���

         } break;

     case WM_SETFOCUS: {    // ��ý���
         ::CreateCaret(hWnd, (HBITMAP)NULL, 1, TEXTBOX_HEIGHT-5);     // �������
         _SetCaretPos(hWnd, text, cursorPos);                            // ���ù��λ��
         ::ShowCaret(hWnd);                   // ��ʾ���
         } break;

     case WM_KILLFOCUS: // ʧȥ����
         ::HideCaret(hWnd);                   // ���ع��
         ::DestroyCaret();                    // ���ٹ��
         break;

     case WM_SETCURSOR: {  // ���ù����״
         static HCURSOR hCursor = ::LoadCursor(NULL, IDC_IBEAM);
         ::SetCursor(hCursor);
         } break;

     case WM_CHAR: {    // �ַ���Ϣ
         TCHAR code = (TCHAR)wParam;
         int len = ::_tcslen(text);
         if(code < (TCHAR)'0' || code > (TCHAR)'9' || len >= MAX_INPUT_LEN)
              return 0;

         ::MoveMemory(text + cursorPos + 1, text + cursorPos, (len - cursorPos + 1) * sizeof(TCHAR));
         text[cursorPos ++] = code;

         _UpdateWindow(hWnd);
         _SetCaretPos(hWnd, text, cursorPos);

         } break;
     case WM_GETDLGCODE:
        return DLGC_WANTALLKEYS;

     case WM_KEYDOWN: {  // ��������Ϣ
         TCHAR code = (TCHAR)wParam;

         switch (code)
         {
         case VK_LEFT: // �����
              if(cursorPos > 0)
                   cursorPos --;
              break;

         case VK_RIGHT:     // �ҹ���
              if(cursorPos < (int)::_tcslen(text))
                   cursorPos ++;
              break;

         case VK_HOME: // HOME ��
              cursorPos = 0;
              break;

         case VK_END:  // END ��
              cursorPos = ::_tcslen(text);
              break;

         case VK_BACK: // �˸��
              if(cursorPos > 0)
              {
                   ::MoveMemory(text + cursorPos - 1, text + cursorPos, (::_tcslen(text)-cursorPos + 1) * sizeof(TCHAR));
                   cursorPos --;
                   _UpdateWindow(hWnd);
              }
              break;

         case VK_DELETE: {  // ɾ����
              int len = ::_tcslen(text);
              if(cursorPos < len)
              {
                   ::MoveMemory(text + cursorPos, text + cursorPos + 1, (::_tcslen(text) - cursorPos + 1) * sizeof(TCHAR));
                   _UpdateWindow(hWnd);
              }

              } break;

         }

         _SetCaretPos(hWnd, text, cursorPos);

         } break;

     case WM_LBUTTONDOWN: {  // ��굥�������ù��λ��
         int x = LOWORD(lParam);
         HDC hDc = ::GetDC(hWnd);

         int strLen = ::_tcslen(text), strPos = 0;
         SIZE size;

         for (strPos=0; strPos<strLen; strPos++)
         {
              ::GetTextExtentPoint(hDc, text, strPos, &size);

              if(size.cx + 4 >= x)
                   break;
         }

         cursorPos = strPos;
         ::GetTextExtentPoint(hDc, text, strPos, &size);
         ::SetCaretPos(size.cx + 4, 3);

		 ::SetFocus(hWnd);

         ::ReleaseDC(hWnd, hDc);

         } break;

     default:
         return ::DefWindowProc(hWnd, message, wParam, lParam);
     }

	 *org = cursorPos;

     return (LRESULT)0;
}

// ���´���
void _UpdateWindow(HWND hWnd)
{
     RECT rect;
     ::GetClientRect(hWnd, &rect);
     ::InvalidateRect(hWnd, &rect, TRUE);
     ::UpdateWindow(hWnd);
}

// �����ı�
void _DrawText(HDC hDC, TCHAR text[])
{
     int len = ::_tcslen(text);
     ::TextOut(hDC, 4, 2, text, len);
}

// ���ù��λ��
void _SetCaretPos(HWND hWnd, TCHAR text[], int cursorPos)
{
     HDC hDC = ::GetDC(hWnd);

     SIZE size;
     ::GetTextExtentPoint(hDC, text, cursorPos, &size);
     ::SetCaretPos(4 + size.cx, 3);

     ::ReleaseDC(hWnd, hDC);
}

// ���߼��������Ϣ�������
INT_PTR CALLBACK Profession(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
    PAINTSTRUCT ps;
	HDC hdc;
    int i = 0;
	int len = sizeof(g_proTextInfo)/sizeof(TextInfo);

	switch (message)
	{
	case WM_INITDIALOG:
        {
            g_isProTextProc = 1;
            GetIntToText(g_proTextInfo, sizeof(g_proTextInfo)/sizeof(TextInfo));

            for(i = 0; i < len; i++)
		    {
			    _RegisterTextBoxClass(g_proTextInfo[i].textBoxName);
			    g_proTextInfo[i].hWnd = _CreateTextBoxWindow(hDlg, g_proTextInfo[i].textBoxName, g_proTextInfo[i].leftPos, g_proTextInfo[i].topPos);
		        if(g_isProEnable == 0) ::EnableWindow(g_proTextInfo[i].hWnd, FALSE);
            }
		    return (INT_PTR)TRUE;
        }
	case WM_COMMAND:
        {
		    if (LOWORD(wParam) == IDOK)
		    {
                if(!CheckInput(g_proTextInfo, sizeof(g_proTextInfo)/sizeof(TextInfo), ProSpecialCheck))
                {
                    MessageBox(hDlg, TEXT("�٣���������������ѽ"), TEXT("duang...duang..."), MB_OK);
                    return (INT_PTR)TRUE;
                }

                SaveTextToInt(g_proTextInfo, sizeof(g_proTextInfo)/sizeof(TextInfo));

                g_isProTextProc = 0;
			    EndDialog(hDlg, LOWORD(wParam));
			    return (INT_PTR)TRUE;
		    }
            if (LOWORD(wParam) == IDCANCEL)
		    {
                g_isProTextProc = 0;
			    EndDialog(hDlg, LOWORD(wParam));
			    return (INT_PTR)TRUE;
		    }
		    break;
        }
    case WM_PAINT:
        {
            hdc = BeginPaint(hDlg, &ps);
            int mode = SetBkMode(hdc, TRANSPARENT);
            for(i = 0; i < len; i++)
		    {
			    TextOut(hdc, g_proTextInfo[i].leftPos - PRO_NAME_POS, g_proTextInfo[i].topPos, g_proTextInfo[i].textBoxName, wcslen(g_proTextInfo[i].textBoxName));
		    }
            SetBkMode(hdc, mode);//��ԭΪ֮ǰ��ģʽ, ���ﲻ�Ǳ����
            EndPaint(hDlg, &ps);
            break;
        }
    case WM_CLOSE:
        {
            g_isProTextProc = 0;
            EndDialog(hDlg ,LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
	}

	return (INT_PTR)FALSE;
}

// �������߼����ܡ������Ϣ�������
INT_PTR CALLBACK ProfessionEnable(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK)
		{
            int pass_len;
            TCHAR password[64];
            pass_len = GetDlgItemText(hDlg, IDC_PASSWORD, password, 64);

            if(0 == wcscmp(password, _T("xfxx")))
            {
                g_isProEnable = 1;
                MessageBox(hDlg, TEXT("���ˣ���ʹ�á�"), TEXT("��������"), MB_OK);
                EndDialog(hDlg, LOWORD(wParam));
			    return (INT_PTR)TRUE;
            }
            g_isProEnable = 0;
            MessageBox(hDlg, TEXT("�߼����ɲ������Ϳ���ʹ�õ�Ӵ��"), TEXT("��һ��������"), MB_OK);
			return (INT_PTR)TRUE;
		}
        if (LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

// �����ڡ������Ϣ�������
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
