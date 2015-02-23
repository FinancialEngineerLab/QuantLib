/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
This class is created for using the extended hull-white model with
term structure of volatility.

Created by Jihoon Lee, 20150127

*/

/*! \file hullwhitevolatility.hpp
	\brief HullWhite functions
*/

#ifndef quantlib_hullwhitevolatility_hpp
#define quantlib_hullwhitevolatility_hpp

#include <ql/time/date.hpp>
#include <ql/math/matrix.hpp>
#include <ql/time/daycounter.hpp>
#include <ql/models/parameter.hpp>
#include <ql/termstructures/interpolatedcurve.hpp>
#include <ql/math/interpolations/linearinterpolation.hpp>
#include <boost/function.hpp>

#include <ql/types.hpp>

namespace QuantLib {

	class HullWhiteVolatility {
	 public:
		 HullWhiteVolatility();

		 HullWhiteVolatility(
			 Date referenceDate,
			 DayCounter dc,
			 std::vector<Volatility>& vols,
			 std::vector<Date>& volDates);		 

		 boost::function<Real (Time)> vol() const;

		 Real sigma() const { return sigma_(0.0); }

	 private:

		 Date referencedate_;
		 DayCounter daycounter_;

		 std::vector<Date> volstructure_;
		 std::vector<Time> volperiods_;		 

		 Parameter sigma_;
	};

}

#endif