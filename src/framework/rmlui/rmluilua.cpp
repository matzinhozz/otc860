#include <framework/luaengine/luainterface.h>
#include <framework/core/logger.h>
#include <framework/core/resourcemanager.h>
#include <framework/stdext/string.h>
#include "rmluimanager.h"
#include <RmlUi/Core.h>
#include <RmlUi/Debugger.h>

class LuaEventListener : public Rml::EventListener {
public:
    LuaEventListener(const std::string& code) : m_code(code) {}
    void ProcessEvent(Rml::Event& event) override {
        g_lua.evaluateExpression(m_code);
    }
private:
    std::string m_code;
};

void registerRmlUiLuaFunctions()
{
    g_lua.registerSingletonClass("g_rmlui");
    g_lua.bindSingletonFunction("g_rmlui", "init", &RmlUiManager::init, &g_rmlui);
    g_lua.bindSingletonFunction("g_rmlui", "terminate", &RmlUiManager::terminate, &g_rmlui);
    g_lua.bindSingletonFunction("g_rmlui", "update", &RmlUiManager::update, &g_rmlui);
    g_lua.bindSingletonFunction("g_rmlui", "render", &RmlUiManager::render, &g_rmlui);
    g_lua.bindSingletonFunction("g_rmlui", "resize", &RmlUiManager::resize, &g_rmlui);

    g_lua.bindSingletonFunction("g_rmlui", "createContext", [](const std::string& name, int w, int h) {
        auto* ctx = g_rmlui.createContext(name, w, h);
        return ctx ? name : "";
    });

    g_lua.bindSingletonFunction("g_rmlui", "removeContext", &RmlUiManager::removeContext, &g_rmlui);

    g_lua.bindSingletonFunction("g_rmlui", "loadFontFace", &RmlUiManager::loadFontFace, &g_rmlui);

    g_lua.bindSingletonFunction("g_rmlui", "loadDocument", [](const std::string& path, const std::string& contextName) {
        Rml::Context* ctx = contextName.empty() ? g_rmlui.getMainContext() : g_rmlui.m_contexts[contextName];
        auto* doc = g_rmlui.loadDocument(path, ctx);
        return reinterpret_cast<uintptr_t>(doc);
    });

    g_lua.bindSingletonFunction("g_rmlui", "closeDocument", [](uintptr_t docPtr) {
        auto* doc = reinterpret_cast<Rml::ElementDocument*>(docPtr);
        g_rmlui.closeDocument(doc);
    });

    g_lua.bindSingletonFunction("g_rmlui", "setProperty", [](uintptr_t elemPtr, const std::string& name, const std::string& value) {
        auto* elem = reinterpret_cast<Rml::Element*>(elemPtr);
        if (elem) elem->SetProperty(name, value);
    });

    g_lua.bindSingletonFunction("g_rmlui", "getProperty", [](uintptr_t elemPtr, const std::string& name) -> std::string {
        auto* elem = reinterpret_cast<Rml::Element*>(elemPtr);
        if (elem) return elem->GetProperty<Rml::String>(name);
        return "";
    });

    g_lua.bindSingletonFunction("g_rmlui", "getElementById", [](uintptr_t docPtr, const std::string& id) -> uintptr_t {
        auto* doc = reinterpret_cast<Rml::ElementDocument*>(docPtr);
        if (doc) return reinterpret_cast<uintptr_t>(doc->GetElementById(id));
        return 0;
    });

    g_lua.bindSingletonFunction("g_rmlui", "setInnerRML", [](uintptr_t elemPtr, const std::string& rml) {
        auto* elem = reinterpret_cast<Rml::Element*>(elemPtr);
        if (elem) elem->SetInnerRML(rml);
    });

    g_lua.bindSingletonFunction("g_rmlui", "getInnerRML", [](uintptr_t elemPtr) -> std::string {
        auto* elem = reinterpret_cast<Rml::Element*>(elemPtr);
        return elem ? elem->GetInnerRML() : "";
    });

    g_lua.bindSingletonFunction("g_rmlui", "setAttribute", [](uintptr_t elemPtr, const std::string& name, const std::string& value) {
        auto* elem = reinterpret_cast<Rml::Element*>(elemPtr);
        if (elem) elem->SetAttribute(name, value);
    });

    g_lua.bindSingletonFunction("g_rmlui", "getAttribute", [](uintptr_t elemPtr, const std::string& name) -> std::string {
        auto* elem = reinterpret_cast<Rml::Element*>(elemPtr);
        if (elem) return elem->GetAttribute<Rml::String>(name, "");
        return "";
    });

    g_lua.bindSingletonFunction("g_rmlui", "addEventListener", [](uintptr_t elemPtr, const std::string& event, const std::string& luaCode) {
        auto* elem = reinterpret_cast<Rml::Element*>(elemPtr);
        if (!elem) return;
        auto* listener = new LuaEventListener(luaCode);
        elem->AddEventListener(event, listener);
    });

    g_lua.bindSingletonFunction("g_rmlui", "createDataModel", [](const std::string& contextName, const std::string& modelName) -> bool {
        Rml::Context* ctx = contextName.empty() ? g_rmlui.getMainContext() : g_rmlui.m_contexts[contextName];
        if (!ctx) return false;
        Rml::DataModelConstructor constructor = ctx->CreateDataModel(modelName);
        return (bool)constructor;
    });

    g_lua.bindSingletonFunction("g_rmlui", "debugger", [](bool visible) {
        Rml::Debugger::SetVisible(visible);
    });

    g_logger.info("[RmlUi] Lua bindings registered");
}
