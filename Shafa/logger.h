#pragma once

#include <iostream>
#include <string>
#include <print>
#include <cstdio>
#include <format>

#include <windows.h>

namespace shafa {

#define LOG_INFO(msg) logger::log(msg, shafa::LogLevel::Info, {__FUNCTIONW__, __FILEW__}, __LINE__);
#define LOG_DEBUG(msg) logger::log(msg, shafa::LogLevel::Debug, {__FUNCTIONW__, __FILEW__}, __LINE__);
#define LOG_WARNING(msg) logger::log(msg, shafa::LogLevel::Warning, {__FUNCTIONW__, __FILEW__}, __LINE__);
#define LOG_ERROR(msg) logger::log(msg, shafa::LogLevel::Error, {__FUNCTIONW__, __FILEW__}, __LINE__);

	typedef struct Color
	{
		static inline constexpr WORD GREY = 0x0008;
		static inline constexpr WORD WHITE = 0x000F;
		static inline constexpr WORD GREEN = 0x0002;
		static inline constexpr WORD RED = 0x0004;
		static inline constexpr WORD YELLOW = 0x0006;
		static inline constexpr WORD BLUE = 0x0001;

		static inline constexpr WORD INTENSITY = 0x0008;
	};


	typedef enum class LogLevel
	{
		Info = 0x00,
		Debug = 0x01,
		Warning = 0x02,
		Error = 0x03,
	} LogLevel;

	typedef struct FunctStats
	{
		std::wstring_view funcName{ L"null" };
		std::wstring_view fileName{ L"null" };
	} FunctStats;

	class logger
	{
	public:
		~logger() = default;

		static void log(const std::wstring_view& msg, LogLevel logLevel, const FunctStats& functionStats = {}, std::int32_t line = 0)
		{ 
			HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

			CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
			GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
			WORD originalAttributes = consoleInfo.wAttributes;

			switch (logLevel)
			{
			case LogLevel::Info:
				SetConsoleTextAttribute(hConsole, Color::WHITE | Color::INTENSITY);
				std::wcout << std::format(L"INFO: \t{}", msg) << std::endl;
				break;
			case LogLevel::Debug:
#				ifdef _DEBUG
					SetConsoleTextAttribute(hConsole, Color::BLUE | Color::INTENSITY);
					std::wcout << std::format(L"DEBUG ({}(), line {}): \t{}", functionStats.funcName, line, msg) << std::endl;
#				endif // DEBUG
				break;
			case LogLevel::Warning:
				SetConsoleTextAttribute(hConsole, Color::YELLOW | Color::INTENSITY);
				std::wclog << std::format(L"WARNING ({}(), line {}): \t{}", functionStats.funcName, line, msg)  << std::endl;
				break;
			case LogLevel::Error:
				SetConsoleTextAttribute(hConsole, Color::RED | Color::INTENSITY);
				std::wcerr << std::format(L"ERROR ({}(), line {}): \t{}", functionStats.funcName, line, msg) << std::endl;
				break;
			}

			SetConsoleTextAttribute(hConsole, originalAttributes);
		}
		static void log_new_line()
		{
			std::wcout << '\n';
		}
	};
}
