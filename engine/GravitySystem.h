#pragma once

namespace ECSEngine
{

template <typename... Components>
class GravitySystem : public System<Components...> {
public:
	const std::string & GetName() const override
	{ static std::string name("GravitySystem"); return name; }
	
	tSystemReturn Run(EngineView<Components...> *engine) override
	{
	    auto& manager = engine->GetEntityManager();
	    auto& window = engine->GetWindowManager();
	    std::vector<EntityID> mEntityID;

	    for (auto& entities : manager) {
	        if (manager.template HasComponent<GravityComponent>(entities.GetID()) && manager.template HasComponent<MovementComponent>(entities.GetID()) &&
	            manager.template HasComponent<CollisionComponent>(entities.GetID())) {
	            mEntityID.push_back(entities.GetID());

	        }
	    }

	    for (auto entities : mEntityID) {
	        auto& collision = manager.template GetComponent<CollisionComponent>(entities);
	        auto& gravity = manager.template GetComponent<GravityComponent>(entities);
	        auto& movement = manager.template GetComponent<MovementComponent>(entities);

	        if (gravity.disabledTimer != 0) {
	            gravity.disabledTimer -= 1;
	            movement.velocity.y = 0;
	            continue;
	        }
	        if (collision.onGround == 0 && (collision.onLeftWall == 1 || collision.onRightWall == 1)) {
                gravity.acceleration.y = 0.3;
	            movement.velocity.y = std::min(movement.velocity.y + gravity.acceleration.y, 3.f);
	        } else {
	            gravity.acceleration.y = 2;
	            movement.velocity.y += gravity.acceleration.y;
	        }

	    }

		return kContinue;
	}

private:

};


}



