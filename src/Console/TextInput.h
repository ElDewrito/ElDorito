#pragma once
#include <string>

class TextInput
{
public:

	std::string currentInput;
	int currentPointerIndex = 0;

	TextInput();
	~TextInput();

	void type(char c)
	{
		currentInput.insert(currentPointerIndex, &c);
		currentPointerIndex++;
	}

	void backspace()
	{
		if (currentPointerIndex > 0) {
			currentInput.erase(--currentPointerIndex, 1);
		}
	}

	void del()
	{
		if (currentPointerIndex > 0 && currentPointerIndex < currentInput.size()) {
			currentInput.erase(currentPointerIndex, 1);
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