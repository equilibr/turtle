#ifndef TYPES_H
#define TYPES_H

//Globaly used types

#include <array>
#include <algorithm>

namespace Turtle
{
	template <typename T, int N>
	struct Coordinate
	{
		using Type = Coordinate<T,N>;
		using value_type = T;
		static constexpr size_t Count = N;

		using V = std::array<T, N>;

		Coordinate(V v = {}) : m_v(v) {}

		template <typename R> Coordinate(const Coordinate<R,N> & r)
		{std::generate(m_v.begin(), m_v.end(),[&r, i = 0]() mutable {return r.v()[i++];});}


		//Shorthand versions for simplified construction

		Coordinate(T x, T y) : m_v({x,y}) {}
		Coordinate(T x, T y, T z) : m_v({x,y,z}) {}


		//Access functors

		V & v() {return m_v;}
		const V & v() const {return m_v;}

		T & x() {return m_v[0];}
		const T & x() const {return m_v[0];}

		T & y() {return m_v[1];}
		const T & y() const {return m_v[1];}

		T & z() {return m_v[3];}
		const T & z() const {return m_v[3];}


		//Unary operators

		Type operator-() const { return apply(std::negate<T>());}


		//Operators with scalars

		Type operator+(const T & r) const { return apply(r, std::plus<T>());}
		Type operator-(const T & r) const { return apply(r, std::minus<T>());}
		Type operator*(const T & r) const { return apply(r, std::multiplies<T>());}
		Type operator/(const T & r) const { return apply(r, std::divides<T>());}


		//Operators with Coordinate of the same size (but possible different type)

		template <typename R> Type operator+(const Coordinate<R,N> & r) const { return apply(r, std::plus<T>());}
		template <typename R> Type operator-(const Coordinate<R,N> & r) const { return apply(r, std::minus<T>());}
		template <typename R> Type operator*(const Coordinate<R,N> & r) const { return apply(r, std::multiplies<T>());}
		template <typename R> Type operator/(const Coordinate<R,N> & r) const { return apply(r, std::divides<T>());}


		//Return the result of applying a unary callable on each element of this
		template <typename Operator> Type apply(Operator op) const;

		//Return the result of applying a callable on each element of this and r
		template <typename R, typename Operator> Type apply(const R & r, Operator op) const;

		//Return the result of applying a callable on each element of this and each element of r
		template <typename R, typename Operator> Type apply(const Coordinate<R,N> & r, Operator op) const;

	private:
		V m_v;
	};

	//Return the result of applying a unary callable on each element of this
	template <typename T, int N>
	template <typename Operator>
	inline
	typename Coordinate<T,N>::Type Coordinate<T,N>::apply(Operator op) const
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
	typename Coordinate<T,N>::Type Coordinate<T,N>::apply(const R & r, Operator op) const
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
	typename Coordinate<T,N>::Type Coordinate<T,N>::apply(const Coordinate<R,N> & r, Operator op) const
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
}

#endif // TYPES_H
