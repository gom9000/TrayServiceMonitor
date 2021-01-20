/*                                                     ''~``
 * @(#)TrayServiceMonitor.cpp 1.1  2008/10/13         ( o o )
 * +---------------------------------------------.oooO--(_)--Oooo.----+
 * |   ________        _________________.________                     |
 * |  /  _____/  ____ /   _____/   __   \   ____/                     |
 * | /   \  ___ /  _ \\_____  \\____    /____  \                      |
 * | \    \_\  (  <_> )        \  /    //       \                     |
 * |  \______  /\____/_______  / /____//______  /                     |
 * |         \/              \/               \/                      |
 * | Copyright (c) 2008 2009 by                                       |
 * | Alessandro Fraschetti (gos95@gommagomma.net)                     |
 * |                                                                  |
 * | Project...: TSMon - A system tray windows service monitor        |
 * | Module....: WinMain                                              |
 * | Author....: Alessandro Fraschetti                                |
 * | Notes.....:                                                      |
 * |                                              .oooO               |
 * |                                             (     )    Oooo.     |
 * +-----------------------------------------------\  (----(    )-----+
 *                                                  \_ )    )  /
 * For more information about TrayServiceMonitor visit:    ( _/
 *     http://gommagomma.net/gos95/TrayServiceMonitor
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as 
 *  published by the Free Software Foundation; either version 2 of the
 *  License, or (at your option) any later version. 
 *
 *  This program is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, see <http://www.gnu.org/licenses/>.
 */


#define STRICT
#define WIN32_LEAN_AND_MEAN


#include <process.h>
#include <windows.h>
#include <commctrl.h>
#include <shellapi.h>
#include <stdio.h>
#include "TrayServiceMonitor.h"


// Windows Application Function Prototypes
ATOM             RegisterTheClass(HINSTANCE hInstance);
BOOL             CreateTheWindow(HINSTANCE hInstance, int nCmdShow);
LRESULT CALLBACK TheWindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
void             DisplayMessage(UINT uResource, UINT uType, LPCSTR altMessage);

// TaskBar Function Prototypes
BOOL             AddTaskBarIcon(HINSTANCE hInstance, HWND hWnd, UINT uID, UINT icon, LPCSTR lpszTip);
BOOL             DeleteTaskBarIcon(HWND hWnd, UINT uID);
BOOL             ModifyTaskBarIcon(HINSTANCE hInstance, HWND hWnd, UINT uID, UINT icon, LPCSTR lpszTip);
void             AddAppropriateTaskBarIcon(HINSTANCE hInstance, HWND hWnd);
void             ModifyAppropriateTaskBarIcon(HINSTANCE hInstance, HWND hWnd);
void             ShowPopupMenu(HINSTANCE hInstance, HWND hWnd);

// Service Manager Function Prototypes
BOOL             CheckRunnings(void);
BOOL             StartService(HWND hWnd, LPSTR lpService);
BOOL             StopService(HWND hWnd, LPSTR lpService);

// Dialog Function Prototypes
BOOL CALLBACK    AboutDlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK    PropertiesDlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK    ConfigureDlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


// Global Variables
TCHAR szTitle[MAX_LOADSTRING];
TCHAR szWindowClass[MAX_LOADSTRING];
TCHAR szTryToolTip[MAX_LOADSTRING];

TCHAR szService[MAX_LOADSTRING] = "\0";
BOOL isServiceEnable = TRUE,  isServiceRunning = FALSE;

int flagRightButtonDown = 0;


/*
 * Display a message passed by parameter or read from resources.
 */
void DisplayMessage(UINT uResource, UINT  uType, LPCSTR altMessage)
{
	TCHAR szMessage[MAX_LOADSTRING];

	if ( LoadString(GetModuleHandle(NULL), uResource, szMessage, MAX_LOADSTRING) )
        MessageBox(NULL, szMessage, szTitle, uType | MB_OK);
	else
		MessageBox(NULL, altMessage, szTitle, uType | MB_OK);
}


/*
 * WinMain
 */
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nCmdShow)
{
	MSG msg;

    UNREFERENCED_PARAMETER(lpCmdLine);
    UNREFERENCED_PARAMETER(hPrevInst);

	InitCommonControls();

    // Initialize Global Strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_APP_WCLASSNAME, szWindowClass, MAX_LOADSTRING);
	LoadString(hInstance, IDS_TRY_TOOLTIP, szTryToolTip, MAX_LOADSTRING);

	//Registering the Window Class
    if ( !RegisterTheClass(hInstance) ) {
		DisplayMessage(0, MB_ICONSTOP, "Window Registration Failed!");
        return FALSE;
    }

    // Creating the Window
	if ( !CreateTheWindow(hInstance, nCmdShow) ) {
		DisplayMessage(0, MB_ICONSTOP, "Window Creation Failed!");
		return FALSE;
	}

    // The Message Loop
    while ( GetMessage(&msg, NULL, 0, 0) ) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
    }
	
	return (int)msg.wParam;
}


ATOM RegisterTheClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize         = sizeof(WNDCLASSEX); 
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)TheWindowProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APP_ICON));
	wcex.hCursor		= LoadCursor(hInstance, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)GetStockObject(WHITE_BRUSH);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm        = NULL;

	return RegisterClassEx(&wcex);
}


BOOL CreateTheWindow(HINSTANCE hInstance, int nCmdShow)
{
    HWND hWnd;

	hWnd = CreateWindowEx(
		WS_DISABLED,
        szWindowClass,
        szTitle,
		0,
        CW_USEDEFAULT, CW_USEDEFAULT, 0, 0,
        NULL, NULL, hInstance, NULL
	);

    if (!hWnd) return FALSE;

    return TRUE;
}


BOOL AddTaskBarIcon(HINSTANCE hInstance, HWND hWnd, UINT uID, UINT icon, LPCSTR lpszTip)
{
    BOOL res;
    NOTIFYICONDATA nId;
	HICON hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(icon));

	nId.cbSize = sizeof(NOTIFYICONDATA); 
    nId.hWnd = hWnd; 
    nId.uID = uID; 
    nId.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP; 
    nId.uCallbackMessage = NOTIFYICON_MESSAGE;
	nId.hIcon = hIcon;
	wsprintf(nId.szTip, "%s", lpszTip);

	res = Shell_NotifyIcon(NIM_ADD, &nId); 
 
    if (hIcon) DestroyIcon(hIcon);

    return res;
}


BOOL DeleteTaskBarIcon(HWND hWnd, UINT uID)
{ 
    BOOL res; 
    NOTIFYICONDATA nId; 
 
    nId.cbSize = sizeof(NOTIFYICONDATA); 
    nId.hWnd = hWnd; 
    nId.uID = uID; 
         
    res = Shell_NotifyIcon(NIM_DELETE, &nId); 
    return res; 
}


BOOL ModifyTaskBarIcon(HINSTANCE hInstance, HWND hWnd, UINT uID, UINT icon, LPCSTR lpszTip)
{
    BOOL res;
    NOTIFYICONDATA nId;
	HICON hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(icon));

	nId.cbSize = sizeof(NOTIFYICONDATA); 
    nId.hWnd = hWnd; 
    nId.uID = uID; 
    nId.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP; 
    nId.uCallbackMessage = NOTIFYICON_MESSAGE;
	nId.hIcon = hIcon;
	wsprintf(nId.szTip, "%s", lpszTip);

	res = Shell_NotifyIcon(NIM_MODIFY, &nId); 
 
    if (hIcon) DestroyIcon(hIcon);

    return res;
}


void AddAppropriateTaskBarIcon(HINSTANCE hInstance, HWND hWnd)
{
	if (isServiceRunning)
	    AddTaskBarIcon(hInstance, hWnd, NOTIFYICON_ID, IDI_NOTIFYICON_RUNNING, szTryToolTip);
	else
	    AddTaskBarIcon(hInstance, hWnd, NOTIFYICON_ID, IDI_NOTIFYICON_STOPPED, szTryToolTip);
}


void ModifyAppropriateTaskBarIcon(HINSTANCE hInstance, HWND hWnd)
{
	if (isServiceRunning)
	    ModifyTaskBarIcon(hInstance, hWnd, NOTIFYICON_ID, IDI_NOTIFYICON_RUNNING, szTryToolTip);
	else
	    ModifyTaskBarIcon(hInstance, hWnd, NOTIFYICON_ID, IDI_NOTIFYICON_STOPPED, szTryToolTip);
}


void ShowPopupMenu(HINSTANCE hInstance, HWND hWnd)
{
    HMENU hMenu, hPopup;
    POINT point;

    hMenu = LoadMenu(hInstance, MAKEINTRESOURCE(IDR_APP_MENU));
    hPopup = GetSubMenu(hMenu, 0);

	SetMenuDefaultItem(hPopup, 0, TRUE);

    // enable/gray menuitems
	if ( isServiceRunning && isServiceEnable )
	{
        EnableMenuItem(hPopup, ID_CMD_START_SERVICE, MF_GRAYED);
		EnableMenuItem(hPopup, ID_CMD_STOP_SERVICE, MF_ENABLED);
	}
	else if ( !isServiceRunning && isServiceEnable ) 
	{
        EnableMenuItem(hPopup, ID_CMD_START_SERVICE, MF_ENABLED);
	    EnableMenuItem(hPopup, ID_CMD_STOP_SERVICE, MF_GRAYED);
	}

	GetCursorPos(&point);
    SetForegroundWindow(hWnd);
    TrackPopupMenuEx(hPopup, TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, hWnd, NULL);
    PostMessage(hWnd, WM_USER, 0, 0);
    DestroyMenu(hMenu);
}                                                                             


BOOL CheckRunnings(void)
{
	SERVICEINFOSTRUCT info;

	if (isServiceEnable)
	{
        if ( GetServiceInfo(szService, &info) == FALSE ) return FALSE;
        if ( info.isRunning ) isServiceRunning = TRUE;
		else isServiceRunning = FALSE;
	}

    return TRUE;
}


BOOL StartService(HWND hWnd, LPSTR lpService)
{
    SERVICEINFOSTRUCT info;

	if ( !StartService(lpService) )	return FALSE;
	if ( !WaitPendingStatus(lpService) ) return FALSE;
	if ( !GetServiceInfo(lpService, &info) ) return FALSE;

	if ( info.isRunning ) {
        isServiceRunning = TRUE;
		DisplayMessage(IDS_SERVICE_STARTED, MB_ICONINFORMATION, NULL);
	} else {
		DisplayMessage(IDS_SERVICE_NOT_STARTED, MB_ICONEXCLAMATION, NULL);
	}

    return TRUE;
}


BOOL StopService(HWND hWnd, LPSTR lpService)
{
    SERVICEINFOSTRUCT info;

	if ( !StopService(lpService) ) return FALSE;
	if ( !WaitPendingStatus(lpService) ) return FALSE;
	if ( !GetServiceInfo(lpService, &info) ) return FALSE;

	if ( !info.isRunning ) {
        isServiceRunning = FALSE;
		DisplayMessage(IDS_SERVICE_STOPPED, MB_ICONINFORMATION, NULL);
	} else {
		DisplayMessage(IDS_SERVICE_NOT_STOPPED, MB_ICONEXCLAMATION, NULL);
	}

    return TRUE;
}


BOOL loadConfiguration(void)
{
    FILE *fp;

	if ((fp = fopen(TSM_CONF_FILE, "r+" )) == NULL) return FALSE;
	fgets(szService, MAX_LOADSTRING, fp);
	fclose(fp);

    return TRUE;
}


BOOL saveConfiguration(void)
{
    FILE *fp;

	if ((fp = fopen(TSM_CONF_FILE, "w" )) == NULL) return FALSE;
	fputs(szService, fp);
	fclose(fp);

    return TRUE;
}


// The Window Procedure
LRESULT CALLBACK TheWindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	HINSTANCE hInstance = GetModuleHandle(NULL);

	switch ( msg )
    {
        case WM_CREATE:
		    loadConfiguration();
            if ( !CheckRunnings() )
			{
                DisplayMessage(0, MB_ICONSTOP, GetLastErrorMessage());
				DialogBox(hInstance, MAKEINTRESOURCE(IDD_CONFIGURE), hWnd, ConfigureDlgProc);
				if (saveConfiguration() != TRUE)
				{
				    MessageBox(NULL, "Unable to create configuration file.", szTitle, MB_ICONSTOP | MB_OK);
			        DestroyWindow(hWnd);	
				}
	        }
			AddAppropriateTaskBarIcon(hInstance, hWnd);
			SetTimer(hWnd, IDT_TIMER, 5000, NULL);
        break;

        case WM_CLOSE:
            DestroyWindow(hWnd);
        break;

		case WM_DESTROY:
		    KillTimer(hWnd, IDT_TIMER);
		    DeleteTaskBarIcon(hWnd, NOTIFYICON_ID);
		    PostQuitMessage(0);
        break;

        case NOTIFYICON_MESSAGE:
            if ( !CheckRunnings() )
			{
                DisplayMessage(0, MB_ICONSTOP, GetLastErrorMessage());
	            DestroyWindow(hWnd);
	        }

			if ( LOWORD(wParam) == NOTIFYICON_ID )
			{
			    switch ( lParam )
				{
	                case WM_LBUTTONDBLCLK:
		                SendMessage(hWnd, WM_COMMAND, ID_CMD_PROPERTIES, (LPARAM)NULL);
		            break;

	                case WM_RBUTTONDOWN:
	  	                flagRightButtonDown = 1;
		            break;

	                case WM_RBUTTONUP:
		                if ( flagRightButtonDown == 1 )
						{
						    flagRightButtonDown = 0;
						    ShowPopupMenu(hInstance, hWnd);
						}
		        }
			}
		break;

        case WM_TIMER:
            if ( !CheckRunnings() )
			{
                DisplayMessage(0, MB_ICONSTOP, GetLastErrorMessage());
	            DestroyWindow(hWnd);
	        }
            ModifyAppropriateTaskBarIcon(hInstance, hWnd);
		    SetTimer(hWnd, IDT_TIMER, 5000, NULL);
		break;

        case WM_COMMAND:
		    switch ( LOWORD(wParam) )
			{
				case ID_CMD_PROPERTIES:
				    LPARAM lResult;
					lResult = DialogBox(hInstance, MAKEINTRESOURCE(IDD_PROPERTIES), hWnd, PropertiesDlgProc);
                    switch ( lResult )
                    {
					    case IDSTART:
						    SendMessage(hWnd, WM_COMMAND, ID_CMD_START_SERVICE, (LPARAM)NULL);
							break;

						case IDSTOP:
						    SendMessage(hWnd, WM_COMMAND, ID_CMD_STOP_SERVICE, (LPARAM)NULL);
							break;
					}
                break;

				case ID_CMD_START_SERVICE:
                    if ( !StartService(hWnd, szService) )
					{
                        DisplayMessage(0, MB_ICONSTOP, GetLastErrorMessage());
	                    DestroyWindow(hWnd);
	                }
					ModifyAppropriateTaskBarIcon(hInstance, hWnd);
                break;

				case ID_CMD_STOP_SERVICE:
                    if ( !StopService(hWnd, szService) )
					{
                        DisplayMessage(0, MB_ICONSTOP, GetLastErrorMessage());
	                    DestroyWindow(hWnd);
	                }
					ModifyAppropriateTaskBarIcon(hInstance, hWnd);
				break;

                case ID_CMD_CONFIGURE:
				    DialogBox(hInstance, MAKEINTRESOURCE(IDD_CONFIGURE), hWnd, ConfigureDlgProc);
					saveConfiguration();
					//_spawnlp(_P_WAIT , "notepad", TSM_CONF_FILE, TSM_CONF_FILE, NULL, NULL);
					//SendMessage(hWnd, WM_CREATE, 0, (LPARAM)NULL);
                break;

                case ID_CMD_EXIT:
					PostMessage(hWnd, WM_CLOSE, (WPARAM)NULL, (LPARAM)NULL);
                break;

				case ID_CMD_ABOUT:
					DialogBox(hInstance, MAKEINTRESOURCE(IDD_ABOUT), hWnd, AboutDlgProc);
                break;
            }
        break;

		default:
            return DefWindowProc(hWnd, msg, wParam, lParam);
    }

	return TRUE;
}
