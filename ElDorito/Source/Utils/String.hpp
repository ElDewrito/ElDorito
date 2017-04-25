#pragma once

#include <string>
#include <vector>
#include <sstream>

namespace Utils
{
	namespace String
	{
		std::string Base64Encode(unsigned char const* bytes_to_encode, unsigned int in_len);
		std::string Base64Decode(std::string const& encoded_string);
		int Base64DecodeBinary(char* b64message, unsigned char* buffer, size_t* length);

		void RemoveCharsFromString(std::string &str, char* charsToRemove);

		void HexStringToBytes(const std::string &in, void *const data, size_t length);
		void BytesToHexString(void *const data, const size_t dataLength, std::string &dest);

		std::string ToLower(const std::string &str);

		void ReplaceCharacters(std::string& str, char replace, char with);
		bool ReplaceString(std::string &str, const std::string &replace, const std::string &with);

		std::wstring WidenString(const std::string &str);
		std::string ThinString(const std::wstring &str);

		std::vector<std::string> SplitString(const std::string &stringToSplit, char delim = ' ');

		std::string Trim(const std::string &string, bool fromEnd = true);

		std::vector<std::string> Wrap(const std::string &string, size_t lineLength);

		template<class T, class InputIt>
		std::string Join(InputIt first, InputIt last, const char *delim = " ")
		{
			std::ostringstream stream;
			std::copy(first, last, std::ostream_iterator<T>(stream, delim));
			auto str = stream.str();
			str.erase(str.length() - 1);
			return str;
		}

		template<class T>
		std::string Join(const std::vector<T> &vec, const char *delim = " ")
		{
			return Join<T>(vec.begin(), vec.end(), delim);
		}
	}
}