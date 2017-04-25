#pragma once

#include <string>
#include <unordered_map>
#include <algorithm>

namespace Utils
{
	// An immutable table of name-value pairs that allows searching for values
	// case-insensitively and that allows searching for the name corresponding
	// to a value.
	template<class TValue>
	class NameValueTable
	{
	public:
		// Constructs a NameValueTable from a list of name-value pairs.
		NameValueTable(std::initializer_list<std::pair<std::string, TValue>> pairs)
		{
			valuesInOrder.insert(valuesInOrder.begin(), pairs.begin(), pairs.end());
			valuesByName.insert(pairs.begin(), pairs.end());
			for (auto &&pair : pairs)
			{
				auto lowercase = pair.first;
				std::transform(lowercase.begin(), lowercase.end(), lowercase.begin(), tolower);
				valuesByLowercaseName[lowercase] = pair.second;
				valueNames[pair.second] = pair.first;
			}
		}

		// Finds a value by name. Case-sensitive.
		bool FindValue(const std::string &name, TValue *result) const
		{
			auto it = valuesByName.find(name);
			if (it == valuesByName.end())
				return false;
			if (result)
				*result = it->second;
			return true;
		}

		// Finds a value by name. Case-insensitive.
		bool FindValueIgnoreCase(const std::string &name, TValue *result) const
		{
			auto lowercase = name;
			std::transform(lowercase.begin(), lowercase.end(), lowercase.begin(), tolower);
			auto it = valuesByLowercaseName.find(lowercase);
			if (it == valuesByLowercaseName.end())
				return false;
			if (result)
				*result = it->second;
			return true;
		}

		// Finds a name by its value.
		bool FindName(TValue value, std::string *result) const
		{
			auto it = valueNames.find(value);
			if (it == valueNames.end())
				return false;
			if (result)
				*result = it->second;
			return true;
		}

		// Returns whether a name is in the table. Case-sensitive.
		bool ContainsName(const std::string &name) const
		{
			return FindValue(name, nullptr);
		}

		// Returns whether a name is in the table. Case-insensitive.
		bool ContainsNameIgnoreCase(const std::string &name) const
		{
			return FindValueIgnoreCase(name, nullptr);
		}

		// Returns whether a value is in the table.
		bool ContainsValue(TValue value) const
		{
			return FindName(value, nullptr);
		}

		// Returns an iterator to the first name-value pair in the table.
		typename std::vector<std::pair<std::string, TValue>>::const_iterator begin() const
		{
			return valuesInOrder.begin();
		}

		// Returns an iterator to the end of the table.
		typename std::vector<std::pair<std::string, TValue>>::const_iterator end() const
		{
			return valuesInOrder.end();
		}

	private:
		std::vector<std::pair<std::string, TValue>> valuesInOrder;
		std::unordered_map<std::string, TValue> valuesByName;
		std::unordered_map<std::string, TValue> valuesByLowercaseName;
		std::unordered_map<TValue, std::string> valueNames;
	};
}