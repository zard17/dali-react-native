#include "DaliRenderer.h"
#include "DaliMountingManager.h"
#include "DeviceInstanceManager.h"
#include <dali/dali.h>
#include <dali/devel-api/adaptor-framework/application-devel.h>

DaliRenderer::DaliRenderer() {
  std::cout << "DaliRenderer Constructed" << std::endl;
}

DaliRenderer::~DaliRenderer() {
  std::cout << "DaliRenderer Destructed" << std::endl;
}

void DaliRenderer::Init(Dali::Application &application) {
  std::cout << "DaliRenderer Init called" << std::endl;

  mDeviceInstanceManager = std::make_unique<DeviceInstanceManager>();
  mDeviceInstanceManager->Initialize();

  mMountingManager = std::make_unique<DaliMountingManager>();
  mDeviceInstanceManager->SetMountingManager(mMountingManager.get());

  Dali::Window window = application.GetWindow();
  mMountingManager->SetWindow(window);

  // Disable depth test for proper 2D rendering order (painter's algorithm)
  // This ensures actors render in add-order, not by depth
  window.GetRootLayer().SetProperty(Dali::Layer::Property::DEPTH_TEST, false);

  // Disable frustum culling on the default render task
  Dali::RenderTaskList taskList = window.GetRenderTaskList();
  Dali::RenderTask defaultTask = taskList.GetTask(0);
  defaultTask.SetCullMode(false);
  std::cout << "  -> Frustum culling disabled on default render task" << std::endl;

  // Create a custom offset layer using CENTER/CENTER
  // This layer will contain all actors and use its center as the origin
  Dali::Layer offsetLayer = Dali::Layer::New();
  offsetLayer.SetProperty(Dali::Actor::Property::PARENT_ORIGIN, Dali::ParentOrigin::CENTER);
  offsetLayer.SetProperty(Dali::Actor::Property::ANCHOR_POINT, Dali::AnchorPoint::CENTER);
  offsetLayer.SetProperty(Dali::Actor::Property::SIZE, Dali::Vector2(1920.0f, 1080.0f));
  offsetLayer.SetProperty(Dali::Actor::Property::POSITION, Dali::Vector3(0.0f, 0.0f, 0.0f));
  window.Add(offsetLayer);
  std::cout << "  -> Custom offset layer created with CENTER/CENTER" << std::endl;

  // Load JavaScript bundle and start React app
  mDeviceInstanceManager->LoadJSBundle("bundle.js");
  mDeviceInstanceManager->StartSurface();
  mDeviceInstanceManager->StartReactApp("DaliRNApp", 1);

  // Phase 3: Start event loop timer for continuous task processing
  // Deprecated:
  // mDeviceInstanceManager->SimulateJSExecution(mMountingManager.get());

  // Setup Idle Callback for Continuous Event Loop Pumping
  Dali::DevelApplication::AddIdleWithReturnValue(
      application, MakeCallback(this, &DaliRenderer::OnIdle));

  std::cout << "Event loop setup complete (Driven by Idle Callback)"
            << std::endl;
}

bool DaliRenderer::OnIdle() {
  if (mDeviceInstanceManager) {
    mDeviceInstanceManager->TickEventLoop();
  }
  return true;
}
