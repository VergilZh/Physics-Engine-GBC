#include "AeroSurfaceSystem.h"
#include "RigidbodyComponent.h"
#include "ForceAndTorqueAccumulatorComponent.h"

namespace Reality
{
	AeroSurfaceSystem::AeroSurfaceSystem()
	{
		requireComponent<TransformComponentV2>();
		requireComponent<AeroSurfaceComponent>();
	}

	void AeroSurfaceSystem::Update(float deltaTime)
	{
		for (auto e : getEntities())
		{
			auto& surfaceTransform = e.getComponent<TransformComponentV2>();
			auto& aero = e.getComponent<AeroSurfaceComponent>();

			if (aero.attachedEntity.hasComponent<TransformComponentV2>() &&
				aero.attachedEntity.hasComponent<RigidbodyComponent>() &&
				aero.attachedEntity.hasComponent<ForceAndTorqueAccumulatorComponent>())
			{
				auto& targetTransform = aero.attachedEntity.getComponent<TransformComponentV2>();
				auto& rigidbody = aero.attachedEntity.getComponent<RigidbodyComponent>();
				auto& forceAndTorque = aero.attachedEntity.getComponent<ForceAndTorqueAccumulatorComponent>();

				// HACK: For lack of scene graph
				Vector3 worldSurfacePosition = targetTransform.LocalToWorldPosition(aero.localOffset);
				surfaceTransform.SetPosition(worldSurfacePosition);
				surfaceTransform.SetOrientation(targetTransform.GetOrientation());
				getWorld().data.renderUtil->DrawCube(surfaceTransform.GetPosition(),
					Vector3(1, 1, 1),
					surfaceTransform.GetOrientation());

				Vector3 force = CalculateWorldAerodynamicForce(aero.aerodynamicForce, targetTransform);
				forceAndTorque.AddForceAtPoint(force,
					surfaceTransform.GetPosition(),
					targetTransform.GetPosition());
			}
		}
	}

	const Vector3 & AeroSurfaceSystem::CalculateWorldAerodynamicForce(const Vector3& localAeroForce, TransformComponentV2& transform)
	{
		return transform.LocalToWorldDirection(localAeroForce);
	}
}
