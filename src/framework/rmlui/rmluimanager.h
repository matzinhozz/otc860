#ifndef RMLUIMANAGER_H
#define RMLUIMANAGER_H

#include <RmlUi/Core/Context.h>
#include <RmlUi/Core/ElementDocument.h>
#include <RmlUi/Core/DataModelHandle.h>
#include <string>
#include <unordered_map>
#include <memory>

class RmlUiRenderInterface;
class RmlUiSystemInterface;

class RmlUiManager {
public:
    void init();
    void terminate();

    void update();
    void render();

    void resize(int width, int height);

    Rml::Context* getMainContext() { return m_mainContext; }
    Rml::Context* createContext(const std::string& name, int width, int height);
    void removeContext(const std::string& name);

    Rml::ElementDocument* loadDocument(const std::string& path, Rml::Context* context = nullptr);
    Rml::ElementDocument* loadDocumentFromString(const std::string& rml, Rml::Context* context = nullptr);
    void closeDocument(Rml::ElementDocument* doc);

    void processKeyDown(Rml::Input::KeyIdentifier key, int modifiers);
    void processKeyUp(Rml::Input::KeyIdentifier key, int modifiers);
    void processTextInput(Rml::Character c);
    void processTextInput(const std::string& text);
    void processMouseMove(int x, int y, int modifiers);
    void processMouseButtonDown(int button, int modifiers);
    void processMouseButtonUp(int button, int modifiers);
    void processMouseWheel(float delta, int modifiers);

    bool loadFontFace(const std::string& path);

    std::unordered_map<std::string, Rml::Context*> m_contexts;

private:
    Rml::Context* m_mainContext = nullptr;
    RmlUiRenderInterface* m_renderInterface = nullptr;
    RmlUiSystemInterface* m_systemInterface = nullptr;
    bool m_initialized = false;
};

extern RmlUiManager g_rmlui;

#endif
