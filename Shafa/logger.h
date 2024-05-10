#pragma once

#include <iostream>
#include <string>
#include <print>
#include <cstdio>
#include <format>

namespace shafa {
	typedef enum class LogLevel
	{
		Info = 0x00,
		Debug = 0x01,
		Warning = 0x02,
		Error = 0x03,
	} LogLevel;

	class logger
	{
	public:
		logger() = default;
		~logger() = default;

		static void log(const std::wstring_view& msg, LogLevel logLevel = LogLevel::Debug)
		{ 
			switch (logLevel)
			{
			case LogLevel::Info:
				std::wcout << std::format(L"INFO: \t{}", msg) << std::endl;
				break;
			case LogLevel::Debug:
#ifdef _DEBUG
				std::wcout << std::format(L"DEBUG: \t{}", msg) << std::endl;
#endif // DEBUG
				break;
			case LogLevel::Warning:
				std::wclog << std::format(L"WARNING: \t{}", msg)  << std::endl;
				break;
			case LogLevel::Error:
				std::wcerr << std::format(L"ERROR: \t{}", msg) << std::endl;
				break;
			default:
				break;
			}

		}
		static void log_new_line()
		{
			std::wcout << '\n';
		}
	};
}
