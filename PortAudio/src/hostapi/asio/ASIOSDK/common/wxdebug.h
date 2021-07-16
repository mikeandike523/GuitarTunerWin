 
#ifndef __WXDEBUG__
#define __WXDEBUG__
#undef ASSERT
enum {  LOG_TIMING = 0x01,     
LOG_TRACE = 0x02,      
LOG_MEMORY =  0x04,    
LOG_LOCKING = 0x08,    
LOG_ERROR = 0x10 };    
enum {  CDISP_HEX = 0x01,
CDISP_DEC = 0x02};
typedef struct tag_ObjectDesc {
TCHAR *m_pName;
DWORD m_dwCookie;
tag_ObjectDesc *m_pNext;
} ObjectDesc;
#define DLLIMPORT __declspec(dllimport)
#define DLLEXPORT __declspec(dllexport)
#ifdef DEBUG
#define NAME(x) TEXT(x)
void DbgInitKeyLevels(HKEY hKey);
void DbgInitGlobalSettings();
void DbgInitModuleSettings();
void DbgInitModuleName();
DWORD DbgRegisterObjectCreation(TCHAR *pObjectName);
BOOL DbgRegisterObjectDestruction(DWORD dwCookie);
BOOL DbgCheckModuleLevel(DWORD Type,DWORD Level);
void DbgSetModuleLevel(DWORD Type,DWORD Level);
void DbgInitialise(HINSTANCE hInst);
void DbgTerminate();
void DbgDumpObjectRegister();
void DbgAssert(const TCHAR *pCondition,const TCHAR *pFileName,INT iLine);
void DbgBreakPoint(const TCHAR *pCondition,const TCHAR *pFileName,INT iLine);
void DbgKernelAssert(const TCHAR *pCondition,const TCHAR *pFileName,INT iLine);
void DbgLogInfo(DWORD Type,DWORD Level,const TCHAR *pFormat,...);
void DbgOutString(LPCTSTR psz);
DWORD DbgWaitForSingleObject(HANDLE h);
DWORD DbgWaitForMultipleObjects(DWORD nCount,
CONST HANDLE *lpHandles,
BOOL bWaitAll);
void DbgSetWaitTimeout(DWORD dwTimeout);
#ifdef __strmif_h__
void DisplayType(LPSTR label, const AM_MEDIA_TYPE *pmtIn);
#endif
#define KASSERT(_x_) if (!(_x_))         \
DbgKernelAssert(TEXT(#_x_),TEXT(__FILE__),__LINE__)
#define KDbgBreak(_x_)                   \
DbgKernelAssert(TEXT(#_x_),TEXT(__FILE__),__LINE__)
#define ASSERT(_x_) if (!(_x_))         \
DbgAssert(TEXT(#_x_),TEXT(__FILE__),__LINE__)
#define DbgBreak(_x_)                   \
DbgBreakPoint(TEXT(#_x_),TEXT(__FILE__),__LINE__)
#define EXECUTE_ASSERT(_x_) ASSERT(_x_)
#define DbgLog(_x_) DbgLogInfo _x_
#define NOTE(_x_)             DbgLog((LOG_TRACE,5,TEXT(_x_)));
#define NOTE1(_x_,a)          DbgLog((LOG_TRACE,5,TEXT(_x_),a));
#define NOTE2(_x_,a,b)        DbgLog((LOG_TRACE,5,TEXT(_x_),a,b));
#define NOTE3(_x_,a,b,c)      DbgLog((LOG_TRACE,5,TEXT(_x_),a,b,c));
#define NOTE4(_x_,a,b,c,d)    DbgLog((LOG_TRACE,5,TEXT(_x_),a,b,c,d));
#define NOTE5(_x_,a,b,c,d,e)  DbgLog((LOG_TRACE,5,TEXT(_x_),a,b,c,d,e));
#else
#define NAME(_x_) NULL
#define DbgInitialise(hInst)
#define DbgTerminate()
#define DbgLog(_x_)
#define DbgOutString(psz)
#define DbgRegisterObjectCreation(pObjectName)
#define DbgRegisterObjectDestruction(dwCookie)
#define DbgDumpObjectRegister()
#define DbgCheckModuleLevel(Type,Level)
#define DbgSetModuleLevel(Type,Level)
#define DbgWaitForSingleObject(h)  WaitForSingleObject(h, INFINITE)
#define DbgWaitForMultipleObjects(nCount, lpHandles, bWaitAll)     \
WaitForMultipleObjects(nCount, lpHandles, bWaitAll, INFINITE)
#define DbgSetWaitTimeout(dwTimeout)
#define KDbgBreak(_x_)
#define DbgBreak(_x_)
#define KASSERT(_x_)
#define ASSERT(_x_)
#define EXECUTE_ASSERT(_x_) _x_
#define NOTE(_x_)
#define NOTE1(_x_,a)
#define NOTE2(_x_,a,b)
#define NOTE3(_x_,a,b,c)
#define NOTE4(_x_,a,b,c,d)
#define NOTE5(_x_,a,b,c,d,e)
#define DisplayType(label, pmtIn)
#endif
#define CheckPointer(p,ret) {if((p)==NULL) return (ret);}
#ifdef DEBUG
#define VFWROBUST
#endif
#ifdef VFWROBUST
#define ValidateReadPtr(p,cb) \
{if(IsBadReadPtr((PVOID)p,cb) == TRUE) \
DbgBreak("Invalid read pointer");}
#define ValidateWritePtr(p,cb) \
{if(IsBadWritePtr((PVOID)p,cb) == TRUE) \
DbgBreak("Invalid write pointer");}
#define ValidateReadWritePtr(p,cb) \
{ValidateReadPtr(p,cb) ValidateWritePtr(p,cb)}
#define ValidateStringPtr(p) \
{if(IsBadStringPtr((LPCTSTR)p,INFINITE) == TRUE) \
DbgBreak("Invalid string pointer");}
#define ValidateStringPtrA(p) \
{if(IsBadStringPtrA((LPCSTR)p,INFINITE) == TRUE) \
DbgBreak("Invalid ANSII string pointer");}
#define ValidateStringPtrW(p) \
{if(IsBadStringPtrW((LPCWSTR)p,INFINITE) == TRUE) \
DbgBreak("Invalid UNICODE string pointer");}
#else
#define ValidateReadPtr(p,cb)
#define ValidateWritePtr(p,cb)
#define ValidateReadWritePtr(p,cb)
#define ValidateStringPtr(p)
#define ValidateStringPtrA(p)
#define ValidateStringPtrW(p)
#endif
#ifdef _OBJBASE_H_
typedef struct {
TCHAR   *szName;
GUID    guid;
} GUID_STRING_ENTRY;
class CGuidNameList {
public:
TCHAR *operator [] (const GUID& guid);
};
extern CGuidNameList GuidNames;
#endif
#define QUOTE(x) #x
#define QQUOTE(y) QUOTE(y)
#define REMIND(str) __FILE__ "(" QQUOTE(__LINE__) ") :  " str
class CDispBasic
{
public:
CDispBasic() { m_pString = m_String; };
~CDispBasic();
protected:
PTCHAR m_pString;   
TCHAR m_String[50];
};
class CDisp : public CDispBasic
{
public:
CDisp(LONGLONG ll, int Format = CDISP_HEX);  
CDisp(REFCLSID clsid);       
CDisp(double d);             
#ifdef __strmif_h__
#ifdef __STREAMS__
CDisp(CRefTime t);           
#endif
CDisp(IPin *pPin);           
#endif  
~CDisp();
operator LPCTSTR()
{
return (LPCTSTR)m_pString;
};
};
#endif  
