/*
 -------------------------------------------------------------------------------
    This file is part of the test suite for weather-information-collector.
    Copyright (C) 2017, 2022, 2023, 2025  Dirk Stolle

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
#include <algorithm>
#include <filesystem>
#include <fstream>
#include "../../../src/conf/Configuration.hpp"

// utility function to write a configuration file for testing
bool writeConfiguration(const std::filesystem::path& path, const std::string& content)
{
  std::ofstream stream(path, std::ios::out | std::ios::binary);
  if (!stream.good())
    return false;
  if (!stream.write(content.c_str(), content.size()).good())
    return false;
  stream.close();
  return stream.good();
}

// guard to ensure file deletion when it goes out of scope
class FileGuard
{
  private:
    std::filesystem::path path;
  public:
    FileGuard(const std::filesystem::path& filePath)
    : path(filePath)
    { }

    FileGuard(const FileGuard& op) = delete;
    FileGuard(FileGuard&& op) = delete;

    ~FileGuard()
    {
      std::filesystem::remove(path);
    }
};

// guard to ensure directory deletion when it goes out of scope
class DirectoryGuard
{
  private:
    std::filesystem::path path;
  public:
    DirectoryGuard(const std::filesystem::path& filePath)
    : path(filePath)
    { }

    DirectoryGuard(const DirectoryGuard& op) = delete;
    DirectoryGuard(DirectoryGuard&& op) = delete;

    ~DirectoryGuard()
    {
      std::filesystem::remove_all(path);
    }
};

TEST_CASE("Class Configuration")
{
  using namespace wic;

  SECTION("constructor")
  {
    Configuration c;

    REQUIRE_FALSE( c.connectionInfo().isComplete() );
    REQUIRE( c.taskDirectory().empty() );
    REQUIRE( c.taskExtension().empty() );
    REQUIRE( c.tasks().empty() );
  }

  SECTION("potentialFileNames")
  {
    const auto pf = Configuration::potentialFileNames();

    REQUIRE_FALSE( pf.empty() );
    REQUIRE( pf.size() > 1 );
    for (const auto & fn : pf)
    {
      REQUIRE( fn.find(".conf") != std::string::npos );
    }
  }

  SECTION("potentialTaskDirectories")
  {
    const auto td = Configuration::potentialTaskDirectories();

    REQUIRE_FALSE( td.empty() );
    REQUIRE( td.size() > 1 );
    for (const auto & dn : td)
    {
      #if defined(_WIN32)
      const bool hasSlash = dn.find("/") != std::string::npos;
      const bool hasBackslash = dn.find("\\") != std::string::npos;
      const bool hasSomeSeparator = hasSlash || hasBackslash;
      REQUIRE( hasSomeSeparator );
      #else
      REQUIRE( dn.find("/") != std::string::npos );
      #endif
    }
  }

  SECTION("load")
  {
    SECTION("configuration file does not exist")
    {
      Configuration conf;
      REQUIRE_FALSE( conf.load("/this/does/not-exist.conf") );
    }

    SECTION("load example configuration file, but skip tasks")
    {
      const std::filesystem::path path{"example-skipping-tasks.conf"};
      const std::string content = R"conf(
      # This line is a comment and will be ignored by the program.
      #And so is this line.

      # database settings
      db.host=localhost
      db.name=weather_information_collector
      db.user=i_am_the_user
      db.password=secret(!) password
      db.port=3308
      tasks.directory=/home/user/.wic/task.d
      tasks.extension=.task
      # API keys
      key.owm=0123456789abcdefdeadbeef1c0ffee1
      key.apixu=abcdef9876543210affe1affe2affe
      key.darksky=f00ba12735743210fedcba9876543210
      key.weatherbit=fedcba98765432100123456789abcdef
      key.weatherstack=f00ba12abcdef1234567890abcdef123
      # plans
      plan.owm=developer
      plan.weatherbit=standard
      plan.weatherstack=free
      )conf";
      REQUIRE( writeConfiguration(path, content) );
      FileGuard guard{path};

      Configuration conf;
      REQUIRE( conf.load(path.string(), true) );

      REQUIRE( conf.connectionInfo().hostname() == "localhost" );
      REQUIRE( conf.connectionInfo().db() == "weather_information_collector" );
      REQUIRE( conf.connectionInfo().user() == "i_am_the_user" );
      REQUIRE( conf.connectionInfo().password() == "secret(!) password" );
      REQUIRE( conf.connectionInfo().port() == 3308 );
      REQUIRE( conf.taskDirectory() == "/home/user/.wic/task.d" );
      REQUIRE( conf.taskExtension() == ".task" );
      REQUIRE( conf.key(ApiType::OpenWeatherMap) == "0123456789abcdefdeadbeef1c0ffee1" );
      REQUIRE( conf.key(ApiType::Apixu) == "abcdef9876543210affe1affe2affe" );
      REQUIRE( conf.key(ApiType::DarkSky) == "f00ba12735743210fedcba9876543210" );
      REQUIRE( conf.key(ApiType::Weatherbit) == "fedcba98765432100123456789abcdef" );
      REQUIRE( conf.key(ApiType::Weatherstack) == "f00ba12abcdef1234567890abcdef123" );
      REQUIRE( conf.planOpenWeatherMap() == PlanOwm::Developer );
      REQUIRE( conf.planWeatherbit() == PlanWeatherbit::Standard );
      REQUIRE( conf.planWeatherstack() == PlanWeatherstack::Free );
    }

    SECTION("missing port automatically becomes default MariaDB port")
    {
      const std::filesystem::path path{"missing-db-port.conf"};
      const std::string content = R"conf(
      # database settings
      db.host=horst.local
      db.name=weather_data
      db.user=the_user
      db.password=p4ssw0rd!
      tasks.directory=/home/user/.wic/task.d
      tasks.extension=.task
      # API keys
      key.owm=9876543210abcdefdeadbeef1c0ffee1
      # plans
      plan.owm=developer
      plan.weatherbit=standard
      plan.weatherstack=standard
      )conf";
      REQUIRE( writeConfiguration(path, content) );
      FileGuard guard{path};

      Configuration conf;
      REQUIRE( conf.load(path.string(), true) );

      REQUIRE( conf.connectionInfo().hostname() == "horst.local" );
      REQUIRE( conf.connectionInfo().db() == "weather_data" );
      REQUIRE( conf.connectionInfo().user() == "the_user" );
      REQUIRE( conf.connectionInfo().password() == "p4ssw0rd!" );
      REQUIRE( conf.connectionInfo().port() == 3306 );
      REQUIRE( conf.taskDirectory() == "/home/user/.wic/task.d" );
      REQUIRE( conf.taskExtension() == ".task" );
      REQUIRE( conf.key(ApiType::OpenWeatherMap) == "9876543210abcdefdeadbeef1c0ffee1" );
      REQUIRE( conf.key(ApiType::Apixu).empty() );
      REQUIRE( conf.key(ApiType::DarkSky).empty() );
      REQUIRE( conf.key(ApiType::Weatherbit).empty() );
      REQUIRE( conf.key(ApiType::Weatherstack).empty() );
      REQUIRE( conf.planOpenWeatherMap() == PlanOwm::Developer );
      REQUIRE( conf.planWeatherbit() == PlanWeatherbit::Standard );
      REQUIRE( conf.planWeatherstack() == PlanWeatherstack::Standard );
    }

    SECTION("missing plans automatically become free plans")
    {
      const std::filesystem::path path{"missing-plans-become-free-plans.conf"};
      const std::string content = R"conf(
      # database settings
      db.host=localhost
      db.name=my_database
      db.user=user
      db.password=secret(!) password
      db.port=3309
      tasks.directory=/home/user/.wic/task.d
      tasks.extension=.task
      # API keys
      key.owm=1234567890abcdef
      # no plans specified
      )conf";
      REQUIRE( writeConfiguration(path, content) );
      FileGuard guard{path};

      Configuration conf;
      REQUIRE( conf.load(path.string(), true) );

      REQUIRE( conf.connectionInfo().hostname() == "localhost" );
      REQUIRE( conf.connectionInfo().db() == "my_database" );
      REQUIRE( conf.connectionInfo().user() == "user" );
      REQUIRE( conf.connectionInfo().password() == "secret(!) password" );
      REQUIRE( conf.connectionInfo().port() == 3309 );
      REQUIRE( conf.taskDirectory() == "/home/user/.wic/task.d" );
      REQUIRE( conf.taskExtension() == ".task" );
      REQUIRE( conf.key(ApiType::OpenWeatherMap) == "1234567890abcdef" );
      REQUIRE( conf.key(ApiType::Apixu).empty() );
      REQUIRE( conf.key(ApiType::DarkSky).empty() );
      REQUIRE( conf.key(ApiType::Weatherbit).empty() );
      REQUIRE( conf.key(ApiType::Weatherstack).empty() );
      REQUIRE( conf.planOpenWeatherMap() == PlanOwm::Free );
      REQUIRE( conf.planWeatherbit() == PlanWeatherbit::Free );
      REQUIRE( conf.planWeatherstack() == PlanWeatherstack::Free );
    }

    SECTION("missing keys when keys are required")
    {
      const std::filesystem::path path{"missing-keys.conf"};
      const std::string content = R"conf(
      # database settings
      db.host=horst.local
      db.name=weather_data
      db.user=the_user
      db.password=p4ssw0rd!
      tasks.directory=missing_keys_dir
      tasks.extension=.task
      # no API keys
      # plans
      plan.owm=developer
      plan.weatherbit=standard
      plan.weatherstack=standard
      )conf";
      REQUIRE( writeConfiguration(path, content) );
      FileGuard guard{path};

      const std::filesystem::path tasks{"missing_keys_dir"};
      REQUIRE( std::filesystem::create_directory(tasks) );
      DirectoryGuard dirGuard{tasks};
      const std::string task_content = R"conf(
      api=OpenWeatherMap
      data=current
      location.name=London
      location.countrycode=GB
      location.coordinates=51.507301,-0.1277
      location.id=2643743
      interval=3600
      )conf";
      REQUIRE( writeConfiguration(tasks / "london.task", task_content) );

      Configuration conf;
      REQUIRE_FALSE( conf.load(path.string(), false) );
    }

    SECTION("missing keys when missing keys are allowed")
    {
      const std::filesystem::path path{"missing-keys-allowed.conf"};
      const std::string content = R"conf(
      # database settings
      db.host=lost.local
      db.name=weather_db
      db.user=one_user
      db.password=p4ssw0rd!
      tasks.directory=missing-keys-allowed
      tasks.extension=.task
      # no API keys
      # plans
      plan.owm=developer
      plan.weatherbit=standard
      plan.weatherstack=standard
      )conf";
      REQUIRE( writeConfiguration(path, content) );
      FileGuard guard{path};

      const std::filesystem::path tasks{"missing-keys-allowed"};
      REQUIRE( std::filesystem::create_directory(tasks) );
      DirectoryGuard dirGuard{tasks};
      const std::string task_content = R"conf(
      api=OpenWeatherMap
      data=current
      location.name=London
      location.countrycode=GB
      location.coordinates=51.507301,-0.1277
      location.id=2643743
      interval=3600
      )conf";
      REQUIRE( writeConfiguration(tasks / "london.task", task_content) );

      Configuration conf;
      REQUIRE( conf.load(path.string(), true, true) );

      REQUIRE( conf.connectionInfo().hostname() == "lost.local" );
      REQUIRE( conf.connectionInfo().db() == "weather_db" );
      REQUIRE( conf.connectionInfo().user() == "one_user" );
      REQUIRE( conf.connectionInfo().password() == "p4ssw0rd!" );
      REQUIRE( conf.connectionInfo().port() == 3306 );
      REQUIRE( conf.taskDirectory() == "missing-keys-allowed" );
      REQUIRE( conf.taskExtension() == ".task" );
      REQUIRE( conf.key(ApiType::OpenWeatherMap).empty() );
      REQUIRE( conf.key(ApiType::Apixu).empty() );
      REQUIRE( conf.key(ApiType::DarkSky).empty() );
      REQUIRE( conf.key(ApiType::Weatherbit).empty() );
      REQUIRE( conf.key(ApiType::Weatherstack).empty() );
      REQUIRE( conf.planOpenWeatherMap() == PlanOwm::Developer );
      REQUIRE( conf.planWeatherbit() == PlanWeatherbit::Standard );
      REQUIRE( conf.planWeatherstack() == PlanWeatherstack::Standard );
    }

    SECTION("configuration with empty task extension (i. e. match all extensions)")
    {
      const std::filesystem::path path{"empty-task-extension.conf"};
      const std::string content = R"conf(
      # database settings
      db.host=localhost
      db.name=my_database
      db.user=user
      db.password=secret(!) password
      db.port=3310
      tasks.directory=/home/user/.wic/task.d
      tasks.extension=
      # API keys
      key.owm=1234567890abcdef
      # no plans specified
      )conf";
      REQUIRE( writeConfiguration(path, content) );
      FileGuard guard{path};

      Configuration conf;
      REQUIRE( conf.load(path.string(), true) );

      REQUIRE( conf.connectionInfo().hostname() == "localhost" );
      REQUIRE( conf.connectionInfo().db() == "my_database" );
      REQUIRE( conf.connectionInfo().user() == "user" );
      REQUIRE( conf.connectionInfo().password() == "secret(!) password" );
      REQUIRE( conf.connectionInfo().port() == 3310 );
      REQUIRE( conf.taskDirectory() == "/home/user/.wic/task.d" );
      REQUIRE( conf.taskExtension().empty() );
      REQUIRE( conf.key(ApiType::OpenWeatherMap) == "1234567890abcdef" );
      REQUIRE( conf.key(ApiType::Apixu).empty() );
      REQUIRE( conf.key(ApiType::DarkSky).empty() );
      REQUIRE( conf.key(ApiType::Weatherbit).empty() );
      REQUIRE( conf.key(ApiType::Weatherstack).empty() );
      REQUIRE( conf.planOpenWeatherMap() == PlanOwm::Free );
      REQUIRE( conf.planWeatherbit() == PlanWeatherbit::Free );
      REQUIRE( conf.planWeatherstack() == PlanWeatherstack::Free );
    }

    SECTION("invalid setting: empty value")
    {
      const std::filesystem::path path{"empty-setting-value.conf"};
      const std::string content = R"conf(
      # database settings
      db.host=localhost
      db.name=my_database
      db.user=user
      db.password=
      db.port=3310
      tasks.directory=/home/user/.wic/task.d
      tasks.extension=.task
      # API keys
      key.owm=1234567890abcdef
      # no plans specified
      )conf";
      REQUIRE( writeConfiguration(path, content) );
      FileGuard guard{path};

      Configuration conf;
      REQUIRE_FALSE( conf.load(path.string(), true) );
    }

    SECTION("invalid: incomplete database information")
    {
      const std::filesystem::path path{"incomplete-database-settings.conf"};
      const std::string content = R"conf(
      # database settings
      db.host=localhost
      db.name=my_database
      db.port=3310
      tasks.directory=/home/user/.wic/task.d
      tasks.extension=.task
      # API keys
      key.owm=1234567890abcdef
      # no plans specified
      )conf";
      REQUIRE( writeConfiguration(path, content) );
      FileGuard guard{path};

      Configuration conf;
      REQUIRE_FALSE( conf.load(path.string(), true) );
    }

    SECTION("port invalid: not an integer")
    {
      const std::filesystem::path path{"invalid-port-no-integer.conf"};
      const std::string content = R"conf(
      # database settings
      db.host=localhost
      db.name=my_database
      db.user=user
      db.password=secret(!) password
      db.port=not_an_int
      tasks.directory=/home/user/.wic/task.d
      tasks.extension=.task
      # API keys
      key.owm=1234567890abcdef
      )conf";
      REQUIRE( writeConfiguration(path, content) );
      FileGuard guard{path};

      Configuration conf;
      REQUIRE_FALSE( conf.load(path.string(), true) );
    }

    SECTION("port invalid: zero is not an acceptable port number")
    {
      const std::filesystem::path path{"invalid-port-zero.conf"};
      const std::string content = R"conf(
      # database settings
      db.host=localhost
      db.name=my_database
      db.user=user
      db.password=secret(!) password
      db.port=0
      tasks.directory=/home/user/.wic/task.d
      tasks.extension=.task
      # API keys
      key.owm=1234567890abcdef
      )conf";
      REQUIRE( writeConfiguration(path, content) );
      FileGuard guard{path};

      Configuration conf;
      REQUIRE_FALSE( conf.load(path.string(), true) );
    }

    SECTION("port invalid: port number out of range")
    {
      const std::filesystem::path path{"invalid-port-out-of-range.conf"};
      const std::string content = R"conf(
      # database settings
      db.host=localhost
      db.name=my_database
      db.user=user
      db.password=secret(!) password
      db.port=70123
      tasks.directory=/home/user/.wic/task.d
      tasks.extension=.task
      # API keys
      key.owm=1234567890abcdef
      )conf";
      REQUIRE( writeConfiguration(path, content) );
      FileGuard guard{path};

      Configuration conf;
      REQUIRE_FALSE( conf.load(path.string(), true) );
    }

    SECTION("port invalid: port was specified more than once")
    {
      const std::filesystem::path path{"port-specified-twice.conf"};
      const std::string content = R"conf(
      # database settings
      db.host=localhost
      db.name=my_database
      db.user=user
      db.password=secret(!) password
      db.port=3306
      db.port=3307
      tasks.directory=/home/user/.wic/task.d
      tasks.extension=.task
      # API keys
      key.owm=1234567890abcdef
      )conf";
      REQUIRE( writeConfiguration(path, content) );
      FileGuard guard{path};

      Configuration conf;
      REQUIRE_FALSE( conf.load(path.string(), true) );
    }

    SECTION("host invalid: host was specified more than once")
    {
      const std::filesystem::path path{"twice-the-host-twice-the-fun.conf"};
      const std::string content = R"conf(
      # database settings
      db.host=localhost
      db.host=the.host.local
      db.name=my_database
      db.user=user
      db.password=secret(!) password
      db.port=3306
      tasks.directory=/home/user/.wic/task.d
      tasks.extension=.task
      # API keys
      key.owm=1234567890abcdef
      )conf";
      REQUIRE( writeConfiguration(path, content) );
      FileGuard guard{path};

      Configuration conf;
      REQUIRE_FALSE( conf.load(path.string(), true) );
    }

    SECTION("DB name invalid: name was specified more than once")
    {
      const std::filesystem::path path{"twice-the-db-name-twice-the-fun.conf"};
      const std::string content = R"conf(
      # database settings
      db.host=the.host.local
      db.name=maria_maria
      db.name=my_database
      db.user=user
      db.password=secret(!) password
      db.port=3306
      tasks.directory=/home/user/.wic/task.d
      tasks.extension=.task
      # API keys
      key.owm=1234567890abcdef
      )conf";
      REQUIRE( writeConfiguration(path, content) );
      FileGuard guard{path};

      Configuration conf;
      REQUIRE_FALSE( conf.load(path.string(), true) );
    }

    SECTION("DB user invalid: user was specified more than once")
    {
      const std::filesystem::path path{"twice-the-db-user-twice-the-fun.conf"};
      const std::string content = R"conf(
      # database settings
      db.host=the.host.local
      db.name=my_database
      db.user=user
      db.user=user2
      db.password=secret(!) password
      db.port=3306
      tasks.directory=/home/user/.wic/task.d
      tasks.extension=.task
      # API keys
      key.owm=1234567890abcdef
      )conf";
      REQUIRE( writeConfiguration(path, content) );
      FileGuard guard{path};

      Configuration conf;
      REQUIRE_FALSE( conf.load(path.string(), true) );
    }

    SECTION("DB password invalid: password was specified more than once")
    {
      const std::filesystem::path path{"twice-the-db-password-twice-the-fun.conf"};
      const std::string content = R"conf(
      # database settings
      db.host=the.host.local
      db.name=my_database
      db.user=user
      db.password=secret(!) password
      db.password=hmm... this will fail
      db.port=3306
      tasks.directory=/home/user/.wic/task.d
      tasks.extension=.task
      # API keys
      key.owm=1234567890abcdef
      )conf";
      REQUIRE( writeConfiguration(path, content) );
      FileGuard guard{path};

      Configuration conf;
      REQUIRE_FALSE( conf.load(path.string(), true) );
    }

    SECTION("task directory invalid: directory was specified more than once")
    {
      const std::filesystem::path path{"twice-the-task-directory-twice-the-fun.conf"};
      const std::string content = R"conf(
      # database settings
      db.host=the.host.local
      db.name=my_database
      db.user=user
      db.password=secret(!) password
      db.port=3306
      tasks.directory=/home/user/.wic/task.d
      tasks.directory=/home/fail/here/task.d
      tasks.extension=.task
      # API keys
      key.owm=1234567890abcdef
      )conf";
      REQUIRE( writeConfiguration(path, content) );
      FileGuard guard{path};

      Configuration conf;
      REQUIRE_FALSE( conf.load(path.string(), true) );
    }

    SECTION("task extension invalid: extension was specified more than once")
    {
      const std::filesystem::path path{"twice-the-task-extension-twice-the-fun.conf"};
      const std::string content = R"conf(
      # database settings
      db.host=the.host.local
      db.name=my_database
      db.user=user
      db.password=secret(!) password
      db.port=3306
      tasks.directory=/home/user/.wic/task.d
      tasks.extension=.task
      tasks.extension=.fail
      # API keys
      key.owm=1234567890abcdef
      )conf";
      REQUIRE( writeConfiguration(path, content) );
      FileGuard guard{path};

      Configuration conf;
      REQUIRE_FALSE( conf.load(path.string(), true) );
    }

    SECTION("key invalid: Apixu key was specified more than once")
    {
      const std::filesystem::path path{"twice-the-apixu-key-twice-the-fun.conf"};
      const std::string content = R"conf(
      # database settings
      db.host=the.host.local
      db.name=my_database
      db.user=user
      db.password=secret(!) password
      db.port=3306
      tasks.directory=/home/user/.wic/task.d
      tasks.extension=.task
      # API keys
      key.apixu=1234567890abcdef
      key.apixu=1234567890abfail
      )conf";
      REQUIRE( writeConfiguration(path, content) );
      FileGuard guard{path};

      Configuration conf;
      REQUIRE_FALSE( conf.load(path.string(), true) );
    }

    SECTION("key invalid: OpenWeatherMap key was specified more than once")
    {
      const std::filesystem::path path{"twice-the-owm-key-twice-the-fun.conf"};
      const std::string content = R"conf(
      # database settings
      db.host=the.host.local
      db.name=my_database
      db.user=user
      db.password=secret(!) password
      db.port=3306
      tasks.directory=/home/user/.wic/task.d
      tasks.extension=.task
      # API keys
      key.owm=1234567890abcdef
      key.owm=1234567890abfail
      )conf";
      REQUIRE( writeConfiguration(path, content) );
      FileGuard guard{path};

      Configuration conf;
      REQUIRE_FALSE( conf.load(path.string(), true) );
    }

    SECTION("key invalid: DarkSky key was specified more than once")
    {
      const std::filesystem::path path{"twice-the-darksky-key-twice-the-fun.conf"};
      const std::string content = R"conf(
      # database settings
      db.host=the.host.local
      db.name=my_database
      db.user=user
      db.password=secret(!) password
      db.port=3306
      tasks.directory=/home/user/.wic/task.d
      tasks.extension=.task
      # API keys
      key.darksky=1234567890abcdef
      key.darksky=1234567890abfail
      )conf";
      REQUIRE( writeConfiguration(path, content) );
      FileGuard guard{path};

      Configuration conf;
      REQUIRE_FALSE( conf.load(path.string(), true) );
    }

    SECTION("key invalid: Weatherbit key was specified more than once")
    {
      const std::filesystem::path path{"twice-the-weatherbit-key-twice-the-fun.conf"};
      const std::string content = R"conf(
      # database settings
      db.host=the.host.local
      db.name=my_database
      db.user=user
      db.password=secret(!) password
      db.port=3306
      tasks.directory=/home/user/.wic/task.d
      tasks.extension=.task
      # API keys
      key.weatherbit=1234567890abcdef
      key.weatherbit=1234567890abfail
      )conf";
      REQUIRE( writeConfiguration(path, content) );
      FileGuard guard{path};

      Configuration conf;
      REQUIRE_FALSE( conf.load(path.string(), true) );
    }

    SECTION("key invalid: Weatherstack key was specified more than once")
    {
      const std::filesystem::path path{"twice-the-Weatherstack-key-twice-the-fun.conf"};
      const std::string content = R"conf(
      # database settings
      db.host=the.host.local
      db.name=my_database
      db.user=user
      db.password=secret(!) password
      db.port=3306
      tasks.directory=/home/user/.wic/task.d
      tasks.extension=.task
      # API keys
      key.weatherstack=1234567890abcdef
      key.weatherstack=1234567890abfail
      )conf";
      REQUIRE( writeConfiguration(path, content) );
      FileGuard guard{path};

      Configuration conf;
      REQUIRE_FALSE( conf.load(path.string(), true) );
    }

    SECTION("plan invalid: OpenWeatherMap plan was specified more than once")
    {
      const std::filesystem::path path{"twice-the-owm-plan-twice-the-fun.conf"};
      const std::string content = R"conf(
      # database settings
      db.host=the.host.local
      db.name=my_database
      db.user=user
      db.password=secret(!) password
      db.port=3306
      tasks.directory=/home/user/.wic/task.d
      tasks.extension=.task
      # API keys
      key.owm=1234567890abcdef
      # plans
      plan.owm=developer
      plan.owm=free
      )conf";
      REQUIRE( writeConfiguration(path, content) );
      FileGuard guard{path};

      Configuration conf;
      REQUIRE_FALSE( conf.load(path.string(), true) );
    }

    SECTION("plan invalid: OpenWeatherMap plan value is not recognized")
    {
      const std::filesystem::path path{"unrecognized-owm-plan.conf"};
      const std::string content = R"conf(
      # database settings
      db.host=the.host.local
      db.name=my_database
      db.user=user
      db.password=secret(!) password
      db.port=3306
      tasks.directory=/home/user/.wic/task.d
      tasks.extension=.task
      # API keys
      key.owm=1234567890abcdef
      # plans
      plan.owm=this_should_be_another_value
      )conf";
      REQUIRE( writeConfiguration(path, content) );
      FileGuard guard{path};

      Configuration conf;
      REQUIRE_FALSE( conf.load(path.string(), true) );
    }

    SECTION("plan invalid: Weatherbit plan was specified more than once")
    {
      const std::filesystem::path path{"twice-the-weatherbit-plan-twice-the-fun.conf"};
      const std::string content = R"conf(
      # database settings
      db.host=the.host.local
      db.name=my_database
      db.user=user
      db.password=secret(!) password
      db.port=3306
      tasks.directory=/home/user/.wic/task.d
      tasks.extension=.task
      # API keys
      key.owm=1234567890abcdef
      key.weatherbit=foo
      # plans
      plan.weatherbit=standard
      plan.weatherbit=standard
      )conf";
      REQUIRE( writeConfiguration(path, content) );
      FileGuard guard{path};

      Configuration conf;
      REQUIRE_FALSE( conf.load(path.string(), true) );
    }

    SECTION("plan invalid: Weatherbit plan value is not recognized")
    {
      const std::filesystem::path path{"unrecognized-weatherbit-plan.conf"};
      const std::string content = R"conf(
      # database settings
      db.host=the.host.local
      db.name=my_database
      db.user=user
      db.password=secret(!) password
      db.port=3306
      tasks.directory=/home/user/.wic/task.d
      tasks.extension=.task
      # API keys
      key.weatherbit=1234567890abcdef
      key.owm=1234567890abcdef
      # plans
      plan.weatherbit=no_this_is_not_correct
      )conf";
      REQUIRE( writeConfiguration(path, content) );
      FileGuard guard{path};

      Configuration conf;
      REQUIRE_FALSE( conf.load(path.string(), true) );
    }

    SECTION("plan invalid: Weatherstack plan was specified more than once")
    {
      const std::filesystem::path path{"twice-the-weatherstack-plan-twice-the-fun.conf"};
      const std::string content = R"conf(
      # database settings
      db.host=the.host.local
      db.name=my_database
      db.user=user
      db.password=secret(!) password
      db.port=3306
      tasks.directory=/home/user/.wic/task.d
      tasks.extension=.task
      # API keys
      key.weatherstack=1234567890abcdef
      key.owm=1234567890abcdef
      # plans
      plan.weatherstack=professional
      plan.weatherstack=professional
      )conf";
      REQUIRE( writeConfiguration(path, content) );
      FileGuard guard{path};

      Configuration conf;
      REQUIRE_FALSE( conf.load(path.string(), true) );
    }

    SECTION("plan invalid: Weatherstack plan value is not recognized")
    {
      const std::filesystem::path path{"unrecognized-weatherstack-plan.conf"};
      const std::string content = R"conf(
      # database settings
      db.host=the.host.local
      db.name=my_database
      db.user=user
      db.password=secret(!) password
      db.port=3306
      tasks.directory=/home/user/.wic/task.d
      tasks.extension=.task
      # API keys
      key.weatherstack=1234567890abcdef
      key.owm=1234567890abcdef
      # plans
      plan.weatherstack=what_is_this
      )conf";
      REQUIRE( writeConfiguration(path, content) );
      FileGuard guard{path};

      Configuration conf;
      REQUIRE_FALSE( conf.load(path.string(), true) );
    }

    SECTION("setting invalid: unrecognized setting name")
    {
      const std::filesystem::path path{"unrecognized-setting-name.conf"};
      const std::string content = R"conf(
      # database settings
      db.host=the.host.local
      db.name=my_database
      db.user=user
      db.password=secret(!) password
      db.port=3306
      tasks.directory=/home/user/.wic/task.d
      tasks.extension=.task
      # API keys
      key.owm=1234567890abcdef
      # plans
      plan.owm=developer
      thisisunrecognized=fails_here
      )conf";
      REQUIRE( writeConfiguration(path, content) );
      FileGuard guard{path};

      Configuration conf;
      REQUIRE_FALSE( conf.load(path.string(), true) );
    }

    SECTION("setting invalid: missing separator between name and value")
    {
      const std::filesystem::path path{"missing-separator-btween-setting-name-and-value.conf"};
      const std::string content = R"conf(
      # database settings
      db.host=the.host.local
      db.name_no_separator_fails_here
      db.user=user
      db.password=secret(!) password
      db.port=3306
      tasks.directory=/home/user/.wic/task.d
      tasks.extension=.task
      # API keys
      key.owm=1234567890abcdef
      # plans
      plan.owm=developer
      )conf";
      REQUIRE( writeConfiguration(path, content) );
      FileGuard guard{path};

      Configuration conf;
      REQUIRE_FALSE( conf.load(path.string(), true) );
    }

    SECTION("load example configuration file, including tasks")
    {
      const std::filesystem::path path{"example-loading-tasks.conf"};
      const std::string content = R"conf(
      # This line is a comment and will be ignored by the program.
      #And so is this line.

      # database settings
      db.host=db.domain.local
      db.name=weather_db
      db.user=user
      db.password=secret(!) password
      db.port=3366
      tasks.directory=example-loading-tasks-dir
      tasks.extension=.task
      # API keys
      key.owm=0123456789abcdefdeadbeef1c0ffee1
      key.apixu=abcdef9876543210affe1affe2affe
      key.darksky=f00ba12735743210fedcba9876543210
      key.weatherbit=fedcba98765432100123456789abcdef
      key.weatherstack=f00ba12abcdef1234567890abcdef123
      # plans
      plan.owm=enterprise
      plan.weatherbit=business
      plan.weatherstack=business
      )conf";
      REQUIRE( writeConfiguration(path, content) );
      FileGuard guard{path};

      const std::filesystem::path tasks{"example-loading-tasks-dir"};
      REQUIRE( std::filesystem::create_directory(tasks) );
      DirectoryGuard dirGuard{tasks};
      const std::string task_content = R"conf(
      ## This line is a comment and will be ignored by the program.
      #And so is this line.

      # use OpenWeatherMap API
      api=OpenWeatherMap
      # collect data of weather forecast
      data=forecast
      # city of London, United Kingdom
      location.name=London
      location.countrycode=GB
      location.coordinates=51.507301,-0.1277
      # location's ID for OpenWeatherMap
      location.id=2643743
      # one request every hour, e.g. every 3600 seconds
      interval=3600
      )conf";
      REQUIRE( writeConfiguration(tasks / "owm.task", task_content) );
      const std::string task2_content = R"conf(
      # use Weatherstack API
      api=Weatherstack
      # collect data about current weather
      data=current
      # location somewhere in England, near the town of Newbury
      location.coordinates=51.5,-1.2
      # one request two hours minutes, e.g. every 7200 seconds
      interval=7200
      )conf";
      REQUIRE( writeConfiguration(tasks / "wstack.task", task2_content) );
      const std::string task3_content = R"conf(
      # use Open-Meteo API
      api=OpenMeteo
      # collect current and forecast data
      data=current+forecast
      # location: Alcatraz Island
      location.coordinates=37.8267,-122.4233
      # one request every 20 minutes, e.g. every 1200 seconds
      interval=1200
      )conf";
      REQUIRE( writeConfiguration(tasks / "meteo.task", task3_content) );

      Configuration conf;
      REQUIRE( conf.load(path.string(), false) );

      REQUIRE( conf.connectionInfo().hostname() == "db.domain.local" );
      REQUIRE( conf.connectionInfo().db() == "weather_db" );
      REQUIRE( conf.connectionInfo().user() == "user" );
      REQUIRE( conf.connectionInfo().password() == "secret(!) password" );
      REQUIRE( conf.connectionInfo().port() == 3366 );
      REQUIRE( conf.taskDirectory() == "example-loading-tasks-dir" );
      REQUIRE( conf.taskExtension() == ".task" );
      REQUIRE( conf.key(ApiType::OpenWeatherMap) == "0123456789abcdefdeadbeef1c0ffee1" );
      REQUIRE( conf.key(ApiType::Apixu) == "abcdef9876543210affe1affe2affe" );
      REQUIRE( conf.key(ApiType::DarkSky) == "f00ba12735743210fedcba9876543210" );
      REQUIRE( conf.key(ApiType::Weatherbit) == "fedcba98765432100123456789abcdef" );
      REQUIRE( conf.key(ApiType::Weatherstack) == "f00ba12abcdef1234567890abcdef123" );
      REQUIRE( conf.planOpenWeatherMap() == PlanOwm::Enterprise );
      REQUIRE( conf.planWeatherbit() == PlanWeatherbit::Business );
      REQUIRE( conf.planWeatherstack() == PlanWeatherstack::Business );

      REQUIRE( conf.tasks().size() == 3 );

      const Task& own_task = *std::find_if(conf.tasks().begin(), conf.tasks().end(),
          [](const Task& t) { return t.api() == ApiType::OpenWeatherMap; });
      REQUIRE( own_task.location().owmId() == 2643743 );
      REQUIRE( own_task.location().name() == "London" );
      REQUIRE( own_task.location().countryCode() == "GB" );
      REQUIRE( own_task.location().latitude() == 51.507301f );
      REQUIRE( own_task.location().longitude() == -0.1277f );
      REQUIRE( own_task.location().postcode().empty() );
      REQUIRE( own_task.api() == ApiType::OpenWeatherMap );
      REQUIRE( own_task.data() == DataType::Forecast );
      REQUIRE( own_task.interval() == std::chrono::seconds{3600} );

      const Task& ws_task = *std::find_if(conf.tasks().begin(), conf.tasks().end(),
          [](const Task& t) { return t.api() == ApiType::Weatherstack; });
      REQUIRE_FALSE( ws_task.location().hasOwmId() );
      REQUIRE( ws_task.location().name().empty() );
      REQUIRE( ws_task.location().countryCode().empty() );
      REQUIRE( ws_task.location().latitude() == 51.5f );
      REQUIRE( ws_task.location().longitude() == -1.2f );
      REQUIRE( ws_task.location().postcode().empty() );
      REQUIRE( ws_task.api() == ApiType::Weatherstack );
      REQUIRE( ws_task.data() == DataType::Current );
      REQUIRE( ws_task.interval() == std::chrono::seconds{7200} );

      const Task& third_task = *std::find_if(conf.tasks().begin(), conf.tasks().end(),
          [](const Task& t) { return t.api() == ApiType::OpenMeteo; });
      REQUIRE_FALSE( third_task.location().hasOwmId() );
      REQUIRE( third_task.location().name().empty() );
      REQUIRE( third_task.location().countryCode().empty() );
      REQUIRE( third_task.location().latitude() == 37.8267f );
      REQUIRE( third_task.location().longitude() == -122.4233f );
      REQUIRE( third_task.location().postcode().empty() );
      REQUIRE( third_task.api() == ApiType::OpenMeteo );
      REQUIRE( third_task.data() == DataType::CurrentAndForecast );
      REQUIRE( third_task.interval() == std::chrono::seconds{1200} );
    }

    SECTION("load example configuration file, but task directory does not exist")
    {
      const std::filesystem::path path{"example-loading-tasks-without-directory.conf"};
      const std::string content = R"conf(
      # This line is a comment and will be ignored by the program.
      #And so is this line.

      # database settings
      db.host=db.domain.local
      db.name=weather_db
      db.user=user
      db.password=secret(!) password
      db.port=3366
      tasks.directory=/this/directory/does-not/exist
      tasks.extension=.task
      # API keys
      key.owm=0123456789abcdefdeadbeef1c0ffee1
      key.apixu=abcdef9876543210affe1affe2affe
      key.darksky=f00ba12735743210fedcba9876543210
      key.weatherbit=fedcba98765432100123456789abcdef
      key.weatherstack=f00ba12abcdef1234567890abcdef123
      # plans
      plan.owm=enterprise
      plan.weatherbit=business
      plan.weatherstack=business
      )conf";
      REQUIRE( writeConfiguration(path, content) );
      FileGuard guard{path};

      Configuration conf;
      REQUIRE_FALSE( conf.load(path.string(), false) );
    }

    SECTION("load example configuration file, but task 'directory' is a file")
    {
      const std::filesystem::path path{"example-loading-tasks-without-directory.conf"};
      const std::string content = R"conf(
      # This line is a comment and will be ignored by the program.
      #And so is this line.

      # database settings
      db.host=db.domain.local
      db.name=weather_db
      db.user=user
      db.password=secret(!) password
      db.port=3366
      tasks.directory=task-directory.file
      tasks.extension=.task
      # API keys
      key.owm=0123456789abcdefdeadbeef1c0ffee1
      key.apixu=abcdef9876543210affe1affe2affe
      key.darksky=f00ba12735743210fedcba9876543210
      key.weatherbit=fedcba98765432100123456789abcdef
      key.weatherstack=f00ba12abcdef1234567890abcdef123
      # plans
      plan.owm=enterprise
      plan.weatherbit=business
      plan.weatherstack=business
      )conf";
      REQUIRE( writeConfiguration(path, content) );
      FileGuard guard{path};
      const std::filesystem::path task_path{"task-directory.file"};
      REQUIRE( writeConfiguration(task_path, "") );
      FileGuard guard2{task_path};

      Configuration conf;
      REQUIRE_FALSE( conf.load(path.string(), false) );
    }

    SECTION("load example configuration file, but loading tasks fails")
    {
      const std::filesystem::path path{"example-failing-tasks.conf"};
      const std::string content = R"conf(
      # This line is a comment and will be ignored by the program.
      #And so is this line.

      # database settings
      db.host=db.domain.local
      db.name=weather_db
      db.user=user
      db.password=secret(!) password
      db.port=3366
      tasks.directory=example-failing-tasks-dir
      tasks.extension=.task
      # API keys
      key.owm=0123456789abcdefdeadbeef1c0ffee1
      key.apixu=abcdef9876543210affe1affe2affe
      key.darksky=f00ba12735743210fedcba9876543210
      key.weatherbit=fedcba98765432100123456789abcdef
      key.weatherstack=f00ba12abcdef1234567890abcdef123
      # plans
      plan.owm=enterprise
      plan.weatherbit=business
      plan.weatherstack=business
      )conf";
      REQUIRE( writeConfiguration(path, content) );
      FileGuard guard{path};

      const std::filesystem::path tasks{"example-failing-tasks-dir"};
      REQUIRE( std::filesystem::create_directory(tasks) );
      DirectoryGuard dirGuard{tasks};
      const std::string task_content = R"conf(
      fail=true
      hardfail
      )conf";
      REQUIRE( writeConfiguration(tasks / "hardfail.task", task_content) );

      Configuration conf;
      REQUIRE_FALSE( conf.load(path.string(), false) );
    }

    SECTION("load example configuration file, but there are duplicate tasks")
    {
      const std::filesystem::path path{"duplicate-tasks.conf"};
      const std::string content = R"conf(
      # This line is a comment and will be ignored by the program.
      #And so is this line.

      # database settings
      db.host=db.domain.local
      db.name=weather_db
      db.user=user
      db.password=secret(!) password
      db.port=3366
      tasks.directory=the-duplicate-tasks-dir
      tasks.extension=.task
      # API keys
      key.owm=0123456789abcdefdeadbeef1c0ffee1
      key.apixu=abcdef9876543210affe1affe2affe
      key.darksky=f00ba12735743210fedcba9876543210
      key.weatherbit=fedcba98765432100123456789abcdef
      key.weatherstack=f00ba12abcdef1234567890abcdef123
      # plans
      plan.owm=enterprise
      plan.weatherbit=standard
      plan.weatherstack=business
      )conf";
      REQUIRE( writeConfiguration(path, content) );
      FileGuard guard{path};

      const std::filesystem::path tasks{"the-duplicate-tasks-dir"};
      REQUIRE( std::filesystem::create_directory(tasks) );
      DirectoryGuard dirGuard{tasks};
      const std::string task_content = R"conf(
      api=OpenWeatherMap
      data=forecast
      location.name=London
      location.countrycode=GB
      location.coordinates=51.507301,-0.1277
      location.id=2643743
      interval=3600
      )conf";
      REQUIRE( writeConfiguration(tasks / "first.task", task_content) );
      REQUIRE( writeConfiguration(tasks / "second.task", task_content) );

      Configuration conf;
      REQUIRE_FALSE( conf.load(path.string(), false) );
    }

    SECTION("load example configuration file, but tasks contain one Apixu task")
    {
      const std::filesystem::path path{"deprecated-apixu-task.conf"};
      const std::string content = R"conf(
      # This line is a comment and will be ignored by the program.
      #And so is this line.

      # database settings
      db.host=db.domain.local
      db.name=weather_db
      db.user=user
      db.password=secret(!) password
      db.port=3366
      tasks.directory=deprecated-apixu-task-dir
      tasks.extension=.task
      # API keys
      key.owm=0123456789abcdefdeadbeef1c0ffee1
      key.apixu=abcdef9876543210affe1affe2affe
      key.darksky=f00ba12735743210fedcba9876543210
      key.weatherbit=fedcba98765432100123456789abcdef
      key.weatherstack=f00ba12abcdef1234567890abcdef123
      # plans
      plan.owm=enterprise
      plan.weatherbit=free
      plan.weatherstack=business
      )conf";
      REQUIRE( writeConfiguration(path, content) );
      FileGuard guard{path};

      const std::filesystem::path tasks{"deprecated-apixu-task-dir"};
      REQUIRE( std::filesystem::create_directory(tasks) );
      DirectoryGuard dirGuard{tasks};
      const std::string task_content = R"conf(
      api=Apixu
      data=current
      location.name=London
      location.countrycode=GB
      location.coordinates=51.507301,-0.1277
      location.id=2643743
      interval=3600
      )conf";
      REQUIRE( writeConfiguration(tasks / "apixu.task", task_content) );

      Configuration conf;
      REQUIRE_FALSE( conf.load(path.string(), false) );
    }

    SECTION("load example configuration file, but tasks contain several Apixu tasks")
    {
      const std::filesystem::path path{"deprecated-apixu-tasks.conf"};
      const std::string content = R"conf(
      # This line is a comment and will be ignored by the program.
      #And so is this line.

      # database settings
      db.host=db.domain.local
      db.name=weather_db
      db.user=user
      db.password=secret(!) password
      db.port=3366
      tasks.directory=deprecated-apixu-tasks-dir
      tasks.extension=.task
      # API keys
      key.owm=0123456789abcdefdeadbeef1c0ffee1
      key.apixu=abcdef9876543210affe1affe2affe
      key.darksky=f00ba12735743210fedcba9876543210
      key.weatherbit=fedcba98765432100123456789abcdef
      key.weatherstack=f00ba12abcdef1234567890abcdef123
      # plans
      plan.owm=enterprise
      plan.weatherbit=free
      plan.weatherstack=business
      )conf";
      REQUIRE( writeConfiguration(path, content) );
      FileGuard guard{path};

      const std::filesystem::path tasks{"deprecated-apixu-tasks-dir"};
      REQUIRE( std::filesystem::create_directory(tasks) );
      DirectoryGuard dirGuard{tasks};
      const std::string task_content = R"conf(
      api=Apixu
      data=current
      location.name=London
      location.countrycode=GB
      location.coordinates=51.507301,-0.1277
      location.id=2643743
      interval=3600
      )conf";
      REQUIRE( writeConfiguration(tasks / "apixu1.task", task_content) );
      const std::string task2_content = R"conf(
      api=Apixu
      data=current
      location.name=Paris
      location.countrycode=FR
      location.coordinates=48.853,2.349
      interval=7200
      )conf";
      REQUIRE( writeConfiguration(tasks / "apixu2.task", task2_content) );

      Configuration conf;
      REQUIRE_FALSE( conf.load(path.string(), false) );
    }

    SECTION("load configuration file, but tasks contain one DarkSky task")
    {
      const std::filesystem::path path{"deprecated-darksky-task.conf"};
      const std::string content = R"conf(
      # This line is a comment and will be ignored by the program.
      #And so is this line.

      # database settings
      db.host=db.domain.local
      db.name=weather_db
      db.user=user
      db.password=secret(!) password
      db.port=3366
      tasks.directory=deprecated-darksky-task-dir
      tasks.extension=.task
      # API keys
      key.owm=0123456789abcdefdeadbeef1c0ffee1
      key.apixu=abcdef9876543210affe1affe2affe
      key.darksky=f00ba12735743210fedcba9876543210
      key.weatherbit=fedcba98765432100123456789abcdef
      key.weatherstack=f00ba12abcdef1234567890abcdef123
      # plans
      plan.owm=enterprise
      plan.weatherbit=standard
      plan.weatherstack=business
      )conf";
      REQUIRE( writeConfiguration(path, content) );
      FileGuard guard{path};

      const std::filesystem::path tasks{"deprecated-darksky-task-dir"};
      REQUIRE( std::filesystem::create_directory(tasks) );
      DirectoryGuard dirGuard{tasks};
      const std::string task_content = R"conf(
      api=DarkSky
      data=current
      location.name=London
      location.countrycode=GB
      location.coordinates=51.507301,-0.1277
      location.id=2643743
      interval=3600
      )conf";
      REQUIRE( writeConfiguration(tasks / "darksky.task", task_content) );

      Configuration conf;
      REQUIRE_FALSE( conf.load(path.string(), false) );

      // Technically, one cannot rely on the following information being there,
      // because the configuration failed to load. But the current
      // implementation is such that those values are set.
      REQUIRE( conf.connectionInfo().hostname() == "db.domain.local" );
      REQUIRE( conf.connectionInfo().db() == "weather_db" );
      REQUIRE( conf.connectionInfo().user() == "user" );
      REQUIRE( conf.connectionInfo().password() == "secret(!) password" );
      REQUIRE( conf.connectionInfo().port() == 3366 );
      REQUIRE( conf.taskDirectory() == "deprecated-darksky-task-dir" );
      REQUIRE( conf.taskExtension() == ".task" );
      REQUIRE( conf.key(ApiType::OpenWeatherMap) == "0123456789abcdefdeadbeef1c0ffee1" );
      REQUIRE( conf.key(ApiType::Apixu) == "abcdef9876543210affe1affe2affe" );
      REQUIRE( conf.key(ApiType::DarkSky) == "f00ba12735743210fedcba9876543210" );
      REQUIRE( conf.key(ApiType::Weatherbit) == "fedcba98765432100123456789abcdef" );
      REQUIRE( conf.key(ApiType::Weatherstack) == "f00ba12abcdef1234567890abcdef123" );
      REQUIRE( conf.planOpenWeatherMap() == PlanOwm::Enterprise );
      REQUIRE( conf.planWeatherbit() == PlanWeatherbit::Standard );
      REQUIRE( conf.planWeatherstack() == PlanWeatherstack::Business );

      REQUIRE( conf.tasks().size() == 1 );

      const Task& darksky_task = conf.tasks().at(0);
      REQUIRE( darksky_task.location().owmId() == 2643743 );
      REQUIRE( darksky_task.location().name() == "London" );
      REQUIRE( darksky_task.location().countryCode() == "GB" );
      REQUIRE( darksky_task.location().latitude() == 51.507301f );
      REQUIRE( darksky_task.location().longitude() == -0.1277f );
      REQUIRE( darksky_task.location().postcode().empty() );
      REQUIRE( darksky_task.api() == ApiType::DarkSky );
      REQUIRE( darksky_task.data() == DataType::Current );
      REQUIRE( darksky_task.interval() == std::chrono::seconds{3600} );
    }

    SECTION("load configuration file, but tasks contain several DarkSky tasks")
    {
      const std::filesystem::path path{"deprecated-darksky-tasks.conf"};
      const std::string content = R"conf(
      # This line is a comment and will be ignored by the program.
      #And so is this line.

      # database settings
      db.host=db.domain.local
      db.name=weather_db
      db.user=user
      db.password=secret(!) password
      db.port=3366
      tasks.directory=deprecated-darksky-tasks-dir
      tasks.extension=.task
      # API keys
      key.owm=0123456789abcdefdeadbeef1c0ffee1
      key.apixu=abcdef9876543210affe1affe2affe
      key.darksky=f00ba12735743210fedcba9876543210
      key.weatherbit=fedcba98765432100123456789abcdef
      key.weatherstack=f00ba12abcdef1234567890abcdef123
      # plans
      plan.owm=enterprise
      plan.weatherbit=standard
      plan.weatherstack=business
      )conf";
      REQUIRE( writeConfiguration(path, content) );
      FileGuard guard{path};

      const std::filesystem::path tasks{"deprecated-darksky-tasks-dir"};
      REQUIRE( std::filesystem::create_directory(tasks) );
      DirectoryGuard dirGuard{tasks};
      const std::string task_content = R"conf(
      api=DarkSky
      data=current
      location.name=London
      location.countrycode=GB
      location.coordinates=51.507301,-0.1277
      location.id=2643743
      interval=3600
      )conf";
      REQUIRE( writeConfiguration(tasks / "darksky1.task", task_content) );
      const std::string task2_content = R"conf(
      api=DarkSky
      data=current+forecast
      location.name=Paris
      location.countrycode=FR
      location.coordinates=48.853,2.349
      interval=7200
      )conf";
      REQUIRE( writeConfiguration(tasks / "darksky2.task", task2_content) );

      Configuration conf;
      REQUIRE_FALSE( conf.load(path.string(), false) );

      // Technically, one cannot rely on the following information being there,
      // because the configuration failed to load. But the current
      // implementation is such that those values are set.
      REQUIRE( conf.connectionInfo().hostname() == "db.domain.local" );
      REQUIRE( conf.connectionInfo().db() == "weather_db" );
      REQUIRE( conf.connectionInfo().user() == "user" );
      REQUIRE( conf.connectionInfo().password() == "secret(!) password" );
      REQUIRE( conf.connectionInfo().port() == 3366 );
      REQUIRE( conf.taskDirectory() == "deprecated-darksky-tasks-dir" );
      REQUIRE( conf.taskExtension() == ".task" );
      REQUIRE( conf.key(ApiType::OpenWeatherMap) == "0123456789abcdefdeadbeef1c0ffee1" );
      REQUIRE( conf.key(ApiType::Apixu) == "abcdef9876543210affe1affe2affe" );
      REQUIRE( conf.key(ApiType::DarkSky) == "f00ba12735743210fedcba9876543210" );
      REQUIRE( conf.key(ApiType::Weatherbit) == "fedcba98765432100123456789abcdef" );
      REQUIRE( conf.key(ApiType::Weatherstack) == "f00ba12abcdef1234567890abcdef123" );
      REQUIRE( conf.planOpenWeatherMap() == PlanOwm::Enterprise );
      REQUIRE( conf.planWeatherbit() == PlanWeatherbit::Standard );
      REQUIRE( conf.planWeatherstack() == PlanWeatherstack::Business );

      REQUIRE( conf.tasks().size() == 2 );

      const Task& first_task = *std::find_if(conf.tasks().begin(), conf.tasks().end(),
          [](const Task& t) { return t.data() == DataType::Current; });
      REQUIRE( first_task.location().owmId() == 2643743 );
      REQUIRE( first_task.location().name() == "London" );
      REQUIRE( first_task.location().countryCode() == "GB" );
      REQUIRE( first_task.location().latitude() == 51.507301f );
      REQUIRE( first_task.location().longitude() == -0.1277f );
      REQUIRE( first_task.location().postcode().empty() );
      REQUIRE( first_task.api() == ApiType::DarkSky );
      REQUIRE( first_task.data() == DataType::Current );
      REQUIRE( first_task.interval() == std::chrono::seconds{3600} );

      const Task& second_task = *std::find_if(conf.tasks().begin(), conf.tasks().end(),
          [](const Task& t) { return t.data() == DataType::CurrentAndForecast; });
      REQUIRE_FALSE( second_task.location().hasOwmId() );
      REQUIRE( second_task.location().name() == "Paris" );
      REQUIRE( second_task.location().countryCode() == "FR" );
      REQUIRE( second_task.location().latitude() == 48.853f );
      REQUIRE( second_task.location().longitude() == 2.349f );
      REQUIRE( second_task.location().postcode().empty() );
      REQUIRE( second_task.api() == ApiType::DarkSky );
      REQUIRE( second_task.data() == DataType::CurrentAndForecast );
      REQUIRE( second_task.interval() == std::chrono::seconds{7200} );
    }
  }
}
