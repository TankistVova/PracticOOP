#pragma once

#include "pch.h"

namespace practice
{
    class ILogger
    {
    public:
        virtual ~ILogger() = default;
        virtual void SendLog(const std::string& message) = 0;
        virtual std::string GetInfo() const = 0;
    };

    class ConsoleLogger : public ILogger
    {
    public:
        ConsoleLogger();
        void SendLog(const std::string& message) override;
        std::string GetInfo() const override;

    private:
        std::string createdAt_;
    };

    class FileLogger : public ILogger
    {
    public:
        explicit FileLogger(std::filesystem::path filePath);
        void SendLog(const std::string& message) override;
        std::string GetInfo() const override;
        const std::filesystem::path& GetPath() const;

    private:
        std::filesystem::path filePath_;
        std::string createdAt_;
    };

    class LoggerFactory
    {
    public:
        enum class LoggerType
        {
            Console,
            File
        };

        static std::unique_ptr<ILogger> CreateLogger(LoggerType type, const std::filesystem::path& filePath = {});
    };

    void RunPractice9();
}
