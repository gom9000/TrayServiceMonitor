/*                                                     ''~``
 * @(#)ServiceManager.h 1.0  2008/10/13               ( o o )
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
 * | Module....: Service Manager APIs header file                     |
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


#define MAX_NAMELEN 256


typedef struct SERVICE_INFO_STRUCT
{
	TCHAR name[MAX_NAMELEN];
	TCHAR displayName[MAX_NAMELEN];
	TCHAR description[MAX_NAMELEN];
	TCHAR exePath[MAX_NAMELEN];
	DWORD startupType;
	DWORD isRunning;
} SERVICEINFOSTRUCT;


// Function Prototypes
BOOL GetServiceInfo(LPCTSTR lpServiceName, SERVICEINFOSTRUCT *info);
BOOL StartService(LPCTSTR lpServiceName);
BOOL StopService(LPCTSTR lpServiceName);
BOOL WaitPendingStatus(LPCTSTR lpServiceName);
TCHAR *GetLastErrorMessage(void);
BOOL GetServicesListSize(int *size);
BOOL GetServicesList(SERVICEINFOSTRUCT info[]);
