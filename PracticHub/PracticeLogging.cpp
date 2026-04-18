#include "pch.h"
#include "PracticeLogging.h"
#include <Windows.h>

namespace practice
{
    namespace
    {
        std::string GetTimeStamp()
        {
            std::time_t now = std::time(nullptr);
            std::tm localTime{};
            localtime_s(&localTime, &now);

            std::ostringstream stream;
            stream << std::put_time(&localTime, "%d.%m.%Y %H:%M:%S");
            return stream.str();
        }

        std::wstring ToWide(const std::string& value)
        {
            if (value.empty())
            {
                return L"";
            }

            const int size = MultiByteToWideChar(CP_UTF8, 0, value.c_str(), -1, nullptr, 0);
            std::wstring result(static_cast<std::size_t>(size - 1), L'\0');
            MultiByteToWideChar(CP_UTF8, 0, value.c_str(), -1, result.data(), size);
            return result;
        }

        std::filesystem::path GetDefaultLogFilePath()
        {
            return std::filesystem::current_path() / "practice_data" / "logs" / "security.log";
        }
    }

    ConsoleLogger::ConsoleLogger()
        : createdAt_(GetTimeStamp())
    {
    }

    void ConsoleLogger::SendLog(const std::string& message)
    {
        if (GetConsoleWindow() == nullptr)
        {
            AllocConsole();
        }

        const std::wstring line = ToWide("[" + GetTimeStamp() + "] " + message + "\n");
        DWORD written = 0;
        WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE), line.c_str(), static_cast<DWORD>(line.size()), &written, nullptr);
    }

    std::string ConsoleLogger::GetInfo() const
    {
        return "ConsoleLogger | created: " + createdAt_;
    }

    FileLogger::FileLogger(std::filesystem::path filePath)
        : filePath_(filePath.empty() ? GetDefaultLogFilePath() : std::move(filePath)),
          createdAt_(GetTimeStamp())
    {
    }

    void FileLogger::SendLog(const std::string& message)
    {
        if (!filePath_.parent_path().empty())
        {
            std::filesystem::create_directories(filePath_.parent_path());
        }

        std::ofstream output(filePath_, std::ios::app | std::ios::binary);
        output << "[" << GetTimeStamp() << "] " << message << "\n";
    }

    std::string FileLogger::GetInfo() const
    {
        return "FileLogger | created: " + createdAt_ + " | file: " + filePath_.string();
    }

    const std::filesystem::path& FileLogger::GetPath() const
    {
        return filePath_;
    }

    std::unique_ptr<ILogger> LoggerFactory::CreateLogger(LoggerType type, const std::filesystem::path& filePath)
    {
        if (type == LoggerType::Console)
        {
            return std::make_unique<ConsoleLogger>();
        }

        return std::make_unique<FileLogger>(filePath);
    }

    void RunPractice9()
    {
        std::cout << "\n=== Practice 9: Logger Factory ===\n";

        int passed = 0;
        int total = 0;

        const auto check = [&](bool condition, const std::string& name) {
            ++total;
            if (condition)
            {
                ++passed;
                std::cout << "[OK] " << name << "\n";
            }
            else
            {
                std::cout << "[FAIL] " << name << "\n";
            }
        };

        std::unique_ptr<ILogger> consoleLogger = LoggerFactory::CreateLogger(LoggerFactory::LoggerType::Console);
        check(static_cast<bool>(consoleLogger), "factory creates console logger");
        if (consoleLogger)
        {
            consoleLogger->SendLog("Security event: console logger created");
            std::cout << consoleLogger->GetInfo() << "\n";
        }

        const std::filesystem::path filePath = std::filesystem::current_path() / "practice_data" / "logs" / "practice9.log";
        std::unique_ptr<ILogger> fileLogger = LoggerFactory::CreateLogger(LoggerFactory::LoggerType::File, filePath);
        check(static_cast<bool>(fileLogger), "factory creates file logger");
        if (fileLogger)
        {
            fileLogger->SendLog("Security event: failed login attempt saved to file");
            std::cout << fileLogger->GetInfo() << "\n";
        }

        check(std::filesystem::exists(filePath), "file logger creates a text log file");
        std::cout << "Passed " << passed << " of " << total << " checks.\n";
    }
}
