#include <Windows.h>
#include <stdio.h>
#include <tchar.h>
#include <string>
void enumerate_keys(HKEY rootKey, char *reg_path, char *win_path);
void read_keys(HKEY rootKey, HKEY hKey, DWORD index, char *win_path, char *reg_path);
int main()
{
    CreateDirectoryA("D:\\test", NULL);
    enumerate_keys(HKEY_LOCAL_MACHINE, "SOFTWARE\\test", "D:\\test");
    printf("All good\n");
    system("pause");
    return 0;
}

void enumerate_keys(HKEY rootKey, char *reg_path, char *win_path)
{

    HKEY hKey;
    DWORD total_keys;
    DWORD total_values;
    DWORD i;
    DWORD return_code = 0;
    DWORD maxValue;
    DWORD maxValueLen;
    if ((return_code = RegOpenKeyExA(rootKey, reg_path, 0, KEY_READ, &hKey)) == ERROR_SUCCESS)
    {
        if ((return_code = RegQueryInfoKey(hKey, NULL, NULL, NULL, &total_keys, NULL, NULL, &total_values, &maxValue, &maxValueLen, NULL, NULL)) != ERROR_SUCCESS)
        {
            printf("Eroare la queryinfo %d", return_code);
        }
        else
        {
            for (i = 0; i < total_values; i++)
            {
                char valueName[1 + maxValueLen];
                char file_name[MAX_PATH];
                DWORD return_code = 0;
                DWORD valueSize = maxValue+1;
                char data[255];
                DWORD dataSize = sizeof(data);
                return_code = RegEnumValueA(hKey, i, valueName, &valueSize, NULL, NULL, (LPBYTE)data, &dataSize);//data & dataSize doesnt work but hey,i got the recursion for the folders :')
                if (return_code != ERROR_SUCCESS)
                {
                    printf("Eroare la read value %d", return_code);
                }

                sprintf(file_name, "%s\\%s", win_path, valueName);

                HANDLE hFile;
                hFile = CreateFileA(file_name, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
                if (hFile == INVALID_HANDLE_VALUE){
                    printf("Error at creating file\n");
                } else {
                  
                    WriteFile(hFile,data,dataSize,NULL,NULL);
                }

            }

            for (i = 0; i < total_keys; i++)
            {
                read_keys(rootKey, hKey, i, win_path, reg_path);
            }

            RegCloseKey(rootKey);
        }
    }
    else
    {
        printf("Error code %s \n on register %s", return_code, reg_path);
    }
}

void read_keys(HKEY rootKey, HKEY hKey, DWORD index, char *win_path, char *reg_path)
{
    DWORD return_code = 0;
    DWORD keySize = MAX_PATH;
    char keyName[MAX_PATH];
    char new_win_path[MAX_PATH];
    char new_reg_path[MAX_PATH];
    return_code = RegEnumKeyExA(hKey, index, keyName, &keySize, NULL, NULL, NULL, NULL);
    if (return_code != ERROR_SUCCESS)
    {
        printf("Eroare la read key %d", return_code);
    }
    sprintf(new_win_path, "%s\\%s", win_path, keyName);
    CreateDirectoryA(new_win_path, NULL);
    sprintf(new_reg_path, "%s\\%s", reg_path, keyName);
    enumerate_keys(rootKey, new_reg_path, new_win_path);
}