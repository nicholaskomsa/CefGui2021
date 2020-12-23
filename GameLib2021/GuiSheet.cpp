#include "guisheet.h"


IGuiSheet::IGuiSheet(const std::string& url)
	: mUrl(url)
{}

std::string IGuiSheet::getUrl() { return mUrl; }


ArgExtractor::ArgExtractor(const CefRefPtr<CefListValue>& args)
	:mArgs(args)
{}

ArgExtractor& ArgExtractor::next() {
	mIndex++;
	return *this;
}

ArgExtractor::operator int() {
	return mArgs->GetInt(mIndex - 1);
}
ArgExtractor::operator double() {
	return mArgs->GetDouble(mIndex - 1);
}
ArgExtractor::operator std::string() {
	return mArgs->GetString(mIndex - 1);
}
ArgExtractor::operator bool() {
	return mArgs->GetBool(mIndex - 1);
}

