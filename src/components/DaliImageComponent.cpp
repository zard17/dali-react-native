#include "DaliImageComponent.h"
#include <iostream>

Dali::Toolkit::ImageView DaliImageComponent::New() {
  // Default image or placeholder
  auto imageView = Dali::Toolkit::ImageView::New();
  // Use CENTER/CENTER to avoid TOP_LEFT rendering bug
  imageView.SetProperty(Dali::Actor::Property::ANCHOR_POINT,
                        Dali::AnchorPoint::CENTER);
  imageView.SetProperty(Dali::Actor::Property::PARENT_ORIGIN,
                        Dali::ParentOrigin::CENTER);
  return imageView;
}

// Helper to trim quotes from string values
std::string stripQuotes(std::string val) {
  if (val.length() >= 2 && val.front() == '"' && val.back() == '"') {
    return val.substr(1, val.length() - 2);
  }
  return val;
}

void DaliImageComponent::ApplyProps(Dali::Toolkit::ImageView imageView,
                                    const std::string &props) {
  std::cout << "Applying props to Image: " << props << std::endl;

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
    return -1.0f;
  };

  float x = parseValue("x:");
  float y = parseValue("y:");
  float w = parseValue("width:");
  float h = parseValue("height:");

  if (x != -1.0f && y != -1.0f) {
    imageView.SetProperty(Dali::Actor::Property::POSITION,
                          Dali::Vector3(x, y, 0));
  }
  if (w != -1.0f && h != -1.0f) {
    imageView.SetProperty(Dali::Actor::Property::SIZE, Dali::Vector2(w, h));
  }

  // Parse Source URL
  // Supports url:"..." or src:"..."
  std::string srcKey = "url:";
  size_t srcPos = props.find(srcKey);
  if (srcPos == std::string::npos) {
    srcKey = "src:";
    srcPos = props.find(srcKey);
  }

  if (srcPos != std::string::npos) {
    size_t valStart = srcPos + srcKey.length();
    size_t valEnd = props.find_first_of(",}", valStart);
    if (valEnd != std::string::npos) {
      std::string url = props.substr(valStart, valEnd - valStart);
      url = stripQuotes(url);
      imageView.SetImage(url);
      std::cout << "  -> Set Image URL: " << url << std::endl;
    }
  }
}
