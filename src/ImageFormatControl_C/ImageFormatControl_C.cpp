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
 *	@example ImageFormatControl_C.cpp
 *
 *	@brief ImageFormatControl_C.cpp shows how to apply custom image settings to
 *	the camera. It relies on information provided in the Enumeration_C,
 *	Acquisition_C, and NodeMapInfo_C examples.
 *
 *	This example demonstrates setting minimums to offsets, X and Y, and 
 *	maximums to width and height. It also shows the setting of a new pixel 
 *	format, which is an enumeration type node.
 *
 *	Following this, we suggest familiarizing yourself with the Exposure_C 
 *	example if you haven't already. Exposure is another example on camera 
 *	customization that is shorter and simpler than many of the others. Once 
 *	comfortable with Exposure_C and ImageFormatControl_C, we suggest checking 
 *	out any of the longer, more complicated examples related to camera 
 *	configuration: ChunkData_C, LookupTable_C, Sequencer_C, or Trigger_C.
 */

#include "SpinnakerC.h"
#include "stdio.h"
#include "string.h"

// Compiler warning C4996 suppressed due to deprecated strcpy() and sprintf() 
// functions on Windows platform.
#if defined WIN32 || defined _WIN32 || defined WIN64 || defined _WIN64 
	#pragma warning(disable : 4996)
#endif

// This macro helps with C-strings.
#define MAX_BUFF_LEN 256

// This function configures a number of settings on the camera including 
// offsets X and Y, width, height, and pixel format. These settings will be 
// applied before spinCameraBeginAcquisition() is called; otherwise, they will 
// be read only. Also, it is important to note that settings are applied 
// immediately. This means if you plan to reduce the width and move the x 
// offset accordingly, you need to apply such changes in the appropriate order.
spinError ConfigureCustomImageSettings(spinNodeMapHandle hNodeMap)
{
	spinError err = SPINNAKER_ERR_SUCCESS;

	printf("\n\n*** CONFIGURING CUSTOM IMAGE SETTINGS ***\n\n");

	//
	// Apply mono 8 pixel format
	//
	// *** NOTES ***
	// Enumeration nodes are slightly more complicated to set than other
	// nodes. This is because setting an enumeration node requires working
	// with two nodes instead of the usual one. 
	//
	// As such, there are a number of steps to setting an enumeration node:
	// retrieve the enumeration node from the nodemap, retrieve the desired
	// entry node from the enumeration node, retrieve the integer value 
	// from the entry node, and set the new value of the enumeration node
	// with the integer value from the entry node.
	//
	// It is important to note that there are two sets of functions that might
	// produce erroneous results if they were to be mixed up. The first two 
	// functions, spinEnumerationSetIntValue() and 
	// spinEnumerationEntryGetIntValue(), use the integer values stored on each
	// individual cameras. The second two, spinEnumerationSetEnumValue() and
	// spinEnumerationEntryGetEnumValue(), use enum values defined in the 
	// Spinnaker library. The int and enum values will most likely be
	// different from another.
	//
	spinNodeHandle hPixelFormat = NULL;
	spinNodeHandle hPixelFormatMono8 = NULL;
	int64_t pixelFormatMono8 = 0;

	// Retrieve enumeration node from the nodemap
	err = spinNodeMapGetNode(hNodeMap, "PixelFormat", &hPixelFormat);
	if (err != SPINNAKER_ERR_SUCCESS)
	{
		printf("Unable to set pixel format (node retrieval). Aborting with error %d...\n\n", err);
		return err;
	}

	// Retrieve desired entry node from the enumeration node
	err = spinEnumerationGetEntryByName(hPixelFormat, "Mono8", &hPixelFormatMono8);
	if (err != SPINNAKER_ERR_SUCCESS)
	{
		printf("Unable to set pixel format (enum entry retrieval). Aborting with error %d...\n\n", err);
		return err;
	}

	// Retrieve integer value from entry node
	err = spinEnumerationEntryGetIntValue(hPixelFormatMono8, &pixelFormatMono8);
	if (err != SPINNAKER_ERR_SUCCESS)
	{
		printf("Unable to set pixel format (enum entry int value retrieval). Aborting with error %d...\n\n", err);
		return err;
	}

	// Set integer as new value for enumeration node
	err = spinEnumerationSetIntValue(hPixelFormat, pixelFormatMono8);
	if (err != SPINNAKER_ERR_SUCCESS)
	{
		printf("Unable to set pixel format (enum entry setting). Aborting with error %d...\n\n", err);
		return err;
	}

	printf("Pixel format set to 'mono8'...\n");
	
	// 
	// Apply minimum to offset X
	//
	// *** NOTES ***
	// Numeric nodes have both a minimum and maximum. A minimum is retrieved
	// with the method GetMin(). Sometimes it can be important to check 
	// minimums to ensure that your desired value is within range.
	//
	// Notice that the node type is explicitly expressed in the name of the
	// second and third functions. Although node types are not expressed in 
	// node handles, knowing the node type is important to interacting with
	// a node in any meaningful way.
	// 
	spinNodeHandle hOffsetX = NULL;
	int64_t offsetXMin = 0;

	err = spinNodeMapGetNode(hNodeMap, "OffsetX", &hOffsetX);
	if (err != SPINNAKER_ERR_SUCCESS)
	{
		printf("Unable to set offset x. Aborting with error %d...\n\n", err);
		return err;
	}

	err = spinIntegerGetMin(hOffsetX, &offsetXMin);
	if (err != SPINNAKER_ERR_SUCCESS)
	{
		printf("Unable to set offset x. Aborting with error %d...\n\n", err);
		return err;
	}

	err = spinIntegerSetValue(hOffsetX, offsetXMin);
	if (err != SPINNAKER_ERR_SUCCESS)
	{
		printf("Unable to set offset x. Aborting with error %d...\n\n", err);
		return err;
	}

	printf("Offset X set to %d...\n", (int)offsetXMin);

	//
	// Apply minimum to offset Y
	// 
	// *** NOTES ***
	// It is often desirable to check the increment as well. The increment
	// is a number of which a desired value must be a multiple. Certain
	// nodes, such as those corresponding to offsets X and Y, have an
	// increment of 1, which basically means that any value within range
	// is appropriate. The increment is retrieved with the method 
	// spinIntegerGetInc().
	//
	// The offsets both hold integer values. As such, if a double were input 
	// as an argument or if a string function were used, problems would
	// occur.
	//
	spinNodeHandle hOffsetY = NULL;
	int64_t offsetYMin = 0;

	err = spinNodeMapGetNode(hNodeMap, "OffsetY", &hOffsetY);
	if (err != SPINNAKER_ERR_SUCCESS)
	{
		printf("Unable to set offset y. Aborting with error %d...\n\n", err);
		return err;
	}

	err = spinIntegerGetMax(hOffsetY, &offsetYMin);
	if (err != SPINNAKER_ERR_SUCCESS)
	{
		printf("Unable to set offset y. Aborting with error %d...\n\n", err);
		return err;
	}

	err = spinIntegerSetValue(hOffsetY, offsetYMin);
	if (err != SPINNAKER_ERR_SUCCESS)
	{
		printf("Unable to set offset y. Aborting with error %d...\n\n", err);
		return err;
	}

	printf("Offset Y set to %d...\n", (int)offsetYMin);

	//
	// Set maximum width
	//
	// *** NOTES ***
	// Other nodes, such as those corresponding to image width and height, 
	// might have an increment other than 1. In these cases, it can be
	// important to check that the desired value is a multiple of the
	// increment. 
	// 
	// This is often the case for width and height nodes. However, because
	// these nodes are being set to their maximums, there is no real reason
	// to check against the increment.
	//
	spinNodeHandle hWidth = NULL;
	int64_t widthToSet = 0;

	// Retrieve width node
	err = spinNodeMapGetNode(hNodeMap, "Width", &hWidth);
	if (err != SPINNAKER_ERR_SUCCESS)
	{
		printf("Unable to set width. Aborting with error %d...\n\n", err);
		return err;
	}

	// Retrieve maximum width
	err = spinIntegerGetMax(hWidth, &widthToSet);
	if (err != SPINNAKER_ERR_SUCCESS)
	{
		printf("Unable to set width. Aborting with error %d...\n\n", err);
		return err;
	}

	// Set width
	err = spinIntegerSetValue(hWidth, widthToSet);
	if (err != SPINNAKER_ERR_SUCCESS)
	{
		printf("Unable to set width. Aborting with error %d...\n\n", err);
		return err;
	}

	printf("Width set to %d...\n", (int)widthToSet);

	//
	// Set maximum height
	//
	// *** NOTES ***
	// A maximum is retrieved with the method spinIntegerGetMax(). A node's 
	// minimum and maximum should always be multiples of the increment.
	//
	spinNodeHandle hHeight = NULL;
	int64_t heightToSet = 0;

	// Retrieve node "Height"
	err = spinNodeMapGetNode(hNodeMap, "Height", &hHeight);
	if (err != SPINNAKER_ERR_SUCCESS)
	{
		printf("Unable to set height. Aborting with error %d...\n\n", err);
		return err;
	}

	// Retrieve maximum
	err = spinIntegerGetMax(hHeight, &heightToSet);
	if (err != SPINNAKER_ERR_SUCCESS)
	{
		printf("Unable to set height. Aborting with error %d...\n\n", err);
		return err;
	}

	// Set to desired value
	err = spinIntegerSetValue(hHeight, heightToSet);
	if (err != SPINNAKER_ERR_SUCCESS)
	{
		printf("Unable to set height. Aborting with error %d...\n\n", err);
		return err;
	}

	printf("Height set to %d...\n\n", (int)heightToSet);

	return err;
}

// This function prints the device information of the camera from the transport
// layer; please see NodeMapInfo_C example for more in-depth comments on 
// printing device information from the nodemap.
spinError PrintDeviceInfo(spinNodeMapHandle hNodeMap)
{
	spinError err = SPINNAKER_ERR_SUCCESS;
	unsigned int i = 0;

	printf("\n*** DEVICE INFORMATION ***\n\n");

	// Retrieve device information category node
	spinNodeHandle hDeviceInformation = NULL;

	err = spinNodeMapGetNode(hNodeMap, "DeviceInformation", &hDeviceInformation);
	if (err != SPINNAKER_ERR_SUCCESS)
	{
		printf("Unable to retrieve node. Non-fatal error %d...\n\n", err);
		return err;
	}

	// Retrieve number of nodes within device information node
	size_t numFeatures = 0;

	err = spinCategoryGetNumFeatures(hDeviceInformation, &numFeatures);
	if (err != SPINNAKER_ERR_SUCCESS)
	{
		printf("Unable to retrieve number of nodes. Non-fatal error %d...\n\n", err);
		return err;
	}

	// Iterate through nodes and print information
	for (i = 0; i < numFeatures; i++)
	{
		spinNodeHandle hFeatureNode = NULL;

		err = spinCategoryGetFeatureByIndex(hDeviceInformation, i, &hFeatureNode);
		if (err != SPINNAKER_ERR_SUCCESS)
		{
			printf("Unable to retrieve node. Non-fatal error %d...\n\n", err);
			continue;
		}

		spinNodeType featureType = UnknownNode;

		err = spinNodeGetType(hFeatureNode, &featureType);
		if (err != SPINNAKER_ERR_SUCCESS)
		{
			printf("Unable to retrieve node type. Non-fatal error %d...\n\n", err);
			continue;
		}

		char featureName[MAX_BUFF_LEN];
		size_t lenFeatureName = MAX_BUFF_LEN;
		char featureValue[MAX_BUFF_LEN];
		size_t lenFeatureValue = MAX_BUFF_LEN;

		err = spinNodeGetName(hFeatureNode, featureName, &lenFeatureName);
		if (err != SPINNAKER_ERR_SUCCESS)
		{
			strcpy(featureName, "Unknown name");
		}

		err = spinNodeToString(hFeatureNode, featureValue, &lenFeatureValue);
		if (err != SPINNAKER_ERR_SUCCESS)
		{
			strcpy(featureValue, "Unknown value");
		}

		printf("%s: %s\n", featureName, featureValue);
	}

	return err;
}

// This function acquires and saves 10 images from a device; please see
// Acquisition_C example for more in-depth comments on the acquisition of
// images.
spinError AcquireImages(spinCamera hCam, spinNodeMapHandle hNodeMap, spinNodeMapHandle hNodeMapTLDevice)
{
	spinError err = SPINNAKER_ERR_SUCCESS;

	printf("\n*** IMAGE ACQUISITION ***\n\n");

	// Set acquisition mode to continuous
	spinNodeHandle hAcquisitionMode = NULL;
	spinNodeHandle hAcquisitionModeContinuous = NULL;
	int64_t acquisitionModeContinuous = 0;

	err = spinNodeMapGetNode(hNodeMap, "AcquisitionMode", &hAcquisitionMode);
	if (err != SPINNAKER_ERR_SUCCESS)
	{
		printf("Unable to set acquisition mode to continuous (node retrieval). Aborting with error %d...\n\n", err);
		return err;
	}

	err = spinEnumerationGetEntryByName(hAcquisitionMode, "Continuous", &hAcquisitionModeContinuous);
	if (err != SPINNAKER_ERR_SUCCESS)
	{
		printf("Unable to set acquisition mode to continuous (entry 'continuous' retrieval). Aborting with error %d...\n\n", err);
		return err;
	}

	err = spinEnumerationEntryGetIntValue(hAcquisitionModeContinuous, &acquisitionModeContinuous);
	if (err != SPINNAKER_ERR_SUCCESS)
	{
		printf("Unable to set acquisition mode to continuous (entry int value retrieval). Aborting with error %d...\n\n", err);
		return err;
	}

	err = spinEnumerationSetIntValue(hAcquisitionMode, acquisitionModeContinuous);
	if (err != SPINNAKER_ERR_SUCCESS)
	{
		printf("Unable to set acquisition mode to continuous (entry int value setting). Aborting with error %d...\n\n", err);
		return err;
	}

	printf("Acquisition mode set to continuous...\n");

	// Begin acquiring images
	err = spinCameraBeginAcquisition(hCam);
	if (err != SPINNAKER_ERR_SUCCESS)
	{
		printf("Unable to begin image acquisition. Aborting with error %d...\n\n", err);
		return err;
	}

	printf("Acquiring images...\n");

	// Retrieve device serial number for filename
	spinNodeHandle hDeviceSerialNumber = NULL;
	char deviceSerialNumber[MAX_BUFF_LEN];
	size_t lenDeviceSerialNumber = MAX_BUFF_LEN;

	err = spinNodeMapGetNode(hNodeMapTLDevice, "DeviceSerialNumber", &hDeviceSerialNumber);
	if (err != SPINNAKER_ERR_SUCCESS)
	{
		strcpy(deviceSerialNumber, "");
		lenDeviceSerialNumber = 0;
	}
	else
	{
		err = spinStringGetValue(hDeviceSerialNumber, deviceSerialNumber, &lenDeviceSerialNumber);
		if (err != SPINNAKER_ERR_SUCCESS)
		{
			strcpy(deviceSerialNumber, "");
			lenDeviceSerialNumber = 0;
		}

		printf("Device serial number retrieved as %s...\n", deviceSerialNumber);
	}
	printf("\n");

	// Retrieve, convert, and save images
	const unsigned int k_numImages = 10;
	unsigned int imageCnt = 0;

	for (imageCnt = 0; imageCnt < k_numImages; imageCnt++)
	{
		// Retrieve next received image 
		spinImage hResultImage = NULL;
		bool8_t isIncomplete = False;
		bool8_t hasFailed = False;

		err = spinCameraGetNextImage(hCam, &hResultImage);
		if (err != SPINNAKER_ERR_SUCCESS)
		{
			printf("Unable to get next image. Non-fatal error %d...\n\n", err);
			continue;
		}

		// Ensure image completion
		err = spinImageIsIncomplete(hResultImage, &isIncomplete);
		if (err != SPINNAKER_ERR_SUCCESS)
		{
			printf("Unable to determine image completion. Non-fatal error %d...\n\n", err);
			hasFailed = True;
		}

		if (isIncomplete)
		{
			spinImageStatus imageStatus = IMAGE_NO_ERROR;

			err = spinImageGetStatus(hResultImage, &imageStatus);
			if (err != SPINNAKER_ERR_SUCCESS)
			{
				printf("Unable to retrieve image status. Non-fatal error %d...\n\n", err);
			}
			else
			{
				printf("Image incomplete with image status %d...\n", imageStatus);
			}

			hasFailed = True;
		}

		// Release incomplete or failed image
		if (hasFailed)
		{
			err = spinImageRelease(hResultImage);
			if (err != SPINNAKER_ERR_SUCCESS)
			{
				printf("Unable to release image. Non-fatal error %d...\n\n", err);
			}

			continue;
		}

		// Print image information
		size_t width = 0;
		size_t height = 0;

		err = spinImageGetWidth(hResultImage, &width);
		if (err != SPINNAKER_ERR_SUCCESS)
		{
			printf("Unable to retrieve image width. Non-fatal error %d...\n", err);
		}

		err = spinImageGetHeight(hResultImage, &height);
		if (err != SPINNAKER_ERR_SUCCESS)
		{
			printf("Unable to retrieve image height. Non-fatal error %d...\n", err);
		}

		printf("Grabbed image %u, width = %u, height = %u\n", imageCnt, (unsigned int)width, (unsigned int)height);

		// Convert image to mono 8
		spinImage hConvertedImage = NULL;

		err = spinImageCreateEmpty(&hConvertedImage);
		if (err != SPINNAKER_ERR_SUCCESS)
		{
			printf("Unable to create image. Non-fatal error %d...\n\n", err);
			hasFailed = True;
		}

		err = spinImageConvert(hResultImage, PixelFormat_Mono8, hConvertedImage);
		if (err != SPINNAKER_ERR_SUCCESS)
		{
			printf("Unable to convert image. Non-fatal error %d...\n\n", err);
			hasFailed = True;
		}

		// Create unique file name 
		char filename[MAX_BUFF_LEN];

		if (lenDeviceSerialNumber == 0)
		{
			sprintf(filename, "ImageFormatControl-C-%d.jpg", imageCnt);
		}
		else
		{
			sprintf(filename, "ImageFormatControl-C-%s-%d.jpg", deviceSerialNumber, imageCnt);
		}

		// Save image
		err = spinImageSave(hConvertedImage, filename, JPEG);
		if (err != SPINNAKER_ERR_SUCCESS)
		{
			printf("Unable to save image. Non-fatal error %d...\n", err);
		}
		else
		{
			printf("Image saved at %s\n\n", filename);
		}

		// Destroy converted image
		err = spinImageDestroy(hConvertedImage);
		if (err != SPINNAKER_ERR_SUCCESS)
		{
			printf("Unable to destroy image. Non-fatal error %d...\n\n", err);
		}

		// Release image
		err = spinImageRelease(hResultImage);
		if (err != SPINNAKER_ERR_SUCCESS)
		{
			printf("Unable to release image. Non-fatal error %d...\n\n", err);
		}
	}

	// End Acquisition
	err = spinCameraEndAcquisition(hCam);
	if (err != SPINNAKER_ERR_SUCCESS)
	{
		printf("Unable to end acquisition. Non-fatal error %d...\n\n", err);
	}

	return err;
}

// This function acts as the body of the example; please see NodeMapInfo_C 
// example for more in-depth comments on setting up cameras.
spinError RunSingleCamera(spinCamera hCam)
{
	spinError err = SPINNAKER_ERR_SUCCESS;

	// Retrieve TL device nodemap and print device information
	spinNodeMapHandle hNodeMapTLDevice = NULL;

	err = spinCameraGetTLDeviceNodeMap(hCam, &hNodeMapTLDevice);
	if (err != SPINNAKER_ERR_SUCCESS)
	{
		printf("Unable to retrieve TL device nodemap. Non-fatal error %d...\n\n", err);
	}
	else
	{
		err = PrintDeviceInfo(hNodeMapTLDevice);
	}

	// Initialize camera
	err = spinCameraInit(hCam);
	if (err != SPINNAKER_ERR_SUCCESS)
	{
		printf("Unable to initialize camera. Aborting with error %d...\n\n", err);
		return err;
	}

	// Retrieve GenICam nodemap
	spinNodeMapHandle hNodeMap = NULL;

	err = spinCameraGetNodeMap(hCam, &hNodeMap);
	if (err != SPINNAKER_ERR_SUCCESS)
	{
		printf("Unable to retrieve GenICam nodemap. Aborting with error %d...\n\n", err);
		return err;
	}

	// Configure custom image settings
	err = ConfigureCustomImageSettings(hNodeMap);
	if (err != SPINNAKER_ERR_SUCCESS)
	{
		return err;
	}

	// Acquire images
	err = AcquireImages(hCam, hNodeMap, hNodeMapTLDevice);
	if (err != SPINNAKER_ERR_SUCCESS)
	{
		return err;
	}

	// Deinitialize camera
	err = spinCameraDeInit(hCam);
	if (err != SPINNAKER_ERR_SUCCESS)
	{
		printf("Unable to deinitialize camera. Non-fatal error %d...\n\n", err);
	}

	return err;
}

// Example entry point; please see Enumeration_C example for more in-depth
// comments on preparing and cleaning up the system.
int main(/*int argc, char** argv*/)
{
	spinError errReturn = SPINNAKER_ERR_SUCCESS;
	spinError err = SPINNAKER_ERR_SUCCESS;
	unsigned int i = 0;

	// Print application build information
	printf("Application build date: %s %s \n\n", __DATE__, __TIME__);

	// Retrieve singleton reference to system object
	spinSystem hSystem = NULL;

	err = spinSystemGetInstance(&hSystem);
	if (err != SPINNAKER_ERR_SUCCESS)
	{
		printf("Unable to retrieve system instance. Aborting with error %d...\n\n", err);
		return err;
	}

	// Retrieve list of cameras from the system
	spinCameraList hCameraList = NULL;

	err = spinCameraListCreateEmpty(&hCameraList);
	if (err != SPINNAKER_ERR_SUCCESS)
	{
		printf("Unable to create camera list. Aborting with error %d...\n\n", err);
		return err;
	}

	err = spinSystemGetCameras(hSystem, hCameraList);
	if (err != SPINNAKER_ERR_SUCCESS)
	{
		printf("Unable to retrieve camera list. Aborting with error %d...\n\n", err);
		return err;
	}

	// Retrieve number of cameras
	size_t numCameras = 0;

	err = spinCameraListGetSize(hCameraList, &numCameras);
	if (err != SPINNAKER_ERR_SUCCESS)
	{
		printf("Unable to retrieve number of cameras. Aborting with error %d...\n\n", err);
		return err;
	}

	printf("Number of cameras detected: %u\n\n", (unsigned int)numCameras);

	// Finish if there are no cameras
	if (numCameras == 0)
	{
		// Clear and destroy camera list before releasing system
		err = spinCameraListClear(hCameraList);
		if (err != SPINNAKER_ERR_SUCCESS)
		{
			printf("Unable to clear camera list. Aborting with error %d...\n\n", err);
			return err;
		}

		err = spinCameraListDestroy(hCameraList);
		if (err != SPINNAKER_ERR_SUCCESS)
		{
			printf("Unable to destroy camera list. Aborting with error %d...\n\n", err);
			return err;
		}

		// Release system
		err = spinSystemReleaseInstance(hSystem);
		if (err != SPINNAKER_ERR_SUCCESS)
		{
			printf("Unable to release system instance. Aborting with error %d...\n\n", err);
			return err;
		}

		printf("Not enough cameras!\n");
		printf("Done! Press Enter to exit...\n");
		getchar();

		return -1;
	}

	// Run example on each camera
	for (i = 0; i < numCameras; i++)
	{
		printf("\nRunning example for camera %d...\n", i);

		// Select camera
		spinCamera hCamera = NULL;

		err = spinCameraListGet(hCameraList, i, &hCamera);
		if (err != SPINNAKER_ERR_SUCCESS)
		{
			printf("Unable to retrieve camera from list. Aborting with error %d...\n\n", err);
			errReturn = err;
		}
		else
		{
			// Run example
			err = RunSingleCamera(hCamera);
			if (err != SPINNAKER_ERR_SUCCESS)
			{
				errReturn = err;
			}
		}

		// Release camera
		err = spinCameraRelease(hCamera);
		if (err != SPINNAKER_ERR_SUCCESS)
		{
			errReturn = err;
		}

		printf("Camera %d example complete...\n\n", i);
	}

	// Clear and destroy camera list before releasing system
	err = spinCameraListClear(hCameraList);
	if (err != SPINNAKER_ERR_SUCCESS)
	{
		printf("Unable to clear camera list. Aborting with error %d...\n\n", err);
		return err;
	}

	err = spinCameraListDestroy(hCameraList);
	if (err != SPINNAKER_ERR_SUCCESS)
	{
		printf("Unable to destroy camera list. Aborting with error %d...\n\n", err);
		return err;
	}

	// Release system
	err = spinSystemReleaseInstance(hSystem);
	if (err != SPINNAKER_ERR_SUCCESS)
	{
		printf("Unable to release system instance. Aborting with error %d...\n\n", err);
		return err;
	}

	printf("\nDone! Press Enter to exit...\n");
	getchar();

	return errReturn;
}
