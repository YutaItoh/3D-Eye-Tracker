/*
 * Ubitrack - Library for Ubiquitous Tracking
 * Copyright 2006, Technische Universitaet Muenchen, and individual
 * contributors as indicated by the @authors tag. See the
 * copyright.txt in the distribution for a full listing of individual
 * contributors.
 *
 * This is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this software; if not, write to the Free
 * Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA, or see the FSF site: http://www.fsf.org.
 */

/**
 * @ingroup vision_components
 * @file
 * Reads camera images using DirectShow
 *
 * @author Daniel Pustka <daniel.pustka@in.tum.de>
 */


#pragma comment(lib,"Strmiids.lib")

#include "DirectShowFrameGrabber.h"


#include <opencv2/highgui.hpp>
namespace Ubitrack { namespace Drivers {



//DirectShowFrameGrabber::DirectShowFrameGrabber( const std::string& sName, boost::shared_ptr< Graph::UTQLSubgraph > subgraph )
DirectShowFrameGrabber::DirectShowFrameGrabber( const std::string& sName )
	: //Dataflow::Component( sName )
	 m_timeOffset( 0 )
	, m_divisor( 1 )
	, m_desiredWidth( 640 )
	, m_desiredHeight( 480 )
	, m_desiredName( sName )
	, m_cameraExposure( 0 )
	, m_cameraExposureAuto( true )
	, m_cameraBrightness( 0 )
	, m_cameraContrast( 11 )
	, m_cameraSaturation( 4 )
	, m_cameraSharpness( 3 )
	, m_cameraGamma( 150 )
	, m_cameraWhitebalance( 4500 )
	, m_cameraWhitebalanceAuto( true )
	, m_cameraBacklightComp( false )
	, m_cameraGain( 34 )
	, m_nFrames( 0 )
	, m_lastTime( -1e10 )
	, m_running(false)
	, is_image_arrived_(false)
	//, m_syncer( 1.0 )
	////, m_undistorter( *subgraph )
	//, m_outPort( "Output", *this )
	//, m_colorOutPort( "ColorOutput", *this )
	//, m_intrinsicsPort( "Intrinsics", *this, boost::bind( &DirectShowFrameGrabber::getIntrinsic, this, _1 ) )
	//, m_outPortRAW("OutputRAW", *this)
{
	HRESULT hRes = CoInitializeEx( NULL, COINIT_MULTITHREADED );
	if ( hRes == RPC_E_CHANGED_MODE )
	{ 
		LOG4CPP_WARN( logger, "CoInitializeEx failed with RPC_E_CHANGED_MODE, continuing..." );
	}
	else if ( FAILED( hRes ) )
	{
		std::ostringstream os;
		os << "Error in CoInitializeEx:" << std::hex << hRes;
		UBITRACK_THROW( os.str() );
	}
#if 0
	subgraph->m_DataflowAttributes.getAttributeData( "timeOffset", m_timeOffset );
	subgraph->m_DataflowAttributes.getAttributeData( "divisor", m_divisor );
	subgraph->m_DataflowAttributes.getAttributeData( "imageWidth", m_desiredWidth );
	subgraph->m_DataflowAttributes.getAttributeData( "imageHeight", m_desiredHeight );
	m_desiredDevicePath = subgraph->m_DataflowAttributes.getAttributeString( "devicePath" );
	m_desiredName = subgraph->m_DataflowAttributes.getAttributeString( "cameraName" );
	
	if (subgraph->m_DataflowAttributes.hasAttribute( "cameraExposure" ))
		subgraph->m_DataflowAttributes.getAttributeData( "cameraExposure", m_cameraExposure );

	if (subgraph->m_DataflowAttributes.hasAttribute( "cameraExposureAuto" ))
		m_cameraExposureAuto = subgraph->m_DataflowAttributes.getAttributeString( "cameraExposureAuto" ) == "true";

	if (subgraph->m_DataflowAttributes.hasAttribute( "cameraBrightness" ))
		subgraph->m_DataflowAttributes.getAttributeData( "cameraBrightness", m_cameraBrightness );
	
	if (subgraph->m_DataflowAttributes.hasAttribute( "cameraContrast" ))
		subgraph->m_DataflowAttributes.getAttributeData( "cameraContrast", m_cameraContrast );
	
	if (subgraph->m_DataflowAttributes.hasAttribute( "cameraSaturation" ))
		subgraph->m_DataflowAttributes.getAttributeData( "cameraSaturation", m_cameraSaturation );
	
	if (subgraph->m_DataflowAttributes.hasAttribute( "cameraSharpness" ))
		subgraph->m_DataflowAttributes.getAttributeData( "cameraSharpness", m_cameraSharpness );
	
	if (subgraph->m_DataflowAttributes.hasAttribute( "cameraGamma" ))
		subgraph->m_DataflowAttributes.getAttributeData( "cameraGamma", m_cameraGamma );

	if (subgraph->m_DataflowAttributes.hasAttribute( "cameraWhitebalance" ))
		subgraph->m_DataflowAttributes.getAttributeData( "cameraWhitebalance", m_cameraWhitebalance );
	
	if (subgraph->m_DataflowAttributes.hasAttribute( "cameraWhitebalanceAuto" ))
		m_cameraWhitebalanceAuto = subgraph->m_DataflowAttributes.getAttributeString( "cameraWhitebalanceAuto" ) == "true";

	if (subgraph->m_DataflowAttributes.hasAttribute( "cameraBacklightComp" ))
		m_cameraBacklightComp = subgraph->m_DataflowAttributes.getAttributeString( "cameraBacklightComp" ) == "true";

	if (subgraph->m_DataflowAttributes.hasAttribute( "cameraGain" ))
		subgraph->m_DataflowAttributes.getAttributeData( "cameraGain", m_cameraGain );

	if (subgraph->m_DataflowAttributes.hasAttribute("cameraModelFile")){
		std::string cameraModelFile = subgraph->m_DataflowAttributes.getAttributeString("cameraModelFile");
		m_undistorter.reset(new Vision::Undistortion(cameraModelFile));
	}
	else {
		std::string intrinsicFile = subgraph->m_DataflowAttributes.getAttributeString("intrinsicMatrixFile");
		std::string distortionFile = subgraph->m_DataflowAttributes.getAttributeString("distortionFile");


		m_undistorter.reset(new Vision::Undistortion(intrinsicFile, distortionFile));
	}


	// dynamically generate input ports
	for (Graph::UTQLSubgraph::EdgeMap::iterator it = subgraph->m_Edges.begin(); it != subgraph->m_Edges.end(); it++)
	{
		if (it->second->isInput())
		{
			if (0 == it->first.compare(0, 15, "InputIntrinsics")) {
				m_intrinsicInPort.reset(new Dataflow::PushConsumer< Measurement::CameraIntrinsics >(it->first, *this,
					boost::bind(&DirectShowFrameGrabber::newIntrinsicsPush, this, _1)));

			}
			
		}
	}

#endif

	initGraph();
}

//void DirectShowFrameGrabber::newIntrinsicsPush(Measurement::CameraIntrinsics intrinsics) {
//	m_undistorter.reset(new Vision::Undistortion(*intrinsics));
//}


DirectShowFrameGrabber::~DirectShowFrameGrabber()
{
	if ( m_pMediaControl )
		m_pMediaControl->Stop();
	CoUninitialize();
	m_running = false;
}


void DirectShowFrameGrabber::start()
{
	if ( !m_running && m_pMediaControl )
		m_pMediaControl->Run();

//	Component::start();
	m_running = true;
}


void DirectShowFrameGrabber::stop()
{
	if ( m_running && m_pMediaControl )
		m_pMediaControl->Pause();	
//	Component::stop();
//	stop();
}


void DirectShowFrameGrabber::initGraph()
{
	// Create the System Device Enumerator.
	AutoComPtr< ICreateDevEnum > pDevEnum;
	AutoComPtr< IEnumMoniker > pEnum;

	HRESULT hr = pDevEnum.CoCreateInstance( CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER );
	if ( SUCCEEDED( hr ) )
		// Create an enumerator for the video capture category.
		hr = pDevEnum->CreateClassEnumerator( CLSID_VideoInputDeviceCategory, &pEnum.p, 0 );

	AutoComPtr< IMoniker > pMoniker;
	AutoComPtr< IMoniker > pSelectedMoniker;
	std::string sSelectedCamera;
	while ( pEnum->Next( 1, &pMoniker.p, NULL ) == S_OK )
	{
		if ( !pSelectedMoniker )
			pSelectedMoniker = pMoniker;

		AutoComPtr< IPropertyBag > pPropBag;
		hr = pMoniker->BindToStorage( 0, 0, IID_IPropertyBag, (void**)(&pPropBag.p) );
		if ( FAILED( hr ) )
		{
			pMoniker.Release();
			continue;  // Skip this one, maybe the next one will work.
		} 

		// Find the device of the camera.
		VARIANT varDevicePath;
		VariantInit( &varDevicePath );
		hr = pPropBag->Read( L"DevicePath", &varDevicePath, 0 );
		char sDevicePath[ 128 ];
		if ( SUCCEEDED( hr ) )
			// could be optimized somehow ..
			WideCharToMultiByte( CP_ACP, 0, varDevicePath.bstrVal, -1, sDevicePath, 128, 0, 0 );

		// Find the description or friendly name.
		VARIANT varName;
		VariantInit( &varName );
		hr = pPropBag->Read( L"Description", &varName, 0 );
		if ( FAILED( hr ) )
			hr = pPropBag->Read( L"FriendlyName", &varName, 0 );

		

		if ( SUCCEEDED( hr ) )
		{
			char sName[ 128 ];
			WideCharToMultiByte( CP_ACP, 0, varName.bstrVal, -1, sName, 128, 0, 0 );
			LOG4CPP_INFO( logger, "Possible capture device: " << &sName[0] << " device path: " << sDevicePath );

			// select device based on name
			if ( !m_desiredName.empty() && strstr( sName, m_desiredName.c_str() ) )
			{
				if( m_desiredDevicePath.empty() )
				{
					sSelectedCamera = sName;
					pSelectedMoniker = pMoniker;
					break;
				}
				else if ( strstr( sDevicePath, m_desiredDevicePath.c_str() ) )
				{
					sSelectedCamera = sName;
					pSelectedMoniker = pMoniker;
					LOG4CPP_INFO( logger, "Found device with path-identifier: " << m_desiredDevicePath );
					break;
				}
			}

			VariantClear( &varName ); 
		}

		pMoniker.Release();
	}

	// check if a capture device was found
	if ( !pSelectedMoniker )
		UBITRACK_THROW( "No video capture device found" );

	LOG4CPP_INFO( logger, "Using camera: " << sSelectedCamera );

	// create capture graph
    AutoComPtr< IGraphBuilder > pGraph;
    AutoComPtr< ICaptureGraphBuilder2 > pBuild;

    // Create the Capture Graph Builder.
    hr = pBuild.CoCreateInstance( CLSID_CaptureGraphBuilder2, NULL, CLSCTX_INPROC_SERVER );
    if ( SUCCEEDED( hr ) )
    {
        // Create the Filter Graph Manager.
        hr = pGraph.CoCreateInstance( CLSID_FilterGraph, 0, CLSCTX_INPROC_SERVER );
        if ( SUCCEEDED( hr ) )
            // Initialize the Capture Graph Builder.
            pBuild->SetFiltergraph( pGraph );
        else
			UBITRACK_THROW( "Error creating filter graph manager" );
    }
	else
		UBITRACK_THROW( "Error creating capture graph builder" );

	// create capture device filter
	AutoComPtr< IBaseFilter > pCaptureFilter;
	if ( FAILED( pSelectedMoniker->BindToObject( 0, 0, IID_IBaseFilter, (void**)&pCaptureFilter.p ) ) )
		UBITRACK_THROW( "Unable to create capture filter" );

	if ( FAILED( pGraph->AddFilter( pCaptureFilter, L"Capture" ) ) )
		UBITRACK_THROW( "Unable to add capture filter" );

	// find output pin for configuration
	AutoComPtr< IPin > pPin;
	if ( FAILED( pBuild->FindPin( pCaptureFilter, PINDIR_OUTPUT, &PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video, FALSE, 0, &pPin.p ) ) )
		UBITRACK_THROW( "Unable to find pin" );

	// enumerate media types
	AutoComPtr< IAMStreamConfig > pStreamConfig;
	if ( FAILED( pPin.QueryInterface< IAMStreamConfig >( pStreamConfig ) ) )
	{ LOG4CPP_WARN( logger, "Unable to get IAMStreamConfig interface" ); }
	else
	{
		int iCount, iSize;
		pStreamConfig->GetNumberOfCapabilities( &iCount, &iSize );
		boost::scoped_array< BYTE > buf( new BYTE[ iSize ] );

		bool bSet = false;
		for ( int iCap = 0; iCap < iCount; iCap++ )
		{
			AM_MEDIA_TYPE *pMediaType;
			pStreamConfig->GetStreamCaps( iCap, &pMediaType, buf.get() );

			if ( pMediaType->majortype != MEDIATYPE_Video || pMediaType->formattype != FORMAT_VideoInfo )
				continue;
			VIDEOINFOHEADER* pInfo = (VIDEOINFOHEADER*)pMediaType->pbFormat;

			LOG4CPP_INFO( logger, "Media type: fps=" << 1e7 / pInfo->AvgTimePerFrame << 
				", width=" << pInfo->bmiHeader.biWidth << ", height=" << pInfo->bmiHeader.biHeight <<
				", type=" << ( pMediaType->subtype == MEDIASUBTYPE_RGB24 ? "RGB24" : "?" ) );

			// set first format with correct size, but prefer RGB24 
			if ( ( m_desiredWidth <= 0 || pInfo->bmiHeader.biWidth == m_desiredWidth ) && 
				( m_desiredHeight <= 0 || pInfo->bmiHeader.biHeight == m_desiredHeight ) &&
				( !bSet || pMediaType->subtype == MEDIASUBTYPE_RGB24 ) )
			{
				pStreamConfig->SetFormat( pMediaType );
				if ( bSet )
					break;
				bSet = true;
			}

			// TODO: DeleteMediaType
		}
	}

	// create sample grabber filter
	AutoComPtr< IBaseFilter > pSampleGrabberFilter;
	if ( FAILED( pSampleGrabberFilter.CoCreateInstance( CLSID_SampleGrabber, NULL, CLSCTX_INPROC_SERVER ) ) )
		UBITRACK_THROW( "Unable to create sample grabber filter" );

	if ( FAILED( pGraph->AddFilter( pSampleGrabberFilter, L"SampleGrab" ) ) )
		UBITRACK_THROW( "Unable to add sample grabber filter" );

	// configure sample grabber
	AutoComPtr< ISampleGrabber > pSampleGrabber;
	pSampleGrabberFilter.QueryInterface( pSampleGrabber );
	pSampleGrabber->SetOneShot( FALSE );
	pSampleGrabber->SetBufferSamples( FALSE );
	pSampleGrabber->SetCallback( this, 0 ); // 0 = Use the SampleCB callback method.




	// make it picky on media types
	AM_MEDIA_TYPE mediaType;
	memset( &mediaType, 0, sizeof( mediaType ) );
	mediaType.majortype = MEDIATYPE_Video;
	mediaType.subtype = MEDIASUBTYPE_RGB24;
	pSampleGrabber->SetMediaType( &mediaType );

	// null renderer
	AutoComPtr< IBaseFilter > pNullRenderer;
	if ( FAILED( pNullRenderer.CoCreateInstance( CLSID_NullRenderer, NULL, CLSCTX_INPROC_SERVER ) ) )
		UBITRACK_THROW( "Unable to create null renderer filter" );

	if ( FAILED( pGraph->AddFilter( pNullRenderer, L"NullRender" ) ) )
		UBITRACK_THROW( "Unable to add null renderer filter" );

	// connect all filters
	hr = pBuild->RenderStream(
		&PIN_CATEGORY_CAPTURE, // Connect this pin ...
		&MEDIATYPE_Video,      // with this media type ...
		pCaptureFilter,        // on this filter ...
		pSampleGrabberFilter,  // to the Sample Grabber ...
		pNullRenderer );       // ... and finally to the Null Renderer.
	if ( FAILED( hr ) )
		UBITRACK_THROW( "Unable to render stream" );

	// get media type
	pSampleGrabber->GetConnectedMediaType( &mediaType );
	if ( mediaType.majortype != MEDIATYPE_Video || mediaType.subtype != MEDIASUBTYPE_RGB24 ||
		mediaType.formattype != FORMAT_VideoInfo )
		UBITRACK_THROW( "Unsupported MEDIATYPE" );

	VIDEOINFOHEADER* pVidInfo = (VIDEOINFOHEADER*)mediaType.pbFormat;
	m_sampleWidth = pVidInfo->bmiHeader.biWidth;
	m_sampleHeight = pVidInfo->bmiHeader.biHeight;
	double fps = (1.0 / pVidInfo->AvgTimePerFrame) * 10000000.0;
	LOG4CPP_INFO( logger, "Image dimensions: " << m_sampleWidth << "x" << m_sampleHeight << " FPS: " << fps );
	// TODO: FreeMediaType( &mediaType );


#ifdef HAVE_DIRECTSHOW
	/* additionally control camera parameters infos at:
	 * http://msdn.microsoft.com/en-us/library/dd318253(v=vs.85).aspx
	 */

	LOG4CPP_INFO( logger, "Setting additional direct show parameter ");

	IAMCameraControl *pCameraControl; 
	hr = pCaptureFilter->QueryInterface(IID_IAMCameraControl, (void **)&pCameraControl); 
	if ( SUCCEEDED(hr) ) {

		// could check if provided value is within range
		//hr = pCameraControl->GetRange(CameraControl_Exposure,
		//						NULL, // min
		//						NULL, // max
		//						NULL, // minstep
		//						&defaultExposureValue, // default
		//						NULL); // capflags

		int expFlag = CameraControl_Flags_Manual;
		if (m_cameraExposureAuto)
			expFlag = CameraControl_Flags_Auto;

		hr = pCameraControl->Set(CameraControl_Exposure, // property
								m_cameraExposure, // value
								expFlag); 
		if (FAILED(hr))
			LOG4CPP_ERROR( logger, "Error setting camera exposure property to " << m_cameraExposure);
	}

	IAMVideoProcAmp *pAMVideoProcAmp;
	hr = pCaptureFilter->QueryInterface(IID_IAMVideoProcAmp, (void**)&pAMVideoProcAmp);
	if (SUCCEEDED(hr)) {

		long Min, Max, Step, Default, Flags, Val;

		pAMVideoProcAmp->GetRange(VideoProcAmp_Brightness, &Min, &Max, &Step, &Default, &Flags);
		LOG4CPP_INFO(logger, "Possible Settings for VideoProcAmp_Brightness: min=" <<Min << " max="<<Max << " Step=" << Step << " Default="<<Default << " Flags="<<Flags );
		pAMVideoProcAmp->Get(VideoProcAmp_Brightness, &Default, &Flags);
		LOG4CPP_INFO(logger, "Current Settings for VideoProcAmp_Brightness: Default=" << Default << " Flags=" << Flags);

		pAMVideoProcAmp->GetRange(VideoProcAmp_Contrast, &Min, &Max, &Step, &Default, &Flags);
		LOG4CPP_INFO(logger, "Possible Settings for VideoProcAmp_Contrast: min=" << Min << " max=" << Max << " Step=" << Step << " Default=" << Default << " Flags=" << Flags);
		pAMVideoProcAmp->Get(VideoProcAmp_Contrast, &Default, &Flags);
		LOG4CPP_INFO(logger, "Current Settings for VideoProcAmp_Contrast: Default=" << Default << " Flags=" << Flags);
		
		pAMVideoProcAmp->GetRange(VideoProcAmp_Saturation, &Min, &Max, &Step, &Default, &Flags);
		LOG4CPP_INFO(logger, "Possible Settings for VideoProcAmp_Saturation: min=" << Min << " max=" << Max << " Step=" << Step << " Default=" << Default << " Flags=" << Flags);
		pAMVideoProcAmp->Get(VideoProcAmp_Saturation, &Default, &Flags);
		LOG4CPP_INFO(logger, "Current Settings for VideoProcAmp_Saturation: Default=" << Default << " Flags=" << Flags);
		
		pAMVideoProcAmp->GetRange(VideoProcAmp_Sharpness, &Min, &Max, &Step, &Default, &Flags);
		LOG4CPP_INFO(logger, "Possible Settings for VideoProcAmp_Sharpness: min=" << Min << " max=" << Max << " Step=" << Step << " Default=" << Default << " Flags=" << Flags);
		pAMVideoProcAmp->Get(VideoProcAmp_Sharpness, &Default, &Flags);
		LOG4CPP_INFO(logger, "Current Settings for VideoProcAmp_Sharpness: Default=" << Default << " Flags=" << Flags);
		
		pAMVideoProcAmp->GetRange(VideoProcAmp_Gamma, &Min, &Max, &Step, &Default, &Flags);
		LOG4CPP_INFO(logger, "Possible Settings for VideoProcAmp_Gamma: min=" << Min << " max=" << Max << " Step=" << Step << " Default=" << Default << " Flags=" << Flags);
		pAMVideoProcAmp->Get(VideoProcAmp_Gamma, &Default, &Flags);
		LOG4CPP_INFO(logger, "Current Settings for VideoProcAmp_Gamma: Default=" << Default << " Flags=" << Flags);
		
		pAMVideoProcAmp->GetRange(VideoProcAmp_WhiteBalance, &Min, &Max, &Step, &Default, &Flags);
		LOG4CPP_INFO(logger, "Possible Settings for VideoProcAmp_WhiteBalance: min=" << Min << " max=" << Max << " Step=" << Step << " Default=" << Default << " Flags=" << Flags);
		pAMVideoProcAmp->Get(VideoProcAmp_WhiteBalance, &Default, &Flags);
		LOG4CPP_INFO(logger, "Current Settings for VideoProcAmp_WhiteBalance: Default=" << Default << " Flags=" << Flags);
		
		pAMVideoProcAmp->GetRange(VideoProcAmp_BacklightCompensation, &Min, &Max, &Step, &Default, &Flags);
		LOG4CPP_INFO(logger, "Possible Settings for VideoProcAmp_BacklightCompensation: min=" << Min << " max=" << Max << " Step=" << Step << " Default=" << Default << " Flags=" << Flags);
		pAMVideoProcAmp->Get(VideoProcAmp_BacklightCompensation, &Default, &Flags);
		LOG4CPP_INFO(logger, "Current Settings for VideoProcAmp_BacklightCompensation: Default=" << Default << " Flags=" << Flags);
		
		pAMVideoProcAmp->GetRange(VideoProcAmp_Gain, &Min, &Max, &Step, &Default, &Flags);
		LOG4CPP_INFO(logger, "Possible Settings for VideoProcAmp_Gain: min=" << Min << " max=" << Max << " Step=" << Step << " Default=" << Default << " Flags=" << Flags);
		pAMVideoProcAmp->Get(VideoProcAmp_Gain, &Default, &Flags);
		LOG4CPP_INFO(logger, "Current Settings for VideoProcAmp_Gain: Default=" << Default << " Flags=" << Flags);
		

		hr = pAMVideoProcAmp->Set(VideoProcAmp_Brightness, m_cameraBrightness, VideoProcAmp_Flags_Manual);
		if (FAILED(hr))
			LOG4CPP_ERROR( logger, "Error setting camera exposure brightness to " << m_cameraBrightness);

		hr = pAMVideoProcAmp->Set(VideoProcAmp_Contrast, m_cameraContrast, VideoProcAmp_Flags_Manual);
		if (FAILED(hr))
			LOG4CPP_ERROR( logger, "Error setting camera contrast property to " << m_cameraContrast);

		hr = pAMVideoProcAmp->Set(VideoProcAmp_Saturation, m_cameraSaturation, VideoProcAmp_Flags_Manual);
		if (FAILED(hr))
			LOG4CPP_ERROR( logger, "Error setting camera saturation property to " << m_cameraSaturation);

		hr = pAMVideoProcAmp->Set(VideoProcAmp_Sharpness, m_cameraSharpness, VideoProcAmp_Flags_Manual);
		if (FAILED(hr))
			LOG4CPP_ERROR( logger, "Error setting camera sharpness property to " << m_cameraSharpness);

		hr = pAMVideoProcAmp->Set(VideoProcAmp_Gamma, m_cameraGamma, VideoProcAmp_Flags_Manual);
		if (FAILED(hr))
			LOG4CPP_ERROR( logger, "Error setting camera gamma property to " << m_cameraGamma);

		int wbFlags = VideoProcAmp_Flags_Manual;
		if (m_cameraWhitebalanceAuto)
			wbFlags = VideoProcAmp_Flags_Auto;
		hr = pAMVideoProcAmp->Set(VideoProcAmp_WhiteBalance, m_cameraWhitebalance, wbFlags);
		if (FAILED(hr))
			LOG4CPP_ERROR( logger, "Error setting camera whitebalance property to " << m_cameraWhitebalance);

		int backlightComp = m_cameraBacklightComp ? 1 : 0;
		hr = pAMVideoProcAmp->Set(VideoProcAmp_BacklightCompensation, backlightComp, VideoProcAmp_Flags_Manual);
		if (FAILED(hr))
			LOG4CPP_ERROR( logger, "Error setting camera backlight compensation property to " << backlightComp);

		hr = pAMVideoProcAmp->Set(VideoProcAmp_Gain, m_cameraGain, VideoProcAmp_Flags_Manual);
		if (FAILED(hr))
			LOG4CPP_ERROR( logger, "Error setting camera gain property to " << m_cameraGain);

	}

#endif


	// start stream
	pGraph.QueryInterface< IMediaControl >( m_pMediaControl );
	m_pMediaControl->Pause();
}

void DirectShowFrameGrabber::getFrame(cv::Mat &img){
	{
      std::unique_lock<std::mutex> lk(m_mutex);
	  while (is_image_arrived_ == false) {
        m_cv.wait(lk);  // ÅöèåèïœêîavailÇ÷ÇÃí ímÇë“ã@
      }
	  img = m_buffer_img.clone();
	  is_image_arrived_ = false;
	}
	cv::flip(img, img, 0);// flip image
}

//void DirectShowFrameGrabber::handleFrame( Measurement::Timestamp utTime, const Vision::Image& bufferImage )
void DirectShowFrameGrabber::handleFrame( const cv::Mat& bufferImage )
{
#if 0
//	boost::shared_ptr< Vision::Image > pColorImage;
	cv::Mat pColorImage;
	bool bColorImageDistorted = true;
	
	if ( ( m_desiredWidth > 0 && m_desiredHeight > 0 ) && 
		( bufferImage.cols > m_desiredWidth || bufferImage.rows > m_desiredHeight ) )
	{
	    LOG4CPP_DEBUG( logger, "downsampling" );
		pColorImage = cv::Mat( m_desiredHeight, m_desiredWidth, CV_8UC3 ) ;
//		pColorImage->origin = bufferImage.origin;
//		cvResize( bufferImage, *pColorImage );
	}
//	cv::imshow(m_desiredName,bufferImage);
#endif

	{
		std::lock_guard<std::mutex> lk(m_mutex);
		m_buffer_img = bufferImage.clone();
		is_image_arrived_ = true;
	}
	m_cv.notify_one();
#if 0
	if ( ( m_desiredWidth > 0 && m_desiredHeight > 0 ) && 
		( bufferImage.width > m_desiredWidth || bufferImage.height > m_desiredHeight ) )
	{
	    LOG4CPP_DEBUG( logger, "downsampling" );
		pColorImage.reset( new Vision::Image( m_desiredWidth, m_desiredHeight, 3 ) );
		pColorImage->origin = bufferImage.origin;
		cvResize( bufferImage, *pColorImage );
	}

	if (m_outPortRAW.isConnected()) {
		m_outPortRAW.send(Measurement::ImageMeasurement(utTime, bufferImage.Clone()));
	}

	if ( m_colorOutPort.isConnected() )
	{
		if ( pColorImage )
			pColorImage = m_undistorter->undistort( pColorImage );
		else
			pColorImage = m_undistorter->undistort( bufferImage );
		bColorImageDistorted = false;

		memcpy( pColorImage->channelSeq, "BGR", 4 );
		
		m_colorOutPort.send( Measurement::ImageMeasurement( utTime, pColorImage ) );
	}

	if ( m_outPort.isConnected() )
	{
		boost::shared_ptr< Vision::Image > pGreyImage;
		if ( pColorImage )
			pGreyImage = pColorImage->CvtColor( CV_BGR2GRAY, 1 );
		else
			pGreyImage = bufferImage.CvtColor( CV_BGR2GRAY, 1 );
		
		if ( bColorImageDistorted )
			pGreyImage = m_undistorter->undistort( pGreyImage );
		
		m_outPort.send( Measurement::ImageMeasurement( utTime, pGreyImage ) );
	}
#endif
}


STDMETHODIMP DirectShowFrameGrabber::SampleCB( double Time, IMediaSample *pSample )
{
	// TODO: check for double frames when using multiple cameras...
//	LOG4CPP_DEBUG( logger, "SampleCB called" );

	if ( Time == m_lastTime )
	{
		// this was a problem with DSVideoLib and multiple cameras
		LOG4CPP_INFO( logger, "Got double frame" );
		return S_OK;
	}
	m_lastTime = Time;

	if ( !m_running || ( ++m_nFrames % m_divisor ) )
		return S_OK;

	if ( pSample->GetSize() < m_sampleWidth * m_sampleHeight * 3 )
	{
		LOG4CPP_INFO( logger, "Invalid sample size" );
		return S_OK;
	}

	BYTE* pBuffer;
	if ( FAILED( pSample->GetPointer( &pBuffer ) ) )
	{
		LOG4CPP_INFO( logger, "GetPointer failed" );
		return S_OK;
	}

	// create IplImage, convert and send
#if 0
	Vision::Image bufferImage( m_sampleWidth, m_sampleHeight, 3, pBuffer, IPL_DEPTH_8U, 1 );
	Measurement::Timestamp utTime = m_syncer.convertNativeToLocal( Time );
	
	handleFrame( utTime + 1000000L * m_timeOffset, bufferImage );
#endif
	cv::Mat bufferImage(m_sampleHeight, m_sampleWidth, CV_8UC3,pBuffer);
	handleFrame(bufferImage );
//	cv::waitKey(5);
	return S_OK;
}

} } // namespace Ubitrack::Driver


