#include "rmluimanager.h"
#include "rmluirenderer.h"
#include "rmluisystem.h"
#include <framework/global.h>
#include <framework/core/logger.h>
#include <framework/core/resourcemanager.h>
#include <framework/stdext/string.h>
#include <RmlUi/Core.h>
#include <RmlUi/Debugger.h>

RmlUiManager g_rmlui;

void RmlUiManager::init()
{
    if (m_initialized) return;

    m_renderInterface = new RmlUiRenderInterface();
    m_systemInterface = new RmlUiSystemInterface();

    Rml::SetRenderInterface(m_renderInterface);
    Rml::SetSystemInterface(m_systemInterface);

    Rml::Initialise();

    m_initialized = true;
    g_logger.info("[RmlUi] Initialized");
}

void RmlUiManager::terminate()
{
    if (!m_initialized) return;

    for (auto& pair : m_contexts) {
        Rml::RemoveContext(pair.first);
    }
    m_contexts.clear();
    m_mainContext = nullptr;

    Rml::Shutdown();

    delete m_systemInterface;
    m_systemInterface = nullptr;

    delete m_renderInterface;
    m_renderInterface = nullptr;

    m_initialized = false;
    g_logger.info("[RmlUi] Terminated");
}

void RmlUiManager::update()
{
    if (!m_initialized) return;

    for (auto& pair : m_contexts) {
        pair.second->Update();
    }

    for (auto it = m_contexts.begin(); it != m_contexts.end();) {
        if (it->second->GetNumDocuments() == 0) {
            it = m_contexts.erase(it);
        } else {
            ++it;
        }
    }
}

void RmlUiManager::render()
{
    if (!m_initialized) return;

    for (auto& pair : m_contexts) {
        pair.second->Render();
    }
}

void RmlUiManager::resize(int width, int height)
{
    for (auto& pair : m_contexts) {
        pair.second->SetDimensions(Rml::Vector2i(width, height));
    }
}

Rml::Context* RmlUiManager::createContext(const std::string& name, int width, int height)
{
    if (!m_initialized) return nullptr;

    Rml::Context* ctx = Rml::CreateContext(name, Rml::Vector2i(width, height));
    if (ctx) {
        m_contexts[name] = ctx;
        if (!m_mainContext)
            m_mainContext = ctx;
    }
    return ctx;
}

void RmlUiManager::removeContext(const std::string& name)
{
    if (m_mainContext && m_mainContext->GetName() == name)
        m_mainContext = nullptr;
    Rml::RemoveContext(name);
    m_contexts.erase(name);
}

Rml::ElementDocument* RmlUiManager::loadDocument(const std::string& path, Rml::Context* context)
{
    if (!m_initialized) return nullptr;

    Rml::Context* ctx = context ? context : m_mainContext;
    if (!ctx) return nullptr;

    auto resolvedPath = g_resources.resolvePath(path);
    auto doc = ctx->LoadDocument(resolvedPath);
    if (doc) {
        doc->Show();
        g_logger.info(stdext::format("[RmlUi] Loaded document: %s", path));
    } else {
        g_logger.error(stdext::format("[RmlUi] Failed to load document: %s", path));
    }
    return doc;
}

Rml::ElementDocument* RmlUiManager::loadDocumentFromString(const std::string& rml, Rml::Context* context)
{
    if (!m_initialized) return nullptr;

    Rml::Context* ctx = context ? context : m_mainContext;
    if (!ctx) return nullptr;

    auto doc = ctx->LoadDocumentFromMemory(rml, "memory.rml");
    if (doc) {
        doc->Show();
    }
    return doc;
}

void RmlUiManager::closeDocument(Rml::ElementDocument* doc)
{
    if (doc)
        doc->Close();
}

bool RmlUiManager::loadFontFace(const std::string& path)
{
    auto resolvedPath = g_resources.resolvePath(path);
    return Rml::LoadFontFace(resolvedPath);
}

void RmlUiManager::processKeyDown(Rml::Input::KeyIdentifier key, int modifiers)
{
    for (auto& pair : m_contexts)
        pair.second->ProcessKeyDown(key, modifiers);
}

void RmlUiManager::processKeyUp(Rml::Input::KeyIdentifier key, int modifiers)
{
    for (auto& pair : m_contexts)
        pair.second->ProcessKeyUp(key, modifiers);
}

void RmlUiManager::processTextInput(Rml::Character c)
{
    for (auto& pair : m_contexts)
        pair.second->ProcessTextInput(c);
}

void RmlUiManager::processTextInput(const std::string& text)
{
    for (auto& pair : m_contexts)
        pair.second->ProcessTextInput(text);
}

void RmlUiManager::processMouseMove(int x, int y, int modifiers)
{
    for (auto& pair : m_contexts)
        pair.second->ProcessMouseMove(x, y, modifiers);
}

void RmlUiManager::processMouseButtonDown(int button, int modifiers)
{
    for (auto& pair : m_contexts)
        pair.second->ProcessMouseButtonDown(button, modifiers);
}

void RmlUiManager::processMouseButtonUp(int button, int modifiers)
{
    for (auto& pair : m_contexts)
        pair.second->ProcessMouseButtonUp(button, modifiers);
}

void RmlUiManager::processMouseWheel(float delta, int modifiers)
{
    for (auto& pair : m_contexts)
        pair.second->ProcessMouseWheel(delta, modifiers);
}
