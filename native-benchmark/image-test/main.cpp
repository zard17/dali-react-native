#include <dali-toolkit/dali-toolkit.h>
#include <dali/dali.h>
#include <dali/devel-api/adaptor-framework/application-devel.h>
#include <iostream>
#include <string>
#include <vector>

using namespace Dali;

const int IMAGE_COUNT = 100;

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

    CreateImages(window);
  }

  void CreateImages(Window &window) {
    // Grid 10x10
    int cols = 10;
    int w = 50;
    int h = 50;
    float gridWidth = cols * w;
    float gridHeight = (IMAGE_COUNT / cols) * h;

    // Create a centered container for images
    auto container = Toolkit::Control::New();
    container.SetProperty(Actor::Property::SIZE, Vector2(gridWidth, gridHeight));
    container.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    container.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
    window.Add(container);

    for (int i = 0; i < IMAGE_COUNT; i++) {
      std::string path =
          "/Users/youngsus/tizen/dali-react-native/assets/image_" +
          std::to_string(i % 10) + ".png";

      auto image = Toolkit::ImageView::New(path);
      image.SetProperty(Actor::Property::SIZE, Vector2(w, h));
      image.SetProperty(Actor::Property::POSITION,
                        Vector3((i % cols) * w, (i / cols) * h, 0));
      image.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
      image.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
      container.Add(image);
    }
    std::cout << "Created " << IMAGE_COUNT << " Images (Native)" << std::endl;
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
