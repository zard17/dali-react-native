#include "TurboModuleRegistry.h"
#include "UIManagerModule.h"
#include <iostream>

using namespace facebook;

void TurboModuleRegistry::install(jsi::Runtime &runtime) {
  std::cout << "  -> Installing __turboModuleProxy..." << std::endl;

  // Implement __turboModuleProxy(moduleName)
  // This is what React Native looks for in TurboModuleRegistry.js
  auto turboModuleProxy = jsi::Function::createFromHostFunction(
      runtime, jsi::PropNameID::forAscii(runtime, "__turboModuleProxy"), 1,
      [](jsi::Runtime &rt, const jsi::Value &thisValue, const jsi::Value *args,
         size_t count) -> jsi::Value {
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
                [](jsi::Runtime &rt, const jsi::Value &, const jsi::Value *args,
                   size_t) -> jsi::Value {
                  double id = args[0].getNumber();
                  double duration = args[1].getNumber();
                  std::cout << "  -> Timing.createTimer(id=" << id
                            << ", duration=" << duration << ")" << std::endl;
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
            module.setProperty(rt, "linkRootNode", std::move(linkRootNode));

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
