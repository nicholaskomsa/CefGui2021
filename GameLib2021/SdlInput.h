#pragma once

#include <map>
#include <string>

#include <chrono>

#include <SDL.h>




class SDLEventPoller {

public:
	class SDLEventHandler {
	protected:
		std::string mName;

	public:
		SDLEventHandler(const std::string& name);

		virtual ~SDLEventHandler() = default;

		virtual void handleEvent(SDL_Event* event);

		std::string getName();
	};

	std::map< std::string, SDLEventHandler* > mSDLEventHandlers;

public:
	~SDLEventPoller();

	void setup();
	void shutdown();

	void poll();

	void addEventHandler(const std::string& name, SDLEventHandler* eventHandler);
};


class SDLWindowEventHandler : public SDLEventPoller::SDLEventHandler {

public:

	SDLWindowEventHandler(const std::string& name = "windowEventHandler");

	virtual ~SDLWindowEventHandler() = default;

	void handleEvent(SDL_Event* event);

	virtual void quit(SDL_Event* event); //window X button
	virtual void windowResized(SDL_Event* event);
	virtual void windowClosed(SDL_Event* event);
};

class SDLMouseEventHandler : public SDLEventPoller::SDLEventHandler {
public:

	class Button {
		friend class SDLMouseEventHandler;
		std::chrono::milliseconds mTimeSinceLastClick{ 0 };
		std::chrono::milliseconds mTimeBetweenClicks{ 0 };
		bool mDown{ false };
		Uint8 mButtonID{ 0 };
		SDL_MouseButtonEvent event{ 0 };

	public:
		Button() = default;
		Button(Uint8 buttonID, std::chrono::milliseconds timeBetweenClicks);

		Uint8 getButtonID() const;
		const SDL_MouseButtonEvent& getEvent() const;
	};

	std::map< Uint8, Button > mButtons;

	int32_t mXRel{ 0 }, mYRel{ 0 }, mScrollY{ 0 };

public:
	SDLMouseEventHandler(const std::string& name = "mouseEventHandler");

	virtual ~SDLMouseEventHandler() = default;

	void addButton(Uint8 buttonID, std::chrono::milliseconds timeBetweenClicks = std::chrono::milliseconds(0));
	void handleEvent(SDL_Event* event);

	virtual void mouseMotion(const SDL_MouseMotionEvent& mouseMotion);
	virtual void mouseWheel(const SDL_MouseWheelEvent& mouseWheel);
	virtual void mouseDown(const Button& button);
	virtual void mouseUp(const Button& button);



	void setShowCursor(bool show);

	void setRelativeMouseMode(bool mode);
	bool getRelativeMouseMode();

	int32_t getXRel() { return mXRel; }
	int32_t getYRel() { return mYRel; }
	int getScroll() { return mScrollY; }
	void resetRel() { mXRel = mYRel = 0; mScrollY = 0; }
};



class SDLKeyEventHandler : public SDLEventPoller::SDLEventHandler {
public:


	class Key {
		friend class SDLKeyEventHandler;
		std::chrono::milliseconds mTimeSinceLastClick{ 0 };
		std::chrono::milliseconds mTimeBetweenClicks{ 0 };
		bool mDown{ false };
		SDL_Keycode mKeyCode{ 0 };
		std::string mText;
		SDL_KeyboardEvent event{ 0 };

	public:
		Key() = default;
		Key(SDL_Keycode keyCode, std::chrono::milliseconds timeBetweenClicks);
		SDL_Keycode getKeyCode() const;
		std::string getString() const;
		const SDL_KeyboardEvent& getEvent() const;
	};

	std::map< int, Key > mKeys;

public:
	SDLKeyEventHandler(const std::string& name = "keyEventHandler");

	virtual ~SDLKeyEventHandler() = default;

	void handleEvent(SDL_Event* event);

	void addKey(SDL_Keycode SDLKeyCode, std::chrono::milliseconds timeBetweenClicks = std::chrono::milliseconds(0));
	void addNumberKeys(std::chrono::milliseconds timeBetweenClicks = std::chrono::milliseconds(0));
	void addAlphaKeys(std::chrono::milliseconds timeBetweenClicks = std::chrono::milliseconds(0));

	virtual void keyUp(const Key& k);
	virtual void keyDown(const Key& k);

	const Key& getKey(int SDLKeyCode);
	bool isKeyDown(int SDLKeyCode);
};
