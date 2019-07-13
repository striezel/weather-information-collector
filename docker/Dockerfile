# Image is based on Debian 9, because the current version of Debian 10 does not
# have the libmysql++ package yet.
FROM debian:9-slim
MAINTAINER Dirk Stolle <striezel-dev@web.de>

# Packages should be up to date.
RUN apt-get update && apt-get upgrade -y

# Install all required dependencies + compiler.
RUN apt-get install -y --no-install-recommends catch cmake g++ git \
    libboost-filesystem-dev libcurl4-gnutls-dev libjsoncpp-dev libmysql++-dev \
    make pkg-config
# Install root certificates - required for git clone.
RUN apt-get install -y ca-certificates
RUN mkdir -p /opt
WORKDIR /opt
RUN git clone https://gitlab.com/striezel/weather-information-collector.git
WORKDIR /opt/weather-information-collector
RUN ./ci/gitlab-01-install-dependencies.sh
RUN ./ci/gitlab-02-create-package.sh
RUN dpkg -i weather-information-collector*.deb
# Partial cleanup.
RUN ./ci/docker-cleanup.sh
# -- remove Git repository
WORKDIR /opt
RUN rm -rf /opt/weather-information-collector
# Create new user that will be used to run the program.
RUN useradd --create-home wic
USER wic
WORKDIR /home/wic
# For now just show the version of the binary, we have no configuration files
# to actually use them, yet.
CMD weather-information-collector --version
