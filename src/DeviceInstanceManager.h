#pragma once

#include <dali/dali.h>
#include <memory>
#include <react/renderer/core/Scheduler.h>

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
  std::shared_ptr<facebook::react::Scheduler> mScheduler;
};
