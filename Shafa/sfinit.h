#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>

#include "logger.h"
#include "sftypes.h"
#include "wexception.h"

namespace shafa {
	class sfinit
	{
	public:
		sfinit() = default;
		sfinit(std::shared_ptr<sfConfigSetup> configSetup);
		~sfinit() = default;

		void init_project();

	private:
		std::shared_ptr<sfConfigSetup> m_configSetup;
	};
}

