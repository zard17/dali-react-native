#pragma once

#include <dali/dali.h>
#include <functional>
#include <react/renderer/mounting/MountingCoordinator.h>
#include <react/renderer/mounting/ShadowViewMutation.h>
#include <string>
#include <unordered_map>

class DaliMountingManager : public Dali::ConnectionTracker {
public:
  DaliMountingManager();
  ~DaliMountingManager();

  void
  PerformTransaction(facebook::react::MountingTransaction const &transaction);

  void SetWindow(Dali::Window window);

  // void ProcessMutation(facebook::react::ShadowViewMutation const &mutation);
  // // Real one
  void ProcessMockMutation(int tag, int parentTag, std::string componentName,
                           std::string props);

private:
  // Map from React Tag to DALi Actor
  std::unordered_map<int, Dali::Actor> mActorRegistry;
  Dali::Window mWindow;

  Dali::Actor CreateActor(int tag, std::string componentName,
                          std::string props);

  // Event Handling
  void DispatchEvent(int tag, std::string eventName);

public:
  void SetEventCallback(std::function<void(int, std::string)> callback);

private:
  std::function<void(int, std::string)> mEventCallback;
  bool OnTouch(Dali::Actor actor, const Dali::TouchEvent &event);
};
