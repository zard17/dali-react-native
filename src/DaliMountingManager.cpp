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

void DaliMountingManager::ProcessMockMutation(int tag,
                                              std::string componentName,
                                              std::string props) {
  std::cout << "Processing Mock Mutation: Tag=" << tag
            << " Name=" << componentName << std::endl;

  // Use provided componentName
  // if (tag % 3 == 0) ... (Removed override)

  auto actor = CreateActor(tag, componentName, props);
  if (actor) {
    mActorRegistry[tag] = actor;

    // Simple hierarchy logic for simulation:
    // Tag 3 is Root
    // Tag 10 is child of 3
    // Tag 11 is child of 3

    if (tag == 3) {
      if (mWindow) {
        mWindow.Add(actor);
      }
    } else if (tag == 10 || tag == 11 || tag >= 100) {
      // Find root (Tag 3) and add
      auto rootIt = mActorRegistry.find(3);
      if (rootIt != mActorRegistry.end()) {
        rootIt->second.Add(actor);
      } else {
        // Fallback if root not found yet (should not happen in this seq)
        if (mWindow)
          mWindow.Add(actor);
      }
    } else {
      if (mWindow)
        mWindow.Add(actor);
    }
  }
}

Dali::Actor DaliMountingManager::CreateActor(int tag, std::string componentName,
                                             std::string props) {
  if (componentName == "View") {
    auto view = DaliViewComponent::New();

    if (tag == 3) {
      // Root View - Full Screen
      // In real Fabric, LayoutMetrics define this.
      // For mock, we use Window size if available, or fallback.
      if (mWindow) {
        Dali::Window::WindowSize size = mWindow.GetSize();
        view.SetProperty(Dali::Actor::Property::SIZE,
                         Dali::Vector2(size.GetWidth(), size.GetHeight()));
      } else {
        view.SetProperty(Dali::Actor::Property::SIZE,
                         Dali::Vector2(1920, 1080));
      }
    } else if (tag >= 100) {
      // Grid Items
      view.SetProperty(Dali::Actor::Property::SIZE, Dali::Vector2(100, 100));

      int index = tag - 100;
      int cols = 5;
      int col = index % cols;
      int row = index / cols;
      float stride = 110.0f;
      float startX = 50.0f;
      float startY = 50.0f;

      view.SetProperty(
          Dali::Actor::Property::POSITION,
          Dali::Vector3(startX + col * stride, startY + row * stride, 0));
      view.SetProperty(Dali::Actor::Property::PARENT_ORIGIN,
                       Dali::ParentOrigin::TOP_LEFT);
      view.SetProperty(Dali::Actor::Property::ANCHOR_POINT,
                       Dali::AnchorPoint::TOP_LEFT);

    } else {
      view.SetProperty(Dali::Actor::Property::SIZE,
                       Dali::Vector2(200, 200)); // Default Child view
      view.SetProperty(Dali::Actor::Property::POSITION, Dali::Vector3(0, 0, 0));
    }

    // Root (Tag 3) already handled above. Child (Tag 10) handled above.
    // We can keep specific logic if needed, but for >100 we use grid.

    if (tag == 10) {
      view.SetProperty(Dali::Actor::Property::PARENT_ORIGIN,
                       Dali::ParentOrigin::CENTER);
      view.SetProperty(Dali::Actor::Property::ANCHOR_POINT,
                       Dali::AnchorPoint::CENTER);
    }

    DaliViewComponent::ApplyProps(view, props);
    return view;
  } else if (componentName == "Text" || componentName == "Paragraph") {
    auto text = DaliTextComponent::New();
    text.SetProperty(Dali::Actor::Property::SIZE, Dali::Vector2(400, 100));
    text.SetProperty(Dali::Actor::Property::PARENT_ORIGIN,
                     Dali::ParentOrigin::BOTTOM_CENTER);
    text.SetProperty(Dali::Actor::Property::ANCHOR_POINT,
                     Dali::AnchorPoint::BOTTOM_CENTER);
    text.SetProperty(Dali::Actor::Property::POSITION,
                     Dali::Vector3(0, -100, 0));
    DaliTextComponent::ApplyProps(text, props);
    return text;
  } else if (componentName == "Image") {
    auto image = DaliImageComponent::New();
    image.SetProperty(Dali::Actor::Property::SIZE, Dali::Vector2(100, 100));
    image.SetProperty(Dali::Actor::Property::POSITION,
                      Dali::Vector3(10 * (tag % 10), 10 * (tag / 10) + 100, 0));
    DaliImageComponent::ApplyProps(image, props);
    return image;
  }
  return Dali::Actor();
}
