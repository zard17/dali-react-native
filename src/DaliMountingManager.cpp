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
  // std::cout << "Processing Transaction..." << std::endl;
  // Mock loop
  // for (auto &mut : transaction.getMutations()) ProcessMutation(mut);
}

void DaliMountingManager::ProcessMockMutation(int tag, int parentTag,
                                              std::string componentName,
                                              std::string props) {
  std::cout << "Processing Mock Mutation: Tag=" << tag
            << " Parent=" << parentTag << " Name=" << componentName
            << std::endl;

  auto actor = CreateActor(tag, componentName, props);
  if (actor) {
    mActorRegistry[tag] = actor;

    // Explicit Hierarchy Logic
    if (parentTag > 0) {
      auto parentIt = mActorRegistry.find(parentTag);
      if (parentIt != mActorRegistry.end()) {
        parentIt->second.Add(actor);
      } else {
        // Parent not found? Maybe error or fallback.
        // For mock, we might just add to window to see it.
        std::cout << "Warning: Parent Tag " << parentTag << " not found for "
                  << tag << std::endl;
        if (mWindow)
          mWindow.Add(actor);
      }
    } else {
      // No parent (Root) -> Add to Window
      if (mWindow)
        mWindow.Add(actor);
    }
  }
}

Dali::Actor DaliMountingManager::CreateActor(int tag, std::string componentName,
                                             std::string props) {
  if (componentName == "View") {
    auto view = DaliViewComponent::New();

    // Default size/pos (can be overridden by ApplyProps later)
    view.SetProperty(Dali::Actor::Property::SIZE, Dali::Vector2(0, 0));
    view.SetProperty(Dali::Actor::Property::POSITION, Dali::Vector3(0, 0, 0));

    // Special handling for Root View (usually Tag 3 in our demos)
    if (tag == 3) {
      if (mWindow) {
        Dali::Window::WindowSize size = mWindow.GetSize();
        view.SetProperty(Dali::Actor::Property::SIZE,
                         Dali::Vector2(size.GetWidth(), size.GetHeight()));
      } else {
        view.SetProperty(Dali::Actor::Property::SIZE,
                         Dali::Vector2(1920, 1080));
      }
    }

    DaliViewComponent::ApplyProps(view, props);

    view.SetProperty(Dali::Actor::Property::NAME, std::to_string(tag));
    view.TouchedSignal().Connect(this, &DaliMountingManager::OnTouch);

    return view;
  } else if (componentName == "Text" || componentName == "Paragraph") {
    auto text = DaliTextComponent::New();
    text.SetProperty(Dali::Actor::Property::SIZE,
                     Dali::Vector2(400, 100)); // Default text size
    DaliTextComponent::ApplyProps(text, props);
    return text;
  } else if (componentName == "Image") {
    auto image = DaliImageComponent::New();
    DaliImageComponent::ApplyProps(image, props);
    return image;
  }
  return Dali::Actor();
}

void DaliMountingManager::DispatchEvent(int tag, std::string eventName) {
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
