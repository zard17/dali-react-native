#pragma once

#include <dali-toolkit/dali-toolkit.h>
#include <string>

class DaliViewComponent {
public:
  static Dali::Toolkit::Control New();
  static void ApplyProps(Dali::Toolkit::Control control,
                         const std::string &props);
};
