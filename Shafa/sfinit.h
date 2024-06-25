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
		sfinit(sfConfigSetup* configSetup);
		~sfinit() = default;

		void init_project();

	private:
		sfConfigSetup* m_configSetup;
	};
}

