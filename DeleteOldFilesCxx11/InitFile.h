#pragma once
#include <utility>
#include <string>
#include <vector>
#include "row2.h"

//forward declarations of member functions working with row class.

namespace my {

	void clear_quotes(std::string& quoted_string);

	/*
	Gets qouted string with spaces or unqouted string without spaces
	*/
	std::pair<std::string, size_t>	get1stColumn_qouted_string_or_unqouted_string(const std::string& line1, size_t	startpos);
	
	int how_many_quotes(std::string & s);
	
	bool readInitFile(std::vector<row> & v, std::string& Initfilename);


}