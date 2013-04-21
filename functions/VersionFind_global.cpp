#include "VersionFind_global.h"

//Version number
HWND VF_shared;
bool VF_fdFileIsDll = false;
LPPROCESS_INFORMATION VF_fdProcessInfo = NULL;
char VF_szFileName[256]="";
char VF_version[20]="";

unsigned int VF_version_decrypt_call_dest=0;
unsigned int VF_version_decrypt_call=0;
unsigned int VF_version_decrypt_neweip=0;
unsigned int VF_version_decrypt_buffer=0;

//Extra Options
unsigned int VF_extra_options_reg=0;
unsigned int VF_extra_options=0;

//Raw Options
bool VF_minimal=false;
long VF_fdImageBase=0;
long VF_fdEntryPoint=0;
long VF_fdEntrySectionNumber=0;
long VF_fdEntrySectionOffset=0;
long VF_fdEntrySectionSize=0;
unsigned int VF_raw_options=0;
unsigned int VF_raw_options_reg=0;

unsigned int VF_FindUsbPattern(BYTE* d, unsigned int size)
{
    for(unsigned int i=0; i<size; i++) //55534220646576696365
        if(d[i]==0x55 and d[i+1]==0x53 and d[i+2]==0x42 and d[i+3]==0x20 and d[i+4]==0x64 and d[i+5]==0x65 and d[i+6]==0x76 and d[i+7]==0x69 and d[i+8]==0x63 and d[i+9]==0x65)
        {
            while(d[i]!=0)
                i--;
            return i+1;
        }
    return 0;
}

unsigned int VF_FindAnd20Pattern(BYTE* d, unsigned int size)
{
    for(unsigned int i=0; i<size; i++) //83E?20
        if(d[i]==0x83 and(d[i+1]>>4)==0x0E and d[i+2]==0x20)
            return i;
    return 0;
}

unsigned int VF_FindAnd40000Pattern(BYTE* d, unsigned int size)
{
    for(unsigned int i=0; i<size; i++) //81E?00000400
        if(d[i]==0x81 and(d[i+1]>>4)==0x0E and d[i+2]==0x00 and d[i+3]==0x00 and d[i+4]==0x04 and d[i+5]==0x00)
            return i;
    return 0;
}

bool VF_IsMinimalProtection(ULONG_PTR va)
{
    OutputDebugStringA("IsMinimalProtection");
    int offset=GetPE32Data(VF_szFileName, VF_fdEntrySectionNumber, UE_SECTIONRAWOFFSET);
    BYTE firstbytes[2]= {0};
    memcpy(firstbytes, (void*)(va+offset), 2);
    if(firstbytes[0]==0x60 and firstbytes[1]==0xE8)
        return false;
    return true;
}

void VF_FatalError(const char* msg)
{
    MessageBoxA(VF_shared, msg, "Fatal Error!", MB_ICONERROR);
    StopDebug();
}

unsigned int VF_FindarmVersion(BYTE* d, unsigned int size)
{
    for(unsigned int i=0; i<size; i++) //3C61726D56657273696F6E (<armVersion)
        if(d[i]==0x3C and d[i+1]==0x61 and d[i+2]==0x72 and d[i+3]==0x6D and d[i+4]==0x56 and d[i+5]==0x65 and d[i+6]==0x72 and d[i+7]==0x73 and d[i+8]==0x69 and d[i+9]==0x6F and d[i+10]==0x6E)
        {
            while(d[i]!=0)
                i--;
            return i+1;
        }
    return 0;
}

unsigned int VF_FindPushAddr(BYTE* d, unsigned int size, unsigned int addr)
{
    BYTE b[4]= {0};
    memcpy(b, &addr, 4);
    for(unsigned int i=0; i<size; i++) //68XXXXXXXX
        if(d[i]==0x68 and d[i+1]==b[0] and d[i+2]==b[1] and d[i+3]==b[2] and d[i+4]==b[3])
            return i;
    return 0;
}