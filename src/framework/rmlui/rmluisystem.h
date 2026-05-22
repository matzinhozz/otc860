#ifndef RMLUISYSTEM_H
#define RMLUISYSTEM_H

#include <RmlUi/Core/SystemInterface.h>

class RmlUiSystemInterface : public Rml::SystemInterface {
public:
    double GetElapsedTime() override;
    bool LogMessage(Rml::Log::Type type, const Rml::String& message) override;
};

#endif
