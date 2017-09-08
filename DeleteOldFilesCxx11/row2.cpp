#include "row2.h"
#include "InitFile.h"
#include <utility>
#include <tuple>
#include <chrono>


void my::row::clear() {
	using namespace std::chrono_literals;
	m_dir = "";
	m_full_store = 0h;
	m_fullpattern = "";
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
	int full_store = 0, diff_store = 0;
	std::string dir1;
	std::size_t pos;

	std::tie(m_dir, pos) = my::get1stColumn_qouted_string_or_unqouted_string(line1, 0);
	//field #2 
	std::string DurationField1, DurationField2;

	std::tie(DurationField1, pos) = my::get1stColumn_qouted_string_or_unqouted_string(line1, pos);
	m_full_store = HowManyHours(DurationField1);

	//filed #3
	std::tie(m_fullpattern, pos) = my::get1stColumn_qouted_string_or_unqouted_string(line1, pos);
	clear_quotes(m_fullpattern);


	if (m_dir.empty() || m_full_store == 0h)
	{
		return LoadLineRetvalue::error; //could not read
	}

	if (m_fullpattern.empty())
	{
		m_fullpattern = R"((.*)_[0-9]{4}-[0-9]{2}-[0-9]{2}T.*)";
	}
	/*
	auto FULLstring = "-FULL";
	m_fullpattern += FULLstring; //prevent abuse, what about other files

	if ( m_elsepattern.empty ( ) )
	{
	m_elsepattern = m_fullpattern;
	auto subststring = "-diff"s;
	auto posOfFULLstring = m_elsepattern.rfind ( FULLstring );

	m_elsepattern.replace (
	posOfFULLstring ,
	std::string::npos ,
	subststring
	);

	}*/

	return LoadLineRetvalue::add;
}

