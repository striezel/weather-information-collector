/*
 -------------------------------------------------------------------------------
    This file is part of the test suite for weather-information-collector.
    Copyright (C) 2022, 2025  Dirk Stolle

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 -------------------------------------------------------------------------------
*/

#include "../../find_catch.hpp"
#include "../../../src/net/Request.hpp"
#include "../../../src/util/Environment.hpp"

TEST_CASE("Request")
{
  SECTION("get")
  {
    if (!wic::hasEnvVar("SKIP_NETWORK_TESTS"))
    {
      const std::string httpbin_url = wic::hasEnvVar("USE_LOCAL_HTTPBIN") ? "http://127.0.0.1:8080" : "https://httpbin.org";

      SECTION("successful request")
      {
        const auto opt = Request::get(httpbin_url + "/get", "test::sucessful_request");
        REQUIRE( opt.has_value() );
        REQUIRE_FALSE( opt.value().empty() );
      }

      SECTION("failed request (status 400)")
      {
        const auto opt = Request::get(httpbin_url + "/status/400", "test::failed_request");
        REQUIRE_FALSE( opt.has_value() );
      }

      SECTION("failed request (request fails)")
      {
        const auto opt = Request::get("https://asdfasdf.asdf/asdf", "test::request_fail");
        REQUIRE_FALSE( opt.has_value() );
      }
    }
  }
}
