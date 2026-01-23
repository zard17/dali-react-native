#pragma once

#include <dali-toolkit/dali-toolkit.h>
#include <string>

class DaliTextComponent {
public:
  static Dali::Toolkit::TextLabel New();
  static void ApplyProps(Dali::Toolkit::TextLabel textLabel,
                         const std::string &props);
};
