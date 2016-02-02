#include <d2d1.h>
#include <d2d1helper.h>

class KinectSensor;

typedef long HRESULT;

class KinectWrapper
{
	
public:
	KinectWrapper();
	unsigned int * GetImage_color();
	USHORT * GetImage_depth();
	KinectSensor* sensor;
	HRESULT hr;
};