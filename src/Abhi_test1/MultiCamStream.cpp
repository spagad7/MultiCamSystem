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

// This function streams videos from multiple cameras 
int AcquireImages(CameraList camList)
{
	int result = 0;
	CameraPtr pCam = NULL;

	cout << endl << "*** IMAGE ACQUISITION ***" << endl << endl;

	try
	{
		//
		// Prepare each camera to acquire images
		// 
		// *** NOTES ***
		// For pseudo-simultaneous streaming, each camera is prepared as if it 
		// were just one, but in a loop. Notice that cameras are selected with 
		// an index. We demonstrate pseduo-simultaneous streaming because true 
		// simultaneous streaming would require multiple process or threads,
		// which is too complex for an example. 
		// 
		// Serial numbers are the only persistent objects we gather in this
		// example, which is why a vector is created.
		//
		vector<gcstring> strSerialNumbers(camList.GetSize());

		for (int i = 0; i < camList.GetSize(); i++)
		{
			// Select camera
			pCam = camList.GetByIndex(i);

			// Set acquisition mode to continuous
			CEnumerationPtr ptrAcquisitionMode = pCam->GetNodeMap().GetNode("AcquisitionMode");
			if (!IsAvailable(ptrAcquisitionMode) || !IsWritable(ptrAcquisitionMode))
			{
				cout << "Unable to set acquisition mode to continuous (node retrieval; camera " << i << "). Aborting..." << endl << endl;
				return -1;
			}

			CEnumEntryPtr ptrAcquisitionModeContinuous = ptrAcquisitionMode->GetEntryByName("Continuous");
			if (!IsAvailable(ptrAcquisitionModeContinuous) || !IsReadable(ptrAcquisitionModeContinuous))
			{
				cout << "Unable to set acquisition mode to continuous (entry 'continuous' retrieval " << i << "). Aborting..." << endl << endl;
				return -1;
			}

			int64_t acquisitionModeContinuous = ptrAcquisitionModeContinuous->GetValue();

			ptrAcquisitionMode->SetIntValue(acquisitionModeContinuous);

			cout << "Camera " << i << " acquisition mode set to continuous..." << endl;

			// Begin acquiring images
			pCam->BeginAcquisition();

			cout << "Camera " << i << " started acquiring images..." << endl;

			// Retrieve device serial number for filename
			strSerialNumbers[i] = "";

			CStringPtr ptrStringSerial = pCam->GetTLDeviceNodeMap().GetNode("DeviceSerialNumber");

			if (IsAvailable(ptrStringSerial) && IsReadable(ptrStringSerial))
			{
				strSerialNumbers[i] = ptrStringSerial->GetValue();
				cout << "Camera " << i << " serial number set to " << strSerialNumbers[i] << "..." << endl;
			}
			cout << endl;
		}

		//
		// Retrieve, convert, and save images and stream video for each camera
		//
		// *** NOTES ***
		// In order to work with simultaneous camera streams, nested loops are
		// needed. It is important that the inner loop be the one iterating
		// through the cameras; otherwise, all images will be grabbed from a
		// single camera before grabbing any images from another.
		//

		int imageCnt = 0;

		while(key!=27 && key!='q')
		{
			for (int i = 0; i < camList.GetSize(); i++)
			{
				try
				{
					// Select camera
					pCam = camList.GetByIndex(i);

					// Retrieve next received image and ensure image completion
					ImagePtr pResultImage = pCam->GetNextImage();

					if (pResultImage->IsIncomplete())
					{
						cout << "Image incomplete with image status " << pResultImage->GetImageStatus() << "..." << endl << endl;
					}
					else
					{
						// Convert image to mono 8
						ImagePtr convertedImage = 
							pResultImage->Convert(PixelFormat_Mono8, HQ_LINEAR);

						// Store images in OpenCV Mat.
						unsigned int rowBytes = 
							(int)convertedImage->GetImageSize()/convertedImage->GetHeight();
						
						Mat image = cv::Mat(convertedImage->GetHeight(),
							convertedImage->GetWidth(), CV_8UC1, convertedImage->GetData(),
							rowBytes);
					
						cv::resize(image, image, Size(800, 600), 0, 0, INTER_LINEAR);
		
						cv::imshow("image", image);
				
						// Create a unique filename
						ostringstream filename;

						filename << "Cam-";
						if (strSerialNumbers[i] != "")
						{
							filename << strSerialNumbers[i].c_str();
						}
						else
						{
							filename << i;
						}
						filename << "-" << imageCnt << ".jpg";
 
						char temp[1000];
						sprintf(temp, "/home/umh-admin/Downloads/spinnaker_1_0_0_295_amd64/bin/test/" + filename);
						cv::imwrite(temp, image);
						key = cv::waitKey(1);
					}

					// Release image
					pResultImage->Release();
					cout << endl;
				}
				catch (Spinnaker::Exception &e)
				{
					cout << "Error: " << e.what() << endl;
					result = -1;
				}
			}
			imageCnt++;
		}

		//
		// End acquisition for each camera
		//
		// *** NOTES ***
		// Notice that what is usually a one-step process is now two steps
		// because of the additional step of selecting the camera. It is worth
		// repeating that camera selection needs to be done once per loop.
		//
		// It is possible to interact with cameras through the camera list with
		// GetByIndex(); this is an alternative to retrieving cameras as 
		// CameraPtr objects that can be quick and easy for small tasks.
		//
		for (int i = 0; i < camList.GetSize(); i++)
		{
			// End acquisition
			camList.GetByIndex(i)->EndAcquisition();
		}
	}
	catch (Spinnaker::Exception &e)
	{
		cout << "Error: " << e.what() << endl;
		result = -1;
	}

	return result;
}


// This function takes care of initializing and deinitializing cameras. 
// This function calls function AcquireImages to stream videos from multiple cameras
int RunMultipleCameras(CameraList camList)
{
	int result = 0;
	CameraPtr pCam = NULL;

	try
	{
		
		//
		// Initialize each camera
		// 
		for (int i = 0; i < camList.GetSize(); i++)
		{
			// Select camera
			pCam = camList.GetByIndex(i);

			// Initialize camera
			pCam->Init();
		}
		
		// Acquire images on all cameras
		result = result | AcquireImages(camList);

		// 
		// Deinitialize each camera
		//
		for (int i = 0; i < camList.GetSize(); i++)
		{
			// Select camera
			pCam = camList.GetByIndex(i);

			// Deinitialize camera
			pCam->DeInit();
		}
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

	result = RunMultipleCameras(camList);

	cout << "Video Stream Ended" << endl << endl;

	// Clear camera list before releasing system
	camList.Clear();

	// Release system
	system->ReleaseInstance();

	cout << endl << "Done! Press Enter to exit..." << endl;
	getchar();

	return result;
}
