#ifdef _MSC_VER
#pragma once
#endif

#ifndef SPINNAKER_CAMERA_H
#define SPINNAKER_CAMERA_H

#include <string>

#include <Spinnaker.h>

class SpinnakerCamera {
public:
    SpinnakerCamera();
    virtual ~SpinnakerCamera();

    void release();
    std::string info() const;
    void shot() const;

private:
    void initialize();

    Spinnaker::SystemPtr system = NULL;
    Spinnaker::CameraList camList;
    Spinnaker::CameraPtr pCam = NULL;
};

#endif  // SPINNAKER_CAMERA_H
