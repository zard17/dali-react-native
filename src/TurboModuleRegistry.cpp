#include "TurboModuleRegistry.h"
#include "UIManagerModule.h"
#include <iostream>

using namespace facebook;

#include <react/renderer/runtimescheduler/RuntimeScheduler.h>

void TurboModuleRegistry::install(
    jsi::Runtime &runtime,
    std::shared_ptr<facebook::react::RuntimeScheduler> runtimeScheduler) {
  std::cout << "  -> Installing __turboModuleProxy..." << std::endl;

  // Implement __turboModuleProxy(moduleName)
  // This is what React Native looks for in TurboModuleRegistry.js
  auto turboModuleProxy = jsi::Function::createFromHostFunction(
      runtime, jsi::PropNameID::forAscii(runtime, "__turboModuleProxy"), 1,
      [runtimeScheduler](jsi::Runtime &rt, const jsi::Value &thisValue,
                         const jsi::Value *args, size_t count) -> jsi::Value {
        if (count > 0 && args[0].isString()) {
          std::string moduleName = args[0].asString(rt).utf8(rt);

          if (moduleName == "UIManager") {
            std::cout << "  -> TurboModule requested: " << moduleName
                      << std::endl;
            return UIManagerModule::createModule(rt);
          }

          if (moduleName == "ExceptionsManager") {
            std::cout << "  -> TurboModule requested: " << moduleName
                      << std::endl;
            auto module = jsi::Object(rt);

            auto reportFatalException = jsi::Function::createFromHostFunction(
                rt, jsi::PropNameID::forAscii(rt, "reportFatalException"), 3,
                [](jsi::Runtime &rt, const jsi::Value &thisValue,
                   const jsi::Value *args, size_t count) -> jsi::Value {
                  std::string message = args[0].asString(rt).utf8(rt);
                  std::cout << "🔥 FATAL JS EXCEPTION: " << message
                            << std::endl;
                  return jsi::Value::undefined();
                });
            module.setProperty(rt, "reportFatalException",
                               std::move(reportFatalException));

            auto reportSoftException = jsi::Function::createFromHostFunction(
                rt, jsi::PropNameID::forAscii(rt, "reportSoftException"), 3,
                [](jsi::Runtime &rt, const jsi::Value &thisValue,
                   const jsi::Value *args, size_t count) -> jsi::Value {
                  std::string message = args[0].asString(rt).utf8(rt);
                  std::cout << "⚠️ SOFT JS EXCEPTION: " << message << std::endl;
                  return jsi::Value::undefined();
                });
            module.setProperty(rt, "reportSoftException",
                               std::move(reportSoftException));

            return module;
          }

          if (moduleName == "SourceCode") {
            auto module = jsi::Object(rt);
            auto getConstants = jsi::Function::createFromHostFunction(
                rt, jsi::PropNameID::forAscii(rt, "getConstants"), 0,
                [](jsi::Runtime &rt, const jsi::Value &, const jsi::Value *,
                   size_t) -> jsi::Value {
                  auto constants = jsi::Object(rt);
                  constants.setProperty(
                      rt, "scriptURL",
                      jsi::String::createFromAscii(rt, "bundle.js"));
                  return constants;
                });
            module.setProperty(rt, "getConstants", std::move(getConstants));
            return module;
          }

          if (moduleName == "DeviceInfo") {
            auto module = jsi::Object(rt);
            auto getConstants = jsi::Function::createFromHostFunction(
                rt, jsi::PropNameID::forAscii(rt, "getConstants"), 0,
                [](jsi::Runtime &rt, const jsi::Value &, const jsi::Value *,
                   size_t) -> jsi::Value {
                  auto constants = jsi::Object(rt);
                  auto dims = jsi::Object(rt);

                  auto window = jsi::Object(rt);
                  window.setProperty(rt, "width", 1920);
                  window.setProperty(rt, "height", 1080);
                  window.setProperty(rt, "scale", 1);
                  window.setProperty(rt, "fontScale", 1);

                  dims.setProperty(rt, "windowPhysicalPixels", window);
                  dims.setProperty(rt, "screenPhysicalPixels", window);

                  constants.setProperty(rt, "Dimensions", dims);
                  return constants;
                });
            module.setProperty(rt, "getConstants", std::move(getConstants));
            return module;
          }

          if (moduleName == "Timing") {
            auto module = jsi::Object(rt);
            auto createTimer = jsi::Function::createFromHostFunction(
                rt, jsi::PropNameID::forAscii(rt, "createTimer"), 4,
                [runtimeScheduler](jsi::Runtime &rt, const jsi::Value &,
                                   const jsi::Value *args,
                                   size_t count) -> jsi::Value {
                  double id = args[0].getNumber();
                  double duration = args[1].getNumber();
                  // double jsSchedulingTime = args[2].getNumber();
                  // bool repeats = args[3].getBool();

                  std::cout << "  -> Timing.createTimer(id=" << id
                            << ", duration=" << duration << ")" << std::endl;

                  // Schedule task on RuntimeScheduler
                  if (runtimeScheduler) {
                    runtimeScheduler->scheduleTask(
                        facebook::react::SchedulerPriority::NormalPriority,
                        jsi::Function::createFromHostFunction(
                            rt, jsi::PropNameID::forAscii(rt, "timerCallback"),
                            0,
                            [id](jsi::Runtime &rt, const jsi::Value &,
                                 const jsi::Value *, size_t) -> jsi::Value {
                              try {
                                auto global = rt.global();
                                // Note: JSTimers is usually a module, but
                                // standard JSI might just rely on global or
                                // BatchedBridge. In RN, it is usually
                                // JSTimers.callTimers. But here I'm getting
                                // 'callTimers' from GLOBAL? Wait.

                                // Re-evaluating logic:
                                // "callTimers" is NOT on global usually.
                                // It should be
                                // require('JSTimers').callTimers(ids).

                                // My previous code:
                                // auto jst = global.getPropertyAsObject(rt,
                                // "JSTimers"); (Wait, I didn't see where I got
                                // JSTimers) Ah, I was using 'callTimers'
                                // directly from global in previous snippet?
                                // Let's check original code.
                                // If I tried to get 'callTimers' from global
                                // and it failed...

                                // Let's assume global.JSTimers exists
                                // (polyfill?) or I need to invoke via
                                // BatchedBridge. But let's verify what I HAVE.

                                // For now, logging will tell us.
                                if (global.hasProperty(rt,
                                                       "__fbBatchedBridge")) {
                                  auto bridge = global.getPropertyAsObject(
                                      rt, "__fbBatchedBridge");
                                  auto callFunction =
                                      bridge.getPropertyAsFunction(
                                          rt, "callFunctionReturnFlushedQueue");

                                  auto ids = jsi::Array(rt, 1);
                                  ids.setValueAtIndex(rt, 0, id);

                                  // callFunctionReturnFlushedQueue("JSTimers",
                                  // "callTimers", [ids])
                                  callFunction.call(
                                      rt,
                                      jsi::String::createFromAscii(rt,
                                                                   "JSTimers"),
                                      jsi::String::createFromAscii(
                                          rt, "callTimers"),
                                      ids);
                                  std::cout << "  -> JSTimers.callTimers(" << id
                                            << ") executed via BatchedBridge"
                                            << std::endl;
                                } else {
                                  std::cout << "ERROR: __fbBatchedBridge not "
                                               "found on global"
                                            << std::endl;
                                }
                              } catch (const std::exception &e) {
                                std::cout
                                    << "ERROR in Timer Lambda: " << e.what()
                                    << std::endl;
                              }
                              return jsi::Value::undefined();
                            }));
                  }

                  return jsi::Value::undefined();
                });
            module.setProperty(rt, "createTimer", std::move(createTimer));

            auto deleteTimer = jsi::Function::createFromHostFunction(
                rt, jsi::PropNameID::forAscii(rt, "deleteTimer"), 1,
                [](jsi::Runtime &rt, const jsi::Value &, const jsi::Value *args,
                   size_t) -> jsi::Value {
                  double id = args[0].getNumber();
                  std::cout << "  -> Timing.deleteTimer(id=" << id << ")"
                            << std::endl;
                  // TODO: Implement cancelTask if we store tasks
                  return jsi::Value::undefined();
                });
            module.setProperty(rt, "deleteTimer", std::move(deleteTimer));

            auto setSendIdleEvents = jsi::Function::createFromHostFunction(
                rt, jsi::PropNameID::forAscii(rt, "setSendIdleEvents"), 1,
                [](jsi::Runtime &rt, const jsi::Value &, const jsi::Value *,
                   size_t) -> jsi::Value { return jsi::Value::undefined(); });
            module.setProperty(rt, "setSendIdleEvents",
                               std::move(setSendIdleEvents));

            return module;
          }

          if (moduleName == "NativeDOMCxx") {
            auto module = jsi::Object(rt);
            // linkRootNode seems to be expected on the default export or the
            // module itself The error says "v.default.linkRootNode is not a
            // function" Typically this means the module should return an object
            // that might have 'default'? Or maybe NativeDOMCxx IS the object
            // that has linkRootNode but JS wraps it. Let's implement
            // linkRootNode on the module directly first.

            auto linkRootNode = jsi::Function::createFromHostFunction(
                rt, jsi::PropNameID::forAscii(rt, "linkRootNode"), 2,
                [](jsi::Runtime &rt, const jsi::Value &, const jsi::Value *args,
                   size_t) -> jsi::Value {
                  // args[0] is likely the rootTag (number)
                  // args[1] is likely the shadowNode (object)
                  std::cout << "  -> NativeDOMCxx.linkRootNode called"
                            << std::endl;
                  return jsi::Value::undefined();
                });

            // The error "v.default.linkRootNode is not a function" suggests
            // usage of "export default" So we need to provide { default: {
            // linkRootNode: ... } } OR maybe just { linkRootNode: ... } if it's
            // a named export, but the usage implies default. Let's provide both
            // to be safe.

            module.setProperty(
                rt, "linkRootNode",
                jsi::Value(rt, linkRootNode)); // Named export style ?

            auto defaultExport = jsi::Object(rt);
            defaultExport.setProperty(rt, "linkRootNode",
                                      std::move(linkRootNode));
            module.setProperty(rt, "default", std::move(defaultExport));

            return module;
          }

          if (moduleName == "NativeReactNativeFeatureFlagsCxx") {
            auto module = jsi::Object(rt);
            // Implement common feature flags if needed
            // For now return empty object or mocked functions if requested
            auto common = jsi::Function::createFromHostFunction(
                rt, jsi::PropNameID::forAscii(rt, "common"), 2,
                [](jsi::Runtime &rt, const jsi::Value &, const jsi::Value *args,
                   size_t) -> jsi::Value {
                  return true; // Default to true for feature flags
                });
            module.setProperty(rt, "useModernRuntime", true);
            module.setProperty(rt, "enableMicrotasks", true);
            return module;
          }

          if (moduleName == "NativePerformanceCxx") {
            auto module = jsi::Object(rt);
            auto mark = jsi::Function::createFromHostFunction(
                rt, jsi::PropNameID::forAscii(rt, "mark"), 3,
                [](jsi::Runtime &rt, const jsi::Value &, const jsi::Value *args,
                   size_t) -> jsi::Value {
                  // args[0]: name, args[1]: startTime, args[2]: duration
                  return jsi::Value::undefined();
                });
            module.setProperty(rt, "mark", std::move(mark));

            auto measure = jsi::Function::createFromHostFunction(
                rt, jsi::PropNameID::forAscii(rt, "measure"), 3,
                [](jsi::Runtime &rt, const jsi::Value &, const jsi::Value *args,
                   size_t) -> jsi::Value { return jsi::Value::undefined(); });
            module.setProperty(rt, "measure", std::move(measure));
            return module;
          }

          // For debugging other requests
          std::cout << "  -> TurboModule requested (unknown): " << moduleName
                    << std::endl;
          // Return empty object for unknown modules to prevent crashes
          return jsi::Object(rt);
        }
        return jsi::Object(rt);
      });

  // Install as global.__turboModuleProxy
  runtime.global().setProperty(runtime, "__turboModuleProxy",
                               std::move(turboModuleProxy));

  std::cout << "  -> __turboModuleProxy installed via JSI" << std::endl;
}

jsi::Value TurboModuleRegistry::getModule(jsi::Runtime &runtime,
                                          const std::string &moduleName) {
  // This method is not used anymore, kept for API compatibility
  return jsi::Value::undefined();
}
