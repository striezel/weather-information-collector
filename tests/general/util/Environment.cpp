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
#include <cstdlib>
#include "../../../src/util/Environment.hpp"

TEST_CASE("environment-related functions")
{
  using namespace wic;

  SECTION("hasEnvVar")
  {
    SECTION("missing environment variable")
    {
      REQUIRE_FALSE( hasEnvVar("THIS_IS_VERY_UNLIKELY_TO_BE_SET") );
    }

    #if !defined(_WIN32)
    SECTION("existing environment variable")
    {
      // set a new variable
      REQUIRE( setenv("THIS_IS_JUST_FOR_TESTS", "yes", 0) == 0 );

      // check variable
      REQUIRE( hasEnvVar("THIS_IS_JUST_FOR_TESTS") );

      // delete variable from environment
      REQUIRE( unsetenv("THIS_IS_JUST_FOR_TESTS") == 0 );
    }
    #endif
  }

  SECTION("isMinGW")
  {
    const bool shouldBeMinGW =
    #if defined(__MINGW32__) || defined(__MINGW64__)
    true
    #else
    false
    #endif
    ;
    REQUIRE( isMinGW() == shouldBeMinGW );
  }

  SECTION("isGitlabCi")
  {
    const bool shouldBeGitLab = hasEnvVar("GITLAB_CI");
    REQUIRE( isGitlabCi() == shouldBeGitLab );
  }

  SECTION("isGithubActions")
  {
    const bool shouldBeGitHub = hasEnvVar("GITHUB_ACTIONS");
    REQUIRE( isGithubActions() == shouldBeGitHub );
  }
}
