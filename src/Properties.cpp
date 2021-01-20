/*                                                     ''~``
 * @(#)Properties.cpp 1.0  2008/10/13                 ( o o )
 * +---------------------------------------------.oooO--(_)--Oooo.----+
 * |   ________        _________________.________                     |
 * |  /  _____/  ____ /   _____/   __   \   ____/                     |
 * | /   \  ___ /  _ \\_____  \\____    /____  \                      |
 * | \    \_\  (  <_> )        \  /    //       \                     |
 * |  \______  /\____/_______  / /____//______  /                     |
 * |         \/              \/               \/                      |
 * | Copyright (c) 2008 by                                            |
 * | Alessandro Fraschetti (gos95@gommagomma.net)                     |
 * |                                                                  |
 * | Project...: TSMon - A system tray windows service monitor        |
 * | Module....: Properties dialog procedure                          |
 * | Author....: Alessandro Fraschetti                                |
 * | Notes.....:                                                      |
 * |                                              .oooO               |
 * |                                             (     )    Oooo.     |
 * +-----------------------------------------------\  (----(    )-----+
 *                                                  \_ )    )  /
 * For more information about TrayServiceMonitor visit:    ( _/
 *     http://gommagomma.net/gos95/TSMon
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


#include <windows.h>
#include <stdio.h>
#include "TrayServiceMonitor.h"


extern TCHAR szService[MAX_LOADSTRING];
extern BOOL isServiceEnable,  isServiceRunning;

SERVICEINFOSTRUCT info;
TCHAR *SERVICESTARTUPTYPE[4] = {"", "Automatic", "Manual", "Disabled"};
TCHAR *SERVICESTATUS[2] = {"Stopped", "Started"};

static BOOL isDisplayed = FALSE;


BOOL CALLBACK PropertiesDlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);

    switch ( msg )
	{
		case WM_INITDIALOG:
			if ( isDisplayed ) EndDialog(hWnd, IDOK);
		    isDisplayed = TRUE;
			if ( GetServiceInfo(szService, &info) == FALSE ) return FALSE;

			SetDlgItemText(hWnd, IDC_STATIC_NAME, info.name);
			SetDlgItemText(hWnd, IDC_EDIT_DISPLAYNAME, info.displayName);
			SetDlgItemText(hWnd, IDC_EDIT_DESCRIPTION, info.description);
			SetDlgItemText(hWnd, IDC_EDIT_EXEPATH, info.exePath);

			HWND hComboStartupType = GetDlgItem(hWnd, IDC_COMBO_STARTUPTYPE);
		    SendMessage(hComboStartupType, CB_ADDSTRING, (WPARAM)NULL, (LPARAM)SERVICESTARTUPTYPE[info.startupType-1]);
			SendMessage(hComboStartupType, CB_SETCURSEL, 0, (LPARAM)NULL);

			SetDlgItemText(hWnd, IDC_STATIC_STATUS, SERVICESTATUS[(info.isRunning==0?0:1)]);

			HWND hStart = GetDlgItem(hWnd, IDSTART);
			HWND hStop = GetDlgItem(hWnd, IDSTOP);
			if ( isServiceRunning && isServiceEnable )       EnableWindow(hStop, TRUE);
	        else if ( !isServiceRunning && isServiceEnable ) EnableWindow(hStart, TRUE);
        return TRUE;

		case WM_COMMAND:
            switch ( LOWORD(wParam) )
			{
			    case IDSTART:
				    isDisplayed = FALSE;
					EndDialog(hWnd, IDSTART);
  			    break;

				case IDSTOP:
				    isDisplayed = FALSE;
					EndDialog(hWnd, IDSTOP);
  			    break;
			}
        break;

        case WM_CLOSE:
		    isDisplayed = FALSE;
			EndDialog(hWnd, 0);
		break;

		default:
            return FALSE;
    }

	return TRUE;
}
