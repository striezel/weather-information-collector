#!/bin/bash
apt-get update
if [ $? -ne 0 ]
then
  echo "ERROR: APT package lists could not be updated!"
  exit 1
fi

apt-get -y install --no-install-recommends build-essential bzip2 ca-certificates debhelper devscripts fakeroot g++ git lsb-release tar wget
if [ $? -ne 0 ]
then
  echo "ERROR: Could not install packages required for *.deb creation!"
  exit 1
fi

apt-get -y install --no-install-recommends cmake g++ libcurl4-gnutls-dev libmariadb-dev pkg-config
if [ $? -ne 0 ]
then
  echo "ERROR: Could not install build dependencies!"
  exit 1
fi

apt-get -y install --no-install-recommends catch2 || apt-get -y install --no-install-recommends catch
if [ $? -ne 0 ]
then
  echo "ERROR: Could not install CATCH!"
  exit 1
fi
