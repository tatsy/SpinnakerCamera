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

    void setExposureTime(double seconds);
    void setAutoWhiteBalance(bool enable);
    void setAutoGain(bool enable);
    void setGamma(double gamma);

    void release();
    std::string info() const;
    void trigger();

private:
    void initialize();
    void configure();
    void reset();
    void acquireImage();

    Spinnaker::SystemPtr system = NULL;
    Spinnaker::CameraList camList;
    Spinnaker::CameraPtr pCam = NULL;

    double m_exposureTime = -1.0;
    double m_gamma = 1.0;
    bool m_autoWhiteBlance = false;
    bool m_autoGain = false;
};

#endif  // SPINNAKER_CAMERA_H
