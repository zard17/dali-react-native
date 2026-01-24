#include "DaliRenderer.h"
#include "DaliMountingManager.h"
#include "DeviceInstanceManager.h"

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

  // Run the simulation
  mDeviceInstanceManager->SimulateJSExecution(mMountingManager.get());
}
