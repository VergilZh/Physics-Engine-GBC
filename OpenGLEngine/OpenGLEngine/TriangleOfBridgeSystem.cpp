﻿#include "TriangleOfBridgeSystem.h"
#include "TransformComponent.h"
#include "ParticleContactEvent.h"

namespace Reality
{
	TriangleOfBridgeSystem::TriangleOfBridgeSystem()
	{
		requireComponent<TriangleOfBridgeComponent>();
	}

	void TriangleOfBridgeSystem::Update(float deltaTime)
	{
		if (Sphere != -99)
		{
			for (auto e : getEntities())
			{
				auto& tri = e.getComponent<TriangleOfBridgeComponent>();

				Vector3 p0 = tri.E0.getComponent<TransformComponent>().position;
				Vector3 p1 = tri.E1.getComponent<TransformComponent>().position;
				Vector3 p2 = tri.E2.getComponent<TransformComponent>().position;
				Vector3 center = (p0 + p1 + p2) / 3.f;

				Vector3 v1 = p1 - p0;
				Vector3 v2 = p2 - p0;
				Vector3 planeNorm = glm::normalize(glm::cross(v1, v2));
				float planeD = -(planeNorm.x * center.x + planeNorm.y * center.y + planeNorm.z * center.z);

				//Draw plane's normal
				planeNorm = glm::normalize(planeNorm);
				getWorld().data.renderUtil->DrawLine(center, center + (planeNorm), Color(0, 0, 1, 1));

				Vector4 plane = Vector4(planeNorm, planeD);
				Vector4 spherePos = Vector4(Sphere.getComponent<TransformComponent>().position, 1);
				float distance = glm::dot(plane, spherePos);

				Vector3 dir = Vector3(spherePos) - center;
				dir = glm::normalize(dir);
			
				Vector3 u = p1 - p0;
	
				Vector3 v = p2 - p0;

				Vector3 n = glm::cross(u, v);

				Vector3 w = Sphere.getComponent<TransformComponent>().position - p0;

				float gamma = glm::dot(glm::cross(u, w), n) / glm::dot(n, n);
	
				float beta = glm::dot(glm::cross(w, v), n) / glm::dot(n, n);
				float alpha = 1 - gamma - beta;

				if( ((0 <= alpha) && (alpha <= 1) &&
					(0 <= beta) && (beta <= 1) &&
					(0 <= gamma) && (gamma <= 1)) 
				  )
				{

					if (abs(distance) < 1)
					{
						float penetration = 1 - distance;

						getWorld().getEventManager().emitEvent<ParticleContactEvent>(
							tri.E0,
							Sphere,
							0.8f,
							-planeNorm,
							penetration);

						getWorld().getEventManager().emitEvent<ParticleContactEvent>(
							tri.E1,
							Sphere,
							0.8f,
							-planeNorm,
							penetration);

						getWorld().getEventManager().emitEvent<ParticleContactEvent>(
							tri.E2,
							Sphere,
							0.8f,
							-planeNorm,
							penetration);
					}

					getWorld().data.renderUtil->DrawLine(center, center + (planeNorm * distance), Color(0, 1, 0, 1));

				}
				else
				{
					getWorld().data.renderUtil->DrawLine(center, center + (planeNorm * distance), Color(1, 0, 0, 1));
				}
			}
		}
	}
}