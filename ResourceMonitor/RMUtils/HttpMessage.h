#pragma once

#include <string>
#include <map>

namespace Http {

class Message {
public:
    inline static const std::string STANDARD = "HTTP/1.1";
public:
    virtual const std::string& createStringRepresentation() = 0;
public:
    void addHeader(const std::string& name, const std::string& value);
    const std::string& getHeader(const std::string& name) const;
    void setBody(const std::string& body);
    const std::string& getBody() const;
protected:
    using Headers = std::map<std::string, std::string>;
protected:
    std::string mStringRepresentation;
    Headers mHeaders;
    std::string mBody;
};

class MessageRequest : public Message {
public:
    enum class Method {
        GET,
        PUT,
    };
public:
    MessageRequest();
public:
    const std::string& createStringRepresentation() override;
public:
    void setMethod(const std::string& method);
    Method getMethod() const;
    void setResource(const std::string& resource);
    const std::string& getResource() const;
private:
    Method mMethod;
    std::string mResource;
};

class MessageResponse : public Message {
public:
    MessageResponse();
public:
    const std::string& createStringRepresentation() override;
public:
    void setStatusCode(int statusCode);
    int getStatusCode() const;
    void setStatusMessage(const std::string& statusMessage);
    const std::string& getStatusMessage() const;
private:
    int mStatusCode;
    std::string mStatusMessage;
};

} // namespace Http