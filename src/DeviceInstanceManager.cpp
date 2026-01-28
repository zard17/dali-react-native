#include "DeviceInstanceManager.h"
#include "DaliMountingManager.h"
#include "DaliTextLayoutManager.h"
#include "TurboModuleRegistry.h"
#include <fstream>
#include <iostream>
#include <jsc/JSCRuntime.h>
#include <jsi/jsi.h>
#include <react/renderer/componentregistry/ComponentDescriptorProviderRegistry.h>
#include <react/renderer/runtimescheduler/RuntimeScheduler.h>
#include <react/renderer/scheduler/SchedulerToolbox.h>
#include <react/renderer/uimanager/UIManagerBinding.h>

using namespace facebook::react;

DeviceInstanceManager::DeviceInstanceManager() {
  std::cout << "DeviceInstanceManager Created" << std::endl;
}

DeviceInstanceManager::~DeviceInstanceManager() {
  std::cout << "DeviceInstanceManager Destroying..." << std::endl;

  // Prevent transaction processing during shutdown
  mShuttingDown = true;
  mMountingManager = nullptr;

  // Stop and unregister surface before destroying scheduler
  StopSurface();

  // Clear surface handler
  mSurfaceHandler.reset();

  // Reset scheduler (this will cleanup internal state)
  if (mScheduler) {
    mScheduler.reset();
  }

  // Reset other components
  mRuntimeScheduler.reset();
  mContextContainer.reset();
  mRuntime.reset();

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
  // Validate if Fabric uses "TextLayoutManager" string key or typed key
  // Standard pattern: mContextContainer->insert("TextLayoutManager", ...);
  mContextContainer->insert("TextLayoutManager",
                            std::shared_ptr<facebook::react::TextLayoutManager>(
                                new DaliTextLayoutManager(mContextContainer)));
  std::cout << "  -> Context Container Created (with TextLayoutManager)"
            << std::endl;

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
  mRuntimeScheduler = std::make_shared<RuntimeScheduler>(runtimeExecutor);
  mContextContainer->insert("RuntimeScheduler",
                            std::weak_ptr<RuntimeScheduler>(mRuntimeScheduler));
  std::cout << "  -> RuntimeScheduler Created and Registered" << std::endl;

  // 5.5. Event Beat Factory
  // The Scheduler needs an EventBeat factory to create event processing beats
  toolbox.eventBeatFactory =
      [this](std::shared_ptr<EventBeat::OwnerBox> ownerBox)
      -> std::unique_ptr<EventBeat> {
    // Create an EventBeat with the RuntimeScheduler reference
    return std::make_unique<EventBeat>(ownerBox, *mRuntimeScheduler);
  };

  // 6. Scheduler
  // Note: Animation delegate can be nullptr for now
  mScheduler = std::make_shared<Scheduler>(toolbox, nullptr, this);
  std::cout << "  -> Scheduler Created" << std::endl;

  // 6.5. Install Fabric UIManager Binding (JSI)
  // This creates 'nativeFabricUIManager' in global
  UIManagerBinding::createAndInstallIfNeeded(*mRuntime,
                                             mScheduler->getUIManager());
  std::cout << "  -> UIManagerBinding Installed" << std::endl;

  // 7. Surface Handler
  // Create surface handler for "DaliRNApp" with surfaceId 1 (must match
  // rootTag)
  mSurfaceHandler.emplace("DaliRNApp", 1);
  mSurfaceHandler->setContextContainer(mContextContainer);

  // Set layout constraints (required before starting surface)
  // Note: minimumSize must equal maximumSize for root to have correct dimensions
  // Otherwise Yoga may calculate root width/height as 0 for flex:1 layouts
  LayoutConstraints layoutConstraints;
  layoutConstraints.layoutDirection = LayoutDirection::LeftToRight;
  // Use actual window size (set dynamically via SetWindowSize)
  layoutConstraints.minimumSize = Size{static_cast<float>(mWindowWidth), static_cast<float>(mWindowHeight)};
  layoutConstraints.maximumSize = Size{static_cast<float>(mWindowWidth), static_cast<float>(mWindowHeight)};
  std::cout << "  -> Layout constraints: " << mWindowWidth << "x" << mWindowHeight << std::endl;

  LayoutContext layoutContext;
  layoutContext.pointScaleFactor = 1.0f;
  layoutContext.fontSizeMultiplier = 1.0f;
  layoutContext.viewportOffset = {0, 0};

  mSurfaceHandler->constraintLayout(layoutConstraints, layoutContext);

  // Register surface with Scheduler
  mScheduler->registerSurface(*mSurfaceHandler);

  std::cout << "  -> SurfaceHandler Registered" << std::endl;
}

void DeviceInstanceManager::StartSurface() {
  std::cout << "Starting React Native Surface..." << std::endl;

  // Start Workaround Thread - REMOVED for Thread Safety
  // DALi APIs must be called on Main Thread. We now rely on DaliRenderer's
  // Timer.
  /*
  if (!mWorkaroundRunning) {
    mWorkaroundRunning = true;
    mWorkaroundThread = std::thread([this]() {
      std::cout << "Workaround Thread Started" << std::endl;
      while (mWorkaroundRunning) {
        this->TickEventLoop();
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
      }
      std::cout << "Workaround Thread Stopped" << std::endl;
    });
  }
  */

  if (mSurfaceHandler) {
    if (mSurfaceHandler->getStatus() == SurfaceHandler::Status::Registered) {
      mSurfaceHandler->start();
      // const auto &mountingCoordinator =
      // mSurfaceHandler->getMountingCoordinator(); Ideally we trigger an
      // initial render or wait for transaction
      std::cout << "  -> Surface Started" << std::endl;
    }
  }
}

void DeviceInstanceManager::StopSurface() {
  std::cout << "Stopping React Native Surface..." << std::endl;

  /*
  if (mWorkaroundRunning) {
    mWorkaroundRunning = false;
    if (mWorkaroundThread.joinable()) {
      mWorkaroundThread.join();
    }
  }
  */

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

void DeviceInstanceManager::SetWindowSize(int width, int height) {
  mWindowWidth = width;
  mWindowHeight = height;
  std::cout << "  -> Window size set to: " << width << "x" << height << std::endl;
}

// Scheduler Delegate Implementation
void DeviceInstanceManager::schedulerDidFinishTransaction(
    const std::shared_ptr<const facebook::react::MountingCoordinator>
        &mountingCoordinator) {
  // Skip transaction processing during shutdown
  if (mShuttingDown) {
    return;
  }

  if (mMountingManager) {
    // Correct API usage based on earlier inspection
    auto transaction = mountingCoordinator->pullTransaction();
    if (transaction.has_value()) {
      auto &mutations = transaction.value().getMutations();
      std::cout << "Scheduler finished transaction! Mutations: "
                << mutations.size() << std::endl;

      mMountingManager->PerformTransaction(transaction.value());
    } else {
      std::cout << "Scheduler finished transaction but no transaction pulled"
                << std::endl;
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

void DeviceInstanceManager::LoadJSBundle(const std::string &bundlePath) {
  std::cout << "Loading JavaScript bundle: " << bundlePath << std::endl;

  // Setup global object (required for JSC)
  mRuntime->global().setProperty(*mRuntime, "global", mRuntime->global());
  std::cout << "  -> Global object set" << std::endl;

  // Polyfill console
  auto console = facebook::jsi::Object(*mRuntime);
  auto log = facebook::jsi::Function::createFromHostFunction(
      *mRuntime, facebook::jsi::PropNameID::forAscii(*mRuntime, "log"), 1,
      [](facebook::jsi::Runtime &rt, const facebook::jsi::Value &thisValue,
         const facebook::jsi::Value *args,
         size_t count) -> facebook::jsi::Value {
        std::cout << "JS LOG: ";
        for (size_t i = 0; i < count; ++i) {
          if (args[i].isString()) {
            std::cout << args[i].asString(rt).utf8(rt) << " ";
          } else if (args[i].isNumber()) {
            std::cout << args[i].getNumber() << " ";
          } else if (args[i].isObject()) {
            // Try JSON.stringify
            try {
              auto global = rt.global();
              auto json = global.getPropertyAsObject(rt, "JSON");
              auto stringify = json.getPropertyAsFunction(rt, "stringify");
              auto result = stringify.call(rt, args[i]);
              if (result.isString()) {
                std::cout << result.asString(rt).utf8(rt) << " ";
              } else {
                std::cout << "[Object] ";
              }
            } catch (...) {
              std::cout << "[Object (Circular/Error)] ";
            }
          } else {
            std::cout << "[Value] ";
          }
        }
        std::cout << std::endl;
        return facebook::jsi::Value::undefined();
      });
  console.setProperty(*mRuntime, "log", std::move(log));

  auto error = facebook::jsi::Function::createFromHostFunction(
      *mRuntime, facebook::jsi::PropNameID::forAscii(*mRuntime, "error"), 1,
      [](facebook::jsi::Runtime &rt, const facebook::jsi::Value &thisValue,
         const facebook::jsi::Value *args,
         size_t count) -> facebook::jsi::Value {
        std::cerr << "JS ERROR: ";
        for (size_t i = 0; i < count; ++i) {
          if (args[i].isString()) {
            std::cerr << args[i].asString(rt).utf8(rt) << " ";
          } else if (args[i].isNumber()) {
            std::cerr << args[i].getNumber() << " ";
          } else {
            std::cerr << "[Object] ";
          }
        }
        std::cerr << std::endl;
        return facebook::jsi::Value::undefined();
      });
  console.setProperty(*mRuntime, "error", std::move(error));
  console.setProperty(
      *mRuntime, "warn",
      console.getProperty(*mRuntime, "log")); // Alias warn to log

  mRuntime->global().setProperty(*mRuntime, "console", std::move(console));
  std::cout << "  -> Console polyfilled" << std::endl;

  // 5. Install TurboModuleRegistry
  // This mocks the native module infrastructure
  TurboModuleRegistry::install(*mRuntime, mRuntimeScheduler);
  std::cout << "  -> TurboModuleRegistry installed via JSI" << std::endl;

  // 6. Schedulerevents the __fbBatchedBridgeConfig error
  const char *nativeModuleConfig =
      "global.__fbBatchedBridgeConfig = { remoteModuleConfig: [], "
      "localModulesConfig: [] };";
  auto configBuffer =
      std::make_shared<facebook::jsi::StringBuffer>(nativeModuleConfig);
  mRuntime->evaluateJavaScript(configBuffer, "nativeModuleConfig.js");
  std::cout << "  -> Native module config set" << std::endl;

  // Read bundle file
  std::ifstream bundleFile(bundlePath);
  if (!bundleFile.is_open()) {
    std::cerr << "ERROR: Failed to open bundle file: " << bundlePath
              << std::endl;
    return;
  }

  std::string bundleContent((std::istreambuf_iterator<char>(bundleFile)),
                            std::istreambuf_iterator<char>());
  bundleFile.close();

  std::cout << "  -> Bundle size: " << bundleContent.size() << " bytes"
            << std::endl;

  // Evaluate JavaScript in JSI runtime
  try {
    auto buffer = std::make_shared<facebook::jsi::StringBuffer>(bundleContent);
    mRuntime->evaluateJavaScript(buffer, bundlePath);
    std::cout << "  -> Bundle executed successfully" << std::endl;
  } catch (const facebook::jsi::JSIException &e) {
    std::cerr << "ERROR: Failed to execute bundle: " << e.what() << std::endl;
  } catch (const std::exception &e) {
    std::cerr << "ERROR: Exception during bundle execution: " << e.what()
              << std::endl;
  }
}

void DeviceInstanceManager::StartReactApp(const std::string &appName,
                                          int rootTag) {
  std::cout << "Starting React app: " << appName << " (rootTag: " << rootTag
            << ")" << std::endl;

  try {
    // Execute: AppRegistry.runApplication('appName', { rootTag: rootTag })
    // AppRegistry is now exposed globally in index.js
    std::string jsCode =
        "const startApp = () => {"
        "  try {"
        "    console.log('JS: startApp called');"
        "    const AppRegistry = global.AppRegistry || global.RN$AppRegistry;"
        "    if (!AppRegistry) throw new Error('AppRegistry not found on "
        "global (tried AppRegistry and RN$AppRegistry)');"
        "    console.log('JS: AppRegistry loaded');"
        "    console.log('JS: Registered Apps:', AppRegistry.getAppKeys());"
        "    console.log('JS: nativeFabricUIManager exists:', "
        "!!global.nativeFabricUIManager);"
        "    setTimeout(() => console.log('JS: TEST TIMEOUT 0ms EXECUTED'), "
        "0); "
        "    AppRegistry.runApplication('" +
        appName +
        "', {"
        "    initialProps: {}, "
        "    rootTag: " +
        std::to_string(rootTag) +
        ", "
        "    fabric: true, "
        "    concurrentRoot: true "
        "  }); "
        "  console.log('JS: runApplication finished');"
        "} catch (e) { "
        "  console.error('Failed to run app:', e); "
        "  if (global.TurboModuleRegistry && "
        "global.TurboModuleRegistry.getEnforcing('ExceptionsManager')) { "
        "    "
        "global.TurboModuleRegistry.getEnforcing('ExceptionsManager')."
        "reportFatalException(e.error ? e.error.message : e.message, [], 0); "
        "  } "
        "} "
        "}; startApp();";
    auto buffer = std::make_shared<facebook::jsi::StringBuffer>(jsCode);
    mRuntime->evaluateJavaScript(buffer, "startApp.js");
    std::cout << "  -> React app started" << std::endl;

    // Phase 2: Execute any pending tasks in RuntimeScheduler
    // This should trigger React's render cycle
    std::cout << "  -> Calling RuntimeScheduler.callExpiredTasks..."
              << std::endl;
    if (mRuntimeScheduler) {
      mRuntimeScheduler->callExpiredTasks(*mRuntime);
      std::cout << "  -> RuntimeScheduler tasks executed" << std::endl;
    }
  } catch (const facebook::jsi::JSIException &e) {
    std::cerr << "ERROR: Failed to start React app: " << e.what() << std::endl;
  } catch (const std::exception &e) {
    std::cerr << "ERROR: Exception during app start: " << e.what() << std::endl;
  }
}

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

// Event Loop Tick - called by DALi Timer (or workaround thread)
bool DeviceInstanceManager::TickEventLoop() {
  static int tickCount = 0;
  if (tickCount++ % 600 == 0) {
    // std::cout << "TickEventLoop Pumped (Idle): " << tickCount << std::endl;
  }
  try {
    if (mRuntimeScheduler &&
        mRuntime) { // Keep mRuntime check as it was originally
      mRuntimeScheduler->callExpiredTasks(*mRuntime);
    }
  } catch (const facebook::jsi::JSIException &e) {
    std::cerr << "ERROR in TickEventLoop: " << e.what() << std::endl;
  }
  return true;
}

void DeviceInstanceManager::RenderAppDemo(
    DaliMountingManager *mountingManager) {
  // Keep existing mock implementation just in case
  // ... (omitted to save space, user can restore if they want simulation
  // alongside)
}
