#pragma once

#ifndef KINECT_WRAPPER_H
#define KINECT_WRAPPER_H
// Direct2D Header Files
#include <d2d1.h>
#include <d2d1helper.h>


// DirectWrite
#include <dwrite.h>

#pragma comment ( lib, "winmm.lib" )
#pragma comment ( lib, "d2d1.lib" )


#include <NuiApi.h>

enum {RES_320_240, RES_640_480};

class KinectSensor
{

public:
	KinectSensor(void);
	~KinectSensor(void);
	
	HRESULT					initialize(int res);
	void					stop();
	HRESULT                 Nui_Init(int res = 0 );
    HRESULT                 Nui_Init( OLECHAR * instanceName );
	void                    grab(USHORT * d, unsigned int * c);
    void                    Nui_UnInit( );
    void                    Nui_GotDepthAlert( );
    void                    Nui_GotColorAlert( );
  

private:

    void                    grab_(USHORT *d, unsigned int *c);
    static DWORD WINAPI     Nui_ProcessThread(LPVOID pParam);
    DWORD WINAPI            Nui_ProcessThread();

    // Current kinect
    INuiSensor *            m_pNuiSensor;
	BSTR                    m_instanceId;

    // thread handling
    HANDLE					m_hThNuiProcess;
    HANDLE					m_hEvNuiProcessStop;
    HANDLE					m_hNextDepthFrameEvent;
    HANDLE					m_hNextColorFrameEvent;
    HANDLE					m_pDepthStreamHandle;
    HANDLE					m_pVideoStreamHandle;


	CRITICAL_SECTION        lock;
	bool                    depthReady;
	bool                    colorReady;

    int                     m_DepthFramesTotal;
    int                     m_LastDepthFramesTotal;
    ULONG_PTR               m_GdiplusToken;
	

	int colorWidth;
	int depthWidth;
	int colorHeight;
	int depthHeight;
	int  depthResolution;
	USHORT * depth;
	unsigned int * color;
	unsigned char buffersFull;
	NUI_IMAGE_VIEW_AREA *pcViewArea;
	

};



#endif