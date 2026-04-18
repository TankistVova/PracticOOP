# Практика 2. Анализатор инцидентов

Исходные файлы:

- `PracticHub/PracticeIncidents.h`
- `PracticHub/PracticeIncidents.cpp`

Ключевой листинг показывает загрузку набора инцидентов и сортировку по уровню опасности.

```cpp
class IncidentAnalyzer
{
public:
    std::size_t LoadIncidents(
        const std::vector<std::wstring>& fileNames,
        const std::filesystem::path& directory);

    std::vector<Incident> GetMostDangerous(std::size_t count = 3) const;
    std::vector<Incident> GetLeastDangerous(std::size_t count = 3) const;
    const std::vector<Incident>& GetIncidents() const;

private:
    std::vector<Incident> incidents_;
};

std::size_t IncidentAnalyzer::LoadIncidents(
    const std::vector<std::wstring>& fileNames,
    const std::filesystem::path& directory)
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

void RunPractice2()
{
    IncidentAnalyzer analyzer;
    const std::size_t loaded = LoadDemoIncidents(analyzer);
    const std::vector<Incident> mostDangerous = analyzer.GetMostDangerous();
    const std::vector<Incident> leastDangerous = analyzer.GetLeastDangerous();

    check(loaded == 3, "loader returns the number of valid incidents");
    check(mostDangerous.size() == 3, "most dangerous returns up to three incidents");
    check(leastDangerous.size() == 3, "least dangerous returns up to three incidents");
    check(!mostDangerous.empty() && mostDangerous.front().GetDamage() == DamageLevel::Major,
          "the most dangerous incident is first");
    check(!leastDangerous.empty() && leastDangerous.front().GetDamage() == DamageLevel::Minor,
          "the least dangerous incident is first");
}
```

