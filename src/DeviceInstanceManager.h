#pragma once

#include <dali/dali.h>
#include <memory>

class DeviceInstanceManager {
public:
  DeviceInstanceManager();
  ~DeviceInstanceManager();

  void Initialize();
  void StartSurface();
  void StopSurface();

  // Simulates the JS bridge sending mutations
  void SimulateJSExecution(class DaliMountingManager *mountingManager);

private:
};
