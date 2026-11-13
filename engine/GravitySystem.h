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

	    for (auto& entities : manager) {
	        if (manager.template HasComponent<GravityComponent>(entities.GetID()) && manager.template HasComponent<MovementComponent>(entities.GetID()) &&
	            manager.template HasComponent<CollisionComponent>(entities.GetID())) {
	            mEntityID = entities.GetID();
	            break;
	        }
	    }

	    auto& collision = manager.template GetComponent<CollisionComponent>(mEntityID);
	    auto& gravity = manager.template GetComponent<GravityComponent>(mEntityID);
	    auto& movement = manager.template GetComponent<MovementComponent>(mEntityID);

	    if (collision.onGround == 1) {
	        gravity.acceleration.y = 0;
	    } else if (collision.onLeftWall == 1 || collision.onRightWall == 1) {
	        gravity.acceleration.y = 3;
	    } else {
	        gravity.acceleration.y = 9.81;
	    }

	    movement.velocity.y += gravity.acceleration.y;


		return kContinue;
	}

private:
    EntityID mEntityID;
};


}



