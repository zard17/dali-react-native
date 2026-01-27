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

  // Real Mutation Handling
  void ProcessMutation(facebook::react::ShadowViewMutation const &mutation);

private:
#include <react/renderer/core/EventEmitter.h>

private:
  // Map from React Tag to DALi Actor
  std::unordered_map<int, Dali::Actor> mActorRegistry;
  // Map from React Tag to EventEmitter
  std::unordered_map<int, facebook::react::EventEmitter::Shared>
      mEventEmitterRegistry;

  Dali::Window mWindow;

  Dali::Actor CreateActor(int tag, std::string componentName);
  void UpdateProps(Dali::Actor actor, std::string componentName,
                   facebook::react::Props::Shared const &props);
  void UpdateLayout(Dali::Actor actor,
                    facebook::react::LayoutMetrics const &layoutMetrics);

  // Event Handling
  void DispatchEvent(int tag, std::string eventName);

public:
  void SetEventCallback(std::function<void(int, std::string)> callback);

private:
  std::function<void(int, std::string)> mEventCallback;
  bool OnTouch(Dali::Actor actor, const Dali::TouchEvent &event);
};
