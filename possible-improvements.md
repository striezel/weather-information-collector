# Possible improvements

The weather-information-collector is currently (2017-08-23), although very
functional,  still in an early stage of development. There still is room for
improvements here.

Possible future improvements may include:

* support of more weather data APIs
* support for collecting forecast data
* threaded data collection to avoid slowdown, if the weather data server is
  slow / takes long to respond
* a possibility to stop the collection (other than killing the collection
  process via kill -9)
* API limit checks for other plans than the free API plans
