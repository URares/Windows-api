#include <Windows.h>
#include <cstdio>
#include <stdlib.h>
#include <time.h>
#include <iostream>
using namespace std;
struct numbers {
    DWORD a;
    DWORD b;
};
int main()
{
    HANDLE   hFileMap;
    hFileMap = OpenFileMappingA(FILE_MAP_ALL_ACCESS, FALSE, "Local\\tema5");
    if(hFileMap == NULL) {
        cout<<"OpenFileMap Error "<<GetLastError();
    }
   
    numbers*  nBuffer = (numbers*)MapViewOfFileEx(
    hFileMap,
    FILE_MAP_READ,
    0,
    0,
    1024 * 1024,
    NULL
    );

        if(nBuffer == NULL) {
        cout<<"MapViewOfFile Error "<<GetLastError();
    } 

    HANDLE hMutex;
    hMutex = OpenMutexA(SYNCHRONIZE,false,"Mutex5");

    for(int i = 0 ;i<3;i++){
        
    cout<<WaitForSingleObject(hMutex,INFINITE)<<endl;

    cout<<GetLastError()<<endl;

    cout<<nBuffer->a<<" "<<nBuffer->b<<endl;
    
    ReleaseMutex(hMutex);
   
    }
    

    CloseHandle(hMutex);
    CloseHandle(hFileMap);
    return 0;
}