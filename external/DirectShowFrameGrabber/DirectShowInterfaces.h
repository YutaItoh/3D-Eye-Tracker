
// no doxygen documentation for this file, please!
///@cond HIDDEN

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 7.00.0500 */
/* at Thu Jun 05 11:36:21 2008
 */
/* Compiler settings for DirectShowInterfaces.idl:
    Oicf, W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __DirectShowInterfaces_h__
#define __DirectShowInterfaces_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IMediaControl_FWD_DEFINED__
#define __IMediaControl_FWD_DEFINED__
typedef interface IMediaControl IMediaControl;
#endif 	/* __IMediaControl_FWD_DEFINED__ */


#ifndef __IPin_FWD_DEFINED__
#define __IPin_FWD_DEFINED__
typedef interface IPin IPin;
#endif 	/* __IPin_FWD_DEFINED__ */


#ifndef __ICreateDevEnum_FWD_DEFINED__
#define __ICreateDevEnum_FWD_DEFINED__
typedef interface ICreateDevEnum ICreateDevEnum;
#endif 	/* __ICreateDevEnum_FWD_DEFINED__ */


#ifndef __IFilterGraph_FWD_DEFINED__
#define __IFilterGraph_FWD_DEFINED__
typedef interface IFilterGraph IFilterGraph;
#endif 	/* __IFilterGraph_FWD_DEFINED__ */


#ifndef __IGraphBuilder_FWD_DEFINED__
#define __IGraphBuilder_FWD_DEFINED__
typedef interface IGraphBuilder IGraphBuilder;
#endif 	/* __IGraphBuilder_FWD_DEFINED__ */


#ifndef __ICaptureGraphBuilder2_FWD_DEFINED__
#define __ICaptureGraphBuilder2_FWD_DEFINED__
typedef interface ICaptureGraphBuilder2 ICaptureGraphBuilder2;
#endif 	/* __ICaptureGraphBuilder2_FWD_DEFINED__ */


#ifndef __IMediaFilter_FWD_DEFINED__
#define __IMediaFilter_FWD_DEFINED__
typedef interface IMediaFilter IMediaFilter;
#endif 	/* __IMediaFilter_FWD_DEFINED__ */


#ifndef __IBaseFilter_FWD_DEFINED__
#define __IBaseFilter_FWD_DEFINED__
typedef interface IBaseFilter IBaseFilter;
#endif 	/* __IBaseFilter_FWD_DEFINED__ */


#ifndef __IAMStreamConfig_FWD_DEFINED__
#define __IAMStreamConfig_FWD_DEFINED__
typedef interface IAMStreamConfig IAMStreamConfig;
#endif 	/* __IAMStreamConfig_FWD_DEFINED__ */


#ifndef __ISampleGrabberCB_FWD_DEFINED__
#define __ISampleGrabberCB_FWD_DEFINED__
typedef interface ISampleGrabberCB ISampleGrabberCB;
#endif 	/* __ISampleGrabberCB_FWD_DEFINED__ */


#ifndef __ISampleGrabber_FWD_DEFINED__
#define __ISampleGrabber_FWD_DEFINED__
typedef interface ISampleGrabber ISampleGrabber;
#endif 	/* __ISampleGrabber_FWD_DEFINED__ */


#ifndef __IMediaSample_FWD_DEFINED__
#define __IMediaSample_FWD_DEFINED__
typedef interface IMediaSample IMediaSample;
#endif 	/* __IMediaSample_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


/* interface __MIDL_itf_DirectShowInterfaces_0000_0000 */
/* [local] */ 

typedef LONGLONG REFERENCE_TIME;

typedef long OAFilterState;







typedef interface IEnumMediaTypes IEnumMediaTypes;
typedef interface IEnumFilters IEnumFilters;
typedef interface IEnumPins IEnumPins;
typedef interface IFileSinkFilter IFileSinkFilter;
typedef interface IAMCopyCaptureFileProgress IAMCopyCaptureFileProgress;
typedef interface IReferenceClock IReferenceClock;




extern RPC_IF_HANDLE __MIDL_itf_DirectShowInterfaces_0000_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_DirectShowInterfaces_0000_0000_v0_0_s_ifspec;

#ifndef __IMediaControl_INTERFACE_DEFINED__
#define __IMediaControl_INTERFACE_DEFINED__

/* interface IMediaControl */
/* [object][dual][oleautomation][helpstring][uuid] */ 


EXTERN_C const IID IID_IMediaControl;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("56a868b1-0ad4-11ce-b03a-0020af0ba770")
    IMediaControl : public IDispatch
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Run( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Pause( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Stop( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetState( 
            /* [in] */ LONG msTimeout,
            /* [out] */ OAFilterState *pfs) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RenderFile( 
            /* [in] */ BSTR strFilename) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE AddSourceFilter( 
            /* [in] */ BSTR strFilename,
            /* [out] */ IDispatch **ppUnk) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_FilterCollection( 
            /* [retval][out] */ IDispatch **ppUnk) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_RegFilterCollection( 
            /* [retval][out] */ IDispatch **ppUnk) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE StopWhenReady( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IMediaControlVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IMediaControl * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IMediaControl * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IMediaControl * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IMediaControl * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IMediaControl * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IMediaControl * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IMediaControl * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        HRESULT ( STDMETHODCALLTYPE *Run )( 
            IMediaControl * This);
        
        HRESULT ( STDMETHODCALLTYPE *Pause )( 
            IMediaControl * This);
        
        HRESULT ( STDMETHODCALLTYPE *Stop )( 
            IMediaControl * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetState )( 
            IMediaControl * This,
            /* [in] */ LONG msTimeout,
            /* [out] */ OAFilterState *pfs);
        
        HRESULT ( STDMETHODCALLTYPE *RenderFile )( 
            IMediaControl * This,
            /* [in] */ BSTR strFilename);
        
        HRESULT ( STDMETHODCALLTYPE *AddSourceFilter )( 
            IMediaControl * This,
            /* [in] */ BSTR strFilename,
            /* [out] */ IDispatch **ppUnk);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_FilterCollection )( 
            IMediaControl * This,
            /* [retval][out] */ IDispatch **ppUnk);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_RegFilterCollection )( 
            IMediaControl * This,
            /* [retval][out] */ IDispatch **ppUnk);
        
        HRESULT ( STDMETHODCALLTYPE *StopWhenReady )( 
            IMediaControl * This);
        
        END_INTERFACE
    } IMediaControlVtbl;

    interface IMediaControl
    {
        CONST_VTBL struct IMediaControlVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMediaControl_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IMediaControl_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IMediaControl_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IMediaControl_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IMediaControl_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IMediaControl_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IMediaControl_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IMediaControl_Run(This)	\
    ( (This)->lpVtbl -> Run(This) ) 

#define IMediaControl_Pause(This)	\
    ( (This)->lpVtbl -> Pause(This) ) 

#define IMediaControl_Stop(This)	\
    ( (This)->lpVtbl -> Stop(This) ) 

#define IMediaControl_GetState(This,msTimeout,pfs)	\
    ( (This)->lpVtbl -> GetState(This,msTimeout,pfs) ) 

#define IMediaControl_RenderFile(This,strFilename)	\
    ( (This)->lpVtbl -> RenderFile(This,strFilename) ) 

#define IMediaControl_AddSourceFilter(This,strFilename,ppUnk)	\
    ( (This)->lpVtbl -> AddSourceFilter(This,strFilename,ppUnk) ) 

#define IMediaControl_get_FilterCollection(This,ppUnk)	\
    ( (This)->lpVtbl -> get_FilterCollection(This,ppUnk) ) 

#define IMediaControl_get_RegFilterCollection(This,ppUnk)	\
    ( (This)->lpVtbl -> get_RegFilterCollection(This,ppUnk) ) 

#define IMediaControl_StopWhenReady(This)	\
    ( (This)->lpVtbl -> StopWhenReady(This) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IMediaControl_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_DirectShowInterfaces_0000_0001 */
/* [local] */ 

typedef struct _AMMediaType
    {
    GUID majortype;
    GUID subtype;
    BOOL bFixedSizeSamples;
    BOOL bTemporalCompression;
    ULONG lSampleSize;
    GUID formattype;
    IUnknown *pUnk;
    ULONG cbFormat;
    BYTE *pbFormat;
    } 	AM_MEDIA_TYPE;

typedef 
enum _PinDirection
    {	PINDIR_INPUT	= 0,
	PINDIR_OUTPUT	= ( PINDIR_INPUT + 1 ) 
    } 	PIN_DIRECTION;

#define MAX_PIN_NAME     128
#define MAX_FILTER_NAME  128


extern RPC_IF_HANDLE __MIDL_itf_DirectShowInterfaces_0000_0001_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_DirectShowInterfaces_0000_0001_v0_0_s_ifspec;

#ifndef __IPin_INTERFACE_DEFINED__
#define __IPin_INTERFACE_DEFINED__

/* interface IPin */
/* [unique][uuid][object][local] */ 

typedef struct _PinInfo
    {
    IBaseFilter *pFilter;
    PIN_DIRECTION dir;
    WCHAR achName[ 128 ];
    } 	PIN_INFO;


EXTERN_C const IID IID_IPin;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("56a86891-0ad4-11ce-b03a-0020af0ba770")
    IPin : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Connect( 
            /* [in] */ IPin *pReceivePin,
            /* [in] */ 
            __in_opt  const AM_MEDIA_TYPE *pmt) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ReceiveConnection( 
            /* [in] */ IPin *pConnector,
            /* [in] */ const AM_MEDIA_TYPE *pmt) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Disconnect( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ConnectedTo( 
            /* [out] */ 
            __out  IPin **pPin) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ConnectionMediaType( 
            /* [out] */ 
            __out  AM_MEDIA_TYPE *pmt) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE QueryPinInfo( 
            /* [out] */ 
            __out  PIN_INFO *pInfo) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE QueryDirection( 
            /* [out] */ 
            __out  PIN_DIRECTION *pPinDir) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE QueryId( 
            /* [out] */ 
            __out  LPWSTR *Id) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE QueryAccept( 
            /* [in] */ const AM_MEDIA_TYPE *pmt) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EnumMediaTypes( 
            /* [out] */ 
            __out  IEnumMediaTypes **ppEnum) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE QueryInternalConnections( 
            /* [out] */ 
            __out_ecount_part_opt(*nPin, *nPin)  IPin **apPin,
            /* [out][in] */ ULONG *nPin) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EndOfStream( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE BeginFlush( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EndFlush( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE NewSegment( 
            /* [in] */ REFERENCE_TIME tStart,
            /* [in] */ REFERENCE_TIME tStop,
            /* [in] */ double dRate) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IPinVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IPin * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IPin * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IPin * This);
        
        HRESULT ( STDMETHODCALLTYPE *Connect )( 
            IPin * This,
            /* [in] */ IPin *pReceivePin,
            /* [in] */ 
            __in_opt  const AM_MEDIA_TYPE *pmt);
        
        HRESULT ( STDMETHODCALLTYPE *ReceiveConnection )( 
            IPin * This,
            /* [in] */ IPin *pConnector,
            /* [in] */ const AM_MEDIA_TYPE *pmt);
        
        HRESULT ( STDMETHODCALLTYPE *Disconnect )( 
            IPin * This);
        
        HRESULT ( STDMETHODCALLTYPE *ConnectedTo )( 
            IPin * This,
            /* [out] */ 
            __out  IPin **pPin);
        
        HRESULT ( STDMETHODCALLTYPE *ConnectionMediaType )( 
            IPin * This,
            /* [out] */ 
            __out  AM_MEDIA_TYPE *pmt);
        
        HRESULT ( STDMETHODCALLTYPE *QueryPinInfo )( 
            IPin * This,
            /* [out] */ 
            __out  PIN_INFO *pInfo);
        
        HRESULT ( STDMETHODCALLTYPE *QueryDirection )( 
            IPin * This,
            /* [out] */ 
            __out  PIN_DIRECTION *pPinDir);
        
        HRESULT ( STDMETHODCALLTYPE *QueryId )( 
            IPin * This,
            /* [out] */ 
            __out  LPWSTR *Id);
        
        HRESULT ( STDMETHODCALLTYPE *QueryAccept )( 
            IPin * This,
            /* [in] */ const AM_MEDIA_TYPE *pmt);
        
        HRESULT ( STDMETHODCALLTYPE *EnumMediaTypes )( 
            IPin * This,
            /* [out] */ 
            __out  IEnumMediaTypes **ppEnum);
        
        HRESULT ( STDMETHODCALLTYPE *QueryInternalConnections )( 
            IPin * This,
            /* [out] */ 
            __out_ecount_part_opt(*nPin, *nPin)  IPin **apPin,
            /* [out][in] */ ULONG *nPin);
        
        HRESULT ( STDMETHODCALLTYPE *EndOfStream )( 
            IPin * This);
        
        HRESULT ( STDMETHODCALLTYPE *BeginFlush )( 
            IPin * This);
        
        HRESULT ( STDMETHODCALLTYPE *EndFlush )( 
            IPin * This);
        
        HRESULT ( STDMETHODCALLTYPE *NewSegment )( 
            IPin * This,
            /* [in] */ REFERENCE_TIME tStart,
            /* [in] */ REFERENCE_TIME tStop,
            /* [in] */ double dRate);
        
        END_INTERFACE
    } IPinVtbl;

    interface IPin
    {
        CONST_VTBL struct IPinVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IPin_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IPin_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IPin_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IPin_Connect(This,pReceivePin,pmt)	\
    ( (This)->lpVtbl -> Connect(This,pReceivePin,pmt) ) 

#define IPin_ReceiveConnection(This,pConnector,pmt)	\
    ( (This)->lpVtbl -> ReceiveConnection(This,pConnector,pmt) ) 

#define IPin_Disconnect(This)	\
    ( (This)->lpVtbl -> Disconnect(This) ) 

#define IPin_ConnectedTo(This,pPin)	\
    ( (This)->lpVtbl -> ConnectedTo(This,pPin) ) 

#define IPin_ConnectionMediaType(This,pmt)	\
    ( (This)->lpVtbl -> ConnectionMediaType(This,pmt) ) 

#define IPin_QueryPinInfo(This,pInfo)	\
    ( (This)->lpVtbl -> QueryPinInfo(This,pInfo) ) 

#define IPin_QueryDirection(This,pPinDir)	\
    ( (This)->lpVtbl -> QueryDirection(This,pPinDir) ) 

#define IPin_QueryId(This,Id)	\
    ( (This)->lpVtbl -> QueryId(This,Id) ) 

#define IPin_QueryAccept(This,pmt)	\
    ( (This)->lpVtbl -> QueryAccept(This,pmt) ) 

#define IPin_EnumMediaTypes(This,ppEnum)	\
    ( (This)->lpVtbl -> EnumMediaTypes(This,ppEnum) ) 

#define IPin_QueryInternalConnections(This,apPin,nPin)	\
    ( (This)->lpVtbl -> QueryInternalConnections(This,apPin,nPin) ) 

#define IPin_EndOfStream(This)	\
    ( (This)->lpVtbl -> EndOfStream(This) ) 

#define IPin_BeginFlush(This)	\
    ( (This)->lpVtbl -> BeginFlush(This) ) 

#define IPin_EndFlush(This)	\
    ( (This)->lpVtbl -> EndFlush(This) ) 

#define IPin_NewSegment(This,tStart,tStop,dRate)	\
    ( (This)->lpVtbl -> NewSegment(This,tStart,tStop,dRate) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IPin_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_DirectShowInterfaces_0000_0002 */
/* [local] */ 

typedef IPin *PPIN;



extern RPC_IF_HANDLE __MIDL_itf_DirectShowInterfaces_0000_0002_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_DirectShowInterfaces_0000_0002_v0_0_s_ifspec;

#ifndef __ICreateDevEnum_INTERFACE_DEFINED__
#define __ICreateDevEnum_INTERFACE_DEFINED__

/* interface ICreateDevEnum */
/* [unique][uuid][object][local] */ 


EXTERN_C const IID IID_ICreateDevEnum;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("29840822-5B84-11D0-BD3B-00A0C911CE86")
    ICreateDevEnum : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE CreateClassEnumerator( 
            /* [in] */ REFCLSID clsidDeviceClass,
            /* [out] */ 
            __out  IEnumMoniker **ppEnumMoniker,
            /* [in] */ DWORD dwFlags) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ICreateDevEnumVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ICreateDevEnum * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ICreateDevEnum * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ICreateDevEnum * This);
        
        HRESULT ( STDMETHODCALLTYPE *CreateClassEnumerator )( 
            ICreateDevEnum * This,
            /* [in] */ REFCLSID clsidDeviceClass,
            /* [out] */ 
            __out  IEnumMoniker **ppEnumMoniker,
            /* [in] */ DWORD dwFlags);
        
        END_INTERFACE
    } ICreateDevEnumVtbl;

    interface ICreateDevEnum
    {
        CONST_VTBL struct ICreateDevEnumVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICreateDevEnum_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ICreateDevEnum_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ICreateDevEnum_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ICreateDevEnum_CreateClassEnumerator(This,clsidDeviceClass,ppEnumMoniker,dwFlags)	\
    ( (This)->lpVtbl -> CreateClassEnumerator(This,clsidDeviceClass,ppEnumMoniker,dwFlags) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ICreateDevEnum_INTERFACE_DEFINED__ */


#ifndef __IFilterGraph_INTERFACE_DEFINED__
#define __IFilterGraph_INTERFACE_DEFINED__

/* interface IFilterGraph */
/* [unique][uuid][object][local] */ 


EXTERN_C const IID IID_IFilterGraph;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("56a8689f-0ad4-11ce-b03a-0020af0ba770")
    IFilterGraph : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE AddFilter( 
            /* [in] */ IBaseFilter *pFilter,
            /* [string][in] */ LPCWSTR pName) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RemoveFilter( 
            /* [in] */ IBaseFilter *pFilter) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EnumFilters( 
            /* [out] */ 
            __out  IEnumFilters **ppEnum) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE FindFilterByName( 
            /* [string][in] */ LPCWSTR pName,
            /* [out] */ 
            __out  IBaseFilter **ppFilter) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ConnectDirect( 
            /* [in] */ IPin *ppinOut,
            /* [in] */ IPin *ppinIn,
            /* [unique][in] */ 
            __in_opt  const AM_MEDIA_TYPE *pmt) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Reconnect( 
            /* [in] */ IPin *ppin) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Disconnect( 
            /* [in] */ IPin *ppin) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetDefaultSyncSource( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IFilterGraphVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IFilterGraph * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IFilterGraph * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IFilterGraph * This);
        
        HRESULT ( STDMETHODCALLTYPE *AddFilter )( 
            IFilterGraph * This,
            /* [in] */ IBaseFilter *pFilter,
            /* [string][in] */ LPCWSTR pName);
        
        HRESULT ( STDMETHODCALLTYPE *RemoveFilter )( 
            IFilterGraph * This,
            /* [in] */ IBaseFilter *pFilter);
        
        HRESULT ( STDMETHODCALLTYPE *EnumFilters )( 
            IFilterGraph * This,
            /* [out] */ 
            __out  IEnumFilters **ppEnum);
        
        HRESULT ( STDMETHODCALLTYPE *FindFilterByName )( 
            IFilterGraph * This,
            /* [string][in] */ LPCWSTR pName,
            /* [out] */ 
            __out  IBaseFilter **ppFilter);
        
        HRESULT ( STDMETHODCALLTYPE *ConnectDirect )( 
            IFilterGraph * This,
            /* [in] */ IPin *ppinOut,
            /* [in] */ IPin *ppinIn,
            /* [unique][in] */ 
            __in_opt  const AM_MEDIA_TYPE *pmt);
        
        HRESULT ( STDMETHODCALLTYPE *Reconnect )( 
            IFilterGraph * This,
            /* [in] */ IPin *ppin);
        
        HRESULT ( STDMETHODCALLTYPE *Disconnect )( 
            IFilterGraph * This,
            /* [in] */ IPin *ppin);
        
        HRESULT ( STDMETHODCALLTYPE *SetDefaultSyncSource )( 
            IFilterGraph * This);
        
        END_INTERFACE
    } IFilterGraphVtbl;

    interface IFilterGraph
    {
        CONST_VTBL struct IFilterGraphVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IFilterGraph_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IFilterGraph_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IFilterGraph_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IFilterGraph_AddFilter(This,pFilter,pName)	\
    ( (This)->lpVtbl -> AddFilter(This,pFilter,pName) ) 

#define IFilterGraph_RemoveFilter(This,pFilter)	\
    ( (This)->lpVtbl -> RemoveFilter(This,pFilter) ) 

#define IFilterGraph_EnumFilters(This,ppEnum)	\
    ( (This)->lpVtbl -> EnumFilters(This,ppEnum) ) 

#define IFilterGraph_FindFilterByName(This,pName,ppFilter)	\
    ( (This)->lpVtbl -> FindFilterByName(This,pName,ppFilter) ) 

#define IFilterGraph_ConnectDirect(This,ppinOut,ppinIn,pmt)	\
    ( (This)->lpVtbl -> ConnectDirect(This,ppinOut,ppinIn,pmt) ) 

#define IFilterGraph_Reconnect(This,ppin)	\
    ( (This)->lpVtbl -> Reconnect(This,ppin) ) 

#define IFilterGraph_Disconnect(This,ppin)	\
    ( (This)->lpVtbl -> Disconnect(This,ppin) ) 

#define IFilterGraph_SetDefaultSyncSource(This)	\
    ( (This)->lpVtbl -> SetDefaultSyncSource(This) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IFilterGraph_INTERFACE_DEFINED__ */


#ifndef __IGraphBuilder_INTERFACE_DEFINED__
#define __IGraphBuilder_INTERFACE_DEFINED__

/* interface IGraphBuilder */
/* [unique][uuid][object][local] */ 


EXTERN_C const IID IID_IGraphBuilder;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("56a868a9-0ad4-11ce-b03a-0020af0ba770")
    IGraphBuilder : public IFilterGraph
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Connect( 
            /* [in] */ IPin *ppinOut,
            /* [in] */ IPin *ppinIn) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Render( 
            /* [in] */ IPin *ppinOut) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RenderFile( 
            /* [in] */ LPCWSTR lpcwstrFile,
            /* [unique][in] */ 
            __in_opt  LPCWSTR lpcwstrPlayList) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE AddSourceFilter( 
            /* [in] */ LPCWSTR lpcwstrFileName,
            /* [unique][in] */ 
            __in_opt  LPCWSTR lpcwstrFilterName,
            /* [out] */ 
            __out  IBaseFilter **ppFilter) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetLogFile( 
            /* [in] */ DWORD_PTR hFile) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Abort( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ShouldOperationContinue( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IGraphBuilderVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IGraphBuilder * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IGraphBuilder * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IGraphBuilder * This);
        
        HRESULT ( STDMETHODCALLTYPE *AddFilter )( 
            IGraphBuilder * This,
            /* [in] */ IBaseFilter *pFilter,
            /* [string][in] */ LPCWSTR pName);
        
        HRESULT ( STDMETHODCALLTYPE *RemoveFilter )( 
            IGraphBuilder * This,
            /* [in] */ IBaseFilter *pFilter);
        
        HRESULT ( STDMETHODCALLTYPE *EnumFilters )( 
            IGraphBuilder * This,
            /* [out] */ 
            __out  IEnumFilters **ppEnum);
        
        HRESULT ( STDMETHODCALLTYPE *FindFilterByName )( 
            IGraphBuilder * This,
            /* [string][in] */ LPCWSTR pName,
            /* [out] */ 
            __out  IBaseFilter **ppFilter);
        
        HRESULT ( STDMETHODCALLTYPE *ConnectDirect )( 
            IGraphBuilder * This,
            /* [in] */ IPin *ppinOut,
            /* [in] */ IPin *ppinIn,
            /* [unique][in] */ 
            __in_opt  const AM_MEDIA_TYPE *pmt);
        
        HRESULT ( STDMETHODCALLTYPE *Reconnect )( 
            IGraphBuilder * This,
            /* [in] */ IPin *ppin);
        
        HRESULT ( STDMETHODCALLTYPE *Disconnect )( 
            IGraphBuilder * This,
            /* [in] */ IPin *ppin);
        
        HRESULT ( STDMETHODCALLTYPE *SetDefaultSyncSource )( 
            IGraphBuilder * This);
        
        HRESULT ( STDMETHODCALLTYPE *Connect )( 
            IGraphBuilder * This,
            /* [in] */ IPin *ppinOut,
            /* [in] */ IPin *ppinIn);
        
        HRESULT ( STDMETHODCALLTYPE *Render )( 
            IGraphBuilder * This,
            /* [in] */ IPin *ppinOut);
        
        HRESULT ( STDMETHODCALLTYPE *RenderFile )( 
            IGraphBuilder * This,
            /* [in] */ LPCWSTR lpcwstrFile,
            /* [unique][in] */ 
            __in_opt  LPCWSTR lpcwstrPlayList);
        
        HRESULT ( STDMETHODCALLTYPE *AddSourceFilter )( 
            IGraphBuilder * This,
            /* [in] */ LPCWSTR lpcwstrFileName,
            /* [unique][in] */ 
            __in_opt  LPCWSTR lpcwstrFilterName,
            /* [out] */ 
            __out  IBaseFilter **ppFilter);
        
        HRESULT ( STDMETHODCALLTYPE *SetLogFile )( 
            IGraphBuilder * This,
            /* [in] */ DWORD_PTR hFile);
        
        HRESULT ( STDMETHODCALLTYPE *Abort )( 
            IGraphBuilder * This);
        
        HRESULT ( STDMETHODCALLTYPE *ShouldOperationContinue )( 
            IGraphBuilder * This);
        
        END_INTERFACE
    } IGraphBuilderVtbl;

    interface IGraphBuilder
    {
        CONST_VTBL struct IGraphBuilderVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IGraphBuilder_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IGraphBuilder_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IGraphBuilder_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IGraphBuilder_AddFilter(This,pFilter,pName)	\
    ( (This)->lpVtbl -> AddFilter(This,pFilter,pName) ) 

#define IGraphBuilder_RemoveFilter(This,pFilter)	\
    ( (This)->lpVtbl -> RemoveFilter(This,pFilter) ) 

#define IGraphBuilder_EnumFilters(This,ppEnum)	\
    ( (This)->lpVtbl -> EnumFilters(This,ppEnum) ) 

#define IGraphBuilder_FindFilterByName(This,pName,ppFilter)	\
    ( (This)->lpVtbl -> FindFilterByName(This,pName,ppFilter) ) 

#define IGraphBuilder_ConnectDirect(This,ppinOut,ppinIn,pmt)	\
    ( (This)->lpVtbl -> ConnectDirect(This,ppinOut,ppinIn,pmt) ) 

#define IGraphBuilder_Reconnect(This,ppin)	\
    ( (This)->lpVtbl -> Reconnect(This,ppin) ) 

#define IGraphBuilder_Disconnect(This,ppin)	\
    ( (This)->lpVtbl -> Disconnect(This,ppin) ) 

#define IGraphBuilder_SetDefaultSyncSource(This)	\
    ( (This)->lpVtbl -> SetDefaultSyncSource(This) ) 


#define IGraphBuilder_Connect(This,ppinOut,ppinIn)	\
    ( (This)->lpVtbl -> Connect(This,ppinOut,ppinIn) ) 

#define IGraphBuilder_Render(This,ppinOut)	\
    ( (This)->lpVtbl -> Render(This,ppinOut) ) 

#define IGraphBuilder_RenderFile(This,lpcwstrFile,lpcwstrPlayList)	\
    ( (This)->lpVtbl -> RenderFile(This,lpcwstrFile,lpcwstrPlayList) ) 

#define IGraphBuilder_AddSourceFilter(This,lpcwstrFileName,lpcwstrFilterName,ppFilter)	\
    ( (This)->lpVtbl -> AddSourceFilter(This,lpcwstrFileName,lpcwstrFilterName,ppFilter) ) 

#define IGraphBuilder_SetLogFile(This,hFile)	\
    ( (This)->lpVtbl -> SetLogFile(This,hFile) ) 

#define IGraphBuilder_Abort(This)	\
    ( (This)->lpVtbl -> Abort(This) ) 

#define IGraphBuilder_ShouldOperationContinue(This)	\
    ( (This)->lpVtbl -> ShouldOperationContinue(This) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IGraphBuilder_INTERFACE_DEFINED__ */


#ifndef __ICaptureGraphBuilder2_INTERFACE_DEFINED__
#define __ICaptureGraphBuilder2_INTERFACE_DEFINED__

/* interface ICaptureGraphBuilder2 */
/* [unique][uuid][object][local] */ 


EXTERN_C const IID IID_ICaptureGraphBuilder2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("93E5A4E0-2D50-11d2-ABFA-00A0C9C6E38D")
    ICaptureGraphBuilder2 : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE SetFiltergraph( 
            /* [in] */ IGraphBuilder *pfg) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetFiltergraph( 
            /* [out] */ 
            __out  IGraphBuilder **ppfg) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetOutputFileName( 
            /* [in] */ const GUID *pType,
            /* [in] */ LPCOLESTR lpstrFile,
            /* [out] */ 
            __out  IBaseFilter **ppf,
            /* [out] */ 
            __out  IFileSinkFilter **ppSink) = 0;
        
        virtual /* [local] */ HRESULT STDMETHODCALLTYPE FindInterface( 
            /* [in] */ 
            __in_opt  const GUID *pCategory,
            /* [in] */ 
            __in_opt  const GUID *pType,
            /* [in] */ IBaseFilter *pf,
            /* [in] */ REFIID riid,
            /* [out] */ 
            __out  void **ppint) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RenderStream( 
            /* [in] */ 
            __in_opt  const GUID *pCategory,
            /* [in] */ const GUID *pType,
            /* [in] */ IUnknown *pSource,
            /* [in] */ IBaseFilter *pfCompressor,
            /* [in] */ IBaseFilter *pfRenderer) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ControlStream( 
            /* [in] */ const GUID *pCategory,
            /* [in] */ const GUID *pType,
            /* [in] */ IBaseFilter *pFilter,
            /* [in] */ 
            __in_opt  REFERENCE_TIME *pstart,
            /* [in] */ 
            __in_opt  REFERENCE_TIME *pstop,
            /* [in] */ WORD wStartCookie,
            /* [in] */ WORD wStopCookie) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE AllocCapFile( 
            /* [in] */ LPCOLESTR lpstr,
            /* [in] */ DWORDLONG dwlSize) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CopyCaptureFile( 
            /* [in] */ 
            __in  LPOLESTR lpwstrOld,
            /* [in] */ 
            __in  LPOLESTR lpwstrNew,
            /* [in] */ int fAllowEscAbort,
            /* [in] */ IAMCopyCaptureFileProgress *pCallback) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE FindPin( 
            /* [in] */ IUnknown *pSource,
            /* [in] */ PIN_DIRECTION pindir,
            /* [in] */ 
            __in_opt  const GUID *pCategory,
            /* [in] */ 
            __in_opt  const GUID *pType,
            /* [in] */ BOOL fUnconnected,
            /* [in] */ int num,
            /* [out] */ 
            __out  IPin **ppPin) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ICaptureGraphBuilder2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ICaptureGraphBuilder2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ICaptureGraphBuilder2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ICaptureGraphBuilder2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *SetFiltergraph )( 
            ICaptureGraphBuilder2 * This,
            /* [in] */ IGraphBuilder *pfg);
        
        HRESULT ( STDMETHODCALLTYPE *GetFiltergraph )( 
            ICaptureGraphBuilder2 * This,
            /* [out] */ 
            __out  IGraphBuilder **ppfg);
        
        HRESULT ( STDMETHODCALLTYPE *SetOutputFileName )( 
            ICaptureGraphBuilder2 * This,
            /* [in] */ const GUID *pType,
            /* [in] */ LPCOLESTR lpstrFile,
            /* [out] */ 
            __out  IBaseFilter **ppf,
            /* [out] */ 
            __out  IFileSinkFilter **ppSink);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *FindInterface )( 
            ICaptureGraphBuilder2 * This,
            /* [in] */ 
            __in_opt  const GUID *pCategory,
            /* [in] */ 
            __in_opt  const GUID *pType,
            /* [in] */ IBaseFilter *pf,
            /* [in] */ REFIID riid,
            /* [out] */ 
            __out  void **ppint);
        
        HRESULT ( STDMETHODCALLTYPE *RenderStream )( 
            ICaptureGraphBuilder2 * This,
            /* [in] */ 
            __in_opt  const GUID *pCategory,
            /* [in] */ const GUID *pType,
            /* [in] */ IUnknown *pSource,
            /* [in] */ IBaseFilter *pfCompressor,
            /* [in] */ IBaseFilter *pfRenderer);
        
        HRESULT ( STDMETHODCALLTYPE *ControlStream )( 
            ICaptureGraphBuilder2 * This,
            /* [in] */ const GUID *pCategory,
            /* [in] */ const GUID *pType,
            /* [in] */ IBaseFilter *pFilter,
            /* [in] */ 
            __in_opt  REFERENCE_TIME *pstart,
            /* [in] */ 
            __in_opt  REFERENCE_TIME *pstop,
            /* [in] */ WORD wStartCookie,
            /* [in] */ WORD wStopCookie);
        
        HRESULT ( STDMETHODCALLTYPE *AllocCapFile )( 
            ICaptureGraphBuilder2 * This,
            /* [in] */ LPCOLESTR lpstr,
            /* [in] */ DWORDLONG dwlSize);
        
        HRESULT ( STDMETHODCALLTYPE *CopyCaptureFile )( 
            ICaptureGraphBuilder2 * This,
            /* [in] */ 
            __in  LPOLESTR lpwstrOld,
            /* [in] */ 
            __in  LPOLESTR lpwstrNew,
            /* [in] */ int fAllowEscAbort,
            /* [in] */ IAMCopyCaptureFileProgress *pCallback);
        
        HRESULT ( STDMETHODCALLTYPE *FindPin )( 
            ICaptureGraphBuilder2 * This,
            /* [in] */ IUnknown *pSource,
            /* [in] */ PIN_DIRECTION pindir,
            /* [in] */ 
            __in_opt  const GUID *pCategory,
            /* [in] */ 
            __in_opt  const GUID *pType,
            /* [in] */ BOOL fUnconnected,
            /* [in] */ int num,
            /* [out] */ 
            __out  IPin **ppPin);
        
        END_INTERFACE
    } ICaptureGraphBuilder2Vtbl;

    interface ICaptureGraphBuilder2
    {
        CONST_VTBL struct ICaptureGraphBuilder2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICaptureGraphBuilder2_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ICaptureGraphBuilder2_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ICaptureGraphBuilder2_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ICaptureGraphBuilder2_SetFiltergraph(This,pfg)	\
    ( (This)->lpVtbl -> SetFiltergraph(This,pfg) ) 

#define ICaptureGraphBuilder2_GetFiltergraph(This,ppfg)	\
    ( (This)->lpVtbl -> GetFiltergraph(This,ppfg) ) 

#define ICaptureGraphBuilder2_SetOutputFileName(This,pType,lpstrFile,ppf,ppSink)	\
    ( (This)->lpVtbl -> SetOutputFileName(This,pType,lpstrFile,ppf,ppSink) ) 

#define ICaptureGraphBuilder2_FindInterface(This,pCategory,pType,pf,riid,ppint)	\
    ( (This)->lpVtbl -> FindInterface(This,pCategory,pType,pf,riid,ppint) ) 

#define ICaptureGraphBuilder2_RenderStream(This,pCategory,pType,pSource,pfCompressor,pfRenderer)	\
    ( (This)->lpVtbl -> RenderStream(This,pCategory,pType,pSource,pfCompressor,pfRenderer) ) 

#define ICaptureGraphBuilder2_ControlStream(This,pCategory,pType,pFilter,pstart,pstop,wStartCookie,wStopCookie)	\
    ( (This)->lpVtbl -> ControlStream(This,pCategory,pType,pFilter,pstart,pstop,wStartCookie,wStopCookie) ) 

#define ICaptureGraphBuilder2_AllocCapFile(This,lpstr,dwlSize)	\
    ( (This)->lpVtbl -> AllocCapFile(This,lpstr,dwlSize) ) 

#define ICaptureGraphBuilder2_CopyCaptureFile(This,lpwstrOld,lpwstrNew,fAllowEscAbort,pCallback)	\
    ( (This)->lpVtbl -> CopyCaptureFile(This,lpwstrOld,lpwstrNew,fAllowEscAbort,pCallback) ) 

#define ICaptureGraphBuilder2_FindPin(This,pSource,pindir,pCategory,pType,fUnconnected,num,ppPin)	\
    ( (This)->lpVtbl -> FindPin(This,pSource,pindir,pCategory,pType,fUnconnected,num,ppPin) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [call_as] */ HRESULT STDMETHODCALLTYPE ICaptureGraphBuilder2_RemoteFindInterface_Proxy( 
    ICaptureGraphBuilder2 * This,
    /* [in] */ 
    __in_opt  const GUID *pCategory,
    /* [in] */ 
    __in_opt  const GUID *pType,
    /* [in] */ IBaseFilter *pf,
    /* [in] */ REFIID riid,
    /* [out] */ 
    __out  IUnknown **ppint);


void __RPC_STUB ICaptureGraphBuilder2_RemoteFindInterface_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ICaptureGraphBuilder2_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_DirectShowInterfaces_0000_0005 */
/* [local] */ 


enum _AM_RENSDEREXFLAGS
    {	AM_RENDEREX_RENDERTOEXISTINGRENDERERS	= 0x1
    } ;


extern RPC_IF_HANDLE __MIDL_itf_DirectShowInterfaces_0000_0005_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_DirectShowInterfaces_0000_0005_v0_0_s_ifspec;

/* interface __MIDL_itf_DirectShowInterfaces_0000_0006 */
/* [local] */ 

typedef IFilterGraph *PFILTERGRAPH;



extern RPC_IF_HANDLE __MIDL_itf_DirectShowInterfaces_0000_0006_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_DirectShowInterfaces_0000_0006_v0_0_s_ifspec;

#ifndef __IMediaFilter_INTERFACE_DEFINED__
#define __IMediaFilter_INTERFACE_DEFINED__

/* interface IMediaFilter */
/* [unique][uuid][object][local] */ 

typedef 
enum _FilterState
    {	State_Stopped	= 0,
	State_Paused	= ( State_Stopped + 1 ) ,
	State_Running	= ( State_Paused + 1 ) 
    } 	FILTER_STATE;


EXTERN_C const IID IID_IMediaFilter;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("56a86899-0ad4-11ce-b03a-0020af0ba770")
    IMediaFilter : public IPersist
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Stop( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Pause( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Run( 
            REFERENCE_TIME tStart) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetState( 
            /* [in] */ DWORD dwMilliSecsTimeout,
            /* [out] */ 
            __out  FILTER_STATE *State) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetSyncSource( 
            /* [in] */ 
            __in_opt  IReferenceClock *pClock) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetSyncSource( 
            /* [out] */ 
            __deref_out_opt  IReferenceClock **pClock) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IMediaFilterVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IMediaFilter * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IMediaFilter * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IMediaFilter * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetClassID )( 
            IMediaFilter * This,
            /* [out] */ CLSID *pClassID);
        
        HRESULT ( STDMETHODCALLTYPE *Stop )( 
            IMediaFilter * This);
        
        HRESULT ( STDMETHODCALLTYPE *Pause )( 
            IMediaFilter * This);
        
        HRESULT ( STDMETHODCALLTYPE *Run )( 
            IMediaFilter * This,
            REFERENCE_TIME tStart);
        
        HRESULT ( STDMETHODCALLTYPE *GetState )( 
            IMediaFilter * This,
            /* [in] */ DWORD dwMilliSecsTimeout,
            /* [out] */ 
            __out  FILTER_STATE *State);
        
        HRESULT ( STDMETHODCALLTYPE *SetSyncSource )( 
            IMediaFilter * This,
            /* [in] */ 
            __in_opt  IReferenceClock *pClock);
        
        HRESULT ( STDMETHODCALLTYPE *GetSyncSource )( 
            IMediaFilter * This,
            /* [out] */ 
            __deref_out_opt  IReferenceClock **pClock);
        
        END_INTERFACE
    } IMediaFilterVtbl;

    interface IMediaFilter
    {
        CONST_VTBL struct IMediaFilterVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMediaFilter_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IMediaFilter_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IMediaFilter_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IMediaFilter_GetClassID(This,pClassID)	\
    ( (This)->lpVtbl -> GetClassID(This,pClassID) ) 


#define IMediaFilter_Stop(This)	\
    ( (This)->lpVtbl -> Stop(This) ) 

#define IMediaFilter_Pause(This)	\
    ( (This)->lpVtbl -> Pause(This) ) 

#define IMediaFilter_Run(This,tStart)	\
    ( (This)->lpVtbl -> Run(This,tStart) ) 

#define IMediaFilter_GetState(This,dwMilliSecsTimeout,State)	\
    ( (This)->lpVtbl -> GetState(This,dwMilliSecsTimeout,State) ) 

#define IMediaFilter_SetSyncSource(This,pClock)	\
    ( (This)->lpVtbl -> SetSyncSource(This,pClock) ) 

#define IMediaFilter_GetSyncSource(This,pClock)	\
    ( (This)->lpVtbl -> GetSyncSource(This,pClock) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IMediaFilter_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_DirectShowInterfaces_0000_0007 */
/* [local] */ 

typedef IMediaFilter *PMEDIAFILTER;



extern RPC_IF_HANDLE __MIDL_itf_DirectShowInterfaces_0000_0007_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_DirectShowInterfaces_0000_0007_v0_0_s_ifspec;

#ifndef __IBaseFilter_INTERFACE_DEFINED__
#define __IBaseFilter_INTERFACE_DEFINED__

/* interface IBaseFilter */
/* [unique][uuid][object][local] */ 

typedef struct _FilterInfo
    {
    WCHAR achName[ 128 ];
    IFilterGraph *pGraph;
    } 	FILTER_INFO;


EXTERN_C const IID IID_IBaseFilter;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("56a86895-0ad4-11ce-b03a-0020af0ba770")
    IBaseFilter : public IMediaFilter
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE EnumPins( 
            /* [out] */ 
            __out  IEnumPins **ppEnum) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE FindPin( 
            /* [string][in] */ LPCWSTR Id,
            /* [out] */ 
            __out  IPin **ppPin) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE QueryFilterInfo( 
            /* [out] */ 
            __out  FILTER_INFO *pInfo) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE JoinFilterGraph( 
            /* [in] */ 
            __in_opt  IFilterGraph *pGraph,
            /* [string][in] */ 
            __in_opt  LPCWSTR pName) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE QueryVendorInfo( 
            /* [string][out] */ 
            __out  LPWSTR *pVendorInfo) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IBaseFilterVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IBaseFilter * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IBaseFilter * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IBaseFilter * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetClassID )( 
            IBaseFilter * This,
            /* [out] */ CLSID *pClassID);
        
        HRESULT ( STDMETHODCALLTYPE *Stop )( 
            IBaseFilter * This);
        
        HRESULT ( STDMETHODCALLTYPE *Pause )( 
            IBaseFilter * This);
        
        HRESULT ( STDMETHODCALLTYPE *Run )( 
            IBaseFilter * This,
            REFERENCE_TIME tStart);
        
        HRESULT ( STDMETHODCALLTYPE *GetState )( 
            IBaseFilter * This,
            /* [in] */ DWORD dwMilliSecsTimeout,
            /* [out] */ 
            __out  FILTER_STATE *State);
        
        HRESULT ( STDMETHODCALLTYPE *SetSyncSource )( 
            IBaseFilter * This,
            /* [in] */ 
            __in_opt  IReferenceClock *pClock);
        
        HRESULT ( STDMETHODCALLTYPE *GetSyncSource )( 
            IBaseFilter * This,
            /* [out] */ 
            __deref_out_opt  IReferenceClock **pClock);
        
        HRESULT ( STDMETHODCALLTYPE *EnumPins )( 
            IBaseFilter * This,
            /* [out] */ 
            __out  IEnumPins **ppEnum);
        
        HRESULT ( STDMETHODCALLTYPE *FindPin )( 
            IBaseFilter * This,
            /* [string][in] */ LPCWSTR Id,
            /* [out] */ 
            __out  IPin **ppPin);
        
        HRESULT ( STDMETHODCALLTYPE *QueryFilterInfo )( 
            IBaseFilter * This,
            /* [out] */ 
            __out  FILTER_INFO *pInfo);
        
        HRESULT ( STDMETHODCALLTYPE *JoinFilterGraph )( 
            IBaseFilter * This,
            /* [in] */ 
            __in_opt  IFilterGraph *pGraph,
            /* [string][in] */ 
            __in_opt  LPCWSTR pName);
        
        HRESULT ( STDMETHODCALLTYPE *QueryVendorInfo )( 
            IBaseFilter * This,
            /* [string][out] */ 
            __out  LPWSTR *pVendorInfo);
        
        END_INTERFACE
    } IBaseFilterVtbl;

    interface IBaseFilter
    {
        CONST_VTBL struct IBaseFilterVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IBaseFilter_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IBaseFilter_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IBaseFilter_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IBaseFilter_GetClassID(This,pClassID)	\
    ( (This)->lpVtbl -> GetClassID(This,pClassID) ) 


#define IBaseFilter_Stop(This)	\
    ( (This)->lpVtbl -> Stop(This) ) 

#define IBaseFilter_Pause(This)	\
    ( (This)->lpVtbl -> Pause(This) ) 

#define IBaseFilter_Run(This,tStart)	\
    ( (This)->lpVtbl -> Run(This,tStart) ) 

#define IBaseFilter_GetState(This,dwMilliSecsTimeout,State)	\
    ( (This)->lpVtbl -> GetState(This,dwMilliSecsTimeout,State) ) 

#define IBaseFilter_SetSyncSource(This,pClock)	\
    ( (This)->lpVtbl -> SetSyncSource(This,pClock) ) 

#define IBaseFilter_GetSyncSource(This,pClock)	\
    ( (This)->lpVtbl -> GetSyncSource(This,pClock) ) 


#define IBaseFilter_EnumPins(This,ppEnum)	\
    ( (This)->lpVtbl -> EnumPins(This,ppEnum) ) 

#define IBaseFilter_FindPin(This,Id,ppPin)	\
    ( (This)->lpVtbl -> FindPin(This,Id,ppPin) ) 

#define IBaseFilter_QueryFilterInfo(This,pInfo)	\
    ( (This)->lpVtbl -> QueryFilterInfo(This,pInfo) ) 

#define IBaseFilter_JoinFilterGraph(This,pGraph,pName)	\
    ( (This)->lpVtbl -> JoinFilterGraph(This,pGraph,pName) ) 

#define IBaseFilter_QueryVendorInfo(This,pVendorInfo)	\
    ( (This)->lpVtbl -> QueryVendorInfo(This,pVendorInfo) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IBaseFilter_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_DirectShowInterfaces_0000_0008 */
/* [local] */ 

typedef IBaseFilter *PFILTER;



extern RPC_IF_HANDLE __MIDL_itf_DirectShowInterfaces_0000_0008_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_DirectShowInterfaces_0000_0008_v0_0_s_ifspec;

#ifndef __IAMStreamConfig_INTERFACE_DEFINED__
#define __IAMStreamConfig_INTERFACE_DEFINED__

/* interface IAMStreamConfig */
/* [unique][uuid][object][local] */ 

typedef struct _VIDEO_STREAM_CONFIG_CAPS
    {
    GUID guid;
    ULONG VideoStandard;
    SIZE InputSize;
    SIZE MinCroppingSize;
    SIZE MaxCroppingSize;
    int CropGranularityX;
    int CropGranularityY;
    int CropAlignX;
    int CropAlignY;
    SIZE MinOutputSize;
    SIZE MaxOutputSize;
    int OutputGranularityX;
    int OutputGranularityY;
    int StretchTapsX;
    int StretchTapsY;
    int ShrinkTapsX;
    int ShrinkTapsY;
    LONGLONG MinFrameInterval;
    LONGLONG MaxFrameInterval;
    LONG MinBitsPerSecond;
    LONG MaxBitsPerSecond;
    } 	VIDEO_STREAM_CONFIG_CAPS;

typedef struct _AUDIO_STREAM_CONFIG_CAPS
    {
    GUID guid;
    ULONG MinimumChannels;
    ULONG MaximumChannels;
    ULONG ChannelsGranularity;
    ULONG MinimumBitsPerSample;
    ULONG MaximumBitsPerSample;
    ULONG BitsPerSampleGranularity;
    ULONG MinimumSampleFrequency;
    ULONG MaximumSampleFrequency;
    ULONG SampleFrequencyGranularity;
    } 	AUDIO_STREAM_CONFIG_CAPS;


EXTERN_C const IID IID_IAMStreamConfig;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("C6E13340-30AC-11d0-A18C-00A0C9118956")
    IAMStreamConfig : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE SetFormat( 
            /* [in] */ AM_MEDIA_TYPE *pmt) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetFormat( 
            /* [out] */ 
            __out  AM_MEDIA_TYPE **ppmt) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetNumberOfCapabilities( 
            /* [out] */ 
            __out  int *piCount,
            /* [out] */ 
            __out  int *piSize) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetStreamCaps( 
            /* [in] */ int iIndex,
            /* [out] */ 
            __out  AM_MEDIA_TYPE **ppmt,
            /* [out] */ 
            __out  BYTE *pSCC) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IAMStreamConfigVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IAMStreamConfig * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IAMStreamConfig * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IAMStreamConfig * This);
        
        HRESULT ( STDMETHODCALLTYPE *SetFormat )( 
            IAMStreamConfig * This,
            /* [in] */ AM_MEDIA_TYPE *pmt);
        
        HRESULT ( STDMETHODCALLTYPE *GetFormat )( 
            IAMStreamConfig * This,
            /* [out] */ 
            __out  AM_MEDIA_TYPE **ppmt);
        
        HRESULT ( STDMETHODCALLTYPE *GetNumberOfCapabilities )( 
            IAMStreamConfig * This,
            /* [out] */ 
            __out  int *piCount,
            /* [out] */ 
            __out  int *piSize);
        
        HRESULT ( STDMETHODCALLTYPE *GetStreamCaps )( 
            IAMStreamConfig * This,
            /* [in] */ int iIndex,
            /* [out] */ 
            __out  AM_MEDIA_TYPE **ppmt,
            /* [out] */ 
            __out  BYTE *pSCC);
        
        END_INTERFACE
    } IAMStreamConfigVtbl;

    interface IAMStreamConfig
    {
        CONST_VTBL struct IAMStreamConfigVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IAMStreamConfig_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IAMStreamConfig_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IAMStreamConfig_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IAMStreamConfig_SetFormat(This,pmt)	\
    ( (This)->lpVtbl -> SetFormat(This,pmt) ) 

#define IAMStreamConfig_GetFormat(This,ppmt)	\
    ( (This)->lpVtbl -> GetFormat(This,ppmt) ) 

#define IAMStreamConfig_GetNumberOfCapabilities(This,piCount,piSize)	\
    ( (This)->lpVtbl -> GetNumberOfCapabilities(This,piCount,piSize) ) 

#define IAMStreamConfig_GetStreamCaps(This,iIndex,ppmt,pSCC)	\
    ( (This)->lpVtbl -> GetStreamCaps(This,iIndex,ppmt,pSCC) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IAMStreamConfig_INTERFACE_DEFINED__ */


#ifndef __ISampleGrabberCB_INTERFACE_DEFINED__
#define __ISampleGrabberCB_INTERFACE_DEFINED__

/* interface ISampleGrabberCB */
/* [unique][helpstring][local][uuid][object] */ 


EXTERN_C const IID IID_ISampleGrabberCB;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("0579154A-2B53-4994-B0D0-E773148EFF85")
    ISampleGrabberCB : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE SampleCB( 
            double SampleTime,
            IMediaSample *pSample) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE BufferCB( 
            double SampleTime,
            BYTE *pBuffer,
            long BufferLen) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ISampleGrabberCBVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ISampleGrabberCB * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ISampleGrabberCB * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ISampleGrabberCB * This);
        
        HRESULT ( STDMETHODCALLTYPE *SampleCB )( 
            ISampleGrabberCB * This,
            double SampleTime,
            IMediaSample *pSample);
        
        HRESULT ( STDMETHODCALLTYPE *BufferCB )( 
            ISampleGrabberCB * This,
            double SampleTime,
            BYTE *pBuffer,
            long BufferLen);
        
        END_INTERFACE
    } ISampleGrabberCBVtbl;

    interface ISampleGrabberCB
    {
        CONST_VTBL struct ISampleGrabberCBVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISampleGrabberCB_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ISampleGrabberCB_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ISampleGrabberCB_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ISampleGrabberCB_SampleCB(This,SampleTime,pSample)	\
    ( (This)->lpVtbl -> SampleCB(This,SampleTime,pSample) ) 

#define ISampleGrabberCB_BufferCB(This,SampleTime,pBuffer,BufferLen)	\
    ( (This)->lpVtbl -> BufferCB(This,SampleTime,pBuffer,BufferLen) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ISampleGrabberCB_INTERFACE_DEFINED__ */


#ifndef __ISampleGrabber_INTERFACE_DEFINED__
#define __ISampleGrabber_INTERFACE_DEFINED__

/* interface ISampleGrabber */
/* [unique][helpstring][local][uuid][object] */ 


EXTERN_C const IID IID_ISampleGrabber;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("6B652FFF-11FE-4fce-92AD-0266B5D7C78F")
    ISampleGrabber : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE SetOneShot( 
            BOOL OneShot) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetMediaType( 
            const AM_MEDIA_TYPE *pType) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetConnectedMediaType( 
            AM_MEDIA_TYPE *pType) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetBufferSamples( 
            BOOL BufferThem) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetCurrentBuffer( 
            /* [out][in] */ long *pBufferSize,
            /* [out] */ long *pBuffer) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetCurrentSample( 
            /* [retval][out] */ IMediaSample **ppSample) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetCallback( 
            ISampleGrabberCB *pCallback,
            long WhichMethodToCallback) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ISampleGrabberVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ISampleGrabber * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ISampleGrabber * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ISampleGrabber * This);
        
        HRESULT ( STDMETHODCALLTYPE *SetOneShot )( 
            ISampleGrabber * This,
            BOOL OneShot);
        
        HRESULT ( STDMETHODCALLTYPE *SetMediaType )( 
            ISampleGrabber * This,
            const AM_MEDIA_TYPE *pType);
        
        HRESULT ( STDMETHODCALLTYPE *GetConnectedMediaType )( 
            ISampleGrabber * This,
            AM_MEDIA_TYPE *pType);
        
        HRESULT ( STDMETHODCALLTYPE *SetBufferSamples )( 
            ISampleGrabber * This,
            BOOL BufferThem);
        
        HRESULT ( STDMETHODCALLTYPE *GetCurrentBuffer )( 
            ISampleGrabber * This,
            /* [out][in] */ long *pBufferSize,
            /* [out] */ long *pBuffer);
        
        HRESULT ( STDMETHODCALLTYPE *GetCurrentSample )( 
            ISampleGrabber * This,
            /* [retval][out] */ IMediaSample **ppSample);
        
        HRESULT ( STDMETHODCALLTYPE *SetCallback )( 
            ISampleGrabber * This,
            ISampleGrabberCB *pCallback,
            long WhichMethodToCallback);
        
        END_INTERFACE
    } ISampleGrabberVtbl;

    interface ISampleGrabber
    {
        CONST_VTBL struct ISampleGrabberVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISampleGrabber_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ISampleGrabber_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ISampleGrabber_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ISampleGrabber_SetOneShot(This,OneShot)	\
    ( (This)->lpVtbl -> SetOneShot(This,OneShot) ) 

#define ISampleGrabber_SetMediaType(This,pType)	\
    ( (This)->lpVtbl -> SetMediaType(This,pType) ) 

#define ISampleGrabber_GetConnectedMediaType(This,pType)	\
    ( (This)->lpVtbl -> GetConnectedMediaType(This,pType) ) 

#define ISampleGrabber_SetBufferSamples(This,BufferThem)	\
    ( (This)->lpVtbl -> SetBufferSamples(This,BufferThem) ) 

#define ISampleGrabber_GetCurrentBuffer(This,pBufferSize,pBuffer)	\
    ( (This)->lpVtbl -> GetCurrentBuffer(This,pBufferSize,pBuffer) ) 

#define ISampleGrabber_GetCurrentSample(This,ppSample)	\
    ( (This)->lpVtbl -> GetCurrentSample(This,ppSample) ) 

#define ISampleGrabber_SetCallback(This,pCallback,WhichMethodToCallback)	\
    ( (This)->lpVtbl -> SetCallback(This,pCallback,WhichMethodToCallback) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ISampleGrabber_INTERFACE_DEFINED__ */


#ifndef __IMediaSample_INTERFACE_DEFINED__
#define __IMediaSample_INTERFACE_DEFINED__

/* interface IMediaSample */
/* [unique][uuid][object][local] */ 


EXTERN_C const IID IID_IMediaSample;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("56a8689a-0ad4-11ce-b03a-0020af0ba770")
    IMediaSample : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetPointer( 
            /* [out] */ 
            __out  BYTE **ppBuffer) = 0;
        
        virtual long STDMETHODCALLTYPE GetSize( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetTime( 
            /* [out] */ 
            __out  REFERENCE_TIME *pTimeStart,
            /* [out] */ 
            __out  REFERENCE_TIME *pTimeEnd) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetTime( 
            /* [in] */ 
            __in_opt  REFERENCE_TIME *pTimeStart,
            /* [in] */ 
            __in_opt  REFERENCE_TIME *pTimeEnd) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE IsSyncPoint( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetSyncPoint( 
            BOOL bIsSyncPoint) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE IsPreroll( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetPreroll( 
            BOOL bIsPreroll) = 0;
        
        virtual long STDMETHODCALLTYPE GetActualDataLength( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetActualDataLength( 
            long __MIDL__IMediaSample0000) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetMediaType( 
            /* [out] */ 
            __out  AM_MEDIA_TYPE **ppMediaType) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetMediaType( 
            /* [in] */ 
            __in  AM_MEDIA_TYPE *pMediaType) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE IsDiscontinuity( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetDiscontinuity( 
            BOOL bDiscontinuity) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetMediaTime( 
            /* [out] */ 
            __out  LONGLONG *pTimeStart,
            /* [out] */ 
            __out  LONGLONG *pTimeEnd) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetMediaTime( 
            /* [in] */ 
            __in_opt  LONGLONG *pTimeStart,
            /* [in] */ 
            __in_opt  LONGLONG *pTimeEnd) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IMediaSampleVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IMediaSample * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IMediaSample * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IMediaSample * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetPointer )( 
            IMediaSample * This,
            /* [out] */ 
            __out  BYTE **ppBuffer);
        
        long ( STDMETHODCALLTYPE *GetSize )( 
            IMediaSample * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTime )( 
            IMediaSample * This,
            /* [out] */ 
            __out  REFERENCE_TIME *pTimeStart,
            /* [out] */ 
            __out  REFERENCE_TIME *pTimeEnd);
        
        HRESULT ( STDMETHODCALLTYPE *SetTime )( 
            IMediaSample * This,
            /* [in] */ 
            __in_opt  REFERENCE_TIME *pTimeStart,
            /* [in] */ 
            __in_opt  REFERENCE_TIME *pTimeEnd);
        
        HRESULT ( STDMETHODCALLTYPE *IsSyncPoint )( 
            IMediaSample * This);
        
        HRESULT ( STDMETHODCALLTYPE *SetSyncPoint )( 
            IMediaSample * This,
            BOOL bIsSyncPoint);
        
        HRESULT ( STDMETHODCALLTYPE *IsPreroll )( 
            IMediaSample * This);
        
        HRESULT ( STDMETHODCALLTYPE *SetPreroll )( 
            IMediaSample * This,
            BOOL bIsPreroll);
        
        long ( STDMETHODCALLTYPE *GetActualDataLength )( 
            IMediaSample * This);
        
        HRESULT ( STDMETHODCALLTYPE *SetActualDataLength )( 
            IMediaSample * This,
            long __MIDL__IMediaSample0000);
        
        HRESULT ( STDMETHODCALLTYPE *GetMediaType )( 
            IMediaSample * This,
            /* [out] */ 
            __out  AM_MEDIA_TYPE **ppMediaType);
        
        HRESULT ( STDMETHODCALLTYPE *SetMediaType )( 
            IMediaSample * This,
            /* [in] */ 
            __in  AM_MEDIA_TYPE *pMediaType);
        
        HRESULT ( STDMETHODCALLTYPE *IsDiscontinuity )( 
            IMediaSample * This);
        
        HRESULT ( STDMETHODCALLTYPE *SetDiscontinuity )( 
            IMediaSample * This,
            BOOL bDiscontinuity);
        
        HRESULT ( STDMETHODCALLTYPE *GetMediaTime )( 
            IMediaSample * This,
            /* [out] */ 
            __out  LONGLONG *pTimeStart,
            /* [out] */ 
            __out  LONGLONG *pTimeEnd);
        
        HRESULT ( STDMETHODCALLTYPE *SetMediaTime )( 
            IMediaSample * This,
            /* [in] */ 
            __in_opt  LONGLONG *pTimeStart,
            /* [in] */ 
            __in_opt  LONGLONG *pTimeEnd);
        
        END_INTERFACE
    } IMediaSampleVtbl;

    interface IMediaSample
    {
        CONST_VTBL struct IMediaSampleVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMediaSample_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IMediaSample_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IMediaSample_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IMediaSample_GetPointer(This,ppBuffer)	\
    ( (This)->lpVtbl -> GetPointer(This,ppBuffer) ) 

#define IMediaSample_GetSize(This)	\
    ( (This)->lpVtbl -> GetSize(This) ) 

#define IMediaSample_GetTime(This,pTimeStart,pTimeEnd)	\
    ( (This)->lpVtbl -> GetTime(This,pTimeStart,pTimeEnd) ) 

#define IMediaSample_SetTime(This,pTimeStart,pTimeEnd)	\
    ( (This)->lpVtbl -> SetTime(This,pTimeStart,pTimeEnd) ) 

#define IMediaSample_IsSyncPoint(This)	\
    ( (This)->lpVtbl -> IsSyncPoint(This) ) 

#define IMediaSample_SetSyncPoint(This,bIsSyncPoint)	\
    ( (This)->lpVtbl -> SetSyncPoint(This,bIsSyncPoint) ) 

#define IMediaSample_IsPreroll(This)	\
    ( (This)->lpVtbl -> IsPreroll(This) ) 

#define IMediaSample_SetPreroll(This,bIsPreroll)	\
    ( (This)->lpVtbl -> SetPreroll(This,bIsPreroll) ) 

#define IMediaSample_GetActualDataLength(This)	\
    ( (This)->lpVtbl -> GetActualDataLength(This) ) 

#define IMediaSample_SetActualDataLength(This,__MIDL__IMediaSample0000)	\
    ( (This)->lpVtbl -> SetActualDataLength(This,__MIDL__IMediaSample0000) ) 

#define IMediaSample_GetMediaType(This,ppMediaType)	\
    ( (This)->lpVtbl -> GetMediaType(This,ppMediaType) ) 

#define IMediaSample_SetMediaType(This,pMediaType)	\
    ( (This)->lpVtbl -> SetMediaType(This,pMediaType) ) 

#define IMediaSample_IsDiscontinuity(This)	\
    ( (This)->lpVtbl -> IsDiscontinuity(This) ) 

#define IMediaSample_SetDiscontinuity(This,bDiscontinuity)	\
    ( (This)->lpVtbl -> SetDiscontinuity(This,bDiscontinuity) ) 

#define IMediaSample_GetMediaTime(This,pTimeStart,pTimeEnd)	\
    ( (This)->lpVtbl -> GetMediaTime(This,pTimeStart,pTimeEnd) ) 

#define IMediaSample_SetMediaTime(This,pTimeStart,pTimeEnd)	\
    ( (This)->lpVtbl -> SetMediaTime(This,pTimeStart,pTimeEnd) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IMediaSample_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_DirectShowInterfaces_0000_0012 */
/* [local] */ 

typedef IMediaSample *PMEDIASAMPLE;

typedef struct tagVIDEOINFOHEADER {
    RECT            rcSource;          // The bit we really want to use
    RECT            rcTarget;          // Where the video should go
    DWORD           dwBitRate;         // Approximate bit data rate
    DWORD           dwBitErrorRate;    // Bit error rate for this stream
    REFERENCE_TIME  AvgTimePerFrame;   // Average time per frame (100ns units)
    BITMAPINFOHEADER bmiHeader;
} VIDEOINFOHEADER;
#include "DirectShowGuids.h"


extern RPC_IF_HANDLE __MIDL_itf_DirectShowInterfaces_0000_0012_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_DirectShowInterfaces_0000_0012_v0_0_s_ifspec;

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long *, unsigned long            , BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserMarshal(  unsigned long *, unsigned char *, BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserUnmarshal(unsigned long *, unsigned char *, BSTR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long *, BSTR * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif

///@endcond
