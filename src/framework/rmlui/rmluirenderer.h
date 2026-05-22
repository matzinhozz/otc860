#ifndef RMLUIRENDERER_H
#define RMLUIRENDERER_H

#include <RmlUi/Core/RenderInterface.h>
#include <RmlUi/Core/Types.h>
#include <framework/graphics/declarations.h>
#include <framework/graphics/texture.h>
#include <memory>
#include <vector>
#include <unordered_map>

class RmlUiRenderInterface : public Rml::RenderInterface {
public:
    RmlUiRenderInterface();
    ~RmlUiRenderInterface();

    Rml::CompiledGeometryHandle CompileGeometry(Rml::Span<const Rml::Vertex> vertices,
        Rml::Span<const int> indices) override;

    void RenderGeometry(Rml::CompiledGeometryHandle geometry, Rml::Vector2f translation,
        Rml::TextureHandle texture) override;

    void ReleaseGeometry(Rml::CompiledGeometryHandle geometry) override;

    void EnableScissorRegion(bool enable) override;
    void SetScissorRegion(Rml::Rectanglei region) override;

    Rml::TextureHandle LoadTexture(Rml::Vector2i& texture_dimensions,
        const Rml::String& source) override;
    Rml::TextureHandle GenerateTexture(Rml::Span<const Rml::byte> source,
        Rml::Vector2i source_dimensions) override;
    void ReleaseTexture(Rml::TextureHandle texture) override;

    void SetTransform(const Rml::Matrix4f* transform) override;

private:
    struct CompiledGeometry {
        std::vector<float> vertices;
        std::vector<int> indices;
    };
    std::vector<CompiledGeometry*> m_geometries;
    bool m_scissorEnabled = false;
    Rml::Rectanglei m_scissorRegion;
    std::unordered_map<Rml::TextureHandle, TexturePtr> m_textureCache;
};

#endif
