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
			case sfArgEnum::init:
			{
				std::wstring_view initArg{};

				if (args.size() >= (index + 1))
				{
					auto confArgEnum = sfArgEnumHelper::to_enum(args[index + 1]);
					if (confArgEnum == sfArgEnum::none)
					{
						initArg = args[index + 1];
						break;
					}
					else if (confArgEnum == sfArgEnum::help)
					{
						initArg = args[index + 1];
						break;
					}
				}

				m_args.push_back({ argEnum, { initArg } });
				break;
			}
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
					m_configSetup->compilationList->projectBuildType = sfProjectBuildTypeHelper::to_enum(args[buildIndex]);
					if (confArgEnum == sfArgEnum::none)
					{
						m_configSetup->compilationList->projectBuildType = sfProjectBuildTypeHelper::to_enum(args[buildIndex]);
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
				m_args.push_back({ argEnum, buildArgs });
				break;
			}
			case sfArgEnum::pkg:
			{
				std::vector<std::wstring_view> buildArgs;
				std::size_t buildIndex;
				for (buildIndex = (index + 1); buildIndex < args.size(); buildIndex++)
				{
					auto confArgEnum = sfArgEnumHelper::to_enum(args[buildIndex]);
					sfPkgEvent pkgEvent = sfPkgEventHelper::to_enum(args[buildIndex]);
					if (confArgEnum == sfArgEnum::none)
					{
						switch (pkgEvent)
						{
						case shafa::sfPkgEvent::make:
							buildArgs.push_back(args[buildIndex]);
							break;
						case shafa::sfPkgEvent::extract:
							buildArgs.push_back(args[buildIndex]);
							buildArgs.push_back(args[buildIndex + 1]);
							buildArgs.push_back(args[buildIndex + 2]);
							break;
						case shafa::sfPkgEvent::read:
							buildArgs.push_back(args[buildIndex]);
							break;
						case shafa::sfPkgEvent::install:
							buildArgs.push_back(args[buildIndex]);
							if (args.size() >= (buildIndex + 1))
							{
								buildArgs.push_back(args[buildIndex + 1]);
							} else
							{
								LOG_ERROR(L"Please provide a path to the package");
								break;
							}
							break;
						}
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
				case sfArgEnum::init:
					if (subArgs[0].empty())
					{
						LOG_ERROR(L"Unsupported yet!");
						break;
						configure_hub(m_sfContTable, m_args);
						logger::log_new_line();
						configuration_construct();
						try
						{
							init_project();
						}
						catch (const std::exception& err)
						{
							throw err;
						}
					}
					else
					{
						/*not implemented yet*/
						LOG_INFO(L"Not implemented yet.");
					}
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
								logger::log_new_line();
								configuration_construct();
								goto exitConfigureLoop;
							}
						}
					}
					else {
						configure_hub(m_sfContTable, m_args);
						logger::log_new_line();
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
								LOG_INFO(L"Not implemented yet.");
								goto exitBuildLoop;
							default:
								{
								non_configured_build();
								configuration_construct();
								}
							}
						}
					}
					else {
						non_configured_build();
						configuration_construct();
					}
				exitBuildLoop:
					break;
				case sfArgEnum::run:
					LOG_INFO(L"Not implemented yet.");
					break;
				case sfArgEnum::pkg:
					configure_hub(m_sfContTable, m_args);
					if (subArgs.size() > 0) {
						for (const auto& subArg : subArgs)
						{
							switch (sfPkgEventHelper::to_enum(subArg))
							{
							case sfPkgEvent::none:
								LOG_ERROR(L"Unknown argument");
								break;
							case sfPkgEvent::make:
								try { sfpkg::make_pkg(std::filesystem::current_path()); }
								catch (const wexception& err) { throw err; }
								goto exitPkgLoop;
							case sfPkgEvent::extract:
								if (subArgs.size() >= 3) {
									if (subArgs[1].empty())
									{
										LOG_ERROR(L"Please provide a path to the package");
										goto exitPkgLoop;
									}
									else if (subArgs[2].empty())
									{
										LOG_ERROR(L"Please provide a path to extract the package");
										goto exitPkgLoop;
									}
									else {
										try
										{
											sfpkg::extract_pkg(subArgs[1], subArgs[2]);
											goto exitPkgLoop;
										}
										catch (const wexception& err)
										{
											throw err;
										}
									}
								}
								else {
									throw wexception(L"Not enough arguments, use help to find more information");
								}
								goto exitPkgLoop;
							case sfPkgEvent::read:
								LOG_INFO(L"Not implemented yet.");
								goto exitPkgLoop;

							case sfPkgEvent::install:
								if (subArgs.size() >= 2) {
									if (subArgs[1].empty())
									{
										LOG_ERROR(L"Please provide a path to the package");
										goto exitPkgLoop;
									}
									else {
										try
										{
											sfpkg::install_pkg(subArgs[1]);
											goto exitPkgLoop;
										}
										catch (const wexception& err)
										{
											throw err;
										}
									}
								}
								else {
									throw wexception(L"Not enough arguments, use help to find more information");
								}
								goto exitPkgLoop;
							}
						}
					}
					else {
						LOG_ERROR(L"Not enough arguments, use help to find more information");
					}
				exitPkgLoop:
					break;
					/*
				default:
					LOG_ERROR(L"Unknown argument");
					break;
					*/
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
		for (auto entry = std::filesystem::recursive_directory_iterator(std::filesystem::current_path());
			entry != std::filesystem::recursive_directory_iterator(); ++entry)
		{
			if (entry->path() == m_configSetup->configList->pkgFolder) {
				entry.disable_recursion_pending();
				continue;
			}

			if (
				entry->path().extension() == ".cpp" ||
				entry->path().extension() == ".cxx" ||
				entry->path().extension() == ".cc" ||
				entry->path().extension() == ".h" ||
				entry->path().extension() == ".hpp"
				)
			{
				cppFileIdentity.push_back(std::make_pair(entry->path(), file_hashing(entry->path().wstring())));
			}
		}
		std::vector<std::wstring> hashes;
		for (const auto& pair : cppFileIdentity) {
			hashes.push_back(pair.second);
		}
		std::vector <std::wstring> oldCppFileIdentity = get_hashes(m_configSetup->configList->buildFolder.wstring() + L"\\data.info");
        if (!sftools::compare_vectors<std::wstring>(oldCppFileIdentity, hashes) ||
            ![&]() {
                switch (m_configSetup->compilationList->projectBuildType) {
                    case sfProjectBuildType::debug:
                        switch (m_configSetup->projectSettings->projectType) {
                            case sfProjectType::application:
                                return std::filesystem::exists(m_configSetup->configList->outputDebugFolder.wstring() + L"\\" + m_configSetup->projectSettings->projectName + L".exe");
                            case sfProjectType::staticLibrary:
                                return std::filesystem::exists(m_configSetup->configList->outputDebugFolder.wstring() + L"\\" + m_configSetup->projectSettings->projectName + L".lib");
                            case sfProjectType::dynamicLibrary:
                                return std::filesystem::exists(m_configSetup->configList->outputDebugFolder.wstring() + L"\\" + m_configSetup->projectSettings->projectName + L".dll");
                        }
                        break;
                    case sfProjectBuildType::release:
                        switch (m_configSetup->projectSettings->projectType) {
                            case sfProjectType::application:
                                return std::filesystem::exists(m_configSetup->configList->outputReleaseFolder.wstring() + L"\\" + m_configSetup->projectSettings->projectName + L".exe");
                            case sfProjectType::staticLibrary:
                                return std::filesystem::exists(m_configSetup->configList->outputReleaseFolder.wstring() + L"\\" + m_configSetup->projectSettings->projectName + L".lib");
                            case sfProjectType::dynamicLibrary:
                                return std::filesystem::exists(m_configSetup->configList->outputReleaseFolder.wstring() + L"\\" + m_configSetup->projectSettings->projectName + L".dll");
                        }
                        break;
                }
                return false;
            }())
        {
			std::vector<std::filesystem::path> paths;
			for (const auto& pair : cppFileIdentity) {
				if (pair.first.extension() != ".h" || pair.first.extension() != ".hpp")
				{
					paths.push_back(pair.first);
				}
			}
			m_configSetup->compilerArgs->cppFiles = paths;
			try
			{
				build_hub();
			}
			catch (const wexception& err)
			{
				throw err;
			}

		}
		else
		{
			LOG_INFO(L"Files have not changed. Skipping build.");
		}
	}
}
		