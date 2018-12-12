#include <iostream>

#include "SpinnakerCamera.h"

int main(int argc, char **argv) {
    SpinnakerCamera *camera = NULL;
    try {
        camera = new SpinnakerCamera();
        std::cout << camera->info() << std::endl;

        camera->setPixelFormat(PixelFormat::RGB, 8);
        camera->setGamma(1.0);

        camera->setExposureTime(1.0);
        camera->trigger();
        camera->setExposureTime(2.0);
        camera->trigger();
        camera->setExposureTime(4.0);
        camera->trigger();

    } catch (std::runtime_error &e) {
        std::cerr << e.what() << std::endl;
        std::exit(1);
    }

    delete camera;
}