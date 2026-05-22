#include "rmluisystem.h"
#include <framework/global.h>
#include <framework/stdext/time.h>
#include <framework/stdext/format.h>
#include <framework/core/logger.h>

double RmlUiSystemInterface::GetElapsedTime()
{
    return stdext::micros() / 1000000.0;
}

bool RmlUiSystemInterface::LogMessage(Rml::Log::Type type, const Rml::String& message)
{
    switch (type) {
    case Rml::Log::LT_ALWAYS:
    case Rml::Log::LT_INFO:
        g_logger.info(stdext::format("[RmlUi] %s", message));
        break;
    case Rml::Log::LT_WARNING:
        g_logger.warning(stdext::format("[RmlUi] %s", message));
        break;
    case Rml::Log::LT_ASSERT:
    case Rml::Log::LT_ERROR:
        g_logger.error(stdext::format("[RmlUi] %s", message));
        break;
    case Rml::Log::LT_DEBUG:
        g_logger.debug(stdext::format("[RmlUi] %s", message));
        break;
    default:
        g_logger.info(stdext::format("[RmlUi] %s", message));
        break;
    }
    return true;
}
