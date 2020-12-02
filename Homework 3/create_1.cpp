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

    //CreateProces s2
    PROCESS_INFORMATION pi;
    STARTUPINFO si;
    memset(&si, 0, sizeof(si));
    si.cb = sizeof(si);

    SECURITY_ATTRIBUTES sAtrb;
    sAtrb.nLength = sizeof(sAtrb);
    sAtrb.lpSecurityDescriptor = NULL;
    sAtrb.bInheritHandle = FALSE;
 

    HANDLE hEvent;

    hEvent = CreateEventA(&sAtrb, true, false, "Local\\Event5");

      if (!CreateProcess("W:\\Facultate\\CSSO\\mutex2.exe", NULL, &sAtrb, &sAtrb, FALSE, 0, NULL, NULL, &si, &pi))
    {
        printf("Cannot create process.\n");
        return 0;
    }

    if (hEvent == NULL)
    {
        cout << "hEvent " << GetLastError() << endl;
    }

    for (int i = 0; i < 3; i++)
    {
        nBuffer = (numbers *)MapViewOfFileEx(hFileMap, FILE_MAP_ALL_ACCESS, 0, 0, 1024 * 1024, NULL);

        n1.a = rand() % 100;
        n1.b = n1.a * 2;

        printf("Process 1\n a= %d ; b= %d\n", n1.a, n1.b);

        memcpy(nBuffer, &n1, sizeof(n1));


        SetEvent(hEvent);
        system("pause");
    }

    if (!UnmapViewOfFile(nBuffer))
        {
            cout << "UnMapView Failed" << GetLastError();
        }

    cout << GetLastError() << endl;

    CloseHandle(hFileMap);
    CloseHandle(hEvent);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    return 0;
}
