/*                                                     ''~``
 * @(#)About.cpp 1.0  2008/10/13                      ( o o )
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
 * | Module....: About dialog procedure                               |
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
#include "TrayServiceMonitor.h"


// Global Variables
TCHAR szAboutText[MAX_LOADSTRING];
TCHAR szAboutWeb[MAX_LOADSTRING];
HBITMAP background;


BOOL CALLBACK LicenseDlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);

    switch ( msg )
	{
        case WM_INITDIALOG:
	    return TRUE;

		case WM_COMMAND:
            switch ( LOWORD(wParam) )
			{
				case IDOK:
                    EndDialog(hWnd, IDOK);
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


BOOL CALLBACK AboutDlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	HINSTANCE hInstance = GetModuleHandle(NULL);

	UNREFERENCED_PARAMETER(lParam);

    switch ( msg )
	{
		case WM_INITDIALOG:
			LoadString(GetModuleHandle(NULL), IDS_ABOUT_TEXT, szAboutText, MAX_LOADSTRING);
			LoadString(GetModuleHandle(NULL), IDS_ABOUT_WEB, szAboutWeb, MAX_LOADSTRING);
			background = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BACKGROUND));
		return TRUE;

        case WM_PAINT:
        {
            PAINTSTRUCT ps;
			RECT rect;
            HDC hDC = BeginPaint(hWnd, &ps);
			HDC hMemDC = CreateCompatibleDC(hDC);

	        HBITMAP hBmpOld = (HBITMAP)SelectObject(hMemDC, background);

            SelectObject(hMemDC, background);
            BitBlt(hDC, 0, 35, 320, 120, hMemDC, 0, 0, SRCCOPY);

	        SetBkMode(hDC, TRANSPARENT);
	        SetTextColor(hDC, RGB(30, 30, 40));
			SetRect(&rect,10,45,310,150);
			DrawText(hDC,szAboutText, -1, &rect, DT_LEFT);

	        SelectObject(hMemDC, hBmpOld);
            DeleteDC(hMemDC);

            EndPaint(hWnd, &ps);
        }
        break;

		case WM_COMMAND:
            switch ( LOWORD(wParam) )
			{
				case IDOK:
                    EndDialog(hWnd, IDOK);
                break;

				case ID_LICENSE:
				    DialogBox(hInstance, MAKEINTRESOURCE(IDD_LICENSE), hWnd, LicenseDlgProc);
				break;

				case ID_WEB:
	                ShellExecute(hWnd, "open", szAboutWeb, 0, 0, SW_SHOWDEFAULT);
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
