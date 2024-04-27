module;

#include <chrono>
#include <vector>

#include "structs.h"

export module Spawner;

import Attire;
import GameObject;
import Random;

export class Spawner
{
	std::chrono::steady_clock::time_point m_StartTime;
	int m_Counter = 0;

public:
	inline static int SPAWN_TIME_SECONDS = 3;
	inline static int HAT_COUNT = 3;
	inline static int CANE_COUNT = 3;

	Spawner()
	{
		m_StartTime = std::chrono::steady_clock::now();
	}

	void Spawn(std::vector<Attire>& attirePool)
	{
		std::chrono::steady_clock::time_point t = std::chrono::steady_clock::now();
		if (std::chrono::duration<float>(t - m_StartTime).count() > m_Counter * SPAWN_TIME_SECONDS
			&& m_Counter < 100)
		{
			if (0 <= m_Counter && m_Counter < HAT_COUNT)
			{
				const Point2f pos = { Generate.Float(100, 1100), Generate.Float(100, 700) };
				attirePool.push_back(Attire(generate_uuid_v4(), pos, Attire::HAT_T));
			}
			if (CANE_COUNT <= m_Counter && m_Counter < HAT_COUNT + CANE_COUNT)
			{
				const Point2f pos = { Generate.Float(100, 1100), Generate.Float(100, 700) };
				attirePool.push_back(Attire(generate_uuid_v4(), pos, Attire::CANE_T));
			}
			m_Counter += 1;
		}
	}
};
