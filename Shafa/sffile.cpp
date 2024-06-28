#include "sffile.h"

namespace shafa {
	
	void sffile::check_validity(sfConfigSetup* configSetup)
	{
		m_configSetup = configSetup;
		m_sfPath = configSetup->configFilePath;

		m_sfHub = std::make_unique<sfhub>(m_configSetup);

		if (!std::filesystem::exists(m_sfPath))
			throw wruntime_error(L"File does not exist. Expected: config.toml");
		if (m_sfPath.filename().string() != "config.toml")
			throw wruntime_error(L"Invalid file name. Expected: config.toml");

		try
		{
			m_sfHub->m_sfContTable = toml::parse_file(m_sfPath.c_str());
		}
		catch (const toml::parse_error& err)
		{
			std::wstring werr(err.what(), err.what() + std::strlen((err.what())));
			LOG_ERROR(werr);
			ExitProcess(1);
		}
	}
}
