# Практика 3. Поиск инцидентов

Исходные файлы:

- `PracticHub/PracticeIncidents.h`
- `PracticHub/PracticeIncidents.cpp`

Этот листинг показывает поиск инцидентов по ключевым словам в описании.

```cpp
class IncidentSearcher
{
public:
    explicit IncidentSearcher(const IncidentAnalyzer& analyzer);
    std::vector<Incident> SearchByKeywords(
        const std::vector<std::wstring>& keywords) const;

private:
    const IncidentAnalyzer& analyzer_;
};

std::vector<Incident> IncidentSearcher::SearchByKeywords(
    const std::vector<std::wstring>& keywords) const
{
    std::vector<std::wstring> normalizedKeywords;
    for (const std::wstring& keyword : keywords)
    {
        const std::wstring normalizedKeyword = ToLower(Trim(keyword));
        if (!normalizedKeyword.empty() &&
            std::find(normalizedKeywords.begin(), normalizedKeywords.end(), normalizedKeyword) ==
                normalizedKeywords.end())
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
        const bool containsKeyword = std::any_of(
            normalizedKeywords.begin(),
            normalizedKeywords.end(),
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

void RunPractice3()
{
    IncidentAnalyzer analyzer;
    const std::size_t loaded = LoadDemoIncidents(analyzer);

    IncidentSearcher searcher(analyzer);
    const std::vector<Incident> found =
        searcher.SearchByKeywords({ L"загрузка", L"файл", L"EXE" });
    const std::vector<Incident> passwordSearch =
        searcher.SearchByKeywords({ L"пароль" });
    const std::vector<Incident> emptySearch =
        searcher.SearchByKeywords({ L"   ", L"" });

    check(loaded == 3, "analyzer loads only valid incidents");
    check(found.size() == 1, "search returns incidents with at least one matching keyword");
    check(!found.empty() && found.front().GetId() == L"АБ123456",
          "search finds the EXE file incident");
    check(passwordSearch.size() == 1 && passwordSearch.front().GetId() == L"ВГ654321",
          "search finds the password incident");
    check(emptySearch.empty(), "empty keywords do not produce matches");
}
```
