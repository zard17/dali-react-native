#include "DaliRenderer.h"
#include "DaliMountingManager.h"
#include "DeviceInstanceManager.h"
#include <dali/dali.h>

DaliRenderer::DaliRenderer() {
  std::cout << "DaliRenderer Constructed" << std::endl;
}

DaliRenderer::~DaliRenderer() {
  std::cout << "DaliRenderer Destructed" << std::endl;
}

bool DaliRenderer::OnEventLoopTick() {
  // Tick the RuntimeScheduler to process pending tasks
  mDeviceInstanceManager->TickEventLoop();
  return true; // Keep timer running
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

  // Start a timer to tick the event loop and keep the app running
  mEventLoopTimer = Dali::Timer::New(16); // ~60fps
  mEventLoopTimer.TickSignal().Connect(this, &DaliRenderer::OnEventLoopTick);
  mEventLoopTimer.Start();

  std::cout << "Event loop timer started - window will remain open"
            << std::endl;
}
