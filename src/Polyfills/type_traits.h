#pragma once

#ifndef __TYPE_TRAITS_H__
#define __TYPE_TRAITS_H__

namespace std {
template<bool B, typename T = void>
struct enable_if {};
 
template<typename T>
struct enable_if<true, T> { typedef T type; };
}; // namespace std

#endif // __TYPE_TRAITS_H__
