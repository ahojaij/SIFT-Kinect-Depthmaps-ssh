#include "KinectSensor.h"


class threeDServoing
{
	
public:
	threeDServoing();
	unsigned int * GetImage_color();
	USHORT * GetImage_depth();
	KinectSensor sensor;
	HRESULT hr;
//	~threeDServoing()
};