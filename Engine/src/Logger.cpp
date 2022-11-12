#include "../inc/IO/Logger.h"

void Engine::Log(const char* fmt, ...)
{
    if (ENABLE_LOGGING) {
        static char buffer[1 << 11] = { 0 };

        //Format string
        va_list params;
        va_start(params, fmt);
        vsprintf_s(buffer, fmt, params);
        va_end(params);

        //Output to VS output window
#ifdef _WIN32
        OutputDebugStringA(buffer);
#endif

        //Output to Logfile
        if (LOG_TO_FILE) {
            FILE* fp;
            fopen_s(&fp, "Log.txt", "a+");
            if (fp != nullptr) {
                fprintf(fp, "%s", buffer);
                fclose(fp);
            }
        }
    }
}

void Engine::LogTime()
{
    auto now = Timing::Now();
    auto t = std::localtime(&now);
    int year = t->tm_year + 1900;
    int month = t->tm_mon + 1;
    int day = t->tm_mday;
    int hours = t->tm_hour;
    int minutes = t->tm_min;
    int seconds = t->tm_sec;

    Log("\n[%04d/%02d/%02d - %02d:%02d:%02d]\n", year, month, day, hours, minutes, seconds);
}
