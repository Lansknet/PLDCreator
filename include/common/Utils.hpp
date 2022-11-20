#ifndef UTILS_HPP_
#define UTILS_HPP_

#include <iterator>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <functional>
#include <unordered_map>
#include <algorithm>

namespace Utils
{
	using Buffer = std::string;

	inline void trim(std::string& str, char c)
	{
		str.erase(std::remove_if(str.begin(), str.end(), [&](const auto& item)
		{
			return item == c;
		}), str.end());
	}

	inline void findAndReplace(Buffer& buffer, const std::string& toReplace, const std::string& replaceWith)
	{
		size_t pos = buffer.find(toReplace);
		while (pos != std::string::npos)
		{
			buffer.replace(pos, toReplace.length(), replaceWith);
			pos = buffer.find(toReplace, pos + replaceWith.length());
		}
	}

	template<typename T>
	void copyFileContent(T& buffer, const std::string& filename)
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
			for (std::size_t j = 0; j <= maxSize; j++)
			{
				std::vector<std::string> s;
				for (const auto& x: zip)
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
			for (std::size_t j = 0; j <= maxSize; j++)
			{
				std::vector<std::string> s;
				for (const auto& [key, value]: zip)
				{
					if (j <= value.size() - 1)
						s.push_back(value[j]);
					else s.push_back(T());
				}
				f(s);
			}
		}
	};

	std::string Base64Encode(unsigned char const* buf, unsigned int bufLen)
	{
		std::string ret;
		int i = 0;
		int j = 0;
		unsigned char char_array_3[3];
		unsigned char char_array_4[4];
		static const std::string base64_chars =
				"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
				"abcdefghijklmnopqrstuvwxyz"
				"0123456789+/";
		while (bufLen--)
		{
			char_array_3[i++] = *(buf++);
			if (i == 3)
			{
				char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
				char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
				char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
				char_array_4[3] = char_array_3[2] & 0x3f;

				for (i = 0; (i < 4); i++)
					ret += base64_chars[char_array_4[i]];
				i = 0;
			}
		}

		if (i)
		{
			for (j = i; j < 3; j++)
				char_array_3[j] = '\0';

			char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
			char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
			char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
			char_array_4[3] = char_array_3[2] & 0x3f;

			for (j = 0; (j < i + 1); j++)
				ret += base64_chars[char_array_4[j]];

			while ((i++ < 3))
				ret += '=';
		}

		return ret;
	}

	template<typename T>
	void copyImageContent(T& buffer, const std::string& filename)
	{
		std::ifstream input(filename, std::ios::binary);
		std::stringstream ss;
		ss << input.rdbuf();
		buffer += Utils::Base64Encode(reinterpret_cast<const unsigned char*>(ss.str().c_str()), ss.str().size());
	}

}
#endif /* !UTILS_HPP_ */
