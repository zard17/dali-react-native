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

  mMountingManager->SetWindow(application.GetWindow());

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
