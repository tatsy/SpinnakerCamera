#include <iostream>

#include "SpinnakerCamera.h"

int main(int argc, char **argv) {
    SpinnakerCamera *camera = NULL;
    try {
        camera = new SpinnakerCamera();
    } catch (std::runtime_error &e) {
        std::cerr << e.what() << std::endl;
        std::exit(1);
    }

    std::cout << camera->info() << std::endl;

    camera->shot();

    delete camera;
}