#include "KinectSensor.h"
#include <mmsystem.h>
#include <assert.h>
#include <strsafe.h>

#define _CRT_SECURE_NO_DEPRECATE

#include <iostream>

//-------------------------------------------------------------------
// Constructor
//-------------------------------------------------------------------
KinectSensor::KinectSensor(void)
{
	

	pcViewArea = new NUI_IMAGE_VIEW_AREA;

	if (m_pNuiSensor)
    {
      // m_pNuiSensor->Release();
		m_pNuiSensor = NULL;
    }
    m_hNextDepthFrameEvent = NULL;
    m_hNextColorFrameEvent = NULL;
    m_pDepthStreamHandle = NULL;
    m_pVideoStreamHandle = NULL;
    m_hThNuiProcess = NULL;
    m_hEvNuiProcessStop = NULL;
    m_LastDepthFramesTotal = 0;
	
	depthReady = false;
	colorReady = false;
	
	InitializeCriticalSection (&lock);
   


    std::cout<<"member"<<std::endl; 

}

HRESULT KinectSensor::initialize(int resolution)
{
	return Nui_Init( resolution);
}

void KinectSensor::stop()
{
	Nui_UnInit();
}

//-------------------------------------------------------------------
// Destructor
//-------------------------------------------------------------------
KinectSensor::~KinectSensor()
{
	

	m_hNextDepthFrameEvent = NULL;
    m_hNextColorFrameEvent = NULL;
    m_pDepthStreamHandle = NULL;
    m_pVideoStreamHandle = NULL;
    m_hThNuiProcess = NULL;
    m_hEvNuiProcessStop = NULL;
	SysFreeString(m_instanceId);
}


void KinectSensor::grab(USHORT *d, unsigned int *c)
{
	bool ready = false;
	while(!ready)
	{
		EnterCriticalSection (&lock);
		if(depthReady && colorReady)
		{	
			ready = true;
				grab_(d,c);
				std::cout<<"grabbing   "<<depthReady<<std::endl;
				depthReady = false;
				colorReady = false;
			LeaveCriticalSection (&lock);
		}
		else
			{
			
				LeaveCriticalSection(&lock);
		}
	}
			
	

	//EnterCriticalSection (&lock);
	//dcReady = 0;
	//LeaveCriticalSection(&lock);
	
	
}

void KinectSensor::grab_(USHORT * d, unsigned int * c)
{
	NUI_IMAGE_VIEW_AREA * viewArea = new NUI_IMAGE_VIEW_AREA;
	
	*viewArea = *pcViewArea;
	int n;
	LONG x,y;
	if(!depthResolution)
	{
		for(n=0; n<depthWidth*depthHeight; n++)
		{
			if(NuiImageGetColorPixelCoordinatesFromDepthPixelAtResolution(NUI_IMAGE_RESOLUTION_640x480,NUI_IMAGE_RESOLUTION_320x240, viewArea, n%depthWidth,n/depthWidth,depth[n],&x,&y)==S_OK)
			{
				if(x>=0 && y>=0 && y<depthHeight && x<depthWidth)
					c[n]=color[y*colorWidth+x];
			}
			else
				c[n]=0;
			d[n] = depth[n]>>3;
		}
	}
	else
	{
		for(n=0; n<depthWidth*depthHeight; n++)
		{
			if(NuiImageGetColorPixelCoordinatesFromDepthPixelAtResolution(NUI_IMAGE_RESOLUTION_640x480,NUI_IMAGE_RESOLUTION_640x480, viewArea, n%depthWidth,n/depthWidth,d[n],&x,&y)==S_OK)
			{
				if(x>=0 && y>=0 && y<depthHeight && x<depthWidth)
					c[n]=color[y*colorWidth+x];
			}
			else
				c[n]=0;
			d[n] = depth[n]>>3;
		}
	}
}



//-------------------------------------------------------------------
// Nui_Init
//
// Initialize Kinect by instance name
//-------------------------------------------------------------------
HRESULT KinectSensor::Nui_Init( OLECHAR *instanceName )
{
    // Generic creation failure
    if ( NULL == instanceName )
    {
		std::cout<<"failed to create kinect 1"<<std::endl;
        return E_FAIL;
    }
	
    HRESULT hr = NuiCreateSensorById( instanceName, &m_pNuiSensor );
	// Generic creation failure
	if ( FAILED(hr) )
	{
       std::cout<<"failed to create kinect 2"<<std::endl;
	   return hr;
	}
	
	SysFreeString(m_instanceId);
	
    m_instanceId = m_pNuiSensor->NuiDeviceConnectionId();

	return Nui_Init();

}

//-------------------------------------------------------------------
// Nui_Init
//
// Initialize Kinect
//-------------------------------------------------------------------

HRESULT KinectSensor::Nui_Init(int res )
{

	depthResolution = res;
	if(!depthResolution)
	{
		depthWidth = 320;
		depthHeight = 240;
	}
	else
	{
		depthWidth = 640;
		depthHeight = 480;
	}

	colorWidth = 640;
	colorHeight = 480;
	
	depth = new unsigned short[depthWidth*depthHeight];
	color = new unsigned int[colorWidth*colorHeight];

    HRESULT  hr;

	if ( !m_pNuiSensor )
	{
		HRESULT hr = NuiCreateSensorByIndex(0, &m_pNuiSensor);
		if ( FAILED(hr) )
		{
			std::cout<<"failed to create sensor"<<std::endl;
			return hr;
		}
		std::cout<<"sensor created"<<std::endl;
		

        //SysFreeString(m_instanceId);
        m_instanceId = m_pNuiSensor->NuiDeviceConnectionId();

    }

	m_hNextDepthFrameEvent = CreateEvent( NULL, TRUE, FALSE, NULL );
	m_hNextColorFrameEvent = CreateEvent( NULL, TRUE, FALSE, NULL );

    DWORD nuiFlags = NUI_INITIALIZE_FLAG_USES_DEPTH |  NUI_INITIALIZE_FLAG_USES_COLOR;

    hr = m_pNuiSensor->NuiInitialize( nuiFlags );
   

	 if ( FAILED( hr ) )
	 {
		 if ( E_NUI_DEVICE_IN_USE == hr )
		 {
			 std::cout<<"Error"<<std::endl;   
		 }
		 else
		 {
			 std::cout<<"Error"<<std::endl;   
		 }
        return hr;
	 }
	 
	
	 hr = m_pNuiSensor->NuiImageStreamOpen(NUI_IMAGE_TYPE_COLOR, NUI_IMAGE_RESOLUTION_640x480, 0, 2, m_hNextColorFrameEvent, &m_pVideoStreamHandle );
	 if ( FAILED( hr ) )
	 {
		 std::cout<<"Failed to open image stream"<<std::endl;
		 return hr;
	 } 
	 
	 if(!depthResolution)
	 {
		 hr = m_pNuiSensor->NuiImageStreamOpen(NUI_IMAGE_TYPE_DEPTH, NUI_IMAGE_RESOLUTION_320x240, 0,2, m_hNextDepthFrameEvent, &m_pDepthStreamHandle );
	 } 
	 else
	 {
		 hr = m_pNuiSensor->NuiImageStreamOpen( NUI_IMAGE_TYPE_DEPTH, NUI_IMAGE_RESOLUTION_640x480, 0,2, m_hNextDepthFrameEvent, &m_pDepthStreamHandle );
	 }

    if ( FAILED( hr ) )

    {
		std::cout<<"Failed to open image stream"<<std::endl;
        return hr;

    }
	m_pNuiSensor->NuiImageStreamSetImageFrameFlags(m_pDepthStreamHandle,NUI_IMAGE_STREAM_FLAG_ENABLE_NEAR_MODE | NUI_IMAGE_STREAM_FLAG_DISTINCT_OVERFLOW_DEPTH_VALUES);
	NuiImageStreamSetImageFrameFlags(m_pDepthStreamHandle,NUI_IMAGE_STREAM_FLAG_ENABLE_NEAR_MODE);

    // Start the Nui processing thread
    m_hEvNuiProcessStop = CreateEvent( NULL, FALSE, FALSE, NULL );
    m_hThNuiProcess = CreateThread( NULL, 0, Nui_ProcessThread, this, 0, NULL );

    return hr;

}
//-------------------------------------------------------------------
// Nui_UnInit
//
// Uninitialize Kinect
//-------------------------------------------------------------------
void KinectSensor::Nui_UnInit( )
{
 
    // Stop the Nui processing thread
    if ( NULL != m_hEvNuiProcessStop )
    {
       // Signal the thread
        SetEvent(m_hEvNuiProcessStop);
        // Wait for thread to stop
        if ( NULL != m_hThNuiProcess )
        {
            WaitForSingleObject( m_hThNuiProcess, INFINITE );
            CloseHandle( m_hThNuiProcess );

        }
        CloseHandle( m_hEvNuiProcessStop );
    }

    if ( m_pNuiSensor )
    {
        m_pNuiSensor->NuiShutdown( );
    }

    /*if ( m_hNextSkeletonEvent && ( m_hNextSkeletonEvent != INVALID_HANDLE_VALUE ) )

    {

        CloseHandle( m_hNextSkeletonEvent );

        m_hNextSkeletonEvent = NULL;

    }*/
    if ( m_hNextDepthFrameEvent && ( m_hNextDepthFrameEvent != INVALID_HANDLE_VALUE ) )
    {
        CloseHandle( m_hNextDepthFrameEvent );
        m_hNextDepthFrameEvent = NULL;
    }

    if ( m_hNextColorFrameEvent && ( m_hNextColorFrameEvent != INVALID_HANDLE_VALUE ) )
    {
        CloseHandle( m_hNextColorFrameEvent );
        m_hNextColorFrameEvent = NULL;
    }
    if ( m_pNuiSensor )
    {
        m_pNuiSensor->Release();
        m_pNuiSensor = NULL;
    }
}



DWORD WINAPI KinectSensor::Nui_ProcessThread(LPVOID pParam)
{
    std::cout<<"starting kinect thread"<<std::endl;
	KinectSensor *pthis = (KinectSensor *) pParam;
    return pthis->Nui_ProcessThread();
	
}

//-------------------------------------------------------------------
// Nui_ProcessThread
//
// Thread to handle Kinect processing
//-------------------------------------------------------------------
DWORD WINAPI KinectSensor::Nui_ProcessThread()
{
    const int numEvents = 3;
    HANDLE hEvents[numEvents] = { m_hEvNuiProcessStop, m_hNextDepthFrameEvent, m_hNextColorFrameEvent};
    int    nEventIdx;
	// Main thread loop
	bool continueProcessing = true;
	NUI_IMAGE_FRAME imageFrame;
	while ( continueProcessing )

    {

        // Wait for any of the events to be signalled

        nEventIdx = WaitForMultipleObjects( numEvents, hEvents, FALSE, 100 );

        // Process signal events

        switch ( nEventIdx )
		{
		case WAIT_TIMEOUT:
			continue;
			// If the stop event, stop looping and exit
		case WAIT_OBJECT_0:
			continueProcessing = false;
			continue;
		
		case WAIT_OBJECT_0 + 2:
			EnterCriticalSection (&lock);
				if(!colorReady)
				{
					colorReady = true;
					std::cout<<"Filling color buffer"<<std::endl;
					Nui_GotColorAlert();
					LeaveCriticalSection(&lock);
				}
				else
				{
					
					m_pNuiSensor->NuiImageStreamGetNextFrame( m_pVideoStreamHandle, 0, &imageFrame );
					m_pNuiSensor->NuiImageStreamReleaseFrame( m_pVideoStreamHandle, &imageFrame );
					LeaveCriticalSection (&lock);  
				}
                break; 
		
		case WAIT_OBJECT_0 + 1:
				EnterCriticalSection (&lock);
				if(!depthReady)
				{
					depthReady = true;	
					std::cout<<"Filling depth buffer"<<std::endl;
					Nui_GotDepthAlert();
					LeaveCriticalSection(&lock);
				}
				else
				{
					m_pNuiSensor->NuiImageStreamGetNextFrame( m_pDepthStreamHandle, 0, &imageFrame );
					m_pNuiSensor->NuiImageStreamReleaseFrame( m_pDepthStreamHandle, &imageFrame );
					LeaveCriticalSection (&lock);  
				}
                break; 
		
		default:
			break;
        }
        
    }

    return 0;
}

//-------------------------------------------------------------------
// Nui_GotDepthAlert
//
// Handle new color data
//-------------------------------------------------------------------

void KinectSensor::Nui_GotColorAlert( )
{
    NUI_IMAGE_FRAME imageFrame;

    HRESULT hr = m_pNuiSensor->NuiImageStreamGetNextFrame( m_pVideoStreamHandle, 0, &imageFrame );

    if ( FAILED( hr ) )
    {
        return;
    }
    INuiFrameTexture * pTexture = imageFrame.pFrameTexture;
    NUI_LOCKED_RECT LockedRect;
    pTexture->LockRect( 0, &LockedRect, NULL, 0 );
	
    if ( LockedRect.Pitch != 0 )
    {		
	BYTE * pBuffer = (BYTE*) LockedRect.pBits;
	{

			memcpy(color,LockedRect.pBits,colorHeight*colorWidth*sizeof(unsigned int));

	}
	*pcViewArea = imageFrame.ViewArea;
	}
	else
	{
        OutputDebugString( "Buffer length of received texture is bogus\r\n" );
    }

    pTexture->UnlockRect( 0 );

    m_pNuiSensor->NuiImageStreamReleaseFrame( m_pVideoStreamHandle, &imageFrame );
}

//-------------------------------------------------------------------
// Nui_GotDepthAlert
//
// Handle new depth data
//-------------------------------------------------------------------
void KinectSensor::Nui_GotDepthAlert( )
{
    NUI_IMAGE_FRAME imageFrame;
    HRESULT hr = m_pNuiSensor->NuiImageStreamGetNextFrame(
        m_pDepthStreamHandle,
        0,
        &imageFrame );

    if ( FAILED( hr ) )
    {
        return;
    }
    INuiFrameTexture * pTexture = imageFrame.pFrameTexture;
    NUI_LOCKED_RECT LockedRect;
    pTexture->LockRect( 0, &LockedRect, NULL, 0 );
    if ( 0 != LockedRect.Pitch )
    {
		USHORT * pBufferRun = (USHORT *)LockedRect.pBits;
		{
			
				memcpy(depth,LockedRect.pBits,depthHeight*depthWidth*sizeof(USHORT));
				
		}	
    }
    else
    {
        OutputDebugString( "Buffer length of received texture is bogus\r\n" );
    }

    pTexture->UnlockRect( 0 );

	//reading dull color image
    m_pNuiSensor->NuiImageStreamReleaseFrame( m_pDepthStreamHandle, &imageFrame );


    
}