#define WIN32_LEAN_AND_MEAN
#define NOMINMAX // required to stop windows.h messing up std::min
#include <windows.h>

#include <thread>
#include <chrono>

#include <Graphics.h>
#include <CefProcess.h>
#include <guisheet.h>

#include <Exception.h>
#include <SdlInput.h>

#include <OgreItem.h>


class WindowEventHandler : public SDLWindowEventHandler {
	bool mQuit{ false };
public:
	void quit(SDL_Event* event) override {
		mQuit = true;
	}
	void windowResized(SDL_Event* event) override {

	}
	void windowClosed(SDL_Event* event) override {
		mQuit = true;
	}

	bool quit() { return mQuit; }
};

class KeyEventHandler : public SDLKeyEventHandler {

	CefAppBrowser* mBrowser{ nullptr };

	void keyUp(const Key& k) override {
		mBrowser->getHandler()->keyReleased(k.getEvent());
	}
	void keyDown(const Key& k) override {
		mBrowser->getHandler()->keyPressed(k.getEvent());
	}

public:
	KeyEventHandler(CefAppBrowser* browser)
	:mBrowser(browser)
	{
	}
};

class MouseEventHandler : public SDLMouseEventHandler {

	CefAppBrowser* mBrowser{ nullptr };

	int32_t mXRel{ 0 }, mYRel{ 0 }, mScrollY{ 0 };

	bool mLookMouse{ false };

	void mouseMotion(const SDL_MouseMotionEvent& mouseMotion) override {
		mXRel = mouseMotion.xrel;
		mYRel = mouseMotion.yrel;

		mBrowser->getHandler()->mouseMoved(mouseMotion);
	}
	void mouseWheel(const SDL_MouseWheelEvent& mouseWheel) override {
		mScrollY = mouseWheel.y;
		mBrowser->getHandler()->mouseScrolled(mouseWheel);
	}
	void mouseDown(const Button& button) override {
		mBrowser->getHandler()->mousePressed(button.getEvent());
	}
	void mouseUp(const Button& button) override {
		mBrowser->getHandler()->mouseReleased(button.getEvent());
	}
public:
	MouseEventHandler(CefAppBrowser* browser)
		:mBrowser(browser)
	{
		toggleMouseMode();
	}

	int32_t getXRel() { return mXRel; }
	int32_t getYRel() { return mYRel; }
	int32_t getScrollY() { return mScrollY; }
	void resetRel() { mXRel = 0; mYRel = 0; mScrollY = 0; }

	void toggleMouseMode() {
		mLookMouse = !mLookMouse;

		setShowCursor(!mLookMouse);
		setRelativeMouseMode(mLookMouse);
	}
	bool getFreeLook() { return mLookMouse; }

};


class MyGuiSheet : public GuiSheet {

	bool mQuit{ false };
	bool mLoaded{ false };

	Graphics& mGraphics;
	CefAppBrowser* mBrowser{ nullptr };

	Ogre::Item* mItem{ nullptr };
	Ogre::SceneNode* mSN{ nullptr };

	void unloadMesh() {
		if (mSN) {
			mSN->detachAllObjects();
			mGraphics.getSceneManager()->getRootSceneNode()->removeAndDestroyChild(mSN);
			mSN = nullptr;
		}
		if (mItem) {
			mGraphics.getSceneManager()->destroyItem(mItem);
			mItem = nullptr;
		}
	}

	void onLoadMesh(const CefRefPtr<CefListValue>& args) {

		ArgExtractor arge(args);

		std::string fileName = arge.next();

		unloadMesh();

		if (!fileName.size()) return;

		try {

			mItem = mGraphics.getSceneManager()->createItem(fileName);
			mSN = mGraphics.getSceneManager()->getRootSceneNode()->createChildSceneNode();
			mSN->attachObject(mItem);
		}
		catch (Ogre::Exception& e) {
			mBrowser->call("onLoadMeshError", '\"' + e.getDescription() + '\"');
		}
		catch (...) {
			mBrowser->call("onLoadMeshError", "onLoadMesh unhandled error");
		}

	}

public:
	MyGuiSheet(Graphics& graphics,  CefAppBrowser* browser)
		:GuiSheet("app://gui/debugger/debugger.html")
		, mGraphics(graphics)
		, mBrowser(browser)
	{
		addCallback("onGuiPageLoaded", [&](const CefRefPtr<CefListValue>& args) {
			mLoaded=true;
			});

		//javascript would call: window.onLoadMesh( "file.mesh" );
		addCallback("onLoadMesh", [&](const CefRefPtr<CefListValue>& args) {
			onLoadMesh(args);
			});
	}


	void updateField(const std::string& fieldName, float value) {
		if (!mLoaded) return;

		std::string cargs = "\"" + fieldName + "\", " + std::to_string(value);
		mBrowser->call("updateField", cargs);
	}
	void updateMessage(const std::string& fieldName, const std::string& message) {
		if (!mLoaded) return;

		std::string cargs = "\"" + fieldName + "\", \"" + message + "\"";
		mBrowser->call("updateMessage", cargs);

	}
	void keyPressed(const SDL_KeyboardEvent& evt) override {

		std::string cargs;
		switch (evt.keysym.sym) {
		case SDLK_ESCAPE:
			mQuit = true;
			break;
		case SDLK_F5:
			if (mLoaded) mBrowser->call("toggleDebugger", cargs);
			break;
		case SDLK_F6:
			if (mLoaded) mBrowser->call("fullReset", cargs);
			break;

		}
	}

	bool quit() { return mQuit; }
};


INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInstance, LPSTR strCmdLine, INT nCmdShow) {

    try {

		CefProcess cef;
		cef.branchProcess();


        Graphics graphics;

		auto setupGraphics = [&]() {

			graphics.setup();

			graphics.setAmbientLightColor(Ogre::ColourValue(0, 0, 0));
			graphics.setAmbientLightLevel(1);

			graphics.setupCompositorDefault();
		};
		setupGraphics();


		CefAppBrowser* browser{ nullptr };

		auto setupBrowser = [&]() {

			browser = cef.getBrowserProcess();
			browser->createBrowser(graphics);
		};
		setupBrowser();
		MyGuiSheet guiSheet(graphics, browser);


		SDLEventPoller poller;
		WindowEventHandler windowHandler;
		KeyEventHandler keyHandler(browser);
		MouseEventHandler mouseHandler(browser);

		auto setupInput = [&]() {

			keyHandler.addKey(SDLK_ESCAPE);
			keyHandler.addKey(SDLK_o, std::chrono::seconds(1));

			keyHandler.addKey(SDLK_F1, std::chrono::seconds(1));
			keyHandler.addKey(SDLK_F2, std::chrono::seconds(1));
			keyHandler.addKey(SDLK_F5, std::chrono::seconds(1));
			keyHandler.addKey(SDLK_F6, std::chrono::seconds(1));

			poller.setup();
			poller.addEventHandler("window", &windowHandler);
			poller.addEventHandler("keys", &keyHandler);
			poller.addEventHandler("mouse", &mouseHandler);
		};
		setupInput();

		browser->navigate(&guiSheet);

		auto doGameLoop = [&]() {

			std::chrono::steady_clock::time_point startTime=std::chrono::steady_clock::now(), endTime;

			int frameCount = 0;

			while (!windowHandler.quit() && !guiSheet.quit() ) {


				poller.poll();
				cef.doEvents();

				graphics.updateFrame();
				++frameCount;

				std::this_thread::sleep_for(std::chrono::milliseconds(1000/60));

				endTime = std::chrono::steady_clock::now();
	
				auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

				if (elapsedTime >= std::chrono::milliseconds(1000)) {

					guiSheet.updateField("FPS", frameCount );
					guiSheet.updateMessage("Status", "OK");
					guiSheet.updateMessage("Inputs", "F1, F5, F6");

					frameCount = 0;
					startTime = endTime;
				}
			}
		};
		doGameLoop();
		
		poller.shutdown();

		browser->destroyBrowser(graphics);

        graphics.shutdown();
		cef.shutdown();
    }
    catch ( Exception& e) {
        e.showMessage();
    }
    catch (Ogre::Exception& e) {
        Exception::showMessage(e.getFullDescription());
    }
    catch (...) {
        Exception::showMessage("Graphics::shutdown threw an uncaught exception");
    }

    return 0;
}