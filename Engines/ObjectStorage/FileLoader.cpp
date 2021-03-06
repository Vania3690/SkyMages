#include "FileLoader.hpp"
#include <sstream>
#include "DefaultObjectStorageData.hpp"
void FileLoader::parse_first_line(std::string const& line, std::string const& file_extention) {
	std::istringstream s(line);
	std::string string;
	s >> string;
	if (string.size() < 4 || string.substr(string.size() - 4) != file_extention)
		throw Exceptions::FileParsingException("File seems to be corrupted");
	s >> string >> string >> string;
	if (string != "ObjectStorage")
		throw Exceptions::FileParsingException("File seems to be corrupted");

	char placeholder;
	size_t major, minor, patch, build;
	s >> placeholder >> major >> placeholder >> minor;
	if (major != Object_Storage_Syntax_Major_Version || minor != Object_Storage_Syntax_Minor_Version)
		throw Exceptions::FileVersionException("File storage version is different from the expected one.");

	s >> string >> string >> string;
	if (string != Program_Name)
		throw Exceptions::FileParsingException("File seems to be corrupted");

	s >> placeholder >> major >> placeholder >> minor >> placeholder >> patch >>
		placeholder >> build >> placeholder >> string;
	if (major != Program_Major_Version || minor != Program_Minor_Version || patch != Program_Patch_Version
		|| build != Program_Build_Version || string != Program_Version_Suffix)
	{
		//[[deprecated]] while program being actively developed.
		//throw Exceptions::ProgramVersionException("Program version is different from the expected one.");
	}

	std::getline(s, string);
	parse_file_type_info(string);
}

std::string FileLoader::generate_first_line(std::string const& filename, std::string const& file_extention) {
	std::ostringstream s;
	s << filename << file_extention << " is an ObjectStorage v" << Object_Storage_Syntax_Major_Version << "." << Object_Storage_Syntax_Minor_Version << " file for " << Program_Name
		<< " v" << Program_Major_Version << "." << Program_Minor_Version << "." << Program_Patch_Version << "." << Program_Build_Version << "_" << Program_Version_Suffix << " "
		<< generate_file_type_info();
	return s.str();
}

#include <filesystem>
#include <fstream>
#include "Shared/SharedFunctions.hpp"
void FileLoader::load(std::string const& path_string, std::string const& file_extention) {
	for (auto path : Functions::split_string(path_string))
		if (std::filesystem::exists(std::filesystem::current_path().generic_string() + path))
			for (auto file : std::filesystem::directory_iterator(std::filesystem::current_path().generic_string() + path))
				if (auto temp = file.path().generic_string(); temp.substr(temp.size() - 4) == file_extention) {
					std::ifstream f;
					f.open(temp);
					if (!f.is_open())
						throw Exceptions::FileCannotBeOpennedException("Filesystem parsing failure detected");

					bool first_line_parsed = false;
					std::string temp_line;
					while (std::getline(f, temp_line))
						if (temp_line.size() > 0u && temp_line.at(0) != '#')
							if (first_line_parsed)
								parse_line(temp_line);
							else {
								parse_first_line(temp_line, file_extention);
								first_line_parsed = true;
							}
				}
	finalize_parsing();
}
