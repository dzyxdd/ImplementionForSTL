#pragma once
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <string>
#include <concepts>
#include <typeinfo>
#include "Config.h"
#include <compare>
#include "Utility.hpp"

#define _DZYCXX_ARRAY_INDEX_OUT_OF_BOUNDS(_Index) static_assert(_Index < _Size, "array index out of bounds")

template<class _Tp, std::size_t _Size>
struct Array {

	using value_type = _Tp;
	using size_type = std::size_t;
	using difference_type = std::ptrdiff_t;
	using reference = value_type&;
	using const_reference = value_type const&;
	using pointer = value_type*;
	using const_pointer = value_type const*;
	using iterator = pointer;
	using const_iterator = const_pointer;
	using reverse_iterator = std::reverse_iterator<iterator>;
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;

	value_type _Element[_Size];

	_DZYCXX_CONSTEXPR17 reference at(size_type __index) {
		if (__index >= _Size)
			_DZYCXX_OUT_OF_RANGE(__index);
		return _Element[__index];
	}
	_DZYCXX_CONSTEXPR14 const_reference at(size_type __index) const {
		if (__index >= _Size)
			_DZYCXX_OUT_OF_RANGE(__index);
		return _Element[__index];
	}

	_DZYCXX_CONSTEXPR17 reference operator[](size_type __index) noexcept {
		return _Element[__index];
	}
	_DZYCXX_CONSTEXPR14 const_reference operator[](size_type __index) const noexcept {
		return _Element[__index];
	}

	_DZYCXX_CONSTEXPR17 reference front() noexcept {
		return _Element[0];
	}
	_DZYCXX_CONSTEXPR14 const_reference front() const noexcept {
		return _Element[0];
	}

	_DZYCXX_CONSTEXPR17 reference back() noexcept {
		return _Element[_Size - 1];
	}
	_DZYCXX_CONSTEXPR14 const_reference back() const noexcept {
		return _Element[_Size - 1];
	}

	_DZYCXX_CONSTEXPR17 pointer data() noexcept {
		return _Element;
	}
	_DZYCXX_CONSTEXPR14 const_pointer data() const noexcept {
		return _Element;
	}

	_DZYCXX_CONSTEXPR17 iterator begin() noexcept {
		return iterator{ _Element };
	}
	_DZYCXX_CONSTEXPR17 const_iterator begin() const noexcept {
		return const_iterator{ _Element };
	}
	_DZYCXX_CONSTEXPR17 const_iterator cbegin() const noexcept {
		return begin();
	}

	_DZYCXX_CONSTEXPR17 iterator end() noexcept {
		return iterator{ _Element + _Size };
	}
	_DZYCXX_CONSTEXPR17 const_iterator end() const noexcept {
		return const_iterator{ _Element + _Size };
	}
	_DZYCXX_CONSTEXPR17 const_iterator cend() const noexcept {
		return end();
	}

	_DZYCXX_CONSTEXPR17 reverse_iterator rbegin() noexcept {
		return reverse_iterator{ end() };
	}
	_DZYCXX_CONSTEXPR17 const_reverse_iterator rbegin() const noexcept {
		return const_reverse_iterator{ end() };
	}
	_DZYCXX_CONSTEXPR17 const_reverse_iterator crbegin() const noexcept {
		return rbegin();
	}

	_DZYCXX_CONSTEXPR17 reverse_iterator rend() noexcept {
		return reverse_iterator{ begin() };
	}
	_DZYCXX_CONSTEXPR17 const_reverse_iterator rend() const noexcept {
		return const_reverse_iterator{ begin() };
	}
	_DZYCXX_CONSTEXPR17 const_reverse_iterator crend() const noexcept {
		return rend();
	}

	_DZYCXX_NODISCARD constexpr bool empty() const noexcept {
		return false;
	}

	_DZYCXX_NODISCARD constexpr size_type size() const noexcept {
		return _Size;
	}

	_DZYCXX_NODISCARD constexpr size_type max_size() const noexcept {
		return _Size;
	}

	_DZYCXX_CONSTEXPR20 void fill(const_reference __value)noexcept(std::is_nothrow_copy_assignable_v<_Tp>) {
		for (auto& __elem : _Element) {
			__elem = __value;
		}
	}

	_DZYCXX_CONSTEXPR20 void swap(Array& __other)
#if !_DZYCXX_HASCXX17
		noexcept(noexcept(swap(std::declval<T&>(), std::declval<T&>())))
#else
		noexcept(std::is_nothrow_swappable_v<_Tp>)
#endif
	{
		for (size_type __i = 0; __i < _Size; __i++) {
			std::swap(_Element[__i], __other._Element[__i]);
		}
	}

};

struct _Empty_array_element {};

template<class _Tp>
struct Array<_Tp, 0> {

	using value_type = _Tp;
	using size_type = std::size_t;
	using difference_type = std::ptrdiff_t;
	using reference = _Tp&;
	using const_reference = _Tp const&;
	using pointer = _Tp*;
	using const_pointer = _Tp const*;
	using iterator = pointer;
	using const_iterator = const_pointer;
	using reverse_iterator = std::reverse_iterator<iterator>;
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;


	std::conditional<std::disjunction<std::is_default_constructible<value_type>, std::_Is_implicitly_default_constructible<value_type>>::value, value_type, _Empty_array_element>::type _Element[1]{};

	_DZYCXX_CONSTEXPR17 reference at(size_type __index) {
		_DZYCXX_OUT_OF_RANGE(__index);
	}
	_DZYCXX_CONSTEXPR14 const_reference at(size_type __index) const {
		_DZYCXX_OUT_OF_RANGE(__index);
	}

	_DZYCXX_CONSTEXPR17 reference operator[](size_type __index) noexcept {
		_DZYCXX_UNREACHABLE();
	}
	_DZYCXX_CONSTEXPR14 const_reference operator[](size_type __index) const noexcept {
		_DZYCXX_UNREACHABLE();
	}

	_DZYCXX_CONSTEXPR17 reference front() noexcept {
		_DZYCXX_UNREACHABLE();
	}
	_DZYCXX_CONSTEXPR14 const_reference front() const noexcept {
		_DZYCXX_UNREACHABLE();
	}

	_DZYCXX_CONSTEXPR17 reference back() noexcept {
		_DZYCXX_UNREACHABLE();
	}
	_DZYCXX_CONSTEXPR14 const_reference back() const noexcept {
		_DZYCXX_UNREACHABLE();
	}

	_DZYCXX_CONSTEXPR17 pointer data() noexcept {
		return nullptr;
	}
	_DZYCXX_CONSTEXPR14 const_pointer data() const noexcept {
		return nullptr;
	}

	_DZYCXX_CONSTEXPR17 iterator begin() noexcept {
		return nullptr;
	}
	_DZYCXX_CONSTEXPR17 const_iterator begin() const noexcept {
		return nullptr;
	}
	_DZYCXX_CONSTEXPR17 const_iterator cbegin() const noexcept {
		return begin();
	}

	_DZYCXX_CONSTEXPR17 iterator end() noexcept {
		return nullptr;
	}
	_DZYCXX_CONSTEXPR17 const_iterator end() const noexcept {
		return nullptr;
	}
	_DZYCXX_CONSTEXPR17 const_iterator cend() const noexcept {
		return end();
	}

	_DZYCXX_CONSTEXPR17 reverse_iterator rbegin() noexcept {
		return nullptr;
	}
	_DZYCXX_CONSTEXPR17 const_reverse_iterator rbegin() const noexcept {
		return nullptr;
	}
	_DZYCXX_CONSTEXPR17 const_reverse_iterator crbegin() const noexcept {
		return rbegin();
	}

	_DZYCXX_CONSTEXPR17 reverse_iterator rend() noexcept {
		return nullptr;
	}
	_DZYCXX_CONSTEXPR17 const_reverse_iterator rend() const noexcept {
		return nullptr;
	}
	_DZYCXX_CONSTEXPR17 const_reverse_iterator crend() const noexcept {
		return rend();
	}

	_DZYCXX_NODISCARD constexpr bool empty() const noexcept {
		return true;
	}

	_DZYCXX_NODISCARD constexpr size_type size() const noexcept {
		return 0;
	}

	_DZYCXX_NODISCARD constexpr size_type max_size() const noexcept {
		return 0;
	}

	_DZYCXX_CONSTEXPR20 void fill(const_reference)noexcept {}

	_DZYCXX_CONSTEXPR20 void swap(Array&) noexcept {}
};

template<class _Tp, std::size_t _Size>
_DZYCXX_NODISCARD _DZYCXX_CONSTEXPR20 bool operator==(Array<_Tp, _Size>const& __left, Array<_Tp, _Size>const& __right) {
	return std::equal(__left.begin(), __left.end(), __right.begin());
}

#if defined _DZYCXX_HASCXX20
template<class _Tp, std::size_t _Size>
	requires (std::three_way_comparable<_Tp> || _DZYCXX_BOOL_TESTABLE(_Tp))
_DZYCXX_NODISCARD _DZYCXX_CONSTEXPR20 auto operator<=>(Array<_Tp, _Size> const& __left, Array<_Tp, _Size> const& __right) {
	if _DZYCXX_CONSTEXPR17(std::three_way_comparable<_Tp>) {
		return std::lexicographical_compare_three_way(__left.begin(), __left.end(), __right.begin(), __right.end());
	}
}
#else

template<class _Tp, std::size_t _Size>
_DZYCXX_NODISCARD _DZYCXX_CONSTEXPR20 bool operator!=(Array<_Tp, _Size>const& __left, Array<_Tp, _Size>const& __right) {
	return !(__left == __right);
}

template<class _Tp, std::size_t _Size>
_DZYCXX_NODISCARD _DZYCXX_CONSTEXPR20 bool operator<(Array<_Tp, _Size> const& __left, Array<_Tp, _Size> const& __right) {
	return std::lexicographical_compare(__left.begin(), __left.end(), __right.begin(), __right.end());
}

template<class _Tp, std::size_t _Size>
_DZYCXX_NODISCARD _DZYCXX_CONSTEXPR20 bool operator>(Array<_Tp, _Size> const& __left, Array<_Tp, _Size> const& __right) {
	return __right < __left;
}

template<class _Tp, std::size_t _Size>
_DZYCXX_NODISCARD _DZYCXX_CONSTEXPR20 bool operator<=(Array<_Tp, _Size> const& __left, Array<_Tp, _Size> const& __right) {
	return !(__right < __left);
}

template<class _Tp, std::size_t _Size>
_DZYCXX_NODISCARD _DZYCXX_CONSTEXPR20 bool operator>=(Array<_Tp, _Size> const& __left, Array<_Tp, _Size> const& __right) {
	return !(__left < __right);
}

#endif

template< std::size_t _Index, class _Tp, std::size_t _Size >
_DZYCXX_NODISCARD _DZYCXX_CONSTEXPR14 _Tp& get(Array<_Tp, _Size>& __array) noexcept {
	static_assert(_Index < _Size, "array index out of bounds");
	return __array._Element[_Index];
}

template< std::size_t _Index, class _Tp, std::size_t _Size >
_DZYCXX_NODISCARD _DZYCXX_CONSTEXPR14 _Tp&& get(Array<_Tp, _Size>&& __array) noexcept {
	static_assert(_Index < _Size, "array index out of bounds");
	return std::move(__array._Element[_Index]);
}

template< std::size_t _Index, class _Tp, std::size_t _Size >
_DZYCXX_NODISCARD _DZYCXX_CONSTEXPR14 _Tp const& get(Array<_Tp, _Size>const& __array) noexcept {
	static_assert(_Index < _Size, "array index out of bounds");
	return __array._Element[_Index];
}

template< std::size_t _Index, class _Tp, std::size_t _Size >
_DZYCXX_NODISCARD _DZYCXX_CONSTEXPR14 _Tp const&& get(Array<_Tp, _Size>const&& __array) noexcept {
	static_assert(_Index < _Size, "array index out of bounds");
	return std::move(__array._Element[_Index]);
}

#if defined _DZYCXX_HASCXX20
template< class _Tp, std::size_t _Size>
	requires(_Size == 0 || std::is_swappable_v<_Tp>)
#elif defined _DZYCXX_HASCXX17
template< class _Tp, std::size_t _Size, std::enable_if_t<_Size == 0 || std::is_swappable_v<_Tp>, int> = 0>
#else
template< class _Tp, std::size_t _Size>
#endif
_DZYCXX_CONSTEXPR20 void swap(std::array<_Tp, _Size>&__left, std::array<_Tp, _Size>&__right)
#if defined _DZYCXX_HASCXX17
noexcept(noexcept(__left.swap(__right)))
#endif	
{
	__left.swap(__right);
}

#if _DZYCXX_HASCXX20
#define __cpp_lib_to_array 201907L
template<class _Tp, std::size_t _Size, std::size_t... _Index>
_DZYCXX_NODISCARD constexpr Array<std::remove_cv_t<_Tp>, _Size> __to_array_lvalue_impl(_Tp(&__array)[_Size], std::index_sequence<_Index...>) {
	return { {__array[_Index]...} };
}

template<class _Tp, std::size_t _Size, std::size_t... _Index>
_DZYCXX_NODISCARD constexpr Array<std::remove_cv_t<_Tp>, _Size> __to_array_rvalue_impl(_Tp(&& __array)[_Size], std::index_sequence<_Index...>) {
	return { {std::move(__array[_Index])...} };
}

template< class _Tp, std::size_t _Size>
_DZYCXX_NODISCARD constexpr Array<std::remove_cv_t<_Tp>, _Size> to_array(_Tp(&__array)[_Size]) {
	static_assert(!std::is_array_v<_Tp>);
	static_assert(std::is_constructible_v<_Tp, _Tp&>);
	return __to_array_lvalue_impl(__array, std::make_index_sequence<_Size>{});
}

template< class _Tp, std::size_t _Size>
_DZYCXX_NODISCARD constexpr Array<std::remove_cv_t<_Tp>, _Size> to_array(_Tp(&& __array)[_Size]) {
	static_assert(!std::is_array_v<_Tp>);
	static_assert(std::is_move_constructible_v<_Tp>);
	return __to_array_rvalue_impl(std::move(__array), std::make_index_sequence<_Size>{});
}
#endif

template<class _Tp, std::size_t _Size>
struct tuple_size<Array<_Tp, _Size>> : std::integral_constant<size_t, _Size> {};

template <std::size_t _Index, class _Tp, std::size_t _Size>
struct tuple_element<_Index, Array<_Tp, _Size>> {
	_DZYCXX_ARRAY_INDEX_OUT_OF_BOUNDS(_Index);
	using type = _Tp;
};

#if _DZYCXX_HASCXX17
template<class _Tp, class..._Ts>
Array(_Tp, _Ts...) -> Array<_Tp, 1 + sizeof...(_Ts)>;
#endif
