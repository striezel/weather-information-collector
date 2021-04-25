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

#ifndef WEATHER_INFORMATION_COLLECTOR_NUMERICPRECISION_HPP
#define WEATHER_INFORMATION_COLLECTOR_NUMERICPRECISION_HPP

#include <cmath>
#include <limits>

namespace wic
{

template<typename floatT>
struct NumericPrecision
{
  private:
    static_assert( std::numeric_limits<floatT>::is_iec559 );
    static constexpr floatT factor = static_cast<floatT>(1000);
    static constexpr floatT max_T = std::numeric_limits<floatT>::max() / factor;
    static constexpr floatT min_T = std::numeric_limits<floatT>::lowest() / factor;
  public:
    /** \brief Rounds the floating point value to three decimals after the point.
     *
     * \param f   the floating point value to be rounded
     * \return  Returns the rounded value.
     */
    static floatT enforce(const floatT& f)
    {
      // Handle old style (v0.8.1) rounding for values that are close to the
      // integer value.
      const floatT valueRounded = std::round(f);
      if (std::fabs(valueRounded - f) < 0.00501)
        return valueRounded;
      // If multiplication would overflow, then do not round.
      if (f > max_T || f < min_T)
        return f;
      // Round to three digits after the decimal separator.
      return std::round(f * factor) / factor;
    }
}; // struct

} // namespace

#endif // WEATHER_INFORMATION_COLLECTOR_NUMERICPRECISION_HPP
