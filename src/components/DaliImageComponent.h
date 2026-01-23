#pragma once

#include <dali-toolkit/dali-toolkit.h>
#include <string>

class DaliImageComponent {
public:
  static Dali::Toolkit::ImageView New();
  static void ApplyProps(Dali::Toolkit::ImageView imageView,
                         const std::string &props);
};
