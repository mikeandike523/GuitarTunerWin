
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
#ifndef __endpointvolume_h__
#define __endpointvolume_h__
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif
#ifndef __IAudioEndpointVolumeCallback_FWD_DEFINED__
#define __IAudioEndpointVolumeCallback_FWD_DEFINED__
typedef interface IAudioEndpointVolumeCallback IAudioEndpointVolumeCallback;
#endif 	 
#ifndef __IAudioEndpointVolume_FWD_DEFINED__
#define __IAudioEndpointVolume_FWD_DEFINED__
typedef interface IAudioEndpointVolume IAudioEndpointVolume;
#endif 	 
#ifndef __IAudioMeterInformation_FWD_DEFINED__
#define __IAudioMeterInformation_FWD_DEFINED__
typedef interface IAudioMeterInformation IAudioMeterInformation;
#endif 	 
#include "unknwn.h"
#include "devicetopology.h"
#ifdef __cplusplus
extern "C"{
#endif 
typedef struct AUDIO_VOLUME_NOTIFICATION_DATA
{
GUID guidEventContext;
BOOL bMuted;
float fMasterVolume;
UINT nChannels;
float afChannelVolumes[ 1 ];
} 	AUDIO_VOLUME_NOTIFICATION_DATA;
typedef struct AUDIO_VOLUME_NOTIFICATION_DATA *PAUDIO_VOLUME_NOTIFICATION_DATA;
#define   ENDPOINT_HARDWARE_SUPPORT_VOLUME    0x00000001
#define   ENDPOINT_HARDWARE_SUPPORT_MUTE      0x00000002
#define   ENDPOINT_HARDWARE_SUPPORT_METER     0x00000004
extern RPC_IF_HANDLE __MIDL_itf_endpointvolume_0000_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_endpointvolume_0000_0000_v0_0_s_ifspec;
#ifndef __IAudioEndpointVolumeCallback_INTERFACE_DEFINED__
#define __IAudioEndpointVolumeCallback_INTERFACE_DEFINED__
EXTERN_C const IID IID_IAudioEndpointVolumeCallback;
#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("657804FA-D6AD-4496-8A60-352752AF4F89")
IAudioEndpointVolumeCallback : public IUnknown
{
public:
virtual HRESULT STDMETHODCALLTYPE OnNotify( 
PAUDIO_VOLUME_NOTIFICATION_DATA pNotify) = 0;
};
#else 	 
typedef struct IAudioEndpointVolumeCallbackVtbl
{
BEGIN_INTERFACE
HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
IAudioEndpointVolumeCallback * This,
REFIID riid,
__RPC__deref_out  void **ppvObject);
ULONG ( STDMETHODCALLTYPE *AddRef )( 
IAudioEndpointVolumeCallback * This);
ULONG ( STDMETHODCALLTYPE *Release )( 
IAudioEndpointVolumeCallback * This);
HRESULT ( STDMETHODCALLTYPE *OnNotify )( 
IAudioEndpointVolumeCallback * This,
PAUDIO_VOLUME_NOTIFICATION_DATA pNotify);
END_INTERFACE
} IAudioEndpointVolumeCallbackVtbl;
interface IAudioEndpointVolumeCallback
{
CONST_VTBL struct IAudioEndpointVolumeCallbackVtbl *lpVtbl;
};
#ifdef COBJMACROS
#define IAudioEndpointVolumeCallback_QueryInterface(This,riid,ppvObject)	\
( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define IAudioEndpointVolumeCallback_AddRef(This)	\
( (This)->lpVtbl -> AddRef(This) ) 
#define IAudioEndpointVolumeCallback_Release(This)	\
( (This)->lpVtbl -> Release(This) ) 
#define IAudioEndpointVolumeCallback_OnNotify(This,pNotify)	\
( (This)->lpVtbl -> OnNotify(This,pNotify) ) 
#endif  
#endif 	 
#endif 	 
#ifndef __IAudioEndpointVolume_INTERFACE_DEFINED__
#define __IAudioEndpointVolume_INTERFACE_DEFINED__
EXTERN_C const IID IID_IAudioEndpointVolume;
#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("5CDF2C82-841E-4546-9722-0CF74078229A")
IAudioEndpointVolume : public IUnknown
{
public:
virtual   HRESULT STDMETHODCALLTYPE RegisterControlChangeNotify( 
__in  IAudioEndpointVolumeCallback *pNotify) = 0;
virtual   HRESULT STDMETHODCALLTYPE UnregisterControlChangeNotify( 
__in  IAudioEndpointVolumeCallback *pNotify) = 0;
virtual   HRESULT STDMETHODCALLTYPE GetChannelCount( 
__out  UINT *pnChannelCount) = 0;
virtual   HRESULT STDMETHODCALLTYPE SetMasterVolumeLevel( 
__in  float fLevelDB,
LPCGUID pguidEventContext) = 0;
virtual   HRESULT STDMETHODCALLTYPE SetMasterVolumeLevelScalar( 
__in  float fLevel,
LPCGUID pguidEventContext) = 0;
virtual   HRESULT STDMETHODCALLTYPE GetMasterVolumeLevel( 
__out  float *pfLevelDB) = 0;
virtual   HRESULT STDMETHODCALLTYPE GetMasterVolumeLevelScalar( 
__out  float *pfLevel) = 0;
virtual   HRESULT STDMETHODCALLTYPE SetChannelVolumeLevel( 
__in  UINT nChannel,
float fLevelDB,
LPCGUID pguidEventContext) = 0;
virtual   HRESULT STDMETHODCALLTYPE SetChannelVolumeLevelScalar( 
__in  UINT nChannel,
float fLevel,
LPCGUID pguidEventContext) = 0;
virtual   HRESULT STDMETHODCALLTYPE GetChannelVolumeLevel( 
__in  UINT nChannel,
__out  float *pfLevelDB) = 0;
virtual   HRESULT STDMETHODCALLTYPE GetChannelVolumeLevelScalar( 
__in  UINT nChannel,
__out  float *pfLevel) = 0;
virtual   HRESULT STDMETHODCALLTYPE SetMute( 
__in  BOOL bMute,
LPCGUID pguidEventContext) = 0;
virtual   HRESULT STDMETHODCALLTYPE GetMute( 
__out  BOOL *pbMute) = 0;
virtual   HRESULT STDMETHODCALLTYPE GetVolumeStepInfo( 
__out  UINT *pnStep,
__out  UINT *pnStepCount) = 0;
virtual   HRESULT STDMETHODCALLTYPE VolumeStepUp( 
LPCGUID pguidEventContext) = 0;
virtual   HRESULT STDMETHODCALLTYPE VolumeStepDown( 
LPCGUID pguidEventContext) = 0;
virtual   HRESULT STDMETHODCALLTYPE QueryHardwareSupport( 
__out  DWORD *pdwHardwareSupportMask) = 0;
virtual   HRESULT STDMETHODCALLTYPE GetVolumeRange( 
__out  float *pflVolumeMindB,
__out  float *pflVolumeMaxdB,
__out  float *pflVolumeIncrementdB) = 0;
};
#else 	 
typedef struct IAudioEndpointVolumeVtbl
{
BEGIN_INTERFACE
HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
IAudioEndpointVolume * This,
REFIID riid,
__RPC__deref_out  void **ppvObject);
ULONG ( STDMETHODCALLTYPE *AddRef )( 
IAudioEndpointVolume * This);
ULONG ( STDMETHODCALLTYPE *Release )( 
IAudioEndpointVolume * This);
HRESULT ( STDMETHODCALLTYPE *RegisterControlChangeNotify )( 
IAudioEndpointVolume * This,
__in  IAudioEndpointVolumeCallback *pNotify);
HRESULT ( STDMETHODCALLTYPE *UnregisterControlChangeNotify )( 
IAudioEndpointVolume * This,
__in  IAudioEndpointVolumeCallback *pNotify);
HRESULT ( STDMETHODCALLTYPE *GetChannelCount )( 
IAudioEndpointVolume * This,
__out  UINT *pnChannelCount);
HRESULT ( STDMETHODCALLTYPE *SetMasterVolumeLevel )( 
IAudioEndpointVolume * This,
__in  float fLevelDB,
LPCGUID pguidEventContext);
HRESULT ( STDMETHODCALLTYPE *SetMasterVolumeLevelScalar )( 
IAudioEndpointVolume * This,
__in  float fLevel,
LPCGUID pguidEventContext);
HRESULT ( STDMETHODCALLTYPE *GetMasterVolumeLevel )( 
IAudioEndpointVolume * This,
__out  float *pfLevelDB);
HRESULT ( STDMETHODCALLTYPE *GetMasterVolumeLevelScalar )( 
IAudioEndpointVolume * This,
__out  float *pfLevel);
HRESULT ( STDMETHODCALLTYPE *SetChannelVolumeLevel )( 
IAudioEndpointVolume * This,
__in  UINT nChannel,
float fLevelDB,
LPCGUID pguidEventContext);
HRESULT ( STDMETHODCALLTYPE *SetChannelVolumeLevelScalar )( 
IAudioEndpointVolume * This,
__in  UINT nChannel,
float fLevel,
LPCGUID pguidEventContext);
HRESULT ( STDMETHODCALLTYPE *GetChannelVolumeLevel )( 
IAudioEndpointVolume * This,
__in  UINT nChannel,
__out  float *pfLevelDB);
HRESULT ( STDMETHODCALLTYPE *GetChannelVolumeLevelScalar )( 
IAudioEndpointVolume * This,
__in  UINT nChannel,
__out  float *pfLevel);
HRESULT ( STDMETHODCALLTYPE *SetMute )( 
IAudioEndpointVolume * This,
__in  BOOL bMute,
LPCGUID pguidEventContext);
HRESULT ( STDMETHODCALLTYPE *GetMute )( 
IAudioEndpointVolume * This,
__out  BOOL *pbMute);
HRESULT ( STDMETHODCALLTYPE *GetVolumeStepInfo )( 
IAudioEndpointVolume * This,
__out  UINT *pnStep,
__out  UINT *pnStepCount);
HRESULT ( STDMETHODCALLTYPE *VolumeStepUp )( 
IAudioEndpointVolume * This,
LPCGUID pguidEventContext);
HRESULT ( STDMETHODCALLTYPE *VolumeStepDown )( 
IAudioEndpointVolume * This,
LPCGUID pguidEventContext);
HRESULT ( STDMETHODCALLTYPE *QueryHardwareSupport )( 
IAudioEndpointVolume * This,
__out  DWORD *pdwHardwareSupportMask);
HRESULT ( STDMETHODCALLTYPE *GetVolumeRange )( 
IAudioEndpointVolume * This,
__out  float *pflVolumeMindB,
__out  float *pflVolumeMaxdB,
__out  float *pflVolumeIncrementdB);
END_INTERFACE
} IAudioEndpointVolumeVtbl;
interface IAudioEndpointVolume
{
CONST_VTBL struct IAudioEndpointVolumeVtbl *lpVtbl;
};
#ifdef COBJMACROS
#define IAudioEndpointVolume_QueryInterface(This,riid,ppvObject)	\
( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define IAudioEndpointVolume_AddRef(This)	\
( (This)->lpVtbl -> AddRef(This) ) 
#define IAudioEndpointVolume_Release(This)	\
( (This)->lpVtbl -> Release(This) ) 
#define IAudioEndpointVolume_RegisterControlChangeNotify(This,pNotify)	\
( (This)->lpVtbl -> RegisterControlChangeNotify(This,pNotify) ) 
#define IAudioEndpointVolume_UnregisterControlChangeNotify(This,pNotify)	\
( (This)->lpVtbl -> UnregisterControlChangeNotify(This,pNotify) ) 
#define IAudioEndpointVolume_GetChannelCount(This,pnChannelCount)	\
( (This)->lpVtbl -> GetChannelCount(This,pnChannelCount) ) 
#define IAudioEndpointVolume_SetMasterVolumeLevel(This,fLevelDB,pguidEventContext)	\
( (This)->lpVtbl -> SetMasterVolumeLevel(This,fLevelDB,pguidEventContext) ) 
#define IAudioEndpointVolume_SetMasterVolumeLevelScalar(This,fLevel,pguidEventContext)	\
( (This)->lpVtbl -> SetMasterVolumeLevelScalar(This,fLevel,pguidEventContext) ) 
#define IAudioEndpointVolume_GetMasterVolumeLevel(This,pfLevelDB)	\
( (This)->lpVtbl -> GetMasterVolumeLevel(This,pfLevelDB) ) 
#define IAudioEndpointVolume_GetMasterVolumeLevelScalar(This,pfLevel)	\
( (This)->lpVtbl -> GetMasterVolumeLevelScalar(This,pfLevel) ) 
#define IAudioEndpointVolume_SetChannelVolumeLevel(This,nChannel,fLevelDB,pguidEventContext)	\
( (This)->lpVtbl -> SetChannelVolumeLevel(This,nChannel,fLevelDB,pguidEventContext) ) 
#define IAudioEndpointVolume_SetChannelVolumeLevelScalar(This,nChannel,fLevel,pguidEventContext)	\
( (This)->lpVtbl -> SetChannelVolumeLevelScalar(This,nChannel,fLevel,pguidEventContext) ) 
#define IAudioEndpointVolume_GetChannelVolumeLevel(This,nChannel,pfLevelDB)	\
( (This)->lpVtbl -> GetChannelVolumeLevel(This,nChannel,pfLevelDB) ) 
#define IAudioEndpointVolume_GetChannelVolumeLevelScalar(This,nChannel,pfLevel)	\
( (This)->lpVtbl -> GetChannelVolumeLevelScalar(This,nChannel,pfLevel) ) 
#define IAudioEndpointVolume_SetMute(This,bMute,pguidEventContext)	\
( (This)->lpVtbl -> SetMute(This,bMute,pguidEventContext) ) 
#define IAudioEndpointVolume_GetMute(This,pbMute)	\
( (This)->lpVtbl -> GetMute(This,pbMute) ) 
#define IAudioEndpointVolume_GetVolumeStepInfo(This,pnStep,pnStepCount)	\
( (This)->lpVtbl -> GetVolumeStepInfo(This,pnStep,pnStepCount) ) 
#define IAudioEndpointVolume_VolumeStepUp(This,pguidEventContext)	\
( (This)->lpVtbl -> VolumeStepUp(This,pguidEventContext) ) 
#define IAudioEndpointVolume_VolumeStepDown(This,pguidEventContext)	\
( (This)->lpVtbl -> VolumeStepDown(This,pguidEventContext) ) 
#define IAudioEndpointVolume_QueryHardwareSupport(This,pdwHardwareSupportMask)	\
( (This)->lpVtbl -> QueryHardwareSupport(This,pdwHardwareSupportMask) ) 
#define IAudioEndpointVolume_GetVolumeRange(This,pflVolumeMindB,pflVolumeMaxdB,pflVolumeIncrementdB)	\
( (This)->lpVtbl -> GetVolumeRange(This,pflVolumeMindB,pflVolumeMaxdB,pflVolumeIncrementdB) ) 
#endif  
#endif 	 
#endif 	 
#ifndef __IAudioMeterInformation_INTERFACE_DEFINED__
#define __IAudioMeterInformation_INTERFACE_DEFINED__
EXTERN_C const IID IID_IAudioMeterInformation;
#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("C02216F6-8C67-4B5B-9D00-D008E73E0064")
IAudioMeterInformation : public IUnknown
{
public:
virtual   HRESULT STDMETHODCALLTYPE GetPeakValue( 
float *pfPeak) = 0;
virtual   HRESULT STDMETHODCALLTYPE GetMeteringChannelCount( 
__out  UINT *pnChannelCount) = 0;
virtual   HRESULT STDMETHODCALLTYPE GetChannelsPeakValues( 
UINT32 u32ChannelCount,
float *afPeakValues) = 0;
virtual   HRESULT STDMETHODCALLTYPE QueryHardwareSupport( 
__out  DWORD *pdwHardwareSupportMask) = 0;
};
#else 	 
typedef struct IAudioMeterInformationVtbl
{
BEGIN_INTERFACE
HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
IAudioMeterInformation * This,
REFIID riid,
__RPC__deref_out  void **ppvObject);
ULONG ( STDMETHODCALLTYPE *AddRef )( 
IAudioMeterInformation * This);
ULONG ( STDMETHODCALLTYPE *Release )( 
IAudioMeterInformation * This);
HRESULT ( STDMETHODCALLTYPE *GetPeakValue )( 
IAudioMeterInformation * This,
float *pfPeak);
HRESULT ( STDMETHODCALLTYPE *GetMeteringChannelCount )( 
IAudioMeterInformation * This,
__out  UINT *pnChannelCount);
HRESULT ( STDMETHODCALLTYPE *GetChannelsPeakValues )( 
IAudioMeterInformation * This,
UINT32 u32ChannelCount,
float *afPeakValues);
HRESULT ( STDMETHODCALLTYPE *QueryHardwareSupport )( 
IAudioMeterInformation * This,
__out  DWORD *pdwHardwareSupportMask);
END_INTERFACE
} IAudioMeterInformationVtbl;
interface IAudioMeterInformation
{
CONST_VTBL struct IAudioMeterInformationVtbl *lpVtbl;
};
#ifdef COBJMACROS
#define IAudioMeterInformation_QueryInterface(This,riid,ppvObject)	\
( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define IAudioMeterInformation_AddRef(This)	\
( (This)->lpVtbl -> AddRef(This) ) 
#define IAudioMeterInformation_Release(This)	\
( (This)->lpVtbl -> Release(This) ) 
#define IAudioMeterInformation_GetPeakValue(This,pfPeak)	\
( (This)->lpVtbl -> GetPeakValue(This,pfPeak) ) 
#define IAudioMeterInformation_GetMeteringChannelCount(This,pnChannelCount)	\
( (This)->lpVtbl -> GetMeteringChannelCount(This,pnChannelCount) ) 
#define IAudioMeterInformation_GetChannelsPeakValues(This,u32ChannelCount,afPeakValues)	\
( (This)->lpVtbl -> GetChannelsPeakValues(This,u32ChannelCount,afPeakValues) ) 
#define IAudioMeterInformation_QueryHardwareSupport(This,pdwHardwareSupportMask)	\
( (This)->lpVtbl -> QueryHardwareSupport(This,pdwHardwareSupportMask) ) 
#endif  
#endif 	 
#endif 	 
#ifdef __cplusplus
}
#endif
#endif
