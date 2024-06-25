#include "sfhub.h"
namespace shafa {
	void sfhub::analyze_hub(argparse::ArgumentParser* program)
	{
		m_program = program;
		m_program->add_argument("-h", "--help", "help")
			.help("Show this help message and exit")
			.default_value(false)
			.implicit_value(true);
		m_program->add_argument("-i", "--init", "init")
			.help("Initialize a new project(in-dev)")
			.default_value(false)
			.implicit_value(true);
		m_program->add_argument("-c", "--configure", "configure")
			.help("Configure the project")
			.action([&](const std::string& value) {
					configure_hub(m_sfContTable);
					configuration_construct();
				})
			.default_value(false)
			.implicit_value(true);
		m_program->add_argument("-b", "--build", "build")
			.help("Build the project")
			.action([&](const std::string& value) {
					non_configured_build();
					configuration_construct();
				})
			.default_value(std::string("debug"))
			.implicit_value(true);

		argparse::ArgumentParser pkgParser("install");
		pkgParser.add_argument("pkg")
			.help("Package the project")
			.nargs(1)
			.choices("make", "extract", "read", "install")
			.action([&](const std::string& value) {
			m_program->add_subparser(pkgParser);
				})
			.default_value(false)
			.implicit_value(true);
		m_program->add_argument("-p", "--pkg", "pkg")
			.help("Package the project")
			.nargs(1)
			.choices("make", "extract", "read", "install")
			.action([&](const std::string& value) {
			m_program->add_subparser(pkgParser);
				})
			.default_value(false)
			.implicit_value(true);
		m_program->add_argument("-r", "--run", "run")
			.help("Run the project(in-dev)")
			.default_value(false)
			.implicit_value(true);
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
								sfarghelper::help_build();
								goto fullExit;
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
							switch (sfArgEnumHelper::to_enum(subArg))
							{
							case sfArgEnum::help:
							{
								sfarghelper::help_pkg();
								goto fullExit;
							}
							default:
							{
								switch (sfPkgEventHelper::to_enum(subArg))
								{
								case sfPkgEvent::none:
									LOG_ERROR(L"Unknown argument");
									goto exitPkgLoop;
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
	fullExit: {}
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
		