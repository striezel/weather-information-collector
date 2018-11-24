# Troubleshooting guide for weather-information-collector

## Synchronization tool (weather-information-collector-synchronizer)

Depending on the selected batch size (default: 125) a few problems could occur.

* **Q: Synchronization throws an exception half way through, complaining that
  the "MySQL server is gone". What should I do?**

  A: The reason for this is that MySQL server connections will timeout after a
  certain amount of time. Decrease the batch size by using the `-b` parameter, 
  followed by a new batch size. E.g. `-b 20` will set the batch size to 20.

* **Q: Synchronization takes quite long. Is there a way to decrease the time?**

  A: The tool will synchronize all data between to database servers. Naturally,
  the amount of time needed for that process depends on the amount of data that
  you have collected and written to the involved databases. Also take the load
  of those servers and the network connection from/to those servers into
  account. You can speed up the process by

  * synchronizing data more often: The tool will only insert new data sets into
    the destination database, so less data needs to be inserted when the
    previous synchronization was not that long ago.
  * increasing the batch size: higher batch size will mean faster insertion of
    data into the destination database. However, setting the batch size to a
    value that is too high could hit some MySQL server limits, most likely
    `max_allowed_packet`. More information about it is available at
    <https://mariadb.com/kb/en/library/server-system-variables/#max_allowed_packet>.

## Collector, configuration test and updater

There are no known problems with these programs.
