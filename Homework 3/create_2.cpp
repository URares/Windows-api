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
    hFileMap = OpenFileMappingA(FILE_MAP_READ, true, "Local\\tema5");
    if(hFileMap == NULL) {
        cout<<hFileMap<<endl;
        cout<<"OpenFileMap Error "<<GetLastError();
        system("pause");
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

    HANDLE hEvent;
    hEvent = OpenEventA(SYNCHRONIZE ,false, "Local\\Event5");

    for(int i = 0 ;i<3;i++){
      system("pause");
    cout<<WaitForSingleObject(hEvent,INFINITE)<<endl;
    cout<<GetLastError()<<endl;
    cout<<nBuffer->a<<" "<<nBuffer->b<<endl;

    
    ResetEvent(hEvent);
  
    }
    

    // CloseHandle(hEvent);
    // CloseHandle(hFileMap);
    CloseHandle(hEvent);
    CloseHandle(hFileMap);
    system("pause");
    return 0;
}