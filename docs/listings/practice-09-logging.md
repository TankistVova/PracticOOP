# Практика 9. Логгирование

Исходные файлы:

- `PracticHub/PracticeLogging.h`
- `PracticHub/PracticeLogging.cpp`

Листинг демонстрирует использование фабрики для создания консольного и файлового логгера.

```cpp
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

    static std::unique_ptr<ILogger> CreateLogger(
        LoggerType type,
        const std::filesystem::path& filePath = {});
};

void ConsoleLogger::SendLog(const std::string& message)
{
    if (GetConsoleWindow() == nullptr)
    {
        AllocConsole();
    }

    const std::wstring line = ToWide("[" + GetTimeStamp() + "] " + message + "\n");
    DWORD written = 0;
    WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE),
                  line.c_str(),
                  static_cast<DWORD>(line.size()),
                  &written,
                  nullptr);
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

std::unique_ptr<ILogger> LoggerFactory::CreateLogger(
    LoggerType type,
    const std::filesystem::path& filePath)
{
    if (type == LoggerType::Console)
    {
        return std::make_unique<ConsoleLogger>();
    }

    return std::make_unique<FileLogger>(filePath);
}

void RunPractice9()
{
    std::unique_ptr<ILogger> consoleLogger =
        LoggerFactory::CreateLogger(LoggerFactory::LoggerType::Console);
    check(static_cast<bool>(consoleLogger), "factory creates console logger");

    const std::filesystem::path filePath =
        std::filesystem::current_path() / "practice_data" / "logs" / "practice9.log";
    std::unique_ptr<ILogger> fileLogger =
        LoggerFactory::CreateLogger(LoggerFactory::LoggerType::File, filePath);

    check(static_cast<bool>(fileLogger), "factory creates file logger");
    check(std::filesystem::exists(filePath), "file logger creates a text log file");
}
```
