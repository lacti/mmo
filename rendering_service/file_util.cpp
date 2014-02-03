#include "stdafx.h"
#include "file_util.h"

std::string string_from_text_file(const std::string& filename)
{
    std::string contents;
     
    if (!filename.empty()) {
        std::ifstream ifs(filename.c_str());
        std::string line;
        while(std::getline(ifs, line)) {
            contents += line + "\n";
        }
    }
 
    return contents;
}
