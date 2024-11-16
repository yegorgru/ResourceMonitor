#include "HttpMessage.h"
#include "Log.h"

#include <stdexcept>

namespace Http {

namespace {
	std::string methodToStr(MessageRequest::Method method) {
		switch (method) {
		case MessageRequest::Method::GET:
			return "GET";
		case MessageRequest::Method::PUT:
			return "PUT";
		}
		throw std::runtime_error("Method is not implemented");
	}

	MessageRequest::Method strToMethod(const std::string& str) {
		if (str == "GET") {
			return MessageRequest::Method::GET;
		}
		else if (str == "PUT") {
			return MessageRequest::Method::PUT;
		}
		throw std::runtime_error("Method is not implemented");
	}

	const std::string& getStatusLine(int code) {
		static const std::map<int, std::string> httpStatusTable =
		{
			{ 200, "200 OK" },
			{ 404, "404 Not Found" },
			{ 413, "413 Request Entity Too Large" },
			{ 500, "500 Server Error" },
			{ 501, "501 Not Implemented" },
			{ 505, "505 HTTP Version Not Supported" }
		};
		auto found = httpStatusTable.find(code);
		if (found != httpStatusTable.end()) {
			return found->second;
		}
		LOG::Throw("Status code not found");
		return "";
	}
}

void Message::addHeader(const std::string& name, const std::string& value) {
	mHeaders[name] = value;
}

const std::string& Message::getHeader(const std::string& name) const {
	auto found = mHeaders.find(name);
	if (found != mHeaders.end()) {
		return found->second;
	}
	LOG::Throw("Header not found");
	return "";
}

void Message::setBody(std::string&& body) {
	mBody = std::move(body);
}

const std::string& Message::getBody() const {
	return mBody;
}

MessageRequest::MessageRequest()
	: mMethod(Method::GET)
{
}

const std::string& MessageRequest::createStringRepresentation() {
	if (mStringRepresentation != "") {
		LOG::Throw("String representation was already created");
	}
	mStringRepresentation = methodToStr(mMethod) + " " + mResource + " " + STANDARD + "\r\n";
	for (auto& [name, value] : mHeaders) {
		mStringRepresentation += name + ": " + value + "\r\n";
	}
	mStringRepresentation += "\r\n";
	mStringRepresentation += mBody;
	return mStringRepresentation;
}

void MessageRequest::setMethod(const std::string& method) {
	mMethod = strToMethod(method);
}

MessageRequest::Method MessageRequest::getMethod() const {
	return mMethod;
}

void MessageRequest::setResource(const std::string& resource) {
	mResource = resource;
}

const std::string& MessageRequest::getResource() const {
	return mResource;
}

MessageResponse::MessageResponse()
	: mStatusCode(200)
{
}

const std::string& MessageResponse::createStringRepresentation() {
	if (mStringRepresentation != "") {
		LOG::Throw("String representation was already created");
	}
	mStringRepresentation = std::string("HTTP/1.1 ") + (mStatusMessage == "" ? getStatusLine(mStatusCode) : mStatusMessage) + "\r\n";
	for (const auto& [key, value] : mHeaders) {
		mStringRepresentation += key + ": " + value + "\r\n";
	}
	mStringRepresentation += "\r\n";
	mStringRepresentation += mBody;
	return mStringRepresentation;
}

void MessageResponse::setStatusCode(int statusCode) {
	mStatusCode = statusCode;
}

int MessageResponse::getStatusCode() const {
	return mStatusCode;
}

void MessageResponse::setStatusMessage(const std::string& statusMessage) {
	mStatusMessage = statusMessage;
}

const std::string& MessageResponse::getStatusMessage() const {
	return mStatusMessage;
}

}