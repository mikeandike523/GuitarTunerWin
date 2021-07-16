
#pragma warning( disable: 4049 )   
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 500
#endif
#ifndef __REQUIRED_RPCSAL_H_VERSION__
#define __REQUIRED_RPCSAL_H_VERSION__ 100
#endif
#include "rpc.h"
#include "rpcndr.h"
#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif  
#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif  
#ifndef __mmdeviceapi_h__
#define __mmdeviceapi_h__
#if __GNUC__ >=3
#pragma GCC system_header
#endif
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif
#ifndef __IMMNotificationClient_FWD_DEFINED__
#define __IMMNotificationClient_FWD_DEFINED__
typedef interface IMMNotificationClient IMMNotificationClient;
#endif 	 
#ifndef __IMMDevice_FWD_DEFINED__
#define __IMMDevice_FWD_DEFINED__
typedef interface IMMDevice IMMDevice;
#endif 	 
#ifndef __IMMDeviceCollection_FWD_DEFINED__
#define __IMMDeviceCollection_FWD_DEFINED__
typedef interface IMMDeviceCollection IMMDeviceCollection;
#endif 	 
#ifndef __IMMEndpoint_FWD_DEFINED__
#define __IMMEndpoint_FWD_DEFINED__
typedef interface IMMEndpoint IMMEndpoint;
#endif 	 
#ifndef __IMMDeviceEnumerator_FWD_DEFINED__
#define __IMMDeviceEnumerator_FWD_DEFINED__
typedef interface IMMDeviceEnumerator IMMDeviceEnumerator;
#endif 	 
#ifndef __IMMDeviceActivator_FWD_DEFINED__
#define __IMMDeviceActivator_FWD_DEFINED__
typedef interface IMMDeviceActivator IMMDeviceActivator;
#endif 	 
#ifndef __MMDeviceEnumerator_FWD_DEFINED__
#define __MMDeviceEnumerator_FWD_DEFINED__
#ifdef __cplusplus
typedef class MMDeviceEnumerator MMDeviceEnumerator;
#else
typedef struct MMDeviceEnumerator MMDeviceEnumerator;
#endif  
#endif 	 
#include "unknwn.h"
#include "propsys.h"
#ifdef __cplusplus
extern "C"{
#endif 
#define E_NOTFOUND HRESULT_FROM_WIN32(ERROR_NOT_FOUND)
#define E_UNSUPPORTED_TYPE HRESULT_FROM_WIN32(ERROR_UNSUPPORTED_TYPE)
#define DEVICE_STATE_ACTIVE      0x00000001
#define DEVICE_STATE_DISABLED    0x00000002
#define DEVICE_STATE_NOTPRESENT  0x00000004
#define DEVICE_STATE_UNPLUGGED   0x00000008
#define DEVICE_STATEMASK_ALL     0x0000000f
#ifdef DEFINE_PROPERTYKEY
#undef DEFINE_PROPERTYKEY
#endif
#ifdef INITGUID
#define DEFINE_PROPERTYKEY(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8, pid) EXTERN_C const PROPERTYKEY name = { { l, w1, w2, { b1, b2,  b3,  b4,  b5,  b6,  b7,  b8 } }, pid }
#else
#define DEFINE_PROPERTYKEY(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8, pid) EXTERN_C const PROPERTYKEY name
#endif  
DEFINE_PROPERTYKEY(PKEY_AudioEndpoint_FormFactor, 0x1da5d803, 0xd492, 0x4edd, 0x8c, 0x23, 0xe0, 0xc0, 0xff, 0xee, 0x7f, 0x0e, 0); 
DEFINE_PROPERTYKEY(PKEY_AudioEndpoint_ControlPanelPageProvider, 0x1da5d803, 0xd492, 0x4edd, 0x8c, 0x23, 0xe0, 0xc0, 0xff, 0xee, 0x7f, 0x0e, 1); 
DEFINE_PROPERTYKEY(PKEY_AudioEndpoint_Association, 0x1da5d803, 0xd492, 0x4edd, 0x8c, 0x23, 0xe0, 0xc0, 0xff, 0xee, 0x7f, 0x0e, 2);
DEFINE_PROPERTYKEY(PKEY_AudioEndpoint_PhysicalSpeakers, 0x1da5d803, 0xd492, 0x4edd, 0x8c, 0x23, 0xe0, 0xc0, 0xff, 0xee, 0x7f, 0x0e, 3);
DEFINE_PROPERTYKEY(PKEY_AudioEndpoint_GUID, 0x1da5d803, 0xd492, 0x4edd, 0x8c, 0x23, 0xe0, 0xc0, 0xff, 0xee, 0x7f, 0x0e, 4);
DEFINE_PROPERTYKEY(PKEY_AudioEndpoint_Disable_SysFx, 0x1da5d803, 0xd492, 0x4edd, 0x8c, 0x23, 0xe0, 0xc0, 0xff, 0xee, 0x7f, 0x0e, 5);
#define ENDPOINT_SYSFX_ENABLED          0x00000000   
#define ENDPOINT_SYSFX_DISABLED         0x00000001   
DEFINE_PROPERTYKEY(PKEY_AudioEndpoint_FullRangeSpeakers, 0x1da5d803, 0xd492, 0x4edd, 0x8c, 0x23, 0xe0, 0xc0, 0xff, 0xee, 0x7f, 0x0e, 6);
DEFINE_PROPERTYKEY(PKEY_AudioEngine_DeviceFormat, 0xf19f064d, 0x82c, 0x4e27, 0xbc, 0x73, 0x68, 0x82, 0xa1, 0xbb, 0x8e, 0x4c, 0); 
typedef struct tagDIRECTX_AUDIO_ACTIVATION_PARAMS
{
DWORD cbDirectXAudioActivationParams;
GUID guidAudioSession;
DWORD dwAudioStreamFlags;
} 	DIRECTX_AUDIO_ACTIVATION_PARAMS;
typedef struct tagDIRECTX_AUDIO_ACTIVATION_PARAMS *PDIRECTX_AUDIO_ACTIVATION_PARAMS;
typedef   
enum __MIDL___MIDL_itf_mmdeviceapi_0000_0000_0001
{	eRender	= 0,
	eCapture	= ( eRender + 1 ) ,
	eAll	= ( eCapture + 1 ) ,
	EDataFlow_enum_count	= ( eAll + 1 ) 
} 	EDataFlow;
typedef   
enum __MIDL___MIDL_itf_mmdeviceapi_0000_0000_0002
{	eConsole	= 0,
	eMultimedia	= ( eConsole + 1 ) ,
	eCommunications	= ( eMultimedia + 1 ) ,
	ERole_enum_count	= ( eCommunications + 1 ) 
} 	ERole;
typedef   
enum __MIDL___MIDL_itf_mmdeviceapi_0000_0000_0003
{	RemoteNetworkDevice	= 0,
	Speakers	= ( RemoteNetworkDevice + 1 ) ,
	LineLevel	= ( Speakers + 1 ) ,
	Headphones	= ( LineLevel + 1 ) ,
	Microphone	= ( Headphones + 1 ) ,
	Headset	= ( Microphone + 1 ) ,
	Handset	= ( Headset + 1 ) ,
	UnknownDigitalPassthrough	= ( Handset + 1 ) ,
	SPDIF	= ( UnknownDigitalPassthrough + 1 ) ,
	HDMI	= ( SPDIF + 1 ) ,
	UnknownFormFactor	= ( HDMI + 1 ) 
} 	EndpointFormFactor;
extern RPC_IF_HANDLE __MIDL_itf_mmdeviceapi_0000_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_mmdeviceapi_0000_0000_v0_0_s_ifspec;
#ifndef __IMMNotificationClient_INTERFACE_DEFINED__
#define __IMMNotificationClient_INTERFACE_DEFINED__
EXTERN_C const IID IID_IMMNotificationClient;
#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("7991EEC9-7E89-4D85-8390-6C703CEC60C0")
IMMNotificationClient : public IUnknown
{
public:
virtual   HRESULT STDMETHODCALLTYPE OnDeviceStateChanged( 
__in  LPCWSTR pwstrDeviceId,
__in  DWORD dwNewState) = 0;
virtual   HRESULT STDMETHODCALLTYPE OnDeviceAdded( 
__in  LPCWSTR pwstrDeviceId) = 0;
virtual   HRESULT STDMETHODCALLTYPE OnDeviceRemoved( 
__in  LPCWSTR pwstrDeviceId) = 0;
virtual   HRESULT STDMETHODCALLTYPE OnDefaultDeviceChanged( 
__in  EDataFlow flow,
__in  ERole role,
__in  LPCWSTR pwstrDefaultDeviceId) = 0;
virtual   HRESULT STDMETHODCALLTYPE OnPropertyValueChanged( 
__in  LPCWSTR pwstrDeviceId,
__in  const PROPERTYKEY key) = 0;
};
#else 	 
typedef struct IMMNotificationClientVtbl
{
BEGIN_INTERFACE
HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
IMMNotificationClient * This,
REFIID riid,
__RPC__deref_out  void **ppvObject);
ULONG ( STDMETHODCALLTYPE *AddRef )( 
IMMNotificationClient * This);
ULONG ( STDMETHODCALLTYPE *Release )( 
IMMNotificationClient * This);
HRESULT ( STDMETHODCALLTYPE *OnDeviceStateChanged )( 
IMMNotificationClient * This,
__in  LPCWSTR pwstrDeviceId,
__in  DWORD dwNewState);
HRESULT ( STDMETHODCALLTYPE *OnDeviceAdded )( 
IMMNotificationClient * This,
__in  LPCWSTR pwstrDeviceId);
HRESULT ( STDMETHODCALLTYPE *OnDeviceRemoved )( 
IMMNotificationClient * This,
__in  LPCWSTR pwstrDeviceId);
HRESULT ( STDMETHODCALLTYPE *OnDefaultDeviceChanged )( 
IMMNotificationClient * This,
__in  EDataFlow flow,
__in  ERole role,
__in  LPCWSTR pwstrDefaultDeviceId);
HRESULT ( STDMETHODCALLTYPE *OnPropertyValueChanged )( 
IMMNotificationClient * This,
__in  LPCWSTR pwstrDeviceId,
__in  const PROPERTYKEY key);
END_INTERFACE
} IMMNotificationClientVtbl;
interface IMMNotificationClient
{
CONST_VTBL struct IMMNotificationClientVtbl *lpVtbl;
};
#ifdef COBJMACROS
#define IMMNotificationClient_QueryInterface(This,riid,ppvObject)	\
( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define IMMNotificationClient_AddRef(This)	\
( (This)->lpVtbl -> AddRef(This) ) 
#define IMMNotificationClient_Release(This)	\
( (This)->lpVtbl -> Release(This) ) 
#define IMMNotificationClient_OnDeviceStateChanged(This,pwstrDeviceId,dwNewState)	\
( (This)->lpVtbl -> OnDeviceStateChanged(This,pwstrDeviceId,dwNewState) ) 
#define IMMNotificationClient_OnDeviceAdded(This,pwstrDeviceId)	\
( (This)->lpVtbl -> OnDeviceAdded(This,pwstrDeviceId) ) 
#define IMMNotificationClient_OnDeviceRemoved(This,pwstrDeviceId)	\
( (This)->lpVtbl -> OnDeviceRemoved(This,pwstrDeviceId) ) 
#define IMMNotificationClient_OnDefaultDeviceChanged(This,flow,role,pwstrDefaultDeviceId)	\
( (This)->lpVtbl -> OnDefaultDeviceChanged(This,flow,role,pwstrDefaultDeviceId) ) 
#define IMMNotificationClient_OnPropertyValueChanged(This,pwstrDeviceId,key)	\
( (This)->lpVtbl -> OnPropertyValueChanged(This,pwstrDeviceId,key) ) 
#endif  
#endif 	 
#endif 	 
#ifndef __IMMDevice_INTERFACE_DEFINED__
#define __IMMDevice_INTERFACE_DEFINED__
EXTERN_C const IID IID_IMMDevice;
#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("D666063F-1587-4E43-81F1-B948E807363F")
IMMDevice : public IUnknown
{
public:
virtual   HRESULT STDMETHODCALLTYPE Activate( 
__in  REFIID iid,
__in  DWORD dwClsCtx,
__in_opt  PROPVARIANT *pActivationParams,
__out  void **ppInterface) = 0;
virtual   HRESULT STDMETHODCALLTYPE OpenPropertyStore( 
__in  DWORD stgmAccess,
__out  IPropertyStore **ppProperties) = 0;
virtual   HRESULT STDMETHODCALLTYPE GetId( 
__deref_out  LPWSTR *ppstrId) = 0;
virtual   HRESULT STDMETHODCALLTYPE GetState( 
__out  DWORD *pdwState) = 0;
};
#else 	 
typedef struct IMMDeviceVtbl
{
BEGIN_INTERFACE
HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
IMMDevice * This,
REFIID riid,
__RPC__deref_out  void **ppvObject);
ULONG ( STDMETHODCALLTYPE *AddRef )( 
IMMDevice * This);
ULONG ( STDMETHODCALLTYPE *Release )( 
IMMDevice * This);
HRESULT ( STDMETHODCALLTYPE *Activate )( 
IMMDevice * This,
__in  REFIID iid,
__in  DWORD dwClsCtx,
__in_opt  PROPVARIANT *pActivationParams,
__out  void **ppInterface);
HRESULT ( STDMETHODCALLTYPE *OpenPropertyStore )( 
IMMDevice * This,
__in  DWORD stgmAccess,
__out  IPropertyStore **ppProperties);
HRESULT ( STDMETHODCALLTYPE *GetId )( 
IMMDevice * This,
__deref_out  LPWSTR *ppstrId);
HRESULT ( STDMETHODCALLTYPE *GetState )( 
IMMDevice * This,
__out  DWORD *pdwState);
END_INTERFACE
} IMMDeviceVtbl;
interface IMMDevice
{
CONST_VTBL struct IMMDeviceVtbl *lpVtbl;
};
#ifdef COBJMACROS
#define IMMDevice_QueryInterface(This,riid,ppvObject)	\
( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define IMMDevice_AddRef(This)	\
( (This)->lpVtbl -> AddRef(This) ) 
#define IMMDevice_Release(This)	\
( (This)->lpVtbl -> Release(This) ) 
#define IMMDevice_Activate(This,iid,dwClsCtx,pActivationParams,ppInterface)	\
( (This)->lpVtbl -> Activate(This,iid,dwClsCtx,pActivationParams,ppInterface) ) 
#define IMMDevice_OpenPropertyStore(This,stgmAccess,ppProperties)	\
( (This)->lpVtbl -> OpenPropertyStore(This,stgmAccess,ppProperties) ) 
#define IMMDevice_GetId(This,ppstrId)	\
( (This)->lpVtbl -> GetId(This,ppstrId) ) 
#define IMMDevice_GetState(This,pdwState)	\
( (This)->lpVtbl -> GetState(This,pdwState) ) 
#endif  
#endif 	 
#endif 	 
#ifndef __IMMDeviceCollection_INTERFACE_DEFINED__
#define __IMMDeviceCollection_INTERFACE_DEFINED__
EXTERN_C const IID IID_IMMDeviceCollection;
#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("0BD7A1BE-7A1A-44DB-8397-CC5392387B5E")
IMMDeviceCollection : public IUnknown
{
public:
virtual   HRESULT STDMETHODCALLTYPE GetCount( 
__out  UINT *pcDevices) = 0;
virtual   HRESULT STDMETHODCALLTYPE Item( 
__in  UINT nDevice,
__out  IMMDevice **ppDevice) = 0;
};
#else 	 
typedef struct IMMDeviceCollectionVtbl
{
BEGIN_INTERFACE
HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
IMMDeviceCollection * This,
REFIID riid,
__RPC__deref_out  void **ppvObject);
ULONG ( STDMETHODCALLTYPE *AddRef )( 
IMMDeviceCollection * This);
ULONG ( STDMETHODCALLTYPE *Release )( 
IMMDeviceCollection * This);
HRESULT ( STDMETHODCALLTYPE *GetCount )( 
IMMDeviceCollection * This,
__out  UINT *pcDevices);
HRESULT ( STDMETHODCALLTYPE *Item )( 
IMMDeviceCollection * This,
__in  UINT nDevice,
__out  IMMDevice **ppDevice);
END_INTERFACE
} IMMDeviceCollectionVtbl;
interface IMMDeviceCollection
{
CONST_VTBL struct IMMDeviceCollectionVtbl *lpVtbl;
};
#ifdef COBJMACROS
#define IMMDeviceCollection_QueryInterface(This,riid,ppvObject)	\
( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define IMMDeviceCollection_AddRef(This)	\
( (This)->lpVtbl -> AddRef(This) ) 
#define IMMDeviceCollection_Release(This)	\
( (This)->lpVtbl -> Release(This) ) 
#define IMMDeviceCollection_GetCount(This,pcDevices)	\
( (This)->lpVtbl -> GetCount(This,pcDevices) ) 
#define IMMDeviceCollection_Item(This,nDevice,ppDevice)	\
( (This)->lpVtbl -> Item(This,nDevice,ppDevice) ) 
#endif  
#endif 	 
#endif 	 
#ifndef __IMMEndpoint_INTERFACE_DEFINED__
#define __IMMEndpoint_INTERFACE_DEFINED__
EXTERN_C const IID IID_IMMEndpoint;
#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("1BE09788-6894-4089-8586-9A2A6C265AC5")
IMMEndpoint : public IUnknown
{
public:
virtual   HRESULT STDMETHODCALLTYPE GetDataFlow( 
__out  EDataFlow *pDataFlow) = 0;
};
#else 	 
typedef struct IMMEndpointVtbl
{
BEGIN_INTERFACE
HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
IMMEndpoint * This,
REFIID riid,
__RPC__deref_out  void **ppvObject);
ULONG ( STDMETHODCALLTYPE *AddRef )( 
IMMEndpoint * This);
ULONG ( STDMETHODCALLTYPE *Release )( 
IMMEndpoint * This);
HRESULT ( STDMETHODCALLTYPE *GetDataFlow )( 
IMMEndpoint * This,
__out  EDataFlow *pDataFlow);
END_INTERFACE
} IMMEndpointVtbl;
interface IMMEndpoint
{
CONST_VTBL struct IMMEndpointVtbl *lpVtbl;
};
#ifdef COBJMACROS
#define IMMEndpoint_QueryInterface(This,riid,ppvObject)	\
( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define IMMEndpoint_AddRef(This)	\
( (This)->lpVtbl -> AddRef(This) ) 
#define IMMEndpoint_Release(This)	\
( (This)->lpVtbl -> Release(This) ) 
#define IMMEndpoint_GetDataFlow(This,pDataFlow)	\
( (This)->lpVtbl -> GetDataFlow(This,pDataFlow) ) 
#endif  
#endif 	 
#endif 	 
#ifndef __IMMDeviceEnumerator_INTERFACE_DEFINED__
#define __IMMDeviceEnumerator_INTERFACE_DEFINED__
EXTERN_C const IID IID_IMMDeviceEnumerator;
#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("A95664D2-9614-4F35-A746-DE8DB63617E6")
IMMDeviceEnumerator : public IUnknown
{
public:
virtual   HRESULT STDMETHODCALLTYPE EnumAudioEndpoints( 
__in  EDataFlow dataFlow,
__in  DWORD dwStateMask,
__out  IMMDeviceCollection **ppDevices) = 0;
virtual   HRESULT STDMETHODCALLTYPE GetDefaultAudioEndpoint( 
__in  EDataFlow dataFlow,
__in  ERole role,
__out  IMMDevice **ppEndpoint) = 0;
virtual   HRESULT STDMETHODCALLTYPE GetDevice( 
__in  LPCWSTR pwstrId,
__out  IMMDevice **ppDevice) = 0;
virtual   HRESULT STDMETHODCALLTYPE RegisterEndpointNotificationCallback( 
__in  IMMNotificationClient *pClient) = 0;
virtual   HRESULT STDMETHODCALLTYPE UnregisterEndpointNotificationCallback( 
__in  IMMNotificationClient *pClient) = 0;
};
#else 	 
typedef struct IMMDeviceEnumeratorVtbl
{
BEGIN_INTERFACE
HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
IMMDeviceEnumerator * This,
REFIID riid,
__RPC__deref_out  void **ppvObject);
ULONG ( STDMETHODCALLTYPE *AddRef )( 
IMMDeviceEnumerator * This);
ULONG ( STDMETHODCALLTYPE *Release )( 
IMMDeviceEnumerator * This);
HRESULT ( STDMETHODCALLTYPE *EnumAudioEndpoints )( 
IMMDeviceEnumerator * This,
__in  EDataFlow dataFlow,
__in  DWORD dwStateMask,
__out  IMMDeviceCollection **ppDevices);
HRESULT ( STDMETHODCALLTYPE *GetDefaultAudioEndpoint )( 
IMMDeviceEnumerator * This,
__in  EDataFlow dataFlow,
__in  ERole role,
__out  IMMDevice **ppEndpoint);
HRESULT ( STDMETHODCALLTYPE *GetDevice )( 
IMMDeviceEnumerator * This,
__in  LPCWSTR pwstrId,
__out  IMMDevice **ppDevice);
HRESULT ( STDMETHODCALLTYPE *RegisterEndpointNotificationCallback )( 
IMMDeviceEnumerator * This,
__in  IMMNotificationClient *pClient);
HRESULT ( STDMETHODCALLTYPE *UnregisterEndpointNotificationCallback )( 
IMMDeviceEnumerator * This,
__in  IMMNotificationClient *pClient);
END_INTERFACE
} IMMDeviceEnumeratorVtbl;
interface IMMDeviceEnumerator
{
CONST_VTBL struct IMMDeviceEnumeratorVtbl *lpVtbl;
};
#ifdef COBJMACROS
#define IMMDeviceEnumerator_QueryInterface(This,riid,ppvObject)	\
( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define IMMDeviceEnumerator_AddRef(This)	\
( (This)->lpVtbl -> AddRef(This) ) 
#define IMMDeviceEnumerator_Release(This)	\
( (This)->lpVtbl -> Release(This) ) 
#define IMMDeviceEnumerator_EnumAudioEndpoints(This,dataFlow,dwStateMask,ppDevices)	\
( (This)->lpVtbl -> EnumAudioEndpoints(This,dataFlow,dwStateMask,ppDevices) ) 
#define IMMDeviceEnumerator_GetDefaultAudioEndpoint(This,dataFlow,role,ppEndpoint)	\
( (This)->lpVtbl -> GetDefaultAudioEndpoint(This,dataFlow,role,ppEndpoint) ) 
#define IMMDeviceEnumerator_GetDevice(This,pwstrId,ppDevice)	\
( (This)->lpVtbl -> GetDevice(This,pwstrId,ppDevice) ) 
#define IMMDeviceEnumerator_RegisterEndpointNotificationCallback(This,pClient)	\
( (This)->lpVtbl -> RegisterEndpointNotificationCallback(This,pClient) ) 
#define IMMDeviceEnumerator_UnregisterEndpointNotificationCallback(This,pClient)	\
( (This)->lpVtbl -> UnregisterEndpointNotificationCallback(This,pClient) ) 
#endif  
#endif 	 
#endif 	 
#ifndef __IMMDeviceActivator_INTERFACE_DEFINED__
#define __IMMDeviceActivator_INTERFACE_DEFINED__
EXTERN_C const IID IID_IMMDeviceActivator;
#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("3B0D0EA4-D0A9-4B0E-935B-09516746FAC0")
IMMDeviceActivator : public IUnknown
{
public:
virtual   HRESULT STDMETHODCALLTYPE Activate( 
__in  REFIID iid,
__in  IMMDevice *pDevice,
__in_opt  PROPVARIANT *pActivationParams,
__out  void **ppInterface) = 0;
};
#else 	 
typedef struct IMMDeviceActivatorVtbl
{
BEGIN_INTERFACE
HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
IMMDeviceActivator * This,
REFIID riid,
__RPC__deref_out  void **ppvObject);
ULONG ( STDMETHODCALLTYPE *AddRef )( 
IMMDeviceActivator * This);
ULONG ( STDMETHODCALLTYPE *Release )( 
IMMDeviceActivator * This);
HRESULT ( STDMETHODCALLTYPE *Activate )( 
IMMDeviceActivator * This,
__in  REFIID iid,
__in  IMMDevice *pDevice,
__in_opt  PROPVARIANT *pActivationParams,
__out  void **ppInterface);
END_INTERFACE
} IMMDeviceActivatorVtbl;
interface IMMDeviceActivator
{
CONST_VTBL struct IMMDeviceActivatorVtbl *lpVtbl;
};
#ifdef COBJMACROS
#define IMMDeviceActivator_QueryInterface(This,riid,ppvObject)	\
( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define IMMDeviceActivator_AddRef(This)	\
( (This)->lpVtbl -> AddRef(This) ) 
#define IMMDeviceActivator_Release(This)	\
( (This)->lpVtbl -> Release(This) ) 
#define IMMDeviceActivator_Activate(This,iid,pDevice,pActivationParams,ppInterface)	\
( (This)->lpVtbl -> Activate(This,iid,pDevice,pActivationParams,ppInterface) ) 
#endif  
#endif 	 
#endif 	 
typedef   struct __MIDL___MIDL_itf_mmdeviceapi_0000_0006_0001
{
LPARAM AddPageParam;
IMMDevice *pEndpoint;
IMMDevice *pPnpInterface;
IMMDevice *pPnpDevnode;
} 	AudioExtensionParams;
extern RPC_IF_HANDLE __MIDL_itf_mmdeviceapi_0000_0006_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_mmdeviceapi_0000_0006_v0_0_s_ifspec;
#ifndef __MMDeviceAPILib_LIBRARY_DEFINED__
#define __MMDeviceAPILib_LIBRARY_DEFINED__
EXTERN_C const IID LIBID_MMDeviceAPILib;
EXTERN_C const CLSID CLSID_MMDeviceEnumerator;
#ifdef __cplusplus
class DECLSPEC_UUID("BCDE0395-E52F-467C-8E3D-C4579291692E")
MMDeviceEnumerator;
#endif
#endif  
#ifdef __cplusplus
}
#endif
#endif
