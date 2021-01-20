/*                                                     ''~``
 * @(#)ServiceManager.cpp 1.0  2008/10/13             ( o o )
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
 * | Module....: Service Manager APIs                                 |
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


#include <shlwapi.h>
#include "ServiceManager.h"


// Local Function Prototypes
SC_HANDLE _OpenManager(LPCTSTR lpMachineName, LPCTSTR lpDatabaseName, DWORD dwDesiredAccess);
SC_HANDLE _OpenService(SC_HANDLE hManager, LPCTSTR lpServiceName, DWORD dwDesiredAccess);
BOOL      _ControlService(SC_HANDLE hService, DWORD dwControl, LPSERVICE_STATUS lpServiceStatus);
BOOL      _StartService(SC_HANDLE hService);


// Global Variables
TCHAR errorMessage[MAX_NAMELEN];
LPSTR lpMachineName = NULL;
LPSTR lpDatabaseName = NULL;


/*
 * Get last error message.
 */
 TCHAR *GetLastErrorMessage(void)
 {
     return errorMessage;
 }


/*
 * Get service information.
 */
BOOL GetServiceInfo(LPCTSTR lpServiceName, SERVICEINFOSTRUCT *info)
{
	SC_HANDLE hManager;
    SC_HANDLE hService;
    LPQUERY_SERVICE_CONFIG lpsc = NULL; 
    LPSERVICE_DESCRIPTION lpsd = NULL;
	LPSERVICE_STATUS lstatus = NULL;
	DWORD dwBytesNeeded, cbBufSize = 0, dwError;


    // access to service manager
	hManager = _OpenManager(lpMachineName, lpDatabaseName, SC_MANAGER_ALL_ACCESS);
	if ( hManager == NULL ) return FALSE;

    // access to monitor service
    hService = _OpenService(hManager, lpServiceName, SERVICE_ALL_ACCESS);
    if ( hService == NULL ) {
		CloseServiceHandle(hManager);
		return FALSE;
	}

    // get service configuration
    if( !QueryServiceConfig(hService, NULL, 0, &dwBytesNeeded) ) {
        dwError = GetLastError();
        if ( dwError = ERROR_INSUFFICIENT_BUFFER ) {
            cbBufSize = dwBytesNeeded;
            lpsc = (LPQUERY_SERVICE_CONFIG)LocalAlloc(LMEM_FIXED, cbBufSize);
        } else {
            wsprintf(errorMessage, "QueryServiceConfig failed (%d)", dwError);
			CloseServiceHandle(hService);
            CloseServiceHandle(hManager);
            return FALSE; 
        }
    }
    if( !QueryServiceConfig(hService, lpsc, cbBufSize, &dwBytesNeeded) ) {
        wsprintf(errorMessage, "QueryServiceConfig failed (%d)", GetLastError());
		CloseServiceHandle(hService);
        CloseServiceHandle(hManager);
        return FALSE; 
    }

    if( !QueryServiceConfig2(hService, SERVICE_CONFIG_DESCRIPTION, NULL, 0, &dwBytesNeeded) ) {
        dwError = GetLastError();
        if ( dwError == ERROR_INSUFFICIENT_BUFFER ) {
            cbBufSize = dwBytesNeeded;
            lpsd = (LPSERVICE_DESCRIPTION)LocalAlloc(LMEM_FIXED, cbBufSize);
        } else {
            wsprintf(errorMessage, "QueryServiceConfig2 failed (%d)", dwError);
			CloseServiceHandle(hService);
            CloseServiceHandle(hManager);
			return FALSE;
        }
    }
    if ( !QueryServiceConfig2(hService, SERVICE_CONFIG_DESCRIPTION, (LPBYTE)lpsd, cbBufSize, &dwBytesNeeded) ) {
        wsprintf(errorMessage, "QueryServiceConfig2 failed (%d)", GetLastError());
		CloseServiceHandle(hService);
        CloseServiceHandle(hManager);
        return FALSE; 
    }


    // get service status
	if( !QueryServiceStatusEx(hService, SC_STATUS_PROCESS_INFO, NULL, 0, &dwBytesNeeded) ) {
		dwError = GetLastError();
        if ( dwError == ERROR_INSUFFICIENT_BUFFER ) {
            cbBufSize = dwBytesNeeded;
            lstatus = (LPSERVICE_STATUS)LocalAlloc(LMEM_FIXED, cbBufSize);
        } else {
            wsprintf(errorMessage, "QueryServiceStatus failed (%d)", dwError);
			CloseServiceHandle(hService);
            CloseServiceHandle(hManager);
			return FALSE;
        }
    }

	if( !QueryServiceStatusEx(hService, SC_STATUS_PROCESS_INFO, (LPBYTE)lstatus, cbBufSize, &dwBytesNeeded) ) {
        wsprintf(errorMessage, "QueryServiceStatus failed (%d)", GetLastError());
		CloseServiceHandle(hService);
        CloseServiceHandle(hManager);
        return FALSE; 
    }


    // set info struct
	StrCpy(info->name, lpServiceName);
	StrCpy(info->displayName, lpsc->lpDisplayName);
    StrCpy(info->description, lpsd->lpDescription);
	StrCpy(info->exePath, lpsc->lpBinaryPathName);
    info->startupType = lpsc->dwStartType;
	info->isRunning = lstatus->dwCurrentState&SERVICE_RUNNING;


    // cleanup resources
    LocalFree(lpsc);
    LocalFree(lpsd);
	LocalFree(lstatus);
	CloseServiceHandle(hService);
    CloseServiceHandle(hManager);

	return TRUE;
}


/*
 * Start service.
 */
BOOL StartService(LPCTSTR lpServiceName)
{
	SC_HANDLE hManager;                                     
    SC_HANDLE hService;

    // access to service manager
	hManager = _OpenManager(lpMachineName, lpDatabaseName, SC_MANAGER_ALL_ACCESS);
	if ( hManager == NULL ) return FALSE;

    // access to monitor service
    hService = _OpenService(hManager, lpServiceName, SERVICE_ALL_ACCESS);
    if ( hService == NULL ) {
		CloseServiceHandle(hManager);
		return FALSE;
	}

    // start service
    if ( !_StartService(hService) ) {
		CloseServiceHandle(hService);
        CloseServiceHandle(hManager);
        return FALSE; 
    }

	CloseServiceHandle(hService);
    CloseServiceHandle(hManager);

	return TRUE;
}


/*
 * Start service.
 */
BOOL StopService(LPCTSTR lpServiceName)
{
	SC_HANDLE hManager;                                     
    SC_HANDLE hService;
	SERVICE_STATUS status;

    // access to service manager
	hManager = _OpenManager(lpMachineName, lpDatabaseName, SC_MANAGER_ALL_ACCESS);
	if ( hManager == NULL ) return FALSE;

    // access to monitor service
    hService = _OpenService(hManager, lpServiceName, SERVICE_ALL_ACCESS);
    if ( hService == NULL ) {
		CloseServiceHandle(hManager);
		return FALSE;
	}

    // stop service
	if ( !_ControlService(hService, SERVICE_CONTROL_STOP, &status) ) {
		CloseServiceHandle(hService);
        CloseServiceHandle(hManager);
        return FALSE; 
    }

	CloseServiceHandle(hService);
    CloseServiceHandle(hManager);

	return TRUE;
}


/*
 * wait until the service is no longer pending 
 */
BOOL WaitPendingStatus(LPCTSTR lpServiceName)
{
	SC_HANDLE hManager;                                     
    SC_HANDLE hService;
    SERVICE_STATUS_PROCESS status; 
    DWORD dwOldCheckPoint; 
    DWORD dwStartTickCount;
    DWORD dwWaitTime;
    DWORD dwBytesNeeded;
 

    // access to service manager
	hManager = _OpenManager(lpMachineName, lpDatabaseName, SC_MANAGER_ALL_ACCESS);
	if ( hManager == NULL ) return FALSE;

    // access to monitor service
    hService = _OpenService(hManager, lpServiceName, SERVICE_ALL_ACCESS);
    if ( hService == NULL ) {
		CloseServiceHandle(hManager);
		return FALSE;
	}
 

    // Check the status until the service is no longer pending 

	if ( !QueryServiceStatusEx(hService, SC_STATUS_PROCESS_INFO, (LPBYTE)&status, sizeof(SERVICE_STATUS_PROCESS), &dwBytesNeeded ) ) {
        wsprintf(errorMessage, "QueryServiceStatusEx failed (%d)", GetLastError());
		CloseServiceHandle(hService);
        CloseServiceHandle(hManager);
        return FALSE; 
    }
 
    dwStartTickCount = GetTickCount();
    dwOldCheckPoint = status.dwCheckPoint;

    while ( status.dwCurrentState == SERVICE_START_PENDING || status.dwCurrentState == SERVICE_STOP_PENDING ) {
 
        dwWaitTime = status.dwWaitHint / 10;
        if( dwWaitTime < 1000 )
            dwWaitTime = 1000;
        else if ( dwWaitTime > 10000 )
            dwWaitTime = 10000;
        Sleep(dwWaitTime);

        // Check the status again
        if ( !QueryServiceStatusEx(hService, SC_STATUS_PROCESS_INFO, (LPBYTE)&status, sizeof(SERVICE_STATUS_PROCESS), &dwBytesNeeded ) )
			break;
 
        if ( status.dwCheckPoint > dwOldCheckPoint ) {
            // The service is making progress.
            dwStartTickCount = GetTickCount();
            dwOldCheckPoint = status.dwCheckPoint;
        } else {
            if ( (GetTickCount() - dwStartTickCount) > status.dwWaitHint ) {
                // No progress made within the wait hint
                break;
            }
        }
    } 

	CloseServiceHandle(hService);
    CloseServiceHandle(hManager);

	return TRUE;
}


/*
 * Get services list size
 */
BOOL GetServicesListSize(int *size)
{
    SC_HANDLE hManager;
	ENUM_SERVICE_STATUS serviceStatus, *lpServices;
	DWORD pcbBytesNeeded, lpServicesReturned, lpResumeHandle = 0;
	BOOL retVal;

    // access to service manager
	hManager = _OpenManager(lpMachineName, lpDatabaseName, SC_MANAGER_ENUMERATE_SERVICE);
	if ( hManager == NULL ) return FALSE;

	// enum services
	retVal = EnumServicesStatus(hManager, SERVICE_WIN32, SERVICE_STATE_ALL, &serviceStatus, sizeof(serviceStatus), &pcbBytesNeeded, &lpServicesReturned, &lpResumeHandle);
	DWORD err = GetLastError();
              
    // EnumServicesStatus needs more memory space?
    if ((retVal == FALSE) || err == ERROR_MORE_DATA) 
    {
	    DWORD dwBytes = pcbBytesNeeded + sizeof(ENUM_SERVICE_STATUS);
		lpServices = new ENUM_SERVICE_STATUS[dwBytes];		
        EnumServicesStatus(hManager, SERVICE_WIN32, SERVICE_STATE_ALL, lpServices, dwBytes, &pcbBytesNeeded, &lpServicesReturned, &lpResumeHandle);
	}
    *size = lpServicesReturned;

    CloseServiceHandle(hManager);

	return TRUE;
}


/*
 * Get services list
 */
BOOL GetServicesList(SERVICEINFOSTRUCT info[])
{
    SC_HANDLE hManager;
	ENUM_SERVICE_STATUS serviceStatus, *lpServices;
	DWORD pcbBytesNeeded, lpServicesReturned, lpResumeHandle = 0;
	BOOL retVal;

    // access to service manager
	hManager = _OpenManager(lpMachineName, lpDatabaseName, SC_MANAGER_ENUMERATE_SERVICE);
	if ( hManager == NULL ) return FALSE;

	// enum services
	retVal = EnumServicesStatus(hManager, SERVICE_WIN32, SERVICE_STATE_ALL, &serviceStatus, sizeof(serviceStatus), &pcbBytesNeeded, &lpServicesReturned, &lpResumeHandle);
	DWORD err = GetLastError();


    // EnumServicesStatus needs more memory space?
    if ((retVal == FALSE) || err == ERROR_MORE_DATA) 
    {
	    DWORD dwBytes = pcbBytesNeeded + sizeof(ENUM_SERVICE_STATUS);
		lpServices = new ENUM_SERVICE_STATUS[dwBytes];		
        EnumServicesStatus(hManager, SERVICE_WIN32, SERVICE_STATE_ALL, lpServices, dwBytes, &pcbBytesNeeded, &lpServicesReturned, &lpResumeHandle);

	    for(int ii = 0; ii < (int)lpServicesReturned; ii++)
        {
	        StrCpy(info[ii].name, lpServices[ii].lpServiceName);
	        StrCpy(info[ii].displayName, lpServices[ii].lpDisplayName);
        }
	}

    CloseServiceHandle(hManager);

	return TRUE;
}



// Access to service manager
SC_HANDLE _OpenManager(LPCTSTR lpMachineName, LPCTSTR lpDatabaseName, DWORD dwDesiredAccess)
{
	SC_HANDLE hManager = OpenSCManager(lpMachineName, lpDatabaseName, dwDesiredAccess);

	if ( hManager == NULL ) {
		DWORD dwError = GetLastError();
		switch ( dwError )
		{
		    case ERROR_ACCESS_DENIED:
				wsprintf(errorMessage, "OpenSCManager failed (%d)\n%s", dwError, "The requested access was denied.");
			break;

		    case ERROR_DATABASE_DOES_NOT_EXIST:
				wsprintf(errorMessage, "OpenSCManager failed (%d)\n%s", dwError, "The specified database does not exist.");
			break;

		    case ERROR_INVALID_PARAMETER:
				wsprintf(errorMessage, "OpenSCManager failed (%d)\n%s", dwError, "A specified parameter is invalid.");
			break;

		    default:
				wsprintf(errorMessage, "OpenSCManager failed (%d)", dwError);
		}
		return NULL;
	}

	return hManager;
}


// Access to monitor service
SC_HANDLE _OpenService(SC_HANDLE hManager, LPCTSTR lpServiceName, DWORD dwDesiredAccess)
{
	SC_HANDLE hService = OpenService(hManager, lpServiceName, dwDesiredAccess);

	if ( hService == NULL ) {
		DWORD dwError = GetLastError();
		switch ( dwError )
		{
		    case ERROR_ACCESS_DENIED:
				wsprintf(errorMessage, "OpenService failed (%d)\n%s", dwError, "The handle does not have access to the service.");
			break;

		    case ERROR_INVALID_HANDLE:
				wsprintf(errorMessage, "OpenService failed (%d)\n%s", dwError, "The specified handle is invalid.");
			break;

		    case ERROR_INVALID_NAME:
				wsprintf(errorMessage, "OpenService failed (%d)\n%s", dwError, "The specified service name is invalid.");
			break;

		    case ERROR_SERVICE_DOES_NOT_EXIST:
				wsprintf(errorMessage, "OpenService failed (%d)\n%s", dwError, "The specified service does not exist.");
			break;

		    default:
				wsprintf(errorMessage, "OpenService failed (%d)", dwError);
		}
		return NULL;
	}

	return hService;
}


// Access to Control Service Function
BOOL _ControlService(SC_HANDLE hService, DWORD dwControl, LPSERVICE_STATUS lpServiceStatus)
{
    if ( !ControlService(hService, dwControl, lpServiceStatus) ) {
		DWORD dwError = GetLastError();
		switch ( dwError )
		{
		    case ERROR_ACCESS_DENIED:
				wsprintf(errorMessage, "ControlService failed (%d)\n%s", dwError, "The handle does not have the required access right.");
			break;

		    case ERROR_DEPENDENT_SERVICES_RUNNING:
				wsprintf(errorMessage, "ControlService failed (%d)\n%s", dwError, "The service cannot be stopped because\nother running services are dependent on it.");
			break;

		    case ERROR_INVALID_HANDLE:
				wsprintf(errorMessage, "ControlService failed (%d)\n%s", dwError, "The specified handle is no longer valid.");
			break;

		    case ERROR_INVALID_PARAMETER:
				wsprintf(errorMessage, "ControlService failed (%d)\n%s", dwError, "The requested control code is undefined.");
			break;

		    case ERROR_INVALID_SERVICE_CONTROL:
				wsprintf(errorMessage, "ControlService failed (%d)\n%s", dwError, "The requested control code is not valid, or it is unacceptable to the service.");
			break;

		    case ERROR_SERVICE_CANNOT_ACCEPT_CTRL:
				wsprintf(errorMessage, "ControlService failed (%d)\n%s", dwError, "The requested control code cannot be sent to the service.");
			break;

		    case ERROR_SERVICE_NOT_ACTIVE:
				wsprintf(errorMessage, "ControlService failed (%d)\n%s", dwError, "The service has not been started.");
			break;

		    case ERROR_SERVICE_REQUEST_TIMEOUT:
				wsprintf(errorMessage, "ControlService failed (%d)\n%s", dwError, "ERROR_SERVICE_REQUEST_TIMEOUT.");
			break;

		    case ERROR_SHUTDOWN_IN_PROGRESS:
				wsprintf(errorMessage, "ControlService failed (%d)\n%s", dwError, "The system is shutting down.");
			break;

		    default:
				wsprintf(errorMessage, "ControlService failed (%d)", dwError);
		}
		return FALSE;
	}

    return TRUE;
}


// Start Service
BOOL _StartService(SC_HANDLE hService)
{
	if ( !StartService(hService, 0, NULL) ) {
		DWORD dwError = GetLastError();
		switch ( dwError )
		{
		    case ERROR_ACCESS_DENIED:
				wsprintf(errorMessage, "ControlService failed (%d)\n%s", dwError, "The handle does not have the SERVICE_START access right.");
			break;

		    case ERROR_INVALID_HANDLE:
				wsprintf(errorMessage, "ControlService failed (%d)\n%s", dwError, "The handle is invalid.");
			break;

		    case ERROR_PATH_NOT_FOUND:
				wsprintf(errorMessage, "ControlService failed (%d)\n%s", dwError, "The service binary file could not be found.");
			break;

		    case ERROR_SERVICE_ALREADY_RUNNING:
				wsprintf(errorMessage, "ControlService failed (%d)\n%s", dwError, "An instance of the service is already running.");
			break;

		    case ERROR_SERVICE_DATABASE_LOCKED:
				wsprintf(errorMessage, "ControlService failed (%d)\n%s", dwError, "The database is locked.");
			break;

		    case ERROR_SERVICE_DEPENDENCY_DELETED:
				wsprintf(errorMessage, "ControlService failed (%d)\n%s", dwError, "The service depends on a service that does not exist\nor has been marked for deletion.");
			break;

		    case ERROR_SERVICE_DEPENDENCY_FAIL:
				wsprintf(errorMessage, "ControlService failed (%d)\n%s", dwError, "The service depends on another service that has failed to start.");
			break;

		    case ERROR_SERVICE_DISABLED:
				wsprintf(errorMessage, "ControlService failed (%d)\n%s", dwError, "The service has been disabled.");
			break;

		    case ERROR_SERVICE_LOGON_FAILED:
				wsprintf(errorMessage, "ControlService failed (%d)\n%s", dwError, "The service did not start due to a logon failure.\nThis error occurs if the service is configured to run\nunder an account that does not have the 'Log on as a service' right.");
			break;

		    case ERROR_SERVICE_MARKED_FOR_DELETE:
				wsprintf(errorMessage, "ControlService failed (%d)\n%s", dwError, "The service has been marked for deletion.");
			break;

		    case ERROR_SERVICE_NO_THREAD:
				wsprintf(errorMessage, "ControlService failed (%d)\n%s", dwError, "A thread could not be created for the service.");
			break;

		    case ERROR_SERVICE_REQUEST_TIMEOUT:
				wsprintf(errorMessage, "ControlService failed (%d)\n%s", dwError, "ERROR_SERVICE_REQUEST_TIMEOUT.");
			break;

		    default:
				wsprintf(errorMessage, "ControlService failed (%d)", dwError);
		}
		return FALSE;
	}

    return TRUE;
}
