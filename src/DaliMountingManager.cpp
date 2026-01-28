#include "DaliMountingManager.h"
#include "components/DaliImageComponent.h"
#include "components/DaliTextComponent.h"
#include "components/DaliViewComponent.h"
#include <iostream>
#include <react/renderer/components/image/ImageProps.h>
#include <react/renderer/components/image/ImageState.h>
#include <react/renderer/components/text/ParagraphProps.h>
#include <react/renderer/components/text/ParagraphState.h>
#include <react/renderer/components/view/ViewProps.h>
#include <react/renderer/core/ConcreteState.h>
#include <react/renderer/graphics/Color.h>
using namespace facebook::react;

DaliMountingManager::DaliMountingManager() {
  std::cout << "DaliMountingManager Created" << std::endl;
}

DaliMountingManager::~DaliMountingManager() {
  std::cout << "DaliMountingManager Destroyed" << std::endl;
}

void DaliMountingManager::SetWindow(Dali::Window window) { mWindow = window; }

void DaliMountingManager::PerformTransaction(
    MountingTransaction const &transaction) {
  std::cout << "Processing Transaction... Mutations: "
            << transaction.getMutations().size() << std::endl;

  for (const auto &mut : transaction.getMutations()) {
    ProcessMutation(mut);
  }
}

// Main handling of Fabric mutations
void DaliMountingManager::ProcessMutation(
    facebook::react::ShadowViewMutation const &mutation) {

  // Debug Log
  std::cout << "ProcessMutation: " << (int)mutation.type
            << " Tag: " << mutation.newChildShadowView.tag << std::endl;

  switch (mutation.type) {
  case ShadowViewMutation::Type::Create: {
    auto &view = mutation.newChildShadowView;
    auto tag = view.tag;
    std::string componentName =
        view.componentName ? view.componentName : "View";

    std::cout << "  -> Create: Tag=" << tag << " Component=" << componentName
              << " HasState=" << (view.state ? "yes" : "no") << std::endl;

    // Create Actor
    auto actor = CreateActor(tag, componentName);
    if (actor) {
      mActorRegistry[tag] = actor;
      mEventEmitterRegistry[tag] = view.eventEmitter; // Store EventEmitter
      if (view.eventEmitter) {
        std::cout << "  -> Stored EventEmitter for Tag: " << tag << std::endl;
      } else {
        std::cout << "  -> Warning: Null EventEmitter for Tag: " << tag
                  << std::endl;
      }
      UpdateProps(actor, componentName, view.props);
      UpdateLayout(actor, view.layoutMetrics);

      // Extract text from ParagraphState for text components
      if (componentName == "Paragraph" && view.state) {
        // Cast State::Shared to ConcreteState<ParagraphState>
        auto concreteState =
            std::dynamic_pointer_cast<const ConcreteState<ParagraphState>>(
                view.state);
        if (concreteState) {
          auto &paragraphState = concreteState->getData();
          auto &attributedString = paragraphState.attributedString;
          auto &fragments = attributedString.getFragments();
          std::string fullText = "";
          for (const auto &fragment : fragments) {
            fullText += fragment.string;
          }

          // Apply text to TextLabel
          auto textLabel = Dali::Toolkit::TextLabel::DownCast(actor);
          if (textLabel && !fullText.empty()) {
            textLabel.SetProperty(Dali::Toolkit::TextLabel::Property::TEXT,
                                  fullText);
            textLabel.SetProperty(
                Dali::Toolkit::TextLabel::Property::TEXT_COLOR,
                Dali::Color::BLACK);

            // Always set a visible default point size
            textLabel.SetProperty(
                Dali::Toolkit::TextLabel::Property::POINT_SIZE, 24.0f);

            std::cout << "  -> Text set: \"" << fullText << "\"" << std::endl;

            // Apply text color from first fragment if available
            if (!fragments.empty()) {
              auto &textAttributes = fragments[0].textAttributes;
              if (textAttributes.foregroundColor) {
                auto colorComponents =
                    colorComponentsFromColor(textAttributes.foregroundColor);
                textLabel.SetProperty(
                    Dali::Toolkit::TextLabel::Property::TEXT_COLOR,
                    Dali::Vector4(colorComponents.red, colorComponents.green,
                                  colorComponents.blue, colorComponents.alpha));
              }
              // Apply font size from React Native if available
              if (!std::isnan(textAttributes.fontSize) &&
                  textAttributes.fontSize > 0) {
                textLabel.SetProperty(
                    Dali::Toolkit::TextLabel::Property::POINT_SIZE,
                    textAttributes.fontSize);
                std::cout << "  -> Font size: " << textAttributes.fontSize
                          << std::endl;
              }
            }
          }
        }
      }

      // Extract image source from ImageState for Image components
      if (componentName == "Image" && view.state) {
        auto concreteState =
            std::dynamic_pointer_cast<const ConcreteState<ImageState>>(
                view.state);
        if (concreteState) {
          auto &imageState = concreteState->getData();
          auto imageSource = imageState.getImageSource();
          std::string uri = imageSource.uri;

          if (!uri.empty()) {
            auto imageView = Dali::Toolkit::ImageView::DownCast(actor);
            if (imageView) {
              imageView.SetImage(uri);
              std::cout << "  -> Image URL set: " << uri << std::endl;
            }
          } else {
            std::cout << "  -> Warning: Empty image URI" << std::endl;
          }
        } else {
          // Try extracting from props if state is not available
          auto imageProps =
              std::dynamic_pointer_cast<const ImageProps>(view.props);
          if (imageProps && !imageProps->sources.empty()) {
            std::string uri = imageProps->sources[0].uri;
            if (!uri.empty()) {
              auto imageView = Dali::Toolkit::ImageView::DownCast(actor);
              if (imageView) {
                imageView.SetImage(uri);
                std::cout << "  -> Image URL from props: " << uri << std::endl;
              }
            }
          }
        }
      }
      // If this is the root view (tag 1), add it to the window
      if (tag == 1 && mWindow) {
        mWindow.Add(actor);
        std::cout << "  -> Root actor (tag 1) added to window" << std::endl;
      }
    }
    break;
  }
  case ShadowViewMutation::Type::Delete: {
    auto tag = mutation.oldChildShadowView.tag;
    // Delete actor logic (Remove from parent and destroy)
    // For now just remove from registry?
    // We need to unparent first?
    // Usually Remove mutation happens before Delete.
    // Delete means "destroy".
    // std::cout << "Delete Actor " << tag << std::endl;
    // Implement DeleteActor(tag);
    auto it = mActorRegistry.find(tag);
    if (it != mActorRegistry.end()) {
      it->second.Unparent(); // Ensure removed from parent
      it->second.Reset();    // Release handle
      mActorRegistry.erase(it);
    }
    break;
  }
  case ShadowViewMutation::Type::Insert: {
    auto parentTag = mutation.parentTag;
    auto childTag = mutation.newChildShadowView.tag;
    auto index = mutation.index;

    std::cout << "  -> Insert: Child=" << childTag
              << " into Parent=" << parentTag
              << " at index=" << index << std::endl;

    auto parentIt = mActorRegistry.find(parentTag);
    auto childIt = mActorRegistry.find(childTag);

    if (parentIt != mActorRegistry.end() && childIt != mActorRegistry.end()) {
      parentIt->second.Add(childIt->second);
      std::cout << "  -> Inserted successfully" << std::endl;
    } else {
      std::cout << "Warning: Parent(" << parentTag << ") or Child(" << childTag
                << ") not found for Insert" << std::endl;

      // Special case: if parent not found but we have the child, add to window
      if (childIt != mActorRegistry.end() && mWindow) {
        mWindow.Add(childIt->second);
        std::cout << "  -> Added orphan child " << childTag << " to window"
                  << std::endl;
      }
    }
    break;
  }
  case ShadowViewMutation::Type::Remove: {
    auto childTag = mutation.oldChildShadowView.tag;
    auto childIt = mActorRegistry.find(childTag);
    if (childIt != mActorRegistry.end()) {
      childIt->second.Unparent();
    }
    break;
  }
  case ShadowViewMutation::Type::Update: {
    auto &view = mutation.newChildShadowView;
    auto tag = view.tag;
    auto it = mActorRegistry.find(tag);

    // If actor doesn't exist yet (e.g., root view), create it
    if (it == mActorRegistry.end()) {
      std::string componentName =
          view.componentName ? view.componentName : "View";
      auto actor = CreateActor(tag, componentName);
      if (actor) {
        mActorRegistry[tag] = actor;
        mEventEmitterRegistry[tag] = view.eventEmitter;

        // If this is the root view (tag 1), add it to the window
        if (tag == 1 && mWindow) {
          mWindow.Add(actor);
          // Set root actor size to window size
          Dali::Window::WindowSize winSize = mWindow.GetSize();
          actor.SetProperty(
              Dali::Actor::Property::SIZE,
              Dali::Vector2(winSize.GetWidth(), winSize.GetHeight()));
          actor.SetProperty(Dali::Actor::Property::ANCHOR_POINT,
                            Dali::AnchorPoint::TOP_LEFT);
          actor.SetProperty(Dali::Actor::Property::PARENT_ORIGIN,
                            Dali::ParentOrigin::TOP_LEFT);
          std::cout
              << "  -> Root actor (tag 1) created and added to window (size: "
              << winSize.GetWidth() << "x" << winSize.GetHeight() << ")"
              << std::endl;
        }
      }
      it = mActorRegistry.find(tag);
    }

    if (it != mActorRegistry.end()) {
      std::string componentName =
          view.componentName ? view.componentName : "View";
      UpdateProps(it->second, componentName, view.props);
      UpdateLayout(it->second, view.layoutMetrics);
      mEventEmitterRegistry[tag] = view.eventEmitter; // Update EventEmitter
      if (view.eventEmitter) {
        std::cout << "  -> Updated EventEmitter for Tag: " << tag << std::endl;
      }
    }
    break;
  }
  }
}

Dali::Actor DaliMountingManager::CreateActor(int tag,
                                             std::string componentName) {
  Dali::Actor actor;
  if (componentName == "View") {
    auto view = DaliViewComponent::New();
    view.SetProperty(Dali::Actor::Property::NAME, std::to_string(tag));
    view.TouchedSignal().Connect(this, &DaliMountingManager::OnTouch);
    actor = view;
  } else if (componentName == "Text" || componentName == "Paragraph") {
    auto text = DaliTextComponent::New();
    text.SetProperty(Dali::Actor::Property::NAME, std::to_string(tag));
    // Default size to avoid 0 size if layout missing
    text.SetProperty(Dali::Actor::Property::SIZE, Dali::Vector2(100, 50));
    actor = text;
  } else if (componentName == "Image") {
    auto image = DaliImageComponent::New();
    image.SetProperty(Dali::Actor::Property::NAME, std::to_string(tag));
    actor = image;
  } else {
    // Default fallback
    actor = Dali::Actor::New();
    actor.SetProperty(Dali::Actor::Property::NAME, std::to_string(tag));
  }

  // Special Handling for Root (Tag 1) to attach to Window immediately?
  // No, Insert mutation will handle attaching Root to... wait.
  // Fabric Root usually has no parent in mutations, but we mount it to a
  // Surface. SurfaceHandler/MountingCoordinator handles the root. If Tag 1 is
  // created, do we add it to mWindow? `ProcessMutation` doesn't handle "Root
  // Mount". `DaliMountingManager` needs `MountSurface` method or similar? For
  // now, if Tag matches RootTag (passed in StartReactApp -> 1), we add to
  // Window. But DaliMountingManager doesn't know RootTag? We can infer: Create
  // mutation for Tag 1 -> Add to Window.
  if (tag == 1 && mWindow) {
    mWindow.Add(actor);
    Dali::Window::WindowSize size = mWindow.GetSize();
    actor.SetProperty(Dali::Actor::Property::SIZE,
                      Dali::Vector2(size.GetWidth(), size.GetHeight()));
  }

  return actor;
}

void DaliMountingManager::UpdateProps(
    Dali::Actor actor, std::string componentName,
    facebook::react::Props::Shared const &props) {

  if (!props) {
    return;
  }

  // Handle View components
  if (componentName == "View") {
    auto viewProps = std::dynamic_pointer_cast<const ViewProps>(props);
    if (viewProps) {
      // Extract backgroundColor from ViewProps
      auto bgColor = viewProps->backgroundColor;
      if (bgColor) {
        auto colorComponents = colorComponentsFromColor(bgColor);
        auto control = Dali::Toolkit::Control::DownCast(actor);
        if (control) {
          control.SetBackgroundColor(
              Dali::Vector4(colorComponents.red, colorComponents.green,
                            colorComponents.blue, colorComponents.alpha));
          std::cout << "  -> View backgroundColor set: rgba("
                    << colorComponents.red << ", " << colorComponents.green
                    << ", " << colorComponents.blue << ", "
                    << colorComponents.alpha << ")" << std::endl;
        }
      }
    }
  } else if (componentName == "Image") {
    auto imageProps = std::dynamic_pointer_cast<const ImageProps>(props);
    if (imageProps && !imageProps->sources.empty()) {
      std::string uri = imageProps->sources[0].uri;
      if (!uri.empty()) {
        auto imageView = Dali::Toolkit::ImageView::DownCast(actor);
        if (imageView) {
          imageView.SetImage(uri);
          std::cout << "  -> UpdateProps: Image URL set to: " << uri
                    << std::endl;
        }
      }
    }
  }
}

void DaliMountingManager::UpdateLayout(
    Dali::Actor actor, facebook::react::LayoutMetrics const &layoutMetrics) {
  // DALi uses Top-Left origin usually. Fabric uses Frame (x, y, w, h).
  auto frame = layoutMetrics.frame;

  std::cout << "  -> Layout: x=" << frame.origin.x << " y=" << frame.origin.y
            << " w=" << frame.size.width << " h=" << frame.size.height
            << std::endl;

  actor.SetProperty(Dali::Actor::Property::POSITION,
                    Dali::Vector3(frame.origin.x, frame.origin.y, 0));
  actor.SetProperty(Dali::Actor::Property::SIZE,
                    Dali::Vector2(frame.size.width, frame.size.height));
}

void DaliMountingManager::DispatchEvent(int tag, std::string eventName) {
  // Use cached EventEmitter to dispatch
  auto it = mEventEmitterRegistry.find(tag);
  if (it != mEventEmitterRegistry.end() && it->second) {
    if (eventName == "touchStart") {
      // For simple connectivity verification, we dispatch a generic event.
      // Ideally, we construct a TouchEventPayload (Requires ValueFactory or
      // more complex setup) Fabric's dispatchEvent(type, payload) takes
      // folly::dynamic for payload.
      it->second->dispatchEvent(eventName, folly::dynamic::object());
      std::cout << "Dispatched " << eventName << " (Fabric) on Tag: " << tag
                << std::endl;
    } else if (eventName == "touchEnd") {
      it->second->dispatchEvent(eventName, folly::dynamic::object());
    }
    return;
  }

  // Fallback
  if (mEventCallback) {
    mEventCallback(tag, eventName);
  } else {
    std::cout << "Event Dispatched: " << eventName << " on Tag: " << tag
              << " (No Callback Registered)" << std::endl;
  }
}

void DaliMountingManager::SetEventCallback(
    std::function<void(int, std::string)> callback) {
  mEventCallback = callback;
}

bool DaliMountingManager::OnTouch(Dali::Actor actor,
                                  const Dali::TouchEvent &event) {
  int tag = -1;
  try {
    tag =
        std::stoi(actor.GetProperty<std::string>(Dali::Actor::Property::NAME));
  } catch (...) {
  }

  if (event.GetPointCount() > 0) {
    if (event.GetState(0) == Dali::PointState::DOWN) {
      std::cout << "Touch DOWN Detected on Actor Tag: " << tag << std::endl;
      DispatchEvent(tag, "touchStart");
    } else if (event.GetState(0) == Dali::PointState::UP) {
      std::cout << "Touch UP Detected on Actor Tag: " << tag << std::endl;
      DispatchEvent(tag, "touchEnd");
    }
  }
  return true; // Consume event
}
