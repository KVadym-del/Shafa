#pragma once
#include <vector>
#include <string>
#include <utility>
#include <unordered_map>
#include <filesystem>

#include "wexception.h"


namespace shafa {
	typedef enum class sfArgEnum {
		none = 0x00,
		help = 0x01,
		configure = 0x02,
		build = 0x03,
		run = 0x04,
	} sfArgEnum;
	struct sfArgEnumHelper
	{
		inline static std::wstring to_string(const sfArgEnum& argEnum)
		{
			static const std::unordered_map<sfArgEnum, std::wstring> argEnumMap
			{
				{sfArgEnum::none, L"none"},
				{sfArgEnum::help, L"help"},
				{sfArgEnum::configure, L"configure"},
				{sfArgEnum::build, L"build"},
				{sfArgEnum::run, L"run"}
			};

			auto it = argEnumMap.find(argEnum);
			if (it != argEnumMap.end()) {
				return it->second;
			}

			return L"none";
		}


		inline static sfArgEnum to_enum(const std::wstring& argEnumStr)
		{
			static const std::unordered_map<std::wstring, sfArgEnum> argEnumMap
			{
				{L"none", sfArgEnum::none},
				{L"help", sfArgEnum::help},
				{L"configure", sfArgEnum::configure},
				{L"build", sfArgEnum::build},
				{L"run", sfArgEnum::run}
			};

			auto it = argEnumMap.find(argEnumStr);
			if (it != argEnumMap.end()) {
				return it->second;
			}

			return sfArgEnum::none;
		}
	};

	typedef struct sfArg
	{
		sfArgEnum argEnum;
		std::vector<std::wstring> argEdit;
	} sfArg;

	/*------------------------------------------------------------------*/

	typedef enum class sfCppVersions
	{
		cpp11 = 0x00,
		cpp14 = 0x01,
		cpp17 = 0x02,
		cpp20 = 0x03,
		cpp23 = 0x04,
		cpp26 = 0x05,
	} sfCppVersions;
	struct sfCppVersionsHelper
	{
		inline static std::wstring to_string(const sfCppVersions& cppVersion)
		{
			static const std::unordered_map<sfCppVersions, std::wstring> cppVersionMap
			{
				{sfCppVersions::cpp11, L"c++11"},
				{sfCppVersions::cpp14, L"c++14"},
				{sfCppVersions::cpp17, L"c++17"},
				{sfCppVersions::cpp20, L"c++20"},
				{sfCppVersions::cpp23, L"c++23"},
				{sfCppVersions::cpp26, L"c++26"}
			};

			auto it = cppVersionMap.find(cppVersion);
			if (it != cppVersionMap.end()) {
				return it->second;
			}

			return L"c++20";
		}

		inline static sfCppVersions to_enum(const std::wstring& cppVersionStr)
		{
			static const std::unordered_map<std::wstring, sfCppVersions> cppVersionMap
			{
				{L"c++11", sfCppVersions::cpp11},
				{L"c++14", sfCppVersions::cpp14},
				{L"c++17", sfCppVersions::cpp17},
				{L"c++20", sfCppVersions::cpp20},
				{L"c++23", sfCppVersions::cpp23},
				{L"c++26", sfCppVersions::cpp26}
			};

			auto it = cppVersionMap.find(cppVersionStr);
			if (it != cppVersionMap.end()) {
				return it->second;
			}

			return sfCppVersions::cpp20;
		}
	};

	typedef enum class sfCppCompilers
	{
		clang = 0x00,
		msvc = 0x01,
		gcc = 0x02,
	} sfCppCompilers;
	struct sfCppCompilersHelper
	{
		inline static std::wstring to_string(const sfCppCompilers& cppCompiler)
		{
			static const std::unordered_map<sfCppCompilers, std::wstring> cppCompilerMap
			{
				{sfCppCompilers::clang, L"clang"},
				{sfCppCompilers::msvc, L"msvc"},
				{sfCppCompilers::gcc, L"gcc"}
			};

			auto it = cppCompilerMap.find(cppCompiler);
			if (it != cppCompilerMap.end()) {
				return it->second;
			}

			return L"msvc";
		}

		inline static sfCppCompilers to_enum(const std::wstring& cppCompilerStr)
		{
			static const std::unordered_map<std::wstring, sfCppCompilers> cppCompilerMap
			{
				{L"clang", sfCppCompilers::clang},
				{L"msvc", sfCppCompilers::msvc},
				{L"gcc", sfCppCompilers::gcc}
			};

			auto it = cppCompilerMap.find(cppCompilerStr);
			if (it != cppCompilerMap.end()) {
				return it->second;
			}

			return sfCppCompilers::msvc;
		}
	};

	/*------------------------------------------------------------------*/

	typedef enum class sfProjectType
	{
		application = 0x00,
		staticLibrary = 0x01,
		dynamicLibrary = 0x02,
	} sfProjectType;
	struct sfProjectTypeHelper
	{
		inline static std::wstring to_string(const sfProjectType& projectType)
		{
			static const std::unordered_map<sfProjectType, std::wstring> projectTypeMap
			{
				{sfProjectType::application, L"application"},
				{sfProjectType::staticLibrary, L"staticLibrary"},
				{sfProjectType::dynamicLibrary, L"dynamicLibrary"}
			};

			auto it = projectTypeMap.find(projectType);
			if (it != projectTypeMap.end()) {
				return it->second;
			}

			return L"application";
		}

		inline static sfProjectType to_enum(const std::wstring& projectTypeStr)
		{
			static const std::unordered_map<std::wstring, sfProjectType> projectTypeMap
			{
				{L"application", sfProjectType::application},
				{L"staticLibrary", sfProjectType::staticLibrary},
				{L"dynamicLibrary", sfProjectType::dynamicLibrary}
			};

			auto it = projectTypeMap.find(projectTypeStr);
			if (it != projectTypeMap.end()) {
				return it->second;
			}

			throw wruntime_error(L"Invalid project type. Expected: application, staticLibrary, dynamicLibrary.");
		}
	};

	typedef enum class sfProjectOptimization
	{
		debug = 0x00,
		optimization = 0x01,
		optimizationDebug = 0x02,
	} sfProjectOptimization;
	struct sfProjectOptimizationHelper
	{
		inline static std::wstring to_string(const sfProjectOptimization& projectOptimization)
		{
			static const std::unordered_map<sfProjectOptimization, std::wstring> projectOptimizationMap
			{
				{sfProjectOptimization::debug, L"debug"},
				{sfProjectOptimization::optimization, L"optimization"},
				{sfProjectOptimization::optimizationDebug, L"optimizationDebug"}
			};

			auto it = projectOptimizationMap.find(projectOptimization);
			if (it != projectOptimizationMap.end()) {
				return it->second;
			}

			return L"debug";
		}

		inline static sfProjectOptimization to_enum(const std::wstring& projectOptimizationStr)
		{
			static const std::unordered_map<std::wstring, sfProjectOptimization> projectOptimizationMap
			{
				{L"debug", sfProjectOptimization::debug},
				{L"optimization", sfProjectOptimization::optimization},
				{L"optimizationDebug", sfProjectOptimization::optimizationDebug}
			};

			auto it = projectOptimizationMap.find(projectOptimizationStr);
			if (it != projectOptimizationMap.end()) {
				return it->second;
			}

			throw wruntime_error(L"Invalid project optimization. Expected: debug, optimization, optimizationDebug.");
		}
	};

	/*------------------------------------------------------------------*/

	typedef struct sfProjectSettings
	{
		std::wstring projectName{};
		std::wstring projectVersion{};
		sfProjectType projectType{};
	} sfProjectSettings;

	typedef struct sfCompilation
	{
		sfCppVersions cppVersion{ sfCppVersions::cpp20 };
		sfCppCompilers cppCompiler{ sfCppCompilers::clang };
		sfProjectOptimization cppOptimization{ sfProjectOptimization::debug };
		std::filesystem::path defaultClangCompilerPath { "\\..\\LLVM\\bin\\clang++.exe" };
		std::filesystem::path defaultClangLinkerPath { "\\..\\LLVM\\bin\\lld-link.exe" };
		std::filesystem::path cppCompilerPath{""};
		std::filesystem::path cppLinkerPath{""};
	} sfCompilationList;

	typedef struct sfConfigList
	{
		bool autoSourceSearch{ true };
		std::filesystem::path buildFolder{ L"\\Build" };
		std::filesystem::path outputReleaseFolder{ L"\\Build\\Output\\Release" };
		std::filesystem::path outputDebugFolder{ L"\\Build\\Output\\Debug" };
		std::filesystem::path cacheFilePath{ buildFolder.wstring() + L"\\data.info"};
	} sfConfigList;

	typedef struct sfCompilerArgs
	{
		std::vector<std::filesystem::path> cppFiles{};
		std::vector<std::wstring> cppFlags{};
		std::vector<std::wstring> cppIncludeDirs{};
		std::vector<std::wstring> cppLibDirs{};
		std::vector<std::wstring> cppLibs{};
	} sfCompilerArgs;

	typedef struct sfConfigSetup 
	{
		std::filesystem::path shafaRootPath{};
		std::filesystem::path configFilePath{ "\\config.toml" };
		sfProjectSettings projectSettings{};
		sfCompilationList compilationList{};
		sfConfigList configList{};
		sfCompilerArgs compilerArgs{};
	} sfConfigSetup;

	/*------------------------------------------------------------------*/

	inline std::wstring string_to_wstring(const std::string& stringArg)
	{
		std::wstring wstringArg;
		wstringArg.assign(stringArg.begin(), stringArg.end());
		return wstringArg;
	}

	inline std::string wstring_to_string(const std::wstring& wstringArg)
	{
		std::string stringArg;
		stringArg.assign(wstringArg.begin(), wstringArg.end());
		return stringArg;
	}
}