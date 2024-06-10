#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <conio.h>
#include <Windows.h>

namespace shafa {
	struct sftools
	{
		template<typename T>
		static inline bool compare_vectors(const std::vector<T>& vec1, const std::vector<T>& vec2)
		{
			if (vec1.size() != vec2.size())
			{
				return false;
			}

			for (std::size_t i = 0; i < vec1.size(); i++)
			{
				if (vec1[i] != vec2[i])
				{
					return false;
				}
			}

			return true;
		}

		template<typename T>
		static inline std::uint16_t  make_option(std::vector<T> options)
		{
            std::uint16_t currentOption = 0;

            HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
            DWORD consoleMode;
            GetConsoleMode(hConsole, &consoleMode);
            SetConsoleMode(hConsole, consoleMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);

            while (true) {
                system("cls");

                for (std::int32_t i = 0; i < options.size(); ++i) {
                    std::wcout << "\x1b[" << (i == currentOption ? "7m" : "0m") << options[i] << "\x1b[0m\n";
                }

                std::int32_t key = _getch();

                if (key == 224) {
                    key = _getch();
                    switch (key) {
                    case 72: 
                        currentOption = (currentOption - 1 + options.size()) % options.size();
                        break;
                    case 80:
                        currentOption = (currentOption + 1) % options.size();
                        break;
                    }
                }
                else if (key == 13) {
                    system("cls");
                    return currentOption;
                }
            }
		}

		static std::wstring remove_char(const std::wstring& str, wchar_t ch)
		{
			std::wstring newStr;
			for (const auto& c : str)
			{
				if (c != ch)
				{
					newStr.push_back(c);
				}
			}
			return newStr;
		}
	};
}
