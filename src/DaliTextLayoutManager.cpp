#include "DaliTextLayoutManager.h"
#include <dali-toolkit/dali-toolkit.h>
#include <iostream>

namespace facebook {
namespace react {

DaliTextLayoutManager::DaliTextLayoutManager(
    std::shared_ptr<const ContextContainer> const &contextContainer)
    : TextLayoutManager(contextContainer) {
  std::cout << "DaliTextLayoutManager Created" << std::endl;
}

TextMeasurement DaliTextLayoutManager::measure(
    const AttributedStringBox &attributedStringBox,
    const ParagraphAttributes &paragraphAttributes,
    const TextLayoutContext &layoutContext,
    const LayoutConstraints &layoutConstraints) const {

  // Debug log to confirm usage
  // std::cout << "DaliTextLayoutManager::measure called" << std::endl;

  // 1. Construct text content from fragments
  std::string textContent = "";
  auto &attributedString = attributedStringBox.getValue();
  auto &fragments = attributedString.getFragments();

  if (fragments.empty()) {
    return TextMeasurement{{0, 0}, {}};
  }

  for (const auto &fragment : fragments) {
    textContent += fragment.string;
  }

  // 2. Create off-screen TextLabel for measurement
  // Note: Creating a new Actor for every measurement might be expensive.
  // Ideally use a cached or shared instance.
  auto textLabel = Dali::Toolkit::TextLabel::New();
  textLabel.SetProperty(Dali::Toolkit::TextLabel::Property::TEXT, textContent);

  // 3. Apply styles from first fragment (MVP)
  auto &firstFragment = fragments[0];
  auto &textAttributes = firstFragment.textAttributes;

  if (!textAttributes.fontFamily.empty()) {
    textLabel.SetProperty(Dali::Toolkit::TextLabel::Property::FONT_FAMILY,
                          textAttributes.fontFamily);
  }

  if (!std::isnan(textAttributes.fontSize)) {
    textLabel.SetProperty(Dali::Toolkit::TextLabel::Property::POINT_SIZE,
                          textAttributes.fontSize);
  }

  // 4. Set constraints
  textLabel.SetProperty(Dali::Toolkit::TextLabel::Property::MULTI_LINE, true);

  float maxWidth = layoutConstraints.maximumSize.width;
  auto naturalSize = textLabel.GetNaturalSize();

  float measuredWidth = naturalSize.width;
  float measuredHeight = naturalSize.height;

  // Manual wrap simulation (Fallback)
  if (std::isfinite(maxWidth) && measuredWidth > maxWidth) {
    measuredWidth = maxWidth;
    float lines = std::ceil(naturalSize.width / maxWidth);
    measuredHeight = naturalSize.height * lines;
  }

  std::cout << "  -> Measured: " << measuredWidth << " x " << measuredHeight
            << " (Text: " << textContent.substr(0, 10) << "...)" << std::endl;

  return TextMeasurement{.size = {measuredWidth, measuredHeight},
                         .attachments = {}};
}

} // namespace react
} // namespace facebook
