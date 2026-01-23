#include "DeviceInstanceManager.h"
#include "DaliMountingManager.h"
#include <iostream>
#include <jsc/JSCRuntime.h>
#include <jsi/jsi.h>
#include <react/renderer/core/Scheduler.h>
#include <react/renderer/mounting/ShadowViewMutation.h>

using namespace facebook::react;

DeviceInstanceManager::DeviceInstanceManager() {
  std::cout << "DeviceInstanceManager Created" << std::endl;
}

DeviceInstanceManager::~DeviceInstanceManager() {
  std::cout << "DeviceInstanceManager Destroyed" << std::endl;
}

void DeviceInstanceManager::Initialize() {
  std::cout << "DeviceInstanceManager Initializing Fabric Scheduler..."
            << std::endl;
  mScheduler = std::make_shared<facebook::react::Scheduler>();
}

void DeviceInstanceManager::StartSurface() {
  std::cout << "Starting React Native Surface..." << std::endl;
}

void DeviceInstanceManager::StopSurface() {
  std::cout << "Stopping React Native Surface..." << std::endl;
}

void DeviceInstanceManager::SimulateJSExecution(
    DaliMountingManager *mountingManager) {
  std::cout << "Simulating JS Execution of index.js..." << std::endl;

  // Simulate standard Fabric mutations that would result from the JS:
  // <View> -> <Text> + <Image>

  // Simulate Simple Color Test
  // 1. Root View (Cyan) - Full Screen (Simulated by large size or window size
  // logic in MountingManager)
  mountingManager->ProcessMockMutation(3, "View", "{backgroundColor:\"cyan\"}");

  // 2. Child View (Magenta) - 200x200
  // Note: CreateActor hardcodes size/pos currently. We might need to update
  // that too if we want perfect match.
  mountingManager->ProcessMockMutation(10, "View",
                                       "{backgroundColor:\"magenta\"}");

  // 3. Text Label (for confirmation)
  mountingManager->ProcessMockMutation(11, "Text",
                                       "{text:\"Color Test Running\"}");

  std::cout << "Simulation Complete." << std::endl;
}
