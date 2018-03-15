/* Copyright (C) 2015 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <thread>
#include <iostream>

#include <CommonAPI/CommonAPI.hpp>
#include "MergedInterfaceStubImplB.hpp"


#include <CommonAPI/CommonAPI.hpp>
#include <CommonAPI/Extensions/AttributeCacheExtension.hpp>
#include <v2/commonapi/examples/C/MergedInterfaceCProxy.hpp>

using namespace v2::commonapi::examples::C;

void recv_cb(const CommonAPI::CallStatus& callStatus, const int32_t& val) {
    std::cout << "Receive callback: " << val << std::endl;
}

int client_c() {
    //CommonAPI::Runtime::setProperty("LogContext", "E02CC");
   // CommonAPI::Runtime::setProperty("LogApplication", "E02CC");
   // CommonAPI::Runtime::setProperty("LibraryBase", "MergedInterface-C");

    std::shared_ptr < CommonAPI::Runtime > runtime = CommonAPI::Runtime::get();

    std::string domain = "local";
    std::string instance = "commonapi.examples.C.MergedInterfaceC";
    std::string connection = "client-sample";

    auto myProxy = runtime->buildProxyWithDefaultAttributeExtension<MergedInterfaceCProxy, CommonAPI::Extensions::AttributeCacheExtension>(domain, instance, connection);

    std::cout << "Waiting for service to become available." << std::endl;
    while (!myProxy->isAvailable()) {
        std::this_thread::sleep_for(std::chrono::microseconds(10));
    }

    CommonAPI::CallStatus callStatus;

    int32_t value = 0;

    CommonAPI::CallInfo info(1000);
    info.sender_ = 5678;

    // Get actual attribute value from service
    std::cout << "Proxy C getting attribute X3 value: " << value << std::endl;
    myProxy->getX3Attribute().getValue(callStatus, value, &info);
    if (callStatus != CommonAPI::CallStatus::SUCCESS) {
        std::cerr << "Remote call A failed!\n";
        return -1;
    }
    std::cout << "Proxy C got attribute X3 value: " << value << std::endl;

    // Subscribe for receiving values
    myProxy->getX3Attribute().getChangedEvent().subscribe([&](const int32_t& val) {
        std::cout << "Received attribute X3 change message: " << val << std::endl;
    });

    value = 100;

    // Asynchronous call to set attribute of service
    std::function<void(const CommonAPI::CallStatus&, int32_t)> fcb = recv_cb;
    myProxy->getX3Attribute().setValueAsync(value, fcb, &info);

    while (true) {
        int32_t errorValue = -1;
        int32_t valueCached = *myProxy->getX3AttributeExtension().getCachedValue(errorValue);
        if (valueCached != errorValue) {
            std::cout << "<Proxy C got cached attribute X3 value[" << (int)valueCached << "]: " << valueCached << std::endl;
        } else {
            std::cout << "Proxy C got cached attribute X3 error value[" << (int)valueCached << "]: " << valueCached << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::microseconds(1000000));
    }
}



int server_b() {
//    CommonAPI::Runtime::setProperty("LogContext", "E02SB");
//    CommonAPI::Runtime::setProperty("LogApplication", "E02SB");
//    CommonAPI::Runtime::setProperty("LibraryBase", "MergedInterface-B");

    std::shared_ptr<CommonAPI::Runtime> runtime = CommonAPI::Runtime::get();

    std::string domain = "local";
    std::string instance = "commonapi.examples.B.MergedInterfaceB";
    std::string connection = "service-sample";

    std::shared_ptr<MergedInterfaceStubImplB> myService = std::make_shared<MergedInterfaceStubImplB>();

	std::cout << "bbbbbbbbbbbbbbbb " <<  std::endl;

	while (!runtime->registerService(domain, instance, myService, connection)) {
        std::cout << "Register Service B failed, trying again in 100 milliseconds..." << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    std::cout << "Successfully Registered Service!" << std::endl;

    while (true) {
        myService->incCounter(); // Change value of attribute, see stub implementation
        std::cout << "Waiting for calls... (Abort with CTRL+C)" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
    return 0;
}


//这里是测试同时使用客户端和服务端的可能性，测试结果验证能够同时存在
int main() {
	CommonAPI::Runtime::setProperty("LogContext", "E02SB");
	   CommonAPI::Runtime::setProperty("LogApplication", "E02SB");
	 //  CommonAPI::Runtime::setProperty("LibraryBase", "MergedInterface-B");

  std::cout << "000000000000000000 " <<	std::endl;

  std::thread  c = std::thread(client_c);

   std::this_thread::sleep_for(std::chrono::microseconds(10000));
  std::thread	b = std::thread(server_b);


  c.join();
  b.join();

  return 0;
}

