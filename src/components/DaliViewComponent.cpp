#include "DaliViewComponent.h"
#include <iostream>

Dali::Toolkit::Control DaliViewComponent::New() {
  auto control = Dali::Toolkit::Control::New();
  // Use CENTER/CENTER to avoid TOP_LEFT rendering bug
  control.SetProperty(Dali::Actor::Property::PARENT_ORIGIN,
                      Dali::ParentOrigin::CENTER);
  control.SetProperty(Dali::Actor::Property::ANCHOR_POINT,
                      Dali::AnchorPoint::CENTER);
  // Default background to transparent or set via props
  return control;
}

void DaliViewComponent::ApplyProps(Dali::Toolkit::Control control,
                                   const std::string &props) {
  // Parse props string (mock) and apply
  // In real Fabric, props is Folly::dynamic or specific struct
  // Parse Layout Props (x, y, width, height)
  // Format: ", x:100, y:200, width:50, height:50"

  auto parseValue = [&](std::string key) -> float {
    size_t pos = props.find(key);
    if (pos != std::string::npos) {
      size_t valStart = pos + key.length();
      size_t valEnd = props.find_first_of(",}", valStart);
      if (valEnd != std::string::npos) {
        try {
          return std::stof(props.substr(valStart, valEnd - valStart));
        } catch (...) {
          return 0.0f;
        }
      }
    }
    return -1.0f; // Sentinel
  };

  float x = parseValue("x:");
  float y = parseValue("y:");
  float w = parseValue("width:");
  float h = parseValue("height:");

  if (x != -1.0f && y != -1.0f) {
    control.SetProperty(Dali::Actor::Property::POSITION,
                        Dali::Vector3(x, y, 0));
  }
  if (w != -1.0f && h != -1.0f) {
    control.SetProperty(Dali::Actor::Property::SIZE, Dali::Vector2(w, h));
  }

  // Parse Colors
  if (props.find("backgroundColor:\"cyan\"") != std::string::npos) {
    control.SetBackgroundColor(Dali::Color::CYAN);
  } else if (props.find("backgroundColor:\"magenta\"") != std::string::npos) {
    control.SetBackgroundColor(Dali::Color::MAGENTA);
  } else if (props.find("backgroundColor:\"blue\"") != std::string::npos) {
    control.SetBackgroundColor(Dali::Color::BLUE);
  } else if (props.find("backgroundColor:\"red\"") != std::string::npos) {
    control.SetBackgroundColor(Dali::Color::RED);
  } else if (props.find("backgroundColor:\"green\"") != std::string::npos) {
    control.SetBackgroundColor(Dali::Color::GREEN);
  } else if (props.find("backgroundColor:\"yellow\"") != std::string::npos) {
    control.SetBackgroundColor(Dali::Color::YELLOW);
  } else if (props.find("backgroundColor:\"white\"") != std::string::npos) {
    control.SetBackgroundColor(Dali::Color::WHITE);
  } else if (props.find("backgroundColor:\"black\"") != std::string::npos) {
    control.SetBackgroundColor(Dali::Color::BLACK);
  } else if (props.find("backgroundColor:\"gray\"") != std::string::npos) {
    control.SetBackgroundColor(Dali::Vector4(0.5f, 0.5f, 0.5f, 1.0f));
  } else if (props.find("backgroundColor:\"orange\"") != std::string::npos) {
    control.SetBackgroundColor(Dali::Vector4(1.0f, 0.5f, 0.0f, 1.0f));
  } else if (props.find("backgroundColor:\"purple\"") != std::string::npos) {
    control.SetBackgroundColor(Dali::Vector4(0.5f, 0.0f, 0.5f, 1.0f));
  } else {
    // Default
    control.SetBackgroundColor(Dali::Color::TRANSPARENT);
  }
}
