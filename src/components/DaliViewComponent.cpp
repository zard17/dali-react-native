#include "DaliViewComponent.h"
#include <iostream>

Dali::Toolkit::Control DaliViewComponent::New() {
  auto control = Dali::Toolkit::Control::New();
  control.SetProperty(Dali::Actor::Property::PARENT_ORIGIN,
                      Dali::ParentOrigin::TOP_LEFT);
  control.SetProperty(Dali::Actor::Property::ANCHOR_POINT,
                      Dali::AnchorPoint::TOP_LEFT);
  // Default background to transparent or set via props
  return control;
}

void DaliViewComponent::ApplyProps(Dali::Toolkit::Control control,
                                   const std::string &props) {
  // Parse props string (mock) and apply
  // In real Fabric, props is Folly::dynamic or specific struct
  std::cout << "Applying props to View: " << props << std::endl;

  if (props.find("backgroundColor:\"cyan\"") != std::string::npos) {
    control.SetBackgroundColor(Dali::Color::CYAN);
  } else if (props.find("backgroundColor:\"magenta\"") != std::string::npos) {
    control.SetBackgroundColor(Dali::Color::MAGENTA);
  } else if (props.find("backgroundColor:\"blue\"") != std::string::npos) {
    control.SetBackgroundColor(Dali::Color::BLUE);
  } else if (props.find("backgroundColor:\"red\"") != std::string::npos) {
    control.SetBackgroundColor(Dali::Color::RED);
  } else {
    // Default or transparent
    control.SetBackgroundColor(
        Dali::Color::WHITE); // Default to white for visibility if unspecified
  }
}
