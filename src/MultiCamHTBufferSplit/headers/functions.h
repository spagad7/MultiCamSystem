/*
using namespace Spinnaker;
using namespace Spinnaker::GenApi;
using namespace Spinnaker::GenICam;
using namespace std;
using namespace cv;
*/

#ifndef FUNCTIONS_H
#define FUNCTIONS_H

// Trigger Config
int ConfigureTrigger(INodeMap & nodeMap, bool isPrimary);
int ResetTrigger(INodeMap & nodeMap);
int GrabNextImageByTrigger(INodeMap & nodeMap, CameraPtr pCam);

// Camera Config
int ConfigureCamera(CameraPtr pCam, INodeMap & nodeMap);
void emptyImageBuffer(CameraList camList);
int RunMultipleCameras(CameraList camList);

// Image Acquisiton
int AcquireImages(CameraList camList);

// Miscellaneous
int getMilliCount();
int getMilliSpan(int nTimeStart);

#endif
