#include "fileOperation.hpp"

#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <strsafe.h>
#pragma comment(lib, "User32.lib")
#include <AtlBase.h>

#include <AtlConv.h>
#include <vector>
void DisplayErrorBox(LPTSTR lpszFunction);

const std::vector<std::string> getFileNames(const char *directory)
{
   WIN32_FIND_DATA ffd;
  // LARGE_INTEGER filesize;
   TCHAR szDir[MAX_PATH];
   HANDLE hFind = INVALID_HANDLE_VALUE;
   DWORD dwError=0;
   CA2T directory_path(directory);
   // Check that the input path plus 3 is not longer than MAX_PATH.
   // Three characters are for the "\*" plus NULL appended below.

   // Prepare string for use with FindFile functions.  First, copy the
   // string to a buffer, then append '\*' to the directory name.

   StringCchCopy(szDir, MAX_PATH, directory_path);
   StringCchCat(szDir, MAX_PATH, TEXT("*"));

   // Find the first file in the directory.

   hFind = FindFirstFile(szDir, &ffd);
   // List all the files in the directory with some info about them.
   std::vector<std::string> fileNames;
   do
   {
	   if(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		   continue;	//if is a directory

		char charArr[260];
		char defaultChar = ' ';
		WideCharToMultiByte(CP_ACP,0,ffd.cFileName,-1, charArr,260,& defaultChar, NULL);
		fileNames.push_back(std::string(charArr));
   }
   while (FindNextFile(hFind, &ffd) != 0);
 
   dwError = GetLastError();
   if (dwError != ERROR_NO_MORE_FILES) 
   {
      DisplayErrorBox(TEXT("FindFirstFile"));
   }

   FindClose(hFind);
   return fileNames;
}


void DisplayErrorBox(LPTSTR lpszFunction) 
{ 
    // Retrieve the system error message for the last-error code

    LPVOID lpMsgBuf;
    LPVOID lpDisplayBuf;
    DWORD dw = GetLastError(); 

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &lpMsgBuf,
        0, NULL );

    // Display the error message and clean up

    lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT, 
        (lstrlen((LPCTSTR)lpMsgBuf)+lstrlen((LPCTSTR)lpszFunction)+40)*sizeof(TCHAR)); 
    StringCchPrintf((LPTSTR)lpDisplayBuf, 
        LocalSize(lpDisplayBuf) / sizeof(TCHAR),
        TEXT("%s failed with error %d: %s"), 
        lpszFunction, dw, lpMsgBuf); 
    MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK); 

    LocalFree(lpMsgBuf);
    LocalFree(lpDisplayBuf);
}