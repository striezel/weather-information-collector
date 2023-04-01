# Documentation for weather-information-collector

weather-information-collector is a tool that collects weather information from
various available ReST API services.

# Preparations

In order to use the full potential of weather-information-collector you need
the compiled program (see the readme in the root directory for build
instructions) and a MySQL database server - or any compatible database server
like MariaDB or Percona Server.

## MariaDB / MySQL / Percona server setup

I assume that you know how to install such a database server, as it usually
boils down to one of the following commands in a root terminal:

    apt-get install mariadb-server
    # or
    sudo apt-get install mariadb-server
    # or
    sudo yum install mariadb-server

and the configuration of a password for the root user. I'll leave that to you
as an exercise.

Known compatible database servers are listed in the
[supported databases document](./supported-databases.md).

After that the database and table structure for weather-information-collector
needs to be created. You can use the SQL dump file provided in
[database/weather_information_collector.sql](../database/weather_information_collector.sql)
to accomplish that.

Additionally you should create a MySQL user that has full access to the created
database tables. (Please do not just use the root user for any database access,
that's ugly.) A database user named `wic` can be created with SQL commands like
the following:

```sql
-- don't allow anything ("USAGE") to user wic
GRANT USAGE ON *.* TO 'wic'@'localhost' IDENTIFIED BY 'secret-password';
-- give the user wic all rights ("ALL PRIVILEGES") for tables of the database weather_information_collector
GRANT ALL PRIVILEGES ON weather_information_collector.* TO 'wic'@'localhost' IDENTIFIED BY 'secret-password';
-- flush database privilege cache to make changes effective immediately
FLUSH PRIVILEGES;
```

For more details on GRANT and the complete syntax see the
[MySQL documentation for GRANT](https://dev.mysql.com/doc/refman/5.7/en/grant.html).

## Get API keys for a weather data provider of choice

Since weather-information-collector gets the data via REST-API from a weather
data provider and these providers usually require their users to provide an API
key with every request, the next step is to sign up for a weather data provider
of your choice. Currently, weather-information-collector supports the following
four providers:

* [OpenWeatherMap](https://openweathermap.org/)
* [OpenMeteo](https://open-meteo.com/) (no API key required)
* [Weatherbit](https://www.weatherbit.io/)
* [Weatherstack](https://weatherstack.com/)

Choose the one you like best (or maybe even all of them), and make sure you
have the API key ready. It will be needed for the configuration file of
weather-information-collector.

# Configuration file

The configuration file is the place where some of the core settings like the
database connection information or the API keys are stored. Since this is some
sensitive information, take precaution that no other users or only users who
you trust can access the configuration file. This can usually be achieved by
setting proper file permissions and file ownership for the configuration file.

Full details on the core configuration (supported settings, etc.) can be found
[here](configuration-core.md).

# Task files

Another type of configuration files are the so-called task files. These files
describe what weather information the program shall collect and in which
intervals the collection takes places. Unlike the core configuration file from
the previous section these files do not contain any credentials or similar
confidential information.

The directory where tasks files are located can be set via the core
configuration file (see above).

Full details on the task files (supported settings, etc.) can be found
[here](task-files.md).

# Troubleshooting

Should you encounter any problems with weather-information-collector's programs,
take a look at the [troubleshooting guide](troubleshooting.md).

# Building from source on Windows

For information how to build weather-information-collector on Windows, see the
[MSYS2 build instructions](msys2-build.md).
