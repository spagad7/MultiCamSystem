#ifndef TRIGGER_H
#define TRIGGER_H
// Trigger Config
int ConfigureTrigger(INodeMap & nodeMap, bool isPrimary);
int ResetTrigger(INodeMap & nodeMap);
int GrabNextImageByTrigger(INodeMap & nodeMap, CameraPtr pCam);

#endif
