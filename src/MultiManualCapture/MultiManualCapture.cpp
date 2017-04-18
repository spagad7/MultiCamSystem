//=============================================================================
// Copyright � 2015 Point Grey Research, Inc. All Rights Reserved.
//
// This software is the confidential and proprietary information of
// Point Grey Research, Inc. ("Confidential Information"). You shall not
// disclose such Confidential Information and shall use it only in
// accordance with the terms of the "License Agreement" that you
// entered into with PGR in connection with this software.
//
// UNLESS OTHERWISE SET OUT IN THE LICENSE AGREEMENT, THIS SOFTWARE IS
// PROVIDED ON AN �AS-IS� BASIS AND POINT GREY RESEARCH INC. MAKES NO
// REPRESENTATIONS OR WARRANTIES ABOUT THE SOFTWARE, EITHER EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO ANY IMPLIED WARRANTIES OR
// CONDITIONS OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, OR
// NON-INFRINGEMENT. POINT GREY RESEARCH INC. SHALL NOT BE LIABLE FOR ANY
// DAMAGES, INCLUDING BUT NOT LIMITED TO ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, PUNITIVE, OR CONSEQUENTIAL DAMAGES, OR ANY LOSS OF PROFITS,
// REVENUE, DATA OR DATA USE, ARISING OUT OF OR IN CONNECTION WITH THIS
// SOFTWARE OR OTHERWISE SUFFERED BY YOU AS A RESULT OF USING, MODIFYING
// OR DISTRIBUTING THIS SOFTWARE OR ITS DERIVATIVES.
//=============================================================================

/**
 *  @example AcquisitionMultipleCamera.cpp
 *
 *  @brief AcquisitionMultipleCamera.cpp shows how to capture images from
 *  multiple cameras simultaneously. It relies on information provided in the
 *	Enumeration, Acquisition, and NodeMapInfo examples.
 *
 *	This example reads similarly to the Acquisition example, except that loops
 *	and vectors are used to allow for simultaneous acquisitions.
 */

#include "Spinnaker.h"
#include "SpinGenApi/SpinnakerGenApi.h"
#include <iostream>
#include <sstream>
#include <string>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>

using namespace Spinnaker;
using namespace Spinnaker::GenApi;
using namespace Spinnaker::GenICam;
using namespace std;
using namespace cv;

int PrintDeviceInfo(INodeMap & nodeMap, unsigned int camNum)
{
	int result = 0;

	cout << "Printing device information for camera " << camNum << "..." << endl << endl;

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
	cout << endl;

	return result;
}

// This function acquires and live stream images from each device.
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
			CEnumerationPtr ptrAcquisitionMode
				= pCam->GetNodeMap().GetNode("AcquisitionMode");
			if (!IsAvailable(ptrAcquisitionMode) || !IsWritable(ptrAcquisitionMode))
			{
				cout << "Unable to set acquisition mode to continuous (node retrieval; camera " << i << "). Aborting..." << endl << endl;
				return -1;
			}

			CEnumEntryPtr ptrAcquisitionModeContinuous
			    = ptrAcquisitionMode->GetEntryByName("Continuous");
			if (!IsAvailable(ptrAcquisitionModeContinuous) ||
				!IsReadable(ptrAcquisitionModeContinuous))
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
		}

		//
		// Retrieve, convert, and save images for each camera
		//
		// *** NOTES ***
		// In order to work with simultaneous camera streams, nested loops are
		// needed. It is important that the inner loop be the one iterating
		// through the cameras; otherwise, all images will be grabbed from a
		// single camera before grabbing any images from another.
		//
		char key = 0;
		char ch;
  		bool saveImg = 0;
		cv::Mat src[10];
		string label = "Cam";
		int imgCount = 1;

		while(key!='q')
		{
			cout << "Press Enter to capture images" << endl;
			ch = getchar();
			if(ch == 'q')
				break;

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
						// Convert image to BGR8
						ImagePtr convertedImage
							= pResultImage->Convert(PixelFormat_Mono8, HQ_LINEAR);

						unsigned int rowBytes
							= (int)convertedImage->GetImageSize()/convertedImage->GetHeight();


						src[i] = cv::Mat(convertedImage->GetHeight(),
							convertedImage->GetWidth(), CV_8UC1, convertedImage->GetData(),
							rowBytes);

						//cv::resize(src[i], src[i], Size(640, 480), 0,0, INTER_LINEAR);

						//cv::namedWindow("image", 1);
						cv::imshow(label+to_string(i), src[i]);

						string filename = "/home/umh-admin/LabWork/MultiCamSystem/images/" + to_string(i) + "_" + to_string(imgCount) + ".jpg";
						imwrite(filename, src[i]);

						key = cv::waitKey(1);


#if 0
int dstWidth = convertedImage->GetWidth();
int dstHeight = convertedImage->GetHeight()/2;

cv::Mat dst = cv::Mat(Size(dstWidth, dstHeight), CV_8UC3, 2);

// Copy first image to dst
if(i==1) {
	src[i].copyTo(dst(Rect(0,0,convertedImage->GetWidth()/2, convertedImage->GetHeight()/2)));
}

// Copy second image to dst
if(i==2) {
	src[i].copyTo(dst(Rect(convertedImage->GetWidth()/2, 0, convertedImage->GetWidth(), convertedImage->GetHeight()/2)) );
}

//show all in a single window
cv::namedWindow( "image", CV_WINDOW_AUTOSIZE );
cv::imshow( "image", dst );
#endif

					}

					pResultImage->Release();

				}
				catch (Spinnaker::Exception &e)
				{
					cout << "Error: " << e.what() << endl;
					result = -1;
				}
			}
			imgCount++;			
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
		sleep(5);
		for (int i = 0; i < camList.GetSize(); i++)
		{
			// End acquisition
			camList.GetByIndex(i)->EndAcquisition();
			sleep(1);
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
int RunMultipleCameras(CameraList camList)
{
	int result = 0;
	CameraPtr pCam = NULL;

	try
	{
		//
		// Retrieve transport layer nodemaps and print device information for
		// each camera
		//
		// *** NOTES ***
		// This example retrieves information from the transport layer nodemap
		// twice: once to print device information and once to grab the device
		// serial number. Rather than caching the nodemap, each nodemap is
		// retrieved both times as needed.
		//
		cout << endl << "*** DEVICE INFORMATION ***" << endl << endl;

		for (int i = 0; i < camList.GetSize(); i++)
		{
			// Select camera
			pCam = camList.GetByIndex(i);

			// Retrieve TL device nodemap
			INodeMap & nodeMapTLDevice = pCam->GetTLDeviceNodeMap();

			// Print device information
			result = PrintDeviceInfo(nodeMapTLDevice, i);
		}

		//
		// Initialize each camera
		//
		// *** NOTES ***
		// You may notice that the steps in this function have more loops with
		// less steps per loop; this contrasts the AcquireImages() function
		// which has less loops but more steps per loop. This is done for
		// demonstrative purposes as both work equally well.
		//
		// *** LATER ***
		// Each camera needs to be deinitialized once all images have been
		// acquired.
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
		// *** NOTES ***
		// Again, each camera must be deinitialized separately by first
		// selecting the camera and then deinitializing it.
		//
		sleep(5);
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

	// Run example on all cameras
	cout << endl << "Running example for all cameras..." << endl;

	result = RunMultipleCameras(camList);

	cout << "Example complete..." << endl << endl;

	// Clear camera list before releasing system
	camList.Clear();

	// Release system
	system->ReleaseInstance();

	cout << endl << "Done! Press Enter to exit..." << endl;
	getchar();

	return result;
}
