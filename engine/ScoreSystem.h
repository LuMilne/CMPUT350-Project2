#pragma once

namespace ECSEngine {

template <typename... Components>
class ScoreSystem : public System<Components...> {
public:
    const std::string& GetName() const override {
        static std::string name("ScoreSystem");
        return name;
    }

    tSystemReturn Run(EngineView<Components...>* engine) override {
        auto& manager = engine->GetEntityManager();

        for (auto& entities : manager) {

            if (manager.template HasComponent<ScoreComponent>(entities.GetID())) {

                mScoreID = entities.GetID();

                break;
            }
        }
        auto& scoreComponent = manager.template GetComponent<ScoreComponent>(mScoreID);
        hundreds = scoreComponent.score / 100;
        tens = (scoreComponent.score / 10) % 10;
        ones = scoreComponent.score % 10;
        int scoreDigits[3] = {hundreds, tens, ones};

        //Assigns the texture to the right spot in score to update visuals
        for (int i = 0; i < 3; i++) {
            auto& sprite = manager.template GetComponent<SpriteComponent>(scoreComponent.digitID[i]);
            sprite.textureID = scoreComponent.digitTextureID[scoreDigits[i]];
        }


        return kContinue;
    }

private:
    EntityID mScoreID = 0;
    int hundreds = 0;
    int tens = 0;
    int ones = 0;
};

}  // namespace ECSEngine
