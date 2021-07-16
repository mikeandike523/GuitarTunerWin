 
#ifndef __COMBASE__
#define __COMBASE__
extern HINSTANCE g_hInst;
extern DWORD g_amPlatform;
extern OSVERSIONINFO g_osInfo;      
DECLARE_INTERFACE(INonDelegatingUnknown)
{
STDMETHOD(NonDelegatingQueryInterface) (THIS_ REFIID, LPVOID *) PURE;
STDMETHOD_(ULONG, NonDelegatingAddRef)(THIS) PURE;
STDMETHOD_(ULONG, NonDelegatingRelease)(THIS) PURE;
};
typedef INonDelegatingUnknown *PNDUNKNOWN;
class CBaseObject
{
private:
CBaseObject(const CBaseObject& objectSrc);           
void operator=(const CBaseObject& objectSrc);        
private:
static LONG m_cObjects;      
protected:
#ifdef DEBUG
DWORD m_dwCookie;            
#endif
public:
CBaseObject(TCHAR *pName);
~CBaseObject();
static LONG ObjectsActive() {
return m_cObjects;
};
};
class CUnknown : public INonDelegatingUnknown,
public CBaseObject
{
private:
const LPUNKNOWN m_pUnknown;  
protected:                       
volatile LONG m_cRef;        
public:
CUnknown(TCHAR *pName, LPUNKNOWN pUnk, HRESULT *phr);
virtual ~CUnknown() {};
LPUNKNOWN GetOwner() const {
return m_pUnknown;
};
STDMETHODIMP NonDelegatingQueryInterface(REFIID, void **);
STDMETHODIMP_(ULONG) NonDelegatingAddRef();
STDMETHODIMP_(ULONG) NonDelegatingRelease();
HRESULT GetInterface(LPUNKNOWN pUnk, void **ppv);
};
#if WINVER < 0x0501
static inline LONG InterlockedIncrement( volatile LONG * plong )
{ return InterlockedIncrement( const_cast<LONG*>( plong ) ); }
static inline LONG InterlockedDecrement( volatile LONG * plong )
{ return InterlockedDecrement( const_cast<LONG*>( plong ) ); }
static inline LONG InterlockedExchange( volatile LONG * plong, LONG new_value )
{ return InterlockedExchange( const_cast<LONG*>( plong ), new_value ); }
#endif
typedef CUnknown *(*LPFNNewCOMObject)(LPUNKNOWN pUnkOuter, HRESULT *phr);
typedef void (*LPFNInitRoutine)(BOOL bLoading, const CLSID *rclsid);
class CFactoryTemplate {
public:
const WCHAR *m_Name;
const CLSID *m_ClsID;
LPFNNewCOMObject m_lpfnNew;
LPFNInitRoutine  m_lpfnInit;
BOOL IsClassID(REFCLSID rclsid) const {
return (IsEqualCLSID(*m_ClsID,rclsid));
};
CUnknown *CreateInstance(LPUNKNOWN pUnk, HRESULT *phr) const {
return m_lpfnNew(pUnk, phr);
};
};
#define DECLARE_IUNKNOWN                                        \
STDMETHODIMP QueryInterface(REFIID riid, void **ppv) {      \
return GetOwner()->QueryInterface(riid,ppv);            \
};                                                          \
STDMETHODIMP_(ULONG) AddRef() {                             \
return GetOwner()->AddRef();                            \
};                                                          \
STDMETHODIMP_(ULONG) Release() {                            \
return GetOwner()->Release();                           \
};
#endif  
