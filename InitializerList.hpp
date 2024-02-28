#pragma once
#include "Config.h"

template<class _Tp>
struct InitializerList {
	using value_type = _Tp;
	using reference = const _Tp&;
	using const_reference = const _Tp&;
	using size_type = std::size_t;
	using iterator = const _Tp*;
	using const_iterator = const _Tp*;

	_DZYCXX_CONSTEXPR14 InitializerList() noexcept
		: _First(nullptr), _Last(nullptr) {}

	_DZYCXX_CONSTEXPR14 InitializerList(const_iterator __first, const_iterator __last)noexcept
		:_First(__first), _Last(__last) {}

	// not in std
	_DZYCXX_CONSTEXPR14 InitializerList(std::initializer_list<_Tp>__stdiList)noexcept
		:_First(__stdiList.begin()), _Last(__stdiList.end()) {}

	_DZYCXX_CONSTEXPR14 size_type size() const noexcept {
		return static_cast<size_type>(_Last - _First);
	}

	_DZYCXX_CONSTEXPR14 const_iterator begin() const noexcept {
		return _First;
	}

	_DZYCXX_CONSTEXPR14 const_iterator end() const noexcept {
		return _Last;
	}

private:
	iterator _First, _Last;
};

template<class _Tp>
_DZYCXX_CONSTEXPR14 const _Tp* begin(InitializerList<_Tp> __iList) noexcept {
	return __iList.begin();
}

template<class _Tp>
_DZYCXX_CONSTEXPR14 const _Tp* end(InitializerList<_Tp> __iList) noexcept {
	return __iList.end();
}

#if _DZYCXX_HASCXX14
template<class _Tp>
_DZYCXX_NODISCARD _DZYCXX_CONSTEXPR17 std::reverse_iterator<const _Tp*> rbegin(InitializerList<_Tp> __iList) noexcept {
	return std::reverse_iterator<const _Tp*>(__iList.end());
}

template<class _Tp>
_DZYCXX_NODISCARD _DZYCXX_CONSTEXPR17 auto crbegin(InitializerList<_Tp> __iList) noexcept(noexcept(rbegin(__iList)))
-> decltype(rbegin(__iList)) {
	return rbegin(__iList);
}

template<class _Tp>
_DZYCXX_NODISCARD _DZYCXX_CONSTEXPR17 std::reverse_iterator<const _Tp*> rend(InitializerList<_Tp> __iList) noexcept {
	return std::reverse_iterator<const _Tp*>(__iList.begin());
}

template<class _Tp>
_DZYCXX_NODISCARD _DZYCXX_CONSTEXPR17 auto crend(InitializerList<_Tp> __iList) noexcept(noexcept(rend(__iList)))
-> decltype(rend(__iList)) {
	return rend(__iList);
}
#endif

#if _DZYCXX_HASCXX17
template<class _Tp>
_DZYCXX_NODISCARD constexpr bool empty(InitializerList<_Tp> _Ilist) noexcept {
	return _Ilist.size() == 0;
}

template<class _Tp>
_DZYCXX_NODISCARD constexpr const _Tp* data(InitializerList<_Tp> _Ilist) noexcept {
	return _Ilist.begin();
}
#endif