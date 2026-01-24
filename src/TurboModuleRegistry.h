#pragma once

#include <jsi/jsi.h>
#include <string>

namespace facebook {
namespace jsi {
class Runtime;
}
} // namespace facebook

class TurboModuleRegistry {
public:
  // Install TurboModuleRegistry as a global JSI object
  static void install(facebook::jsi::Runtime &runtime);

private:
  // Get a module by name
  static facebook::jsi::Value getModule(facebook::jsi::Runtime &runtime,
                                        const std::string &moduleName);
};
