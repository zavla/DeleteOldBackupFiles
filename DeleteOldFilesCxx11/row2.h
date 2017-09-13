#pragma once
#include <string>
#include <chrono>

namespace my {
	enum class LoadLineRetvalue { add, ignore, error };
	/*!a class representing a row in an init file
	*/
	struct row { 
		std::u16string					m_dir;
		std::chrono::hours				m_full_store;
		std::u16string					m_fullpattern;
		size_t				line_num_in_file_;
		

		//friend bool operator >> ( std::ifstream& ifs , row & ff );
		LoadLineRetvalue LoadLine(std::string& line1);
		void clear();

		std::chrono::hours HowManyHours(std::string& str);
	};

}