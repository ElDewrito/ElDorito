#include "String.h"

// STL
#include <string>
#include <algorithm>
#include <vector>
#include <sstream>
#include <functional> 
#include <cctype>
#include <codecvt>

namespace Utils
{
	namespace String
	{
		void ReplaceCharacters(std::string& str, char find, char replaceWith)
		{
			for( unsigned i = 0; i < str.length(); ++i )
				if( str[i] == find )
					str[i] = replaceWith;
		}

		std::wstring WidenString(const std::string &s)
		{
			std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> utf16conv;
			return utf16conv.from_bytes(s);
		}

		std::string ThinString(const std::wstring &str)
		{
			std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> utf8conv;
			return utf8conv.to_bytes(str);
		}

		std::string ToLower(const std::string &str)
		{
			std::string retValue(str);
			std::transform(retValue.begin(), retValue.end(), retValue.begin(), ::tolower);
			return retValue;
		}

		std::vector<std::string> SplitString(const std::string &stringToSplit, char delim)
		{
			std::vector<std::string> retValue;
			std::stringstream ss(stringToSplit);
			std::string item;
			while( std::getline(ss, item, delim) )
			{
				retValue.push_back(item);
			}
			return retValue;
		}

		// trim from start
		static inline std::string &ltrim(std::string &s)
		{
			s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
			return s;
		}

		// trim from end
		static inline std::string &rtrim(std::string &s)
		{
			s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
			return s;
		}

		// trim from both ends
		static inline std::string &trim(std::string &s)
		{
			return ltrim(rtrim(s));
		}

		std::string Trim(const std::string &string, bool fromEnd)
		{
			std::string retValue(string);
			if( fromEnd ) // From End
			{
				retValue.erase(std::find_if(retValue.rbegin(), retValue.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), retValue.end());
			}
			else // From Start
			{
				retValue.erase(retValue.begin(), std::find_if(retValue.begin(), retValue.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
			}

			return retValue;
		}

		std::vector<std::string> Wrap(const std::string &string, size_t lineLength)
		{
			std::stringstream sstream;
			sstream << string;

			std::vector<std::string> lines;
			while( sstream.good() ) // Copy all lines into string
			{
				std::string currentLine;
				std::getline(sstream, currentLine);
				lines.push_back(currentLine);
			}

			std::vector<std::string> retValue;

			for( unsigned i = 0; i < lines.size(); ++i )
			{
				std::string currentLine(lines[i]);
				while( currentLine.length() > lineLength )
				{
					int wordEnd = lineLength;
					while( wordEnd > 0 && !std::isspace(currentLine[wordEnd]) )
						--wordEnd;
					if( wordEnd <= 0 )
						wordEnd = lineLength;

					retValue.push_back(currentLine.substr(0, wordEnd));
					currentLine = currentLine.substr(wordEnd, std::string::npos);
				}

				retValue.push_back(currentLine);
			}

			return retValue;
		}
	}
}