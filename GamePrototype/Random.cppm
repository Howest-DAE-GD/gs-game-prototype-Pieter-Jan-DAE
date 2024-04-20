// ReSharper disable CppClangTidyCertMsc51Cpp
module;

#include <random>
#include "structs.h"

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
