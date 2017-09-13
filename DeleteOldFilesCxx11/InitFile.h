#pragma once
#include <utility>
#include <string>
#include <vector>
#include "row2.h"
#include <locale>
#include <codecvt>
//#include <regex>
//-------
//template __PURE_APPDOMAIN_GLOBAL locale::id collate<char16_t>::id;
//namespace std {
	// CLASS regex_traits<char16_t>
	//template<>
	//class regex_traits<char16_t>
	//	: public _Regex_traits<char16_t>
	//{	// specialization for char16_t
	//public:
	//	int value(char16_t _Ch, int _Base) const
	//	{	// map character value to numeric value
	//		if ((_Base != 8 && u'0' <= _Ch && _Ch <= u'9')
	//			|| (_Base == 8 && u'0' <= _Ch && _Ch <= u'7'))
	//			return (_Ch - u'0');
	//		else if (_Base != 16)
	//			;
	//		else if (u'a' <= _Ch && _Ch <= u'f')
	//			return (_Ch - u'a' + 10);
	//		else if (u'A' <= _Ch && _Ch <= u'F')
	//			return (_Ch - u'A' + 10);
	//		return (-1);
	//	}
	//};
//}


namespace my {
	//template <typename T>
	//extern void 
	//clear_quotes(T& quoted_string);
	
	template <typename T>
	extern void
		clear_quotes(T& quoted_string) {
		auto pos = quoted_string.find_first_of('"');
		while (pos != std::string::npos) {
			quoted_string.erase(pos, 1);
			pos = quoted_string.find_first_of('"');
		}
	}



	template <typename T>
	auto from_utf8_to_ucs2(const std::string& dir1) -> T {
#if _MSC_VER >= 1900
		std::wstring_convert
			<std::codecvt_utf8 <int16_t>, int16_t>
			from_utf8_to_ucs2;

		auto wtf = from_utf8_to_ucs2.from_bytes(dir1);
		auto ww = reinterpret_cast<const typename T::value_type*>(wtf.data());
		T ret{ ww, wtf.size() };


#else
		std::wstring_convert <std::codecvt_utf8 <char16_t>, char16_t> from_utf8_to_ucs2;
		T ret = from_utf8_to_ucs2.from_bytes(dir1);
#endif
		return ret;
	}
	template <typename T>
	std::string toUTF8(const std::basic_string<T, std::char_traits<T>, std::allocator<T>>& source)
	{
		std::string result;

		std::wstring_convert< std::codecvt_utf8_utf16<T>, T> convertor;
		result = convertor.to_bytes(source);

		return result;
	}


	//template<>
	//void
	//	clear_quotes<std::string>(std::string& quoted_string);

	//template<>
	//void 
	//clear_quotes<std::wstring>(std::wstring& quoted_string);

	//void
	//	clear_quotes(std::string& quoted_string);

	/*
	Gets qouted string with spaces or unqouted string without spaces
	*/
	std::pair<std::string, size_t>	get1stColumn_qouted_string_or_unqouted_string(const std::string& line1, size_t	startpos);
	
	int how_many_quotes(std::string & s);
	
	bool readInitFile(std::vector<row> & v, std::wstring& Initfilename);


}