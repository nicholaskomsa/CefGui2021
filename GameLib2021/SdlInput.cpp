#pragma once

#include "SdlInput.h"
#include "Exception.h"

#include <algorithm>
#include <SDL_syswm.h>



SDLEventPoller::~SDLEventPoller() {
	shutdown();
}

SDLEventPoller::SDLEventHandler::SDLEventHandler(const std::string& name)
	:mName(name)
{}
void SDLEventPoller::SDLEventHandler::handleEvent(SDL_Event* event) { }

std::string SDLEventPoller::SDLEventHandler::getName() { return mName; }

void SDLEventPoller::setup() {
	if (!SDL_WasInit(SDL_INIT_EVENTS)) {
		SDL_InitSubSystem(SDL_INIT_EVENTS);
	}
}
void  SDLEventPoller::shutdown() {
	try {
		SDL_QuitSubSystem(SDL_INIT_EVENTS);
	}
	catch (...) {
		Exception::showMessage("SDLEventPoller::shutdown uncaught exception");
	}
}

void  SDLEventPoller::poll() {

	SDL_Event event;

	while (SDL_PollEvent(&event)) {
		std::for_each(mSDLEventHandlers.begin(), mSDLEventHandlers.end(), [&](auto eventHandler) {
			eventHandler.second->handleEvent(&event);
		});
	}
}

void  SDLEventPoller::addEventHandler(const std::string& name, SDLEventHandler* eventHandler) {

	mSDLEventHandlers[name] = eventHandler;

}


SDLWindowEventHandler::SDLWindowEventHandler(const std::string& name)
	: SDLEventPoller::SDLEventHandler(name)
{}


void SDLWindowEventHandler::handleEvent(SDL_Event* event) {

	switch (event->type) {
	case SDL_QUIT:
		quit(event);
		return;
	case SDL_WINDOWEVENT:
		switch (event->window.event) {
		case SDL_WINDOWEVENT_RESIZED:
			windowResized(event);
			return;
		case SDL_WINDOWEVENT_CLOSE:
			windowClosed(event);
			return;
		default:
			return;
		}
	}
}
void SDLWindowEventHandler::quit(SDL_Event* event) {}//window X button
void SDLWindowEventHandler::windowResized(SDL_Event* event) {}
void SDLWindowEventHandler::windowClosed(SDL_Event* event) {}


SDLMouseEventHandler::Button::Button(Uint8 buttonID, std::chrono::milliseconds timeBetweenClicks)
	: mButtonID(buttonID)
	, mTimeBetweenClicks(timeBetweenClicks)
{}

Uint8 SDLMouseEventHandler::Button::getButtonID() const { return mButtonID; }
const SDL_MouseButtonEvent& SDLMouseEventHandler::Button::getEvent() const { return event; }

SDLMouseEventHandler::SDLMouseEventHandler(const std::string& name)
	: SDLEventPoller::SDLEventHandler(name)
{}

void SDLMouseEventHandler::addButton(Uint8 buttonID, std::chrono::milliseconds timeBetweenClicks) {
	//note this will replace a previously defined buttonID
	mButtons[buttonID] = Button(buttonID, timeBetweenClicks);
}

void SDLMouseEventHandler::handleEvent(SDL_Event* event) {
	std::chrono::milliseconds curTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch());

	resetRel();

	switch (event->type) {
	case SDL_MOUSEWHEEL:
		mScrollY = event->wheel.y;
		mouseWheel(event->wheel);
		return;
	case SDL_MOUSEMOTION:
		mXRel = event->motion.xrel;
		mYRel = event->motion.yrel;
		mouseMotion(event->motion);
		return;
	}

	bool isButtonDown = false;

	switch (event->type) {
	case SDL_MOUSEBUTTONDOWN:
		isButtonDown = true;
		break;
	case SDL_MOUSEBUTTONUP:
		isButtonDown = false;
		break;
	default:
		return;
	}

	Uint8 mouseButtonID = event->button.button;

	auto buttonPair = mButtons.find(mouseButtonID);
	if (buttonPair == mButtons.end()) {
		//LogManager::get("EventManager") << "Handle Event Button Not Found: " << mName << ", " << (int)mouseButtonID << Log::end;
		return; //EXCEPT <<  "Handle Event Button Not Found";
	}
	auto& button = buttonPair->second;
	button.event = event->button;

	std::chrono::milliseconds elapsedTime = curTime - button.mTimeSinceLastClick;

	if (elapsedTime >= button.mTimeBetweenClicks && isButtonDown) {
		button.mDown = true;
		button.mTimeSinceLastClick = curTime;
		mouseDown(button);
	}
	else if (!isButtonDown && button.mDown) {
		button.mDown = false;
		mouseUp(button);
	}


}

void SDLMouseEventHandler::mouseMotion(const SDL_MouseMotionEvent& mouseMotion) {}
void SDLMouseEventHandler::mouseWheel(const SDL_MouseWheelEvent& mouseWheel) {}
void SDLMouseEventHandler::mouseDown(const Button& button) {}
void SDLMouseEventHandler::mouseUp(const Button& button) {}


void SDLMouseEventHandler::setShowCursor(bool show) {
	SDL_ShowCursor(show ? 1 : 0);
}

void SDLMouseEventHandler::setRelativeMouseMode(bool mode) {
	SDL_SetRelativeMouseMode(mode ? SDL_TRUE : SDL_FALSE);
}
bool SDLMouseEventHandler::getRelativeMouseMode() {
	return SDL_GetRelativeMouseMode();
}

SDLKeyEventHandler::Key::Key(SDL_Keycode keyCode, std::chrono::milliseconds timeBetweenClicks)
	:mKeyCode(keyCode)
	, mTimeBetweenClicks(timeBetweenClicks)
{}
SDL_Keycode SDLKeyEventHandler::Key::getKeyCode() const {
	return mKeyCode;
}
std::string SDLKeyEventHandler::Key::getString() const {
	return mText;
}
const SDL_KeyboardEvent& SDLKeyEventHandler::Key::getEvent() const { return event; }


SDLKeyEventHandler::SDLKeyEventHandler(const std::string& name)
	: SDLEventPoller::SDLEventHandler(name)
{}


void SDLKeyEventHandler::handleEvent(SDL_Event* event) {

	std::chrono::milliseconds curTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch());


	bool isKeyDown = false;
	switch (event->type) {
	case SDL_KEYDOWN:
		isKeyDown = true;
		break;
	case SDL_KEYUP:
		isKeyDown = false;
		break;
	default:
		return;
	}

	int SDLKeyCode = event->key.keysym.sym;

	auto keyPair = mKeys.find(SDLKeyCode);
	if (keyPair == mKeys.end()) {
		//LogManager::get("EventManager") << "Handle Event Key Not Found: " << mName << ", " << SDLKeyCode << Log::end;
		//EXCEPT << "Handle Event KeyNotFound";
		return;
	}
	auto& key = keyPair->second;
	key.event = event->key;

	std::chrono::milliseconds elapsedTime = curTime - key.mTimeSinceLastClick;

	key.mText = event->text.text;

	if (elapsedTime > key.mTimeBetweenClicks&& isKeyDown) {

		keyDown(key);
		key.mTimeSinceLastClick = curTime;
		key.mDown = true;
	}
	else if (!isKeyDown && key.mDown) {
		keyUp(key);
		key.mDown = false;
	}

}

void SDLKeyEventHandler::addKey(SDL_Keycode SDLKeyCode, std::chrono::milliseconds timeBetweenClicks) {
	//note this will replace a previously defined keyID
	mKeys[SDLKeyCode] = Key(SDLKeyCode, timeBetweenClicks);
}
void SDLKeyEventHandler::addNumberKeys(std::chrono::milliseconds timeBetweenClicks) {
	addKey(SDLK_0, timeBetweenClicks);
	addKey(SDLK_1, timeBetweenClicks);
	addKey(SDLK_2, timeBetweenClicks);
	addKey(SDLK_3, timeBetweenClicks);
	addKey(SDLK_4, timeBetweenClicks);
	addKey(SDLK_5, timeBetweenClicks);
	addKey(SDLK_6, timeBetweenClicks);
	addKey(SDLK_7, timeBetweenClicks);
	addKey(SDLK_8, timeBetweenClicks);
	addKey(SDLK_9, timeBetweenClicks);
	addKey(SDLK_PERIOD, timeBetweenClicks);
}
void SDLKeyEventHandler::addAlphaKeys(std::chrono::milliseconds timeBetweenClicks) {
	int sdlKeyStart = SDLK_a;
	int sdlKeyEnd = SDLK_z;

	for (int key = sdlKeyStart; key <= sdlKeyEnd; ++key) {
		addKey(key);
	}

}

void SDLKeyEventHandler::keyUp(const Key& k) {}
void SDLKeyEventHandler::keyDown(const Key& k) {}

const SDLKeyEventHandler::Key& SDLKeyEventHandler::getKey(int SDLKeyCode) {
	auto key = mKeys.find(SDLKeyCode);
	if (key == mKeys.end())
		EXCEPT << "key does not exist";

	return key->second;
}
bool SDLKeyEventHandler::isKeyDown(int SDLKeyCode) {
	if (getKey(SDLKeyCode).mDown) return true;
	return false;
}
