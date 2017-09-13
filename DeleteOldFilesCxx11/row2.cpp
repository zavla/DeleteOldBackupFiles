#include "row2.h"
#include "InitFile.h"
#include <tuple>
#include <chrono>
#include <locale>
#include <codecvt>

//instantiation of static member, from collate class in "local" header. 
//Have made a class collate<char16_t> specialization  with no dllimport statis id member.
__PURE_APPDOMAIN_GLOBAL  std::locale::id std::collate<char16_t>::id; 

void my::row::clear() {
	using namespace std::chrono_literals;
	m_dir = u"";
	m_full_store = 0h;
	m_fullpattern = u"";
};

std::chrono::hours my::row::HowManyHours(std::string& str) {

	/*
	std::string temp; //never used
	temp.reserve ( 5 );
	auto clocale = std::locale::classic ( );
	for ( auto& i : str )
	{
	if ( std::isdigit ( i , clocale ) ) temp.push_back ( i );
	else break ;

	}
	*/
	auto NumHours = static_cast<std::chrono::hours>(std::strtol(str.c_str(), NULL, 10));

	if (str.find("hour") != std::string::npos) {
		return NumHours;
	}
	else {
		return NumHours * 24;
	}
}


/*
reading operator for the "row" type
*/
my::LoadLineRetvalue my::row::LoadLine(std::string& line1) {
	using namespace std::chrono_literals;
	//"c:\program files\myBackUpdir\"  4days regexpOfFileName

	auto firstNonSpace = line1.find_first_not_of(" \t");
	if (firstNonSpace == std::string::npos || line1.substr(firstNonSpace, 2) == R"(//)")
		//empty line or comment
		return LoadLineRetvalue::ignore;

	std::string temp;
	std::size_t pos;

	std::tie(temp, pos) = my::get1stColumn_qouted_string_or_unqouted_string(line1, 0);

	m_dir = from_utf8_to_ucs2 
				<std::u16string> (temp);

	//field #2 
	std::string DurationField1, DurationField2;

	std::tie(DurationField1, pos) = my::get1stColumn_qouted_string_or_unqouted_string(line1, pos);
	m_full_store = HowManyHours(DurationField1);

	//filed #3
	std::tie(temp, pos) = my::get1stColumn_qouted_string_or_unqouted_string(line1, pos);
	my::clear_quotes(temp);

	m_fullpattern = from_utf8_to_ucs2<std::u16string>(temp);

	if (m_dir.empty() || m_full_store == 0h)
	{
		return LoadLineRetvalue::error; //could not read
	}

	if (m_fullpattern.empty())
	{
		m_fullpattern = uR"((.*)_[0-9]{4}-[0-9]{2}-[0-9]{2}T.*)";
	}

	return LoadLineRetvalue::add;
}

