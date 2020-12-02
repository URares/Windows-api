#include <Windows.h>
#include <iostream>
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

//functie preluata -> https://docs.microsoft.com/en-us/windows/win32/secauthz/privilege-constants
BOOL EnablePrivilege()
{
    LUID PrivilegeRequired ;
    DWORD dwLen = 0, iCount = 0;
    BOOL bRes = FALSE;
    HANDLE hToken = NULL;
    BYTE *pBuffer = NULL;
    TOKEN_PRIVILEGES* pPrivs = NULL;

    bRes = LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &PrivilegeRequired);
    if( !bRes) return FALSE;
    hToken = GetCurrentThread();
    bRes = OpenThreadToken(GetCurrentThread(), TOKEN_QUERY | TOKEN_ADJUST_PRIVILEGES, TRUE, &hToken); 
    if(!bRes) return FALSE;
    
    bRes = GetTokenInformation(hToken, TokenPrivileges, NULL, 0, &dwLen);
    if (TRUE == bRes)
    {
        CloseHandle(hToken);
        return FALSE;
    }
    pBuffer = (BYTE*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwLen);
    if(NULL == pBuffer) return FALSE;
    
    if (!GetTokenInformation(hToken, TokenPrivileges, pBuffer, dwLen, &dwLen)) 
    {
        CloseHandle(hToken);
        HeapFree(GetProcessHeap(), 0, pBuffer);
        return FALSE;
    }

    // Iterate through all the privileges and enable the one required
    bRes = FALSE;
    pPrivs = (TOKEN_PRIVILEGES*)pBuffer;
    for(iCount = 0; iCount < pPrivs->PrivilegeCount; iCount++)
    {
        if (pPrivs->Privileges[iCount].Luid.LowPart == PrivilegeRequired.LowPart &&
          pPrivs->Privileges[iCount].Luid.HighPart == PrivilegeRequired.HighPart )
        {
            pPrivs->Privileges[iCount].Attributes |= SE_PRIVILEGE_ENABLED;
            // here it's found
            bRes = AdjustTokenPrivileges(hToken, FALSE, pPrivs, dwLen, NULL, NULL);
            break;
        }
    }

    CloseHandle(hToken);
    HeapFree(GetProcessHeap(), 0, pBuffer);    
    return bRes;
}


int check_is_root(ProcessList* plBuffer,DWORD pp_id) {
    //procesul 0 este system process ppid 0 pid 0 
    for(int j = 1 ; j<plBuffer->count;j++){
            if(plBuffer->procese[j].pid == pp_id )
                return false;
    }
    return true;
}

void check_childrens(ProcessList* plBuffer,DWORD p_id,DWORD spaces) {
    
    DWORD process_array[1024];
    DWORD total_process = 0, nr ;
    for(int j = 1 ; j<plBuffer->count;j++){
            if(plBuffer->procese[j].ppid == p_id ){
                for(int k=0;k<=spaces;k++)
                cout<<"   ";
                 cout<<plBuffer->procese[j].exeName<<" "<<plBuffer->procese[j].pid<<endl;
                 check_childrens(plBuffer,plBuffer->procese[j].pid,spaces+1);
            }
    }
    
   

}


int main(){

    DWORD killProcess;
    LUID PrivilegeRequired ;
    BOOL bPriv = LookupPrivilegeValueA(NULL,SE_DEBUG_NAME,&PrivilegeRequired);
    HANDLE   hFileMap;
    BOOL     bResult;
    bResult = EnablePrivilege();
    if(bResult == 0 ) {
        cout<<"EnablePrivilege error"<<GetLastError();
    } else {
        cout<<"Enable Prilege Success \n";
    }
    hFileMap = OpenFileMappingA(
        FILE_MAP_ALL_ACCESS,
        FALSE,
        "Local\\tema2"
    );
    

    if(hFileMap == NULL) {
        cout<<"OpenFileMap Error "<<GetLastError();
    }
    else {
    cout<<"OpenFileMap Success\n";
    }
    //MapViewOfFile
     ProcessList*  plBuffer = (ProcessList*)MapViewOfFileEx(
    hFileMap,
    FILE_MAP_ALL_ACCESS,
    0,
    0,
    1024 * 1024,
    NULL
    );
    if(plBuffer == NULL) {
        cout<<"MapViewOfFile Error "<<GetLastError();
    } else {
        cout<<"MapViewOfFile Success\n";
    }
printf("Lista cu Procese:\n");

    for(int i = 0 ; i <= plBuffer->count ; i++ ) {

        printf("NAME: %s PPID: %d PID : %d \n",plBuffer->procese[i].exeName,plBuffer->procese[i].ppid,plBuffer->procese[i].pid);

    }




 for(int i = 0 ; i <= plBuffer->count ; i++ ) { 
        printf("NAME: %s PPID: %d PID : %d \n",plBuffer->procese[i].exeName,plBuffer->procese[i].ppid,plBuffer->procese[i].pid);
    }

cout<<"=========== ROOTS =============\n";
int tree_number=0;
DWORD root_to_kill[1024];
 for(int i = 0 ; i <= plBuffer->count ; i++ ) { 

       if( check_is_root(plBuffer,plBuffer->procese[i].ppid ) == true ){

            root_to_kill[tree_number]=plBuffer->procese[i].ppid;
            cout<<"Arbore nr :"<<tree_number<<endl;
            cout<<plBuffer->procese[i].exeName<<" "<<plBuffer->procese[i].pid<<endl;
            check_childrens(plBuffer,plBuffer->procese[i].pid,0);
            tree_number++;
       }

    }


 cout<<"Which process tree to kill: ";
 cin>>killProcess;


HANDLE hProcess = OpenProcess(PROCESS_TERMINATE,FALSE,killProcess);
    if(hProcess == NULL) {
        cout<<"OpenProcess Error"<<GetLastError();
    }


   BOOL tProcess = TerminateProcess(hProcess,22);

   if(tProcess == 0) {
       cout<<"Terminate process ERROR"<<GetLastError();
   } else {
       cout<<"Process Terminated \n";
   }

    bResult = UnmapViewOfFile(plBuffer);
    if(bResult == FALSE) {
        cout<<"UnMapView Failed"<<GetLastError();
    } else {
    cout<<"UnmapView Success\n";
    }

    CloseHandle(hFileMap);
    system("pause");
    return 0;
}