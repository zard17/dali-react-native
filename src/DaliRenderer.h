#pragma once

#include <dali/dali.h>
#include <iostream>
#include <memory>

class DaliRenderer {
public:
  DaliRenderer();
  ~DaliRenderer();

  void Init(Dali::Application &application);

private:
  std::unique_ptr<class DeviceInstanceManager> mDeviceInstanceManager;
  std::unique_ptr<class DaliMountingManager> mMountingManager;
};
