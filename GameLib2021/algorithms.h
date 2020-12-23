#pragma once

#include <algorithm>

#include <vector>
#include <functional>

template<typename T, typename ConstIt = typename T::const_iterator>
ConstIt nth_occurence( const T& search,  const T& locate, const std::size_t n) {

	int i = 0;

	ConstIt r, b = search.begin(), e = search.end(), lb = locate.begin(), le = locate.end();

	int d = std::distance(lb, le);
	do {
		r = std::search(b, e, lb, le);
		b = r + d;
	} while (r != e && b != e && ++i != n);
	return r;
}

template<typename T, typename ConstIt = typename T::const_iterator, typename ValueType = typename T::value_type>
ConstIt nth_occurence(const T& search, const ValueType& locate, const std::size_t n) {
	std::size_t i = 0;

	for (ConstIt it = search.begin(); it != search.end(); ++it) {
		if (*it == locate && ++i == n) return it;
	}
	return search.end();
}

template<typename T, typename Compare>
void fast_remove_if(std::vector<T>& v, const Compare compare) {

	//v.erase(std::remove_if(v.begin(), v.end(), compare), v.end());

	//return;
	if (!v.size()) return;
	
	typedef typename std::vector<T>::value_type* ValuePtr;

	ValuePtr i = &v.front();
	ValuePtr last = &v.back();

	while (i <= last) {
		if (compare(*i)) {

			while ( last != i && compare(*last) ) {
				--last;
			}

			if( last != i )	//do not move self into self, crash
				*i = std::move(*last);

			--last;
		}
		++i;
	}

	v.resize(last + 1 - &v[0]);
}

template<typename T>
void sortAndUnique(std::vector<T>& v) {

	std::sort(v.begin(), v.end());
	auto end = std::unique(v.begin(), v.end());
	v.erase(end, v.end());
}

template<typename T, typename CompareLess, typename CompareUnique>
void sortAndUniqueCustom(std::vector<T>& v, CompareLess compareLess, CompareUnique compareUnique) {

	std::sort(v.begin(), v.end(), compareLess);
	auto end = std::unique(v.begin(), v.end(), compareUnique);
	v.erase(end, v.end());
}