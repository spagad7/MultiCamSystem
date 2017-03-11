#include "Spinnaker.h"
#include "SpinGenApi/SpinnakerGenApi.h"
#include <iostream>
#include <sstream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <ctime>
#include <sys/timeb.h>
#include "../headers/main.h"
#include "../headers/TriggerConfig.h"
#include "../headers/CameraConfig.h"
#include "../headers/ImageAcquisition.h"

// Namespaces
using namespace Spinnaker;
using namespace Spinnaker::GenApi;
using namespace Spinnaker::GenICam;



// Use the following enum and global constant to select whether a software or
// hardware trigger is used.
enum triggerType
{
	SOFTWARE,
	HARDWARE
};

//const triggerType chosenTrigger = HARDWARE;
const triggerType chosenTrigger = HARDWARE;



//
//
// Init Functions
//
//


// Function to initialize and deinitialize each camera
int RunMultipleCameras(CameraList camList)
{
    int result = 0;
    CameraPtr pCam = NULL;
    bool isPrimary = true;

    try
    {
        // Initialize camera and trigger for each camera
        for (int i = 0; i < camList.GetSize(); i++)
        {
            // Select camera
            pCam = camList.GetByIndex(i);

			// Print Camera Serial Number
			cout << "Initializing Camera: " << i << " SerialNum:" << pCam->GetUniqueID() << endl;

            // Initialize camera
            pCam->Init();

            // Retrieve GenICam nodemap
            INodeMap & nodeMap = camList.GetByIndex(i)->GetNodeMap();

            // Set camera with index 0 as Primary camera.
			// FUTURE: Update this part of code to set camera with specific serial number as Primary
            if(i>0)
                isPrimary = false;

            //result = ConfigureTrigger(nodeMap, isPrimary);
            if (result < 0)
            {
				cout << "Error configuring trigger" << endl;
                return result;
            }

            result = ConfigureCamera(pCam, nodeMap);
			if (result < 0)
            {
				cout << "Error configuring camera" << endl;
                return result;
            }
        }// End of initialization of trigger and camera
		

        // Acquire images on all cameras
        result = AcquireImages(camList);
		if (result < 0)
            {
				cout << "Error acquiring images from cameras" << endl;
                return result;
            }


        // Deinitialize each camera
        for (int i = 0; i < camList.GetSize(); i++)
        {
            // Select camera
            pCam = camList.GetByIndex(i);

            // Retrieve GenICam nodemap
            INodeMap & nodeMap = pCam->GetNodeMap();

            // Reset trigger
            result = result | ResetTrigger(nodeMap);

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


// Init: Get conneceted cameras
int main(int /*argc*/, char** /*argv*/)
{
    int result = 0;

    // Print application build information
    cout << "Program build date: " << __DATE__ << " " << __TIME__ << endl << endl;

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

	//Acquire Images from Multiple Cameras
    result = RunMultipleCameras(camList);

    cout << "Closing Program. Doing Clean Up" << endl << endl;

    // Clear camera list before releasing system
    camList.Clear();

    // Release system
    system->ReleaseInstance();

    cout << endl << "Done! Press Enter to exit..." << endl;
    getchar();

    return result;
}
