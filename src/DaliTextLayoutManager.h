#pragma once

#include <react/renderer/textlayoutmanager/TextLayoutManager.h>
#include <react/utils/ContextContainer.h>

namespace facebook {
namespace react {

class DaliTextLayoutManager : public TextLayoutManager {
public:
  DaliTextLayoutManager(
      std::shared_ptr<const ContextContainer> const &contextContainer);
  ~DaliTextLayoutManager() override = default;

  TextMeasurement
  measure(const AttributedStringBox &attributedStringBox,
          const ParagraphAttributes &paragraphAttributes,
          const TextLayoutContext &layoutContext,
          const LayoutConstraints &layoutConstraints) const override;
};

} // namespace react
} // namespace facebook
