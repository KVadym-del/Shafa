#include <cstdint>
#include <iostream>
#include <vector>
#include <string>
#include <memory>

#include "sffile.h"

std::int32_t wmain(std::int32_t argc, wchar_t** argv) {
	std::vector<std::wstring_view> args(argv, argv + argc);
	std::shared_ptr<shafa::sfConfigSetup> configSetup = std::make_shared<shafa::sfConfigSetup>();
	configSetup->shafaRootPath = std::filesystem::path(args[0]).parent_path().parent_path();
	configSetup->configFilePath = std::filesystem::current_path().wstring() + configSetup->configFilePath.wstring();
	try
	{
		shafa::sffile sfFile{ configSetup };
		sfFile.check_validity();
		sfFile.analyze_hub(args);
		sfFile.start_shafa();
	}
	catch (const shafa::wexception& err)
	{
		shafa::logger::log(err.wwhat(), shafa::LogLevel::Error);
		return 1;
	}
	return 0;
}