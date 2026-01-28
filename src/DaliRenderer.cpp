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

  // Get window first to determine actual size
  Dali::Window window = application.GetWindow();
  Dali::Window::WindowSize winSize = window.GetSize();
  int windowWidth = winSize.GetWidth();
  int windowHeight = winSize.GetHeight();
  std::cout << "  -> Actual window size: " << windowWidth << "x" << windowHeight << std::endl;

  // Initialize DeviceInstanceManager with correct window size
  mDeviceInstanceManager = std::make_unique<DeviceInstanceManager>();
  mDeviceInstanceManager->SetWindowSize(windowWidth, windowHeight);
  mDeviceInstanceManager->Initialize();

  mMountingManager = std::make_unique<DaliMountingManager>();
  mDeviceInstanceManager->SetMountingManager(mMountingManager.get());
  mMountingManager->SetWindow(window);

  // Disable depth test for proper 2D rendering order (painter's algorithm)
  // This ensures actors render in add-order, not by depth
  window.GetRootLayer().SetProperty(Dali::Layer::Property::DEPTH_TEST, false);

  // WORKAROUND: DALi Mac/ANGLE bug - Y positions < ~200 don't render
  // Shift root layer up to compensate for Y offset in DaliMountingManager
  static const float DALI_MAC_Y_OFFSET = 200.0f;
  window.GetRootLayer().SetProperty(Dali::Actor::Property::POSITION,
                                    Dali::Vector3(0.0f, -DALI_MAC_Y_OFFSET, 0.0f));
  std::cout << "  -> Mac Y offset workaround: +" << DALI_MAC_Y_OFFSET << " (root layer -" << DALI_MAC_Y_OFFSET << ")" << std::endl;

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
