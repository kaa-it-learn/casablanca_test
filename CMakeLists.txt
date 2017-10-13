cmake_minimum_required(VERSION 3.7)
project(casablanca_test)

#find_path(CPPREST_INCLUDE cpprest/http_client.h)
#find_library(CPPREST_LIB NAMES cpprest_2_9d cpprest_2_9 cpprestd cpprest)

find_package(Boost REQUIRED COMPONENTS random system thread filesystem chrono atomic date_time regex)
find_package(OpenSSL REQUIRED)

include_directories(. ../cpprestsdk/Release/include)
link_directories(../cpprestsdk/Release/build/Binaries)

add_executable(casablanca_test main.cpp base64.cpp sha1.cpp soap.cpp)

target_link_libraries(casablanca_test
#  ${CPPREST_LIB}
  cpprest
  Boost::boost
  Boost::random
  Boost::system
  Boost::thread
  Boost::filesystem
  Boost::chrono
  Boost::atomic
  Boost::date_time
  Boost::regex
  OpenSSL::SSL
)
