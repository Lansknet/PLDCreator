#include <iterator>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <functional>
#include <unordered_map>
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

    template<typename T>
    class Zip
    {
        public:
            Zip(std::function<void(const std::vector<T>&)> f, std::vector<std::vector<T>> zip)
            {
                std::size_t maxSize = zip.size();
                for (std::size_t j = 0; j <= maxSize; j++) {
                    std::vector<std::string> s;
                    for (const auto &x : zip)
                    {
                        if (j <= x.size() - 1)
                            s.push_back(x[j]);
                        else s.push_back(T());
                    }
                    f(s);
                }
            }

            Zip(std::function<void(const std::vector<T>&)> f, std::unordered_map<T, std::vector<T>> zip)
            {
                std::size_t maxSize = zip.size();
                for (std::size_t j = 0; j <= maxSize; j++) {
                    std::vector<std::string> s;
                    for (const auto &[key, value] : zip)
                    {
                        if (j <= value.size() - 1)
                            s.push_back(value[j]);
                        else s.push_back(T());
                    }
                    f(s);
                }
            }
    };
}
#endif /* !UTILS_HPP_ */
