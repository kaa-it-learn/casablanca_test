#ifndef SOAP_H
#define SOAP_H


#include <string>
#include <vector>


namespace IPCamera {
namespace Cameras {


struct Soap
{
    std::string body;
    std::vector<std::string> xmlns;
    std::string user;
    std::string password;

    std::string createRequest(const std::string& address, const std::string& path);
    void sha_to_bytes(const std::string& sha, unsigned char* bytes);
    std::string createUserToken();
    std::string basicAuth();
    std::string getContent();
};


} }


#endif // SOAP_H
