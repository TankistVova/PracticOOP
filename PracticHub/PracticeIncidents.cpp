#include "pch.h"
#include "PracticeIncidents.h"

namespace practice
{
    namespace
    {
        std::wstring Trim(const std::wstring& value)
        {
            const auto begin = value.find_first_not_of(L" \t\r\n");
            if (begin == std::wstring::npos)
            {
                return L"";
            }

            const auto end = value.find_last_not_of(L" \t\r\n");
            return value.substr(begin, end - begin + 1);
        }

        std::wstring ToLower(std::wstring value)
        {
            std::transform(value.begin(), value.end(), value.begin(), [](wchar_t ch) {
                return static_cast<wchar_t>(std::towlower(ch));
            });
            return value;
        }

        std::string WideToUtf8(const std::wstring& text)
        {
            std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
            return converter.to_bytes(text);
        }

        std::wstring Utf8ToWide(const std::string& text)
        {
            std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
            return converter.from_bytes(text);
        }

        std::wstring JoinPathInfo(const std::filesystem::path& path)
        {
            return path.filename().wstring().empty() ? path.wstring() : path.filename().wstring();
        }

        bool IsLoadRequestValid(const std::vector<std::wstring>& fileNames, const std::filesystem::path& directory)
        {
            if (fileNames.empty() || directory.empty() || !std::filesystem::exists(directory) || !std::filesystem::is_directory(directory))
            {
                return false;
            }

            return std::all_of(fileNames.begin(), fileNames.end(), [](const std::wstring& fileName) {
                return Incident::ValidateWindowsFileName(Trim(fileName));
                });
        }

        void PrintIncidentList(const std::vector<Incident>& incidents, const std::string& title)
        {
            std::wcout << L"\n" << Utf8ToWide(title) << L"\n";
            for (const Incident& incident : incidents)
            {
                std::wcout << L"  [" << incident.GetId() << L"] "
                    << incident.GetTitle() << L" | "
                    << Incident::DamageToString(incident.GetDamage()) << L" | "
                    << incident.GetDateTime() << L"\n";
            }
        }

        std::vector<std::wstring> CreateDemoIncidentFiles()
        {
            const std::filesystem::path directory = std::filesystem::current_path() / "practice_data" / "incidents";
            std::filesystem::create_directories(directory);

            const std::vector<std::pair<std::wstring, std::wstring>> files = {
                {
                    L"incident01.log",
                    L"id:АБ123456\n"
                    L"title:Попытка загрузки файла\n"
                    L"description:Пользователь пытался загрузить опасный EXE файл на сервер\n"
                    L"damage:Заметный\n"
                    L"datetime:10.02.2022 12:15\n"
                    L"location:incident01.log\n"
                },
                {
                    L"incident02.log",
                    L"id:ВГ654321\n"
                    L"title:Подбор пароля\n"
                    L"description:Неизвестный пользователь многократно подбирал пароль к учетной записи\n"
                    L"damage:Существенный\n"
                    L"datetime:11.02.2022 08:30\n"
                    L"location:incident02.log\n"
                },
                {
                    L"incident03.log",
                    L"id:ДЕ111111\n"
                    L"title:Ошибка резервного копирования\n"
                    L"description:Система резервного копирования временно пропустила один ночной запуск\n"
                    L"damage:Незначительный\n"
                    L"datetime:12.02.2022 02:45\n"
                    L"location:incident03.log\n"
                },
                {
                    L"broken.log",
                    L"id:BAD\n"
                    L"title:WrongTitle123\n"
                    L"description:Слишком мало слов\n"
                    L"damage:Неизвестный\n"
                    L"datetime:2022-02-12\n"
                    L"location:broken.log\n"
                }
            };

            std::vector<std::wstring> names;
            for (const auto& [name, content] : files)
            {
                std::ofstream output(directory / name, std::ios::binary);
                output << WideToUtf8(content);
                names.push_back(name);
            }

            return names;
        }

        bool HasValidIncidentFields(const std::wstring& id,
                                    const std::wstring& title,
                                    const std::wstring& description,
                                    DamageLevel damage,
                                    const std::wstring& dateTime,
                                    const std::wstring& location)
        {
            return Incident::ValidateId(id) &&
                Incident::ValidateTitle(title) &&
                Incident::ValidateDescription(description) &&
                damage >= DamageLevel::None &&
                damage <= DamageLevel::Major &&
                Incident::ValidateDateTime(dateTime) &&
                Incident::ValidateWindowsFileName(location);
        }
    }

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

    Incident Incident::Invalid(std::wstring location)
    {
        return Incident(L"-1", L"", L"", DamageLevel::None, L"", std::move(location));
    }

    Incident Incident::LoadFromFile(const std::filesystem::path& filePath)
    {
        std::ifstream input(filePath, std::ios::binary);
        if (!input)
        {
            return Invalid(JoinPathInfo(filePath));
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
            const std::wstring value = Trim(Utf8ToWide(line.substr(delimiter + 1)));
            values[key] = value;
        }

        const auto damage = ParseDamage(values["damage"]);
        const std::wstring location = values["location"];

        const bool valid =
            ValidateId(values["id"]) &&
            ValidateTitle(values["title"]) &&
            ValidateDescription(values["description"]) &&
            damage.has_value() &&
            ValidateDateTime(values["datetime"]) &&
            ValidateWindowsFileName(location) &&
            ValidateWindowsFileName(filePath.filename().wstring());

        if (!valid)
        {
            return Invalid(JoinPathInfo(filePath));
        }

        return Incident(values["id"],
                        values["title"],
                        values["description"],
                        *damage,
                        values["datetime"],
                        location);
    }

    bool Incident::ValidateFile(const std::filesystem::path& filePath)
    {
        return LoadFromFile(filePath).IsValid();
    }

    bool Incident::IsValid() const
    {
        return id_ != L"-1" &&
            HasValidIncidentFields(id_, title_, description_, damage_, dateTime_, location_);
    }

    bool Incident::SaveToFile(const std::filesystem::path& filePath) const
    {
        if (!IsValid() ||
            !ValidateWindowsFileName(filePath.filename().wstring()) ||
            !ValidateWindowsFileName(location_))
        {
            return false;
        }

        std::ostringstream output;
        output << "id:" << WideToUtf8(id_) << "\n";
        output << "title:" << WideToUtf8(title_) << "\n";
        output << "description:" << WideToUtf8(description_) << "\n";
        output << "damage:" << WideToUtf8(DamageToString(damage_)) << "\n";
        output << "datetime:" << WideToUtf8(dateTime_) << "\n";
        output << "location:" << WideToUtf8(location_) << "\n";

        std::ofstream file(filePath, std::ios::binary);
        if (!file)
        {
            return false;
        }

        file << output.str();
        return true;
    }

    const std::wstring& Incident::GetId() const
    {
        return id_;
    }

    const std::wstring& Incident::GetTitle() const
    {
        return title_;
    }

    const std::wstring& Incident::GetDescription() const
    {
        return description_;
    }

    DamageLevel Incident::GetDamage() const
    {
        return damage_;
    }

    const std::wstring& Incident::GetDateTime() const
    {
        return dateTime_;
    }

    const std::wstring& Incident::GetLocation() const
    {
        return location_;
    }

    bool Incident::ValidateId(const std::wstring& id)
    {
        static const std::wregex pattern(LR"(^[A-Za-zА-Яа-яЁё]{2}\d{6}$)");
        return std::regex_match(id, pattern);
    }

    bool Incident::ValidateTitle(const std::wstring& title)
    {
        static const std::wregex pattern(LR"(^[А-Яа-яЁё\s-]+$)");
        return !Trim(title).empty() && std::regex_match(title, pattern);
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

    bool Incident::ValidateDateTime(const std::wstring& dateTime)
    {
        static const std::wregex pattern(LR"(^\d{2}\.\d{2}\.\d{4}\s\d{2}:\d{2}$)");
        return std::regex_match(dateTime, pattern);
    }

    bool Incident::ValidateWindowsFileName(const std::wstring& fileName)
    {
        if (fileName.empty())
        {
            return false;
        }

        static const std::wregex invalidChars(LR"([<>:"/\\|?*])");
        if (std::regex_search(fileName, invalidChars))
        {
            return false;
        }

        const wchar_t last = fileName.back();
        return last != L' ' && last != L'.';
    }

    std::optional<DamageLevel> Incident::ParseDamage(const std::wstring& damageText)
    {
        const std::wstring normalized = ToLower(Trim(damageText));

        if (normalized == L"отсутствует")
        {
            return DamageLevel::None;
        }
        if (normalized == L"незначительный")
        {
            return DamageLevel::Minor;
        }
        if (normalized == L"заметный")
        {
            return DamageLevel::Noticeable;
        }
        if (normalized == L"существенный")
        {
            return DamageLevel::Major;
        }

        return std::nullopt;
    }

    std::wstring Incident::DamageToString(DamageLevel damage)
    {
        switch (damage)
        {
        case DamageLevel::None:
            return L"Отсутствует";
        case DamageLevel::Minor:
            return L"Незначительный";
        case DamageLevel::Noticeable:
            return L"Заметный";
        case DamageLevel::Major:
            return L"Существенный";
        }

        return L"Неизвестно";
    }

    std::size_t IncidentAnalyzer::LoadIncidents(const std::vector<std::wstring>& fileNames, const std::filesystem::path& directory)
    {
        incidents_.clear();

        if (!IsLoadRequestValid(fileNames, directory))
        {
            return 0;
        }

        for (const std::wstring& fileName : fileNames)
        {
            const Incident incident = Incident::LoadFromFile(directory / Trim(fileName));
            if (incident.IsValid())
            {
                incidents_.push_back(incident);
            }
        }

        return incidents_.size();
    }

    std::vector<Incident> IncidentAnalyzer::GetMostDangerous(std::size_t count) const
    {
        if (count == 0)
        {
            return {};
        }

        std::vector<Incident> result = incidents_;
        std::sort(result.begin(), result.end(), [](const Incident& left, const Incident& right) {
            if (left.GetDamage() != right.GetDamage())
            {
                return static_cast<int>(left.GetDamage()) > static_cast<int>(right.GetDamage());
            }

            return left.GetId() < right.GetId();
        });

        if (result.size() > count)
        {
            result.erase(result.begin() + static_cast<std::ptrdiff_t>(count), result.end());
        }

        return result;
    }

    std::vector<Incident> IncidentAnalyzer::GetLeastDangerous(std::size_t count) const
    {
        if (count == 0)
        {
            return {};
        }

        std::vector<Incident> result = incidents_;
        std::sort(result.begin(), result.end(), [](const Incident& left, const Incident& right) {
            if (left.GetDamage() != right.GetDamage())
            {
                return static_cast<int>(left.GetDamage()) < static_cast<int>(right.GetDamage());
            }

            return left.GetId() < right.GetId();
        });

        if (result.size() > count)
        {
            result.erase(result.begin() + static_cast<std::ptrdiff_t>(count), result.end());
        }

        return result;
    }

    const std::vector<Incident>& IncidentAnalyzer::GetIncidents() const
    {
        return incidents_;
    }

    IncidentSearcher::IncidentSearcher(const IncidentAnalyzer& analyzer)
        : analyzer_(analyzer)
    {
    }

    std::vector<Incident> IncidentSearcher::SearchByKeywords(const std::vector<std::wstring>& keywords) const
    {
        std::vector<std::wstring> normalizedKeywords;
        for (const std::wstring& keyword : keywords)
        {
            const std::wstring normalizedKeyword = ToLower(Trim(keyword));
            if (!normalizedKeyword.empty() &&
                std::find(normalizedKeywords.begin(), normalizedKeywords.end(), normalizedKeyword) == normalizedKeywords.end())
            {
                normalizedKeywords.push_back(normalizedKeyword);
            }
        }

        if (normalizedKeywords.empty())
        {
            return {};
        }

        std::vector<Incident> result;
        for (const Incident& incident : analyzer_.GetIncidents())
        {
            const std::wstring description = ToLower(incident.GetDescription());
            const bool containsKeyword = std::any_of(normalizedKeywords.begin(), normalizedKeywords.end(),
                [&description](const std::wstring& keyword) {
                    return !keyword.empty() && description.find(keyword) != std::wstring::npos;
                });

            if (containsKeyword)
            {
                result.push_back(incident);
            }
        }

        return result;
    }

    std::size_t LoadDemoIncidents(IncidentAnalyzer& analyzer)
    {
        const std::vector<std::wstring> fileNames = CreateDemoIncidentFiles();
        const std::filesystem::path directory = std::filesystem::current_path() / "practice_data" / "incidents";
        return analyzer.LoadIncidents(fileNames, directory);
    }

    void RunPractice1()
    {
        std::wcout << L"\n=== Practice 1: Incident class ===\n";

        Incident incident(L"ЖЗ222333",
                          L"Подозрительная активность",
                          L"Сотрудник повторно запускал неизвестный исполняемый файл в системе",
                          DamageLevel::Minor,
                          L"14.03.2025 09:40",
                          L"manual_incident.log");

        const std::filesystem::path outFile = std::filesystem::current_path() / "practice_data" / "practice1" / "manual_incident.log";
        std::filesystem::create_directories(outFile.parent_path());

        Incident invalidIncident(L"BAD",
                                 L"Wrong123",
                                 L"Слишком мало слов",
                                 DamageLevel::Major,
                                 L"2025-03-14 09:40",
                                 L"bad?.log");

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

        check(incident.IsValid(), "valid incident object can be created");
        check(incident.SaveToFile(outFile), "valid incident can be saved to file");
        check(Incident::ValidateFile(outFile), "saved file passes validation");

        const Incident loaded = Incident::LoadFromFile(outFile);
        check(loaded.IsValid(), "saved incident can be loaded back");
        check(loaded.GetId() == L"ЖЗ222333", "loaded incident keeps identifier");
        check(loaded.GetTitle() == L"Подозрительная активность", "loaded incident keeps title");
        check(!invalidIncident.IsValid(), "invalid incident gets identifier -1");

        const Incident broken = Incident::LoadFromFile(std::filesystem::current_path() / "practice_data" / "practice1" / "missing.log");
        check(!broken.IsValid() && broken.GetId() == L"-1", "missing file loads as invalid incident");

        std::wcout << L"Saved file: " << outFile.wstring() << L"\n";
        std::cout << "Passed " << passed << " of " << total << " checks.\n";
    }

    void RunPractice2()
    {
        std::wcout << L"\n=== Practice 2: Incident analyzer ===\n";

        IncidentAnalyzer analyzer;
        const std::size_t loaded = LoadDemoIncidents(analyzer);
        const std::vector<Incident> mostDangerous = analyzer.GetMostDangerous();
        const std::vector<Incident> leastDangerous = analyzer.GetLeastDangerous();

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

        check(loaded == 3, "loader returns the number of valid incidents");
        check(mostDangerous.size() == 3, "most dangerous returns up to three incidents");
        check(leastDangerous.size() == 3, "least dangerous returns up to three incidents");
        check(!mostDangerous.empty() && mostDangerous.front().GetDamage() == DamageLevel::Major, "the most dangerous incident is first");
        check(!leastDangerous.empty() && leastDangerous.front().GetDamage() == DamageLevel::Minor, "the least dangerous incident is first");

        std::wcout << L"Loaded incidents: " << loaded << L"\n";
        PrintIncidentList(mostDangerous, "Most dangerous incidents:");
        PrintIncidentList(leastDangerous, "Least dangerous incidents:");
        std::cout << "Passed " << passed << " of " << total << " checks.\n";
    }

    void RunPractice3()
    {
        std::wcout << L"\n=== Practice 3: Incident searcher ===\n";

        IncidentAnalyzer analyzer;
        const std::size_t loaded = LoadDemoIncidents(analyzer);

        IncidentSearcher searcher(analyzer);
        const std::vector<Incident> found = searcher.SearchByKeywords({ L"загрузка", L"файл", L"EXE" });
        const std::vector<Incident> passwordSearch = searcher.SearchByKeywords({ L"пароль" });
        const std::vector<Incident> emptySearch = searcher.SearchByKeywords({ L"   ", L"" });

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

        check(loaded == 3, "analyzer loads only valid incidents");
        check(found.size() == 1, "search returns incidents with at least one matching keyword");
        check(!found.empty() && found.front().GetId() == L"АБ123456", "search finds the EXE file incident");
        check(passwordSearch.size() == 1 && passwordSearch.front().GetId() == L"ВГ654321", "search finds the password incident");
        check(emptySearch.empty(), "empty keywords do not produce matches");

        PrintIncidentList(found, "Search results:");
        std::cout << "Passed " << passed << " of " << total << " checks.\n";
    }
}
