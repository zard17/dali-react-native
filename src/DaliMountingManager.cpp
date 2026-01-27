#include "DaliMountingManager.h"
#include "components/DaliImageComponent.h"
#include "components/DaliTextComponent.h"
#include "components/DaliViewComponent.h"
#include <iostream>

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
    // auto index = mutation.index; // Not used in current Dali::Actor::Add

    auto parentIt = mActorRegistry.find(parentTag);
    auto childIt = mActorRegistry.find(childTag);

    if (parentIt != mActorRegistry.end() && childIt != mActorRegistry.end()) {
      parentIt->second.Add(childIt->second);
    } else {
      std::cout << "Warning: Parent(" << parentTag << ") or Child(" << childTag
                << ") not found for Insert" << std::endl;
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
          std::cout << "  -> Root actor (tag 1) created and added to window"
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
  // TODO: Extract props from Shared Props object
  // For now, hardcode some colors or rely on existing ApplyProps via string if
  // we can serialize But we don't have serializer. Let's just set random color
  // for View to visualize? Or try to cast Props to ViewProps if possible?

  // Simplest visualization:
  // If View, set background color (random or fixed) so we see it.
  if (componentName == "View") {
    // cast to ViewProps?
    // #include <react/renderer/components/view/ViewProps.h> needed
    // For now, just setting a color for debugging
    // actor.SetProperty(Dali::Actor::Property::COLOR, Dali::Vector4(1.0f, 0.0f,
    // 0.0f, 1.0f));
  }

  // However, existing component classes take Strings.
  // We should refactor them later.
}

void DaliMountingManager::UpdateLayout(
    Dali::Actor actor, facebook::react::LayoutMetrics const &layoutMetrics) {
  // DALi uses Top-Left origin usually. Fabric uses Frame (x, y, w, h).
  auto frame = layoutMetrics.frame;
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
