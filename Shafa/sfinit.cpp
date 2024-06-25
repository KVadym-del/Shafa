#include "sfinit.h"

namespace shafa {
	sfinit::sfinit(sfConfigSetup* configSetup)
		: m_configSetup(configSetup)
	{}

	void sfinit::init_project()
	{
		try
		{
			std::filesystem::copy(
				m_configSetup->shafaRootPath.wstring() + L"\\..\\resources\\init",
				m_configSetup->configFilePath.parent_path(),
				std::filesystem::copy_options::overwrite_existing
			);
			LOG_INFO(L"Project initialized successfully");
		}
		catch (const wexception& err)
		{
			throw err;
		}

	}
}
