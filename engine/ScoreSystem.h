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
        score = manager.template GetComponent<ScoreComponent>(mScoreID).score;
        hundreds = score / 100;
        tens = (score / 10) % 10;
        ones = score % 10;

        manager.template GetComponent<ScoreComponent>(mScoreID).digitID[0] = manager.template GetComponent<ScoreComponent>(mScoreID).digitTextureID[hundreds];
        manager.template GetComponent<ScoreComponent>(mScoreID).digitID[1] = manager.template GetComponent<ScoreComponent>(mScoreID).digitTextureID[tens];
        manager.template GetComponent<ScoreComponent>(mScoreID).digitID[2] = manager.template GetComponent<ScoreComponent>(mScoreID).digitTextureID[ones];

        return kContinue;
    }

private:
    EntityID mScoreID = 0;
    int score = 0;
    int hundreds = 0;
    int tens = 0;
    int ones = 0;
};

}  // namespace ECSEngine
