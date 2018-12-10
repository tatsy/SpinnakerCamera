#include "SpinnakerCamera.h"

#include <iostream>
#include <sstream>

using namespace Spinnaker;

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
}