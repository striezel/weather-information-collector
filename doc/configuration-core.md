# weather-information-collector: core configuration file

The configuration file of weather-information-collector is the place where the
core settings like the database connection information or the API keys are
stored. **Since this is sensitive information, take precaution that no other
users or only users who you trust can access the configuration file.** This can
usually be achieved by setting proper file permissions and file ownership for
the configuration file. (Something along the lines of `chmod 600 FILE_NAME` is
the usual approach, together with `chown user:group FILE_NAME`.)

# Location of the configuration file

The location of the configuration file can be specified via the command line
option `-c`. For example

    /path/to/weather-information-collector -c /home/user/some/path/wic.conf

will tell the program to look for the configuration file
`/home/user/some/path/wic.conf`. If no such option is given, the program will
look for the configuration file in these predefined locations, in the given
order:

* `user's home directory`/.wic/wic.conf
* `user's home directory`/.weather-information-collector/wic.conf
* `user's home directory`/.weather-information-collector/weather-information-collector.conf
* /etc/weather-information-collector/weather-information-collector.conf
* /etc/weather-information-collector/wic.conf
* /etc/wic/wic.conf
* ./weather-information-collector.conf
* ./wic.conf

If none of these files exist and no location is given for the configuration
file, then the program will exit with a non-zero exit code.

# Configuration elements

## General format

Configuration settings are basically key-value pairs, where the key is specified
first, followed by an equals sign (`=`), followed by the value of the setting.
For example, the line

    db.host=localhost

sets the value of `db.host` to `localhost`.

## Comments

The configuration file allows comment lines. Comment lines are lines where the
first character is `#`. These lines are ignored by the application. A line has
to be either a comment or configuration data, you cannot mix both on the same
line.

Completely empty lines are ignored, too.

## Database settings

The following database settings are recognized in the configuration file:

* **db.host** - host name of the server where the MySQL database runs
* **db.name** - database name
* **db.user** - name of the MySQL database user
* **db.password** - password for the database user
* **db.port** - _(optional)_ port number for the database server
  (If this setting is not provided, the port will automatically be set to 3306,
   which is the default port for MySQL database servers.)

For example, the database configuration could look like this:

    db.host=localhost
    db.name=weather_information_collector
    db.user=wic
    db.password=super-secret-password!
    db.port=3306

## Task-related settings

Collection tasks are defined in so-called task files. The following settings
are available to tell the weather-information-collector where these task files
can be found.

* **task.directory** - directory where the task files are located
* **task.extension** - file extension for task files: only files with the given
  extension are read by the weather-information-collector. The extension has to
  include the dot, e.g. a possible value would be `.ext`. If you leave the value
  empty, then all files in the task directory are read, regardless of extension.

Possible values for these settings could be:

    task.directory=/home/user/.wic/task.d
    task.extension=.task

## API plans

It is possible to specify the pricing plans in use for some of the weather APIs
used by weather-information-collector. The plans are used to check whether the
configuration collection tasks will hit the API's rate limit. Those settings can
also be omitted, in which case the program assumes the use of the free plan.

The following configuration settings for weather API plans are recognized:

* **plan.owm** - pricing plan for OpenWeatherMap API; currently the following
  plans are recognized:
  * **free** - free plan (up to 60 calls per minute)
  * **startup** - Startup plan (up to 600 calls per minute)
  * **developer** - Developer plan (up to 3000 calls per minute)
  * **professional** - Professional plan (up to 30000 calls per minute)
  * **enterprise** - Enterprise plan (up to 200000 calls per minute)
* **plan.weatherbit** - pricing plan for Weatherbit API; currently the following
  plans are recognized:
  * **free** - free plan (up to 50 calls per day)
  * **standard** - Standard plan (up to 25000 calls per day)
  * **plus** - Plus plan (up to 250000 calls per day)
  * **business** - Business plan (up to 2000000 calls per day)
* **plan.weatherstack** - pricing plan for Weatherstack API; currently the
  following plans are recognized:
  * **free** - free plan (up to 100 calls per month)
  * **standard** - Standard plan (up to 50000 calls per month)
  * **professional** - Professional plan (up to 300000 calls per month)
  * **business** - Business plan (up to 1000000 calls per month)

## API key settings

Finally, one has to specify API key settings. An API key has to be specified for
every distinct API that will be used by the collection tasks, except for the
Open-Meteo API. Failure to give a key for the other used APIs is an error.

The following configurations settings for keys are recognized:

* **key.owm** - API key for OpenWeatherMap API calls
* **key.weatherbit** - API key for Weatherbit API calls
* **key.weatherstack** - API key for Weatherstack API calls
* **key.apixu** - API key for Apixu API calls (deprecated, do not use anymore)
* **key.darksky** - API key for DarkSky API calls (deprecated, do not use anymore)

Possible values of these settings could be:

    key.owm=0123456789abcdefdeadbeef1c0ffee1
    key.weatherbit=fedcba98765432100123456789abcdef
    key.weatherstack=f00ba12abcdef1234567890abcdef123

# Example of a complete configuration file

The following example is a complete core configuration file for the
weather-information-collector program (as of version 0.12.0):

    # This line is a comment and will be ignored by the program.
    #And so is this line.

    # database settings
    db.host=localhost
    db.name=weather_information_collector
    db.user=wic
    db.password=secret, secret, top(!) secret
    db.port=3306
    # task directory information
    tasks.directory=/home/user/.wic/task.d
    tasks.extension=.task
    # API keys
    key.owm=0123456789abcdefdeadbeef1c0ffee1
    key.weatherbit=fedcba98765432100123456789abcdef
    key.weatherstack=f00ba12abcdef1234567890abcdef123
    # API plans
    plan.owm=developer
    plan.weatherbit=standard
    plan.weatherstack=free
