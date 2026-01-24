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

  // Register Event Callback to bridge back to "JS"
  mountingManager->SetEventCallback([](int tag, std::string eventName) {
    std::cout << "[[JS BRIDGE]] Event Received: " << eventName
              << " on Tag: " << tag << std::endl;

    // Simulate JS Logic responding to event
    if (eventName == "touchEnd") {
      std::cout << "  -> JS: Handling onPress for Tag " << tag << std::endl;
      if (tag >= 300 && tag <= 305) {
        std::cout << "  -> JS: Navigate to Screen for Button " << tag
                  << std::endl;
      }
    }
  });

  // Call the App Demo Render logic
  RenderAppDemo(mountingManager);

  std::cout << "Simulation Complete." << std::endl;
}

void DeviceInstanceManager::RenderAppDemo(
    DaliMountingManager *mountingManager) {
  // 1. Root View - White background
  mountingManager->ProcessMockMutation(3, -1, "View",
                                       "{backgroundColor:\"white\"}");

  // 2. Header Title
  mountingManager->ProcessMockMutation(
      10, 3, "Text",
      "{text:\"My DALi App\", x:50, y:50, width:500, height:100}");

  // 3. Carousel (Horizontal Scroll)
  // Label
  mountingManager->ProcessMockMutation(
      11, 3, "Text",
      "{text:\"Recent Items\", x:50, y:150, width:400, height:50}");

  // Banner Image (New Image Component Test)
  mountingManager->ProcessMockMutation(
      50, 3, "Image",
      "{url:\"https://picsum.photos/400/100\", x:550, y:50, width:400, "
      "height:100}");

  // Items
  float carouselY = 220;
  float carouselX = 50;
  float itemSize = 150;
  float gap = 20;
  const std::string colors[] = {"red", "green", "blue", "cyan", "magenta"};

  for (int i = 0; i < 5; ++i) {
    float x = carouselX + i * (itemSize + gap);
    // Fix potential float to string locale issues or just simple concatenation
    std::string props = "{backgroundColor:\"" + colors[i] + "\"" +
                        ", x:" + std::to_string(x) +
                        ", y:" + std::to_string(carouselY) +
                        ", width:" + std::to_string(itemSize) +
                        ", height:" + std::to_string(itemSize) + "}";
    mountingManager->ProcessMockMutation(100 + i, 3, "View", props);

    // Add a label inside the card
    std::string labelProps = "{text:\"Item " + std::to_string(i + 1) +
                             "\", x:10, y:10, width:100, height:50}";
    mountingManager->ProcessMockMutation(200 + i, 100 + i, "Text", labelProps);
  }

  // 4. List (Vertical)
  // Label
  float listStartY = 450;
  mountingManager->ProcessMockMutation(
      12, 3, "Text", "{text:\"Settings\", x:50, y:450, width:400, height:50}");

  float listY = listStartY + 70;
  for (int i = 0; i < 3; ++i) {
    float y = listY + i * (80 + 10);

    // Button Background
    std::string btnProps =
        "{backgroundColor:\"gray\", x:50, y:" + std::to_string(y) +
        ", width:400, height:80}";
    int btnTag = 300 + i;
    mountingManager->ProcessMockMutation(btnTag, 3, "View", btnProps);

    // Button Text
    std::string btnText =
        (i == 0 ? "Profile" : (i == 1 ? "Notifications" : "Log Out"));
    std::string textProps =
        "{text:\"" + btnText + "\", x:20, y:20, width:300, height:40}";
    mountingManager->ProcessMockMutation(400 + i, btnTag, "Text", textProps);
  }
}
