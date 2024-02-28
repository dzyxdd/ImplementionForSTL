#pragma once

template <class _Tp>
struct tuple_size;

#if _DZYCXX_HASCXX17
template <class _Tp>
_DZYCXX_INLINE constexpr std::size_t tuple_size_v = tuple_size<_Tp>::value;
#endif

template<class _Tp>
struct tuple_size< const _Tp > : std::integral_constant<std::size_t, std::tuple_size<_Tp>::value> {};

#if !_DZYCXX_HASCXX20
template<class _Tp>
struct tuple_size<volatile _Tp> : std::integral_constant<std::size_t, std::tuple_size<_Tp>::value> {};

template<class _Tp>
struct tuple_size<const volatile _Tp> : std::integral_constant<std::size_t, std::tuple_size<_Tp>::value> {};
#endif

template <std::size_t _Index, class _Tp>
struct tuple_element;

#if _DZYCXX_HASCXX14
template <std::size_t _Index, class _Tp>
using tuple_element_t = typename tuple_element<_Index, _Tp>::type;
#endif

template<std::size_t _Index, typename _Tp>
struct tuple_element<_Index, const _Tp> {
	using type = typename std::add_const<typename tuple_element<_Index, _Tp>::type>::type;
};

#if !_DZYCXX_HASCXX20
template<std::size_t _Index, typename _Tp>
struct tuple_element<_Index, volatile _Tp> {
	using type = typename std::add_volatile<typename tuple_element<_Index, _Tp>::type>::type;
};

template<std::size_t _Index, typename _Tp>
struct tuple_element<_Index, const volatile _Tp> {
	using type = typename std::add_cv<typename tuple_element<_Index, _Tp>::type>::type;
};
#endif