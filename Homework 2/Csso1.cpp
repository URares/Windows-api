#include <Windows.h>
#include <iostream>
#include <stdio.h>
#include <Tlhelp32.h>
using namespace std;
struct InfoProces
{
  DWORD pid;
  DWORD ppid;
  char exeName[256];
};

struct ProcessList
{
  int count;
  InfoProces procese[2048];
};

int main()
{
  HANDLE hProcess;
  PROCESSENTRY32 pe32;
  //cer un snapshot la procese
  hProcess = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
  if (hProcess == INVALID_HANDLE_VALUE)
  {
    printf("CreateToolhelp32Snapshot failed.err = %d \n", GetLastError());
    return (-1);
  }

  //initializez dwSize cu dimensiunea structurii.
  pe32.dwSize = sizeof(PROCESSENTRY32);
  //obtin informatii despre primul proces
  if (!Process32First(hProcess, &pe32))
  {
    printf("Process32First failed. err = %d \n", GetLastError());
    CloseHandle(hProcess); //inchidem snapshot-ul
    return (-1);
  }
  int i = 0;
  ProcessList pList;

  do
  {
    pList.count = i;
    strcpy(pList.procese[i].exeName, pe32.szExeFile);
    pList.procese[i].pid = pe32.th32ProcessID;
    pList.procese[i].ppid = pe32.th32ParentProcessID;
    i++;
  } while (Process32Next(hProcess, &pe32)); //trec la urmatorul proces
  //inched handle-ul catre snapshot
  CloseHandle(hProcess);

  //FileMapping
  HANDLE hFileMap;
  BOOL     bResult;
  ProcessList* plBuffer = NULL;
  size_t   szBuffer = sizeof(pList);

  hFileMap = CreateFileMappingA(
      INVALID_HANDLE_VALUE,
      NULL,
      PAGE_READWRITE,
      0,
      1024 * 1024,
      "Local\\tema2");
  if(hFileMap == NULL) {
        cout<<"CreateFileMapping Error "<<GetLastError();
    }
    cout<<"CreateFileMapping Success\n";
    
    plBuffer = (ProcessList*)MapViewOfFileEx(
    hFileMap,
    FILE_MAP_ALL_ACCESS,
    0,
    0,
    1024 * 1024,
    NULL
    );
      if(plBuffer == NULL) {
        cout<<"MapViewOfFile Error "<<GetLastError();
    }
        cout<<"MapViewOfFile Success\n";


cout<<pList.count<<endl;
memcpy(plBuffer, &pList,szBuffer);
    

    bResult = UnmapViewOfFile(plBuffer);
    if(bResult == 0) {
        cout<<"UnMapView Failed"<<GetLastError();
    }
    cout<<"UnmapView Success\n";
    
    CloseHandle(hFileMap);
  system("pause");
  return 0;
}