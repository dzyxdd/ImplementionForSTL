#pragma once
#include "Config.h"

template<class _Tp>
struct DefaultDelete {

	constexpr DefaultDelete() noexcept = default;

#if _DZYCXX_HASCXX20
	template<class _Up>
		requires(std::is_convertible_v<_Up*, _Tp*>)
#else
	template <typename _Up, std::enable_if<std::is_convertible<_Up*, _Tp*>::value, int>::type = 0>
#endif
	_DZYCXX_CONSTEXPR23 DefaultDelete(const DefaultDelete<_Up>& __other)noexcept {}

	_DZYCXX_CONSTEXPR23 void operator()(_Tp* __pointer) const {
		static_assert(!std::is_void_v<_Tp>, "can't delete incomplete type");
		static_assert(sizeof(_Tp) > 0, "can't delete incomplete type");
		delete __pointer;
	}
};

template<class _Tp>
struct DefaultDelete<_Tp[]> {

	constexpr DefaultDelete() noexcept = default;

#if _DZYCXX_HASCXX20
	template<class _Up>
		requires(std::is_convertible_v<_Up* [], _Tp* []>)
#else
	template <typename _Up, std::enable_if<std::is_convertible<_Up* [], _Tp* []>::value, int>::type = 0>
#endif
	_DZYCXX_CONSTEXPR23 DefaultDelete(const DefaultDelete<_Up[]>& __other)noexcept {}

	template<class _Up>
		requires(std::is_convertible_v<_Up(*)[], _Tp(*)[]>)
	_DZYCXX_CONSTEXPR23 void operator()(_Up* __pointer) const {
		static_assert(sizeof(_Tp) > 0, "can't delete incomplete type");
		delete[] __pointer;
	}
};

template<class _Tp, class _Dx, class = void>
struct _GetUniquePointerType {
	using type = _Tp*;
};

template<class _Tp, class _Dx>
struct _GetUniquePointerType<_Tp, _Dx, std::void_t<typename std::remove_reference_t<_Dx>::pointer>> {
	using type = typename std::remove_reference_t<_Dx>::pointer;
};

template<class _Tp, class _Dx = DefaultDelete<_Tp>>
struct UniquePtr {

	using pointer = typename _GetUniquePointerType<_Tp, _Dx>::type;
	using element_type = _Tp;
	using deleter_type = _Dx;

	pointer _Pointer;
	deleter_type _Deleter;

	constexpr UniquePtr() noexcept
		requires ((!std::is_pointer_v<_Dx>) && std::is_default_constructible_v<_Dx>&& std::is_nothrow_default_constructible_v<_Dx>)
	: _Pointer(nullptr) {}

	constexpr UniquePtr(std::nullptr_t) noexcept
		requires ((!std::is_pointer_v<_Dx>) && std::is_default_constructible_v<_Dx>&& std::is_nothrow_default_constructible_v<_Dx>)
	: _Pointer(nullptr) {}

	_DZYCXX_CONSTEXPR23 explicit UniquePtr(pointer __p) noexcept
		requires(std::is_nothrow_copy_constructible_v<_Dx>)
	: _Pointer(__p) {}

	//_Dx = A, _Deleter = A const&; _Dx = A&, _Deleter = A&; _Dx = A const&, _Deleter = A const&
	_DZYCXX_CONSTEXPR23 explicit UniquePtr(pointer __pointer, deleter_type const& __deleter) noexcept
		requires (std::is_copy_constructible_v<_Dx>&& std::is_constructible_v<_Dx, _Dx const&>)
	: _Pointer(__pointer), _Deleter(__deleter) {}

	//_Dx = A, _Deleter = A &&
	_DZYCXX_CONSTEXPR23 explicit UniquePtr(pointer __pointer, deleter_type&& __deleter) noexcept
		requires ((!std::is_reference_v<_Dx>) && std::is_move_constructible_v<_Dx>&& std::is_constructible_v<_Dx, _Dx&&>)
	: _Pointer(__pointer), _Deleter(std::move(__deleter)) {}

	//_Dx = A&, _Deleter = A &&; _Dx=A const&, _Deleter = A const&&
	_DZYCXX_CONSTEXPR23 explicit UniquePtr(pointer, std::remove_reference_t<deleter_type>&&) noexcept
		requires (std::is_lvalue_reference_v<_Dx>&& std::is_constructible_v<_Dx, std::remove_reference_t<_Dx>&&>)
	= delete;

	_DZYCXX_CONSTEXPR23 UniquePtr(UniquePtr&& __other)noexcept
		requires (std::is_move_constructible_v<_Dx>)
	: _Pointer(__other.release()), _Deleter(std::forward<_Dx>(__other.get_deleter())) {}

	template<class _Tp2, class _Dx2>
		requires ((!std::is_array_v<_Tp2>) && std::is_convertible_v<typename UniquePtr<_Tp2, _Dx2>::pointer, pointer> && ((std::is_reference_v<_Dx>&& std::is_same_v<_Dx, _Dx2>) || ((!std::is_reference_v<_Dx>) && std::is_convertible_v<_Dx2, _Dx>)))
	_DZYCXX_CONSTEXPR23 UniquePtr(UniquePtr<_Tp2, _Dx2>&& __other)noexcept
		:_Pointer(__other.release()), _Deleter(std::forward<_Dx2>(__other.get_deleter())) {}

#if !_DZYCXX_HASCXX17
	template <class _Tp2, std::enable_if_t<std::conjunction_v<std::is_convertible<_Tp2*, _Tp*>, std::is_same<_Dx, std::default_delete<_Tp>>>, int> = 0>
	UniquePtr(std::auto_ptr<_Tp2>&& __autoPointer) noexcept : _Pointer(__autoPointer.release()) {}
#endif

	UniquePtr(const UniquePtr&) = delete;

	_DZYCXX_CONSTEXPR23 ~UniquePtr() {
		static_assert(std::is_invocable_v<deleter_type&, pointer>, "UniquePtr's deleter must be invocable with a pointer");
		auto __pointer = get();
		if (__pointer != nullptr) {
			get_deleter()(__pointer);
			__pointer = nullptr;
		}
	}

	_DZYCXX_CONSTEXPR23 UniquePtr& operator=(UniquePtr&& __other) noexcept
		requires std::is_move_assignable_v<_Dx> {
		reset(__other.release());
		get_deleter() = std::forward<_Dx>(__other.get_deleter());
		return *this;
	}

	template<class _Tp2, class _Dx2>
		requires (!std::is_array_v<_Tp2>) && std::is_convertible_v<typename UniquePtr<_Tp2, _Dx2>::pointer, pointer>&& std::is_assignable_v<_Dx&, _Dx2&&>
	_DZYCXX_CONSTEXPR23 UniquePtr& operator=(UniquePtr<_Tp2, _Dx2>&& __other) noexcept {
		reset(__other.release());
		get_deleter() = std::forward<_Dx2>(__other.get_deleter());
		return *this;
	}

	_DZYCXX_CONSTEXPR23 UniquePtr& operator=(std::nullptr_t) noexcept {
		reset();
		return *this;
	}

	UniquePtr& operator=(const UniquePtr&) = delete;

	_DZYCXX_CONSTEXPR23 pointer release()noexcept {
		return std::exchange(_Pointer, nullptr);
	}

	_DZYCXX_CONSTEXPR23 void reset(pointer __pointer = pointer()) noexcept {
		auto __old_pointer = std::exchange(_Pointer, __pointer);
		if (__old_pointer != nullptr) {
			get_deleter()(__old_pointer);
		}
	}

	_DZYCXX_CONSTEXPR23 void swap(UniquePtr& __other)noexcept
		requires std::is_swappable_v<_Dx>&& std::is_swappable_v<_Tp> {
		std::swap(_Pointer, __other._Pointer);
		std::swap(_Deleter, __other._Deleter);
	}

	_DZYCXX_CONSTEXPR23 pointer get()const noexcept {
		return _Pointer;
	}

	_DZYCXX_CONSTEXPR23 deleter_type& get_deleter()noexcept {
		return _Deleter;
	}

	_DZYCXX_CONSTEXPR23 deleter_type const& get_deleter()const noexcept {
		return _Deleter;
	}

	_DZYCXX_CONSTEXPR23 explicit operator bool() const noexcept {
		return get() != nullptr;
	}

	_DZYCXX_CONSTEXPR23 std::add_lvalue_reference_t<_Tp> operator*() const noexcept(noexcept(*std::declval<pointer>())) {
		return *get();
	}

	_DZYCXX_CONSTEXPR23 pointer operator->() const noexcept {
		return get();
	}
};

template<class _Tp, class _Dx>
struct UniquePtr<_Tp[], _Dx> {

	using pointer = typename _GetUniquePointerType<_Tp, _Dx>::type;
	using element_type = _Tp;
	using deleter_type = _Dx;

	pointer _Pointer;
	deleter_type _Deleter;

	constexpr UniquePtr() noexcept
		requires ((!std::is_pointer_v<_Dx>) && std::is_default_constructible_v<_Dx>&& std::is_nothrow_default_constructible_v<_Dx>)
	: _Pointer(nullptr) {}

	constexpr UniquePtr(std::nullptr_t) noexcept
		requires ((!std::is_pointer_v<_Dx>) && std::is_default_constructible_v<_Dx>&& std::is_nothrow_default_constructible_v<_Dx>)
	: _Pointer(nullptr) {}

	template<class _Up>
		requires(std::is_same_v <_Up, pointer> || std::is_same_v<_Up, std::nullptr_t> || (std::is_same_v<pointer, element_type*> && std::is_pointer_v<_Up> && std::is_convertible_v < std::remove_reference_t<_Up>(*)[], element_type(*)[]>))
	_DZYCXX_CONSTEXPR23 explicit UniquePtr(_Up __p) noexcept
		requires(std::is_nothrow_copy_constructible_v<_Dx>)
	: _Pointer(__p) {}

	//_Dx = A, _Deleter = A const&; _Dx = A&, _Deleter = A&; _Dx = A const&, _Deleter = A const&
	template<class _Up>
		requires(std::is_same_v <_Up, pointer> || std::is_same_v<_Up, std::nullptr_t> || (std::is_same_v<pointer, element_type*> && std::is_pointer_v<_Up> && std::is_convertible_v < std::remove_reference_t<_Up>(*)[], element_type(*)[]>))
	_DZYCXX_CONSTEXPR23 explicit UniquePtr(_Up __pointer, deleter_type const& __deleter) noexcept
		requires (std::is_copy_constructible_v<_Dx>&& std::is_constructible_v<_Dx, _Dx const&>)
	: _Pointer(__pointer), _Deleter(__deleter) {}

	//_Dx = A, _Deleter = A &&
	template<class _Up>
		requires(std::is_same_v <_Up, pointer> || std::is_same_v<_Up, std::nullptr_t> || (std::is_same_v<pointer, element_type*> && std::is_pointer_v<_Up> && std::is_convertible_v < std::remove_reference_t<_Up>(*)[], element_type(*)[]>))
	_DZYCXX_CONSTEXPR23 explicit UniquePtr(_Up __pointer, deleter_type&& __deleter) noexcept
		requires ((!std::is_reference_v<_Dx>) && std::is_move_constructible_v<_Dx>&& std::is_constructible_v<_Dx, _Dx&&>)
	: _Pointer(std::move(__pointer)), _Deleter(std::move(__deleter)) {}

	//_Dx = A&, _Deleter = A &&; _Dx=A const&, _Deleter = A const&&
	template<class _Up>
		requires(std::is_same_v <_Up, pointer> || std::is_same_v<_Up, std::nullptr_t> || (std::is_same_v<pointer, element_type*> && std::is_pointer_v<_Up> && std::is_convertible_v < std::remove_reference_t<_Up>(*)[], element_type(*)[]>))
	_DZYCXX_CONSTEXPR23 explicit UniquePtr(_Up, std::remove_reference_t<deleter_type>&&) noexcept
		requires (std::is_lvalue_reference_v<_Dx>&& std::is_constructible_v<_Dx, std::remove_reference_t<_Dx>&&>)
	= delete;

	_DZYCXX_CONSTEXPR23 UniquePtr(UniquePtr&& __other)noexcept
		requires (std::is_move_constructible_v<_Dx>)
	: _Pointer(__other.release()), _Deleter(std::forward<_Dx>(__other.get_deleter())) {}

	template<class _Tp2, class _Dx2>
		requires (std::is_array_v<_Tp2>&& std::is_same_v<pointer, element_type*>&& std::is_same_v<typename UniquePtr<_Tp2, _Dx2>::pointer, typename UniquePtr<_Tp2, _Dx2>::element_type*>&& std::is_convertible_v<typename UniquePtr<_Tp2, _Dx2>::element_type(*)[], element_type(*)[]> && ((std::is_reference_v<_Dx>&& std::is_same_v<_Dx, _Dx2>) || ((!std::is_reference_v<_Dx>) && std::is_convertible_v<_Dx2, _Dx>)))
	_DZYCXX_CONSTEXPR23 UniquePtr(UniquePtr<_Tp2, _Dx2>&& __other)noexcept
		: _Pointer(__other.release()), _Deleter(std::forward<_Dx2>(__other.get_deleter())) {}

	UniquePtr(const UniquePtr&) = delete;

	_DZYCXX_CONSTEXPR23 ~UniquePtr() {
		auto __pointer = get();
		if (__pointer != nullptr) {
			get_deleter()(__pointer);
			__pointer = nullptr;
		}
	}

	_DZYCXX_CONSTEXPR23 UniquePtr& operator=(UniquePtr&& __other) noexcept
		requires std::is_move_assignable_v<_Dx> {
		reset(__other.release());
		get_deleter() = std::forward<_Dx>(__other.get_deleter());
		return *this;
	}

	template<class _Tp2, class _Dx2>
		requires ((std::is_array_v<_Tp2>) && std::is_same_v<pointer, element_type*>&& std::is_same_v<typename UniquePtr<_Tp2, _Dx2>::pointer, typename UniquePtr<_Tp2, _Dx2>::element_type*>&& std::is_convertible_v<typename UniquePtr<_Tp2, _Dx2>::element_type(*)[], element_type(*)[]>&& std::is_assignable_v<_Dx&, _Dx2&&>)
	_DZYCXX_CONSTEXPR23 UniquePtr& operator=(UniquePtr<_Tp2, _Dx2>&& __other) noexcept {
		reset(__other.release());
		get_deleter() = std::forward<_Dx2>(__other.get_deleter());
		return *this;
	}

	_DZYCXX_CONSTEXPR23 UniquePtr& operator=(std::nullptr_t) noexcept {
		reset();
		return *this;
	}

	UniquePtr& operator=(const UniquePtr&) = delete;

	_DZYCXX_CONSTEXPR23 pointer release()noexcept {
		return std::exchange(_Pointer, nullptr);
	}

	template<class _Up>
		requires(std::is_same_v <_Up, pointer> || (std::is_same_v<pointer, element_type*> && std::is_pointer_v<_Up> && std::is_convertible_v < std::remove_reference_t<_Up>(*)[], element_type(*)[]>))
	_DZYCXX_CONSTEXPR23 void reset(_Up __pointer) noexcept {
		auto __old_pointer = std::exchange(_Pointer, __pointer);
		if (__old_pointer != nullptr) {
			get_deleter()(__old_pointer);
		}
	}

	_DZYCXX_CONSTEXPR23 void reset(std::nullptr_t = nullptr) noexcept {
		reset(pointer());
	}

	_DZYCXX_CONSTEXPR23 void swap(UniquePtr& __other)noexcept
		requires std::is_swappable_v<_Dx>&& std::is_swappable_v<_Tp> {
		std::swap(_Pointer, __other._Pointer);
		std::swap(_Deleter, __other._Deleter);
	}

	_DZYCXX_CONSTEXPR23 pointer get()const noexcept {
		return _Pointer;
	}

	_DZYCXX_CONSTEXPR23 deleter_type& get_deleter()noexcept {
		return _Deleter;
	}

	_DZYCXX_CONSTEXPR23 deleter_type const& get_deleter()const noexcept {
		return _Deleter;
	}

	_DZYCXX_CONSTEXPR23 explicit operator bool() const noexcept {
		return get() != nullptr;
	}

	_DZYCXX_CONSTEXPR23 element_type& operator*() const noexcept(noexcept(*std::declval<pointer>())) {
		return *get();
	}

	_DZYCXX_CONSTEXPR23 element_type& operator[](std::size_t __index) const noexcept {
		return get()[__index];
	}
};

#if _DZYCXX_HASCXX14
template<class _Tp, class ..._Args>
	requires (!std::is_array_v<_Tp>)
_DZYCXX_CONSTEXPR23 UniquePtr<_Tp> make_unique(_Args&& ...__args) {
	return UniquePtr<_Tp>(new _Tp(std::forward<_Args>(__args)...));
}

template<class _Tp>
	requires (std::is_unbounded_array_v<_Tp>)
_DZYCXX_CONSTEXPR23 UniquePtr<_Tp> make_unique(std::size_t __size) {
	return UniquePtr<_Tp>(new std::remove_extent_t<_Tp>[__size]());
}

template<class _Tp, class ..._Args>
	requires (std::is_bounded_array_v<_Tp>)
void make_unique(_Args&& ...args) = delete;
#endif

#if _DZYCXX_HASCXX20

template<class _Tp>
	requires (!std::is_array_v<_Tp>)
_DZYCXX_CONSTEXPR23 UniquePtr<_Tp> make_unique_for_overwrite() {
	return UniquePtr<_Tp>(new _Tp);
}

template<class _Tp>
	requires (std::is_unbounded_array_v<_Tp>)
_DZYCXX_CONSTEXPR23 UniquePtr<_Tp> make_unique_for_overwrite(std::size_t __size) {
	return UniquePtr<_Tp>(new std::remove_extent_t<_Tp>[__size]);
}

template<class _Tp, class ...Args>
	requires (std::is_bounded_array_v<_Tp>)
void make_unique_for_overwrite(Args&& ...args) = delete;

#endif

template<class _Tp1, class _Dx1, class _Tp2, class _Dx2>
_DZYCXX_CONSTEXPR23 bool operator==(const UniquePtr<_Tp1, _Dx1>& __lhs, const UniquePtr<_Tp2, _Dx2>& __rhs) {
	return __lhs.get() == __rhs.get();
}

#if !_DZYCXX_HASCXX20

template<class _Tp1, class _Dx1, class _Tp2, class _Dx2>
bool operator!=(const UniquePtr<_Tp1, _Dx1>& __lhs, const UniquePtr<_Tp2, _Dx2>& __rhs) {
	return __lhs.get() != __rhs.get();
}

#endif

template<class _Tp1, class _Dx1, class _Tp2, class _Dx2>
_DZYCXX_CONSTEXPR23 bool operator<(const UniquePtr<_Tp1, _Dx1>& __lhs, const UniquePtr<_Tp2, _Dx2>& __rhs) {
	using _CT = std::common_type_t<typename UniquePtr<_Tp1, _Dx1>::pointer, typename UniquePtr<_Tp2, _Dx2>::pointer>;
	return std::less<_CT>()(__lhs.get(), __rhs.get());
}

template<class _Tp1, class _Dx1, class _Tp2, class _Dx2>
_DZYCXX_CONSTEXPR23 bool operator<=(const UniquePtr<_Tp1, _Dx1>& __lhs, const UniquePtr<_Tp2, _Dx2>& __rhs) {
	return !(__rhs < __lhs);
}

template<class _Tp1, class _Dx1, class _Tp2, class _Dx2>
_DZYCXX_CONSTEXPR23 bool operator>(const UniquePtr<_Tp1, _Dx1>& __lhs, const UniquePtr<_Tp2, _Dx2>& __rhs) {
	return __rhs < __lhs;
}

template<class _Tp1, class _Dx1, class _Tp2, class _Dx2>
_DZYCXX_CONSTEXPR23 bool operator>=(const UniquePtr<_Tp1, _Dx1>& __lhs, const UniquePtr<_Tp2, _Dx2>& __rhs) {
	return !(__lhs < __rhs);
}

#if _DZYCXX_HASCXX20
template<class _Tp1, class _Dx1, class _Tp2, class _Dx2>
	requires std::three_way_comparable_with<typename UniquePtr<_Tp1, _Dx1>::pointer, typename UniquePtr<_Tp2, _Dx2>::pointer>
std::compare_three_way_result_t<typename UniquePtr<_Tp1, _Dx1>::pointer, typename UniquePtr<_Tp2, _Dx2>::pointer>operator<=>(const UniquePtr<_Tp1, _Dx1>& __lhs, const UniquePtr<_Tp2, _Dx2>& __rhs) {
	return std::compare_three_way{}(__lhs.get(), __rhs.get());
}
#endif

template<class _Tp, class _Dx>
_DZYCXX_CONSTEXPR23 bool operator==(const UniquePtr<_Tp, _Dx>& __lhs, std::nullptr_t) noexcept {
	return !__lhs;
}

template<class _Tp, class _Dx>
_DZYCXX_CONSTEXPR23 bool operator==(std::nullptr_t, const UniquePtr<_Tp, _Dx>& __rhs) noexcept {
	return !__rhs;
}

template<class _Tp, class _Dx>
_DZYCXX_CONSTEXPR23 bool operator!=(const UniquePtr<_Tp, _Dx>& __lhs, std::nullptr_t) noexcept {
	return static_cast<bool>(__lhs);
}

template<class _Tp, class _Dx>
_DZYCXX_CONSTEXPR23 bool operator!=(std::nullptr_t, const UniquePtr<_Tp, _Dx>& __rhs) noexcept {
	return static_cast<bool>(__rhs);
}

template<class _Tp, class _Dx>
_DZYCXX_CONSTEXPR23 bool operator<(const UniquePtr<_Tp, _Dx>& __lhs, std::nullptr_t) {
	return std::less<typename UniquePtr<_Tp, _Dx>::pointer>()(__lhs.get(), nullptr);
}

template<class _Tp, class _Dx>
_DZYCXX_CONSTEXPR23 bool operator<(std::nullptr_t, const UniquePtr<_Tp, _Dx>& __rhs) {
	return std::less<typename UniquePtr<_Tp, _Dx>::pointer>()(nullptr, __rhs.get());
}

template<class _Tp, class _Dx>
_DZYCXX_CONSTEXPR23 bool operator<=(const UniquePtr<_Tp, _Dx>& __lhs, std::nullptr_t) {
	return !(nullptr < __lhs);
}

template<class _Tp, class _Dx>
_DZYCXX_CONSTEXPR23 bool operator<=(std::nullptr_t, const UniquePtr<_Tp, _Dx>& __rhs) {
	return !(__rhs < nullptr);
}

template<class _Tp, class _Dx>
_DZYCXX_CONSTEXPR23 bool operator>(const UniquePtr<_Tp, _Dx>& __lhs, std::nullptr_t) {
	return nullptr < __lhs;
}

template<class _Tp, class _Dx>
_DZYCXX_CONSTEXPR23 bool operator>(std::nullptr_t, const UniquePtr<_Tp, _Dx>& __rhs) {
	return __rhs < nullptr;
}

template<class _Tp, class _Dx>
_DZYCXX_CONSTEXPR23 bool operator>=(const UniquePtr<_Tp, _Dx>& __lhs, std::nullptr_t) {
	return !(nullptr > __lhs);
}

template<class _Tp, class _Dx>
_DZYCXX_CONSTEXPR23 bool operator>=(std::nullptr_t, const UniquePtr<_Tp, _Dx>& __rhs) {
	return !(__rhs > nullptr);
}

#if _DZYCXX_HASCXX20
template<class _Tp, class _Dx>
	requires std::three_way_comparable<typename UniquePtr<_Tp, _Dx>::pointer>
_DZYCXX_CONSTEXPR23 std::compare_three_way_result_t<typename UniquePtr<_Tp, _Dx>::pointer> operator<=> (const UniquePtr<_Tp, _Dx>& __lhs, std::nullptr_t) {
	return std::compare_three_way{}(__lhs.get(), static_cast<typename UniquePtr<_Tp, _Dx>::pointer>(nullptr));
}
#endif

#if _DZYCXX_HASCXX20
template<class _CharT, class _Traits, class _Tp, class _Dx>
std::basic_ostream<_CharT, _Traits>& operator<<(std::basic_ostream<_CharT, _Traits>& __os, const UniquePtr<_Tp, _Dx>& __p)
	requires requires{__os << __p.get(); }
{
	return __os << __p.get();
}
#endif

template<class _Tp, class _Dx>
	requires (std::is_swappable_v<_Dx>)
_DZYCXX_CONSTEXPR23 void swap(UniquePtr<_Tp, _Dx>& __lhs, UniquePtr<_Tp, _Dx>& __rhs) noexcept {
	__lhs.swap(__rhs);
}

template<class _Tp, class _Dx >
	requires (std::is_default_constructible_v < std::hash<typename UniquePtr<_Tp, _Dx>::pointer>>)
struct std::hash <UniquePtr<_Tp, _Dx>> {
	static size_t _Do_hash(const UniquePtr<_Tp, _Dx>& __p) noexcept/*(std::_Is_nothrow_hashable<typename UniquePtr<_Tp, _Dx>::pointer>::value)*/ {
		return std::hash<typename UniquePtr<_Tp, _Dx>::pointer>{}(__p.get());
	}

	std::size_t operator()(const UniquePtr<_Tp, _Dx>& __p) const noexcept(noexcept(hash<UniquePtr<_Tp, _Dx>>::_Do_hash(__p))) {
		return hash::_Do_hash(__p);
	}
};

template<class _Tp, class _Dx >
	requires (!std::is_default_constructible_v < std::hash<typename UniquePtr<_Tp, _Dx>::pointer>>)
struct std::hash <UniquePtr<_Tp, _Dx>> {
	hash() = delete;
	hash(const hash&) = delete;
	hash(hash&&) = delete;
	hash& operator=(const hash&) = delete;
	hash& operator=(hash&&) = delete;
};