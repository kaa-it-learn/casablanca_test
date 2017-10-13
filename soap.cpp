#include "soap.h"

#include <ctime>

#include <chrono>
#include <iomanip>
#include <sstream>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/sha1.hpp>
#include <boost/uuid/random_generator.hpp>

#include "base64.h"
#include "sha1.hpp"


namespace IPCamera {
namespace Cameras {


std::string Soap::getContent()
{
    std::ostringstream content;

    content << R"(<?xml version="1.0" encoding="UTF-8"?>)"
            << R"(<s:Envelope xmlns:s="http://www.w3.org/2003/05/soap-envelope")";

    for (const auto& xmln : xmlns)
    {
        content << " " << xmln;
    }

    content << ">";

    if (!user.empty())
    {
        content << "<s:Header>" << createUserToken() << "</s:Header>";
    }

    content << "<s:Body>" << body << "</s:Body>";

    content << "</s:Envelope>";

    return content.str();
}


std::string Soap::createRequest(const std::string& address, const std::string& path)
{
    std::ostringstream content;

    content << R"(<?xml version="1.0" encoding="UTF-8"?>)"
            << R"(<s:Envelope xmlns:s="http://www.w3.org/2003/05/soap-envelope")";

    for (const auto& xmln : xmlns)
    {
        content << " " << xmln;
    }

    content << ">";

    if (!user.empty())
    {
        content << "<s:Header>" << createUserToken() << "</s:Header>";
    }

    content << "<s:Body>" << body << "</s:Body>";

    content << "</s:Envelope>";

    std::ostringstream request;

    request << "POST " << path << " HTTP/1.1\r\n";
    request << "Host: " << address << "\r\n";
    request << "User-Agent: ak-http-client/1.0\r\n";
    request << "Content-Length: " << content.str().size() << "\r\n";
    request << "Authorization: Basic " << basicAuth() << "\r\n";
    request << "Charset: utf-8\r\n";
    request << "Content-Type: application/soap+xml\r\n";
    request << "Accept-Encoding: gzip\r\n";
    request << "\r\n" << content.str();

    return request.str();
}


void Soap::sha_to_bytes(const std::string& sha, unsigned char* bytes)
{
    auto it1 = sha.cbegin();
    auto it2 = it1 + 2;
    auto it3 = sha.cend();

    for (int i = 0; i < 20; i++) {
        std::string m {it1, it2};
        bytes[i] = static_cast<unsigned char>(std::stoi(m, 0, 16));

        if (it2 == it3) {
            break;
        }

        it1 += 2;
        it2 += 2;
    }
}


std::string Soap::createUserToken()
{
    boost::uuids::uuid uuid = boost::uuids::random_generator{}();
    std::string nonce(uuid.begin(), uuid.end());

    std::string nonce64 = base64_encode(uuid.data, uuid.size());

    auto now = std::chrono::system_clock::now();
    std::time_t time = std::chrono::system_clock::to_time_t(now);
    struct tm* ptm = gmtime(&time);
    std::ostringstream timeStream;
    timeStream << std::put_time(ptm, "%Y-%m-%dT%H:%M:%SZ");
    std::string timestamp = timeStream.str();

    std::string token = nonce + timestamp + password;

    SHA1 sha;
    sha.update(token);
    const std::string hash = sha.final();
    unsigned char hashBytes[20] {};
    sha_to_bytes(hash, hashBytes);

    std::string shaDigest64 = base64_encode(hashBytes, 20);

    std::ostringstream stream;

    stream << R"(<Security s:mustUnderstand="1" xmlns="http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd">)";
    stream << "<UsernameToken> <Username>" + user + "</Username>";
    stream << R"(<Password Type="http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-username-token-profile-1.0#PasswordDigest">)" + shaDigest64 + "</Password>";
    stream << R"(<Nonce EncodingType="http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-soap-message-security-1.0#Base64Binary">)" + nonce64 + "</Nonce>";
    stream << R"(<Created xmlns="http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-utility-1.0.xsd">)" + timestamp + "</Created>";
    stream << "</UsernameToken> </Security>";

    return stream.str();
}


std::string Soap::basicAuth()
{
    std::string credentials = user + ":" + password;
    return base64_encode(reinterpret_cast<const unsigned char*>(credentials.data()),
                         credentials.size());
}


} }
