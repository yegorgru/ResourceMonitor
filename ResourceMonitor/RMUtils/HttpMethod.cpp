#include "pch.h"
#include "HttpMethod.h"

#include <stdexcept>

namespace Http {

	std::string methodToStr(Method method) {
		switch (method) {
		case Method::GET:
			return "GET";
		case Method::PUT:
			return "PUT";
		}
		throw std::runtime_error("Method is not implemented");
	}

	Method strToMethod(const std::string& str) {
		if (str == "GET") {
			return Method::GET;
		}
		else if (str == "PUT") {
			return Method::PUT;
		}
		throw std::runtime_error("Method is not implemented");
	}

}