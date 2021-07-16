
#pragma warning( disable: 4049 )   
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
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
#ifndef __audioclient_h__
#define __audioclient_h__
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif
#ifndef __IAudioClient_FWD_DEFINED__
#define __IAudioClient_FWD_DEFINED__
typedef interface IAudioClient IAudioClient;
#endif 	 
#ifndef __IAudioRenderClient_FWD_DEFINED__
#define __IAudioRenderClient_FWD_DEFINED__
typedef interface IAudioRenderClient IAudioRenderClient;
#endif 	 
#ifndef __IAudioCaptureClient_FWD_DEFINED__
#define __IAudioCaptureClient_FWD_DEFINED__
typedef interface IAudioCaptureClient IAudioCaptureClient;
#endif 	 
#ifndef __IAudioClock_FWD_DEFINED__
#define __IAudioClock_FWD_DEFINED__
typedef interface IAudioClock IAudioClock;
#endif 	 
#ifndef __ISimpleAudioVolume_FWD_DEFINED__
#define __ISimpleAudioVolume_FWD_DEFINED__
typedef interface ISimpleAudioVolume ISimpleAudioVolume;
#endif 	 
#ifndef __IAudioStreamVolume_FWD_DEFINED__
#define __IAudioStreamVolume_FWD_DEFINED__
typedef interface IAudioStreamVolume IAudioStreamVolume;
#endif 	 
#ifndef __IChannelAudioVolume_FWD_DEFINED__
#define __IChannelAudioVolume_FWD_DEFINED__
typedef interface IChannelAudioVolume IChannelAudioVolume;
#endif 	 
#include "wtypes.h"
#include "unknwn.h"
#include "AudioSessionTypes.h"
#ifdef __cplusplus
extern "C"{
#endif 
#if 0
typedef   struct WAVEFORMATEX
{
WORD wFormatTag;
WORD nChannels;
DWORD nSamplesPerSec;
DWORD nAvgBytesPerSec;
WORD nBlockAlign;
WORD wBitsPerSample;
WORD cbSize;
} 	WAVEFORMATEX;
#else
#include <mmreg.h>
#endif
#if 0
typedef   LONGLONG REFERENCE_TIME;
#else
#define _IKsControl_
#include <ks.h>
#include <ksmedia.h>
#endif
enum _AUDCLNT_BUFFERFLAGS
{	AUDCLNT_BUFFERFLAGS_DATA_DISCONTINUITY	= 0x1,
	AUDCLNT_BUFFERFLAGS_SILENT	= 0x2,
	AUDCLNT_BUFFERFLAGS_TIMESTAMP_ERROR	= 0x4
} ;
extern RPC_IF_HANDLE __MIDL_itf_audioclient_0000_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_audioclient_0000_0000_v0_0_s_ifspec;
#ifndef __IAudioClient_INTERFACE_DEFINED__
#define __IAudioClient_INTERFACE_DEFINED__
EXTERN_C const IID IID_IAudioClient;
#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("1CB9AD4C-DBFA-4c32-B178-C2F568A703B2")
IAudioClient : public IUnknown
{
public:
virtual HRESULT STDMETHODCALLTYPE Initialize( 
__in  AUDCLNT_SHAREMODE ShareMode,
__in  DWORD StreamFlags,
__in  REFERENCE_TIME hnsBufferDuration,
__in  REFERENCE_TIME hnsPeriodicity,
__in  const WAVEFORMATEX *pFormat,
__in_opt  LPCGUID AudioSessionGuid) = 0;
virtual HRESULT STDMETHODCALLTYPE GetBufferSize( 
__out  UINT32 *pNumBufferFrames) = 0;
virtual HRESULT STDMETHODCALLTYPE GetStreamLatency( 
__out  REFERENCE_TIME *phnsLatency) = 0;
virtual HRESULT STDMETHODCALLTYPE GetCurrentPadding( 
__out  UINT32 *pNumPaddingFrames) = 0;
virtual HRESULT STDMETHODCALLTYPE IsFormatSupported( 
__in  AUDCLNT_SHAREMODE ShareMode,
__in  const WAVEFORMATEX *pFormat,
__out_opt  WAVEFORMATEX **ppClosestMatch) = 0;
virtual HRESULT STDMETHODCALLTYPE GetMixFormat( 
__out  WAVEFORMATEX **ppDeviceFormat) = 0;
virtual HRESULT STDMETHODCALLTYPE GetDevicePeriod( 
__out_opt  REFERENCE_TIME *phnsDefaultDevicePeriod,
__out_opt  REFERENCE_TIME *phnsMinimumDevicePeriod) = 0;
virtual HRESULT STDMETHODCALLTYPE Start( void) = 0;
virtual HRESULT STDMETHODCALLTYPE Stop( void) = 0;
virtual HRESULT STDMETHODCALLTYPE Reset( void) = 0;
virtual HRESULT STDMETHODCALLTYPE SetEventHandle( 
HANDLE eventHandle) = 0;
virtual HRESULT STDMETHODCALLTYPE GetService( 
__in  REFIID riid,
__out  void **ppv) = 0;
};
#else 	 
typedef struct IAudioClientVtbl
{
BEGIN_INTERFACE
HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
IAudioClient * This,
REFIID riid,
__RPC__deref_out  void **ppvObject);
ULONG ( STDMETHODCALLTYPE *AddRef )( 
IAudioClient * This);
ULONG ( STDMETHODCALLTYPE *Release )( 
IAudioClient * This);
HRESULT ( STDMETHODCALLTYPE *Initialize )( 
IAudioClient * This,
__in  AUDCLNT_SHAREMODE ShareMode,
__in  DWORD StreamFlags,
__in  REFERENCE_TIME hnsBufferDuration,
__in  REFERENCE_TIME hnsPeriodicity,
__in  const WAVEFORMATEX *pFormat,
__in_opt  LPCGUID AudioSessionGuid);
HRESULT ( STDMETHODCALLTYPE *GetBufferSize )( 
IAudioClient * This,
__out  UINT32 *pNumBufferFrames);
HRESULT ( STDMETHODCALLTYPE *GetStreamLatency )( 
IAudioClient * This,
__out  REFERENCE_TIME *phnsLatency);
HRESULT ( STDMETHODCALLTYPE *GetCurrentPadding )( 
IAudioClient * This,
__out  UINT32 *pNumPaddingFrames);
HRESULT ( STDMETHODCALLTYPE *IsFormatSupported )( 
IAudioClient * This,
__in  AUDCLNT_SHAREMODE ShareMode,
__in  const WAVEFORMATEX *pFormat,
__out_opt  WAVEFORMATEX **ppClosestMatch);
HRESULT ( STDMETHODCALLTYPE *GetMixFormat )( 
IAudioClient * This,
__out  WAVEFORMATEX **ppDeviceFormat);
HRESULT ( STDMETHODCALLTYPE *GetDevicePeriod )( 
IAudioClient * This,
__out_opt  REFERENCE_TIME *phnsDefaultDevicePeriod,
__out_opt  REFERENCE_TIME *phnsMinimumDevicePeriod);
HRESULT ( STDMETHODCALLTYPE *Start )( 
IAudioClient * This);
HRESULT ( STDMETHODCALLTYPE *Stop )( 
IAudioClient * This);
HRESULT ( STDMETHODCALLTYPE *Reset )( 
IAudioClient * This);
HRESULT ( STDMETHODCALLTYPE *SetEventHandle )( 
IAudioClient * This,
HANDLE eventHandle);
HRESULT ( STDMETHODCALLTYPE *GetService )( 
IAudioClient * This,
__in  REFIID riid,
__out  void **ppv);
END_INTERFACE
} IAudioClientVtbl;
interface IAudioClient
{
CONST_VTBL struct IAudioClientVtbl *lpVtbl;
};
#ifdef COBJMACROS
#define IAudioClient_QueryInterface(This,riid,ppvObject)	\
( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define IAudioClient_AddRef(This)	\
( (This)->lpVtbl -> AddRef(This) ) 
#define IAudioClient_Release(This)	\
( (This)->lpVtbl -> Release(This) ) 
#define IAudioClient_Initialize(This,ShareMode,StreamFlags,hnsBufferDuration,hnsPeriodicity,pFormat,AudioSessionGuid)	\
( (This)->lpVtbl -> Initialize(This,ShareMode,StreamFlags,hnsBufferDuration,hnsPeriodicity,pFormat,AudioSessionGuid) ) 
#define IAudioClient_GetBufferSize(This,pNumBufferFrames)	\
( (This)->lpVtbl -> GetBufferSize(This,pNumBufferFrames) ) 
#define IAudioClient_GetStreamLatency(This,phnsLatency)	\
( (This)->lpVtbl -> GetStreamLatency(This,phnsLatency) ) 
#define IAudioClient_GetCurrentPadding(This,pNumPaddingFrames)	\
( (This)->lpVtbl -> GetCurrentPadding(This,pNumPaddingFrames) ) 
#define IAudioClient_IsFormatSupported(This,ShareMode,pFormat,ppClosestMatch)	\
( (This)->lpVtbl -> IsFormatSupported(This,ShareMode,pFormat,ppClosestMatch) ) 
#define IAudioClient_GetMixFormat(This,ppDeviceFormat)	\
( (This)->lpVtbl -> GetMixFormat(This,ppDeviceFormat) ) 
#define IAudioClient_GetDevicePeriod(This,phnsDefaultDevicePeriod,phnsMinimumDevicePeriod)	\
( (This)->lpVtbl -> GetDevicePeriod(This,phnsDefaultDevicePeriod,phnsMinimumDevicePeriod) ) 
#define IAudioClient_Start(This)	\
( (This)->lpVtbl -> Start(This) ) 
#define IAudioClient_Stop(This)	\
( (This)->lpVtbl -> Stop(This) ) 
#define IAudioClient_Reset(This)	\
( (This)->lpVtbl -> Reset(This) ) 
#define IAudioClient_SetEventHandle(This,eventHandle)	\
( (This)->lpVtbl -> SetEventHandle(This,eventHandle) ) 
#define IAudioClient_GetService(This,riid,ppv)	\
( (This)->lpVtbl -> GetService(This,riid,ppv) ) 
#endif  
#endif 	 
#endif 	 
#ifndef __IAudioRenderClient_INTERFACE_DEFINED__
#define __IAudioRenderClient_INTERFACE_DEFINED__
EXTERN_C const IID IID_IAudioRenderClient;
#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("F294ACFC-3146-4483-A7BF-ADDCA7C260E2")
IAudioRenderClient : public IUnknown
{
public:
virtual HRESULT STDMETHODCALLTYPE GetBuffer( 
__in  UINT32 NumFramesRequested,
__out  BYTE **ppData) = 0;
virtual HRESULT STDMETHODCALLTYPE ReleaseBuffer( 
__in  UINT32 NumFramesWritten,
__in  DWORD dwFlags) = 0;
};
#else 	 
typedef struct IAudioRenderClientVtbl
{
BEGIN_INTERFACE
HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
IAudioRenderClient * This,
REFIID riid,
__RPC__deref_out  void **ppvObject);
ULONG ( STDMETHODCALLTYPE *AddRef )( 
IAudioRenderClient * This);
ULONG ( STDMETHODCALLTYPE *Release )( 
IAudioRenderClient * This);
HRESULT ( STDMETHODCALLTYPE *GetBuffer )( 
IAudioRenderClient * This,
__in  UINT32 NumFramesRequested,
__out  BYTE **ppData);
HRESULT ( STDMETHODCALLTYPE *ReleaseBuffer )( 
IAudioRenderClient * This,
__in  UINT32 NumFramesWritten,
__in  DWORD dwFlags);
END_INTERFACE
} IAudioRenderClientVtbl;
interface IAudioRenderClient
{
CONST_VTBL struct IAudioRenderClientVtbl *lpVtbl;
};
#ifdef COBJMACROS
#define IAudioRenderClient_QueryInterface(This,riid,ppvObject)	\
( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define IAudioRenderClient_AddRef(This)	\
( (This)->lpVtbl -> AddRef(This) ) 
#define IAudioRenderClient_Release(This)	\
( (This)->lpVtbl -> Release(This) ) 
#define IAudioRenderClient_GetBuffer(This,NumFramesRequested,ppData)	\
( (This)->lpVtbl -> GetBuffer(This,NumFramesRequested,ppData) ) 
#define IAudioRenderClient_ReleaseBuffer(This,NumFramesWritten,dwFlags)	\
( (This)->lpVtbl -> ReleaseBuffer(This,NumFramesWritten,dwFlags) ) 
#endif  
#endif 	 
#endif 	 
#ifndef __IAudioCaptureClient_INTERFACE_DEFINED__
#define __IAudioCaptureClient_INTERFACE_DEFINED__
EXTERN_C const IID IID_IAudioCaptureClient;
#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("C8ADBD64-E71E-48a0-A4DE-185C395CD317")
IAudioCaptureClient : public IUnknown
{
public:
virtual HRESULT STDMETHODCALLTYPE GetBuffer( 
__out  BYTE **ppData,
__out  UINT32 *pNumFramesToRead,
__out  DWORD *pdwFlags,
__out_opt  UINT64 *pu64DevicePosition,
__out_opt  UINT64 *pu64QPCPosition) = 0;
virtual HRESULT STDMETHODCALLTYPE ReleaseBuffer( 
__in  UINT32 NumFramesRead) = 0;
virtual HRESULT STDMETHODCALLTYPE GetNextPacketSize( 
__out  UINT32 *pNumFramesInNextPacket) = 0;
};
#else 	 
typedef struct IAudioCaptureClientVtbl
{
BEGIN_INTERFACE
HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
IAudioCaptureClient * This,
REFIID riid,
__RPC__deref_out  void **ppvObject);
ULONG ( STDMETHODCALLTYPE *AddRef )( 
IAudioCaptureClient * This);
ULONG ( STDMETHODCALLTYPE *Release )( 
IAudioCaptureClient * This);
HRESULT ( STDMETHODCALLTYPE *GetBuffer )( 
IAudioCaptureClient * This,
__out  BYTE **ppData,
__out  UINT32 *pNumFramesToRead,
__out  DWORD *pdwFlags,
__out_opt  UINT64 *pu64DevicePosition,
__out_opt  UINT64 *pu64QPCPosition);
HRESULT ( STDMETHODCALLTYPE *ReleaseBuffer )( 
IAudioCaptureClient * This,
__in  UINT32 NumFramesRead);
HRESULT ( STDMETHODCALLTYPE *GetNextPacketSize )( 
IAudioCaptureClient * This,
__out  UINT32 *pNumFramesInNextPacket);
END_INTERFACE
} IAudioCaptureClientVtbl;
interface IAudioCaptureClient
{
CONST_VTBL struct IAudioCaptureClientVtbl *lpVtbl;
};
#ifdef COBJMACROS
#define IAudioCaptureClient_QueryInterface(This,riid,ppvObject)	\
( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define IAudioCaptureClient_AddRef(This)	\
( (This)->lpVtbl -> AddRef(This) ) 
#define IAudioCaptureClient_Release(This)	\
( (This)->lpVtbl -> Release(This) ) 
#define IAudioCaptureClient_GetBuffer(This,ppData,pNumFramesToRead,pdwFlags,pu64DevicePosition,pu64QPCPosition)	\
( (This)->lpVtbl -> GetBuffer(This,ppData,pNumFramesToRead,pdwFlags,pu64DevicePosition,pu64QPCPosition) ) 
#define IAudioCaptureClient_ReleaseBuffer(This,NumFramesRead)	\
( (This)->lpVtbl -> ReleaseBuffer(This,NumFramesRead) ) 
#define IAudioCaptureClient_GetNextPacketSize(This,pNumFramesInNextPacket)	\
( (This)->lpVtbl -> GetNextPacketSize(This,pNumFramesInNextPacket) ) 
#endif  
#endif 	 
#endif 	 
#define AUDIOCLOCK_CHARACTERISTIC_FIXED_FREQ  0x00000001
extern RPC_IF_HANDLE __MIDL_itf_audioclient_0000_0003_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_audioclient_0000_0003_v0_0_s_ifspec;
#ifndef __IAudioClock_INTERFACE_DEFINED__
#define __IAudioClock_INTERFACE_DEFINED__
EXTERN_C const IID IID_IAudioClock;
#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("CD63314F-3FBA-4a1b-812C-EF96358728E7")
IAudioClock : public IUnknown
{
public:
virtual HRESULT STDMETHODCALLTYPE GetFrequency( 
__out  UINT64 *pu64Frequency) = 0;
virtual HRESULT STDMETHODCALLTYPE GetPosition( 
__out  UINT64 *pu64Position,
__out_opt  UINT64 *pu64QPCPosition) = 0;
virtual HRESULT STDMETHODCALLTYPE GetCharacteristics( 
__out  DWORD *pdwCharacteristics) = 0;
};
#else 	 
typedef struct IAudioClockVtbl
{
BEGIN_INTERFACE
HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
IAudioClock * This,
REFIID riid,
__RPC__deref_out  void **ppvObject);
ULONG ( STDMETHODCALLTYPE *AddRef )( 
IAudioClock * This);
ULONG ( STDMETHODCALLTYPE *Release )( 
IAudioClock * This);
HRESULT ( STDMETHODCALLTYPE *GetFrequency )( 
IAudioClock * This,
__out  UINT64 *pu64Frequency);
HRESULT ( STDMETHODCALLTYPE *GetPosition )( 
IAudioClock * This,
__out  UINT64 *pu64Position,
__out_opt  UINT64 *pu64QPCPosition);
HRESULT ( STDMETHODCALLTYPE *GetCharacteristics )( 
IAudioClock * This,
__out  DWORD *pdwCharacteristics);
END_INTERFACE
} IAudioClockVtbl;
interface IAudioClock
{
CONST_VTBL struct IAudioClockVtbl *lpVtbl;
};
#ifdef COBJMACROS
#define IAudioClock_QueryInterface(This,riid,ppvObject)	\
( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define IAudioClock_AddRef(This)	\
( (This)->lpVtbl -> AddRef(This) ) 
#define IAudioClock_Release(This)	\
( (This)->lpVtbl -> Release(This) ) 
#define IAudioClock_GetFrequency(This,pu64Frequency)	\
( (This)->lpVtbl -> GetFrequency(This,pu64Frequency) ) 
#define IAudioClock_GetPosition(This,pu64Position,pu64QPCPosition)	\
( (This)->lpVtbl -> GetPosition(This,pu64Position,pu64QPCPosition) ) 
#define IAudioClock_GetCharacteristics(This,pdwCharacteristics)	\
( (This)->lpVtbl -> GetCharacteristics(This,pdwCharacteristics) ) 
#endif  
#endif 	 
#endif 	 
#ifndef __ISimpleAudioVolume_INTERFACE_DEFINED__
#define __ISimpleAudioVolume_INTERFACE_DEFINED__
EXTERN_C const IID IID_ISimpleAudioVolume;
#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("87CE5498-68D6-44E5-9215-6DA47EF883D8")
ISimpleAudioVolume : public IUnknown
{
public:
virtual HRESULT STDMETHODCALLTYPE SetMasterVolume( 
__in  float fLevel,
LPCGUID EventContext) = 0;
virtual HRESULT STDMETHODCALLTYPE GetMasterVolume( 
__out  float *pfLevel) = 0;
virtual HRESULT STDMETHODCALLTYPE SetMute( 
__in  const BOOL bMute,
LPCGUID EventContext) = 0;
virtual HRESULT STDMETHODCALLTYPE GetMute( 
__out  BOOL *pbMute) = 0;
};
#else 	 
typedef struct ISimpleAudioVolumeVtbl
{
BEGIN_INTERFACE
HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
ISimpleAudioVolume * This,
REFIID riid,
__RPC__deref_out  void **ppvObject);
ULONG ( STDMETHODCALLTYPE *AddRef )( 
ISimpleAudioVolume * This);
ULONG ( STDMETHODCALLTYPE *Release )( 
ISimpleAudioVolume * This);
HRESULT ( STDMETHODCALLTYPE *SetMasterVolume )( 
ISimpleAudioVolume * This,
__in  float fLevel,
LPCGUID EventContext);
HRESULT ( STDMETHODCALLTYPE *GetMasterVolume )( 
ISimpleAudioVolume * This,
__out  float *pfLevel);
HRESULT ( STDMETHODCALLTYPE *SetMute )( 
ISimpleAudioVolume * This,
__in  const BOOL bMute,
LPCGUID EventContext);
HRESULT ( STDMETHODCALLTYPE *GetMute )( 
ISimpleAudioVolume * This,
__out  BOOL *pbMute);
END_INTERFACE
} ISimpleAudioVolumeVtbl;
interface ISimpleAudioVolume
{
CONST_VTBL struct ISimpleAudioVolumeVtbl *lpVtbl;
};
#ifdef COBJMACROS
#define ISimpleAudioVolume_QueryInterface(This,riid,ppvObject)	\
( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define ISimpleAudioVolume_AddRef(This)	\
( (This)->lpVtbl -> AddRef(This) ) 
#define ISimpleAudioVolume_Release(This)	\
( (This)->lpVtbl -> Release(This) ) 
#define ISimpleAudioVolume_SetMasterVolume(This,fLevel,EventContext)	\
( (This)->lpVtbl -> SetMasterVolume(This,fLevel,EventContext) ) 
#define ISimpleAudioVolume_GetMasterVolume(This,pfLevel)	\
( (This)->lpVtbl -> GetMasterVolume(This,pfLevel) ) 
#define ISimpleAudioVolume_SetMute(This,bMute,EventContext)	\
( (This)->lpVtbl -> SetMute(This,bMute,EventContext) ) 
#define ISimpleAudioVolume_GetMute(This,pbMute)	\
( (This)->lpVtbl -> GetMute(This,pbMute) ) 
#endif  
#endif 	 
#endif 	 
#ifndef __IAudioStreamVolume_INTERFACE_DEFINED__
#define __IAudioStreamVolume_INTERFACE_DEFINED__
EXTERN_C const IID IID_IAudioStreamVolume;
#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("93014887-242D-4068-8A15-CF5E93B90FE3")
IAudioStreamVolume : public IUnknown
{
public:
virtual HRESULT STDMETHODCALLTYPE GetChannelCount( 
__out  UINT32 *pdwCount) = 0;
virtual HRESULT STDMETHODCALLTYPE SetChannelVolume( 
__in  UINT32 dwIndex,
__in  const float fLevel) = 0;
virtual HRESULT STDMETHODCALLTYPE GetChannelVolume( 
__in  UINT32 dwIndex,
__out  float *pfLevel) = 0;
virtual HRESULT STDMETHODCALLTYPE SetAllVolumes( 
__in  UINT32 dwCount,
__in_ecount(dwCount)  const float *pfVolumes) = 0;
virtual HRESULT STDMETHODCALLTYPE GetAllVolumes( 
__in  UINT32 dwCount,
__out_ecount(dwCount)  float *pfVolumes) = 0;
};
#else 	 
typedef struct IAudioStreamVolumeVtbl
{
BEGIN_INTERFACE
HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
IAudioStreamVolume * This,
REFIID riid,
__RPC__deref_out  void **ppvObject);
ULONG ( STDMETHODCALLTYPE *AddRef )( 
IAudioStreamVolume * This);
ULONG ( STDMETHODCALLTYPE *Release )( 
IAudioStreamVolume * This);
HRESULT ( STDMETHODCALLTYPE *GetChannelCount )( 
IAudioStreamVolume * This,
__out  UINT32 *pdwCount);
HRESULT ( STDMETHODCALLTYPE *SetChannelVolume )( 
IAudioStreamVolume * This,
__in  UINT32 dwIndex,
__in  const float fLevel);
HRESULT ( STDMETHODCALLTYPE *GetChannelVolume )( 
IAudioStreamVolume * This,
__in  UINT32 dwIndex,
__out  float *pfLevel);
HRESULT ( STDMETHODCALLTYPE *SetAllVolumes )( 
IAudioStreamVolume * This,
__in  UINT32 dwCount,
__in_ecount(dwCount)  const float *pfVolumes);
HRESULT ( STDMETHODCALLTYPE *GetAllVolumes )( 
IAudioStreamVolume * This,
__in  UINT32 dwCount,
__out_ecount(dwCount)  float *pfVolumes);
END_INTERFACE
} IAudioStreamVolumeVtbl;
interface IAudioStreamVolume
{
CONST_VTBL struct IAudioStreamVolumeVtbl *lpVtbl;
};
#ifdef COBJMACROS
#define IAudioStreamVolume_QueryInterface(This,riid,ppvObject)	\
( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define IAudioStreamVolume_AddRef(This)	\
( (This)->lpVtbl -> AddRef(This) ) 
#define IAudioStreamVolume_Release(This)	\
( (This)->lpVtbl -> Release(This) ) 
#define IAudioStreamVolume_GetChannelCount(This,pdwCount)	\
( (This)->lpVtbl -> GetChannelCount(This,pdwCount) ) 
#define IAudioStreamVolume_SetChannelVolume(This,dwIndex,fLevel)	\
( (This)->lpVtbl -> SetChannelVolume(This,dwIndex,fLevel) ) 
#define IAudioStreamVolume_GetChannelVolume(This,dwIndex,pfLevel)	\
( (This)->lpVtbl -> GetChannelVolume(This,dwIndex,pfLevel) ) 
#define IAudioStreamVolume_SetAllVolumes(This,dwCount,pfVolumes)	\
( (This)->lpVtbl -> SetAllVolumes(This,dwCount,pfVolumes) ) 
#define IAudioStreamVolume_GetAllVolumes(This,dwCount,pfVolumes)	\
( (This)->lpVtbl -> GetAllVolumes(This,dwCount,pfVolumes) ) 
#endif  
#endif 	 
#endif 	 
#ifndef __IChannelAudioVolume_INTERFACE_DEFINED__
#define __IChannelAudioVolume_INTERFACE_DEFINED__
EXTERN_C const IID IID_IChannelAudioVolume;
#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("1C158861-B533-4B30-B1CF-E853E51C59B8")
IChannelAudioVolume : public IUnknown
{
public:
virtual HRESULT STDMETHODCALLTYPE GetChannelCount( 
__out  UINT32 *pdwCount) = 0;
virtual HRESULT STDMETHODCALLTYPE SetChannelVolume( 
__in  UINT32 dwIndex,
__in  const float fLevel,
LPCGUID EventContext) = 0;
virtual HRESULT STDMETHODCALLTYPE GetChannelVolume( 
__in  UINT32 dwIndex,
__out  float *pfLevel) = 0;
virtual HRESULT STDMETHODCALLTYPE SetAllVolumes( 
__in  UINT32 dwCount,
__in_ecount(dwCount)  const float *pfVolumes,
LPCGUID EventContext) = 0;
virtual HRESULT STDMETHODCALLTYPE GetAllVolumes( 
__in  UINT32 dwCount,
__out_ecount(dwCount)  float *pfVolumes) = 0;
};
#else 	 
typedef struct IChannelAudioVolumeVtbl
{
BEGIN_INTERFACE
HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
IChannelAudioVolume * This,
REFIID riid,
__RPC__deref_out  void **ppvObject);
ULONG ( STDMETHODCALLTYPE *AddRef )( 
IChannelAudioVolume * This);
ULONG ( STDMETHODCALLTYPE *Release )( 
IChannelAudioVolume * This);
HRESULT ( STDMETHODCALLTYPE *GetChannelCount )( 
IChannelAudioVolume * This,
__out  UINT32 *pdwCount);
HRESULT ( STDMETHODCALLTYPE *SetChannelVolume )( 
IChannelAudioVolume * This,
__in  UINT32 dwIndex,
__in  const float fLevel,
LPCGUID EventContext);
HRESULT ( STDMETHODCALLTYPE *GetChannelVolume )( 
IChannelAudioVolume * This,
__in  UINT32 dwIndex,
__out  float *pfLevel);
HRESULT ( STDMETHODCALLTYPE *SetAllVolumes )( 
IChannelAudioVolume * This,
__in  UINT32 dwCount,
__in_ecount(dwCount)  const float *pfVolumes,
LPCGUID EventContext);
HRESULT ( STDMETHODCALLTYPE *GetAllVolumes )( 
IChannelAudioVolume * This,
__in  UINT32 dwCount,
__out_ecount(dwCount)  float *pfVolumes);
END_INTERFACE
} IChannelAudioVolumeVtbl;
interface IChannelAudioVolume
{
CONST_VTBL struct IChannelAudioVolumeVtbl *lpVtbl;
};
#ifdef COBJMACROS
#define IChannelAudioVolume_QueryInterface(This,riid,ppvObject)	\
( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define IChannelAudioVolume_AddRef(This)	\
( (This)->lpVtbl -> AddRef(This) ) 
#define IChannelAudioVolume_Release(This)	\
( (This)->lpVtbl -> Release(This) ) 
#define IChannelAudioVolume_GetChannelCount(This,pdwCount)	\
( (This)->lpVtbl -> GetChannelCount(This,pdwCount) ) 
#define IChannelAudioVolume_SetChannelVolume(This,dwIndex,fLevel,EventContext)	\
( (This)->lpVtbl -> SetChannelVolume(This,dwIndex,fLevel,EventContext) ) 
#define IChannelAudioVolume_GetChannelVolume(This,dwIndex,pfLevel)	\
( (This)->lpVtbl -> GetChannelVolume(This,dwIndex,pfLevel) ) 
#define IChannelAudioVolume_SetAllVolumes(This,dwCount,pfVolumes,EventContext)	\
( (This)->lpVtbl -> SetAllVolumes(This,dwCount,pfVolumes,EventContext) ) 
#define IChannelAudioVolume_GetAllVolumes(This,dwCount,pfVolumes)	\
( (This)->lpVtbl -> GetAllVolumes(This,dwCount,pfVolumes) ) 
#endif  
#endif 	 
#endif 	 
#define FACILITY_AUDCLNT 0x889
#define AUDCLNT_ERR(n) MAKE_HRESULT(SEVERITY_ERROR, FACILITY_AUDCLNT, n)
#define AUDCLNT_SUCCESS(n) MAKE_SCODE(SEVERITY_SUCCESS, FACILITY_AUDCLNT, n)
#define AUDCLNT_E_NOT_INITIALIZED            AUDCLNT_ERR(0x001)
#define AUDCLNT_E_ALREADY_INITIALIZED        AUDCLNT_ERR(0x002)
#define AUDCLNT_E_WRONG_ENDPOINT_TYPE        AUDCLNT_ERR(0x003)
#define AUDCLNT_E_DEVICE_INVALIDATED         AUDCLNT_ERR(0x004)
#define AUDCLNT_E_NOT_STOPPED                AUDCLNT_ERR(0x005)
#define AUDCLNT_E_BUFFER_TOO_LARGE           AUDCLNT_ERR(0x006)
#define AUDCLNT_E_OUT_OF_ORDER               AUDCLNT_ERR(0x007)
#define AUDCLNT_E_UNSUPPORTED_FORMAT         AUDCLNT_ERR(0x008)
#define AUDCLNT_E_INVALID_SIZE               AUDCLNT_ERR(0x009)
#define AUDCLNT_E_DEVICE_IN_USE              AUDCLNT_ERR(0x00a)
#define AUDCLNT_E_BUFFER_OPERATION_PENDING   AUDCLNT_ERR(0x00b)
#define AUDCLNT_E_THREAD_NOT_REGISTERED      AUDCLNT_ERR(0x00c)
#define AUDCLNT_E_EXCLUSIVE_MODE_NOT_ALLOWED AUDCLNT_ERR(0x00e)
#define AUDCLNT_E_ENDPOINT_CREATE_FAILED     AUDCLNT_ERR(0x00f)
#define AUDCLNT_E_SERVICE_NOT_RUNNING        AUDCLNT_ERR(0x010)
#define AUDCLNT_E_EVENTHANDLE_NOT_EXPECTED     AUDCLNT_ERR(0x011)
#define AUDCLNT_E_EXCLUSIVE_MODE_ONLY          AUDCLNT_ERR(0x012)
#define AUDCLNT_E_BUFDURATION_PERIOD_NOT_EQUAL AUDCLNT_ERR(0x013)
#define AUDCLNT_E_EVENTHANDLE_NOT_SET          AUDCLNT_ERR(0x014)
#define AUDCLNT_E_INCORRECT_BUFFER_SIZE        AUDCLNT_ERR(0x015)
#define AUDCLNT_E_BUFFER_SIZE_ERROR            AUDCLNT_ERR(0x016)
#define AUDCLNT_E_CPUUSAGE_EXCEEDED            AUDCLNT_ERR(0x017)
#define AUDCLNT_S_BUFFER_EMPTY              AUDCLNT_SUCCESS(0x001)
#define AUDCLNT_S_THREAD_ALREADY_REGISTERED AUDCLNT_SUCCESS(0x002)
#define AUDCLNT_S_POSITION_STALLED		   AUDCLNT_SUCCESS(0x003)
extern RPC_IF_HANDLE __MIDL_itf_audioclient_0000_0007_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_audioclient_0000_0007_v0_0_s_ifspec;
#ifdef __cplusplus
}
#endif
#endif
