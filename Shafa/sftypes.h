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
		init = 0x05,
		pkg = 0x06
	} sfArgEnum;
	struct sfArgEnumHelper
	{
		inline static std::wstring_view to_string(const sfArgEnum& argEnum)
		{
			static const std::unordered_map<sfArgEnum, std::wstring> argEnumMap
			{
				{sfArgEnum::none, L"none"},
				{sfArgEnum::help, L"help"},
				{sfArgEnum::configure, L"configure"},
				{sfArgEnum::build, L"build"},
				{sfArgEnum::run, L"run"},
				{sfArgEnum::init, L"init"},
				{sfArgEnum::pkg, L"pkg"}
			};

			auto it = argEnumMap.find(argEnum);
			if (it != argEnumMap.end()) {
				return it->second;
			}

			return L"none";
		}


		inline static sfArgEnum to_enum(const std::wstring_view& argEnumStr)
		{
			static const std::unordered_map<std::wstring_view, sfArgEnum> argEnumMap
			{
				{L"none", sfArgEnum::none},
				{L"help", sfArgEnum::help},
				{L"configure", sfArgEnum::configure},
				{L"build", sfArgEnum::build},
				{L"run", sfArgEnum::run},
				{L"init", sfArgEnum::init},
				{L"pkg", sfArgEnum::pkg}
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
		std::vector<std::wstring_view> argEdit;
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
	
	typedef enum class sfProjectBuildType
	{
		debug = 0x00,
		release = 0x01,
	} sfProjectBuildType;
	struct sfProjectBuildTypeHelper
	{
		inline static std::wstring_view to_string(const sfProjectBuildType& projectOptimization)
		{
			static const std::unordered_map<sfProjectBuildType, std::wstring> projectOptimizationMap
			{
				{sfProjectBuildType::debug, L"debug"},
				{sfProjectBuildType::release, L"release"}
			};

			auto it = projectOptimizationMap.find(projectOptimization);
			if (it != projectOptimizationMap.end()) {
				return it->second;
			}

			return L"debug";
		}

		inline static sfProjectBuildType to_enum(const std::wstring_view& projectOptimizationStr)
		{
			static const std::unordered_map<std::wstring_view, sfProjectBuildType> projectOptimizationMap
			{
				{L"debug", sfProjectBuildType::debug},
				{L"release", sfProjectBuildType::release}

			};

			auto it = projectOptimizationMap.find(projectOptimizationStr);
			if (it != projectOptimizationMap.end()) {
				return it->second;
			}

			throw wruntime_error(L"Invalid project optimization. Expected: debug, release.");
		}
	};

	typedef enum class sfPkgEvent
	{
		none = 0x00,
		make = 0x01,
		extract = 0x02,
		read = 0x03,
		install = 0x04,
	} sfPkgEvent;
	struct sfPkgEventHelper
	{
		inline static std::wstring_view to_string(const sfPkgEvent& pkgEvent)
		{
			static const std::unordered_map<sfPkgEvent, std::wstring> pkgEventMap
			{
				{sfPkgEvent::none, L"none"},
				{sfPkgEvent::make, L"make"},
				{sfPkgEvent::extract, L"extract"},
				{sfPkgEvent::read, L"read"},
				{sfPkgEvent::install, L"install"}
			};

			auto it = pkgEventMap.find(pkgEvent);
			if (it != pkgEventMap.end()) {
				return it->second;
			}

			return L"none";
		}

		inline static sfPkgEvent to_enum(const std::wstring_view& pkgEventStr)
		{
			static const std::unordered_map<std::wstring_view, sfPkgEvent> pkgEventMap
			{
				{L"none", sfPkgEvent::none},
				{L"make", sfPkgEvent::make},
				{L"extract", sfPkgEvent::extract},
				{L"read", sfPkgEvent::read},
				{L"install", sfPkgEvent::install}
			};

			auto it = pkgEventMap.find(pkgEventStr);
			if (it != pkgEventMap.end()) {
				return it->second;
			}

			return sfPkgEvent::none;
		}
	};

	/*------------------------------------------------------------------*/

	typedef struct sfProjectSettings
	{
		std::wstring projectName{};
		std::wstring projectVersion{};
		sfProjectType projectType{};
	} sfProjectSettings;

	typedef struct sfCompilationList
	{
		sfCppVersions cppVersion{ sfCppVersions::cpp20 };
		sfCppCompilers cppCompiler{ sfCppCompilers::clang };
		sfProjectBuildType projectBuildType{ sfProjectBuildType::debug };
		std::wstring debugFlags{ L" -g -O0 -fno-omit-frame-pointer -Wall -Wextra -Wpedantic" };
		std::wstring releaseFlags{ L" -O3 -flto -DNDEBUG" };
		std::filesystem::path defaultClangCompilerPath { "\\..\\Shafa\\LLVM\\bin\\clang++.exe" };
		std::filesystem::path defaultClangLinkerPath { "\\..\\Shafa\\LLVM\\bin\\lld-link.exe" };
		std::filesystem::path cppCompilerPath{""};
		std::filesystem::path cppLinkerPath{""};
	} sfCompilationList;

	typedef struct sfConfigList
	{
		bool autoSourceSearch{ true };
		bool multiThreadedBuild{ true };
		std::filesystem::path buildFolder{ L"\\Build" };
		std::filesystem::path pkgFolder{ buildFolder.wstring() + L"\\Packages" };
		std::filesystem::path outputFolder{ buildFolder.wstring() + L"\\Output"};
		std::filesystem::path outputReleaseFolder{ outputFolder.wstring() + L"\\Release"};
		std::filesystem::path outputDebugFolder{ outputFolder.wstring() + L"\\Debug" };
		std::filesystem::path cacheFilePath{ buildFolder.wstring() + L"\\data.info"};
	} sfConfigList;

	typedef struct sfCompilerArgs
	{
		std::vector<std::filesystem::path> cppFiles{};
		std::vector<std::wstring> cppFlags{};
		std::vector<std::wstring> cppIncludeDirs{};
		std::vector<std::wstring> cppLibDirs{};
		std::vector<std::wstring> cppLibs{};
		std::vector<std::wstring> packages{};
	} sfCompilerArgs;

	typedef struct sfConfigSetup 
	{
		std::filesystem::path shafaRootPath{};
		std::filesystem::path configFilePath{ "\\config.toml" };
		std::unique_ptr<sfProjectSettings> projectSettings = std::make_unique<sfProjectSettings>();
		std::unique_ptr<sfCompilationList> compilationList = std::make_unique<sfCompilationList>();
		std::unique_ptr<sfConfigList> configList = std::make_unique<sfConfigList>();
		std::unique_ptr<sfCompilerArgs> compilerArgs = std::make_unique<sfCompilerArgs>();
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