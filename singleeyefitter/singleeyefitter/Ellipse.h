#ifndef _ELLIPSE_H_
#define _ELLIPSE_H_

#include <boost/math/constants/constants.hpp>
#include <Eigen/Core>

namespace singleeyefitter {

    template<typename T>
    class Conic;

    template<typename T>
    class Ellipse2D {
    public:
        typedef T Scalar;
        typedef Eigen::Matrix<Scalar, 2, 1> Vector;
        Vector centre;
        Scalar major_radius;
        Scalar minor_radius;
        Scalar angle;

        Ellipse2D()
            : centre(0, 0), major_radius(0), minor_radius(0), angle(0)
        {
        }
        template<typename Derived>
        Ellipse2D(const Eigen::EigenBase<Derived>& centre, Scalar major_radius, Scalar minor_radius, Scalar angle)
            : centre(centre), major_radius(major_radius), minor_radius(minor_radius), angle(angle)
        {
			validateEllipse();
        }
        Ellipse2D(Scalar x, Scalar y, Scalar major_radius, Scalar minor_radius, Scalar angle)
            : centre(x, y), major_radius(major_radius), minor_radius(minor_radius), angle(angle)
        {
			validateEllipse();
        }


        template<typename U>
        explicit Ellipse2D(const Conic<U>& conic) {
            using std::atan2;
            using std::sin;
            using std::cos;
            using std::sqrt;
            using std::abs;

            angle = 0.5*atan2(conic.B, conic.A - conic.C);
            auto cost = cos(angle);
            auto sint = sin(angle);
            auto sin_squared = sint * sint;
            auto cos_squared = cost * cost;

            auto Ao = conic.F;
            auto Au = conic.D * cost + conic.E * sint;
            auto Av = -conic.D * sint + conic.E * cost;
            auto Auu = conic.A * cos_squared + conic.C * sin_squared + conic.B * sint * cost;
            auto Avv = conic.A * sin_squared + conic.C * cos_squared - conic.B * sint * cost;

            // ROTATED = [Ao Au Av Auu Avv]

            auto tuCentre = -Au / (2.0*Auu);
            auto tvCentre = -Av / (2.0*Avv);
            auto wCentre = Ao - Auu*tuCentre*tuCentre - Avv*tvCentre*tvCentre;

            centre[0] = tuCentre * cost - tvCentre * sint;
            centre[1] = tuCentre * sint + tvCentre * cost;

            major_radius = sqrt(abs(-wCentre / Auu));
            minor_radius = sqrt(abs(-wCentre / Avv));

			validateEllipse();
        }

        EIGEN_MAKE_ALIGNED_OPERATOR_NEW_IF((sizeof(Vector) % 16) == 0)

        Vector major_axis() const {
            using std::sin;
            using std::cos;
            return Vector(major_radius*sin(angle), major_radius*cos(angle));
        }
        Vector minor_axis() const {
            using std::sin;
            using std::cos;
            return Vector(-minor_radius*cos(angle), minor_radius*sin(angle));
        }

        static const Ellipse2D Null;

		template<typename T>
		double similarity(const Ellipse2D<T>& e, double sig_pow2 = 1.0) {
			const double kAngleSig = 5.0 / 180.0*boost::math::double_constants::pi;
			
//			std::cout << "E1: " << e.minor_radius << " " << e.major_radius << " " << e.angle << std::endl;
//			std::cout << "E2: " << minor_radius << " " << major_radius << " " << angle << std::endl;
			const double kRate1 = e.minor_radius / e.major_radius;
			const double kRate2 = minor_radius / major_radius;
			const double kRateDiff = (kRate1 - kRate2);
			
			const double kS = (e.minor_radius + e.major_radius)/2;

			const double kdx = (e.centre[0]-centre[0])/kS;
			const double kdy = (e.centre[1]-centre[1])/kS;
			const double kd = kdx*kdx + kdy*kdy;
			
			const double kA = (minor_radius - e.minor_radius) / kS;
			const double kB = (major_radius - e.major_radius) / kS;
			const double kC = (angle        - e.angle) / kAngleSig;
			const double kR = e.minor_radius/e.major_radius;
			// plot(1/(1+exp(25*x-10) )) // close to zero at x=1 and become almost 1 less than 0.5
			/// const double kD = 1.0 / (1 + exp(25 * kR - 10));// too loose
			const double kD = 10.0 * exp(-5.0 * kR);

			const double kE = 1-(minor_radius*major_radius)/(e.minor_radius*e.major_radius);
			const double kA2 = kRateDiff*kRateDiff*10.0;
			const double kC2 = kD*kC*kC;
			const double kE2 = kE*kE;
			//std::cout << "kRateDiff: " << kRateDiff << std::endl;
			//std::cout << "kC:        " << kC << std::endl;
			//std::cout << "Angle:     " << angle << std::endl;
			//std::cout << "Result:    " <<kA2  << " " << kC2 << std::endl;
			return exp(-(kd + kE2 + kA2  + kC2) / (2.0*sig_pow2));
			//const double kAngleSig = 5.0 / 180.0*boost::math::double_constants::pi;
			//const double kA = (minor_radius - e.minor_radius) / e.major_radius;
			//const double kB = (major_radius - e.major_radius) / e.major_radius;
			//return exp(
			//	-pow(kA, 2.0) * pow(kB, 2.0) *
			//	pow((angle - e.angle) / kAngleSig, 2.0)
			//	/ (2.0*sig_pow2)
			//	);// / sqrt(2.0*boost::math::double_constants::pi*sig_pow2);
		}


    private:
        // Safe bool stuff
        typedef void (Ellipse2D::*safe_bool_type)() const;
        void this_type_does_not_support_comparisons() const {}

		void validateEllipse(){
            if (major_radius < minor_radius) {
                std::swap(major_radius, minor_radius);
                angle = angle + boost::math::double_constants::pi / 2;
            }
            if (angle > boost::math::double_constants::pi)
                angle = angle - boost::math::double_constants::pi;

		}
    public:
        operator safe_bool_type() const {
            return *this != Null ? &Ellipse2D::this_type_does_not_support_comparisons : 0;
        }
    };

    template<typename Scalar>
    const Ellipse2D<Scalar> Ellipse2D<Scalar>::Null = Ellipse2D<Scalar>();

    template<typename T, typename U>
    bool operator==(const Ellipse2D<T>& el1, const Ellipse2D<U>& el2) {
        return el1.centre[0] == el2.centre[0] &&
            el1.centre[1] == el2.centre[1] &&
            el1.major_radius == el2.major_radius &&
            el1.minor_radius == el2.minor_radius &&
            el1.angle == el2.angle;
    }
    template<typename T, typename U>
    bool operator!=(const Ellipse2D<T>& el1, const Ellipse2D<U>& el2) {
        return !(el1 == el2);
    }

    template<typename T>
    std::ostream& operator<< (std::ostream& os, const Ellipse2D<T>& ellipse) {
        return os << "Ellipse { centre: (" << ellipse.centre[0] << "," << ellipse.centre[1] << "), a: " <<
            ellipse.major_radius << ", b: " << ellipse.minor_radius << ", theta: " << (ellipse.angle / boost::math::double_constants::pi) << "pi }";
    }

    template<typename T, typename U>
    Ellipse2D<T> scaled(const Ellipse2D<T>& ellipse, U scale) {
        return Ellipse2D<T>(
            ellipse.centre[0].a,
            ellipse.centre[1].a,
            ellipse.major_radius.a,
            ellipse.minor_radius.a,
            ellipse.angle.a);
    }

    template<class Scalar, class Scalar2>
    inline Eigen::Matrix<typename std::common_type<Scalar, Scalar2>::type, 2, 1> pointAlongEllipse(const Ellipse2D<Scalar>& el, Scalar2 t)
    {
        using std::sin;
        using std::cos;
        auto xt = el.centre.x() + el.major_radius*cos(el.angle)*cos(t) - el.minor_radius*sin(el.angle)*sin(t);
        auto yt = el.centre.y() + el.major_radius*sin(el.angle)*cos(t) + el.minor_radius*cos(el.angle)*sin(t);
        return Eigen::Matrix<typename std::common_type<Scalar, Scalar2>::type, 2, 1>(xt, yt);
    }

}

/*namespace matlab {
template<typename T>
struct matlab_traits<typename Ellipse<T>, typename std::enable_if<
    matlab::internal::mxHelper<T>::exists
>::type>
{
    static std::unique_ptr<mxArray, decltype(&mxDestroyArray)> createMxArray(const Ellipse<T>& ellipse) throw() {
        std::unique_ptr<mxArray, decltype(&mxDestroyArray)> arr(
            internal::mxHelper<T>::createMatrix(1, 5),
            mxDestroyArray);
        T* data = (T*)mxGetData(arr.get());
        data[0] = ellipse.centre[0];
        data[1] = ellipse.centre[1];
        data[2] = ellipse.major_radius;
        data[3] = ellipse.minor_radius;
        data[4] = ellipse.angle;
        return arr;
    }
};
}*/

#endif
