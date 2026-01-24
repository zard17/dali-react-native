#include "UIManagerModule.h"
#include <iostream>

using namespace facebook;

jsi::Object UIManagerModule::createModule(jsi::Runtime &runtime) {
  auto module = jsi::Object(runtime);

  // getViewManagerConfig(viewManagerName)
  auto getViewManagerConfig = jsi::Function::createFromHostFunction(
      runtime, jsi::PropNameID::forAscii(runtime, "getViewManagerConfig"), 1,
      [](jsi::Runtime &rt, const jsi::Value &thisValue, const jsi::Value *args,
         size_t count) -> jsi::Value {
        // Return empty config for now
        return jsi::Object(rt);
      });
  module.setProperty(runtime, "getViewManagerConfig",
                     std::move(getViewManagerConfig));

  // getConstants()
  auto getConstants = jsi::Function::createFromHostFunction(
      runtime, jsi::PropNameID::forAscii(runtime, "getConstants"), 0,
      [](jsi::Runtime &rt, const jsi::Value &thisValue, const jsi::Value *args,
         size_t count) -> jsi::Value {
        // Return empty constants for now
        return jsi::Object(rt);
      });
  module.setProperty(runtime, "getConstants", std::move(getConstants));

  // measure(node, callback) - stub
  auto measure = jsi::Function::createFromHostFunction(
      runtime, jsi::PropNameID::forAscii(runtime, "measure"), 2,
      [](jsi::Runtime &rt, const jsi::Value &thisValue, const jsi::Value *args,
         size_t count) -> jsi::Value {
        // Stub - would call callback with measurements
        return jsi::Value::undefined();
      });
  module.setProperty(runtime, "measure", std::move(measure));

  // measureInWindow(node, callback) - stub
  auto measureInWindow = jsi::Function::createFromHostFunction(
      runtime, jsi::PropNameID::forAscii(runtime, "measureInWindow"), 2,
      [](jsi::Runtime &rt, const jsi::Value &thisValue, const jsi::Value *args,
         size_t count) -> jsi::Value {
        // Stub - would call callback with measurements
        return jsi::Value::undefined();
      });
  module.setProperty(runtime, "measureInWindow", std::move(measureInWindow));

  std::cout << "  -> UIManager module created" << std::endl;
  return module;
}
