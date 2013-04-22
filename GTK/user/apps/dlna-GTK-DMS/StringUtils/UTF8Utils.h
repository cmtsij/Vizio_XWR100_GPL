

#ifndef UTF8UTILS_H

#include <ctype.h>
#include <stdlib.h>

/*! \file UTF8Utils.h 
	\brief UTF-8 Encoding/Decoding.
*/

/*! \defgroup UTF8Utils DLNA - UTF8 Encoding/Decoding
	\brief 
	This module provides the encoding/decoding functionality of C strings from/to UTF8 Strings, stored in CDS metadata. 

	\{
*/

/*!	\brief Encodes a multibyte C-style string to UTF-8 compliant format, returned string must be freed.

	\param[in] mbcsStr The input string to be encoded.

	\returns A multi-byte UTF-8 string.
*/
char* EncodeToUTF8(const char* mbcsStr);

/*!	\brief Decodes a UTF-8 string to C-Style multibyte format, returned string must be freed.

	\param[in] utf8Str The input string to be encoded.

	\returns A C-Style multibyte format.
*/
char* DecodeFromUTF8(const char* utf8Str);

/*! \} */

#endif
