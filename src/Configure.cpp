/*                                                     ''~``
 * @(#)Configure.cpp 1.0  2011/17/01                  ( o o )
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
 * | Module....: Configure dialog procedure                           |
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
#include <shellapi.h>
#include <shlwapi.h>
#include "TrayServiceMonitor.h"


extern TCHAR szService[MAX_LOADSTRING];


HWND hListBox;
SERVICEINFOSTRUCT *infolist, current;
TCHAR item[MAX_LOADSTRING];
int size;


BOOL CALLBACK ConfigureDlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);

    switch ( msg )
	{
		case WM_INITDIALOG:
		    int idx;
		    if ( GetServiceInfo(szService, &current) == FALSE ) StrCpy(current.displayName, "undefined\0");
		    SetDlgItemText(hWnd, IDC_STATIC_NAME, current.displayName);

            if ( GetServicesListSize(&size) == FALSE ) return FALSE;
            infolist = new SERVICEINFOSTRUCT[size];
            if ( GetServicesList(infolist) == FALSE ) return FALSE;
	
		    hListBox = GetDlgItem(hWnd, ID_SERVICESLIST);
			for (int ii = 0; ii < size; ii++) {
			    idx = SendMessage(hListBox, LB_ADDSTRING, (WPARAM)0, (LPARAM)infolist[ii].displayName);
				if (!StrCmp(infolist[ii].name, szService)) {
				    SendMessage(hListBox, LB_SETCURSEL, (WPARAM)idx, (LPARAM)0);
					EnableWindow(GetDlgItem(hWnd, IDOK), TRUE);
				}
			}
		break;

		case WM_COMMAND:
            switch ( LOWORD(wParam) )
			{
				case IDOK:
				    for (int ii = 0; ii < size; ii++)
						if (!StrCmp(infolist[ii].displayName, item))
						    { StrCpy(szService, infolist[ii].name); break; }
                    EndDialog(hWnd, IDOK);
                break;

				case ID_SERVICESLIST:
				    if ( HIWORD(wParam) == LBN_SELCHANGE )
				    {
					    EnableWindow(GetDlgItem(hWnd, IDOK), TRUE);
					    int idx = SendMessage(hListBox, LB_GETCURSEL, 0, 0);
						SendMessage(hListBox, LB_GETTEXT, (WPARAM)idx, (LPARAM)item);
						SetDlgItemText(hWnd, IDC_STATIC_NAME, item);
					}
				break;
			}
        break;

        case WM_CLOSE:
			EndDialog(hWnd, 0);
		break;

		default:
            return FALSE;
    }

	return TRUE;
}
