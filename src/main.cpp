#include "DaliRenderer.h"
#include <dali-toolkit/dali-toolkit.h>
#include <dali/dali.h>

using namespace Dali;

class DaliRnDemo : public ConnectionTracker {
public:
  DaliRnDemo(Application &application) : mApplication(application) {
    mApplication.InitSignal().Connect(this, &DaliRnDemo::Create);
  }

  void Create(Application &application) {
    Window window = application.GetWindow();
    window.SetBackgroundColor(Color::WHITE);

    mRenderer = std::make_unique<DaliRenderer>();
    mRenderer->Init(application);

    // Toolkit::TextLabel textLabel =
    //     Toolkit::TextLabel::New("DALi React Native Renderer");
    // textLabel.SetProperty(Actor::Property::ANCHOR_POINT,
    // AnchorPoint::CENTER);
    // textLabel.SetProperty(Actor::Property::PARENT_ORIGIN,
    // ParentOrigin::CENTER); window.Add(textLabel);
  }

private:
  Application &mApplication;
  std::unique_ptr<DaliRenderer> mRenderer;
};

int main(int argc, char **argv) {
  // Use smaller window for testing (800x600)
  Application application = Application::New(&argc, &argv, "",
      Dali::Application::OPAQUE,
      Dali::PositionSize(100, 100, 800, 600));
  DaliRnDemo test(application);
  application.MainLoop();
  return 0;
}
