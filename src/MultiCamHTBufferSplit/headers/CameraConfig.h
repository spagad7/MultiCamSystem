#ifndef CAMERA_H
#define CAMERA_H

// Camera Config
int ConfigureCamera(CameraPtr pCam, INodeMap & nodeMap);
void emptyImageBuffer(CameraList camList);
int RunMultipleCameras(CameraList camList);

#endif
