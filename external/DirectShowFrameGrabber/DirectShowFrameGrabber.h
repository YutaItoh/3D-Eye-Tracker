#ifndef DIRECT_SHOW_FRAME_GRABBER_H
#define DIRECT_SHOW_FRAME_GRABBER_H
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

#include <objbase.h>

#include <mutex>
#include <thread>


#ifdef HAVE_DIRECTSHOW
	#include <DShow.h>
	#pragma include_alias( "dxtrans.h", "qedit.h" )
	#define __IDxtCompositor_INTERFACE_DEFINED__
	#define __IDxtAlphaSetter_INTERFACE_DEFINED__
	#define __IDxtJpeg_INTERFACE_DEFINED__
	#define __IDxtKey_INTERFACE_DEFINED__
	#include <Qedit.h>
	#include <strmif.h>

struct __declspec(uuid("0579154A-2B53-4994-B0D0-E773148EFF85")) ISampleGrabberCB;
struct __declspec(uuid("6B652FFF-11FE-4fce-92AD-0266B5D7C78F")) ISampleGrabber;

	#pragma warning(disable: 4995)
#else
	// this include file contains just the directshow interfaces necessary to compile this component
	// if you need more, install the Windows SDK, the DirectX SDK (old version with dxtrans.h, e.g. August 2007) and
	// use the alternative above
	#include "DirectShowInterfaces.h"
#endif

#include "AutoComPtr.h"

#include <string>
#include <list>
#include <iostream>
#include <iomanip>
#include <strstream>
//#include <log4cpp/Category.hh>

#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/scoped_array.hpp>

//#include <opencv/cv.h>
#include <opencv2/imgproc/imgproc.hpp>


#include <iosfwd>
#include <string>
#include <stdexcept>
#define UBITRACK_THROW( message ) {std::cerr<<  message <<std::endl;throw;}

#define LOG4CPP_INFO( logger, message ) std::cout<<message<<std::endl;
#ifdef NDEBUG
#define LOG4CPP_DEBUG( logger, message ) std::cout<<message<<std::endl;
#else
#define LOG4CPP_DEBUG( logger, message ) ;
#endif
#define LOG4CPP_WARN( logger, message ) std::cout<<message<<std::endl;

// get a logger
//static log4cpp::Category& logger( log4cpp::Category::getInstance( "Ubitrack.Vision.DirectShowFrameGrabber" ) );

//using namespace Ubitrack;
//using namespace Ubitrack::Vision;

namespace Ubitrack { namespace Drivers {

/**
 * @ingroup vision_components
 *
 * @par Input Ports
 * None.
 *
 * @par Output Ports
 * \c Output push port of type Ubitrack::Measurement::ImageMeasurement.
 *
 * @par Configuration
 * The configuration tag contains a \c <dsvl_input> configuration.
 * For details, see the DirectShow documentation...
 *
 */
class DirectShowFrameGrabber
	: protected ISampleGrabberCB
{
public:

	/** constructor */
	DirectShowFrameGrabber( const std::string& sName );

	/** destructor, waits until thread stops */
	~DirectShowFrameGrabber();

	/** starts the camera */
	void start();

	/** stops the camera */
	void stop();

	void getFrame(cv::Mat &img);

protected:
	/** initializes the direct show filter graph */
	void initGraph();

	/** handles a frame after being converted to Vision::Image */
	void handleFrame( const cv::Mat& bufferImage );


	// width of resulting image
	LONG m_sampleWidth;

	// height of resulting image
	LONG m_sampleHeight;

	// shift timestamps (ms)
	int m_timeOffset;

	// only send every nth image
	int m_divisor;

	/** desired width */
	int m_desiredWidth;

	/** desired image height */
	int m_desiredHeight;

	// desired camera name
	std::string m_desiredName;

	// desired camera index (e.g. for multiple cameras with the same name as the Vuzix HMD)
	std::string m_desiredDevicePath;


	/** exposure control */
	int m_cameraExposure;
	bool m_cameraExposureAuto;

	/** brightness control */
	int m_cameraBrightness;

	/** contrast control */
	int m_cameraContrast;

	/** saturation control */
	int m_cameraSaturation;

	/** sharpness control */
	int m_cameraSharpness;

	/** gamma control */
	int m_cameraGamma;

	/** whitebalance control */
	int m_cameraWhitebalance;
	bool m_cameraWhitebalanceAuto;

	/** gain control */
	bool m_cameraBacklightComp;

	/** gain control */
	int m_cameraGain;


	/** number of frames received */
	int m_nFrames;

	/** timestamp of last frame */
	double m_lastTime;

	
	bool m_running;
	cv::Mat m_buffer_img;

	/** pointer to DirectShow filter graph */
	AutoComPtr< IMediaControl > m_pMediaControl;

    // ISampleGrabberCB: fake reference counting.
    STDMETHODIMP_(ULONG) AddRef() 
	{ return 1; }

    STDMETHODIMP_(ULONG) Release() 
	{ return 2; }

    STDMETHODIMP QueryInterface( REFIID riid, void **ppvObject )
    {
		if ( NULL == ppvObject ) 
			return E_POINTER;
		if ( riid == __uuidof( IUnknown ) )
		{
			*ppvObject = static_cast<IUnknown*>( this );
			 return S_OK;
		}
		if ( riid == __uuidof( ISampleGrabberCB ) )
		{
			*ppvObject = static_cast<ISampleGrabberCB*>( this );
			 return S_OK;
		}
		return E_NOTIMPL;
	}

	STDMETHODIMP SampleCB( double Time, IMediaSample *pSample );

	STDMETHODIMP BufferCB( double Time, BYTE *pBuffer, long BufferLen )
	{
		LOG4CPP_INFO( logger, "BufferCB called" );
		return E_NOTIMPL;
	}


	std::mutex m_mutex;
	std::condition_variable m_cv;
	bool is_image_arrived_;
};


} } // namespace Ubitrack::Driver


#endif // DIRECT_SHOW_FRAME_GRABBER_H