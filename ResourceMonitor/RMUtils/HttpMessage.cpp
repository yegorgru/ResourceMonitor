#include "HttpMessage.h"
#include "Log.h"

#include <boost/algorithm/string.hpp>

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

	const std::string& getStatusLine(StatusCode code) {
		static const std::map<StatusCode, std::string> httpStatusTable =
		{
			{ StatusCode::Ok, "200 OK" },
			{ StatusCode::NotFound, "404 Not Found" },
			{ StatusCode::UnsupportedMediaType, "415 Unsupported Media Type" },
			{ StatusCode::RequestEntityTooLarge, "413 Request Entity Too Large" },
			{ StatusCode::ServerError, "500 Server Error" },
			{ StatusCode::NotImplemented, "501 Not Implemented" },
			{ StatusCode::HttpVersionNotSupported, "505 HTTP Version Not Supported" }
		};
		auto found = httpStatusTable.find(code);
		if (found != httpStatusTable.end()) {
			return found->second;
		}
		LOG::Throw("Status code not found");
		return "";
	}
}

StatusCode intToStatusCode(int value) {
	switch (value) {
	case 200:
		return StatusCode::Ok;
	case 404:
		return StatusCode::NotFound;
	case 413:
		return StatusCode::RequestEntityTooLarge;
	case 415:
		return StatusCode::UnsupportedMediaType;
	case 500:
		return StatusCode::ServerError;
	case 501:
		return StatusCode::NotImplemented;
	case 505:
		return StatusCode::HttpVersionNotSupported;
	}
	throw std::logic_error("Can't convert provided value to StatusCode");
}

void Message::addHeader(const std::string& name, const std::string& value) {
	mHeaders[boost::algorithm::to_lower_copy(name)] = value;
}

const std::string& Message::getHeader(const std::string& name) const {
	auto found = mHeaders.find(boost::algorithm::to_lower_copy(name));
	if (found != mHeaders.end()) {
		return found->second;
	}
	LOG::Throw("Header not found");
	return "";
}

bool Message::hasHeader(const std::string& name) const {
	auto found = mHeaders.find(boost::algorithm::to_lower_copy(name));
	return found != mHeaders.end();
}

void Message::appendBody(const std::string& body) {
	mBody += body;
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
	: mStatusCode(StatusCode::Ok)
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

void MessageResponse::setStatusCode(StatusCode statusCode) {
	mStatusCode = statusCode;
}

StatusCode MessageResponse::getStatusCode() const {
	return mStatusCode;
}

void MessageResponse::setStatusMessage(const std::string& statusMessage) {
	mStatusMessage = statusMessage;
}

const std::string& MessageResponse::getStatusMessage() const {
	return mStatusMessage;
}

}