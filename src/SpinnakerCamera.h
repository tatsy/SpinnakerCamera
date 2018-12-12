#ifdef _MSC_VER
#pragma once
#endif

#ifndef SPINNAKER_CAMERA_H
#define SPINNAKER_CAMERA_H

#include <string>
#include <unordered_map>

#include <Spinnaker.h>

enum class PixelFormat : int {
    Mono,
    RGB,
    BayerRG,
    BayerGB
};

class SpinnakerCamera {
public:
    SpinnakerCamera();
    virtual ~SpinnakerCamera();

    void setExposureTime(double seconds);
    void setAutoWhiteBalance(bool enable);
    void setAutoGain(bool enable);
    void setPixelFormat(PixelFormat format, int bitDepth);
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

    PixelFormat m_pixelFormat = PixelFormat::Mono;
    int m_bitDepth = 8;
    double m_exposureTime = -1.0;
    double m_gamma = 1.0;
    bool m_autoWhiteBlance = false;
    bool m_autoGain = false;

    static std::unordered_map<PixelFormat, std::string> fmt2str;
};

#endif  // SPINNAKER_CAMERA_H
