#include <dali-toolkit/dali-toolkit.h>
#include <dali/dali.h>
#include <dali/devel-api/adaptor-framework/application-devel.h>
#include <iostream>
#include <string>
#include <vector>

using namespace Dali;

const int VIEW_COUNT = 1000;

class BenchmarkApp : public ConnectionTracker {
public:
  BenchmarkApp(Application &application) : mApplication(application) {
    mApplication.InitSignal().Connect(this, &BenchmarkApp::Create);
  }

  void Create(Application &application) {
    Window window = application.GetWindow();
    window.SetBackgroundColor(Color::WHITE);

    // Disable depth test like in dali-demo benchmark
    window.GetRootLayer().SetProperty(Layer::Property::DEPTH_TEST, false);

    CreateViews(window);
  }

  void CreateViews(Window &window) {
    // Grid 20x50
    int cols = 20;
    int w = 20;
    int h = 20;
    for (int i = 0; i < VIEW_COUNT; i++) {
      auto view = Toolkit::Control::New();
      view.SetBackgroundColor(Vector4(
          (float)(i % 10) / 10.0f, (float)((i / 10) % 10) / 10.0f, 0.5f, 1.0f));
      view.SetProperty(Actor::Property::SIZE, Vector2(w, h));
      view.SetProperty(Actor::Property::POSITION,
                       Vector3((i % cols) * w, (i / cols) * h, 0));
      view.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
      view.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
      window.Add(view);
    }
    std::cout << "Created " << VIEW_COUNT << " Views (Native)" << std::endl;
  }

private:
  Application &mApplication;
};

int main(int argc, char **argv) {
  // Use 800x600 window for consistent testing
  Application application = Application::New(&argc, &argv, "",
      Application::OPAQUE,
      PositionSize(100, 100, 800, 600));
  BenchmarkApp test(application);
  application.MainLoop();
  return 0;
}
