 
#include <windows.h>
#include "wxdebug.h"
#include "combase.h"
#pragma warning( disable : 4514 )    
LONG CBaseObject::m_cObjects = 0;
CBaseObject::CBaseObject(TCHAR *pName)
{
InterlockedIncrement(&m_cObjects);
#ifdef DEBUG
m_dwCookie = DbgRegisterObjectCreation(pName);
#endif
}
CBaseObject::~CBaseObject()
{
InterlockedDecrement(&m_cObjects);
#ifdef DEBUG
DbgRegisterObjectDestruction(m_dwCookie);
#endif
}
#pragma warning( disable : 4355 4100 ) 
CUnknown::CUnknown(TCHAR *pName, LPUNKNOWN pUnk, HRESULT *phr) 
: CBaseObject(pName)
, m_cRef(0)
, m_pUnknown( pUnk != 0 ? pUnk : reinterpret_cast<LPUNKNOWN>( static_cast<PNDUNKNOWN>(this) ) )
{
}
#pragma warning( default : 4355 4100 ) 
STDMETHODIMP CUnknown::NonDelegatingQueryInterface(REFIID riid, void ** ppv)
{
CheckPointer(ppv,E_POINTER);
ValidateReadWritePtr(ppv,sizeof(PVOID));
if (riid == IID_IUnknown) {
GetInterface((LPUNKNOWN) (PNDUNKNOWN) this, ppv);
return NOERROR;
} else {
*ppv = NULL;
return E_NOINTERFACE;
}
}
#ifdef max
#undef max
#endif
template<class T> inline static T max( const T & a, const T & b )
{
return a > b ? a : b;
}
STDMETHODIMP_(ULONG) CUnknown::NonDelegatingAddRef()
{
LONG lRef = InterlockedIncrement( &m_cRef );
ASSERT(lRef > 0);
DbgLog((LOG_MEMORY,3,TEXT("    Obj %d ref++ = %d"),
m_dwCookie, m_cRef));
return max(ULONG(m_cRef), 1ul);
}
STDMETHODIMP_(ULONG) CUnknown::NonDelegatingRelease()
{
LONG lRef = InterlockedDecrement( &m_cRef );
ASSERT(lRef >= 0);
DbgLog((LOG_MEMORY,3,TEXT("    Object %d ref-- = %d"),
	    m_dwCookie, m_cRef));
if (lRef == 0) {
m_cRef++;
delete this;
return ULONG(0);
} else {
return max(ULONG(m_cRef), 1ul);
}
}
HRESULT CUnknown::GetInterface(LPUNKNOWN pUnk, void **ppv)
{
CheckPointer(ppv, E_POINTER);
*ppv = pUnk;
pUnk->AddRef();
return NOERROR;
}
BOOL IsEqualObject(IUnknown *pFirst, IUnknown *pSecond)
{
if (pFirst == pSecond) {
return TRUE;
}
LPUNKNOWN pUnknown1;      
LPUNKNOWN pUnknown2;      
HRESULT hr;               
ASSERT(pFirst);
ASSERT(pSecond);
hr = pFirst->QueryInterface(IID_IUnknown,(void **) &pUnknown1);
ASSERT(SUCCEEDED(hr));
ASSERT(pUnknown1);
hr = pSecond->QueryInterface(IID_IUnknown,(void **) &pUnknown2);
ASSERT(SUCCEEDED(hr));
ASSERT(pUnknown2);
pUnknown1->Release();
pUnknown2->Release();
return (pUnknown1 == pUnknown2);
}
