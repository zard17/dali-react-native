#include "DeviceInstanceManager.h"
#include "DaliMountingManager.h"
#include <iostream>
#include <jsc/JSCRuntime.h>
#include <jsi/jsi.h>
#include <react/renderer/mounting/ShadowViewMutation.h>

#include <string>
#include <vector>

using namespace facebook::react;

DeviceInstanceManager::DeviceInstanceManager() {
  std::cout << "DeviceInstanceManager Created" << std::endl;
}

DeviceInstanceManager::~DeviceInstanceManager() {
  std::cout << "DeviceInstanceManager Destroyed" << std::endl;
}

void DeviceInstanceManager::Initialize() {
  std::cout << "DeviceInstanceManager Initializing Fabric Scheduler... "
               "(Skipped for Simulation)"
            << std::endl;
  // mScheduler = std::make_shared<facebook::react::Scheduler>();
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
  // 1. Root View - White background
  mountingManager->ProcessMockMutation(3, "View",
                                       "{backgroundColor:\"white\"}");

  // 2. Many Colored Squares
  const std::string colors[] = {"red",   "green",   "blue",   "yellow",
                                "cyan",  "magenta", "orange", "purple",
                                "black", "gray"};

  for (int i = 0; i < 50; ++i) {
    std::string color = colors[i % 10];
    std::string props = "{backgroundColor:\"" + color + "\"}";
    mountingManager->ProcessMockMutation(100 + i, "View", props);
  }

  // 3. Text Label
  mountingManager->ProcessMockMutation(11, "Text",
                                       "{text:\"Grid Layout Demo\"}");

  std::cout << "Simulation Complete." << std::endl;
}
