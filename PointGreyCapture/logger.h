#pragma once

#include <string>

namespace logging {
	bool init();
	void init_cerr();
	void init_cout();
	void error(const std::string& message);
};