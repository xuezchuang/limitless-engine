#pragma once

#include <vector>
#include <limitless/fx/emitters/abstract_emitter.hpp>

namespace Limitless {
    class Context;
    class Camera;
}

namespace Limitless::fx {
    class AbstractEmitter;

    enum class ModuleType {
        InitialLocation,
        InitialRotation,
        InitialVelocity,
        InitialColor,
        InitialSize,
        InitialAcceleration,

        InitialMeshLocation,
        MeshLocationAttachment,
        SubUV,
        VelocityByLife,
        ColorByLife,
        RotationRate,
        SizeByLife,
        CustomMaterial,
        CustomMaterialByLife,
        Time,

        Beam_InitialDisplacement,
        Beam_InitialOffset,
        Beam_InitialRebuild,
        Beam_InitialTarget,
        BeamSpeed,
        BeamBuilder,

        // should be at last
        Lifetime
    };

    template<typename Particle>
    class Module
    {
    protected:
        ModuleType type;
        explicit Module(ModuleType _type) noexcept : type {_type} {}

        Module(const Module&) = default;
        Module(Module&&) noexcept = default;
    public:
        virtual ~Module() = default;

        [[nodiscard]] virtual Module* clone() const = 0;

        [[nodiscard]] const auto& getType() const noexcept { return type; }

        virtual void initialize([[maybe_unused]] AbstractEmitter& e, [[maybe_unused]] Particle& p, [[maybe_unused]] size_t index) noexcept {}

        virtual void deinitialize([[maybe_unused]] const std::vector<size_t>& indices) {}

        virtual void update([[maybe_unused]] AbstractEmitter& emitter,
                            [[maybe_unused]] std::vector<Particle>& particles,
                            [[maybe_unused]] float dt,
                            [[maybe_unused]] Context& ctx,
                            [[maybe_unused]] const Camera& camera) noexcept {}
    };
}
