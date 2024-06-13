#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <conio.h>
#include <Windows.h>
#include <toml++/toml.hpp>

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

        static inline void clear_console()
        {
            HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
            CONSOLE_SCREEN_BUFFER_INFO csbi;
            DWORD count;
            DWORD cellCount;
            COORD homeCoords = { 0, 0 };

            if (hConsole == INVALID_HANDLE_VALUE) return;

            if (!GetConsoleScreenBufferInfo(hConsole, &csbi)) return;
            cellCount = csbi.dwSize.X * csbi.dwSize.Y;

            if (!FillConsoleOutputCharacter(hConsole, (TCHAR)' ', cellCount, homeCoords, &count)) return;

            if (!FillConsoleOutputAttribute(hConsole, csbi.wAttributes, cellCount, homeCoords, &count)) return;

            SetConsoleCursorPosition(hConsole, homeCoords);
        }

        template<typename T>
        static inline void display_menu(const std::vector<T>& options, const std::wstring& label, std::uint16_t currentOption)
        {
            clear_console();
            std::wcout << label << L"\n\n";
            for (std::int32_t i = 0; i < options.size(); ++i) {
                if (i == currentOption) {
                    std::wcout << L"\x1b[7m" << options[i] << L"\x1b[0m\n";
                }
                else {
                    std::wcout << options[i] << L"\n";
                }
            }
        }

        template<typename T>
        static inline std::uint16_t make_option(const std::vector<T>& options, const std::wstring& label)
        {
            std::uint16_t currentOption = 0;

            HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
            DWORD consoleMode = 0;
            GetConsoleMode(hConsole, &consoleMode);
            SetConsoleMode(hConsole, consoleMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);

            while (true) {
                display_menu(options, label, currentOption);

                int key = _getch();

                if (key == 224) {
                    key = _getch();
                    switch (key) {
                    case 72:
                        currentOption = (currentOption == 0) ? options.size() - 1 : currentOption - 1;
                        break;
                    case 80:
                        currentOption = (currentOption + 1) % options.size();
                        break;
                    }
                }
                else if (key == 13) {
                    clear_console();
                    return currentOption;
                }
            }
        }

        static inline std::wstring remove_char(const std::wstring& str, wchar_t ch)
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

        static inline std::vector<std::filesystem::path> split_path(const std::filesystem::path& path) {
            std::vector<std::filesystem::path> components;
            for (const auto& part : path) {
                components.push_back(part);
            }
            return components;
        }

        static inline std::filesystem::path find_highest_common_path(const std::vector<std::filesystem::path>& paths) {
            if (paths.empty()) {
                return "";
            }

            std::vector<std::filesystem::path> referenceComponents = split_path(paths[0]);
            size_t commonLength = referenceComponents.size();

            for (const auto& path : paths) {
                std::vector<std::filesystem::path> components = split_path(path);
                commonLength = min(commonLength, components.size());
                for (size_t i = 0; i < commonLength; ++i) {
                    if (referenceComponents[i] != components[i]) {
                        commonLength = i;
                        break;
                    }
                }
            }

            std::filesystem::path commonPath;
            for (size_t i = 0; i < commonLength; ++i) {
                commonPath /= referenceComponents[i];
            }

            return commonPath;
        }

        static inline std::vector<std::wstring> string_split(const std::wstring& str) {
            std::vector<std::wstring> result;
            std::wistringstream iss(str);
            for (std::wstring s; iss >> s; )
                result.push_back(s);
            return result;
        }

        static inline toml::array vector_to_toml_array(const std::vector<std::wstring>& vec) {
			toml::array arr;
			for (const auto& item : vec) {
				arr.push_back(wstring_to_string(item));
			}
			return arr;
		}

        static inline toml::array wstring_to_toml_array(const std::wstring& vec) {
            return vector_to_toml_array(string_split(vec));
        }


    };

}
