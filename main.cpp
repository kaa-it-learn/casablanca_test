#include <cpprest/http_client.h>

#include <vector>
#include <thread>
#include <chrono>

#include <soap.h>

//using namespace cpprest;

std::vector<std::string> onvifXMLNs = {
    R"(xmlns:tds="http://www.onvif.org/ver10/device/wsdl")",
    R"(xmlns:tt="http://www.onvif.org/ver10/schema")",
    R"(xmlns:trt="http://www.onvif.org/ver10/media/wsdl")",
    R"(xmlns:tt="http://www.onvif.org/ver10/schema")"
};

int main()
{
    IPCamera::Cameras::Soap soap {
        "<trt:GetProfiles/>",
        onvifXMLNs,
        "admin",
        "9999"
    };

    //web::http::client::http_client_config config;
    //config.set_credentials({U("admin"), U("9999")});
    web::http::client::http_client client { U("http://192.168.11.88")/*, config*/ };
    web::http::http_request req(web::http::methods::POST);
    req.set_request_uri(U("onvif/device_service"));
    req.headers().add(web::http::header_names::authorization, soap.basicAuth()); // 'Basic' word is not understand by camera
    req.set_body(U(soap.getContent()));

    pplx::cancellation_token_source cts;
    auto token = cts.get_token();

    auto task = client.request(req, token).then([](pplx::task<web::http::http_response> task) {

        try {
            auto response = task.get();

            if (response.status_code() == web::http::status_codes::OK)
            {
                //response.headers().set_content_type(U("plain/text"));
                auto body = response.extract_string(true).get();
                std::cout << body << std::endl;
            }
        } catch (std::exception &e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    });

    std::this_thread::sleep_for(std::chrono::seconds{100});

    //cts.cancel();

    /*auto response = task.get();

    if (response.status_code() == web::http::status_codes::OK)
    {
        //response.headers().set_content_type(U("plain/text"));
        auto body = response.extract_string(true).get();
        std::cout << body << std::endl;
    }*/
}
