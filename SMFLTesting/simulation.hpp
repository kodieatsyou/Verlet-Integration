#pragma once
#include <string>
#include <Windows.h>

struct Settings
{
	std::string name;
	uint32_t max_object_count;
};

class Simulation
{
	public:
		Settings settings{};

		Simulation(Settings settings) {
			this->settings = settings;
		}

		std::string toString() {
			return "Simulation(" + settings.name + ")\n";
		}
		
	private:
};