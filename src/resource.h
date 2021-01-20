/*                                                     ''~``
 * @(#)resource.h 1.0  2008/10/13                     ( o o )
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
 * | Module....: resource header file                                 |
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


// application constants
#define MANIFEST_RESOURCE_ID		  1

#define IDS_APP_TITLE				100
#define IDC_APP_WCLASSNAME   		101
#define IDS_TRY_TOOLTIP				110
#define IDS_ABOUT_TEXT              111
#define IDS_ABOUT_WEB				120
#define IDS_SERVICE_STARTED         112
#define IDS_SERVICE_NOT_STARTED     113
#define IDS_SERVICE_STOPPED         114
#define IDS_SERVICE_NOT_STOPPED     115


// icons constants
#define IDI_APP_ICON				200
#define IDI_NOTIFYICON_STOPPED		201
#define IDI_NOTIFYICON_RUNNING		202
#define IDI_NOTIFYICON_ALL_RUNNING	203

// bitmaps constants
#define IDB_LOGO   				    300
#define IDB_BACKGROUND				301


// notifyicon constants
#define NOTIFYICON_ID				400
#define NOTIFYICON_MESSAGE			WM_USER+1


#define IDT_TIMER                   500


// main menu constants
#define IDR_APP_MENU				1000
#define ID_CMD_PROPERTIES			2000
#define ID_CMD_START_SERVICE		3000
#define ID_CMD_STOP_SERVICE			4000
#define ID_CMD_CONFIGURE            5000
#define ID_CMD_EXIT					6000
#define ID_CMD_ABOUT	    		7000

// properties dialog constants
#define IDD_PROPERTIES				2101
#define IDC_STATIC_NAME				2110
#define IDC_EDIT_DISPLAYNAME		2111
#define IDC_EDIT_DESCRIPTION		2112
#define IDC_EDIT_EXEPATH			2113
#define IDC_COMBO_STARTUPTYPE		2114
#define IDC_STATIC_STATUS			2115
#define IDSTART						2116
#define IDSTOP						2117
#define IDRESTART					2118

// configure dialog constants
#define IDD_CONFIGURE				5101
#define ID_SERVICESLIST				5111

// about dialog constants
#define IDD_ABOUT					7101
#define ID_LICENSE					7111
#define ID_WEB						7112

#define IDD_LICENSE					7501
