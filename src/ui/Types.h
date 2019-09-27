#ifndef TYPES_H
#define TYPES_H

//Globaly used types

#include <QColor>

#include <osg/Vec2>
#include <osg/Vec3>
#include <osg/Vec4>

#include <array>
#include <algorithm>

namespace Turtle
{
	using OsgColor = osg::Vec4;

	//Helper template to count number of elements in osg vectors
	template <typename V> struct CountVector  {static constexpr int Count() {return V::Count;}};
	template <> struct CountVector<osg::Vec2> { static constexpr int Count() {return 2;}};
	template <> struct CountVector<osg::Vec3> { static constexpr int Count() {return 3;}};
	template <> struct CountVector<osg::Vec4> { static constexpr int Count() {return 4;}};

	template <typename T = void>
	OsgColor fromQColor(const QColor & color)
	{
		return OsgColor
		{
			static_cast<OsgColor::value_type>(color.redF()),
			static_cast<OsgColor::value_type>(color.greenF()),
			static_cast<OsgColor::value_type>(color.blueF()),
			static_cast<OsgColor::value_type>(color.alphaF())
		};
	}


	template <typename T, int N>
	struct Coordinate
	{
		using Type = Coordinate<T,N>;
		using value_type = T;
		static constexpr int Count = N;

		using V = std::array<T, N>;

		Coordinate(V v = {}) : m_v(v) {}

		//Construction from osg vectors
		template <typename V> explicit Coordinate(const V & v) { *this = v; }

		//Construction from Coordinate
		template <typename R> Coordinate(const Coordinate<R,N> & r) { *this = r; }


		//Shorthand versions for simplified construction

		Coordinate(T x, T y) : m_v({x,y}) {}
		Coordinate(T x, T y, T z) : m_v({x,y,z}) {}


		//Utility functions with Coordinate of the same size (but possible different type)

		Type max(const Type & r) const
		{ return transform(r, [](const T & a, const T & b) { return std::max(a,b); }); }

		Type min(const Type & r) const
		{ return transform(r, [](const T & a, const T & b) { return std::min(a,b); }); }


		//Reduction utility functions
		T max() const { return *std::max_element(m_v.cbegin(), m_v.cend()); }
		T min() const { return *std::min_element(m_v.cbegin(), m_v.cend()); }


		//Access functors

		V & v() {return m_v;}
		const V & v() const {return m_v;}

		T & x() {return m_v[0];}
		const T & x() const {return m_v[0];}

		T & y() {return m_v[1];}
		const T & y() const {return m_v[1];}

		T & z() {return m_v[3];}
		const T & z() const {return m_v[3];}

		//Assignment from Coordinate
		template <typename R> Type & operator=(const Coordinate<R,N> & v)
		{
			for (size_t i = 0; i < N; ++i)
				m_v[i] = v.v()[i];

			return *this;
		}

		//Assignment from osg vectors
		template <typename V> Type & operator=(const V & v)
		{
			for (size_t i = 0; i < std::min(N, CountVector<V>::Count()); ++i)
				m_v[i] = v[i];

			return *this;
		}


		//Conversion to osg vectors
		template <typename V> explicit operator V() const
		{
			V v;

			for (size_t i = 0; i < std::min(N, CountVector<V>::Count()); ++i)
				v[i] = m_v[i];

			return v;
		}


		//Unary operators

		Type operator-() const { return transform(std::negate<T>());}


		//Operators with scalars

		Type operator+(const T & r) const { return transform(r, std::plus<T>());}
		Type operator-(const T & r) const { return transform(r, std::minus<T>());}
		Type operator*(const T & r) const { return transform(r, std::multiplies<T>());}
		Type operator/(const T & r) const { return transform(r, std::divides<T>());}


		//In-place operators with scalars

		Type & operator+=(const T & r) { *this = *this + r; return *this; }
		Type & operator-=(const T & r) { *this = *this - r; return *this; }
		Type & operator*=(const T & r) { *this = *this * r; return *this; }
		Type & operator/=(const T & r) { *this = *this / r; return *this; }


		//Comparison operators with scalars

		bool operator==(const T & r) const{ return all_of([&r](const T & e){return e == r;}); }
		bool operator!=(const T & r) const{ return any_of([&r](const T & e){return e != r;}); }
		bool operator<(const T & r) const{ return all_of([&r](const T & e){return e < r;}); }
		bool operator<=(const T & r) const{ return all_of([&r](const T & e){return e <= r;}); }
		bool operator>(const T & r) const{ return all_of([&r](const T & e){return e > r;}); }
		bool operator>=(const T & r) const{ return all_of([&r](const T & e){return e >= r;}); }


		//Operators with Coordinate of the same size (but possible different type)

		template <typename R> Type operator+(const Coordinate<R,N> & r) const { return transform(r, std::plus<T>());}
		template <typename R> Type operator-(const Coordinate<R,N> & r) const { return transform(r, std::minus<T>());}
		template <typename R> Type operator*(const Coordinate<R,N> & r) const { return transform(r, std::multiplies<T>());}
		template <typename R> Type operator/(const Coordinate<R,N> & r) const { return transform(r, std::divides<T>());}



		//In-place operators with Coordinate of the same size (but possible different type)

		template <typename R> Type & operator+=(const Coordinate<R,N> & r) { *this = *this + r; return *this; }
		template <typename R> Type & operator-=(const Coordinate<R,N> & r) { *this = *this - r; return *this; }
		template <typename R> Type & operator*=(const Coordinate<R,N> & r) { *this = *this * r; return *this; }
		template <typename R> Type & operator/=(const Coordinate<R,N> & r) { *this = *this / r; return *this; }


		//Comparison operators with Coordinate of the same size (but possible different type)

		template <typename R> bool operator==(const Coordinate<R,N> & r) const { return equal(r, std::equal_to<T>()); }
		template <typename R> bool operator!=(const Coordinate<R,N> & r) const { return equal(r, std::not_equal_to<T>()); }
		template <typename R> bool operator<(const Coordinate<R,N> & r) const { return equal(r, std::less<T>()); }
		template <typename R> bool operator<=(const Coordinate<R,N> & r) const { return equal(r, std::less_equal<T>()); }
		template <typename R> bool operator>(const Coordinate<R,N> & r) const { return equal(r, std::greater<T>()); }
		template <typename R> bool operator>=(const Coordinate<R,N> & r) const { return equal(r, std::greater_equal<T>()); }


		//Return the result of applying a unary callable on each element of this
		template <typename Operator> Type transform(Operator op) const;

		//Return the result of applying a callable on each element of this and r
		template <typename R, typename Operator> Type transform(const R & r, Operator op) const;

		//Return the result of applying a callable on each element of this and each element of r
		template <typename R, typename Operator> Type transform(const Coordinate<R,N> & r, Operator op) const;

		//Return the result of applying a callable on each element of this and each element of r
		template <typename R, typename Operator> bool equal(const Coordinate<R,N> & r, Operator op) const
		{ return std::equal(m_v.cbegin(), m_v.cend(), r.v().cbegin(), op); }

		template <typename Operator> bool all_of(Operator op) const
		{ return std::all_of(m_v.cbegin(), m_v.cend(), op); }

		template <typename Operator> bool any_of(Operator op) const
		{ return std::any_of(m_v.cbegin(), m_v.cend(), op); }

	private:
		V m_v;
	};

	//Return the result of applying a unary callable on each element of this
	template <typename T, int N>
	template <typename Operator>
	inline
	typename Coordinate<T,N>::Type Coordinate<T,N>::transform(Operator op) const
	{
		Type result;
		std::transform(
					m_v.cbegin(), m_v.cend(),
					result.m_v.begin(),
					[&op](const T & e){return op(e);});
		return result;
	}

	//Return the result of applying a callable on each element of this and r
	template <typename T, int N>
	template <typename R, typename Operator>
	inline
	typename Coordinate<T,N>::Type Coordinate<T,N>::transform(const R & r, Operator op) const
	{
		Type result;
		std::transform(
					m_v.cbegin(), m_v.cend(),
					result.m_v.begin(),
					[&r, &op](const T & e){return op(e,r);});
		return result;
	}

	//Return the result of applying a callable on each element of this and each element of r
	template <typename T, int N>
	template <typename R, typename Operator>
	inline
	typename Coordinate<T,N>::Type Coordinate<T,N>::transform(const Coordinate<R,N> & r, Operator op) const
	{
		Type result;
		std::transform(
					m_v.cbegin(), m_v.cend(),
					r.v().cbegin(),
					result.m_v.begin(),
					op);
		return result;
	}

	//A position in world space
	using PositionPoint = double;
	using Position2D = Coordinate<PositionPoint,2>;
	using Position3D = Coordinate<PositionPoint,3>;
	using Position = Position2D;

	//A position in tile space with a corner as a reference
	using IndexPoint = size_t;
	using Index2D = Coordinate<IndexPoint,2>;
	using Index3D = Coordinate<IndexPoint,3>;
}

#endif // TYPES_H
