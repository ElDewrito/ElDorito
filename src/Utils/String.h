#pragma once

#include <string>
#include <vector>

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
	}
}