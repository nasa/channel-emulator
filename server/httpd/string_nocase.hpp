/* -*- C++ -*- */

//=============================================================================
/**
 * @file   string_nocase.hpp
 * @author Tad Kollar  
 *
 * $Id: string_nocase.hpp 1670 2012-08-28 13:25:28Z tkollar $ 
 * Copyright (c) 2010.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _NASA_STRING_NOCASE_HPP_
#define _NASA_STRING_NOCASE_HPP_

#include <string>
#include <cctype>
#include <cstring>
#include <iostream>


namespace nasaCE {

//=============================================================================
/**
 * @class string_nocase
 * @brief Case insensitive string class
 * @see http://www.daniweb.com/code/snippet216949.html
 *
 * String preserves case; comparisons are case insensitive.
 */
//=============================================================================

struct traits_nocase: std::char_traits<char> {
	static bool eq( const char& c1, const char& c2 ) { return toupper(c1) == toupper(c2) ; }
	static bool lt( const char& c1, const char& c2 ) { return toupper(c1) < toupper(c2) ; }
	static int compare( const char* s1, const char* s2, size_t N ) { return strncasecmp( s1, s2, N ); }

	static const char* find( const char* s, size_t N, const char& a ) {
		for( size_t i=0 ; i<N ; ++i ) 
			if( toupper(s[i]) == toupper(a) ) return s+i;

		return 0;
	}

	static bool eq_int_type ( const int_type& c1, const int_type& c2 ) { return toupper(c1) == toupper(c2) ; }
};


typedef std::basic_string< char, traits_nocase > string_nocase;

// make string_nocase work like a std::string
// with streams using std::char_traits
// std::basic_istream< char, std::char_traits<char> > (std::istream) and
// std::basic_ostream< char, std::char_traits<char> > (std::ostream)
inline std::ostream& operator<< ( std::ostream& stm, const string_nocase& str ) {
	return stm << reinterpret_cast<const std::string&>(str);
}

inline std::istream& operator>> ( std::istream& stm, string_nocase& str ) {
	std::string s;
	stm >> s;

	if(stm) str.assign(s.begin(),s.end());

	return stm;
}

inline std::istream& getline( std::istream& stm, string_nocase& str ) {
	std::string s;
	std::getline(stm,s);

	if(stm) str.assign(s.begin(),s.end());

	return stm;
}

} // namespace nasaCE

#endif
