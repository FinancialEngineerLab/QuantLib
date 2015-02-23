/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
This class is created for using the extended hull-white model with
term structure of volatility.

Created by Jihoon Lee, 20150127

*/

#include <ql/termstructures/volatility/hullwhitevolatility.hpp>

namespace QuantLib {

	namespace {

		class PiecewiseLinearCurve : public InterpolatedCurve<Linear> {
		public:
			PiecewiseLinearCurve(const std::vector<Time>& times,
				const std::vector<Real>& data)
				: InterpolatedCurve<Linear>(times, data) {
					setupInterpolation();
			}

			Real operator()(Time t) {
				return interpolation_(t,true);
			}
		};

		class PiecewiseConstantParameter2 : public Parameter {
		private:
			class Impl : public Parameter::Impl {
			public:
				Impl(const std::vector<Time>& times)
					: times_(times) {}

				Real value(const Array& params, Time t) const {
					Size size = times_.size();
					for (Size i=0; i<size; i++) {
						if (t<times_[i])
							return params[i];
					}
					return params[size-1];
				}
			private:
				std::vector<Time> times_;
			};
		public:
			PiecewiseConstantParameter2(const std::vector<Time>& times,
										const Constraint& constraint =
															NoConstraint())
				: Parameter(times.size(),
							boost::shared_ptr<Parameter::Impl>(
								new PiecewiseConstantParameter2::Impl(times)),
							constraint)
			{}
		};

		Real identity(Real x) {
			return x;
		}
	}

	HullWhiteVolatility::HullWhiteVolatility()
		: referencedate_(),
		  daycounter_(),
		  sigma_(),
		  volstructure_(){

	}

	HullWhiteVolatility::HullWhiteVolatility(
		Date referenceDate, 
		DayCounter dc,
		std::vector<Volatility>& vols, 
		std::vector<Date>& volDates)
		: referencedate_(referenceDate),
		  daycounter_(dc),
		  sigma_(),
		  volstructure_(volDates){

			for (Size i = 0; i < volstructure_.size(); i++)
				volperiods_.push_back(daycounter_.yearFraction(referencedate_, volstructure_[i]));
			
			sigma_ = PiecewiseConstantParameter2(volperiods_, PositiveConstraint());

			for (Size i=0; i< sigma_.size();i++) {
				sigma_.setParam(i,vols[i]);
			}

	}


	boost::function<Real (Time)> HullWhiteVolatility::vol() const {

		std::vector<Real> volvals;
		volvals.push_back(sigma_(0.001));

		if (sigma_.size()==1) {
			std::vector<Time> volper;
			volper.push_back(0.0);
			volper.push_back(200); //200 years to approximate the
			//constant coefficient model
			volvals.push_back(sigma_(0.001));
			return PiecewiseLinearCurve(volper, volvals);
		}

		for (Size i=0;i<sigma_.size()-1;i++)
			volvals.push_back(
			sigma_(
			//(volstructure_[i+1] - Settings::Settings::instance().evaluationDate() )/365.0
			(volstructure_[i+1] - referencedate_ )/365.0
			- 0.001));

		return PiecewiseLinearCurve(volperiods_, volvals);
	}

}
