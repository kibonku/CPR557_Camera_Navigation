#ifndef __MY_UTILS_H__
#define __MY_UTILS_H__

#include <functional>

// Note: c++17 flag needed
// 
// from: https://stackoverflow.com/a/57595105
template <typename T, typename... Rest>
void myHashCombine(std::size_t& seed, const T& v, const Rest&... rest) {
	seed ^= std::hash<T>{}(v)+0x9e3779b9 + (seed << 6) + (seed >> 2);
	(myHashCombine(seed, rest), ...);
};

#endif

