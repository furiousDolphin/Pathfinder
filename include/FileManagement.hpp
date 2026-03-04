
#ifndef FILEMANAGEMENT_HPP_
#define FILEMANAGEMENT_HPP_

#include <stdexcept>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

namespace fs = std::filesystem;
using CSV = std::vector<std::vector<double>>;

CSV load_csv( const std::string& path );
void save_csv( const std::string& path, const CSV& data );



#endif