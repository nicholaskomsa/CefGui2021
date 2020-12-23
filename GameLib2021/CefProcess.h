#pragma once

#include "include/cef_browser.h"
#include "include/cef_client.h"
#include "include/cef_app.h"

class Graphics;
class ManualTexture;
class IGuiSheet;

#include "SdlKeyCodesToWindows.h"
struct SDL_KeyboardEvent;
struct SDL_MouseButtonEvent;
struct SDL_MouseMotionEvent;
struct SDL_MouseWheelEvent;

#include "GuiRenderObject.h"


// Client app implementation for other process types.
class CefAppOther : public CefApp {

    IMPLEMENT_REFCOUNTING(CefAppOther);
    DISALLOW_COPY_AND_ASSIGN(CefAppOther);

public:
    CefAppOther() = default;
    virtual ~CefAppOther() = default;
};


class MyV8Handler : public CefV8Handler {

    IMPLEMENT_REFCOUNTING(MyV8Handler);

public:
    CefRefPtr<CefBrowser> mBrowser;
    std::vector<CefString>& mBindings;

    MyV8Handler(CefRefPtr<CefBrowser> browser, std::vector<CefString>& bindings);
    virtual ~MyV8Handler();

    bool Execute(const CefString& name,
        CefRefPtr<CefV8Value> object,
        const CefV8ValueList& arguments,
        CefRefPtr<CefV8Value>& retval,
        CefString& exception) override;

};

class CefAppRenderer : public CefApp, public CefRenderProcessHandler {

    IMPLEMENT_REFCOUNTING(CefAppRenderer);
    DISALLOW_COPY_AND_ASSIGN(CefAppRenderer);

    CefRefPtr<CefV8Handler> mHandler;

    std::vector< CefString > mCallbackNames;


    CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler();

    void OnContextCreated(CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        CefRefPtr<CefV8Context> context) override;

    void OnContextReleased(CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        CefRefPtr<CefV8Context> context) override;

    bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        CefProcessId source_process,
        CefRefPtr<CefProcessMessage> message) override;

public:

    CefAppRenderer() = default;
    virtual ~CefAppRenderer() = default;
};

// Returns a data: URI with the specified contents.
std::string GetDataURI(const std::string& data, const std::string& mime_type);

class SimpleHandler : public CefClient,
    public CefDisplayHandler,
    public CefLifeSpanHandler,
    public CefLoadHandler,
    public CefRenderHandler {

    // Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(SimpleHandler);

    CefRefPtr<CefBrowser> mBrowser;

    ManualTexture* mTex{ nullptr };

    IGuiSheet* mGuiSheet{ nullptr };

    int mCurrentKeyModifiers{ 0 };


    bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        CefProcessId source_process,
        CefRefPtr<CefProcessMessage> message) override;

    // CefCefRenderHandlerOgre interface
    void GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect) override;
    void OnPaint(CefRefPtr<CefBrowser> browser, PaintElementType type, const RectList& dirtyRects, const void* buffer, int width, int height) override;

    CefRefPtr<CefRenderHandler> GetRenderHandler() override;
    CefRefPtr<CefDisplayHandler> GetDisplayHandler() override;
    CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() override;
    CefRefPtr<CefLoadHandler> GetLoadHandler() override;

    // CefLifeSpanHandler methods:
    void OnAfterCreated(CefRefPtr<CefBrowser> browser) override;
    bool DoClose(CefRefPtr<CefBrowser> browser) override;
    void OnBeforeClose(CefRefPtr<CefBrowser> browser) override;

    bool OnConsoleMessage(CefRefPtr<CefBrowser> browser,
        cef_log_severity_t level,
        const CefString& message,
        const CefString& source,
        int line) override;

    // CefLoadHandler methods:
    void OnLoadError(CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        ErrorCode errorCode,
        const CefString& errorText,
        const CefString& failedUrl) override;


public:
    SimpleHandler() =default;

    virtual ~SimpleHandler() = default;


    void setTexture(ManualTexture& tex);
    void navigate(IGuiSheet* guiSheet);
    void CloseAllBrowsers();

    void call(const std::string& funcName, const std::string& args);

    void keyPressed(const SDL_KeyboardEvent& evt);
    void keyReleased(const SDL_KeyboardEvent& evt);

    void mousePressed(const SDL_MouseButtonEvent& evt);
    void mouseReleased(const SDL_MouseButtonEvent& evt);
    void mouseMoved(const SDL_MouseMotionEvent& evt);

    void mouseScrolled(const SDL_MouseWheelEvent& evt);
};

// Client app implementation for the browser process.
class CefAppBrowser : public CefApp, public CefBrowserProcessHandler {

    IMPLEMENT_REFCOUNTING(CefAppBrowser);
    DISALLOW_COPY_AND_ASSIGN(CefAppBrowser);

    CefRefPtr<SimpleHandler> mHandler;

    GuiRenderObject mGuiRenderObject;
public:
    CefAppBrowser() = default;
    virtual ~CefAppBrowser() = default;

    CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() override;

    void OnBeforeCommandLineProcessing(
        const CefString& process_type,
        CefRefPtr<CefCommandLine> command_line) override;

    void navigate(IGuiSheet* guiSheet);

    void OnContextInitialized() override;

    void destroyBrowser(Graphics& graphics);
    void createBrowser(Graphics& graphics);

    void call(const std::string& functionName, const std::string& params);

    CefRefPtr<SimpleHandler> getHandler();
};


class CefProcess {

    enum class ProcessType {
        BrowserProcess,
        RendererProcess,
        ZygoteProcess,
        OtherProcess,
    };

    ProcessType mProcessType;

    CefProcess::ProcessType GetProcessType(CefRefPtr<CefCommandLine> commandLine);
    void determineProcessType();

    CefRefPtr< CefApp> mApp;

public:
    ~CefProcess() = default;

    CefAppBrowser* getBrowserProcess();

    void branchProcess();

    void doEvents();

    void shutdown();
};