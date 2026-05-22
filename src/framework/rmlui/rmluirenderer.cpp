#include "rmluirenderer.h"
#include <framework/global.h>
#include <framework/graphics/painter.h>
#include <framework/graphics/texturemanager.h>
#include <framework/graphics/coordsbuffer.h>
#include <framework/core/logger.h>
#include <RmlUi/Core.h>
#include <algorithm>

RmlUiRenderInterface::RmlUiRenderInterface()
{
}

RmlUiRenderInterface::~RmlUiRenderInterface()
{
    for (auto* geo : m_geometries)
        delete geo;
    m_geometries.clear();
}

Rml::CompiledGeometryHandle RmlUiRenderInterface::CompileGeometry(
    Rml::Span<const Rml::Vertex> vertices, Rml::Span<const int> indices)
{
    auto* geo = new CompiledGeometry();
    for (const auto& v : vertices) {
        geo->vertices.push_back(v.position.x);
        geo->vertices.push_back(v.position.y);
        geo->vertices.push_back(static_cast<float>(v.colour.red) / 255.0f);
        geo->vertices.push_back(static_cast<float>(v.colour.green) / 255.0f);
        geo->vertices.push_back(static_cast<float>(v.colour.blue) / 255.0f);
        geo->vertices.push_back(static_cast<float>(v.colour.alpha) / 255.0f);
        geo->vertices.push_back(v.tex_coord.x);
        geo->vertices.push_back(v.tex_coord.y);
    }
    for (const auto& idx : indices)
        geo->indices.push_back(idx);

    m_geometries.push_back(geo);
    return Rml::CompiledGeometryHandle(geo);
}

void RmlUiRenderInterface::RenderGeometry(Rml::CompiledGeometryHandle geometry,
    Rml::Vector2f translation, Rml::TextureHandle texture)
{
    auto* geo = reinterpret_cast<CompiledGeometry*>(geometry);
    if (!geo || geo->indices.empty()) return;

    CoordsBuffer coordsBuffer;
    for (size_t i = 0; i + 2 < geo->indices.size(); i += 3) {
        int i0 = geo->indices[i];
        int i1 = geo->indices[i + 1];
        int i2 = geo->indices[i + 2];

        float* v = geo->vertices.data();
        int stride = 8;
        Point p0(v[i0 * stride] + translation.x, v[i0 * stride + 1] + translation.y);
        Point p1(v[i1 * stride] + translation.x, v[i1 * stride + 1] + translation.y);
        Point p2(v[i2 * stride] + translation.x, v[i2 * stride + 1] + translation.y);

        PointF t0(v[i0 * stride + 6], v[i0 * stride + 7]);
        PointF t1(v[i1 * stride + 6], v[i1 * stride + 7]);
        PointF t2(v[i2 * stride + 6], v[i2 * stride + 7]);

        coordsBuffer.addTexturedTriangle(p0, p1, p2, t0, t1, t2);
    }

    if (texture != 0) {
        auto it = m_textureCache.find(texture);
        if (it != m_textureCache.end())
            g_painter->setTexture(it->second);
    } else {
        g_painter->setTexture(nullptr);
    }

    g_painter->drawCoords(coordsBuffer);
}

void RmlUiRenderInterface::ReleaseGeometry(Rml::CompiledGeometryHandle geometry)
{
    auto* geo = reinterpret_cast<CompiledGeometry*>(geometry);
    if (!geo) return;
    auto it = std::find(m_geometries.begin(), m_geometries.end(), geo);
    if (it != m_geometries.end())
        m_geometries.erase(it);
    delete geo;
}

void RmlUiRenderInterface::EnableScissorRegion(bool enable)
{
    m_scissorEnabled = enable;
    if (!enable)
        g_painter->resetClipRect();
}

void RmlUiRenderInterface::SetScissorRegion(Rml::Rectanglei region)
{
    m_scissorRegion = region;
    if (m_scissorEnabled) {
        Rect r(region.Left(), region.Top(), region.Width(), region.Height());
        g_painter->setClipRect(r);
    }
}

Rml::TextureHandle RmlUiRenderInterface::LoadTexture(Rml::Vector2i& texture_dimensions,
    const Rml::String& source)
{
    try {
        TexturePtr tex = g_textures.getTexture(source);
        if (!tex) return 0;
        Rml::TextureHandle handle = reinterpret_cast<Rml::TextureHandle>(tex.get());
        texture_dimensions = Rml::Vector2i(tex->getWidth(), tex->getHeight());
        m_textureCache[handle] = tex;
        return handle;
    } catch (...) {
        g_logger.error(stdext::format("RmlUi: failed to load texture '%s'", source));
        return 0;
    }
}

Rml::TextureHandle RmlUiRenderInterface::GenerateTexture(Rml::Span<const Rml::byte> source,
    Rml::Vector2i source_dimensions)
{
    return 0;
}

void RmlUiRenderInterface::ReleaseTexture(Rml::TextureHandle texture)
{
    m_textureCache.erase(texture);
}

void RmlUiRenderInterface::SetTransform(const Rml::Matrix4f* transform)
{
    if (transform) {
        const float* d = transform->data();
        Matrix3 m;
        m(1,1) = d[0];  m(1,2) = d[4];  m(1,3) = d[12];
        m(2,1) = d[1];  m(2,2) = d[5];  m(2,3) = d[13];
        g_painter->setTransformMatrix(m);
    } else {
        g_painter->resetTransformMatrix();
    }
}
