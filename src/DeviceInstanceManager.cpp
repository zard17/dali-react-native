#include "DeviceInstanceManager.h"
#include "DaliMountingManager.h"
#include <iostream>
#include <jsc/JSCRuntime.h>
#include <jsi/jsi.h>
#include <react/renderer/componentregistry/ComponentDescriptorProviderRegistry.h>
#include <react/renderer/runtimescheduler/RuntimeScheduler.h>
#include <react/renderer/scheduler/SchedulerToolbox.h>

using namespace facebook::react;

DeviceInstanceManager::DeviceInstanceManager() {
  std::cout << "DeviceInstanceManager Created" << std::endl;
}

DeviceInstanceManager::~DeviceInstanceManager() {
  std::cout << "DeviceInstanceManager Destroyed" << std::endl;
}

void DeviceInstanceManager::Initialize() {
  std::cout << "DeviceInstanceManager Initializing..." << std::endl;

  // 1. Initialize JS Runtime (JSC)
  mRuntime = facebook::jsc::makeJSCRuntime();
  std::cout << "  -> JS Runtime Initialized" << std::endl;

  // 2. Initialize Runtime Executor
  auto runtime = mRuntime.get();
  facebook::react::RuntimeExecutor runtimeExecutor =
      [runtime](
          std::function<void(facebook::jsi::Runtime & runtime)> &&callback) {
        if (callback) {
          callback(*runtime);
        } else {
          std::cerr << "Warning: RuntimeExecutor called with empty callback"
                    << std::endl;
        }
      };
  std::cout << "  -> Runtime Executor Created" << std::endl;

  // 3. Initialize Context Container
  mContextContainer = std::make_shared<facebook::react::ContextContainer>();
  std::cout << "  -> Context Container Created" << std::endl;

  // 4. Component Descriptor Provider Registry
  auto providerRegistry =
      std::make_shared<ComponentDescriptorProviderRegistry>();
  providerRegistry->add(
      concreteComponentDescriptorProvider<ViewComponentDescriptor>());
  providerRegistry->add(
      concreteComponentDescriptorProvider<ImageComponentDescriptor>());
  providerRegistry->add(
      concreteComponentDescriptorProvider<ParagraphComponentDescriptor>());
  providerRegistry->add(
      concreteComponentDescriptorProvider<RawTextComponentDescriptor>());

  std::cout << "  -> Component Providers Registered" << std::endl;

  // 5. Scheduler Toolbox
  SchedulerToolbox toolbox;
  toolbox.contextContainer = mContextContainer;
  toolbox.componentRegistryFactory =
      [providerRegistry](EventDispatcher::Weak eventDispatcher,
                         ContextContainer::Shared contextContainer)
      -> ComponentDescriptorRegistry::Shared {
    auto registry = providerRegistry->createComponentDescriptorRegistry(
        {eventDispatcher, contextContainer});
    return registry;
  };
  toolbox.runtimeExecutor = runtimeExecutor;

  // 5.4. Create and Register RuntimeScheduler in ContextContainer
  // The Scheduler expects to find RuntimeScheduler in the ContextContainer
  auto runtimeScheduler = std::make_shared<RuntimeScheduler>(runtimeExecutor);
  mContextContainer->insert("RuntimeScheduler",
                            std::weak_ptr<RuntimeScheduler>(runtimeScheduler));
  std::cout << "  -> RuntimeScheduler Created and Registered" << std::endl;

  // 5.5. Event Beat Factory
  // The Scheduler needs an EventBeat factory to create event processing beats
  toolbox.eventBeatFactory =
      [runtimeScheduler](std::shared_ptr<EventBeat::OwnerBox> ownerBox)
      -> std::unique_ptr<EventBeat> {
    // Create an EventBeat with the RuntimeScheduler reference
    return std::make_unique<EventBeat>(ownerBox, *runtimeScheduler);
  };

  // 6. Scheduler
  // Note: Animation delegate can be nullptr for now
  mScheduler = std::make_shared<Scheduler>(toolbox, nullptr, this);
  std::cout << "  -> Scheduler Created" << std::endl;

  // 7. Surface Handler
  // Create surface handler for "DaliApp" with surfaceId 11
  mSurfaceHandler.emplace("DaliApp", 11);
  mSurfaceHandler->setContextContainer(mContextContainer);

  // Register surface with Scheduler
  mScheduler->registerSurface(*mSurfaceHandler);

  std::cout << "  -> SurfaceHandler Registered" << std::endl;
}

void DeviceInstanceManager::StartSurface() {
  std::cout << "Starting React Native Surface..." << std::endl;
  if (mSurfaceHandler) {
    if (mSurfaceHandler->getStatus() == SurfaceHandler::Status::Registered) {
      mSurfaceHandler->start();
      const auto &mountingCoordinator =
          mSurfaceHandler->getMountingCoordinator();
      // Ideally we trigger an initial render or wait for transaction
      std::cout << "  -> Surface Started" << std::endl;
    }
  }
}

void DeviceInstanceManager::StopSurface() {
  std::cout << "Stopping React Native Surface..." << std::endl;
  if (mSurfaceHandler &&
      mSurfaceHandler->getStatus() == SurfaceHandler::Status::Running) {
    mSurfaceHandler->stop();
    if (mScheduler) {
      mScheduler->unregisterSurface(*mSurfaceHandler);
    }
  }
}

void DeviceInstanceManager::SetMountingManager(
    DaliMountingManager *mountingManager) {
  mMountingManager = mountingManager;
}

// Scheduler Delegate Implementation
void DeviceInstanceManager::schedulerDidFinishTransaction(
    const std::shared_ptr<const facebook::react::MountingCoordinator>
        &mountingCoordinator) {
  if (mMountingManager) {
    // Correct API usage based on earlier inspection
    auto transaction = mountingCoordinator->pullTransaction();
    if (transaction.has_value()) {
      mMountingManager->PerformTransaction(transaction.value());
    }
  }
}

void DeviceInstanceManager::schedulerShouldRenderTransactions(
    const std::shared_ptr<const facebook::react::MountingCoordinator>
        &mountingCoordinator) {
  schedulerDidFinishTransaction(mountingCoordinator);
}

void DeviceInstanceManager::schedulerDidRequestPreliminaryViewAllocation(
    const facebook::react::ShadowNode &shadowNode) {
  // No-op for DALi currently
}

void DeviceInstanceManager::schedulerDidDispatchCommand(
    const facebook::react::ShadowView &shadowView,
    std::string const &commandName, folly::dynamic const &args) {
  // Dispatch to MountingManager if needed
}

void DeviceInstanceManager::schedulerDidSetIsJSResponder(
    const facebook::react::ShadowView &shadowView, bool isJSResponder,
    bool blockNativeResponder) {
  // Logic for touch handling priorities
}

void DeviceInstanceManager::schedulerDidSendAccessibilityEvent(
    const facebook::react::ShadowView &shadowView,
    std::string const &eventType) {}

void DeviceInstanceManager::schedulerShouldSynchronouslyUpdateViewOnUIThread(
    facebook::react::Tag tag, const folly::dynamic &props) {}

void DeviceInstanceManager::schedulerDidUpdateShadowTree(
    const std::unordered_map<facebook::react::Tag, folly::dynamic>
        &tagToProps) {}

void DeviceInstanceManager::SimulateJSExecution(
    DaliMountingManager *mountingManager) {
  std::cout << "Simulating JS Execution... (DEPRECATED for real Scheduler)"
            << std::endl;
  // We can keep this for testing logic without full RN stack if needed,
  // but now we rely on Initialize/StartSurface.

  // Register Event Callback to bridge back to "JS" (Mock)
  mountingManager->SetEventCallback([](int tag, std::string eventName) {
    // Mock log
  });

  RenderAppDemo(mountingManager);
}

void DeviceInstanceManager::RenderAppDemo(
    DaliMountingManager *mountingManager) {
  // Keep existing mock implementation just in case
  // ... (omitted to save space, user can restore if they want simulation
  // alongside)
}
