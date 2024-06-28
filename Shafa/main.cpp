#include <cstdint>
#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <boost/program_options.hpp>

#include "sffile.h"

namespace po = boost::program_options;

void argument_analyzer(std::int32_t argc, char* argv[], shafa::sfConfigSetup* configSetup);

template<typename function>
void run_shafa(shafa::sffile* sfFile, shafa::sfConfigSetup* configSetup, function func);

std::int32_t main(std::int32_t argc, char* argv[]) {
	shafa::sfConfigSetup configSetup{};
	configSetup.shafaRootPath = std::filesystem::path(argv[0]).parent_path().parent_path();
	configSetup.configFilePath = std::filesystem::current_path().wstring() + configSetup.configFilePath.wstring();

	argument_analyzer(argc, argv, &configSetup);

	return 0;
}

void argument_analyzer(std::int32_t argc, char* argv[], shafa::sfConfigSetup* configSetup)
{
	po::options_description desc("Allowed options");
	desc.add_options()
		("help,h", "produce help message")
		("init,i", "initialize a new project")
		("configure,c", "configure the project")
		("build,b", po::value<std::string>(), "build the project")
		("pkg,p", po::value<std::vector<std::string>>()->multitoken(), "install the pkg")
		("run,r", "run the project");

	po::variables_map vm;
	try {
		po::store(po::parse_command_line(argc, argv, desc), vm);
		po::notify(vm);
	}
	catch (const po::error& ex) {
		std::cerr << ex.what() << std::endl;
		ExitProcess(1);
	}

	shafa::sffile sfFile{};
	if (vm.count("help")) {
		std::cout << desc << "\n";
	}
	else if (vm.count("init")) {
		std::cout << "Initializing a new project\n";
	}
	else if (vm.count("configure")) {
		run_shafa(&sfFile, configSetup, [&]() {
			sfFile.get_sf_hub()->configure_hub(sfFile.get_sf_hub()->m_sfContTable);
			sfFile.get_sf_hub()->configuration_construct();
			}
		);
	}
	else if (vm.count("build")) {
		run_shafa(&sfFile, configSetup, [&]() {
			sfFile.get_sf_hub()->configure_hub(sfFile.get_sf_hub()->m_sfContTable);
			configSetup->compilationList->projectBuildType = shafa::sfProjectBuildTypeHelper::to_enum(shafa::string_to_wstring(vm["build"].as<std::string>()));
			sfFile.get_sf_hub()->non_configured_build();
			sfFile.get_sf_hub()->configuration_construct();
			}
		);
	}
	else if (vm.count("pkg")) {
		run_shafa(&sfFile, configSetup, [&]() {
			sfFile.get_sf_hub()->configure_hub(sfFile.get_sf_hub()->m_sfContTable);
			std::vector<std::string> pkgs;
			if (!vm["pkg"].empty() && (pkgs = vm["pkg"].as<std::vector<std::string> >()).size() >= 1) {
				if (pkgs[0] == "make") {
					sfFile.get_sf_hub()->make_pkg(std::filesystem::current_path());
				}
				else if (pkgs[0] == "extract") {
					if (pkgs.size() == 3)
						sfFile.get_sf_hub()->extract_pkg(pkgs[0], pkgs[1]);
					else
						shafa::LOG_ERROR(L"Invalid pkg extract option");
				}
				else if (pkgs[0] == "install") {
					if (pkgs.size() == 2)
						sfFile.get_sf_hub()->install_pkg(pkgs[1]);
					else
						shafa::LOG_ERROR(L"Invalid pkg install option");
				}
				else {
					shafa::LOG_ERROR(L"Invalid pkg option");
				}
			}
			}
		);
	}
	else if (vm.count("run")) {
		shafa::LOG_ERROR(L"Running the project");
	}
	else {
		shafa::LOG_ERROR(L"No arguments provided");
	}
}

template<typename function>
void run_shafa(shafa::sffile* sfFile, shafa::sfConfigSetup* configSetup, function func)
{
	try
	{
		sfFile->check_validity(configSetup);
		func();
	}
	catch (const shafa::wexception& err)
	{
		shafa::LOG_ERROR(err.wwhat());
		ExitProcess(1);
	}
}