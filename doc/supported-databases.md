# Supported databases

In general, weather-information-collector supports any somewhat recent MariaDB
database server. This also includes compatible systems like MySQL or Percona
Server.

| DBMS          | Supported   | Tested                   |
| ------------- | ----------- | ------------------------ |
| MariaDB 11.6  | yes         | yes                      |
| MariaDB 11.5  | yes         | yes                      |
| MariaDB 11.4  | yes         | yes                      |
| MariaDB 11.3  | yes         | no (reached end of life) |
| MariaDB 11.2  | yes         | yes                      |
| MariaDB 11.1  | yes         | no (reached end of life) |
| MariaDB 11.0  | yes         | no (reached end of life) |
| MariaDB 10.11 | yes         | yes                      |
| MariaDB 10.10 | yes         | no (reached end of life) |
| MariaDB 10.9  | yes         | no (reached end of life) |
| MariaDB 10.8  | yes         | no (reached end of life) |
| MariaDB 10.7  | yes         | no (reached end of life) |
| MariaDB 10.6  | yes         | yes                      |
| MariaDB 10.5  | yes         | yes                      |
| MariaDB 10.4  | yes         | no (reached end of life) |
| MariaDB 10.3  | yes         | no (reached end of life) |
| MariaDB 10.2  | yes         | no (reached end of life) |
| MySQL 9.0     | yes _(*)_   | yes                      |
| MySQL 8.4     | yes _(*)_   | yes                      |
| MySQL 8.3     | yes _(*)_   | no (reached end of life) |
| MySQL 8.2     | yes _(*)_   | no (reached end of life) |
| MySQL 8.0     | yes _(*)_   | yes                      |
| MySQL 5.7     | yes         | yes                      |
| Percona 8.4   | yes _(*)_   | yes                      |
| Percona 8.0   | yes _(*)_   | yes                      |
| Percona 5.7   | yes         | yes                      |

In theory, weather-information-collector should even be able to work with older
DBMS versions down to MariaDB 5.5 / MySQL Server 5.5 / Percona Server 5.5.
However, these old versions have not been tested and have reached their end of
life anyway.

_(*)_ = MySQL 8.0.4 introduced a backwards incompatible, breaking change and
changed the default authentication plugin from `mysql_native_password` to
`caching_sha2_password`. (See <https://dev.mysql.com/doc/relnotes/mysql/8.0/en/news-8-0-4.html>.)
This may cause connection problems when using certain older versions of the
MariaDB client library which cannot handle that authentication mechanism
correctly. _(MariaDB just went for the more secure `client_ed25519`
authentication plugin using elliptic curves to replace the old
`mysql_native_password`.)_ If the database account you want to use to connect to
the database uses the `caching_sha2_password` plugin authentication, then you
may need to change that to be able to use weather-information-collector with it.
You can see that the database user is affected by this, if a warning that looks
like

> Authentication plugin 'caching_sha2_password'
  cannot be loaded: /usr/lib/mysql/plugin/caching_sha2_password.so: cannot open
  shared object file: No such file or directory

occurs when trying to connect to the database.

A quick fix would be to log in as administrative database user and type these
SQL statements, assuming the affected database user is `weather_user` and has
the password `"secret-password"`:

```sql
-- change user to use mysql_native_password authentication plugin
ALTER USER weather_user@localhost IDENTIFIED WITH mysql_native_password BY '';
-- set password again, this time using the compatible authentiction
ALTER USER weather_user@localhost IDENTIFIED BY 'secret-password';
-- flush database privileges so changes take effect immediately without the need
-- to perform a database server restart
FLUSH PRIVILEGES;
```
As with any sensitive database change - and changing the authentication surely
is such a change - you should backup your database _before_ proceeding.
