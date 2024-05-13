#include "sfinit.h"

namespace shafa {
	sfinit::sfinit(std::shared_ptr<sfConfigSetup> configSetup)
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
			logger::log(L"Project initialized successfully", LogLevel::Info);
		}
		catch (const wexception& err)
		{
			throw err;
		}

	}
}
