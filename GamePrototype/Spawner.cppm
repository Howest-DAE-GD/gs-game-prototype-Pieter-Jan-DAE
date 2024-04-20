module;

#include <chrono>
#include <map>
#include <vector>

export module Spawner;

import Attire;
import GameObject;

export class Spawner
{
public:
	std::chrono::steady_clock::time_point m_StartTime;
	int m_Counter = 1;

	Spawner()
	{
		m_StartTime = std::chrono::steady_clock::now();
	}

	void Spawn(std::vector<Attire>& attirePool)
	{
		std::chrono::steady_clock::time_point t = std::chrono::steady_clock::now();
		if (std::chrono::duration<float>(t - m_StartTime).count() > m_Counter * 5
			&& m_Counter < 100)
		{
			if (0 <= m_Counter && m_Counter < 10)
			{
				attirePool.push_back(Attire::Hat());
			}
			if (10 <= m_Counter && m_Counter < 200)
			{
				attirePool.push_back(Attire::Cane());
			}
			m_Counter += 1;
		}
	}
};
