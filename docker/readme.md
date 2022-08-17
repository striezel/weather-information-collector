# Docker container for weather-information-collector

The Dockerfile in this directory builds an image that downloads the latest
tagged version of weather-information-collector from GitLab, builds a Debian
package (*.deb) out of it, which will then be installed to provide all the
necessary binary files. _(This means that the image will not use the current Git
checkout, but only the latest public tag. If that is not what you want, you need
to change the involved scripts.)_

## Quickstart (also known as "tl;dr")

Add you configuration files to the `configuration/` directory and type

    docker build -t weather .
    docker run -d weather

into your terminal. That's it. If you are on Linux (or a compatible system with
Bash) you can also use the utility script for building the image and starting a
container from that image:

    ./start.sh

For a more detailed walkthrough see below.

## Setting up the Docker container

### Preparation: Installation of Docker

First, you have to make sure that Docker is installed. For the purpose of this
installation, the readme will assume that you are using Debian 8.0 "jessie".
The required steps should be similar on other Linux distributions. You may want
to check <https://docs.docker.com/install/> for instructions on other Linux
distributions or other operating systems like Windows or MacOS.

To install Docker, you need to add the APT repository for jessie-backports to
your APT sources, if that has not been done yet. Execute the following command
as root user to add the AP repository:

    echo "deb http://ftp.de.debian.org/debian/ jessie-backports main" >> /etc/apt/sources.list

After that you can install Docker by typing the following commands:

    # update of package lists
    apt-get update
    # install Docker package and dependencies without further confirmation
    apt-get install -y docker.io

The standard configuration right after the installation of Docker only allows
the root user to use Docker. Since it is more advisable to run Docker as a
different user, you need to add this other user (here: user1) to the group
docker. Type

    gpasswd -a user1 docker

to add the user _user1_ to the docker group. Repeat for other users, if needed.
Group membership will be applied _after the next login_ of the user, so these
users might need to log off and on again before they can continue.

Furthermore you should restart the Docker daemon:

    systemctl start docker

or

    systemctl restart docker

should trigger the restart.

After that the user _user1_ can type

    docker info

into a terminal in order to check whether he/she can execute docker commands in
his/her user context. If the command displays an error, then something is not
quite right yet.

### Add configuration files

You need to add configuration files for weather-information-collector to run
properly. Place them inside the `configuration/` directory. They will be copied
to the proper place inside the Docker image during the build process. See
[the readme inside the directory](./configuration/readme.md) for some more
information.

### Let's party: Build docker image and create container

The following commands have to be issued in the root directory of the checked
out source code:

    # create image - may take several minutes during the first run; runs after
    # that are faster, because Docker caches some data
    docker build -t weather .
    # start container based on that image
    docker run -d weather

After that weather-information-collector will run inside the container and start
the collection of weather data. Just make sure the MySQL / MariaDB database can
be accessed from inside the container.
