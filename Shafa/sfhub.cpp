#include "sfhub.h"
namespace shafa {
	void sfhub::analyze_hub(const std::vector<std::wstring_view>& args)
	{
		for (std::size_t index = 1; index < args.size(); index++)
		{
			auto argEnum = sfArgEnumHelper::to_enum(args[index]);
			switch (argEnum)
			{
			case sfArgEnum::none:
				m_args.push_back({ argEnum, {} });
				break;
			case sfArgEnum::help:
				m_args.push_back({ argEnum, {} });
				break;
			case sfArgEnum::configure:
			{
				std::vector<std::wstring_view> configureArgs;
				std::size_t confIndex;
				for (confIndex = (index + 1); confIndex < args.size(); confIndex++)
				{
					auto confArgEnum = sfArgEnumHelper::to_enum(args[confIndex]);
					if (confArgEnum == sfArgEnum::none)
					{
						configureArgs.push_back(args[confIndex]);
					}
					else if (confArgEnum == sfArgEnum::help)
					{
						configureArgs.push_back(args[confIndex]);
						break;
					}
					else
					{
						--confIndex;
						break;
					}
				}
				index = confIndex;
				m_args.push_back({ argEnum, configureArgs });
				break;
			}
			case sfArgEnum::build:
			{ 
				std::vector<std::wstring_view> buildArgs;
				std::size_t buildIndex;
				for (buildIndex = (index + 1); buildIndex < args.size(); buildIndex++)
				{
					auto confArgEnum = sfArgEnumHelper::to_enum(args[buildIndex]);
					m_configSetup.compilationList.projectBuildType = sfProjectBuildTypeHelper::to_enum(args[buildIndex]);
					if (confArgEnum == sfArgEnum::none)
					{
						m_configSetup.compilationList.projectBuildType = sfProjectBuildTypeHelper::to_enum(args[buildIndex]);
						buildArgs.push_back(args[buildIndex]);
						break;
					}
					else if (confArgEnum == sfArgEnum::help)
					{
						buildArgs.push_back(args[buildIndex]);
						break;
					}
					else
					{
						--buildIndex;
						break;
					}
				}
				index = buildIndex;
				m_args.push_back({ argEnum, buildArgs });
				break;
			}
			case sfArgEnum::run:
				m_args.push_back({ argEnum, {} });
				break;
			default:
				break;
			}
		}
	}
	void sfhub::start_shafa()
	{
		if (m_args.size() > 0)
		{
			for (const auto& [arg, subArgs] : m_args)
			{
				switch (arg)
				{
				case sfArgEnum::help:
					sfarghelper::help_args();
					break;
				case sfArgEnum::configure:
					if (subArgs.size() > 0)
					{
						for (const auto& subArg : subArgs)
						{
							switch (sfArgEnumHelper::to_enum(subArg))
							{
							case sfArgEnum::help:
								sfarghelper::help_configure();
								goto exitConfigureLoop;
							default:
								configure_hub(m_sfContTable, m_args);
								configuration_construct();
								goto exitConfigureLoop;
							}
						}
					}
					else {
						configure_hub(m_sfContTable, m_args);
						configuration_construct();
					}

				exitConfigureLoop:
					break;
				case sfArgEnum::build:
					configure_hub(m_sfContTable, m_args);
					if (subArgs.size() > 0) {
						for (const auto& subArg : subArgs)
						{
							switch (sfArgEnumHelper::to_enum(subArg))
							{
							case sfArgEnum::help:
								logger::log(L"Not implemented yet.", LogLevel::Info);
								goto exitBuildLoop;
							default:
								{
								non_configured_build();
								}
							}
						}
					}
					else {
						non_configured_build();
					}
				exitBuildLoop:
					break;
				case sfArgEnum::run:
					logger::log(L"Not implemented yet.", LogLevel::Info);
					break;
				default:
					logger::log(L"Unknown argument", LogLevel::Error);
					break;
				}
			}
		}
		else {
			sfarghelper::help_args();
		}
	}

	void sfhub::non_configured_build()
	{
		std::vector<std::pair<std::filesystem::path, std::wstring>> cppFileIdentity;
		for (const auto& entry : std::filesystem::recursive_directory_iterator(std::filesystem::current_path()))
		{
			if (
				entry.path().extension() == ".cpp" ||
				entry.path().extension() == ".cxx" ||
				entry.path().extension() == ".h" ||
				entry.path().extension() == ".hpp"
				)
			{
				cppFileIdentity.push_back(std::make_pair(entry.path(), file_hashing(entry.path().wstring())));
			}
		}
		std::vector<std::wstring> hashes;
		for (const auto& pair : cppFileIdentity) {
			hashes.push_back(pair.second);
		}
		std::vector <std::wstring> oldCppFileIdentity = get_hashes(m_configSetup.configList.buildFolder.wstring() + L"\\data.info");
		if (
			!compareVectors<std::wstring>(oldCppFileIdentity, hashes) || 
			![&]() {
				switch (m_configSetup.compilationList.projectBuildType)
				{
				case sfProjectBuildType::debug:
					switch (m_configSetup.projectSettings.projectType)
					{
					case sfProjectType::application:
						return std::filesystem::exists(m_configSetup.configList.outputDebugFolder.wstring() + L"\\" + m_configSetup.projectSettings.projectName + L".exe");
					case sfProjectType::staticLibrary:
						return std::filesystem::exists(m_configSetup.configList.outputDebugFolder.wstring() + L"\\" + m_configSetup.projectSettings.projectName + L".lib");
					case sfProjectType::dynamicLibrary:
						return std::filesystem::exists(m_configSetup.configList.outputDebugFolder.wstring() + L"\\" + m_configSetup.projectSettings.projectName + L".dll");
					default:
						break;
					}
					break;
				case sfProjectBuildType::release:
					switch (m_configSetup.projectSettings.projectType)
					{
					case sfProjectType::application:
						return std::filesystem::exists(m_configSetup.configList.outputReleaseFolder.wstring() + L"\\" + m_configSetup.projectSettings.projectName + L".exe");
					case sfProjectType::staticLibrary:
						return std::filesystem::exists(m_configSetup.configList.outputReleaseFolder.wstring() + L"\\" + m_configSetup.projectSettings.projectName + L".lib");
					case sfProjectType::dynamicLibrary:
						return std::filesystem::exists(m_configSetup.configList.outputReleaseFolder.wstring() + L"\\" + m_configSetup.projectSettings.projectName + L".dll");
					default:
						break;
					}
					break;
				}

			}())
		{
			std::vector<std::filesystem::path> paths;
			for (const auto& pair : cppFileIdentity) {
				if (pair.first.extension() != ".h" || pair.first.extension() != ".hpp")
				{
					paths.push_back(pair.first);
				}
			}
			m_configSetup.compilerArgs.cppFiles = paths;
			build_hub();
		}
		else
		{
			logger::log(L"Files have not changed. Skipping build.", LogLevel::Info);
		}
	}
}
		