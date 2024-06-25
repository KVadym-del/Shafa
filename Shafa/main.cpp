#include <cstdint>
#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <argparse/argparse.hpp>

#include "sffile.h"

std::int32_t wmain(std::int32_t argc, wchar_t** argv) {
	argparse::ArgumentParser program("Shafa", "v0.3.1-alpha");

	shafa::sfConfigSetup configSetup{};
	configSetup.shafaRootPath = std::filesystem::path(argv[0]).parent_path().parent_path();
	configSetup.configFilePath = std::filesystem::current_path().wstring() + configSetup.configFilePath.wstring();
	try
	{
		shafa::sffile sfFile{ &configSetup };
		sfFile.check_validity();
		sfFile.analyze_hub(args);
		sfFile.start_shafa();
	}
	catch (const shafa::wexception& err)
	{
		shafa::LOG_ERROR(err.wwhat());
		return 1;
	}
	return 0;
}