
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
#ifndef __devicetopology_h__
#define __devicetopology_h__
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif
#ifndef __IKsControl_FWD_DEFINED__
#define __IKsControl_FWD_DEFINED__
typedef interface IKsControl IKsControl;
#endif 	 
#ifndef __IPerChannelDbLevel_FWD_DEFINED__
#define __IPerChannelDbLevel_FWD_DEFINED__
typedef interface IPerChannelDbLevel IPerChannelDbLevel;
#endif 	 
#ifndef __IAudioVolumeLevel_FWD_DEFINED__
#define __IAudioVolumeLevel_FWD_DEFINED__
typedef interface IAudioVolumeLevel IAudioVolumeLevel;
#endif 	 
#ifndef __IAudioChannelConfig_FWD_DEFINED__
#define __IAudioChannelConfig_FWD_DEFINED__
typedef interface IAudioChannelConfig IAudioChannelConfig;
#endif 	 
#ifndef __IAudioLoudness_FWD_DEFINED__
#define __IAudioLoudness_FWD_DEFINED__
typedef interface IAudioLoudness IAudioLoudness;
#endif 	 
#ifndef __IAudioInputSelector_FWD_DEFINED__
#define __IAudioInputSelector_FWD_DEFINED__
typedef interface IAudioInputSelector IAudioInputSelector;
#endif 	 
#ifndef __IAudioOutputSelector_FWD_DEFINED__
#define __IAudioOutputSelector_FWD_DEFINED__
typedef interface IAudioOutputSelector IAudioOutputSelector;
#endif 	 
#ifndef __IAudioMute_FWD_DEFINED__
#define __IAudioMute_FWD_DEFINED__
typedef interface IAudioMute IAudioMute;
#endif 	 
#ifndef __IAudioBass_FWD_DEFINED__
#define __IAudioBass_FWD_DEFINED__
typedef interface IAudioBass IAudioBass;
#endif 	 
#ifndef __IAudioMidrange_FWD_DEFINED__
#define __IAudioMidrange_FWD_DEFINED__
typedef interface IAudioMidrange IAudioMidrange;
#endif 	 
#ifndef __IAudioTreble_FWD_DEFINED__
#define __IAudioTreble_FWD_DEFINED__
typedef interface IAudioTreble IAudioTreble;
#endif 	 
#ifndef __IAudioAutoGainControl_FWD_DEFINED__
#define __IAudioAutoGainControl_FWD_DEFINED__
typedef interface IAudioAutoGainControl IAudioAutoGainControl;
#endif 	 
#ifndef __IAudioPeakMeter_FWD_DEFINED__
#define __IAudioPeakMeter_FWD_DEFINED__
typedef interface IAudioPeakMeter IAudioPeakMeter;
#endif 	 
#ifndef __IDeviceSpecificProperty_FWD_DEFINED__
#define __IDeviceSpecificProperty_FWD_DEFINED__
typedef interface IDeviceSpecificProperty IDeviceSpecificProperty;
#endif 	 
#ifndef __IKsFormatSupport_FWD_DEFINED__
#define __IKsFormatSupport_FWD_DEFINED__
typedef interface IKsFormatSupport IKsFormatSupport;
#endif 	 
#ifndef __IKsJackDescription_FWD_DEFINED__
#define __IKsJackDescription_FWD_DEFINED__
typedef interface IKsJackDescription IKsJackDescription;
#endif 	 
#ifndef __IPartsList_FWD_DEFINED__
#define __IPartsList_FWD_DEFINED__
typedef interface IPartsList IPartsList;
#endif 	 
#ifndef __IPart_FWD_DEFINED__
#define __IPart_FWD_DEFINED__
typedef interface IPart IPart;
#endif 	 
#ifndef __IConnector_FWD_DEFINED__
#define __IConnector_FWD_DEFINED__
typedef interface IConnector IConnector;
#endif 	 
#ifndef __ISubunit_FWD_DEFINED__
#define __ISubunit_FWD_DEFINED__
typedef interface ISubunit ISubunit;
#endif 	 
#ifndef __IControlInterface_FWD_DEFINED__
#define __IControlInterface_FWD_DEFINED__
typedef interface IControlInterface IControlInterface;
#endif 	 
#ifndef __IControlChangeNotify_FWD_DEFINED__
#define __IControlChangeNotify_FWD_DEFINED__
typedef interface IControlChangeNotify IControlChangeNotify;
#endif 	 
#ifndef __IDeviceTopology_FWD_DEFINED__
#define __IDeviceTopology_FWD_DEFINED__
typedef interface IDeviceTopology IDeviceTopology;
#endif 	 
#ifndef __DeviceTopology_FWD_DEFINED__
#define __DeviceTopology_FWD_DEFINED__
#ifdef __cplusplus
typedef class DeviceTopology DeviceTopology;
#else
typedef struct DeviceTopology DeviceTopology;
#endif  
#endif 	 
#ifndef __IPartsList_FWD_DEFINED__
#define __IPartsList_FWD_DEFINED__
typedef interface IPartsList IPartsList;
#endif 	 
#ifndef __IPerChannelDbLevel_FWD_DEFINED__
#define __IPerChannelDbLevel_FWD_DEFINED__
typedef interface IPerChannelDbLevel IPerChannelDbLevel;
#endif 	 
#ifndef __IAudioVolumeLevel_FWD_DEFINED__
#define __IAudioVolumeLevel_FWD_DEFINED__
typedef interface IAudioVolumeLevel IAudioVolumeLevel;
#endif 	 
#ifndef __IAudioLoudness_FWD_DEFINED__
#define __IAudioLoudness_FWD_DEFINED__
typedef interface IAudioLoudness IAudioLoudness;
#endif 	 
#ifndef __IAudioInputSelector_FWD_DEFINED__
#define __IAudioInputSelector_FWD_DEFINED__
typedef interface IAudioInputSelector IAudioInputSelector;
#endif 	 
#ifndef __IAudioMute_FWD_DEFINED__
#define __IAudioMute_FWD_DEFINED__
typedef interface IAudioMute IAudioMute;
#endif 	 
#ifndef __IAudioBass_FWD_DEFINED__
#define __IAudioBass_FWD_DEFINED__
typedef interface IAudioBass IAudioBass;
#endif 	 
#ifndef __IAudioMidrange_FWD_DEFINED__
#define __IAudioMidrange_FWD_DEFINED__
typedef interface IAudioMidrange IAudioMidrange;
#endif 	 
#ifndef __IAudioTreble_FWD_DEFINED__
#define __IAudioTreble_FWD_DEFINED__
typedef interface IAudioTreble IAudioTreble;
#endif 	 
#ifndef __IAudioAutoGainControl_FWD_DEFINED__
#define __IAudioAutoGainControl_FWD_DEFINED__
typedef interface IAudioAutoGainControl IAudioAutoGainControl;
#endif 	 
#ifndef __IAudioOutputSelector_FWD_DEFINED__
#define __IAudioOutputSelector_FWD_DEFINED__
typedef interface IAudioOutputSelector IAudioOutputSelector;
#endif 	 
#ifndef __IAudioPeakMeter_FWD_DEFINED__
#define __IAudioPeakMeter_FWD_DEFINED__
typedef interface IAudioPeakMeter IAudioPeakMeter;
#endif 	 
#ifndef __IDeviceSpecificProperty_FWD_DEFINED__
#define __IDeviceSpecificProperty_FWD_DEFINED__
typedef interface IDeviceSpecificProperty IDeviceSpecificProperty;
#endif 	 
#ifndef __IKsFormatSupport_FWD_DEFINED__
#define __IKsFormatSupport_FWD_DEFINED__
typedef interface IKsFormatSupport IKsFormatSupport;
#endif 	 
#include "oaidl.h"
#include "ocidl.h"
#include "propidl.h"
#ifdef __cplusplus
extern "C"{
#endif 
#define E_NOTFOUND HRESULT_FROM_WIN32(ERROR_NOT_FOUND)
#define DEVTOPO_HARDWARE_INITIATED_EVENTCONTEXT 'draH'
DEFINE_GUID(EVENTCONTEXT_VOLUMESLIDER, 0xE2C2E9DE,0x09B1,0x4B04,0x84, 0xE5, 0x07, 0x93, 0x12, 0x25, 0xEE, 0x04);
#define _IKsControl_
#include "ks.h"
#include "ksmedia.h"
#ifndef _KS_
typedef   struct __MIDL___MIDL_itf_devicetopology_0000_0000_0001
{
ULONG FormatSize;
ULONG Flags;
ULONG SampleSize;
ULONG Reserved;
GUID MajorFormat;
GUID SubFormat;
GUID Specifier;
} 	KSDATAFORMAT;
typedef struct __MIDL___MIDL_itf_devicetopology_0000_0000_0001 *PKSDATAFORMAT;
typedef   struct __MIDL___MIDL_itf_devicetopology_0000_0000_0002
{
union 
{
struct 
{
GUID Set;
ULONG Id;
ULONG Flags;
} 	;
LONGLONG Alignment;
} 	;
} 	KSIDENTIFIER;
typedef struct __MIDL___MIDL_itf_devicetopology_0000_0000_0002 *PKSIDENTIFIER;
typedef   
enum __MIDL___MIDL_itf_devicetopology_0000_0000_0005
{	ePcxChanMap_FL_FR	= 0,
	ePcxChanMap_FC_LFE	= ( ePcxChanMap_FL_FR + 1 ) ,
	ePcxChanMap_BL_BR	= ( ePcxChanMap_FC_LFE + 1 ) ,
	ePcxChanMap_FLC_FRC	= ( ePcxChanMap_BL_BR + 1 ) ,
	ePcxChanMap_SL_SR	= ( ePcxChanMap_FLC_FRC + 1 ) ,
	ePcxChanMap_Unknown	= ( ePcxChanMap_SL_SR + 1 ) 
} 	EChannelMapping;
typedef   
enum __MIDL___MIDL_itf_devicetopology_0000_0000_0006
{	eConnTypeUnknown	= 0,
	eConnTypeEighth	= ( eConnTypeUnknown + 1 ) ,
	eConnTypeQuarter	= ( eConnTypeEighth + 1 ) ,
	eConnTypeAtapiInternal	= ( eConnTypeQuarter + 1 ) ,
	eConnTypeRCA	= ( eConnTypeAtapiInternal + 1 ) ,
	eConnTypeOptical	= ( eConnTypeRCA + 1 ) ,
	eConnTypeOtherDigital	= ( eConnTypeOptical + 1 ) ,
	eConnTypeOtherAnalog	= ( eConnTypeOtherDigital + 1 ) ,
	eConnTypeMultichannelAnalogDIN	= ( eConnTypeOtherAnalog + 1 ) ,
	eConnTypeXlrProfessional	= ( eConnTypeMultichannelAnalogDIN + 1 ) ,
	eConnTypeRJ11Modem	= ( eConnTypeXlrProfessional + 1 ) ,
	eConnTypeCombination	= ( eConnTypeRJ11Modem + 1 ) 
} 	EPcxConnectionType;
typedef   
enum __MIDL___MIDL_itf_devicetopology_0000_0000_0007
{	eGeoLocRear	= 0x1,
	eGeoLocFront	= ( eGeoLocRear + 1 ) ,
	eGeoLocLeft	= ( eGeoLocFront + 1 ) ,
	eGeoLocRight	= ( eGeoLocLeft + 1 ) ,
	eGeoLocTop	= ( eGeoLocRight + 1 ) ,
	eGeoLocBottom	= ( eGeoLocTop + 1 ) ,
	eGeoLocRearOPanel	= ( eGeoLocBottom + 1 ) ,
	eGeoLocRiser	= ( eGeoLocRearOPanel + 1 ) ,
	eGeoLocInsideMobileLid	= ( eGeoLocRiser + 1 ) ,
	eGeoLocDrivebay	= ( eGeoLocInsideMobileLid + 1 ) ,
	eGeoLocHDMI	= ( eGeoLocDrivebay + 1 ) ,
	eGeoLocOutsideMobileLid	= ( eGeoLocHDMI + 1 ) ,
	eGeoLocATAPI	= ( eGeoLocOutsideMobileLid + 1 ) ,
	eGeoLocReserved5	= ( eGeoLocATAPI + 1 ) ,
	eGeoLocReserved6	= ( eGeoLocReserved5 + 1 ) 
} 	EPcxGeoLocation;
typedef   
enum __MIDL___MIDL_itf_devicetopology_0000_0000_0008
{	eGenLocPrimaryBox	= 0,
	eGenLocInternal	= ( eGenLocPrimaryBox + 1 ) ,
	eGenLocSeperate	= ( eGenLocInternal + 1 ) ,
	eGenLocOther	= ( eGenLocSeperate + 1 ) 
} 	EPcxGenLocation;
typedef   
enum __MIDL___MIDL_itf_devicetopology_0000_0000_0009
{	ePortConnJack	= 0,
	ePortConnIntegratedDevice	= ( ePortConnJack + 1 ) ,
	ePortConnBothIntegratedAndJack	= ( ePortConnIntegratedDevice + 1 ) ,
	ePortConnUnknown	= ( ePortConnBothIntegratedAndJack + 1 ) 
} 	EPxcPortConnection;
typedef   struct __MIDL___MIDL_itf_devicetopology_0000_0000_0010
{
EChannelMapping ChannelMapping;
COLORREF Color;
EPcxConnectionType ConnectionType;
EPcxGeoLocation GeoLocation;
EPcxGenLocation GenLocation;
EPxcPortConnection PortConnection;
BOOL IsConnected;
} 	KSJACK_DESCRIPTION;
typedef struct __MIDL___MIDL_itf_devicetopology_0000_0000_0010 *PKSJACK_DESCRIPTION;
typedef KSIDENTIFIER KSPROPERTY;
typedef KSIDENTIFIER *PKSPROPERTY;
typedef KSIDENTIFIER KSMETHOD;
typedef KSIDENTIFIER *PKSMETHOD;
typedef KSIDENTIFIER KSEVENT;
typedef KSIDENTIFIER *PKSEVENT;
#endif
typedef   
enum __MIDL___MIDL_itf_devicetopology_0000_0000_0011
{	In	= 0,
	Out	= ( In + 1 ) 
} 	DataFlow;
typedef   
enum __MIDL___MIDL_itf_devicetopology_0000_0000_0012
{	Connector	= 0,
	Subunit	= ( Connector + 1 ) 
} 	PartType;
#define PARTTYPE_FLAG_CONNECTOR 0x00010000
#define PARTTYPE_FLAG_SUBUNIT   0x00020000
#define PARTTYPE_MASK           0x00030000
#define PARTID_MASK             0x0000ffff
typedef   
enum __MIDL___MIDL_itf_devicetopology_0000_0000_0013
{	Unknown_Connector	= 0,
	Physical_Internal	= ( Unknown_Connector + 1 ) ,
	Physical_External	= ( Physical_Internal + 1 ) ,
	Software_IO	= ( Physical_External + 1 ) ,
	Software_Fixed	= ( Software_IO + 1 ) ,
	Network	= ( Software_Fixed + 1 ) 
} 	ConnectorType;
extern RPC_IF_HANDLE __MIDL_itf_devicetopology_0000_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_devicetopology_0000_0000_v0_0_s_ifspec;
#ifndef __IKsControl_INTERFACE_DEFINED__
#define __IKsControl_INTERFACE_DEFINED__
EXTERN_C const IID IID_IKsControl;
#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("28F54685-06FD-11D2-B27A-00A0C9223196")
IKsControl : public IUnknown
{
public:
virtual HRESULT STDMETHODCALLTYPE KsProperty( 
PKSPROPERTY Property,
ULONG PropertyLength,
void *PropertyData,
ULONG DataLength,
ULONG *BytesReturned) = 0;
virtual HRESULT STDMETHODCALLTYPE KsMethod( 
PKSMETHOD Method,
ULONG MethodLength,
void *MethodData,
ULONG DataLength,
ULONG *BytesReturned) = 0;
virtual HRESULT STDMETHODCALLTYPE KsEvent( 
PKSEVENT Event,
ULONG EventLength,
void *EventData,
ULONG DataLength,
ULONG *BytesReturned) = 0;
};
#else 	 
typedef struct IKsControlVtbl
{
BEGIN_INTERFACE
HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
IKsControl * This,
REFIID riid,
__RPC__deref_out  void **ppvObject);
ULONG ( STDMETHODCALLTYPE *AddRef )( 
IKsControl * This);
ULONG ( STDMETHODCALLTYPE *Release )( 
IKsControl * This);
HRESULT ( STDMETHODCALLTYPE *KsProperty )( 
IKsControl * This,
PKSPROPERTY Property,
ULONG PropertyLength,
void *PropertyData,
ULONG DataLength,
ULONG *BytesReturned);
HRESULT ( STDMETHODCALLTYPE *KsMethod )( 
IKsControl * This,
PKSMETHOD Method,
ULONG MethodLength,
void *MethodData,
ULONG DataLength,
ULONG *BytesReturned);
HRESULT ( STDMETHODCALLTYPE *KsEvent )( 
IKsControl * This,
PKSEVENT Event,
ULONG EventLength,
void *EventData,
ULONG DataLength,
ULONG *BytesReturned);
END_INTERFACE
} IKsControlVtbl;
interface IKsControl
{
CONST_VTBL struct IKsControlVtbl *lpVtbl;
};
#ifdef COBJMACROS
#define IKsControl_QueryInterface(This,riid,ppvObject)	\
( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define IKsControl_AddRef(This)	\
( (This)->lpVtbl -> AddRef(This) ) 
#define IKsControl_Release(This)	\
( (This)->lpVtbl -> Release(This) ) 
#define IKsControl_KsProperty(This,Property,PropertyLength,PropertyData,DataLength,BytesReturned)	\
( (This)->lpVtbl -> KsProperty(This,Property,PropertyLength,PropertyData,DataLength,BytesReturned) ) 
#define IKsControl_KsMethod(This,Method,MethodLength,MethodData,DataLength,BytesReturned)	\
( (This)->lpVtbl -> KsMethod(This,Method,MethodLength,MethodData,DataLength,BytesReturned) ) 
#define IKsControl_KsEvent(This,Event,EventLength,EventData,DataLength,BytesReturned)	\
( (This)->lpVtbl -> KsEvent(This,Event,EventLength,EventData,DataLength,BytesReturned) ) 
#endif  
#endif 	 
#endif 	 
#ifndef __IPerChannelDbLevel_INTERFACE_DEFINED__
#define __IPerChannelDbLevel_INTERFACE_DEFINED__
EXTERN_C const IID IID_IPerChannelDbLevel;
#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("C2F8E001-F205-4BC9-99BC-C13B1E048CCB")
IPerChannelDbLevel : public IUnknown
{
public:
virtual   HRESULT STDMETHODCALLTYPE GetChannelCount( 
__out  UINT *pcChannels) = 0;
virtual   HRESULT STDMETHODCALLTYPE GetLevelRange( 
__in  UINT nChannel,
__out  float *pfMinLevelDB,
__out  float *pfMaxLevelDB,
__out  float *pfStepping) = 0;
virtual   HRESULT STDMETHODCALLTYPE GetLevel( 
__in  UINT nChannel,
__out  float *pfLevelDB) = 0;
virtual   HRESULT STDMETHODCALLTYPE SetLevel( 
__in  UINT nChannel,
__in  float fLevelDB,
__in_opt  LPCGUID pguidEventContext) = 0;
virtual   HRESULT STDMETHODCALLTYPE SetLevelUniform( 
__in  float fLevelDB,
__in_opt  LPCGUID pguidEventContext) = 0;
virtual   HRESULT STDMETHODCALLTYPE SetLevelAllChannels( 
__in_ecount(cChannels)  float aLevelsDB[  ],
__in  ULONG cChannels,
__in_opt  LPCGUID pguidEventContext) = 0;
};
#else 	 
typedef struct IPerChannelDbLevelVtbl
{
BEGIN_INTERFACE
HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
IPerChannelDbLevel * This,
REFIID riid,
__RPC__deref_out  void **ppvObject);
ULONG ( STDMETHODCALLTYPE *AddRef )( 
IPerChannelDbLevel * This);
ULONG ( STDMETHODCALLTYPE *Release )( 
IPerChannelDbLevel * This);
HRESULT ( STDMETHODCALLTYPE *GetChannelCount )( 
IPerChannelDbLevel * This,
__out  UINT *pcChannels);
HRESULT ( STDMETHODCALLTYPE *GetLevelRange )( 
IPerChannelDbLevel * This,
__in  UINT nChannel,
__out  float *pfMinLevelDB,
__out  float *pfMaxLevelDB,
__out  float *pfStepping);
HRESULT ( STDMETHODCALLTYPE *GetLevel )( 
IPerChannelDbLevel * This,
__in  UINT nChannel,
__out  float *pfLevelDB);
HRESULT ( STDMETHODCALLTYPE *SetLevel )( 
IPerChannelDbLevel * This,
__in  UINT nChannel,
__in  float fLevelDB,
__in_opt  LPCGUID pguidEventContext);
HRESULT ( STDMETHODCALLTYPE *SetLevelUniform )( 
IPerChannelDbLevel * This,
__in  float fLevelDB,
__in_opt  LPCGUID pguidEventContext);
HRESULT ( STDMETHODCALLTYPE *SetLevelAllChannels )( 
IPerChannelDbLevel * This,
__in_ecount(cChannels)  float aLevelsDB[  ],
__in  ULONG cChannels,
__in_opt  LPCGUID pguidEventContext);
END_INTERFACE
} IPerChannelDbLevelVtbl;
interface IPerChannelDbLevel
{
CONST_VTBL struct IPerChannelDbLevelVtbl *lpVtbl;
};
#ifdef COBJMACROS
#define IPerChannelDbLevel_QueryInterface(This,riid,ppvObject)	\
( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define IPerChannelDbLevel_AddRef(This)	\
( (This)->lpVtbl -> AddRef(This) ) 
#define IPerChannelDbLevel_Release(This)	\
( (This)->lpVtbl -> Release(This) ) 
#define IPerChannelDbLevel_GetChannelCount(This,pcChannels)	\
( (This)->lpVtbl -> GetChannelCount(This,pcChannels) ) 
#define IPerChannelDbLevel_GetLevelRange(This,nChannel,pfMinLevelDB,pfMaxLevelDB,pfStepping)	\
( (This)->lpVtbl -> GetLevelRange(This,nChannel,pfMinLevelDB,pfMaxLevelDB,pfStepping) ) 
#define IPerChannelDbLevel_GetLevel(This,nChannel,pfLevelDB)	\
( (This)->lpVtbl -> GetLevel(This,nChannel,pfLevelDB) ) 
#define IPerChannelDbLevel_SetLevel(This,nChannel,fLevelDB,pguidEventContext)	\
( (This)->lpVtbl -> SetLevel(This,nChannel,fLevelDB,pguidEventContext) ) 
#define IPerChannelDbLevel_SetLevelUniform(This,fLevelDB,pguidEventContext)	\
( (This)->lpVtbl -> SetLevelUniform(This,fLevelDB,pguidEventContext) ) 
#define IPerChannelDbLevel_SetLevelAllChannels(This,aLevelsDB,cChannels,pguidEventContext)	\
( (This)->lpVtbl -> SetLevelAllChannels(This,aLevelsDB,cChannels,pguidEventContext) ) 
#endif  
#endif 	 
#endif 	 
#ifndef __IAudioVolumeLevel_INTERFACE_DEFINED__
#define __IAudioVolumeLevel_INTERFACE_DEFINED__
EXTERN_C const IID IID_IAudioVolumeLevel;
#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("7FB7B48F-531D-44A2-BCB3-5AD5A134B3DC")
IAudioVolumeLevel : public IPerChannelDbLevel
{
public:
};
#else 	 
typedef struct IAudioVolumeLevelVtbl
{
BEGIN_INTERFACE
HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
IAudioVolumeLevel * This,
REFIID riid,
__RPC__deref_out  void **ppvObject);
ULONG ( STDMETHODCALLTYPE *AddRef )( 
IAudioVolumeLevel * This);
ULONG ( STDMETHODCALLTYPE *Release )( 
IAudioVolumeLevel * This);
HRESULT ( STDMETHODCALLTYPE *GetChannelCount )( 
IAudioVolumeLevel * This,
__out  UINT *pcChannels);
HRESULT ( STDMETHODCALLTYPE *GetLevelRange )( 
IAudioVolumeLevel * This,
__in  UINT nChannel,
__out  float *pfMinLevelDB,
__out  float *pfMaxLevelDB,
__out  float *pfStepping);
HRESULT ( STDMETHODCALLTYPE *GetLevel )( 
IAudioVolumeLevel * This,
__in  UINT nChannel,
__out  float *pfLevelDB);
HRESULT ( STDMETHODCALLTYPE *SetLevel )( 
IAudioVolumeLevel * This,
__in  UINT nChannel,
__in  float fLevelDB,
__in_opt  LPCGUID pguidEventContext);
HRESULT ( STDMETHODCALLTYPE *SetLevelUniform )( 
IAudioVolumeLevel * This,
__in  float fLevelDB,
__in_opt  LPCGUID pguidEventContext);
HRESULT ( STDMETHODCALLTYPE *SetLevelAllChannels )( 
IAudioVolumeLevel * This,
__in_ecount(cChannels)  float aLevelsDB[  ],
__in  ULONG cChannels,
__in_opt  LPCGUID pguidEventContext);
END_INTERFACE
} IAudioVolumeLevelVtbl;
interface IAudioVolumeLevel
{
CONST_VTBL struct IAudioVolumeLevelVtbl *lpVtbl;
};
#ifdef COBJMACROS
#define IAudioVolumeLevel_QueryInterface(This,riid,ppvObject)	\
( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define IAudioVolumeLevel_AddRef(This)	\
( (This)->lpVtbl -> AddRef(This) ) 
#define IAudioVolumeLevel_Release(This)	\
( (This)->lpVtbl -> Release(This) ) 
#define IAudioVolumeLevel_GetChannelCount(This,pcChannels)	\
( (This)->lpVtbl -> GetChannelCount(This,pcChannels) ) 
#define IAudioVolumeLevel_GetLevelRange(This,nChannel,pfMinLevelDB,pfMaxLevelDB,pfStepping)	\
( (This)->lpVtbl -> GetLevelRange(This,nChannel,pfMinLevelDB,pfMaxLevelDB,pfStepping) ) 
#define IAudioVolumeLevel_GetLevel(This,nChannel,pfLevelDB)	\
( (This)->lpVtbl -> GetLevel(This,nChannel,pfLevelDB) ) 
#define IAudioVolumeLevel_SetLevel(This,nChannel,fLevelDB,pguidEventContext)	\
( (This)->lpVtbl -> SetLevel(This,nChannel,fLevelDB,pguidEventContext) ) 
#define IAudioVolumeLevel_SetLevelUniform(This,fLevelDB,pguidEventContext)	\
( (This)->lpVtbl -> SetLevelUniform(This,fLevelDB,pguidEventContext) ) 
#define IAudioVolumeLevel_SetLevelAllChannels(This,aLevelsDB,cChannels,pguidEventContext)	\
( (This)->lpVtbl -> SetLevelAllChannels(This,aLevelsDB,cChannels,pguidEventContext) ) 
#endif  
#endif 	 
#endif 	 
#ifndef __IAudioChannelConfig_INTERFACE_DEFINED__
#define __IAudioChannelConfig_INTERFACE_DEFINED__
EXTERN_C const IID IID_IAudioChannelConfig;
#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("BB11C46F-EC28-493C-B88A-5DB88062CE98")
IAudioChannelConfig : public IUnknown
{
public:
virtual   HRESULT STDMETHODCALLTYPE SetChannelConfig( 
DWORD dwConfig,
__in_opt  LPCGUID pguidEventContext) = 0;
virtual   HRESULT STDMETHODCALLTYPE GetChannelConfig( 
DWORD *pdwConfig) = 0;
};
#else 	 
typedef struct IAudioChannelConfigVtbl
{
BEGIN_INTERFACE
HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
IAudioChannelConfig * This,
REFIID riid,
__RPC__deref_out  void **ppvObject);
ULONG ( STDMETHODCALLTYPE *AddRef )( 
IAudioChannelConfig * This);
ULONG ( STDMETHODCALLTYPE *Release )( 
IAudioChannelConfig * This);
HRESULT ( STDMETHODCALLTYPE *SetChannelConfig )( 
IAudioChannelConfig * This,
DWORD dwConfig,
__in_opt  LPCGUID pguidEventContext);
HRESULT ( STDMETHODCALLTYPE *GetChannelConfig )( 
IAudioChannelConfig * This,
DWORD *pdwConfig);
END_INTERFACE
} IAudioChannelConfigVtbl;
interface IAudioChannelConfig
{
CONST_VTBL struct IAudioChannelConfigVtbl *lpVtbl;
};
#ifdef COBJMACROS
#define IAudioChannelConfig_QueryInterface(This,riid,ppvObject)	\
( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define IAudioChannelConfig_AddRef(This)	\
( (This)->lpVtbl -> AddRef(This) ) 
#define IAudioChannelConfig_Release(This)	\
( (This)->lpVtbl -> Release(This) ) 
#define IAudioChannelConfig_SetChannelConfig(This,dwConfig,pguidEventContext)	\
( (This)->lpVtbl -> SetChannelConfig(This,dwConfig,pguidEventContext) ) 
#define IAudioChannelConfig_GetChannelConfig(This,pdwConfig)	\
( (This)->lpVtbl -> GetChannelConfig(This,pdwConfig) ) 
#endif  
#endif 	 
#endif 	 
#ifndef __IAudioLoudness_INTERFACE_DEFINED__
#define __IAudioLoudness_INTERFACE_DEFINED__
EXTERN_C const IID IID_IAudioLoudness;
#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("7D8B1437-DD53-4350-9C1B-1EE2890BD938")
IAudioLoudness : public IUnknown
{
public:
virtual   HRESULT STDMETHODCALLTYPE GetEnabled( 
__out  BOOL *pbEnabled) = 0;
virtual   HRESULT STDMETHODCALLTYPE SetEnabled( 
__in  BOOL bEnable,
__in_opt  LPCGUID pguidEventContext) = 0;
};
#else 	 
typedef struct IAudioLoudnessVtbl
{
BEGIN_INTERFACE
HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
IAudioLoudness * This,
REFIID riid,
__RPC__deref_out  void **ppvObject);
ULONG ( STDMETHODCALLTYPE *AddRef )( 
IAudioLoudness * This);
ULONG ( STDMETHODCALLTYPE *Release )( 
IAudioLoudness * This);
HRESULT ( STDMETHODCALLTYPE *GetEnabled )( 
IAudioLoudness * This,
__out  BOOL *pbEnabled);
HRESULT ( STDMETHODCALLTYPE *SetEnabled )( 
IAudioLoudness * This,
__in  BOOL bEnable,
__in_opt  LPCGUID pguidEventContext);
END_INTERFACE
} IAudioLoudnessVtbl;
interface IAudioLoudness
{
CONST_VTBL struct IAudioLoudnessVtbl *lpVtbl;
};
#ifdef COBJMACROS
#define IAudioLoudness_QueryInterface(This,riid,ppvObject)	\
( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define IAudioLoudness_AddRef(This)	\
( (This)->lpVtbl -> AddRef(This) ) 
#define IAudioLoudness_Release(This)	\
( (This)->lpVtbl -> Release(This) ) 
#define IAudioLoudness_GetEnabled(This,pbEnabled)	\
( (This)->lpVtbl -> GetEnabled(This,pbEnabled) ) 
#define IAudioLoudness_SetEnabled(This,bEnable,pguidEventContext)	\
( (This)->lpVtbl -> SetEnabled(This,bEnable,pguidEventContext) ) 
#endif  
#endif 	 
#endif 	 
#ifndef __IAudioInputSelector_INTERFACE_DEFINED__
#define __IAudioInputSelector_INTERFACE_DEFINED__
EXTERN_C const IID IID_IAudioInputSelector;
#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("4F03DC02-5E6E-4653-8F72-A030C123D598")
IAudioInputSelector : public IUnknown
{
public:
virtual   HRESULT STDMETHODCALLTYPE GetSelection( 
__out  UINT *pnIdSelected) = 0;
virtual   HRESULT STDMETHODCALLTYPE SetSelection( 
__in  UINT nIdSelect,
__in_opt  LPCGUID pguidEventContext) = 0;
};
#else 	 
typedef struct IAudioInputSelectorVtbl
{
BEGIN_INTERFACE
HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
IAudioInputSelector * This,
REFIID riid,
__RPC__deref_out  void **ppvObject);
ULONG ( STDMETHODCALLTYPE *AddRef )( 
IAudioInputSelector * This);
ULONG ( STDMETHODCALLTYPE *Release )( 
IAudioInputSelector * This);
HRESULT ( STDMETHODCALLTYPE *GetSelection )( 
IAudioInputSelector * This,
__out  UINT *pnIdSelected);
HRESULT ( STDMETHODCALLTYPE *SetSelection )( 
IAudioInputSelector * This,
__in  UINT nIdSelect,
__in_opt  LPCGUID pguidEventContext);
END_INTERFACE
} IAudioInputSelectorVtbl;
interface IAudioInputSelector
{
CONST_VTBL struct IAudioInputSelectorVtbl *lpVtbl;
};
#ifdef COBJMACROS
#define IAudioInputSelector_QueryInterface(This,riid,ppvObject)	\
( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define IAudioInputSelector_AddRef(This)	\
( (This)->lpVtbl -> AddRef(This) ) 
#define IAudioInputSelector_Release(This)	\
( (This)->lpVtbl -> Release(This) ) 
#define IAudioInputSelector_GetSelection(This,pnIdSelected)	\
( (This)->lpVtbl -> GetSelection(This,pnIdSelected) ) 
#define IAudioInputSelector_SetSelection(This,nIdSelect,pguidEventContext)	\
( (This)->lpVtbl -> SetSelection(This,nIdSelect,pguidEventContext) ) 
#endif  
#endif 	 
#endif 	 
#ifndef __IAudioOutputSelector_INTERFACE_DEFINED__
#define __IAudioOutputSelector_INTERFACE_DEFINED__
EXTERN_C const IID IID_IAudioOutputSelector;
#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("BB515F69-94A7-429e-8B9C-271B3F11A3AB")
IAudioOutputSelector : public IUnknown
{
public:
virtual   HRESULT STDMETHODCALLTYPE GetSelection( 
__out  UINT *pnIdSelected) = 0;
virtual   HRESULT STDMETHODCALLTYPE SetSelection( 
__in  UINT nIdSelect,
__in_opt  LPCGUID pguidEventContext) = 0;
};
#else 	 
typedef struct IAudioOutputSelectorVtbl
{
BEGIN_INTERFACE
HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
IAudioOutputSelector * This,
REFIID riid,
__RPC__deref_out  void **ppvObject);
ULONG ( STDMETHODCALLTYPE *AddRef )( 
IAudioOutputSelector * This);
ULONG ( STDMETHODCALLTYPE *Release )( 
IAudioOutputSelector * This);
HRESULT ( STDMETHODCALLTYPE *GetSelection )( 
IAudioOutputSelector * This,
__out  UINT *pnIdSelected);
HRESULT ( STDMETHODCALLTYPE *SetSelection )( 
IAudioOutputSelector * This,
__in  UINT nIdSelect,
__in_opt  LPCGUID pguidEventContext);
END_INTERFACE
} IAudioOutputSelectorVtbl;
interface IAudioOutputSelector
{
CONST_VTBL struct IAudioOutputSelectorVtbl *lpVtbl;
};
#ifdef COBJMACROS
#define IAudioOutputSelector_QueryInterface(This,riid,ppvObject)	\
( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define IAudioOutputSelector_AddRef(This)	\
( (This)->lpVtbl -> AddRef(This) ) 
#define IAudioOutputSelector_Release(This)	\
( (This)->lpVtbl -> Release(This) ) 
#define IAudioOutputSelector_GetSelection(This,pnIdSelected)	\
( (This)->lpVtbl -> GetSelection(This,pnIdSelected) ) 
#define IAudioOutputSelector_SetSelection(This,nIdSelect,pguidEventContext)	\
( (This)->lpVtbl -> SetSelection(This,nIdSelect,pguidEventContext) ) 
#endif  
#endif 	 
#endif 	 
#ifndef __IAudioMute_INTERFACE_DEFINED__
#define __IAudioMute_INTERFACE_DEFINED__
EXTERN_C const IID IID_IAudioMute;
#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("DF45AEEA-B74A-4B6B-AFAD-2366B6AA012E")
IAudioMute : public IUnknown
{
public:
virtual   HRESULT STDMETHODCALLTYPE SetMute( 
__in  BOOL bMuted,
__in_opt  LPCGUID pguidEventContext) = 0;
virtual   HRESULT STDMETHODCALLTYPE GetMute( 
__out  BOOL *pbMuted) = 0;
};
#else 	 
typedef struct IAudioMuteVtbl
{
BEGIN_INTERFACE
HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
IAudioMute * This,
REFIID riid,
__RPC__deref_out  void **ppvObject);
ULONG ( STDMETHODCALLTYPE *AddRef )( 
IAudioMute * This);
ULONG ( STDMETHODCALLTYPE *Release )( 
IAudioMute * This);
HRESULT ( STDMETHODCALLTYPE *SetMute )( 
IAudioMute * This,
__in  BOOL bMuted,
__in_opt  LPCGUID pguidEventContext);
HRESULT ( STDMETHODCALLTYPE *GetMute )( 
IAudioMute * This,
__out  BOOL *pbMuted);
END_INTERFACE
} IAudioMuteVtbl;
interface IAudioMute
{
CONST_VTBL struct IAudioMuteVtbl *lpVtbl;
};
#ifdef COBJMACROS
#define IAudioMute_QueryInterface(This,riid,ppvObject)	\
( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define IAudioMute_AddRef(This)	\
( (This)->lpVtbl -> AddRef(This) ) 
#define IAudioMute_Release(This)	\
( (This)->lpVtbl -> Release(This) ) 
#define IAudioMute_SetMute(This,bMuted,pguidEventContext)	\
( (This)->lpVtbl -> SetMute(This,bMuted,pguidEventContext) ) 
#define IAudioMute_GetMute(This,pbMuted)	\
( (This)->lpVtbl -> GetMute(This,pbMuted) ) 
#endif  
#endif 	 
#endif 	 
#ifndef __IAudioBass_INTERFACE_DEFINED__
#define __IAudioBass_INTERFACE_DEFINED__
EXTERN_C const IID IID_IAudioBass;
#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("A2B1A1D9-4DB3-425D-A2B2-BD335CB3E2E5")
IAudioBass : public IPerChannelDbLevel
{
public:
};
#else 	 
typedef struct IAudioBassVtbl
{
BEGIN_INTERFACE
HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
IAudioBass * This,
REFIID riid,
__RPC__deref_out  void **ppvObject);
ULONG ( STDMETHODCALLTYPE *AddRef )( 
IAudioBass * This);
ULONG ( STDMETHODCALLTYPE *Release )( 
IAudioBass * This);
HRESULT ( STDMETHODCALLTYPE *GetChannelCount )( 
IAudioBass * This,
__out  UINT *pcChannels);
HRESULT ( STDMETHODCALLTYPE *GetLevelRange )( 
IAudioBass * This,
__in  UINT nChannel,
__out  float *pfMinLevelDB,
__out  float *pfMaxLevelDB,
__out  float *pfStepping);
HRESULT ( STDMETHODCALLTYPE *GetLevel )( 
IAudioBass * This,
__in  UINT nChannel,
__out  float *pfLevelDB);
HRESULT ( STDMETHODCALLTYPE *SetLevel )( 
IAudioBass * This,
__in  UINT nChannel,
__in  float fLevelDB,
__in_opt  LPCGUID pguidEventContext);
HRESULT ( STDMETHODCALLTYPE *SetLevelUniform )( 
IAudioBass * This,
__in  float fLevelDB,
__in_opt  LPCGUID pguidEventContext);
HRESULT ( STDMETHODCALLTYPE *SetLevelAllChannels )( 
IAudioBass * This,
__in_ecount(cChannels)  float aLevelsDB[  ],
__in  ULONG cChannels,
__in_opt  LPCGUID pguidEventContext);
END_INTERFACE
} IAudioBassVtbl;
interface IAudioBass
{
CONST_VTBL struct IAudioBassVtbl *lpVtbl;
};
#ifdef COBJMACROS
#define IAudioBass_QueryInterface(This,riid,ppvObject)	\
( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define IAudioBass_AddRef(This)	\
( (This)->lpVtbl -> AddRef(This) ) 
#define IAudioBass_Release(This)	\
( (This)->lpVtbl -> Release(This) ) 
#define IAudioBass_GetChannelCount(This,pcChannels)	\
( (This)->lpVtbl -> GetChannelCount(This,pcChannels) ) 
#define IAudioBass_GetLevelRange(This,nChannel,pfMinLevelDB,pfMaxLevelDB,pfStepping)	\
( (This)->lpVtbl -> GetLevelRange(This,nChannel,pfMinLevelDB,pfMaxLevelDB,pfStepping) ) 
#define IAudioBass_GetLevel(This,nChannel,pfLevelDB)	\
( (This)->lpVtbl -> GetLevel(This,nChannel,pfLevelDB) ) 
#define IAudioBass_SetLevel(This,nChannel,fLevelDB,pguidEventContext)	\
( (This)->lpVtbl -> SetLevel(This,nChannel,fLevelDB,pguidEventContext) ) 
#define IAudioBass_SetLevelUniform(This,fLevelDB,pguidEventContext)	\
( (This)->lpVtbl -> SetLevelUniform(This,fLevelDB,pguidEventContext) ) 
#define IAudioBass_SetLevelAllChannels(This,aLevelsDB,cChannels,pguidEventContext)	\
( (This)->lpVtbl -> SetLevelAllChannels(This,aLevelsDB,cChannels,pguidEventContext) ) 
#endif  
#endif 	 
#endif 	 
#ifndef __IAudioMidrange_INTERFACE_DEFINED__
#define __IAudioMidrange_INTERFACE_DEFINED__
EXTERN_C const IID IID_IAudioMidrange;
#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("5E54B6D7-B44B-40D9-9A9E-E691D9CE6EDF")
IAudioMidrange : public IPerChannelDbLevel
{
public:
};
#else 	 
typedef struct IAudioMidrangeVtbl
{
BEGIN_INTERFACE
HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
IAudioMidrange * This,
REFIID riid,
__RPC__deref_out  void **ppvObject);
ULONG ( STDMETHODCALLTYPE *AddRef )( 
IAudioMidrange * This);
ULONG ( STDMETHODCALLTYPE *Release )( 
IAudioMidrange * This);
HRESULT ( STDMETHODCALLTYPE *GetChannelCount )( 
IAudioMidrange * This,
__out  UINT *pcChannels);
HRESULT ( STDMETHODCALLTYPE *GetLevelRange )( 
IAudioMidrange * This,
__in  UINT nChannel,
__out  float *pfMinLevelDB,
__out  float *pfMaxLevelDB,
__out  float *pfStepping);
HRESULT ( STDMETHODCALLTYPE *GetLevel )( 
IAudioMidrange * This,
__in  UINT nChannel,
__out  float *pfLevelDB);
HRESULT ( STDMETHODCALLTYPE *SetLevel )( 
IAudioMidrange * This,
__in  UINT nChannel,
__in  float fLevelDB,
__in_opt  LPCGUID pguidEventContext);
HRESULT ( STDMETHODCALLTYPE *SetLevelUniform )( 
IAudioMidrange * This,
__in  float fLevelDB,
__in_opt  LPCGUID pguidEventContext);
HRESULT ( STDMETHODCALLTYPE *SetLevelAllChannels )( 
IAudioMidrange * This,
__in_ecount(cChannels)  float aLevelsDB[  ],
__in  ULONG cChannels,
__in_opt  LPCGUID pguidEventContext);
END_INTERFACE
} IAudioMidrangeVtbl;
interface IAudioMidrange
{
CONST_VTBL struct IAudioMidrangeVtbl *lpVtbl;
};
#ifdef COBJMACROS
#define IAudioMidrange_QueryInterface(This,riid,ppvObject)	\
( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define IAudioMidrange_AddRef(This)	\
( (This)->lpVtbl -> AddRef(This) ) 
#define IAudioMidrange_Release(This)	\
( (This)->lpVtbl -> Release(This) ) 
#define IAudioMidrange_GetChannelCount(This,pcChannels)	\
( (This)->lpVtbl -> GetChannelCount(This,pcChannels) ) 
#define IAudioMidrange_GetLevelRange(This,nChannel,pfMinLevelDB,pfMaxLevelDB,pfStepping)	\
( (This)->lpVtbl -> GetLevelRange(This,nChannel,pfMinLevelDB,pfMaxLevelDB,pfStepping) ) 
#define IAudioMidrange_GetLevel(This,nChannel,pfLevelDB)	\
( (This)->lpVtbl -> GetLevel(This,nChannel,pfLevelDB) ) 
#define IAudioMidrange_SetLevel(This,nChannel,fLevelDB,pguidEventContext)	\
( (This)->lpVtbl -> SetLevel(This,nChannel,fLevelDB,pguidEventContext) ) 
#define IAudioMidrange_SetLevelUniform(This,fLevelDB,pguidEventContext)	\
( (This)->lpVtbl -> SetLevelUniform(This,fLevelDB,pguidEventContext) ) 
#define IAudioMidrange_SetLevelAllChannels(This,aLevelsDB,cChannels,pguidEventContext)	\
( (This)->lpVtbl -> SetLevelAllChannels(This,aLevelsDB,cChannels,pguidEventContext) ) 
#endif  
#endif 	 
#endif 	 
#ifndef __IAudioTreble_INTERFACE_DEFINED__
#define __IAudioTreble_INTERFACE_DEFINED__
EXTERN_C const IID IID_IAudioTreble;
#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("0A717812-694E-4907-B74B-BAFA5CFDCA7B")
IAudioTreble : public IPerChannelDbLevel
{
public:
};
#else 	 
typedef struct IAudioTrebleVtbl
{
BEGIN_INTERFACE
HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
IAudioTreble * This,
REFIID riid,
__RPC__deref_out  void **ppvObject);
ULONG ( STDMETHODCALLTYPE *AddRef )( 
IAudioTreble * This);
ULONG ( STDMETHODCALLTYPE *Release )( 
IAudioTreble * This);
HRESULT ( STDMETHODCALLTYPE *GetChannelCount )( 
IAudioTreble * This,
__out  UINT *pcChannels);
HRESULT ( STDMETHODCALLTYPE *GetLevelRange )( 
IAudioTreble * This,
__in  UINT nChannel,
__out  float *pfMinLevelDB,
__out  float *pfMaxLevelDB,
__out  float *pfStepping);
HRESULT ( STDMETHODCALLTYPE *GetLevel )( 
IAudioTreble * This,
__in  UINT nChannel,
__out  float *pfLevelDB);
HRESULT ( STDMETHODCALLTYPE *SetLevel )( 
IAudioTreble * This,
__in  UINT nChannel,
__in  float fLevelDB,
__in_opt  LPCGUID pguidEventContext);
HRESULT ( STDMETHODCALLTYPE *SetLevelUniform )( 
IAudioTreble * This,
__in  float fLevelDB,
__in_opt  LPCGUID pguidEventContext);
HRESULT ( STDMETHODCALLTYPE *SetLevelAllChannels )( 
IAudioTreble * This,
__in_ecount(cChannels)  float aLevelsDB[  ],
__in  ULONG cChannels,
__in_opt  LPCGUID pguidEventContext);
END_INTERFACE
} IAudioTrebleVtbl;
interface IAudioTreble
{
CONST_VTBL struct IAudioTrebleVtbl *lpVtbl;
};
#ifdef COBJMACROS
#define IAudioTreble_QueryInterface(This,riid,ppvObject)	\
( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define IAudioTreble_AddRef(This)	\
( (This)->lpVtbl -> AddRef(This) ) 
#define IAudioTreble_Release(This)	\
( (This)->lpVtbl -> Release(This) ) 
#define IAudioTreble_GetChannelCount(This,pcChannels)	\
( (This)->lpVtbl -> GetChannelCount(This,pcChannels) ) 
#define IAudioTreble_GetLevelRange(This,nChannel,pfMinLevelDB,pfMaxLevelDB,pfStepping)	\
( (This)->lpVtbl -> GetLevelRange(This,nChannel,pfMinLevelDB,pfMaxLevelDB,pfStepping) ) 
#define IAudioTreble_GetLevel(This,nChannel,pfLevelDB)	\
( (This)->lpVtbl -> GetLevel(This,nChannel,pfLevelDB) ) 
#define IAudioTreble_SetLevel(This,nChannel,fLevelDB,pguidEventContext)	\
( (This)->lpVtbl -> SetLevel(This,nChannel,fLevelDB,pguidEventContext) ) 
#define IAudioTreble_SetLevelUniform(This,fLevelDB,pguidEventContext)	\
( (This)->lpVtbl -> SetLevelUniform(This,fLevelDB,pguidEventContext) ) 
#define IAudioTreble_SetLevelAllChannels(This,aLevelsDB,cChannels,pguidEventContext)	\
( (This)->lpVtbl -> SetLevelAllChannels(This,aLevelsDB,cChannels,pguidEventContext) ) 
#endif  
#endif 	 
#endif 	 
#ifndef __IAudioAutoGainControl_INTERFACE_DEFINED__
#define __IAudioAutoGainControl_INTERFACE_DEFINED__
EXTERN_C const IID IID_IAudioAutoGainControl;
#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("85401FD4-6DE4-4b9d-9869-2D6753A82F3C")
IAudioAutoGainControl : public IUnknown
{
public:
virtual   HRESULT STDMETHODCALLTYPE GetEnabled( 
__out  BOOL *pbEnabled) = 0;
virtual   HRESULT STDMETHODCALLTYPE SetEnabled( 
__in  BOOL bEnable,
__in_opt  LPCGUID pguidEventContext) = 0;
};
#else 	 
typedef struct IAudioAutoGainControlVtbl
{
BEGIN_INTERFACE
HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
IAudioAutoGainControl * This,
REFIID riid,
__RPC__deref_out  void **ppvObject);
ULONG ( STDMETHODCALLTYPE *AddRef )( 
IAudioAutoGainControl * This);
ULONG ( STDMETHODCALLTYPE *Release )( 
IAudioAutoGainControl * This);
HRESULT ( STDMETHODCALLTYPE *GetEnabled )( 
IAudioAutoGainControl * This,
__out  BOOL *pbEnabled);
HRESULT ( STDMETHODCALLTYPE *SetEnabled )( 
IAudioAutoGainControl * This,
__in  BOOL bEnable,
__in_opt  LPCGUID pguidEventContext);
END_INTERFACE
} IAudioAutoGainControlVtbl;
interface IAudioAutoGainControl
{
CONST_VTBL struct IAudioAutoGainControlVtbl *lpVtbl;
};
#ifdef COBJMACROS
#define IAudioAutoGainControl_QueryInterface(This,riid,ppvObject)	\
( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define IAudioAutoGainControl_AddRef(This)	\
( (This)->lpVtbl -> AddRef(This) ) 
#define IAudioAutoGainControl_Release(This)	\
( (This)->lpVtbl -> Release(This) ) 
#define IAudioAutoGainControl_GetEnabled(This,pbEnabled)	\
( (This)->lpVtbl -> GetEnabled(This,pbEnabled) ) 
#define IAudioAutoGainControl_SetEnabled(This,bEnable,pguidEventContext)	\
( (This)->lpVtbl -> SetEnabled(This,bEnable,pguidEventContext) ) 
#endif  
#endif 	 
#endif 	 
#ifndef __IAudioPeakMeter_INTERFACE_DEFINED__
#define __IAudioPeakMeter_INTERFACE_DEFINED__
EXTERN_C const IID IID_IAudioPeakMeter;
#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("DD79923C-0599-45e0-B8B6-C8DF7DB6E796")
IAudioPeakMeter : public IUnknown
{
public:
virtual   HRESULT STDMETHODCALLTYPE GetChannelCount( 
__out  UINT *pcChannels) = 0;
virtual   HRESULT STDMETHODCALLTYPE GetLevel( 
__in  UINT nChannel,
__out  float *pfLevel) = 0;
};
#else 	 
typedef struct IAudioPeakMeterVtbl
{
BEGIN_INTERFACE
HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
IAudioPeakMeter * This,
REFIID riid,
__RPC__deref_out  void **ppvObject);
ULONG ( STDMETHODCALLTYPE *AddRef )( 
IAudioPeakMeter * This);
ULONG ( STDMETHODCALLTYPE *Release )( 
IAudioPeakMeter * This);
HRESULT ( STDMETHODCALLTYPE *GetChannelCount )( 
IAudioPeakMeter * This,
__out  UINT *pcChannels);
HRESULT ( STDMETHODCALLTYPE *GetLevel )( 
IAudioPeakMeter * This,
__in  UINT nChannel,
__out  float *pfLevel);
END_INTERFACE
} IAudioPeakMeterVtbl;
interface IAudioPeakMeter
{
CONST_VTBL struct IAudioPeakMeterVtbl *lpVtbl;
};
#ifdef COBJMACROS
#define IAudioPeakMeter_QueryInterface(This,riid,ppvObject)	\
( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define IAudioPeakMeter_AddRef(This)	\
( (This)->lpVtbl -> AddRef(This) ) 
#define IAudioPeakMeter_Release(This)	\
( (This)->lpVtbl -> Release(This) ) 
#define IAudioPeakMeter_GetChannelCount(This,pcChannels)	\
( (This)->lpVtbl -> GetChannelCount(This,pcChannels) ) 
#define IAudioPeakMeter_GetLevel(This,nChannel,pfLevel)	\
( (This)->lpVtbl -> GetLevel(This,nChannel,pfLevel) ) 
#endif  
#endif 	 
#endif 	 
#ifndef __IDeviceSpecificProperty_INTERFACE_DEFINED__
#define __IDeviceSpecificProperty_INTERFACE_DEFINED__
EXTERN_C const IID IID_IDeviceSpecificProperty;
#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("3B22BCBF-2586-4af0-8583-205D391B807C")
IDeviceSpecificProperty : public IUnknown
{
public:
virtual   HRESULT STDMETHODCALLTYPE GetType( 
__deref_out  VARTYPE *pVType) = 0;
virtual   HRESULT STDMETHODCALLTYPE GetValue( 
__out  void *pvValue,
__inout  DWORD *pcbValue) = 0;
virtual   HRESULT STDMETHODCALLTYPE SetValue( 
__in  void *pvValue,
DWORD cbValue,
__in_opt  LPCGUID pguidEventContext) = 0;
virtual   HRESULT STDMETHODCALLTYPE Get4BRange( 
__deref_out  LONG *plMin,
__deref_out  LONG *plMax,
__deref_out  LONG *plStepping) = 0;
};
#else 	 
typedef struct IDeviceSpecificPropertyVtbl
{
BEGIN_INTERFACE
HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
IDeviceSpecificProperty * This,
REFIID riid,
__RPC__deref_out  void **ppvObject);
ULONG ( STDMETHODCALLTYPE *AddRef )( 
IDeviceSpecificProperty * This);
ULONG ( STDMETHODCALLTYPE *Release )( 
IDeviceSpecificProperty * This);
HRESULT ( STDMETHODCALLTYPE *GetType )( 
IDeviceSpecificProperty * This,
__deref_out  VARTYPE *pVType);
HRESULT ( STDMETHODCALLTYPE *GetValue )( 
IDeviceSpecificProperty * This,
__out  void *pvValue,
__inout  DWORD *pcbValue);
HRESULT ( STDMETHODCALLTYPE *SetValue )( 
IDeviceSpecificProperty * This,
__in  void *pvValue,
DWORD cbValue,
__in_opt  LPCGUID pguidEventContext);
HRESULT ( STDMETHODCALLTYPE *Get4BRange )( 
IDeviceSpecificProperty * This,
__deref_out  LONG *plMin,
__deref_out  LONG *plMax,
__deref_out  LONG *plStepping);
END_INTERFACE
} IDeviceSpecificPropertyVtbl;
interface IDeviceSpecificProperty
{
CONST_VTBL struct IDeviceSpecificPropertyVtbl *lpVtbl;
};
#ifdef COBJMACROS
#define IDeviceSpecificProperty_QueryInterface(This,riid,ppvObject)	\
( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define IDeviceSpecificProperty_AddRef(This)	\
( (This)->lpVtbl -> AddRef(This) ) 
#define IDeviceSpecificProperty_Release(This)	\
( (This)->lpVtbl -> Release(This) ) 
#define IDeviceSpecificProperty_GetType(This,pVType)	\
( (This)->lpVtbl -> GetType(This,pVType) ) 
#define IDeviceSpecificProperty_GetValue(This,pvValue,pcbValue)	\
( (This)->lpVtbl -> GetValue(This,pvValue,pcbValue) ) 
#define IDeviceSpecificProperty_SetValue(This,pvValue,cbValue,pguidEventContext)	\
( (This)->lpVtbl -> SetValue(This,pvValue,cbValue,pguidEventContext) ) 
#define IDeviceSpecificProperty_Get4BRange(This,plMin,plMax,plStepping)	\
( (This)->lpVtbl -> Get4BRange(This,plMin,plMax,plStepping) ) 
#endif  
#endif 	 
#endif 	 
#ifndef __IKsFormatSupport_INTERFACE_DEFINED__
#define __IKsFormatSupport_INTERFACE_DEFINED__
EXTERN_C const IID IID_IKsFormatSupport;
#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("3CB4A69D-BB6F-4D2B-95B7-452D2C155DB5")
IKsFormatSupport : public IUnknown
{
public:
virtual   HRESULT STDMETHODCALLTYPE IsFormatSupported( 
PKSDATAFORMAT pKsFormat,
__in  DWORD cbFormat,
__out  BOOL *pbSupported) = 0;
virtual   HRESULT STDMETHODCALLTYPE GetDevicePreferredFormat( 
PKSDATAFORMAT *ppKsFormat) = 0;
};
#else 	 
typedef struct IKsFormatSupportVtbl
{
BEGIN_INTERFACE
HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
IKsFormatSupport * This,
REFIID riid,
__RPC__deref_out  void **ppvObject);
ULONG ( STDMETHODCALLTYPE *AddRef )( 
IKsFormatSupport * This);
ULONG ( STDMETHODCALLTYPE *Release )( 
IKsFormatSupport * This);
HRESULT ( STDMETHODCALLTYPE *IsFormatSupported )( 
IKsFormatSupport * This,
PKSDATAFORMAT pKsFormat,
__in  DWORD cbFormat,
__out  BOOL *pbSupported);
HRESULT ( STDMETHODCALLTYPE *GetDevicePreferredFormat )( 
IKsFormatSupport * This,
PKSDATAFORMAT *ppKsFormat);
END_INTERFACE
} IKsFormatSupportVtbl;
interface IKsFormatSupport
{
CONST_VTBL struct IKsFormatSupportVtbl *lpVtbl;
};
#ifdef COBJMACROS
#define IKsFormatSupport_QueryInterface(This,riid,ppvObject)	\
( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define IKsFormatSupport_AddRef(This)	\
( (This)->lpVtbl -> AddRef(This) ) 
#define IKsFormatSupport_Release(This)	\
( (This)->lpVtbl -> Release(This) ) 
#define IKsFormatSupport_IsFormatSupported(This,pKsFormat,cbFormat,pbSupported)	\
( (This)->lpVtbl -> IsFormatSupported(This,pKsFormat,cbFormat,pbSupported) ) 
#define IKsFormatSupport_GetDevicePreferredFormat(This,ppKsFormat)	\
( (This)->lpVtbl -> GetDevicePreferredFormat(This,ppKsFormat) ) 
#endif  
#endif 	 
#endif 	 
#ifndef __IKsJackDescription_INTERFACE_DEFINED__
#define __IKsJackDescription_INTERFACE_DEFINED__
EXTERN_C const IID IID_IKsJackDescription;
#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("4509F757-2D46-4637-8E62-CE7DB944F57B")
IKsJackDescription : public IUnknown
{
public:
virtual   HRESULT STDMETHODCALLTYPE GetJackCount( 
__out  UINT *pcJacks) = 0;
virtual   HRESULT STDMETHODCALLTYPE GetJackDescription( 
UINT nJack,
__out  KSJACK_DESCRIPTION *pDescription) = 0;
};
#else 	 
typedef struct IKsJackDescriptionVtbl
{
BEGIN_INTERFACE
HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
IKsJackDescription * This,
REFIID riid,
__RPC__deref_out  void **ppvObject);
ULONG ( STDMETHODCALLTYPE *AddRef )( 
IKsJackDescription * This);
ULONG ( STDMETHODCALLTYPE *Release )( 
IKsJackDescription * This);
HRESULT ( STDMETHODCALLTYPE *GetJackCount )( 
IKsJackDescription * This,
__out  UINT *pcJacks);
HRESULT ( STDMETHODCALLTYPE *GetJackDescription )( 
IKsJackDescription * This,
UINT nJack,
__out  KSJACK_DESCRIPTION *pDescription);
END_INTERFACE
} IKsJackDescriptionVtbl;
interface IKsJackDescription
{
CONST_VTBL struct IKsJackDescriptionVtbl *lpVtbl;
};
#ifdef COBJMACROS
#define IKsJackDescription_QueryInterface(This,riid,ppvObject)	\
( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define IKsJackDescription_AddRef(This)	\
( (This)->lpVtbl -> AddRef(This) ) 
#define IKsJackDescription_Release(This)	\
( (This)->lpVtbl -> Release(This) ) 
#define IKsJackDescription_GetJackCount(This,pcJacks)	\
( (This)->lpVtbl -> GetJackCount(This,pcJacks) ) 
#define IKsJackDescription_GetJackDescription(This,nJack,pDescription)	\
( (This)->lpVtbl -> GetJackDescription(This,nJack,pDescription) ) 
#endif  
#endif 	 
#endif 	 
#ifndef __IPartsList_INTERFACE_DEFINED__
#define __IPartsList_INTERFACE_DEFINED__
EXTERN_C const IID IID_IPartsList;
#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("6DAA848C-5EB0-45CC-AEA5-998A2CDA1FFB")
IPartsList : public IUnknown
{
public:
virtual   HRESULT STDMETHODCALLTYPE GetCount( 
__out  UINT *pCount) = 0;
virtual   HRESULT STDMETHODCALLTYPE GetPart( 
__in  UINT nIndex,
__out  IPart **ppPart) = 0;
};
#else 	 
typedef struct IPartsListVtbl
{
BEGIN_INTERFACE
HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
IPartsList * This,
REFIID riid,
__RPC__deref_out  void **ppvObject);
ULONG ( STDMETHODCALLTYPE *AddRef )( 
IPartsList * This);
ULONG ( STDMETHODCALLTYPE *Release )( 
IPartsList * This);
HRESULT ( STDMETHODCALLTYPE *GetCount )( 
IPartsList * This,
__out  UINT *pCount);
HRESULT ( STDMETHODCALLTYPE *GetPart )( 
IPartsList * This,
__in  UINT nIndex,
__out  IPart **ppPart);
END_INTERFACE
} IPartsListVtbl;
interface IPartsList
{
CONST_VTBL struct IPartsListVtbl *lpVtbl;
};
#ifdef COBJMACROS
#define IPartsList_QueryInterface(This,riid,ppvObject)	\
( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define IPartsList_AddRef(This)	\
( (This)->lpVtbl -> AddRef(This) ) 
#define IPartsList_Release(This)	\
( (This)->lpVtbl -> Release(This) ) 
#define IPartsList_GetCount(This,pCount)	\
( (This)->lpVtbl -> GetCount(This,pCount) ) 
#define IPartsList_GetPart(This,nIndex,ppPart)	\
( (This)->lpVtbl -> GetPart(This,nIndex,ppPart) ) 
#endif  
#endif 	 
#endif 	 
#ifndef __IPart_INTERFACE_DEFINED__
#define __IPart_INTERFACE_DEFINED__
EXTERN_C const IID IID_IPart;
#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("AE2DE0E4-5BCA-4F2D-AA46-5D13F8FDB3A9")
IPart : public IUnknown
{
public:
virtual   HRESULT STDMETHODCALLTYPE GetName( 
__deref_out  LPWSTR *ppwstrName) = 0;
virtual   HRESULT STDMETHODCALLTYPE GetLocalId( 
__out  UINT *pnId) = 0;
virtual   HRESULT STDMETHODCALLTYPE GetGlobalId( 
__deref_out  LPWSTR *ppwstrGlobalId) = 0;
virtual   HRESULT STDMETHODCALLTYPE GetPartType( 
__out  PartType *pPartType) = 0;
virtual   HRESULT STDMETHODCALLTYPE GetSubType( 
GUID *pSubType) = 0;
virtual   HRESULT STDMETHODCALLTYPE GetControlInterfaceCount( 
__out  UINT *pCount) = 0;
virtual   HRESULT STDMETHODCALLTYPE GetControlInterface( 
__in  UINT nIndex,
__out  IControlInterface **ppInterfaceDesc) = 0;
virtual   HRESULT STDMETHODCALLTYPE EnumPartsIncoming( 
__out  IPartsList **ppParts) = 0;
virtual   HRESULT STDMETHODCALLTYPE EnumPartsOutgoing( 
__out  IPartsList **ppParts) = 0;
virtual   HRESULT STDMETHODCALLTYPE GetTopologyObject( 
__out  IDeviceTopology **ppTopology) = 0;
virtual   HRESULT STDMETHODCALLTYPE Activate( 
__in  DWORD dwClsContext,
__in  REFIID refiid,
__out_opt  void **ppvObject) = 0;
virtual   HRESULT STDMETHODCALLTYPE RegisterControlChangeCallback( 
__in  REFGUID riid,
__in  IControlChangeNotify *pNotify) = 0;
virtual   HRESULT STDMETHODCALLTYPE UnregisterControlChangeCallback( 
__in  IControlChangeNotify *pNotify) = 0;
};
#else 	 
typedef struct IPartVtbl
{
BEGIN_INTERFACE
HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
IPart * This,
REFIID riid,
__RPC__deref_out  void **ppvObject);
ULONG ( STDMETHODCALLTYPE *AddRef )( 
IPart * This);
ULONG ( STDMETHODCALLTYPE *Release )( 
IPart * This);
HRESULT ( STDMETHODCALLTYPE *GetName )( 
IPart * This,
__deref_out  LPWSTR *ppwstrName);
HRESULT ( STDMETHODCALLTYPE *GetLocalId )( 
IPart * This,
__out  UINT *pnId);
HRESULT ( STDMETHODCALLTYPE *GetGlobalId )( 
IPart * This,
__deref_out  LPWSTR *ppwstrGlobalId);
HRESULT ( STDMETHODCALLTYPE *GetPartType )( 
IPart * This,
__out  PartType *pPartType);
HRESULT ( STDMETHODCALLTYPE *GetSubType )( 
IPart * This,
GUID *pSubType);
HRESULT ( STDMETHODCALLTYPE *GetControlInterfaceCount )( 
IPart * This,
__out  UINT *pCount);
HRESULT ( STDMETHODCALLTYPE *GetControlInterface )( 
IPart * This,
__in  UINT nIndex,
__out  IControlInterface **ppInterfaceDesc);
HRESULT ( STDMETHODCALLTYPE *EnumPartsIncoming )( 
IPart * This,
__out  IPartsList **ppParts);
HRESULT ( STDMETHODCALLTYPE *EnumPartsOutgoing )( 
IPart * This,
__out  IPartsList **ppParts);
HRESULT ( STDMETHODCALLTYPE *GetTopologyObject )( 
IPart * This,
__out  IDeviceTopology **ppTopology);
HRESULT ( STDMETHODCALLTYPE *Activate )( 
IPart * This,
__in  DWORD dwClsContext,
__in  REFIID refiid,
__out_opt  void **ppvObject);
HRESULT ( STDMETHODCALLTYPE *RegisterControlChangeCallback )( 
IPart * This,
__in  REFGUID riid,
__in  IControlChangeNotify *pNotify);
HRESULT ( STDMETHODCALLTYPE *UnregisterControlChangeCallback )( 
IPart * This,
__in  IControlChangeNotify *pNotify);
END_INTERFACE
} IPartVtbl;
interface IPart
{
CONST_VTBL struct IPartVtbl *lpVtbl;
};
#ifdef COBJMACROS
#define IPart_QueryInterface(This,riid,ppvObject)	\
( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define IPart_AddRef(This)	\
( (This)->lpVtbl -> AddRef(This) ) 
#define IPart_Release(This)	\
( (This)->lpVtbl -> Release(This) ) 
#define IPart_GetName(This,ppwstrName)	\
( (This)->lpVtbl -> GetName(This,ppwstrName) ) 
#define IPart_GetLocalId(This,pnId)	\
( (This)->lpVtbl -> GetLocalId(This,pnId) ) 
#define IPart_GetGlobalId(This,ppwstrGlobalId)	\
( (This)->lpVtbl -> GetGlobalId(This,ppwstrGlobalId) ) 
#define IPart_GetPartType(This,pPartType)	\
( (This)->lpVtbl -> GetPartType(This,pPartType) ) 
#define IPart_GetSubType(This,pSubType)	\
( (This)->lpVtbl -> GetSubType(This,pSubType) ) 
#define IPart_GetControlInterfaceCount(This,pCount)	\
( (This)->lpVtbl -> GetControlInterfaceCount(This,pCount) ) 
#define IPart_GetControlInterface(This,nIndex,ppInterfaceDesc)	\
( (This)->lpVtbl -> GetControlInterface(This,nIndex,ppInterfaceDesc) ) 
#define IPart_EnumPartsIncoming(This,ppParts)	\
( (This)->lpVtbl -> EnumPartsIncoming(This,ppParts) ) 
#define IPart_EnumPartsOutgoing(This,ppParts)	\
( (This)->lpVtbl -> EnumPartsOutgoing(This,ppParts) ) 
#define IPart_GetTopologyObject(This,ppTopology)	\
( (This)->lpVtbl -> GetTopologyObject(This,ppTopology) ) 
#define IPart_Activate(This,dwClsContext,refiid,ppvObject)	\
( (This)->lpVtbl -> Activate(This,dwClsContext,refiid,ppvObject) ) 
#define IPart_RegisterControlChangeCallback(This,riid,pNotify)	\
( (This)->lpVtbl -> RegisterControlChangeCallback(This,riid,pNotify) ) 
#define IPart_UnregisterControlChangeCallback(This,pNotify)	\
( (This)->lpVtbl -> UnregisterControlChangeCallback(This,pNotify) ) 
#endif  
#endif 	 
#endif 	 
#ifndef __IConnector_INTERFACE_DEFINED__
#define __IConnector_INTERFACE_DEFINED__
EXTERN_C const IID IID_IConnector;
#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("9c2c4058-23f5-41de-877a-df3af236a09e")
IConnector : public IUnknown
{
public:
virtual   HRESULT STDMETHODCALLTYPE GetType( 
__out  ConnectorType *pType) = 0;
virtual   HRESULT STDMETHODCALLTYPE GetDataFlow( 
__out  DataFlow *pFlow) = 0;
virtual   HRESULT STDMETHODCALLTYPE ConnectTo( 
__in  IConnector *pConnectTo) = 0;
virtual   HRESULT STDMETHODCALLTYPE Disconnect( void) = 0;
virtual   HRESULT STDMETHODCALLTYPE IsConnected( 
__out  BOOL *pbConnected) = 0;
virtual   HRESULT STDMETHODCALLTYPE GetConnectedTo( 
__out  IConnector **ppConTo) = 0;
virtual   HRESULT STDMETHODCALLTYPE GetConnectorIdConnectedTo( 
__deref_out  LPWSTR *ppwstrConnectorId) = 0;
virtual   HRESULT STDMETHODCALLTYPE GetDeviceIdConnectedTo( 
__deref_out  LPWSTR *ppwstrDeviceId) = 0;
};
#else 	 
typedef struct IConnectorVtbl
{
BEGIN_INTERFACE
HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
IConnector * This,
REFIID riid,
__RPC__deref_out  void **ppvObject);
ULONG ( STDMETHODCALLTYPE *AddRef )( 
IConnector * This);
ULONG ( STDMETHODCALLTYPE *Release )( 
IConnector * This);
HRESULT ( STDMETHODCALLTYPE *GetType )( 
IConnector * This,
__out  ConnectorType *pType);
HRESULT ( STDMETHODCALLTYPE *GetDataFlow )( 
IConnector * This,
__out  DataFlow *pFlow);
HRESULT ( STDMETHODCALLTYPE *ConnectTo )( 
IConnector * This,
__in  IConnector *pConnectTo);
HRESULT ( STDMETHODCALLTYPE *Disconnect )( 
IConnector * This);
HRESULT ( STDMETHODCALLTYPE *IsConnected )( 
IConnector * This,
__out  BOOL *pbConnected);
HRESULT ( STDMETHODCALLTYPE *GetConnectedTo )( 
IConnector * This,
__out  IConnector **ppConTo);
HRESULT ( STDMETHODCALLTYPE *GetConnectorIdConnectedTo )( 
IConnector * This,
__deref_out  LPWSTR *ppwstrConnectorId);
HRESULT ( STDMETHODCALLTYPE *GetDeviceIdConnectedTo )( 
IConnector * This,
__deref_out  LPWSTR *ppwstrDeviceId);
END_INTERFACE
} IConnectorVtbl;
interface IConnector
{
CONST_VTBL struct IConnectorVtbl *lpVtbl;
};
#ifdef COBJMACROS
#define IConnector_QueryInterface(This,riid,ppvObject)	\
( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define IConnector_AddRef(This)	\
( (This)->lpVtbl -> AddRef(This) ) 
#define IConnector_Release(This)	\
( (This)->lpVtbl -> Release(This) ) 
#define IConnector_GetType(This,pType)	\
( (This)->lpVtbl -> GetType(This,pType) ) 
#define IConnector_GetDataFlow(This,pFlow)	\
( (This)->lpVtbl -> GetDataFlow(This,pFlow) ) 
#define IConnector_ConnectTo(This,pConnectTo)	\
( (This)->lpVtbl -> ConnectTo(This,pConnectTo) ) 
#define IConnector_Disconnect(This)	\
( (This)->lpVtbl -> Disconnect(This) ) 
#define IConnector_IsConnected(This,pbConnected)	\
( (This)->lpVtbl -> IsConnected(This,pbConnected) ) 
#define IConnector_GetConnectedTo(This,ppConTo)	\
( (This)->lpVtbl -> GetConnectedTo(This,ppConTo) ) 
#define IConnector_GetConnectorIdConnectedTo(This,ppwstrConnectorId)	\
( (This)->lpVtbl -> GetConnectorIdConnectedTo(This,ppwstrConnectorId) ) 
#define IConnector_GetDeviceIdConnectedTo(This,ppwstrDeviceId)	\
( (This)->lpVtbl -> GetDeviceIdConnectedTo(This,ppwstrDeviceId) ) 
#endif  
#endif 	 
#endif 	 
#ifndef __ISubunit_INTERFACE_DEFINED__
#define __ISubunit_INTERFACE_DEFINED__
EXTERN_C const IID IID_ISubunit;
#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("82149A85-DBA6-4487-86BB-EA8F7FEFCC71")
ISubunit : public IUnknown
{
public:
};
#else 	 
typedef struct ISubunitVtbl
{
BEGIN_INTERFACE
HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
ISubunit * This,
REFIID riid,
__RPC__deref_out  void **ppvObject);
ULONG ( STDMETHODCALLTYPE *AddRef )( 
ISubunit * This);
ULONG ( STDMETHODCALLTYPE *Release )( 
ISubunit * This);
END_INTERFACE
} ISubunitVtbl;
interface ISubunit
{
CONST_VTBL struct ISubunitVtbl *lpVtbl;
};
#ifdef COBJMACROS
#define ISubunit_QueryInterface(This,riid,ppvObject)	\
( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define ISubunit_AddRef(This)	\
( (This)->lpVtbl -> AddRef(This) ) 
#define ISubunit_Release(This)	\
( (This)->lpVtbl -> Release(This) ) 
#endif  
#endif 	 
#endif 	 
#ifndef __IControlInterface_INTERFACE_DEFINED__
#define __IControlInterface_INTERFACE_DEFINED__
EXTERN_C const IID IID_IControlInterface;
#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("45d37c3f-5140-444a-ae24-400789f3cbf3")
IControlInterface : public IUnknown
{
public:
virtual   HRESULT STDMETHODCALLTYPE GetName( 
__deref_out  LPWSTR *ppwstrName) = 0;
virtual   HRESULT STDMETHODCALLTYPE GetIID( 
__out  GUID *pIID) = 0;
};
#else 	 
typedef struct IControlInterfaceVtbl
{
BEGIN_INTERFACE
HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
IControlInterface * This,
REFIID riid,
__RPC__deref_out  void **ppvObject);
ULONG ( STDMETHODCALLTYPE *AddRef )( 
IControlInterface * This);
ULONG ( STDMETHODCALLTYPE *Release )( 
IControlInterface * This);
HRESULT ( STDMETHODCALLTYPE *GetName )( 
IControlInterface * This,
__deref_out  LPWSTR *ppwstrName);
HRESULT ( STDMETHODCALLTYPE *GetIID )( 
IControlInterface * This,
__out  GUID *pIID);
END_INTERFACE
} IControlInterfaceVtbl;
interface IControlInterface
{
CONST_VTBL struct IControlInterfaceVtbl *lpVtbl;
};
#ifdef COBJMACROS
#define IControlInterface_QueryInterface(This,riid,ppvObject)	\
( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define IControlInterface_AddRef(This)	\
( (This)->lpVtbl -> AddRef(This) ) 
#define IControlInterface_Release(This)	\
( (This)->lpVtbl -> Release(This) ) 
#define IControlInterface_GetName(This,ppwstrName)	\
( (This)->lpVtbl -> GetName(This,ppwstrName) ) 
#define IControlInterface_GetIID(This,pIID)	\
( (This)->lpVtbl -> GetIID(This,pIID) ) 
#endif  
#endif 	 
#endif 	 
#ifndef __IControlChangeNotify_INTERFACE_DEFINED__
#define __IControlChangeNotify_INTERFACE_DEFINED__
EXTERN_C const IID IID_IControlChangeNotify;
#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("A09513ED-C709-4d21-BD7B-5F34C47F3947")
IControlChangeNotify : public IUnknown
{
public:
virtual   HRESULT STDMETHODCALLTYPE OnNotify( 
__in  DWORD dwSenderProcessId,
__in_opt  LPCGUID pguidEventContext) = 0;
};
#else 	 
typedef struct IControlChangeNotifyVtbl
{
BEGIN_INTERFACE
HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
IControlChangeNotify * This,
REFIID riid,
__RPC__deref_out  void **ppvObject);
ULONG ( STDMETHODCALLTYPE *AddRef )( 
IControlChangeNotify * This);
ULONG ( STDMETHODCALLTYPE *Release )( 
IControlChangeNotify * This);
HRESULT ( STDMETHODCALLTYPE *OnNotify )( 
IControlChangeNotify * This,
__in  DWORD dwSenderProcessId,
__in_opt  LPCGUID pguidEventContext);
END_INTERFACE
} IControlChangeNotifyVtbl;
interface IControlChangeNotify
{
CONST_VTBL struct IControlChangeNotifyVtbl *lpVtbl;
};
#ifdef COBJMACROS
#define IControlChangeNotify_QueryInterface(This,riid,ppvObject)	\
( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define IControlChangeNotify_AddRef(This)	\
( (This)->lpVtbl -> AddRef(This) ) 
#define IControlChangeNotify_Release(This)	\
( (This)->lpVtbl -> Release(This) ) 
#define IControlChangeNotify_OnNotify(This,dwSenderProcessId,pguidEventContext)	\
( (This)->lpVtbl -> OnNotify(This,dwSenderProcessId,pguidEventContext) ) 
#endif  
#endif 	 
#endif 	 
#ifndef __IDeviceTopology_INTERFACE_DEFINED__
#define __IDeviceTopology_INTERFACE_DEFINED__
EXTERN_C const IID IID_IDeviceTopology;
#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("2A07407E-6497-4A18-9787-32F79BD0D98F")
IDeviceTopology : public IUnknown
{
public:
virtual   HRESULT STDMETHODCALLTYPE GetConnectorCount( 
__out  UINT *pCount) = 0;
virtual   HRESULT STDMETHODCALLTYPE GetConnector( 
__in  UINT nIndex,
__out  IConnector **ppConnector) = 0;
virtual   HRESULT STDMETHODCALLTYPE GetSubunitCount( 
__out  UINT *pCount) = 0;
virtual   HRESULT STDMETHODCALLTYPE GetSubunit( 
__in  UINT nIndex,
__deref_out  ISubunit **ppSubunit) = 0;
virtual   HRESULT STDMETHODCALLTYPE GetPartById( 
__in  UINT nId,
__deref_out  IPart **ppPart) = 0;
virtual   HRESULT STDMETHODCALLTYPE GetDeviceId( 
__deref_out  LPWSTR *ppwstrDeviceId) = 0;
virtual   HRESULT STDMETHODCALLTYPE GetSignalPath( 
__in  IPart *pIPartFrom,
__in  IPart *pIPartTo,
__in  BOOL bRejectMixedPaths,
__deref_out  IPartsList **ppParts) = 0;
};
#else 	 
typedef struct IDeviceTopologyVtbl
{
BEGIN_INTERFACE
HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
IDeviceTopology * This,
REFIID riid,
__RPC__deref_out  void **ppvObject);
ULONG ( STDMETHODCALLTYPE *AddRef )( 
IDeviceTopology * This);
ULONG ( STDMETHODCALLTYPE *Release )( 
IDeviceTopology * This);
HRESULT ( STDMETHODCALLTYPE *GetConnectorCount )( 
IDeviceTopology * This,
__out  UINT *pCount);
HRESULT ( STDMETHODCALLTYPE *GetConnector )( 
IDeviceTopology * This,
__in  UINT nIndex,
__out  IConnector **ppConnector);
HRESULT ( STDMETHODCALLTYPE *GetSubunitCount )( 
IDeviceTopology * This,
__out  UINT *pCount);
HRESULT ( STDMETHODCALLTYPE *GetSubunit )( 
IDeviceTopology * This,
__in  UINT nIndex,
__deref_out  ISubunit **ppSubunit);
HRESULT ( STDMETHODCALLTYPE *GetPartById )( 
IDeviceTopology * This,
__in  UINT nId,
__deref_out  IPart **ppPart);
HRESULT ( STDMETHODCALLTYPE *GetDeviceId )( 
IDeviceTopology * This,
__deref_out  LPWSTR *ppwstrDeviceId);
HRESULT ( STDMETHODCALLTYPE *GetSignalPath )( 
IDeviceTopology * This,
__in  IPart *pIPartFrom,
__in  IPart *pIPartTo,
__in  BOOL bRejectMixedPaths,
__deref_out  IPartsList **ppParts);
END_INTERFACE
} IDeviceTopologyVtbl;
interface IDeviceTopology
{
CONST_VTBL struct IDeviceTopologyVtbl *lpVtbl;
};
#ifdef COBJMACROS
#define IDeviceTopology_QueryInterface(This,riid,ppvObject)	\
( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define IDeviceTopology_AddRef(This)	\
( (This)->lpVtbl -> AddRef(This) ) 
#define IDeviceTopology_Release(This)	\
( (This)->lpVtbl -> Release(This) ) 
#define IDeviceTopology_GetConnectorCount(This,pCount)	\
( (This)->lpVtbl -> GetConnectorCount(This,pCount) ) 
#define IDeviceTopology_GetConnector(This,nIndex,ppConnector)	\
( (This)->lpVtbl -> GetConnector(This,nIndex,ppConnector) ) 
#define IDeviceTopology_GetSubunitCount(This,pCount)	\
( (This)->lpVtbl -> GetSubunitCount(This,pCount) ) 
#define IDeviceTopology_GetSubunit(This,nIndex,ppSubunit)	\
( (This)->lpVtbl -> GetSubunit(This,nIndex,ppSubunit) ) 
#define IDeviceTopology_GetPartById(This,nId,ppPart)	\
( (This)->lpVtbl -> GetPartById(This,nId,ppPart) ) 
#define IDeviceTopology_GetDeviceId(This,ppwstrDeviceId)	\
( (This)->lpVtbl -> GetDeviceId(This,ppwstrDeviceId) ) 
#define IDeviceTopology_GetSignalPath(This,pIPartFrom,pIPartTo,bRejectMixedPaths,ppParts)	\
( (This)->lpVtbl -> GetSignalPath(This,pIPartFrom,pIPartTo,bRejectMixedPaths,ppParts) ) 
#endif  
#endif 	 
#endif 	 
#ifndef __DevTopologyLib_LIBRARY_DEFINED__
#define __DevTopologyLib_LIBRARY_DEFINED__
EXTERN_C const IID LIBID_DevTopologyLib;
EXTERN_C const CLSID CLSID_DeviceTopology;
#ifdef __cplusplus
class DECLSPEC_UUID("1DF639D0-5EC1-47AA-9379-828DC1AA8C59")
DeviceTopology;
#endif
#endif  
#ifdef __cplusplus
}
#endif
#endif
