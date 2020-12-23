

#include "CefProcess.h"


#include <thread>
#include <chrono>
#include <filesystem>
#include <fstream>

#include "include/cef_parser.h"
#include "include/cef_command_line.h"
#include <include/wrapper/cef_helpers.h>

#include <sdl_keycode.h>
#include <SDL_keyboard.h>
#include <SDL_config_windows.h>




#include "guisheet.h"
#include "ManualTexture.h"
#include "CefCustomSchemeHandler.h"
#include "exception.h"
#include "graphics.h"




// Returns a data: URI with the specified contents.
std::string GetDataURI(const std::string& data, const std::string& mime_type) {
	return "data:" + mime_type + ";base64," +
		CefURIEncode(CefBase64Encode(data.data(), data.size()), false)
		.ToString();
}


MyV8Handler::MyV8Handler(CefRefPtr<CefBrowser> browser,  std::vector<CefString>& bindings)
	: mBrowser(browser)
	, mBindings(bindings)
{}

MyV8Handler::~MyV8Handler() {
	mBrowser = NULL;
}


bool MyV8Handler::Execute(const CefString& name,
	CefRefPtr<CefV8Value> object,
	const CefV8ValueList& arguments,
	CefRefPtr<CefV8Value>& retval,
	CefString& exception) {

	if (std::find(mBindings.begin(), mBindings.end(), name) == mBindings.end()) return false;

	CefRefPtr<CefProcessMessage> msg = CefProcessMessage::Create(name);
	CefRefPtr<CefListValue> msgArgs = msg->GetArgumentList();

	msgArgs->SetSize(arguments.size());

	for (std::size_t i = 0; i < arguments.size(); ++i) {

		CefV8Value& value = *arguments[i];

		if (value.IsInt())
			msgArgs->SetInt(i, value.GetIntValue());

		else if (value.IsDouble())
			msgArgs->SetDouble(i, value.GetDoubleValue());

		else if (value.IsBool())
			msgArgs->SetBool(i, value.GetBoolValue());

		else //make string
			msgArgs->SetString(i, value.GetStringValue());
	}

	mBrowser->GetMainFrame()->SendProcessMessage(CefProcessId::PID_BROWSER, msg);

	return true;
}

CefRefPtr<CefRenderProcessHandler> CefAppRenderer::GetRenderProcessHandler() { return this; }

void CefAppRenderer::OnContextCreated(CefRefPtr<CefBrowser> browser,
	CefRefPtr<CefFrame> frame,
	CefRefPtr<CefV8Context> context) {

	CefRefPtr<CefV8Value> globalContext = context->GetGlobal();

	// Create an instance of my CefV8Handler object.
	mHandler = new MyV8Handler(browser, mCallbackNames);

	for (auto& funcName : mCallbackNames) {

		CefRefPtr<CefV8Value> func = CefV8Value::CreateFunction(funcName, mHandler);

		globalContext->SetValue(funcName, func, V8_PROPERTY_ATTRIBUTE_NONE);
	}
}

void CefAppRenderer::OnContextReleased(CefRefPtr<CefBrowser> browser,
	CefRefPtr<CefFrame> frame,
	CefRefPtr<CefV8Context> context) {

	mHandler = NULL;
}


bool CefAppRenderer::OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
	CefRefPtr<CefFrame> frame,
	CefProcessId source_process,
	CefRefPtr<CefProcessMessage> message) {

	if (message->GetName() != "guicallbacks") return false;

	CefRefPtr<CefListValue> msgArgs = message->GetArgumentList();

	mCallbackNames.resize(msgArgs->GetSize() - 1);

	size_t n = 0;
	for (auto& name : mCallbackNames) {
		name = msgArgs->GetString(n++);
	}
	browser->GetMainFrame()->LoadURL(msgArgs->GetString(n));

	return true;
}


bool SimpleHandler::OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
	CefRefPtr<CefFrame> frame,
	CefProcessId source_process,
	CefRefPtr<CefProcessMessage> message) {

	CefRefPtr<CefListValue> msgArgs = message->GetArgumentList();

	mGuiSheet->call(message->GetName(), msgArgs);

	return true;
}

void SimpleHandler::navigate(IGuiSheet* guiSheet) {
	mGuiSheet = guiSheet;

	CefRefPtr<CefProcessMessage> msg = CefProcessMessage::Create("guicallbacks");
	CefRefPtr<CefListValue> msgArgs = msg->GetArgumentList();

	std::vector<std::string> callbackNames = mGuiSheet->getCallbackNames();

	std::size_t i = 0;
	for (auto name : callbackNames) {
		msgArgs->SetString(i++, name);
	}
	//the last string is the url to set
	msgArgs->SetString(i, mGuiSheet->getUrl());
	mBrowser->StopLoad();

	mBrowser->GetMainFrame()->SendProcessMessage(CefProcessId::PID_RENDERER, msg);
}


void SimpleHandler::keyPressed(const SDL_KeyboardEvent& evt) {

	if (mBrowser == NULL) return;

	if (mGuiSheet) mGuiSheet->keyPressed(evt);

	CefKeyEvent event;
	mCurrentKeyModifiers = getKeyboardModifiers(evt.keysym.mod);
	event.modifiers = mCurrentKeyModifiers;

	event.native_key_code = SDL_GetScancodeFromKey(evt.keysym.sym);
	event.windows_key_code = getWindowsKeyCode(evt.keysym);;

	event.type = KEYEVENT_RAWKEYDOWN;
	mBrowser->GetHost()->SendKeyEvent(event);
}

void SimpleHandler::keyReleased(const SDL_KeyboardEvent& evt) {

	if (mBrowser == NULL) return;

	if (mGuiSheet) mGuiSheet->keyReleased(evt);

	CefKeyEvent event;
	mCurrentKeyModifiers = getKeyboardModifiers(evt.keysym.mod);
	event.modifiers = mCurrentKeyModifiers;

	event.windows_key_code = getWindowsKeyCode(evt.keysym);;
	event.native_key_code = SDL_GetScancodeFromKey(evt.keysym.sym);

	event.type = KEYEVENT_KEYUP;
	mBrowser->GetHost()->SendKeyEvent(event);
}

void SimpleHandler::mousePressed(const SDL_MouseButtonEvent& evt) {
	if (mBrowser == NULL) return;

	if (mGuiSheet) mGuiSheet->mousePressed(evt);

	CefMouseEvent mouse_event;
	mouse_event.x = evt.x;
	mouse_event.y = evt.y;
	mouse_event.modifiers = mCurrentKeyModifiers;

	Uint8 id = evt.button;

	CefBrowserHost::MouseButtonType btnType = MBT_LEFT;
	switch (id) {
	case SDL_BUTTON_RIGHT:
		btnType = MBT_RIGHT;
		break;
	case SDL_BUTTON_MIDDLE:
		btnType = MBT_MIDDLE;
		break;
	}

	mBrowser->GetHost()->SendMouseClickEvent(mouse_event, btnType, false, 1);
}
void SimpleHandler::mouseReleased(const SDL_MouseButtonEvent& evt) {
	if (mBrowser == NULL) return;

	if (mGuiSheet) mGuiSheet->mouseReleased(evt);

	CefMouseEvent mouse_event;
	mouse_event.x = evt.x;
	mouse_event.y = evt.y;
	mouse_event.modifiers = mCurrentKeyModifiers;

	Uint8 id = evt.button;

	CefBrowserHost::MouseButtonType btnType = MBT_LEFT;
	switch (id) {
	case SDL_BUTTON_RIGHT:
		btnType = MBT_RIGHT;
		break;
	case SDL_BUTTON_MIDDLE:
		btnType = MBT_MIDDLE;
		break;
	}

	mBrowser->GetHost()->SendMouseClickEvent(mouse_event, btnType, true, 1);
}

void SimpleHandler::mouseMoved(const SDL_MouseMotionEvent& evt) {
	if (mBrowser == NULL) return;

	if (mGuiSheet) mGuiSheet->mouseMoved(evt);

	CefMouseEvent mouse_event;
	mouse_event.x = evt.x;
	mouse_event.y = evt.y;
	mouse_event.modifiers = mCurrentKeyModifiers;

	mBrowser->GetHost()->SendMouseMoveEvent(mouse_event, false);
}

void SimpleHandler::mouseScrolled(const SDL_MouseWheelEvent& evt) {

	CefMouseEvent mouse_event;
	mouse_event.x = evt.x;
	mouse_event.y = evt.y;
	mouse_event.modifiers = mCurrentKeyModifiers;

	mBrowser->GetHost()->SendMouseWheelEvent(mouse_event, evt.x, evt.y);
}



void SimpleHandler::setTexture(ManualTexture& tex) {
	mTex = &tex;
	mBrowser->GetHost()->WasResized();
}

// CefCefRenderHandlerOgre interface
void SimpleHandler::GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect) {
	if (!mTex)
		rect = CefRect(0, 0, 1, 1);
	else
		rect = CefRect(0, 0, mTex->getImage().getWidth(), mTex->getImage().getHeight());

}
void SimpleHandler::OnPaint(CefRefPtr<CefBrowser> browser, PaintElementType type, const RectList& dirtyRects, const void* buffer, int width, int height) {
	if (!mTex) return;

	for (auto dirty : dirtyRects) {

		Ogre::Rect rect(dirty.x, dirty.y, dirty.x + dirty.width, dirty.y + dirty.height);

		mTex->regionCopy(static_cast<const Ogre::uint8*>(buffer), rect);
	}

	//mTex->copy(static_cast<const Ogre::uint8*>(buffer));
	mTex->update();
}

CefRefPtr<CefRenderHandler> SimpleHandler::GetRenderHandler() { return this; }
CefRefPtr<CefDisplayHandler> SimpleHandler::GetDisplayHandler() { return this; }
CefRefPtr<CefLifeSpanHandler> SimpleHandler::GetLifeSpanHandler() { return this; }
CefRefPtr<CefLoadHandler> SimpleHandler::GetLoadHandler() { return this; }

// CefLifeSpanHandler methods:
void SimpleHandler::OnAfterCreated(CefRefPtr<CefBrowser> browser) {
	
	mBrowser = browser; 

	mBrowser->GetHost()->Invalidate(CefBrowserHost::PaintElementType::PET_VIEW);
}
bool SimpleHandler::DoClose(CefRefPtr<CefBrowser> browser) { return false; }
void SimpleHandler::OnBeforeClose(CefRefPtr<CefBrowser> browser) { }

bool SimpleHandler::OnConsoleMessage(CefRefPtr<CefBrowser> browser,
	cef_log_severity_t level,
	const CefString& message,
	const CefString& source,
	int line) {

	return false;
}

// CefLoadHandler methods:
void SimpleHandler::OnLoadError(CefRefPtr<CefBrowser> browser,
	CefRefPtr<CefFrame> frame,
	ErrorCode errorCode,
	const CefString& errorText,
	const CefString& failedUrl) {

	CEF_REQUIRE_UI_THREAD();

	// Don't display an error for downloaded files.
	if (errorCode == ERR_ABORTED)
		return;

	// Display a load error message using a data: URI.
	std::stringstream ss;
	ss << "<html><body bgcolor=\"white\">"
		"<h2>Failed to load URL "
		<< std::string(failedUrl) << " with error " << std::string(errorText)
		<< " (" << errorCode << ").</h2></body></html>";

	frame->LoadURL(GetDataURI(ss.str(), "text/html"));
}

void SimpleHandler::CloseAllBrowsers() {

	mBrowser->GetHost()->CloseBrowser(true);

	while (!mBrowser->HasOneRef()) {
		CefDoMessageLoopWork();
	}

	mBrowser = NULL;
}
void SimpleHandler::call(const std::string& funcName, const std::string& args) {
	mBrowser->GetMainFrame()->ExecuteJavaScript(funcName + "(" + args + ");", mGuiSheet->getUrl(), 0);
}


CefRefPtr<CefBrowserProcessHandler> CefAppBrowser::GetBrowserProcessHandler() {
	return this;
}

void CefAppBrowser::OnBeforeCommandLineProcessing(
	const CefString& process_type,
	CefRefPtr<CefCommandLine> command_line) {

	command_line->AppendSwitch("--disable-extensions");
}

void CefAppBrowser::navigate(IGuiSheet* guiSheet) {
	mHandler->navigate(guiSheet);
}


void CefAppBrowser::OnContextInitialized() {
	bool browserInit = true;

};

void CefAppBrowser::call(const std::string& functionName, const std::string& params) {

	mHandler->call(functionName, params);
}

void CefAppBrowser::destroyBrowser(Graphics& graphics) {
	mHandler->CloseAllBrowsers();
	mHandler = NULL;

	CefClearSchemeHandlerFactories();

	mGuiRenderObject.destroy(graphics);
}

void CefAppBrowser::createBrowser(Graphics& graphics) {

	CEF_REQUIRE_UI_THREAD();

	mHandler = new SimpleHandler();

	std::string url = "app://gui/cefgui_index.html";

	std::string scheme = "app";
	std::string domain = "gui";

	CefRegisterSchemeHandlerFactory(scheme, domain, new ClientSchemeHandlerFactory("../resources/gui/"));

	std::size_t hwnd = 0;
	graphics.getRenderWindow()->getCustomAttribute("WINDOW", &hwnd);

	CefWindowInfo windowInfo;
	windowInfo.SetAsWindowless((HWND)hwnd);
	windowInfo.windowless_rendering_enabled = true;

	CefBrowserSettings browserSettings;
	browserSettings.background_color = CefColorSetARGB(0, 0, 0, 0);	//make transparent
	browserSettings.windowless_frame_rate = 30;

	CefBrowserHost::CreateBrowser(windowInfo, mHandler, url, browserSettings,
		NULL, NULL);

	CefDoMessageLoopWork();

	mGuiRenderObject.create(graphics);

	mHandler->setTexture(mGuiRenderObject.getRenderTexture());
}
CefRefPtr<SimpleHandler> CefAppBrowser::getHandler() {
	return mHandler;
}


CefProcess::ProcessType CefProcess::GetProcessType(CefRefPtr<CefCommandLine> commandLine) {

	const std::string processTypeID{ "type" };
	const std::string rendererProcessID{ "renderer" };

	// The command-line flag won't be specified for the browser process.
	if (!commandLine->HasSwitch(processTypeID.c_str()))
		return ProcessType::BrowserProcess;

	const std::string& process_type = commandLine->GetSwitchValue(processTypeID.c_str());
	if (process_type == rendererProcessID)
		return ProcessType::RendererProcess;

	return ProcessType::OtherProcess;
}
void CefProcess::determineProcessType() {

	CefRefPtr<CefCommandLine> commandLine;
	commandLine = CefCommandLine::CreateCommandLine();
	commandLine->InitFromString(::GetCommandLineW());
	commandLine->AppendSwitchWithValue("autoplay-policy", "no-user-gesture-required");

	mProcessType = GetProcessType(commandLine);

	switch (mProcessType) {

	case ProcessType::BrowserProcess:
		mApp = new CefAppBrowser();
		break;

	case ProcessType::RendererProcess:
	case ProcessType::ZygoteProcess:
		mApp = new CefAppRenderer();
		break;

	case ProcessType::OtherProcess:
		mApp = new CefAppOther();
		break;

	default:
		EXCEPT << "CefProcess::determineProcessType; invalid type!";
	}
}



void CefProcess::branchProcess() {

	CefMainArgs main_args(::GetModuleHandle(NULL));

	determineProcessType();

	void* sandbox_info = NULL;

	int exit_code = CefExecuteProcess(main_args, mApp, sandbox_info);
	if (exit_code >= 0) {
		// The sub-process has completed so exit.
		mApp = NULL;
		exit(exit_code);
	}

	// Specify CEF global settings here.
	CefSettings settings;
	settings.windowless_rendering_enabled = true;
	settings.no_sandbox = true;

	//reset the debug file
	std::ofstream("debug.log", std::ios::out);

	std::filesystem::path path = std::filesystem::canonical("..\\resources\\cefcache");
	CefString(&settings.cache_path).FromWString(path.generic_wstring());

	CefInitialize(main_args, settings, mApp, sandbox_info);
}

void CefProcess::doEvents() {
	CefDoMessageLoopWork();
}

void CefProcess::shutdown() {
	mApp = NULL;
	for (std::size_t i = 0; i < 10; ++i) {
		CefDoMessageLoopWork();
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
	}
	CefShutdown();
}

CefAppBrowser* CefProcess::getBrowserProcess() {
	CefAppBrowser* browser = dynamic_cast<CefAppBrowser*>(mApp.get());
	return browser;
}
