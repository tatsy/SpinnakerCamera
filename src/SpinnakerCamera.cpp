#include "SpinnakerCamera.h"

#include <iostream>
#include <sstream>

#include "SpinGenApi/SpinnakerGenApi.h"

using namespace Spinnaker;
using namespace Spinnaker::GenApi;
using namespace Spinnaker::GenICam;

enum TriggerType {
    SOFTWARE, 
    HARDWARE
};

const TriggerType chosenTrigger = SOFTWARE;

namespace {

int PrintDeviceInfo(INodeMap &nodeMap) {
    int result = 0;

    std::cout << "*** DEVICE INFORMATION ***" << std::endl;
    
    try {
        FeatureList_t features;
        CCategoryPtr category = nodeMap.GetNode("DeviceInformation");
        if (IsAvailable(category) && IsReadable(category)) {
            category->GetFeatures(features);

            FeatureList_t::const_iterator it;
            for (it = features.begin(); it != features.end(); ++it) {
                CNodePtr pFeatureNode = *it;
                std::cout << pFeatureNode->GetName() << " : ";
                CValuePtr pValue = (CValuePtr)pFeatureNode;
                std::cout << (IsReadable(pValue) ? pValue->ToString() : "Node not readable");
                std::cout << std::endl;
            }
        } else {
            std::cout << "Device control information not available." << std::endl;
        }
    } catch (Spinnaker::Exception &e) {
        std::cout << "Error: " << e.what() << std::endl;
        result = -1;
    }

    return result;
}

int ConfigureTrigger(INodeMap &nodeMap) {
    int result = 0;

    std::cout << "*** CONFIGURING TRIGGER ***" << std::endl;

    if (chosenTrigger == SOFTWARE) {
        std::cout << "Software trigger chosen..." << std::endl;
    } else if (chosenTrigger == HARDWARE) {
        std::cout << "Hardware trigger chosen..." << std::endl;
    }

    try {
        // Disable trigger mode
        CEnumerationPtr pTriggerMode = nodeMap.GetNode("TriggerMode");
        if (!IsAvailable(pTriggerMode) || !IsReadable(pTriggerMode)) {
            std::cout << "Unable to disable trigger mode (node retrieval). Aborting..." << std::endl;
            return -1;
        }

        CEnumEntryPtr pTriggerModeOff = pTriggerMode->GetEntryByName("Off");
        if (!IsAvailable(pTriggerModeOff) || !IsReadable(pTriggerModeOff)) {
            std::cout << "Unable to disable trigger model (enum entry retrieval). Aborting..." << std::endl;
            return -1;
        }

        pTriggerMode->SetIntValue(pTriggerModeOff->GetValue());

        std::cout << "Trigger mode disabled..." << std::endl;

        // Chose trigger source
        CEnumerationPtr pTriggerSource = nodeMap.GetNode("TriggerSource");
        if (!IsAvailable(pTriggerSource) || !IsWritable(pTriggerSource)) {
            std::cout << "Unable to set trigger mode (node retrieval). Aborting..." << std::endl;
            return -1;
        }

        if (chosenTrigger == SOFTWARE) {
            CEnumEntryPtr pTriggerSourceSoftware = pTriggerSource->GetEntryByName("Software");
            if (!IsAvailable(pTriggerSourceSoftware) || !IsReadable(pTriggerSourceSoftware)) {
                std::cout << "Unable to set trigger mode (enum entry retrieval). Aborting..." << std::endl;
                return -1;
            }

            pTriggerSource->SetIntValue(pTriggerSourceSoftware->GetValue());

            std::cout << "Trigger source set to software..." << std::endl;
        } else if (chosenTrigger == HARDWARE) {
            CEnumEntryPtr pTriggerSourceHardware = pTriggerSource->GetEntryByName("Line0");
            if (!IsAvailable(pTriggerSourceHardware) || !IsReadable(pTriggerSourceHardware)) {
                std::cout << "Unable to set trigger mode(enum entry retrieval). Aborting..." << std::endl;
                return -1;
            }

            pTriggerSource->SetIntValue(pTriggerSourceHardware->GetValue());

            std::cout << "Trigger source set to hardware..." << std::endl;
        }

        CEnumEntryPtr pTriggerModeOn = pTriggerMode->GetEntryByName("On");
        if (!IsAvailable(pTriggerModeOn) || !IsReadable(pTriggerModeOn)) {
            std::cout << "Unable to enable trigger mode (enum entry retrieval). Aborting..." << std::endl;
            return -1;
        }

        pTriggerMode->SetIntValue(pTriggerModeOn->GetValue());

        std::cout << "Trigger mode turned back on..." << std::endl;
    } catch (Spinnaker::Exception &e) {
        std::cout << "Error: " << e.what() << std::endl;
        return -1;
    }

    return result;
}

int GrabNextImageByTrigger(INodeMap &nodeMap, CameraPtr pCam) {
    int result = 0;

    try {
        if (chosenTrigger == SOFTWARE) {
            std::cout << "Press the Enter key to initiate software trigger." << std::endl;
            getchar();

            // Execute software trigger
            CCommandPtr pSoftwareTriggerCommand = nodeMap.GetNode("TriggerSoftware");
            if (!IsAvailable(pSoftwareTriggerCommand) || !IsWritable(pSoftwareTriggerCommand)) {
                std::cout << "Unable to execute trigger. Aborting..." << std::endl;
                return -1;
            }

            pSoftwareTriggerCommand->Execute();
        } else if (chosenTrigger == HARDWARE) {
            // Execute hardware trigger
            std::cout << "Use the hardware to trigger image acquisition." << std::endl;
        }
    } catch (Spinnaker::Exception &e) {
        std::cout << "Error: " << e.what() << std::endl;
        result = -1;
    }

    return result;
}

int ResetTrigger(INodeMap &nodeMap) {
    int result = 0;

    try {
        CEnumerationPtr pTriggerMode = nodeMap.GetNode("TriggerMode");
        if (!IsAvailable(pTriggerMode) || !IsWritable(pTriggerMode)) {
            std::cout << "Unable to disable trigger mode (node retrieval). Non-fatal error" << std::endl;
            return -1;
        }

        CEnumEntryPtr pTriggerModeOff = pTriggerMode->GetEntryByName("Off");
        if (!IsAvailable(pTriggerModeOff) || !IsReadable(pTriggerModeOff)) {
            std::cout << "Unable to disable trigger mode (enum entry retrieval). Non-fatal error..." << std::endl;
            return -1;
        }

        pTriggerMode->SetIntValue(pTriggerModeOff->GetValue());

        std::cout << "Trigger mode disabled..." << std::endl;
    } catch (Spinnaker::Exception &e) {
        std::cout << "Error: " << e.what() << std::endl;
        result = -1;
    }

    return result;
}

int AcquireImages(CameraPtr pCam, INodeMap &nodeMap, INodeMap &nodeMapTLDevice) {
    int result = 0;

    std::cout << "*** IMAGE ACQUISITION ***" << std::endl;

    try {
        CEnumerationPtr pAcquisitionMode = nodeMap.GetNode("AcquisitionMode");
        if (!IsAvailable(pAcquisitionMode) || !IsWritable(pAcquisitionMode)) {
            std::cout << "Unable to set acquisition mode to cotinuous (enum retrieval). Aborting..." << std::endl;
            return -1;
        }

        CEnumEntryPtr pAcquisitionModeContinuous = pAcquisitionMode->GetEntryByName("Continuous");
        if (!IsAvailable(pAcquisitionModeContinuous) || !IsReadable(pAcquisitionModeContinuous)) {
            std::cout << "Unable to set acqusition mode to continous (enum retrieval). Aborting..." << std::endl;
            return -1;
        }

        int64_t acquisitionModeContinuous = pAcquisitionModeContinuous->GetValue();
        pAcquisitionMode->SetIntValue(acquisitionModeContinuous);

        std::cout << "Acquisition mode set to continous" << std::endl;

        pCam->BeginAcquisition();

        std::cout << "Acquireing images..." << std::endl;

        gcstring deviceSerialNumber("");
        CStringPtr pStringSerial = nodeMapTLDevice.GetNode("DeviceSerialNumber");
        if (IsAvailable(pStringSerial) || IsReadable(pStringSerial)) {
            deviceSerialNumber = pStringSerial->GetValue();
            std::cout << "Device serial number retrieved as " << deviceSerialNumber << "..." << std::endl;
        }
        std::cout << std::endl;

        const uint32_t k_numImages = 10;

        for (uint32_t imageCount = 0; imageCount < k_numImages; imageCount++) {
            try {

                result = result | GrabNextImageByTrigger(nodeMap, pCam);

                ImagePtr pResultImage = pCam->GetNextImage();

                if (pResultImage->IsIncomplete()) {
                    std::cout << "Image incomplete: "
                        << Image::GetImageStatusDescription(pResultImage->GetImageStatus())
                        << "..." << std::endl;
                } else {
                    const size_t width = pResultImage->GetWidth();
                    const size_t height = pResultImage->GetHeight();
                    std::cout << "Grabbed image " << imageCount << ", width = " << width << ", height = " << height << std::endl;

                    ImagePtr convertedImage = pResultImage->Convert(PixelFormat_RGB8, HQ_LINEAR);

                    std::ostringstream filename;

                    filename << "Acquisition-";
                    if (deviceSerialNumber != "") {
                        filename << deviceSerialNumber << "-";
                    }
                    filename << imageCount << ".jpg";

                    convertedImage->Save(filename.str().c_str());
                    std::cout << "Image saved at " << filename.str() << std::endl;
                }

                pResultImage->Release();

                std::cout << std::endl;
            } catch (Spinnaker::Exception &e) {
                std::cout << "Error: " << e.what() << std::endl;
                result = -1;
            }
        }

        pCam->EndAcquisition();
    } catch (Spinnaker::Exception &e) {
        std::cout << "Error: " << e.what() << std::endl;
        result = -1;
    }

    return result;
}

int ConfigureCustomImageSettings (INodeMap &nodeMap) {
    int result = 0;

    std::cout << "*** CONFIGURING CUSTOM IMAGE SETTINGS ***" << std::endl;

    try {
        // Pixel format setting
        CEnumerationPtr pPixelFormat = nodeMap.GetNode("PixelFormat");
        if (IsAvailable (pPixelFormat) && IsWritable (pPixelFormat)) {
            CEnumEntryPtr pPixelFormatType = pPixelFormat->GetEntryByName("RGB8");
            if (IsAvailable (pPixelFormatType) && IsReadable (pPixelFormatType)) {
                int64_t pixelFormatType = pPixelFormatType->GetValue();
                pPixelFormat->SetIntValue(pixelFormatType);

                std::cout << "Pixel format set to " << pPixelFormat->GetCurrentEntry()->GetSymbolic() << "..." << std::endl;
            } else {
                throw std::runtime_error("Failed to set pixel format!");
            }
        } else {
            throw std::runtime_error("Pixel format not available!");
        }

        // White balance
        CEnumerationPtr pBalanceWhiteAuto = nodeMap.GetNode("BalanceWhiteAuto");
        if (!IsAvailable(pBalanceWhiteAuto) || !IsWritable(pBalanceWhiteAuto)) {
            std::cout << "Unable to disable automatic white balance (node retrieval). Aborting..." << std::endl;
            return -1;
        }

        CEnumEntryPtr pBalanceWhiteOff = pBalanceWhiteAuto->GetEntryByName("Off");
        if (!IsAvailable(pBalanceWhiteOff) || !IsReadable(pBalanceWhiteOff)) {
            std::cout << "Unable to disable automatic white balance (node retrieval). Aborting..." << std::endl;
            return -1;
        }

        pBalanceWhiteAuto->SetIntValue(pBalanceWhiteOff->GetValue());

        std::cout << "Automatic white balance disabled..." << std::endl;

        // Exposure settings
        CEnumerationPtr pExposureAuto = nodeMap.GetNode("ExposureAuto");
        if (!IsAvailable(pExposureAuto) || !IsWritable(pExposureAuto)) {
            std::cout << "Unable to disable automatic exposure (node retrieval). Aborting..." << std::endl;
            return -1;
        }

        CEnumEntryPtr pExposureAutoOff = pExposureAuto->GetEntryByName("Off");
        if (!IsAvailable(pExposureAutoOff) || !IsReadable(pExposureAutoOff)) {
            std::cout << "Unable to disable automatic exposure (node retrieval). Aborting..." << std::endl;
            return -1;
        }

        pExposureAuto->SetIntValue(pExposureAutoOff->GetValue());
        std::cout << "Automatic exposure disabled..." << std::endl;

        CFloatPtr pExposureTime = nodeMap.GetNode("ExposureTime");
        if (!IsAvailable(pExposureTime) || !IsWritable(pExposureTime)) {
            std::cout << "Unable to set exposure time. Aborting..." << std::endl;
            return -1;
        }

        const double exposureTimeMax = pExposureTime->GetMax();
        double exposureTimeToSet = 5000000.0;  // 2.0 seconds

        if (exposureTimeToSet > exposureTimeMax) {
            exposureTimeToSet = exposureTimeMax;
        }

        pExposureTime->SetValue(exposureTimeToSet);

        std::cout << "Exposure time set to " << exposureTimeToSet << " us..." << std::endl;

        // Image offset and size settings
        CIntegerPtr pOffsetY = nodeMap.GetNode("OffsetY");
        if (IsAvailable(pOffsetY) && IsWritable(pOffsetY)) {
            pOffsetY->SetValue(pOffsetY->GetMin());
            std::cout << "Offset Y set to " << pOffsetY->GetMin() << "..." << std::endl;
        } else {
            throw std::runtime_error("Offset Y not available!");
        }

        CIntegerPtr pOffsetX = nodeMap.GetNode("OffsetX");
        if (IsAvailable(pOffsetX) && IsWritable(pOffsetX)) {
            pOffsetX->SetValue(pOffsetX->GetMin());
            std::cout << "Offset X set to " << pOffsetX->GetMin() << "..." << std::endl;
        } else {
            throw std::runtime_error("Offset X not available!");
        }

        CIntegerPtr pWidth = nodeMap.GetNode("Width");
        if (IsAvailable(pWidth) && IsWritable(pWidth)) {
            int64_t widthToSet = pWidth->GetMax();
            pWidth->SetValue(widthToSet);
            std::cout << "Width set to " << pWidth->GetValue() << "..." << std::endl;
        } else {
            throw std::runtime_error("Width not available!");
        }

        CIntegerPtr pHeight = nodeMap.GetNode("Height");
        if (IsAvailable(pHeight) && IsWritable(pHeight)) {
            int64_t heightToSet = pHeight->GetMax();
            pHeight->SetValue(heightToSet);
            std::cout << "Height set to " << pHeight->GetValue() << std::endl;
        } else {
            throw std::runtime_error("Height not available!");
        }

    } catch (Spinnaker::Exception &e) {
        std::cout << "Error: " << e.what() << std::endl;
        return -1;
    }

    return result;
}

int ResetExposure(INodeMap &nodeMap) {
    int result = 0;

    try {
        CEnumerationPtr pExposureAuto = nodeMap.GetNode("ExposureAuto");
        if (!IsAvailable(pExposureAuto) || !IsWritable(pExposureAuto)) {
            std::cout << "Unable to enable automatic exposure (node retrieval). Non-fatal error..." << std::endl;
            return -1;
        }

        CEnumEntryPtr pExposureAutoContinuous = pExposureAuto->GetEntryByName("Continuous");
        if (!IsAvailable(pExposureAutoContinuous) || !IsReadable(pExposureAutoContinuous)) {
            std::cout << "Unable to enable automatic exposure (node retrieval). Non-fatal error..." << std::endl;
            return -1;
        }

        pExposureAuto->SetIntValue(pExposureAutoContinuous->GetValue());

        std::cout << "Automatic exposure enabled..." << std::endl;
    } catch (Spinnaker::Exception &e) {
        std::cout << "Error: " << e.what() << std::endl;
        result = -1;
    }

    return result;
}

}  // anonymous namespace

// -----------------------------------------------------------------------------
// Public methods
// -----------------------------------------------------------------------------

SpinnakerCamera::SpinnakerCamera() {
    initialize();
}

SpinnakerCamera::~SpinnakerCamera() {
    release();
}

void SpinnakerCamera::release() {
    pCam = NULL;
    camList.Clear();
    system->ReleaseInstance();
}

std::string SpinnakerCamera::info() const {
    const LibraryVersion version = system->GetLibraryVersion();
    std::stringstream ss;

    // Spinnaker library version
    ss << "Spinnaker library version: "
       << version.major << "."
       << version.minor << "."
       << version.type << "."
       << version.build << std::endl;

    // Camera info
    ss << "#camera: " << camList.GetSize() << std::endl;

    return ss.str();
}

void SpinnakerCamera::shot() const {
    int result = 0;
    int error = 0;

    try {
        INodeMap &nodeMapTLDevice = pCam->GetTLDeviceNodeMap();

        result = PrintDeviceInfo(nodeMapTLDevice);

        pCam->Init();

        INodeMap &nodeMap = pCam->GetNodeMap();

        // Configure trigger
        error = ConfigureTrigger(nodeMap);
        if (error < 0) {
            throw std::runtime_error("Trigger configuration failed!");
        }

        // Configure custom image settings
        error = ConfigureCustomImageSettings(nodeMap);
        if (error < 0) {
            throw std::runtime_error("Custom image configuration failed!");
        }

        // Acquire images
        result = result | AcquireImages(pCam, nodeMap, nodeMapTLDevice);

        // Reset exposure
        result = result | ResetExposure(nodeMap);

        // Reset trigger
        result = result | ResetTrigger(nodeMap);

        pCam->DeInit();
    } catch (Spinnaker::Exception &e) {
        std::cout << "Error: " << e.what() << std::endl;
        throw e;
    }
}

// -----------------------------------------------------------------------------
// Private methods
// -----------------------------------------------------------------------------

void SpinnakerCamera::initialize() {
    // Retrieve singleton reference to system object
    system = System::GetInstance();

    // Retrive camera list
    camList = system->GetCameras();

    if (camList.GetSize() == 0) {
        throw std::runtime_error("No camera detected!");
    }

    // Select first camera
    pCam = camList.GetByIndex(0);
}