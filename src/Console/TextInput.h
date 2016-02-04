#pragma once
#include <string>

class TextInput
{
public:

	std::string currentInput;
	size_t currentPointerIndex = 0;

	TextInput() { }
	~TextInput() { }

	void type(char c)
	{
		currentInput.insert(currentPointerIndex, 1, c);
		currentPointerIndex++;
	}

	void type(const std::string& str)
	{
		currentInput.insert(currentPointerIndex, str);
		currentPointerIndex += str.length();
	}

	void backspace()
	{
		if (currentPointerIndex > 0) {
			currentInput.erase(--currentPointerIndex, 1);
		}
	}

	void del()
	{
		if (currentPointerIndex < currentInput.size()) {
			currentInput.erase(currentPointerIndex, 1);
		}
	}

	void home()
	{
		if (currentPointerIndex != 0)
		{
			currentPointerIndex == 0;
		}
	}

	void end()
	{
		if (currentPointerIndex != -1)
		{
			currentPointerIndex == -1;
		}
	}

	void left()
	{
		if (currentPointerIndex > 0)
		{
			currentPointerIndex--;
		}
	}

	void right()
	{
		if (currentPointerIndex < currentInput.size())
		{
			currentPointerIndex++;
		}
	}

	void set(std::string basic_string)
	{
		currentInput = basic_string;
		currentPointerIndex = basic_string.length();
	}

};
