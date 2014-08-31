// MSI_VerifyUT2K4Dir.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "MSI_VerifyUT2K4Dir.h"


#ifdef _MANAGED
#pragma managed(push, off)
#endif

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
    return TRUE;
}

#ifdef _MANAGED
#pragma managed(pop)
#endif

#define MB_RETURN_ERROR(text){ \
	::MessageBox(NULL, text, TEXT("Error"), MB_OK); \
	return ERROR_INSTALL_FAILURE; }


wstring MsiGetPropertyStl( MSIHANDLE hInstall, LPCWSTR szName )
{
	TCHAR* szValueBuf = NULL;
	DWORD cchValueBuf = 0;
	UINT uiStat =  MsiGetProperty(hInstall, szName, TEXT(""), &cchValueBuf);
	if (ERROR_MORE_DATA == uiStat)
	{
		++cchValueBuf; // on output does not include terminating null, so add 1
		szValueBuf = new TCHAR[cchValueBuf];
		if (szValueBuf)
		{
			uiStat = MsiGetProperty(hInstall, szName, szValueBuf, &cchValueBuf);
		}
	}

	if (ERROR_SUCCESS != uiStat)
	{
		if (szValueBuf != NULL) 
			delete [] szValueBuf;
		return wstring(TEXT(""));
	}

	wstring result = szValueBuf;
	delete [] szValueBuf;
	return result;
}

UINT __stdcall VerifyUT2K4Dir(MSIHANDLE hInstaller)
{
	// Reset result property to "FALSE"
	if( ::MsiSetProperty(hInstaller, TEXT("MB_UT2K4DIR_VALID"), TEXT("0") ) != ERROR_SUCCESS )
		MB_RETURN_ERROR( TEXT("VerifyUT2K4Dir(Get MB_UT2K4DIR_VALID) failed.") );

	// Get paths
	wstring installPath = MsiGetPropertyStl(hInstaller, TEXT("INSTALLDIR"));
	wstring verifyPath = installPath + wstring(TEXT("./System/UT2004.exe"));

	// See if path exists
	if( ::PathFileExists(verifyPath.c_str()) == TRUE )
	{
		// Set result property
		if( ::MsiSetProperty(hInstaller, TEXT("MB_UT2K4DIR_VALID"), TEXT("1") ) != ERROR_SUCCESS )
			MB_RETURN_ERROR( TEXT("VerifyUT2K4Dir(Set MB_UT2K4DIR_VALID) failed.") );
	}

	// Let the installer know we completed successfully
	return ERROR_SUCCESS;
}

UINT __stdcall VerifyUT2K4Version(MSIHANDLE hInstaller)
{
	// Reset result property to "FALSE"
	if( ::MsiSetProperty(hInstaller, TEXT("MB_UT2K4VERSION"), TEXT("0") ) != ERROR_SUCCESS )
		MB_RETURN_ERROR( TEXT("VerifyUT2K4Version(Get MB_UT2K4VERSION) failed.") );

	// Get paths
	wstring installPath = MsiGetPropertyStl(hInstaller, TEXT("INSTALLDIR"));
	wstring verifyPath = installPath + wstring(TEXT("./System/UT2004.ini"));

	// Check path
	if( ::PathFileExists(verifyPath.c_str()) == TRUE )
	{
		// Read version from UT2004.ini
		TCHAR szInstallVersion[MAX_PATH] = {0};
		DWORD dInstallVersionLen = MAX_PATH;
		DWORD rInstallVersion = ::GetPrivateProfileString( TEXT("FirstRun"), TEXT("FirstRun"), TEXT("0"), szInstallVersion, dInstallVersionLen, verifyPath.c_str() );

		// Set result property
		if( ::MsiSetProperty(hInstaller, TEXT("MB_UT2K4VERSION"), szInstallVersion ) != ERROR_SUCCESS )
			MB_RETURN_ERROR( TEXT("VerifyUT2K4Version(Set MB_UT2K4VERSION) failed.") );
	}

	// Let the installer know we completed successfully
	return ERROR_SUCCESS;
}