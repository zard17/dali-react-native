#pragma once

#include <jsi/jsi.h>

namespace facebook {
namespace jsi {
class Runtime;
}
} // namespace facebook

class UIManagerModule {
public:
  // Create UIManager TurboModule as a JSI object
  static facebook::jsi::Object createModule(facebook::jsi::Runtime &runtime);
};
