#pragma once
//#pragma	warning (disable 4996)
#include <vector>
#include "row2.h"
#include <fstream>
#include <iostream>
#include "InitFile.h"

namespace my {
	

	/*! deletes quotes from string*/

	bool readInitFile(std::vector<row> & v, std::wstring& Initfilename) 
	{
		//!
		//file format is:
		//"j:\b\"  4days (ubcd_sklad_2010)_[0-9]{4}-[0-9]{2}-[0-9]{2}T.*
		//
		std::ifstream ifs{ Initfilename };
		if (!ifs.is_open()) //only when file not found
		{


			std::wcerr << "was unable to open file: " << '\n'
				<< Initfilename << '\n'
				// ReSharper disable CppDeprecatedEntity
				<< my::from_utf8_to_ucs2<std::wstring>(strerror(errno));
			// ReSharper restore CppDeprecatedEntity
			return false;
		}
		row ff;

		std::string line1;
		size_t	linenumber = 0;
		while (std::getline(ifs, line1)) {
			linenumber++;
			auto LineReadResult = ff.LoadLine(line1); //actualy loading line into row object
			if (LineReadResult == LoadLineRetvalue::add) {
				ff.line_num_in_file_ = linenumber;
				v.push_back(ff);
				ff.clear();

			}
			else if (LineReadResult == LoadLineRetvalue::ignore) {
			}
			else {
				std::wcerr << "Error: was unable to read line " << linenumber << '\n';
				continue;

			};

		}
		return true;
	}

	/*
	Gets qouted string with spaces or unqouted string without spaces
	*/
	std::pair<std::string, size_t>	
		get1stColumn_qouted_string_or_unqouted_string(const std::string& line1, size_t	startpos)
	{
		//while (startpos != std::string::npos && (line1[startpos] == ' ' || line1[startpos] == '\t' )) startpos++;
		startpos = line1.find_first_not_of(" \t", startpos);
		if (startpos == std::string::npos) return std::make_pair("", std::string::npos);

		auto pos = line1.find_first_of(" \t", startpos);

		std::string dir1 = line1.substr(startpos, pos - startpos);
		/*number of quotes*/
		auto qnumber = my::how_many_quotes(dir1);
		while (pos != std::string::npos && qnumber % 2 == 1)
		{	//string has spaces, for example "c:\program files\"
			auto start = pos;
			pos = line1.find_first_of(" \t", start + 1);
			auto tempstr = line1.substr(start, pos - start);
			qnumber += my::how_many_quotes(tempstr);

			dir1 += tempstr;

		}
		return std::make_pair(dir1, pos);
	}

	/*
	finds how many quotes are there in the string
	*/
	int how_many_quotes(std::string & s) {
		auto pos = s.find('"');
		int res = 0;
		while (pos != std::string::npos)
		{
			res++;
			pos = s.find('"', pos + 1);
		}
		return res;
	}


}