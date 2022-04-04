#pragma once

#ifndef __TYPE_TRAITS_H__
#define __TYPE_TRAITS_H__

namespace std {
template<bool BOOL, typename T = void>
struct enable_if {};
 
template<typename T>
struct enable_if<true, T> { typedef T type; };

};

#endif // __TYPE_TRAITS_H__
