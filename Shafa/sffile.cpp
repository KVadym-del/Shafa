#include "sffile.h"

namespace shafa {

	sffile::sffile(sfConfigSetup& configSetup)
		: m_configSetup(configSetup), m_sfPath(configSetup.configFilePath), sfhub(configSetup)
	{
		if (!std::filesystem::exists(m_sfPath))
			throw wruntime_error(L"File does not exist. Expected: config.toml");
		if (m_sfPath.filename().string() != "config.toml")
			throw wruntime_error(L"Invalid file name. Expected: config.toml");
	}

	void sffile::check_validity(void)
	{
		try
		{
			m_sfContTable = toml::parse_file(m_sfPath.c_str());
		}
		catch (const toml::parse_error& err)
		{
			std::wstring werr(err.what(), err.what() + std::strlen((err.what())));
			logger::log(werr, LogLevel::Error);
		}
	}
}
