#include <string>
#include <fstream>
#include <sstream>

#ifndef UTILS_HPP_
#define UTILS_HPP_

namespace Utils {
    template <typename T>
    void copyFileContent(T &buffer, const std::string &filename)
    {
        std::ifstream css(filename);
        std::ostringstream ss;
        ss << css.rdbuf();
        buffer += ss.str();
    }
}
#endif /* !UTILS_HPP_ */
