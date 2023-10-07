#include <gubg/string/concat.hpp>

#include <cppcodec/base64_rfc4648.hpp>

#define CPPHTTPLIB_OPENSSL_SUPPORT
#include <httplib.h>

#include <catch.hpp>

#include <cstdlib>
#include <string>

TEST_CASE("httplib tests", "[ut][httplib]")
{
    if (const auto api_token = std::getenv("jira_api_token"))
    {
        const std::string email = "geert.fannes@auro-3d.com";

        const std::string credentials = cppcodec::base64_rfc4648::encode(gubg::string::concat(email, ':', api_token));

        httplib::Client client("https://auro-3d.atlassian.net");
        httplib::Headers headers = {
            {"Authorization", gubg::string::concat("Basic", ' ', credentials)},
            {"Content-Type", "application/json"},
        };

        auto res = client.Get("/rest/api/2/issue/CXMOB-50", headers);
        REQUIRE(res);
        REQUIRE(res->status == 200);
        std::cout << res->body << std::endl;
    }
}
