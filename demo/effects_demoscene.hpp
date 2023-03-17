#include <limitless/scene.hpp>
#include <limitless/instances/effect_instance.hpp>
#include <corecrt_math_defines.h>
namespace LimitlessDemo {
    class EffectsScene : public Limitless::Scene {
    private:
        Limitless::EffectInstance* hurricane {};

        void addInstances(Limitless::Assets& assets);
    public:
        EffectsScene(Limitless::Context& ctx, Limitless::Assets& assets);
        ~EffectsScene() override = default;

        void update(Limitless::Context& context, const Limitless::Camera& camera) override;
    };
}