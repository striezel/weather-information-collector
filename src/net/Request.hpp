/*
 -------------------------------------------------------------------------------
    This file is part of the weather information collector.
    Copyright (C) 2021  Dirk Stolle

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 -------------------------------------------------------------------------------
*/

#ifndef WEATHER_INFORMATION_COLLECTOR_NET_REQUEST_HPP
#define WEATHER_INFORMATION_COLLECTOR_NET_REQUEST_HPP

#include <optional>
#include <string>
#include <string_view>

/** Utility class to simplify HTTP requests. */
class Request
{
  public:
    /** \brief Performs an HTTP GET request to the given URL.
     *
     * \param url   the URL to get
     * \param fn    name of the calling function (only used in error case)
     * \return Returns an optional containing the response, if the request was
     *         successful. Returns an empty optional, if the request failed.
     *         Every HTTP status code other than 200 is considered an error.
     * \remark This method may print to the standard error stream, if the
     *         request fails (i. e. returns an HTTP status code other than 200).
     */
    static std::optional<std::string> get(const std::string& url, const std::string_view fn);
}; // class

#endif // WEATHER_INFORMATION_COLLECTOR_NET_REQUEST_HPP
