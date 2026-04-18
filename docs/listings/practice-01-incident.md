# Практика 1. Класс инцидента

Исходные файлы:

- `PracticHub/PracticeIncidents.h`
- `PracticHub/PracticeIncidents.cpp`

Ниже приведён ключевой листинг класса инцидента: создание объекта, проверка полей, чтение из файла и сохранение.

```cpp
class Incident
{
public:
    Incident(std::wstring id,
             std::wstring title,
             std::wstring description,
             DamageLevel damage,
             std::wstring dateTime,
             std::wstring location);

    static Incident Invalid(std::wstring location);
    static Incident LoadFromFile(const std::filesystem::path& filePath);
    static bool ValidateFile(const std::filesystem::path& filePath);

    bool IsValid() const;
    bool SaveToFile(const std::filesystem::path& filePath) const;

    static bool ValidateId(const std::wstring& id);
    static bool ValidateTitle(const std::wstring& title);
    static bool ValidateDescription(const std::wstring& description);
    static bool ValidateDateTime(const std::wstring& dateTime);
    static bool ValidateWindowsFileName(const std::wstring& fileName);
};

Incident::Incident(std::wstring id,
                   std::wstring title,
                   std::wstring description,
                   DamageLevel damage,
                   std::wstring dateTime,
                   std::wstring location)
    : id_(std::move(id)),
      title_(std::move(title)),
      description_(std::move(description)),
      damage_(damage),
      dateTime_(std::move(dateTime)),
      location_(std::move(location))
{
    if (!HasValidIncidentFields(id_, title_, description_, damage_, dateTime_, location_))
    {
        id_ = L"-1";
    }
}

Incident Incident::LoadFromFile(const std::filesystem::path& filePath)
{
    std::ifstream input(filePath, std::ios::binary);
    if (!input)
    {
        return Invalid(filePath.filename().wstring());
    }

    std::map<std::string, std::wstring> values;
    std::string line;
    while (std::getline(input, line))
    {
        const std::size_t delimiter = line.find(':');
        if (delimiter == std::string::npos)
        {
            continue;
        }

        const std::string key = line.substr(0, delimiter);
        const std::wstring value = Utf8ToWide(line.substr(delimiter + 1));
        values[key] = value;
    }

    const auto damage = ParseDamage(values["damage"]);
    const bool valid =
        ValidateId(values["id"]) &&
        ValidateTitle(values["title"]) &&
        ValidateDescription(values["description"]) &&
        damage.has_value() &&
        ValidateDateTime(values["datetime"]) &&
        ValidateWindowsFileName(values["location"]);

    if (!valid)
    {
        return Invalid(filePath.filename().wstring());
    }

    return Incident(
        values["id"],
        values["title"],
        values["description"],
        *damage,
        values["datetime"],
        values["location"]);
}

bool Incident::SaveToFile(const std::filesystem::path& filePath) const
{
    if (!IsValid() || !ValidateWindowsFileName(filePath.filename().wstring()))
    {
        return false;
    }

    std::ofstream file(filePath, std::ios::binary);
    if (!file)
    {
        return false;
    }

    file << "id:" << WideToUtf8(id_) << "\n";
    file << "title:" << WideToUtf8(title_) << "\n";
    file << "description:" << WideToUtf8(description_) << "\n";
    file << "damage:" << WideToUtf8(DamageToString(damage_)) << "\n";
    file << "datetime:" << WideToUtf8(dateTime_) << "\n";
    file << "location:" << WideToUtf8(location_) << "\n";
    return true;
}

bool Incident::ValidateId(const std::wstring& id)
{
    static const std::wregex pattern(LR"(^[A-Za-zА-Яа-яЁё]{2}\d{6}$)");
    return std::regex_match(id, pattern);
}

bool Incident::ValidateDescription(const std::wstring& description)
{
    std::wistringstream stream(description);
    std::wstring word;
    std::size_t words = 0;
    while (stream >> word)
    {
        ++words;
    }

    return words >= 5;
}

void RunPractice1()
{
    Incident incident(
        L"ЖЗ222333",
        L"Подозрительная активность",
        L"Сотрудник повторно запускал неизвестный исполняемый файл в системе",
        DamageLevel::Minor,
        L"14.03.2025 09:40",
        L"manual_incident.log");

    const std::filesystem::path outFile =
        std::filesystem::current_path() / "practice_data" / "practice1" / "manual_incident.log";

    check(incident.IsValid(), "valid incident object can be created");
    check(incident.SaveToFile(outFile), "valid incident can be saved to file");
    check(Incident::ValidateFile(outFile), "saved file passes validation");
}
```

