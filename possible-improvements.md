# Possible improvements

The weather-information-collector is currently (2018-08-06, version 0.6.7)
fully functional. However, there still is room for improvements here.

Possible future improvements may include:

* support of more weather data APIs
* threaded data collection to avoid slowdown, if the weather data server is
  slow / takes long to respond
* a possibility to stop the collection (other than killing the collection
  process via kill -9)
