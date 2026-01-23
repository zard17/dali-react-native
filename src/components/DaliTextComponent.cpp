#include "DaliTextComponent.h"
#include <iostream>

Dali::Toolkit::TextLabel DaliTextComponent::New() {
  auto textLabel = Dali::Toolkit::TextLabel::New("Hello World");
  textLabel.SetProperty(Dali::Actor::Property::ANCHOR_POINT,
                        Dali::AnchorPoint::TOP_LEFT);
  textLabel.SetProperty(Dali::Actor::Property::PARENT_ORIGIN,
                        Dali::ParentOrigin::TOP_LEFT);
  return textLabel;
}

void DaliTextComponent::ApplyProps(Dali::Toolkit::TextLabel textLabel,
                                   const std::string &props) {
  std::cout << "Applying props to Text: " << props << std::endl;

  std::string textToShow = "Mapped Text Component";
  size_t textPos = props.find("text:");
  if (textPos != std::string::npos) {
    textToShow = props.substr(textPos + 5);
    // Trim quotes if any (simplified parser)
    if (textToShow.size() > 0 && textToShow.back() == '}')
      textToShow.pop_back();
    if (textToShow.size() > 0 && textToShow.back() == '"')
      textToShow.pop_back();
    if (textToShow.size() > 0 && textToShow.front() == '"')
      textToShow.erase(0, 1);
  }

  // Only update if we found something meaningful, or leave default
  if (textPos != std::string::npos) {
    textLabel.SetProperty(Dali::Toolkit::TextLabel::Property::TEXT, textToShow);
  }
  textLabel.SetProperty(Dali::Toolkit::TextLabel::Property::TEXT_COLOR,
                        Dali::Color::BLACK);
}
