#include "TurboModuleRegistry.h"
#include "UIManagerModule.h"
#include <iostream>

using namespace facebook;

void TurboModuleRegistry::install(jsi::Runtime &runtime) {
  // Create TurboModuleRegistry object
  auto registry = jsi::Object(runtime);

  // Implement getEnforcing(moduleName)
  auto getEnforcing = jsi::Function::createFromHostFunction(
      runtime, jsi::PropNameID::forAscii(runtime, "getEnforcing"), 1,
      [](jsi::Runtime &rt, const jsi::Value &thisValue, const jsi::Value *args,
         size_t count) -> jsi::Value {
        if (count > 0 && args[0].isString()) {
          std::string moduleName = args[0].asString(rt).utf8(rt);
          std::cout << "  -> TurboModule requested: " << moduleName
                    << std::endl;

          if (moduleName == "UIManager") {
            return UIManagerModule::createModule(rt);
          }

          // For unknown modules, return empty object to prevent crashes
          std::cout << "  -> Unknown module, returning empty object"
                    << std::endl;
          return jsi::Object(rt);
        }
        return jsi::Object(rt);
      });
  registry.setProperty(runtime, "getEnforcing", std::move(getEnforcing));

  // Implement get(moduleName) - same as getEnforcing for now
  auto get = jsi::Function::createFromHostFunction(
      runtime, jsi::PropNameID::forAscii(runtime, "get"), 1,
      [](jsi::Runtime &rt, const jsi::Value &thisValue, const jsi::Value *args,
         size_t count) -> jsi::Value {
        if (count > 0 && args[0].isString()) {
          std::string moduleName = args[0].asString(rt).utf8(rt);
          std::cout << "  -> TurboModule requested (get): " << moduleName
                    << std::endl;

          if (moduleName == "UIManager") {
            return UIManagerModule::createModule(rt);
          }

          return jsi::Value::undefined();
        }
        return jsi::Value::undefined();
      });
  registry.setProperty(runtime, "get", std::move(get));

  // Install as global.TurboModuleRegistry
  runtime.global().setProperty(runtime, "TurboModuleRegistry",
                               std::move(registry));

  std::cout << "  -> TurboModuleRegistry installed via JSI" << std::endl;
}

jsi::Value TurboModuleRegistry::getModule(jsi::Runtime &runtime,
                                          const std::string &moduleName) {
  // This method is not used anymore, kept for API compatibility
  return jsi::Value::undefined();
}
