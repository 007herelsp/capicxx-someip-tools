#include <dlfcn.h>

#include <chrono>
#include <iostream>
#include <thread>

#include <CommonAPI/CommonAPI.hpp>

#include "methodSampleStubImpl.hpp"

int main(int argc, char **argv) {
    std::shared_ptr<CommonAPI::Runtime> runtime = CommonAPI::Runtime::get();
    std::shared_ptr<methodSampleStubImpl> myService = std::make_shared<methodSampleStubImpl>();
    if (runtime->registerService("local", "BMW.ATM", myService, ""))
    {
        std::cout << "Service registered." << std::endl;
    }
    else
    {
        std::cout << "Service not registered." << std::endl;
    }

    while (true)
    {
        std::cout << "Waiting for calls... (Abort with CTRL+C)" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(60));
    }

    return 0;
}
