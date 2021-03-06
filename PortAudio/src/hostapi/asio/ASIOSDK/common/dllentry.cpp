 
#include <windows.h>
#include "wxdebug.h"
#include "combase.h"
#ifdef DEBUG
#include <tchar.h>
#endif
#include <stdio.h>
extern CFactoryTemplate g_Templates[];
extern int g_cTemplates;
HINSTANCE hinstance = 0;
DWORD	  g_amPlatform;		 
OSVERSIONINFO g_osInfo;
class CClassFactory : public IClassFactory
{
private:
const CFactoryTemplate * m_pTemplate;
ULONG m_cRef;
static int m_cLocked;
public:
CClassFactory(const CFactoryTemplate *);
STDMETHODIMP QueryInterface(REFIID riid, void ** ppv);
STDMETHODIMP_(ULONG)AddRef();
STDMETHODIMP_(ULONG)Release();
STDMETHODIMP CreateInstance(LPUNKNOWN pUnkOuter, REFIID riid, void **pv);
STDMETHODIMP LockServer(BOOL fLock);
static BOOL IsLocked() {
return (m_cLocked > 0);
};
};
int CClassFactory::m_cLocked = 0;
CClassFactory::CClassFactory(const CFactoryTemplate *pTemplate)
{
m_cRef = 0;
m_pTemplate = pTemplate;
}
STDMETHODIMP CClassFactory::QueryInterface(REFIID riid,void **ppv)
{
CheckPointer(ppv,E_POINTER)
ValidateReadWritePtr(ppv,sizeof(PVOID));
*ppv = NULL;
if ((riid == IID_IUnknown) || (riid == IID_IClassFactory)) {
*ppv = (LPVOID) this;
	 
((LPUNKNOWN) *ppv)->AddRef();
return NOERROR;
}
return ResultFromScode(E_NOINTERFACE);
}
STDMETHODIMP_(ULONG) CClassFactory::AddRef()
{
return ++m_cRef;
}
STDMETHODIMP_(ULONG) CClassFactory::Release()
{
	LONG	rc;
if (--m_cRef == 0) {
		delete this;
		rc = 0;
} else rc = m_cRef;
	return rc;
}
STDMETHODIMP CClassFactory::CreateInstance(LPUNKNOWN pUnkOuter,REFIID riid,void **pv)
{
	CheckPointer(pv,E_POINTER)
ValidateReadWritePtr(pv,sizeof(void *));
if (pUnkOuter != NULL) {
if (IsEqualIID(riid,IID_IUnknown) == FALSE) {
return ResultFromScode(E_NOINTERFACE);
}
}
HRESULT hr = NOERROR;
CUnknown *pObj = m_pTemplate->CreateInstance(pUnkOuter, &hr);
if (pObj == NULL) {
return E_OUTOFMEMORY;
}
	if (FAILED(hr)) {
		delete pObj;
return hr;
}
pObj->NonDelegatingAddRef();
hr = pObj->NonDelegatingQueryInterface(riid, pv);
pObj->NonDelegatingRelease();
if (SUCCEEDED(hr)) {
ASSERT(*pv);
}
return hr;
}
STDMETHODIMP CClassFactory::LockServer(BOOL fLock)
{
if (fLock) {
m_cLocked++;
} else {
m_cLocked--;
}
return NOERROR;
}
STDAPI DllGetClassObject(REFCLSID rClsID,REFIID riid,void **pv)
{
	 
if (!(riid == IID_IUnknown) && !(riid == IID_IClassFactory)) {
return E_NOINTERFACE;
}
for (int i = 0; i < g_cTemplates; i++) {
const CFactoryTemplate * pT = &g_Templates[i];
if (pT->IsClassID(rClsID)) {
*pv = (LPVOID) (LPUNKNOWN) new CClassFactory(pT);
if (*pv == NULL) {
return E_OUTOFMEMORY;
}
((LPUNKNOWN)*pv)->AddRef();
return NOERROR;
}
}
return CLASS_E_CLASSNOTAVAILABLE;
}
void DllInitClasses(BOOL bLoading)
{
int i;
	 
	 
for (i = 0; i < g_cTemplates; i++) {
		const CFactoryTemplate * pT = &g_Templates[i];
if (pT->m_lpfnInit != NULL) {
			(*pT->m_lpfnInit)(bLoading, pT->m_ClsID);
}
}
}
STDAPI DllCanUnloadNow()
{
	 
	DbgLog((LOG_MEMORY,2,TEXT("DLLCanUnloadNow called - IsLocked = %d, Active objects = %d"),
CClassFactory::IsLocked(),
CBaseObject::ObjectsActive()));
if (CClassFactory::IsLocked() || CBaseObject::ObjectsActive()) {
	
		return S_FALSE;
} 
	else {
		return S_OK;
}
}
BOOL WINAPI DllEntryPoint (HINSTANCE hInstance,ULONG ulReason,LPVOID pv)
{
	 
	switch (ulReason) {
	
		case DLL_PROCESS_ATTACH:
			DisableThreadLibraryCalls(hInstance);
			DbgInitialise(hInstance);
			{
				 
				 
				 
g_amPlatform = VER_PLATFORM_WIN32_WINDOWS;  
g_osInfo.dwOSVersionInfoSize = sizeof(g_osInfo);
if (GetVersionEx(&g_osInfo)) {
			g_amPlatform = g_osInfo.dwPlatformId;
				} 
				else {
					DbgLog((LOG_ERROR, 1, "Failed to get the OS platform, assuming Win95"));
				}
			}
			hinstance = hInstance;
			DllInitClasses(TRUE);
			
			break;
		case DLL_PROCESS_DETACH:
			DllInitClasses(FALSE);
#ifdef DEBUG
			if (CBaseObject::ObjectsActive()) {
				DbgSetModuleLevel(LOG_MEMORY, 2);
TCHAR szInfo[512];
extern TCHAR m_ModuleName[];      
TCHAR FullName[_MAX_PATH];       
TCHAR *pName;                    
GetModuleFileName(NULL,FullName,_MAX_PATH);
pName = _tcsrchr(FullName,'\\');
if (pName == NULL) {
pName = FullName;
} 
				else {
					pName++;
}
				DWORD cch = wsprintf(szInfo, TEXT("Executable: %s  Pid %x  Tid %x. "),
					pName, GetCurrentProcessId(), GetCurrentThreadId());
wsprintf(szInfo+cch, TEXT("Module %s, %d objects left active!"),
m_ModuleName, CBaseObject::ObjectsActive());
DbgAssert(szInfo, TEXT(__FILE__),__LINE__);
				 
				 
DbgDumpObjectRegister();
			}
			DbgTerminate();
#endif
			break;
}
return TRUE;
}
