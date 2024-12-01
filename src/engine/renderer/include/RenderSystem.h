#pragma once

#include "../../ecs/include/System.h"
#include "MeshRenderer.h"
#include <vector>

class RenderSystem : public System
{
public:
	void Update(float deltaTime) override
	{
		for (auto entity : entities)
		{
			auto meshRenderer = entity.GetComponent<MeshRenderer>();
			if (meshRenderer)
			{
				meshRenderer->Render();
			}
		}
	}
};