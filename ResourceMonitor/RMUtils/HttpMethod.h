#pragma once

#include <string>

namespace Http {

enum class Method {
	GET,
	PUT,
};

std::string methodToStr(Method method);
Method strToMethod(const std::string& str);


} // namespace Http