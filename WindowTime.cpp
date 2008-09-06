#define _CRT_SECURE_NO_DEPRECATE

#include <windows.h>
#include <iostream>
#include <ctime>
#include <cstdio>

using namespace std;

void showmsg(DWORD err)
{
    LPVOID lpMsgBuf;
    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,
        NULL,
        err,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &lpMsgBuf,
        0,
        NULL );
    //MessageBox(NULL,(LPCSTR)lpMsgBuf, "Error", MB_OK);
    //cerr<<lpMsgBuf<<endl;
    LocalFree(lpMsgBuf);
}


int main(int argc,char *argv[])
{
    HKEY hKey;

    DWORD it;
    DWORD type;
    DWORD size;

    bool found=false;

    //设置注册表中相关的路径

    LPCTSTR datepath="Software\\Microsoft\\Windows NT\\CurrentVersion";
    LPCTSTR item="InstallDate";
    LPCTSTR datepath2="Software\\Microsoft\\Windows\\CurrentVersion";
    LPCTSTR item2="FirstInstallDate";

    //打开注册表中的相应项

    long ret0=RegOpenKeyEx(HKEY_LOCAL_MACHINE,datepath,0,KEY_READ,&hKey);
    if(ret0==ERROR_SUCCESS) {
        //读取相应的信息

        long ret2=RegQueryValueEx(hKey,item,NULL,&type,(LPBYTE)&it,&size);
        if(ret2==ERROR_SUCCESS) {
            found=true;
            //cout<<it<<endl;
        }
        else {
            long ret1=RegOpenKeyEx(HKEY_LOCAL_MACHINE,datepath2,0,KEY_READ,&hKey);
            if(ret1==ERROR_SUCCESS) {
                //读取相应的信息

                long ret3=RegQueryValueEx(hKey,item2,NULL,&type,(LPBYTE)&it,&size);
                if(ret3==ERROR_SUCCESS) {
                    found=true;
                    //cout<<it<<endl;
                };
            }
        }
    }

    if(found) {
        time_t tm;
        tm=it;
        char tmp[128];
        strftime( tmp, sizeof(tmp), "系统安装于: %z %Y年%m月%d日 %X %A ", localtime((const time_t*)&tm) );
        cout<<tmp<<endl;
    }
    else {
        cout<<"无法读取系统安装时间"<<endl;
    }

    //关闭注册表中的相应的项

    RegCloseKey(hKey);


	RegCloseKey(hKey);

	DWORD t=GetTickCount();
	t/=1000;
	cout<<"系统已经启动了"<<t/(60*60)<<"小时"<<t%(60*60)/60<<"分"<<endl;

    return 0;
}
