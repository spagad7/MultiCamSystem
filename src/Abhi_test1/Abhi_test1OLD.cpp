/**
 *	@example Acquisition.cpp
 *
 *	@brief Acquisition.cpp shows how to acquire images. It relies on 
 *	information provided in the Enumeration example. Also, check out the
 *	ExceptionHandling and NodeMapInfo examples if you haven't already. 
 *	ExceptionHandling shows the handling of standard and Spinnaker exceptions
 *	while NodeMapInfo explores retrieving information from various node types.  
 *
 *	This example touches on the preparation and cleanup of a camera just before
 *	and just after the acquisition of images. Image retrieval and conversion,
 *	grabbing image data, and saving images are all covered as well.
 *
 *	Once comfortable with Acquisition, we suggest checking out 
 *	AcquisitionMultipleCamera, NodeMapCallback, or SaveToAvi. 
 *	AcquisitionMultipleCamera demonstrates simultaneously acquiring images from 
 *	a number of cameras, NodeMapCallback serves as a good introduction to 
 *	programming with callbacks and events, and SaveToAvi exhibits video creation.
 */
 
#include "Spinnaker.h"
#include "SpinGenApi/SpinnakerGenApi.h"
#include <iostream>
#include <sstream> 
#include <sys/timeb.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>

using namespace Spinnaker;
using namespace Spinnaker::GenApi;
using namespace Spinnaker::GenICam;
using namespace std;
using namespace cv;

int getMilliCount(){
	timeb tb;
	ftime(&tb);
	int nCount = tb.millitm + (tb.time & 0xfffff) * 1000;
	return nCount;
}

int getMilliSpan(int nTimeStart){
	int nSpan = getMilliCount() - nTimeStart;
	if(nSpan < 0)
		nSpan += 0x100000 * 1000;
	return nSpan;
}

// This function acquires and live stream images from a device.  
int AcquireImages(CameraPtr pCam, INodeMap & nodeMap, INodeMap & nodeMapTLDevice)
{
	int result = 0;
	int counter = 0;
	
	try
	{
		//
		// Set acquisition mode to continuous
		//
		// *** NOTES ***
		// Because the example acquires and saves 10 images, setting acquisition 
		// mode to continuous lets the example finish. If set to single frame
		// or multiframe (at a lower number of images), the example would just
		// hang. This would happen because the example has been written to
		// acquire 10 images while the camera would have been programmed to 
		// retrieve less than that.
		// 
		// Setting the value of an enumeration node is slightly more complicated
		// than other node types. Two nodes must be retrieved: first, the 
		// enumeration node is retrieved from the nodemap; and second, the entry
		// node is retrieved from the enumeration node. The integer value of the
		// entry node is then set as the new value of the enumeration node.
		//
		// Notice that both the enumeration and the entry nodes are checked for
		// availability and readability/writability. Enumeration nodes are
		// generally readable and writable whereas their entry nodes are only
		// ever readable.
		// 
		// Retrieve enumeration node from nodemap
		CEnumerationPtr ptrAcquisitionMode = nodeMap.GetNode("AcquisitionMode");
		if (!IsAvailable(ptrAcquisitionMode) || !IsWritable(ptrAcquisitionMode))
		{
			cout << "Unable to set acquisition mode to continuous "
				"(enum retrieval). Aborting..." << endl << endl;
			return -1;
		}
		
		// Retrieve entry node from enumeration node
		CEnumEntryPtr ptrAcquisitionModeContinuous 
			= ptrAcquisitionMode->GetEntryByName("Continuous");
		
		if (!IsAvailable(ptrAcquisitionModeContinuous) || !IsReadable(ptrAcquisitionModeContinuous))
		{
			cout << "Unable to set acquisition mode to continuous "
				"(entry retrieval). Aborting..." << endl << endl;
			return -1;
		}
		
		// Retrieve Acquisition frame rate
		CFloatPtr ptrAcquisitionResultingFrameRate = nodeMap.GetNode("AcquisitionResultingFrameRate");
		if (!IsAvailable(ptrAcquisitionFrameRate) || !IsReadable(ptrAcquisitionFrameRate))
		{
			cout << "Unable to retrieve frame rate. Aborting..." << endl << endl;
			return -1;
		}

		float resultingFrameRate = static_cast<float>(ptrAcquisitionResultingFrameRate->GetValue());

		cout << "Resulting Frame Rate: " << resultingFrameRate << "...";
		
		// Retrieve integer value from entry node
		int64_t acquisitionModeContinuous = ptrAcquisitionModeContinuous->GetValue();
		
		// Set integer value from entry node as new value of enumeration node
		ptrAcquisitionMode->SetIntValue(acquisitionModeContinuous);
		
		//cout << "Acquisition mode set to continuous..." << endl;
		
		//
		// Begin acquiring images
		//
		// *** NOTES ***
		// What happens when the camera begins acquiring images depends on the
		// acquisition mode. Single frame captures only a single image, multi 
		// frame catures a set number of images, and continuous captures a 
		// continuous stream of images. Because the example calls for the 
		// retrieval of 10 images, continuous mode has been set.
		// 
		// *** LATER ***
		// Image acquisition must be ended when no more images are needed.
		//
		pCam->BeginAcquisition();

		cout << "Acquiring images..." << endl;
		
		//-------------------Changes start from here onwards--------------------//
		// Retrieve, convert, and save images
		//const unsigned int k_numImages = 20;
		
		char key = 0;
		
		int start = getMilliCount();
		
		//for (unsigned int imageCnt = 0; imageCnt < k_numImages; imageCnt++)
		while(key!=27 && key!='q')
		{
			try
			{
				//
				// Retrieve next received image
				//
				// *** NOTES ***
				// Capturing an image houses images on the camera buffer. Trying
				// to capture an image that does not exist will hang the camera.
				//
				// *** LATER ***
				// Once an image from the buffer is saved and/or no longer 
				// needed, the image must be released in order to keep the 
				// buffer from filling up.
				//
				
				ImagePtr pResultImage = pCam->GetNextImage();

				//
				// Ensure image completion
				//
				// *** NOTES ***
				// Images can easily be checked for completion. This should be
				// done whenever a complete image is expected or required.
				// Further, check image status for a little more insight into
				// why an image is incomplete.
				//
				if (pResultImage->IsIncomplete())
				{
					
					cout << "Image incomplete with image status " << 
						pResultImage->GetImageStatus() << "..." << endl << endl;
				}
				else
				{
					//
					// Convert image to mono 8
					//
					// *** NOTES ***
					// Images can be converted between pixel formats by using 
					// the appropriate enumeration value. Unlike the original 
					// image, the converted one does not need to be released as 
					// it does not affect the camera buffer.
					//
					// When converting images, color processing algorithm is an
					// optional parameter.
					// 
					ImagePtr convertedImage = 
						pResultImage->Convert(PixelFormat_Mono8, HQ_LINEAR);

					

					// Storing the images in OpenCV Mat and showing it.
					
					unsigned int rowBytes = 
						(int)convertedImage->GetImageSize()/convertedImage->GetHeight();
						
					Mat image = cv::Mat(convertedImage->GetHeight(),
						convertedImage->GetWidth(), CV_8UC1, convertedImage->GetData(),
						rowBytes);
					
					cv::resize(image, image, Size(800, 600), 0, 0, INTER_LINEAR);
					
					cv::imshow("image", image);
					/*char temp[1000];
					
					sprintf(temp, "/home/umh-admin/Downloads/spinnaker_1_0_0_295_amd64/bin/test/image%07d.jpg", counter);
					counter++;
					cv::imwrite(temp, image);*/
					key = cv::waitKey(1);
				}

				//
				// Release image
				//
				// *** NOTES ***
				// Images retrieved directly from the camera (i.e. non-converted
				// images) need to be released in order to keep from filling the
				// buffer.
				//
				pResultImage->Release();

			}
			catch (Spinnaker::Exception &e)
			{
				cout << "Error: " << e.what() << endl;
				result = -1;
			}
		}
		int milliSecondsElapsed = getMilliSpan(start);

		cout << "End system Time in milliseconds is " << milliSecondsElapsed << "." << endl;

		//
		// End acquisition
		//
		// *** NOTES ***
		// Ending acquisition appropriately helps ensure that devices clean up
		// properly and do not need to be power-cycled to maintain integrity.
		//
		pCam->EndAcquisition();
	}
	catch (Spinnaker::Exception &e)
	{
		cout << "Error: " << e.what() << endl;
		result = -1;
	}
	
	return result;
}

// This function prints the device information of the camera from the transport
// layer; please see NodeMapInfo example for more in-depth comments on printing
// device information from the nodemap.
int PrintDeviceInfo(INodeMap & nodeMap)
{
	int result = 0;
	
	cout << endl << "*** DEVICE INFORMATION ***" << endl << endl;

	try
	{
		FeatureList_t features;
		CCategoryPtr category = nodeMap.GetNode("DeviceInformation");
		if (IsAvailable(category) && IsReadable(category))
		{
			category->GetFeatures(features);

			FeatureList_t::const_iterator it;
			for (it = features.begin(); it != features.end(); ++it)
			{
				CNodePtr pfeatureNode = *it;
				cout << pfeatureNode->GetName() << " : ";
				CValuePtr pValue = (CValuePtr)pfeatureNode;
				cout << (IsReadable(pValue) ? pValue->ToString() : "Node not readable");
				cout << endl;
			}
		}
		else
		{
			cout << "Device control information not available." << endl;
		}
	}
	catch (Spinnaker::Exception &e)
	{
		cout << "Error: " << e.what() << endl;
		result = -1;
	}
	
	return result;
}

// This function acts as the body of the example; please see NodeMapInfo example 
// for more in-depth comments on setting up cameras.
int RunSingleCamera(CameraPtr pCam)
{
	int result = 0;

	try
	{
		// Retrieve TL device nodemap and print device information
		INodeMap & nodeMapTLDevice = pCam->GetTLDeviceNodeMap();
		
		//result = PrintDeviceInfo(nodeMapTLDevice);
		
		// Initialize camera
		pCam->Init();
		
		// Retrieve GenICam nodemap
		INodeMap & nodeMap = pCam->GetNodeMap();

		// Acquire images
		result = result | AcquireImages(pCam, nodeMap, nodeMapTLDevice);
		
		// Deinitialize camera
		pCam->DeInit();
	}
	catch (Spinnaker::Exception &e)
	{
		cout << "Error: " << e.what() << endl;
		result = -1;
	}

	return result;
}

// Example entry point; please see Enumeration example for more in-depth 
// comments on preparing and cleaning up the system.
int main(int /*argc*/, char** /*argv*/)
{
	int result = 0;
	
	// Print application build information
	cout << "Application build date: " << __DATE__ << " " << __TIME__ << endl << endl;
	
	// Retrieve singleton reference to system object
	SystemPtr system = System::GetInstance();

	// Retrieve list of cameras from the system
	CameraList camList = system->GetCameras();

	unsigned int numCameras = camList.GetSize();
	
	cout << "Number of cameras detected: " << numCameras << endl << endl;
	
	// Finish if there are no cameras
	if (numCameras == 0)
	{
		// Clear camera list before releasing system
		camList.Clear();

		// Release system
		system->ReleaseInstance();

		cout << "Not enough cameras!" << endl;
		cout << "Done! Press Enter to exit..." << endl;
		getchar();
		
		return -1;
	}

	//
	// Create shared pointer to camera
	//
	// *** NOTES ***
	// The CameraPtr object is a shared pointer, and will generally clean itself
	// up upon exiting its scope. However, if a shared pointer is created in the
	// same scope that a system object is explicitly released (i.e. this scope),
	// the reference to the shared point must be broken manually.
	//
	// *** LATER ***
	// Shared pointers can be terminated manually by assigning them to NULL.
	// This keeps releasing the system from throwing an exception.
	//
	CameraPtr pCam = NULL;
	pCam = camList.GetByIndex(0);
	// Run example on each camera
	/*for (unsigned int i = 0; i < numCameras; i++)
	{
		// Select camera
		pCam = camList.GetByIndex(i);

		cout << endl << "Running example for camera " << i << "..." << endl;
		*/
		// Run example
		result = result | RunSingleCamera(pCam);
		/*
		cout << "Camera " << i << " example complete..." << endl << endl;
	}*/

	//
	// Release reference to the camera
	//
	// *** NOTES ***
	// Had the CameraPtr object been created within the for-loop, it would not
	// be necessary to manually break the reference because the shared pointer
	// would have automatically cleaned itself up upon exiting the loop.
	//
	pCam = NULL;

	// Clear camera list before releasing system
	camList.Clear();

	// Release system
	system->ReleaseInstance();

	cout << endl << "Done! Press Enter to exit..." << endl;
	getchar();

	return result;
}
