/**
 * Pure C++ DALi App - Equivalent to React Native demo
 *
 * This app renders the same UI as our React Native app:
 * - A centered Image (200x200)
 * - "Hello from React Native!" text
 * - "Powered by Dali + Fabric" text
 */

#include <dali-toolkit/dali-toolkit.h>
#include <dali/dali.h>
#include <iostream>

using namespace Dali;
using namespace Dali::Toolkit;

class NativeDaliApp : public ConnectionTracker {
public:
  NativeDaliApp(Application &application) : mApplication(application) {
    mApplication.InitSignal().Connect(this, &NativeDaliApp::Create);
  }

  void Create(Application &application) {
    std::cout << "NativeDaliApp: Creating UI..." << std::endl;

    Window window = application.GetWindow();
    window.SetBackgroundColor(Color::WHITE);

    Window::WindowSize windowSize = window.GetSize();
    float windowWidth = windowSize.GetWidth();
    float windowHeight = windowSize.GetHeight();

    // Create root container (equivalent to View with flex:1)
    Actor rootContainer = Actor::New();
    rootContainer.SetProperty(Actor::Property::ANCHOR_POINT,
                              AnchorPoint::TOP_LEFT);
    rootContainer.SetProperty(Actor::Property::PARENT_ORIGIN,
                              ParentOrigin::TOP_LEFT);
    rootContainer.SetProperty(Actor::Property::SIZE,
                              Vector2(windowWidth, windowHeight));
    window.Add(rootContainer);

    // Image component - 200x200 centered
    // Using same image as RN app
    ImageView imageView = ImageView::New(
        "/Users/youngsus/tizen/dali-react-native/screenshot.png");
    imageView.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
    imageView.SetProperty(Actor::Property::PARENT_ORIGIN,
                          ParentOrigin::TOP_LEFT);
    imageView.SetProperty(Actor::Property::SIZE, Vector2(200, 200));
    // Center horizontally, position similar to RN layout
    float imageX = (windowWidth - 200) / 2.0f;                 // Centered
    float imageY = (windowHeight - 200 - 80 - 20 - 46) / 2.0f; // Above text
    imageView.SetProperty(Actor::Property::POSITION,
                          Vector3(imageX, imageY, 0));
    rootContainer.Add(imageView);

    // Text 1: "Hello from React Native!" - fontSize 32, blue, centered
    TextLabel text1 = TextLabel::New("Hello from React Native!");
    text1.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
    text1.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
    text1.SetProperty(TextLabel::Property::POINT_SIZE, 32.0f);
    text1.SetProperty(TextLabel::Property::TEXT_COLOR, Color::BLUE);
    text1.SetProperty(TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER");
    // Position below image
    float text1Y = imageY + 200 + 20; // 20px margin
    text1.SetProperty(Actor::Property::POSITION, Vector3(0, text1Y, 0));
    text1.SetProperty(Actor::Property::SIZE, Vector2(windowWidth, 80));
    rootContainer.Add(text1);

    // Text 2: "Powered by Dali + Fabric" - fontSize 18, gray, centered
    TextLabel text2 = TextLabel::New("Powered by Dali + Fabric");
    text2.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
    text2.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
    text2.SetProperty(TextLabel::Property::POINT_SIZE, 18.0f);
    text2.SetProperty(TextLabel::Property::TEXT_COLOR,
                      Vector4(0.5f, 0.5f, 0.5f, 1.0f)); // Gray
    text2.SetProperty(TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER");
    // Position below text1
    float text2Y = text1Y + 80 + 20; // 20px margin
    text2.SetProperty(Actor::Property::POSITION, Vector3(0, text2Y, 0));
    text2.SetProperty(Actor::Property::SIZE, Vector2(windowWidth, 46));
    rootContainer.Add(text2);

    std::cout << "NativeDaliApp: UI created successfully" << std::endl;
  }

private:
  Application &mApplication;
};

int main(int argc, char **argv) {
  Application application = Application::New(&argc, &argv);
  NativeDaliApp app(application);
  application.MainLoop();
  return 0;
}
