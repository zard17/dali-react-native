#include "DaliImageComponent.h"
#include <iostream>

Dali::Toolkit::ImageView DaliImageComponent::New() {
  // Default image or placeholder
  auto imageView = Dali::Toolkit::ImageView::New();
  imageView.SetProperty(Dali::Actor::Property::ANCHOR_POINT,
                        Dali::AnchorPoint::TOP_LEFT);
  imageView.SetProperty(Dali::Actor::Property::PARENT_ORIGIN,
                        Dali::ParentOrigin::TOP_LEFT);
  return imageView;
}

void DaliImageComponent::ApplyProps(Dali::Toolkit::ImageView imageView,
                                    const std::string &props) {
  std::cout << "Applying props to Image: " << props << std::endl;
  // Mock prop application
  // imageView.SetImage("path/to/image");
  // In real implementation we'd parse props["source"]
}
