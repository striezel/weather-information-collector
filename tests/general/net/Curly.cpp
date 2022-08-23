/*
 -------------------------------------------------------------------------------
    This file is part of the test suite for weather-information-collector.
    Copyright (C) 2022  Dirk Stolle

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
#include "../../../src/net/Curly.hpp"
#include "../../../src/util/Environment.hpp"

TEST_CASE("Curly")
{
  SECTION("setURL / getURL")
  {
    Curly curly;

    REQUIRE( curly.getURL().empty() );
    curly.setURL("https://example.com/blob");
    REQUIRE( curly.getURL() == "https://example.com/blob" );
  }

  SECTION("addPostField / hasPostField / getPostField / removePostField")
  {
    Curly curly;

    REQUIRE_FALSE( curly.hasPostField("foo") );
    REQUIRE( curly.addPostField("foo", "blubb") );
    REQUIRE( curly.hasPostField("foo") );
    REQUIRE_FALSE( curly.addPostField("", "blubb2") );
    REQUIRE_FALSE( curly.hasPostField("bar") );
    REQUIRE( curly.getPostField("foo") == "blubb" );
    REQUIRE( curly.getPostField("bar") == "" );
    REQUIRE( curly.removePostField("foo") );
    REQUIRE_FALSE( curly.removePostField("bar") );
    REQUIRE_FALSE( curly.hasPostField("foo") );
  }

  SECTION("setPostBody")
  {
    SECTION("fails when post fields are set")
    {
      Curly curly;
      REQUIRE( curly.addPostField("foo", "bar") );

      REQUIRE_FALSE( curly.setPostBody("blobbity blob blob") );
    }

    SECTION("successful")
    {
      Curly curly;
      REQUIRE( curly.setPostBody("blobbity blob blob") );
    }
  }

  SECTION("addHeader / getHeaders")
  {
    SECTION("attempt to add invalid headers")
    {
      Curly curly;

      REQUIRE_FALSE( curly.addHeader("") );
      REQUIRE_FALSE( curly.addHeader("foo") );
      REQUIRE_FALSE( curly.addHeader("foo\nbar") );
      REQUIRE_FALSE( curly.addHeader("foo\rbar") );
      REQUIRE_FALSE( curly.addHeader(": bar") );

      // adding existing headers again
      REQUIRE( curly.addHeader("foo: bar") );
      REQUIRE_FALSE( curly.addHeader("foo: bar") );
    }

    SECTION("add valid headers")
    {
      Curly curly;

      REQUIRE( curly.addHeader("foo: bar") );
      REQUIRE( curly.getHeaders().size() == 1 );
      REQUIRE( curly.getHeaders().at(0) == "foo: bar" );
      REQUIRE( curly.addHeader("content-type: text/html") );
      REQUIRE( curly.getHeaders().size() == 2 );
      REQUIRE( curly.getHeaders().at(0) == "foo: bar" );
      REQUIRE( curly.getHeaders().at(1) == "content-type: text/html" );
      REQUIRE( curly.addHeader("content-length: 42") );
      REQUIRE( curly.getHeaders().size() == 3 );
      REQUIRE( curly.getHeaders().at(0) == "foo: bar" );
      REQUIRE( curly.getHeaders().at(1) == "content-type: text/html" );
      REQUIRE( curly.getHeaders().at(2) == "content-length: 42" );
    }
  }

  SECTION("follow redirects")
  {
    Curly curly;

    REQUIRE_FALSE( curly.followsRedirects() );
    curly.followRedirects(true);
    REQUIRE( curly.followsRedirects() );
    curly.followRedirects(false);
    REQUIRE_FALSE( curly.followsRedirects() );
  }

  SECTION("maximum redirects")
  {
    Curly curly;

    REQUIRE( curly.maximumRedirects() == -1 );
    curly.setMaximumRedirects(5);
    REQUIRE( curly.maximumRedirects() == 5 );
    curly.setMaximumRedirects(1);
    REQUIRE( curly.maximumRedirects() == 1 );
    curly.setMaximumRedirects(-12);
    REQUIRE( curly.maximumRedirects() == -1 );
  }

  if (!wic::hasEnvVar("SKIP_NETWORK_TESTS"))
  {
    SECTION("perform")
    {
      SECTION("URL too short")
      {
        Curly curly;

        curly.setURL("http://a");
        std::string response;
        REQUIRE_FALSE( curly.perform(response) );
      }

      SECTION("request with custom header")
      {
        Curly curly;

        curly.setURL("https://httpbin.org/get");
        REQUIRE( curly.addHeader("foo: barbarbar") );
        std::string response;
        REQUIRE( curly.perform(response) );
        REQUIRE( response.find("barbarbar") != std::string::npos );
      }

      SECTION("request with redirects")
      {
        Curly curly;

        curly.setURL("https://httpbin.org/redirect/2");
        curly.followRedirects(true);
        curly.setMaximumRedirects(5);
        std::string response;
        REQUIRE( curly.perform(response) );
        REQUIRE( curly.getResponseCode() == 200 );

        curly.followRedirects(false);
        curly.setMaximumRedirects(0);
        REQUIRE( curly.perform(response) );
        REQUIRE( curly.getResponseCode() == 302 );
      }
    }
  }
}
