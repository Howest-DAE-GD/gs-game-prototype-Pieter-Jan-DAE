// ReSharper disable CppClangTidyCertMsc51Cpp
module;

#include <random>
#include "structs.h"
#include <sstream>

export module Random;

export class Random
{
public:
	std::mt19937 m_Generator;

	Random()
	{
		m_Generator = std::mt19937(1234567890);
	}

	float Float(const float min, const float max)
	{
		std::uniform_real_distribution<float> distribution(min, max);
		return distribution(m_Generator);
	}

	Color4f Color()
	{
		const float r = Float(0.f, 256.f);
		const float g = Float(0.f, 256.f);
		const float b = Float(0.f, 256.f);
		constexpr float a = 256.f;
		return { r, g, b, a };
	}
};

export Random Generate{};

static std::random_device              rd;
static std::mt19937                    gen(rd());
static std::uniform_int_distribution<> dis(0, 15);
static std::uniform_int_distribution<> dis2(8, 11);

export std::string Generate_UUID() {
	std::stringstream ss;
	int i;
	ss << std::hex;
	for (i = 0; i < 8; i++) {
		ss << dis(gen);
	}
	ss << "-";
	for (i = 0; i < 4; i++) {
		ss << dis(gen);
	}
	ss << "-4";
	for (i = 0; i < 3; i++) {
		ss << dis(gen);
	}
	ss << "-";
	ss << dis2(gen);
	for (i = 0; i < 3; i++) {
		ss << dis(gen);
	}
	ss << "-";
	for (i = 0; i < 12; i++) {
		ss << dis(gen);
	}
	return ss.str();
}
