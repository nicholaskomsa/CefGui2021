#pragma once

#include <string>
#include <map>
#include <functional>

#include <include/cef_client.h>

#include <SDL.h>



class IGuiSheet {
protected:
	std::string mUrl;

	IGuiSheet(const std::string& url);

public:
	std::string getUrl();
	virtual void call(const std::string& name, const CefRefPtr<CefListValue>& args) = 0;
	virtual std::vector< std::string> getCallbackNames() = 0;


	virtual void keyPressed(const SDL_KeyboardEvent& evt) {}
	virtual void keyReleased(const SDL_KeyboardEvent& evt) {}
	virtual void mousePressed(const SDL_MouseButtonEvent& evt) {}
	virtual void mouseReleased(const SDL_MouseButtonEvent& evt) {}
	virtual void mouseMoved(const SDL_MouseMotionEvent& evt) {}
};



class ArgExtractor {
	std::size_t mIndex{ 0 };
	CefRefPtr<CefListValue> mArgs;
public:
	ArgExtractor(const CefRefPtr<CefListValue>& args);
	ArgExtractor& next();
	operator int();
	operator double();
	operator std::string();
	operator bool();
};



class GuiSheet	:	public IGuiSheet {
public:
	typedef std::function<void(const CefRefPtr<CefListValue>& )> Callback;

private:
	typedef std::map< std::string, Callback > Callbacks;
	Callbacks mCallbacks;

protected:

	void addCallback(const std::string& name, Callback callback) {

		mCallbacks[name] = callback;
	}

public:
	GuiSheet(std::string url)
		:IGuiSheet(url)
	{}

	virtual ~GuiSheet() = default;

	void call(const std::string& name, const CefRefPtr<CefListValue>& args) override{
		if (mCallbacks.find(name) == mCallbacks.end()) return;
		mCallbacks[name]( args);
	}

	std::vector< std::string> getCallbackNames() override {
		std::vector<std::string> names;
		names.reserve(mCallbacks.size());
		for (auto& m : mCallbacks) {
			names.push_back(m.first);
		}
		
		return names;
	}
};