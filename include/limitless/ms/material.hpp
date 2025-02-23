#pragma once

#include <limitless/core/uniform.hpp>
#include <limitless/pipeline/shader_pass_types.hpp>
#include <limitless/ms/property.hpp>
#include <limitless/ms/blending.hpp>
#include <limitless/ms/shading.hpp>

#include <unordered_map>
#include <glm/glm.hpp>
#include <stdexcept>
#include <vector>
#include <map>

namespace Limitless {
    class Buffer;
}

namespace Limitless::ms {
    class material_property_not_found : public std::runtime_error {
    public:
        using std::runtime_error::runtime_error;
    };

    class Material final {
    private:
        // contains set of properties
        // the set cant be changed at run-time, but values of props can
        std::map<Property, std::unique_ptr<Uniform>> properties;

        // blending can be changed at run-time
        Blending blending {};

        // shading is compile-time only
        Shading shading {};

        // determines whether cull face on or off
        bool two_sided {};

        // uses screen space refraction based on IoR & Absorption
        bool refraction {};

        // contains material name
        std::string name;

        // unique index describes which shader to use
        uint64_t shader_index {};

        // contains ModelShader type for which this material is used
        ModelShaders model_shaders;

        // opengl buffer that stores properties
        std::shared_ptr<Buffer> material_buffer;

        // properties offsets in the buffer
        std::unordered_map<std::string, uint64_t> uniform_offsets;

        // contains additional custom properties
        std::map<std::string, std::unique_ptr<Uniform>> uniforms;

        // vertex snippet
        std::string vertex_snippet;

        // fragment snippet
        std::string fragment_snippet;

        // global snippet
        std::string global_snippet;

        // tessellation snippet
        std::string tessellation_snippet;

        template<typename V>
        void map(std::vector<std::byte>& block, const Uniform& uniform) const;
        void map(std::vector<std::byte>& block, Uniform& uniform);
        void map();

        friend void swap(Material&, Material&) noexcept;
        Material() = default;

        friend class MaterialBuilder;

        // compares material properties that can be used in same shader
        friend bool operator==(const Material& lhs, const Material& rhs) noexcept;
        friend bool operator<(const Material& lhs, const Material& rhs) noexcept;
    public:
        ~Material() = default;

        Material(const Material&);
        Material& operator=(Material);

        Material(Material&&) noexcept = default;
        Material& operator=(Material&&) noexcept = default;

        void update();

        [[nodiscard]] const UniformValue<glm::vec4>& getColor() const;
        [[nodiscard]] const UniformValue<glm::vec4>& getEmissiveColor() const;
        [[nodiscard]] const UniformValue<glm::vec2>& getTesselationFactor() const;
        [[nodiscard]] const UniformValue<float>& getMetallic() const;
        [[nodiscard]] const UniformValue<float>& getRoughness() const;
        [[nodiscard]] const UniformValue<float>& getIoR() const;
        [[nodiscard]] const UniformValue<float>& getAbsorption() const;
        [[nodiscard]] const UniformSampler& getMetallicTexture() const;
        [[nodiscard]] const UniformSampler& getRoughnessTexture() const;
        [[nodiscard]] const UniformSampler& getDiffuse() const;
        [[nodiscard]] const UniformSampler& getNormal() const;
        [[nodiscard]] const UniformSampler& getEmissiveMask() const;
        [[nodiscard]] const UniformSampler& getBlendMask() const;

        UniformValue<glm::vec4>& getColor();
        UniformValue<glm::vec4>& getEmissiveColor();
        UniformValue<glm::vec2>& getTesselationFactor();
        UniformValue<float>& getMetallic();
        UniformValue<float>& getRoughness();
        UniformValue<float>& getIoR();
        UniformValue<float>& getAbsorption();
        UniformSampler& getMetallicTexture();
        UniformSampler& getRoughnessTexture();
        UniformSampler& getDiffuse();
        UniformSampler& getNormal();
        UniformSampler& getEmissiveMask();
        UniformSampler& getBlendMask();

        [[nodiscard]] const auto& getModelShaders() const noexcept { return model_shaders; }
        [[nodiscard]] auto getBlending() const noexcept { return blending; }
        [[nodiscard]] auto getShading() const noexcept { return shading; }
        [[nodiscard]] auto getTwoSided() const noexcept { return two_sided; }
        [[nodiscard]] const auto& getName() const noexcept { return name; }
        [[nodiscard]] auto getShaderIndex() const noexcept { return shader_index; }
        [[nodiscard]] const auto& getVertexSnippet() const noexcept { return vertex_snippet; }
        [[nodiscard]] const auto& getRefraction() const noexcept { return refraction; }
        [[nodiscard]] const auto& getFragmentSnippet() const noexcept { return fragment_snippet; }
        [[nodiscard]] const auto& getGlobalSnippet() const noexcept { return global_snippet; }
        [[nodiscard]] const auto& getTessellationSnippet() const noexcept { return tessellation_snippet; }
        [[nodiscard]] const auto& getMaterialBuffer() const noexcept { return material_buffer; }
        [[nodiscard]] const auto& getProperties() const noexcept { return properties; }
        [[nodiscard]] const auto& getUniforms() const noexcept { return uniforms; }

        auto& getBlending() noexcept { return blending; }
        auto& getTwoSided() noexcept { return two_sided; }

        bool contains(Property property) const;
        bool contains(const std::string& name) const;

        template<typename Uniform>
        Uniform& getProperty(const std::string& uniform_name) {
            try {
                return static_cast<Uniform&>(*uniforms.at(uniform_name));
            } catch (...) {
                throw material_property_not_found(uniform_name);
            }
        }

        template<typename Uniform>
        const Uniform& getProperty(const std::string& uniform_name) const {
            try {
                return static_cast<const Uniform&>(*uniforms.at(uniform_name));
            } catch (...) {
                throw material_property_not_found(uniform_name);
            }
        }
    };

    void swap(Material& lhs, Material& rhs) noexcept;

    bool operator==(const Material& lhs, const Material& rhs) noexcept;
    bool operator<(const Material& lhs, const Material& rhs) noexcept;
}