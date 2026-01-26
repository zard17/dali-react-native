#pragma once

#include <dali/dali.h>
#include <memory>

// Fabric Includes
// Fabric Includes
#include <ReactCommon/RuntimeExecutor.h>
#include <jsi/jsi.h>
#include <optional>
#include <react/renderer/componentregistry/ComponentDescriptorRegistry.h>
#include <react/renderer/components/image/ImageComponentDescriptor.h>
#include <react/renderer/components/text/ParagraphComponentDescriptor.h>
#include <react/renderer/components/text/RawTextComponentDescriptor.h>
#include <react/renderer/components/view/ViewComponentDescriptor.h>
#include <react/renderer/mounting/MountingCoordinator.h>
#include <react/renderer/scheduler/Scheduler.h>
#include <react/renderer/scheduler/SchedulerDelegate.h>
#include <react/renderer/scheduler/SurfaceHandler.h>
#include <react/utils/ContextContainer.h>

class DeviceInstanceManager : public facebook::react::SchedulerDelegate {
public:
  DeviceInstanceManager();
  ~DeviceInstanceManager();

  void Initialize();
  void StartSurface();
  void StopSurface();

  // JavaScript Bundle Loading
  void LoadJSBundle(const std::string &bundlePath);
  void StartReactApp(const std::string &appName, int rootTag);

  // Event Loop
  void TickEventLoop();

  // Simulates the JS bridge sending mutations
  void SimulateJSExecution(class DaliMountingManager *mountingManager);

  void SetMountingManager(class DaliMountingManager *mountingManager);

  // SchedulerDelegate
  void schedulerDidFinishTransaction(
      const std::shared_ptr<const facebook::react::MountingCoordinator>
          &mountingCoordinator) override;

  void schedulerShouldRenderTransactions(
      const std::shared_ptr<const facebook::react::MountingCoordinator>
          &mountingCoordinator) override;

  void schedulerDidRequestPreliminaryViewAllocation(
      const facebook::react::ShadowNode &shadowNode) override;

  void
  schedulerDidDispatchCommand(const facebook::react::ShadowView &shadowView,
                              std::string const &commandName,
                              folly::dynamic const &args) override;

  void
  schedulerDidSetIsJSResponder(const facebook::react::ShadowView &shadowView,
                               bool isJSResponder,
                               bool blockNativeResponder) override;

  void schedulerDidSendAccessibilityEvent(
      const facebook::react::ShadowView &shadowView,
      std::string const &eventType) override;

  void schedulerShouldSynchronouslyUpdateViewOnUIThread(
      facebook::react::Tag tag, const folly::dynamic &props) override;

  void schedulerDidUpdateShadowTree(
      const std::unordered_map<facebook::react::Tag, folly::dynamic>
          &tagToProps) override;

private:
  void RenderAppDemo(class DaliMountingManager *mountingManager);

  // Fabric Components
  std::shared_ptr<facebook::jsi::Runtime> mRuntime;
  std::shared_ptr<facebook::react::Scheduler> mScheduler;
  std::shared_ptr<facebook::react::ContextContainer> mContextContainer;
  std::shared_ptr<facebook::react::RuntimeScheduler> mRuntimeScheduler;
  std::optional<facebook::react::SurfaceHandler> mSurfaceHandler;

  // DALi
  class DaliMountingManager *mMountingManager = nullptr;
};
