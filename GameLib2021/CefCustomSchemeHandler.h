#pragma once

#include <include/wrapper/cef_stream_resource_handler.h>
#include <include/cef_scheme.h>

#include <string>

class ClientSchemeHandlerFactory;

class ClientResourceHandler : public CefResourceHandler {

	IMPLEMENT_REFCOUNTING(ClientResourceHandler);

	std::string mData;
	size_t mOffset;
	std::string mMimeType;

	ClientSchemeHandlerFactory* mFactory{ nullptr };

public:

	ClientResourceHandler(ClientSchemeHandlerFactory* factory);
	std::string getFileName(std::string url);
	virtual bool ProcessRequest(CefRefPtr<CefRequest> request, CefRefPtr<CefCallback> callback) override;

	virtual void GetResponseHeaders(CefRefPtr<CefResponse> response, int64& response_length, CefString& redirectUrl) override;
	virtual void Cancel() override;
	virtual bool ReadResponse(void* data_out, int bytes_to_read, int& bytes_read, CefRefPtr<CefCallback> callback) override;
};

class ClientSchemeHandlerFactory : public CefSchemeHandlerFactory {

	IMPLEMENT_REFCOUNTING(ClientSchemeHandlerFactory);

	std::string mRootPath;

public:

	ClientSchemeHandlerFactory(const std::string& rootPath);

	std::string getRootPath();

	virtual CefRefPtr<CefResourceHandler> Create(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		const CefString& scheme_name,
		CefRefPtr<CefRequest> request)
		override;

};
