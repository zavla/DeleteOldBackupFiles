#include <fcntl.h> //for _setmode(,_O_U16TEXT)
#include <io.h> //for _setmode(,_O_U16TEXT)
#include <string>
#include <vector>
#include <filesystem>
#include <iostream>
#include <chrono>
#include <regex>
#include <map>

#include "InitFile.h"

//Have made a CLASS regex_traits<char16_t>  specialization for the basic_regex<char16_t>

namespace fs = std::experimental::filesystem;


namespace my {

	using match_results_u16 = std::match_results<std::u16string::const_iterator>;

	enum class return_codes_from_main{ 
		success = 0
		, no_init_file = 1
		, error_reading_init_file	= 2
		
	};


	return_codes_from_main delete_old_backups(std::string& init_file_name); //forward declaration

	void action_on_file ( fs::path & each_file_path ); //forward declaration


	bool is_back_up_file_expired ( fs::path each_file , struct row & folder_name_pair , std::chrono::hours row::* column ) {
		auto ftime = fs::last_write_time ( each_file );
		auto now = std::chrono::system_clock::now ( );
		if ( ( now - ftime ) > folder_name_pair.*column )
		{
			return true;
		}
		return false;
	}
	
	/**
	 * \brief returns true if filename has -FULL  suffix.
	 * \param filename 
	 * \return 
	 */
	template<typename T>
	bool is_this_a_full_backup(const T & filename)
	{
		return (filename.find(u"-FULL") != std::string::npos) 
			|| (filename.find(u"-full") != std::string::npos);

	}

	const wchar_t * to_wchar_ptr(const std::u16string& s)
	{
		
		return reinterpret_cast<const wchar_t *>(s.data());
	};

	/**
	 * \brief Deletes old files according to settings in init file.
	 * \param init_file_name is a init file name. 
	 * \return 0 for success.
	 */
	return_codes_from_main delete_old_backups(std::wstring& init_file_name) {
		
		_setmode(_fileno(stdout), _O_U16TEXT); //to display cyrilic in wcout

		std::vector<row> folders_from_init_file; //from init file, holds folders, file patterns, durations

		if ( !my::readInitFile ( folders_from_init_file , init_file_name ) ) {
			std::wcerr << L"Error reading file " << init_file_name << '\n';
			return my::return_codes_from_main::error_reading_init_file;
		}
		
		//to lookup file name pattern
		const std::basic_regex<char16_t, std::regex_traits<char16_t>>
			nashfile(u"[(](.*)[)].*"); 
		
				
		match_results_u16 results;

		for ( auto& one_init_row : folders_from_init_file )
		{
			auto folder_name = one_init_row.m_dir;
			
			//FilesPatternModTime is a multimap with file names and modification times
			std::multimap< std::u16string , fs::file_time_type>
				files_pattern_mod_time; 

			//files are grouped  by first letters(petterns) in the name loaded from init file
			
			
			std::u16string			start_symbols_of_file_name{};
			if (std::regex_match ( one_init_row.m_fullpattern , results , nashfile)) { //to do not delete any other files then current pattern
				if (results.size() > 1)
				{
					start_symbols_of_file_name = results[1];
				}
			}
			if (start_symbols_of_file_name.empty()) {
				std::wcout << L"An error in file name pattern at ini file: \n" << to_wchar_ptr(one_init_row.m_fullpattern) << '\n';
				/*std::wcout << reinterpret_cast<const wchar_t *>(one_init_row.m_fullpattern.data());*/
				continue;
			}
			
			//path objects (files) for every line of init file
			std::vector<fs::path>		all_files ; 
			
			my::clear_quotes ( folder_name);

			std::chrono::hours	row::*		column;
			column = &row::m_full_store;

			//for each line of init file there is a pattern of files name
			std::basic_regex<char16_t>		rexp ( one_init_row.m_fullpattern );

			if ( ! fs::exists(folder_name) 
				|| ! fs::is_directory(folder_name))
			{
				//system("chcp 1251");
				//std::cout << GetConsoleOutputCP();
				_setmode(_fileno(stdout),_O_U16TEXT);
				std::wcout << L"No folder found: " << to_wchar_ptr(folder_name) << " from init file line number " << one_init_row.line_num_in_file_ << '\n';
				continue;
			}
			//1st pass
			fs::directory_iterator		each_file_iterator ( folder_name );
			for ( const auto& each_file : each_file_iterator )
			{
				auto filename = each_file.path ( ).filename ( ).u16string();
				bool this_is_a_full_back_up = is_this_a_full_backup(filename);


				//decision on file timestamp in general
				if ( !is_back_up_file_expired ( each_file , one_init_row , column ) ) {
					continue; //leave it
				}
				if (filename.substr(0,start_symbols_of_file_name.length()) == start_symbols_of_file_name) { //to do not delete any other files then current pattern

					all_files.push_back(each_file.path());
				} 
				else {
					continue; 
				}

				
				if ( this_is_a_full_back_up ) //for full backups only
				{

					auto mod_time = fs::last_write_time ( each_file.path ( ) ); //chrono::time_point

					//at first pass create multimap<indexname,mod_time>  FilesPatternModTime

					
					if ( std::regex_match ( filename , results , rexp ) ) {
						if ( results.size ( ) > 1 )
						{
							auto sub_match	= results [ 1 ];
							//the multimap with file names and modification times of full copies
							files_pattern_mod_time.emplace ( sub_match.str ( ) , mod_time ); 
						}
					}
				}

			}
			
			//2nd pass over selected at 1st pass files to decide if there is a newer copy of full copy of this database backup files 

			for ( auto it = all_files.cbegin ( ); it != all_files.cend ( ); ++it ) {
				
				auto each_file_path = *it;
				
				auto filename = each_file_path.filename ( ).u16string ( );
				fs::file_time_type		mod_time;

				if (my::is_this_a_full_backup(filename))
				{

					if ( std::regex_match ( filename , results , rexp ) )
					{
						if ( results.size ( ) > 1 )
						{
							try
							{
								mod_time = fs::last_write_time(each_file_path);
							}
							catch (const std::exception&)
							{
								continue;
							}

							auto sub_result = results [ 1 ];
							auto first_chars = sub_result.str ( );
							auto pair_of_itrs = files_pattern_mod_time.equal_range ( first_chars );
							bool may_be_deleted { false };

							if ( ( std::distance ( pair_of_itrs.first , pair_of_itrs.second ) ) > 1 ) //if there are more then 1 copy
							{
								for ( auto i = pair_of_itrs.first
									; i != pair_of_itrs.second
									; i++ )
								{
									if ( mod_time < i->second )
									{
										//there is a new copy
										may_be_deleted = true;
										break;
									}
								}
							}
							if ( may_be_deleted )
							{

								auto todel2 = each_file_path.string ( );
								action_on_file ( each_file_path );

							}

						}
					}

				}
				else //not a full copy
				{
					action_on_file ( each_file_path );
				};

			}
		}
		return my::return_codes_from_main::success;
	}

	void action_on_file ( fs::path& each_file_path ) {
		try
		{

#ifdef _DEBUG
			/*fs::path	newname ( each_file_path );
			newname.replace_filename ( "_" + each_file_path.filename ( ).string ( ) );
			fs::rename ( each_file_path , newname );
			*/
			std::wcout << L"deleting file " << each_file_path.filename().wstring() << "\n";
#else

			fs::remove ( each_file_path );
#endif // DEBUGRENAME
		}
		catch ( const std::exception& )
		{
			//continue;

		}

	}

} //namespace

int main ( size_t argc , char* args[] ) {
	if ( argc != 2 )
	{
		std::wcerr << L"Usage: DeleteOldFiles <full_path_to_init_file>" << '\n';

		std::wcout << R"(init file content are lines with:
		"j:\b\"  4days (databasenamehere)_[0-9]{4}-[0-9]{2}-[0-9]{2}T.*
		)";
		return 1;
	}
	auto init_file_name = std::wstring ( my::from_utf8_to_ucs2 < std::wstring > (args [ 1 ]) );
	//Google for education
	//Michael Kaplan
	return static_cast<int>( my::delete_old_backups ( init_file_name ) );
}