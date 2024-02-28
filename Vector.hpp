#pragma once

#include "Config.h"

template<class _Tp, class _Allocator = std::allocator<_Tp>>
struct Vector {
	using value_type = _Tp;
	using allocator_type = _Allocator;
	using size_type = std::size_t;
	using difference_type = std::ptrdiff_t;
	using reference = value_type&;
	using const_reference = value_type const&;
	using pointer = value_type*;
	using const_pointer = value_type const*;
	using iterator = value_type*;
	using const_iterator = value_type const*;
	using reverse_iterator = std::reverse_iterator<iterator>;
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;

#if _DZYCXX_HASCXX20
	static_assert(std::is_same_v<value_type, typename allocator_type::value_type>, "Allocator::value_type must be same as value_type");
#endif

	pointer _Begin;
	pointer _End;
	pointer _Capacity;
	allocator_type _Alloc;

	_DZYCXX_CONSTEXPR20 Vector()
#if _DZYCXX_HASCXX17
		noexcept(noexcept(allocator_type()))
#endif
		: _Begin(nullptr), _End(nullptr), _Capacity(nullptr), _Alloc() {}

	_DZYCXX_CONSTEXPR20 explicit Vector(allocator_type const& __alloc)noexcept
		: _Begin(nullptr), _End(nullptr), _Capacity(nullptr), _Alloc(__alloc) {}

	_DZYCXX_CONSTEXPR20 Vector(size_type __count, value_type const& __value, allocator_type const& __alloc = allocator_type())
		: _Alloc(__alloc) {
		_Begin = std::allocator_traits<allocator_type>::allocate(_Alloc, __count);
		_Capacity = _End = _Begin + __count;
		std::uninitialized_fill_n(_Begin, __count, __value);
	}

	_DZYCXX_CONSTEXPR20 explicit Vector(size_type __count, allocator_type const& __alloc = allocator_type())
		:Vector(__count, 0, __alloc) {}

	template<class _InputIterator>
		requires(std::input_iterator<_InputIterator>)
	_DZYCXX_CONSTEXPR20 Vector(_InputIterator __first, _InputIterator __last, allocator_type const& __alloc = allocator_type())
		: _Alloc(__alloc) {
		_Begin = std::allocator_traits<allocator_type>::allocate(_Alloc, __last - __first);
		_Capacity = _End = std::uninitialized_copy(__first, __last, _Begin);
	}

	_DZYCXX_CONSTEXPR20 Vector(Vector const& __other)
		: _Alloc(__other._Alloc) {
		if (__other.size()) {
			auto __alloc = std::allocator_traits<allocator_type>::select_on_container_copy_construction(__other._Alloc);
			//_Begin = __alloc.alloate(__other.size());
			_Begin = std::allocator_traits<decltype(__alloc)>::allocate(__alloc, __other.size());
			_End = _Capacity = std::uninitialized_copy(__other._Begin, __other._End, _Begin);
		}
		else {
			_Begin = _End = _Capacity = nullptr;
		}

	}

	_DZYCXX_CONSTEXPR20 Vector(Vector const& __other, allocator_type const& __alloc)
		: _Alloc(__alloc) {
		if (__other.size()) {
			_Begin = std::allocator_traits<allocator_type>::allocate(_Alloc, __other.size());
			_End = _Capacity = std::uninitialized_copy(__other._Begin, __other._End, _Begin);
		}
		else {
			_Begin = _End = _Capacity = nullptr;
		}
	}

	_DZYCXX_CONSTEXPR20 Vector(Vector&& __other)noexcept
		:_Begin(__other._Begin), _End(__other._End), _Capacity(__other._Capacity), _Alloc(std::move(__other._Alloc)) {
		__other._Begin = __other._End = __other._Capacity = nullptr;
	}

	_DZYCXX_CONSTEXPR20 Vector(Vector&& __other, allocator_type const& __alloc) {
		if (__alloc == __other._Alloc) {
			_Begin = __other._Begin;
			_End = __other._End;
			_Capacity = __other._Capacity;
			__other._Begin = __other._End = __other._Capacity = nullptr;
		}
		else {
			if (__other.size()) {
				_Begin = std::allocator_traits<allocator_type>::allocate(__alloc, __other.size());
				_End = _Capacity = std::uninitialized_move(__other._Begin, __other._End, _Begin);
			}
			else {
				_Begin = _End = _Capacity = nullptr;
			}
		}
	}

	_DZYCXX_CONSTEXPR20 Vector(std::initializer_list<value_type> __init, allocator_type const& __alloc = allocator_type())
		: Vector(__init.begin(), __init.end(), __alloc) {}

	_DZYCXX_CONSTEXPR20 ~Vector() {
		if (capacity()) {
			std::destroy(_Begin, _End);
			std::allocator_traits<allocator_type>::deallocate(_Alloc, _Begin, capacity());
		}
	}

	_DZYCXX_CONSTEXPR20 Vector& operator=(Vector const& __other) {
		if (this != &__other) {
			reserve(__other.size());
			_End = _Capacity = std::uninitialized_copy(__other._Begin, __other._End, _Begin);
		}
		return *this;
	}

	_DZYCXX_CONSTEXPR20 Vector& operator=(Vector&& __other)
		noexcept(std::allocator_traits<allocator_type>::propagate_on_container_move_assignment::value
		|| std::allocator_traits<allocator_type>::is_always_equal::value) {
		if (this != &__other) {
			clear();
			if (capacity()) {
				std::allocator_traits<allocator_type>::deallocate(_Alloc, _Begin, capacity());
			}
			_Begin = __other._Begin;
			_End = __other._End;
			_Capacity = __other._Capacity;
			_Alloc = std::move(__other._Alloc);
			__other._Begin = __other._End = __other._Capacity = nullptr;
		}
		return *this;
	}

	_DZYCXX_CONSTEXPR20 Vector& operator=(std::initializer_list<value_type> __init) {
		assign(__init.begin(), __init.end());
		return *this;
	}

	_DZYCXX_CONSTEXPR20 void assign(size_type __count, value_type const& __value) {
		if (__count > capacity()) {
			Vector<value_type, allocator_type> __temp(__count, __value, _Alloc);
			__temp.swap(*this);
		}
		else if (__count > size()) {
			std::fill(_Begin, _End, __value);
			_End = std::uninitialized_fill_n(_End, __count - size(), __value);
		}
		else {
			//std::fill_n(_Begin, __count, __value);
			erase(std::fill_n(_Begin, __count, __value), _End);
		}
	}

	template<class _InputIterator>
		requires(std::input_iterator<_InputIterator>)
	_DZYCXX_CONSTEXPR20 void assign(_InputIterator __first, _InputIterator __last) {
		clear();
		reserve(__last - __first);
		_End = _Capacity = std::uninitialized_copy(__first, __last, _Begin);
	}

	_DZYCXX_CONSTEXPR20 void assign(std::initializer_list<value_type> __init) {
		assign(__init.begin(), __init.end());
	}

	_DZYCXX_CONSTEXPR20 allocator_type get_allocator()const noexcept {
		return _Alloc;
	}

	_DZYCXX_CONSTEXPR20 reference at(size_type __pos) {
		if (__pos >= size())
			_DZYCXX_OUT_OF_RANGE(__pos);
		return _Begin[__pos];
	}

	_DZYCXX_CONSTEXPR20 const_reference at(size_type __pos)const {
		if (__pos >= size())
			_DZYCXX_OUT_OF_RANGE(__pos);
		return _Begin[__pos];
	}

	_DZYCXX_CONSTEXPR20 reference operator[](size_type __pos) {
		return _Begin[__pos];
	}

	_DZYCXX_CONSTEXPR20 const_reference operator[](size_type __pos)const {
		return _Begin[__pos];
	}

	_DZYCXX_CONSTEXPR20 reference front() {
		return *_Begin;
	}

	_DZYCXX_CONSTEXPR20 const_reference front()const {
		return *_Begin;
	}

	_DZYCXX_CONSTEXPR20 reference back() {
		return *(_End - 1);
	}

	_DZYCXX_CONSTEXPR20 const_reference back()const {
		return *(_End - 1);
	}

	_DZYCXX_CONSTEXPR20 pointer data()noexcept {
		if (size() == 0)
			return nullptr;
		return _Begin;
	}

	_DZYCXX_CONSTEXPR20 const_pointer data()const noexcept {
		if (size() == 0)
			return nullptr;
		return _Begin;
	}

	_DZYCXX_CONSTEXPR20 iterator begin()noexcept {
		return _Begin;
	}

	_DZYCXX_CONSTEXPR20 const_iterator begin()const noexcept {
		return _Begin;
	}

	_DZYCXX_CONSTEXPR20 const_iterator cbegin()const noexcept {
		return begin();
	}

	_DZYCXX_CONSTEXPR20 iterator end()noexcept {
		return _End;
	}

	_DZYCXX_CONSTEXPR20 const_iterator end()const noexcept {
		return _End;
	}

	_DZYCXX_CONSTEXPR20 const_iterator cend()const noexcept {
		return end();
	}

	_DZYCXX_CONSTEXPR20 reverse_iterator rbegin()noexcept {
		return reverse_iterator(_End);
	}

	_DZYCXX_CONSTEXPR20 const_reverse_iterator rbegin()const noexcept {
		return const_reverse_iterator(_End);
	}

	_DZYCXX_CONSTEXPR20 const_reverse_iterator crbegin()const noexcept {
		return rbegin();
	}

	_DZYCXX_CONSTEXPR20 reverse_iterator rend()noexcept {
		return reverse_iterator(_Begin);
	}

	_DZYCXX_CONSTEXPR20 const_reverse_iterator rend()const noexcept {
		return const_reverse_iterator(_Begin);
	}

	_DZYCXX_CONSTEXPR20 const_reverse_iterator crend()const noexcept {
		return rend();
	}

	_DZYCXX_NODISCARD _DZYCXX_CONSTEXPR20 bool empty()const noexcept {
		return _Begin == _End;
	}

	_DZYCXX_CONSTEXPR20 size_type size()const noexcept {
		return _End - _Begin;
	}

	_DZYCXX_CONSTEXPR20 size_type max_size()const noexcept {
		return std::max(std::allocator_traits<allocator_type>::max_size(_Alloc), std::numeric_limits<size_type>::max() / sizeof(value_type));
	}

	_DZYCXX_CONSTEXPR20 void reserve(size_type __newCapacity) {
		if (__newCapacity > max_size())
			throw std::length_error("vector::reserve");
		if (__newCapacity > capacity()) {
			__newCapacity = std::max(__newCapacity, capacity() * 2);
			pointer __newFirst = std::allocator_traits<allocator_type>::allocate(_Alloc, __newCapacity);
			pointer __newLast = std::uninitialized_move(_Begin, _End, __newFirst);
			if (capacity()) {
				std::allocator_traits<allocator_type>::deallocate(_Alloc, _Begin, capacity());
			}
			_Begin = __newFirst;
			_End = __newLast;
			_Capacity = _Begin + __newCapacity;
		}
	}

	_DZYCXX_CONSTEXPR20 size_type capacity()const noexcept {
		return _Capacity - _Begin;
	}

	_DZYCXX_CONSTEXPR20 void shrink_to_fit() {
		if (size() < capacity()) {
			pointer __newFirst = std::allocator_traits<allocator_type>::allocate(_Alloc, size());
			pointer __newLast = std::uninitialized_move(_Begin, _End, __newFirst);
			std::allocator_traits<allocator_type>::deallocate(_Alloc, _Begin, capacity());
			_Begin = __newFirst;
			_End = _Capacity = __newLast;
		}
	}

	_DZYCXX_CONSTEXPR20 void clear()noexcept {
		std::destroy(_Begin, _End);
		_End = _Begin;
	}

	_DZYCXX_CONSTEXPR20 iterator insert(const_iterator __pos, value_type const& __value) {
		size_type __size = __pos - _Begin;
		reserve(size() + 1);
		std::move_backward(_Begin + __size, _End, _End + 1);
		std::allocator_traits<allocator_type>::construct(_Alloc, _Begin + __size, __value);
		++_End;
		return _Begin + __size;
	}

	_DZYCXX_CONSTEXPR20 iterator insert(const_iterator __pos, value_type&& __value) {
		size_type __size = __pos - _Begin;
		reserve(size() + 1);
		std::move_backward(_Begin + __size, _End, _End + 1);
		std::allocator_traits<allocator_type>::construct(_Alloc, _Begin + __size, std::move(__value));
		++_End;
		return _Begin + __size;
	}

	_DZYCXX_CONSTEXPR20 iterator insert(const_iterator __pos, size_type __count, value_type const& __value) {
		size_type __size = __pos - _Begin;
		reserve(size() + __count);
		std::move_backward(_Begin + __size, _End, _End + __count);
		std::uninitialized_fill_n(_Begin + __size, __count, __value);
		_End += __count;
		return _Begin + __size;
	}

	template<class _InputIterator>
		requires(std::input_iterator<_InputIterator>)
	_DZYCXX_CONSTEXPR20 iterator insert(const_iterator __pos, _InputIterator __first, _InputIterator __last) {
		size_type __size = __pos - _Begin;
		size_type __count = __last - __first;
		reserve(size() + __count);
		std::move_backward(_Begin + __size, _End, _End + __count);
		std::uninitialized_copy(__first, __last, _Begin + __size);
		_End += __count;
		return _Begin + __size;
	}

	_DZYCXX_CONSTEXPR20 iterator insert(const_iterator __pos, std::initializer_list<value_type> __init) {
		return insert(__pos, __init.begin(), __init.end());
	}

	template<class ...Args>
	_DZYCXX_CONSTEXPR20 iterator emplace(const_iterator __pos, Args&& ...__args) {
		size_type __size = __pos - _Begin;
		reserve(size() + 1);
		std::move_backward(_Begin + __size, _End, _End + 1);
		std::allocator_traits<allocator_type>::construct(_Alloc, _Begin + __size, std::forward<Args>(__args)...);
		++_End;
		return _Begin + __size;
	}

	_DZYCXX_CONSTEXPR20 iterator erase(const_iterator __pos) {
		std::move(pointer(__pos + 1), pointer(_End), pointer(__pos));
		--_End;
		std::allocator_traits<allocator_type>::destroy(_Alloc, _End);
		return iterator(__pos);
	}

	_DZYCXX_CONSTEXPR20 iterator erase(const_iterator __first, const_iterator __last) {
		if (__first != __last) {
			auto __newLast = std::move(pointer(__last), pointer(_End), pointer(__first));
			std::destroy(__newLast, _End);
			_End = __newLast;
		}
		return iterator(__first);
	}

	_DZYCXX_CONSTEXPR20 void push_back(value_type const& __value) {
		reserve(size() + 1);
		std::allocator_traits<allocator_type>::construct(_Alloc, _End, __value);
		++_End;
	}

	_DZYCXX_CONSTEXPR20 void push_back(value_type&& __value) {
		reserve(size() + 1);
		std::allocator_traits<allocator_type>::construct(_Alloc, _End, std::move(__value));

		++_End;
	}

#if _DZYCXX_HASCXX17
	template<class ...Args>
	_DZYCXX_CONSTEXPR20 reference emplace_back(Args&& ...__args)
#else
	template<class ...Args>
	void emplace_back(Args&& ...__args)
#endif
	{
		reserve(size() + 1);
		std::allocator_traits<allocator_type>::construct(_Alloc, _End, std::forward<Args>(__args)...);
		++_End;
#if _DZYCXX_HASCXX17
		return back();
#else
		return;
#endif
	}

	_DZYCXX_CONSTEXPR20 void pop_back() {
		--_End;
		std::allocator_traits<allocator_type>::destroy(_Alloc, _End);
	}

	_DZYCXX_CONSTEXPR20 void resize(size_type __count) {
		if (__count == size())return;
		if (__count < size()) {
			std::destroy(_Begin + __count, _End);
			_End = _Begin + __count;
		}
		else {
			reserve(__count);
			std::uninitialized_fill(_End, _Begin + __count, value_type{});
			_End = _Begin + __count;
		}
	}

	_DZYCXX_CONSTEXPR20 void resize(size_type __count, value_type const& __value) {
		if (__count == size())return;
		if (__count < size()) {
			std::destroy(_Begin + __count, _End);
			_End = _Begin + __count;
		}
		else {
			reserve(__count);
			std::uninitialized_fill(_End, _Begin + __count, __value);
			_End = _Begin + __count;
		}
	}

	_DZYCXX_CONSTEXPR20 void swap(Vector& __other)
#if _DZYCXX_HASCXX17
		noexcept(std::allocator_traits<allocator_type>::propagate_on_container_swap::value
		|| std::allocator_traits<allocator_type>::is_always_equal::value)
#endif
	{
		std::swap(_Begin, __other._Begin);
		std::swap(_End, __other._End);
		std::swap(_Capacity, __other._Capacity);
		if constexpr (std::allocator_traits<allocator_type>::propagate_on_container_swap::value) {
			std::swap(_Alloc, __other._Alloc);
		}
		else {
			assert(_Alloc == __other._Alloc);
		}
	}
};

template<class _Tp, class _Allocator>
_DZYCXX_CONSTEXPR20 bool operator==(Vector<_Tp, _Allocator> const& __lhs, Vector<_Tp, _Allocator> const& __rhs) {
	return std::equal(__lhs.begin(), __lhs.end(), __rhs.begin(), __rhs.end());
}

#if !_DZYCXX_HASCXX20
template<class _Tp, class _Allocator>
bool operator!=(Vector<_Tp, _Allocator> const& __lhs, Vector<_Tp, _Allocator> const& __rhs) {
	return !(__lhs == __rhs);
}

template<class _Tp, class _Allocator>
bool operator<(Vector<_Tp, _Allocator> const& __lhs, Vector<_Tp, _Allocator> const& __rhs) {
	return std::lexicographical_compare(__lhs.begin(), __lhs.end(), __rhs.begin(), __rhs.end());
}

template<class _Tp, class _Allocator>
bool operator<=(Vector<_Tp, _Allocator> const& __lhs, Vector<_Tp, _Allocator> const& __rhs) {
	return !(__rhs < __lhs);
}

template<class _Tp, class _Allocator>
bool operator>(Vector<_Tp, _Allocator> const& __lhs, Vector<_Tp, _Allocator> const& __rhs) {
	return __rhs < __lhs;
}

template<class _Tp, class _Allocator>
bool operator>=(Vector<_Tp, _Allocator> const& __lhs, Vector<_Tp, _Allocator> const& __rhs) {
	return !(__lhs < __rhs);
}

#else
template<class _Tp, class _Allocator>
_DZYCXX_CONSTEXPR20 auto operator<=>(Vector<_Tp, _Allocator> const& __lhs, Vector<_Tp, _Allocator> const& __rhs) {
	return std::lexicographical_compare_three_way(__lhs.begin(), __lhs.end(), __rhs.begin(), __rhs.end());
}
#endif

template<class _Tp, class _Allocator>
_DZYCXX_CONSTEXPR20 void swap(Vector<_Tp, _Allocator>& __lhs, Vector<_Tp, _Allocator>& __rhs)
#if _DZYCXX_HASCXX17
noexcept(noexcept(__lhs.swap(__rhs)))
#endif
{
	__lhs.swap(__rhs);
}

#if _DZYCXX_HASCXX20
template<class _Tp, class _Allocator, class _Up>
constexpr typename Vector<_Tp, _Allocator>::size_type erase(Vector<_Tp, _Allocator>& __c, _Up const& __value) {
	auto __newLast = std::remove(__c.begin(), __c.end(), __value);
	auto __count = __c.end() - __newLast;
	__c.erase(__newLast, __c.end());
	return __count;
}

template<class _Tp, class _Allocator, class _Pred>
	requires(std::predicate<_Pred, _Tp>)
constexpr typename Vector<_Tp, _Allocator>::size_type erase_if(Vector<_Tp, _Allocator>& __c, _Pred __pred) {
	auto __newLast = std::remove_if(__c.begin(), __c.end(), __pred);
	auto __count = __c.end() - __newLast;
	__c.erase(__newLast, __c.end());
	return __count;
}
#endif

#if _DZYCXX_HASCXX17
template<class _InputIterator, class _Allocator = std::allocator<typename std::iterator_traits<_InputIterator>::value_type>>
Vector(_InputIterator, _InputIterator, _Allocator = _Allocator()) -> Vector<typename std::iterator_traits<_InputIterator>::value_type, _Allocator>;
#endif