# Configuration files for use inside the Docker container

Place your configuration files for weather-information-collector inside this
directory, if you want them to be used by the Docker container. You can start
with the example file `wic.conf.example` by renaming it to `wic.conf` and
adjusting the values in it as needed.

The contents of this directory will be copied to `/home/wic/.wic/` inside the
Docker container. Therefore, if you have a file named `wic.conf` here, it will
automatically be the [core configuration file](../../doc/configuration-core.md).

Note that the Docker container will exit directly after its start, if there is
no configuration.
