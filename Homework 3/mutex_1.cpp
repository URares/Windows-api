#include <Windows.h>
#include <cstdio>
#include <stdlib.h>
#include <time.h>
#include <iostream>
using namespace std;
struct numbers
{
    DWORD a;
    DWORD b;
};
int main()
{
    numbers n1;
    srand(time(0));
    //FileMapping
    HANDLE hFileMap;
    numbers *nBuffer = NULL;
    size_t szBuffer = sizeof(numbers);

    hFileMap = CreateFileMappingA(
        INVALID_HANDLE_VALUE,
        NULL,
        PAGE_READWRITE,
        0,
        1024 * 1024,
        "Local\\tema5");
    if (hFileMap == NULL)
    {
        cout << "CreateFileMapping Error " << GetLastError();
    }

    //Mutex

    HANDLE hMutex;
    SECURITY_ATTRIBUTES sAtrb;

    sAtrb.nLength = sizeof(sAtrb);
    sAtrb.lpSecurityDescriptor = NULL;
    sAtrb.bInheritHandle = TRUE;
    hMutex = CreateMutexA(&sAtrb,false, "Mutex5");

    //Interez de 300 de ori

    nBuffer = (numbers *)MapViewOfFileEx(hFileMap, FILE_MAP_ALL_ACCESS, 0, 0, 1024 * 1024, NULL);

    if (nBuffer == NULL)
    {
        cout << "MapViewOfFile Error " << GetLastError();
    }

    //CreateProces s2
    PROCESS_INFORMATION pi;
    STARTUPINFO si;
    memset(&si, 0, sizeof(si));
    si.cb = sizeof(si);

    if (!CreateProcess("W:\\Facultate\\CSSO\\mutex2.exe", NULL, &sAtrb, &sAtrb, FALSE, 0, NULL, NULL, &si, &pi))
    {
        printf("Cannot create process.\n");
        return 0;
    }
    for (int i = 0; i < 3; i++)
    {
        WaitForSingleObject(hMutex,INFINITE);
        n1.a = rand() % 100;
        n1.b = n1.a * 2;
        cout << GetLastError() << endl;
        printf("Process 1\n a= %d ; b= %d\n", n1.a, n1.b);

        memcpy(nBuffer, &n1, sizeof(n1));
          
          
        ReleaseMutex(hMutex);
    }
  
    
    system("pause");

     if (!UnmapViewOfFile(nBuffer))
        {
            cout << "UnMapView Failed" << GetLastError();
        }
    CloseHandle(hMutex);
    CloseHandle(hFileMap);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    //Filemapping Done

    return 0;
}
