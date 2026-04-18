#pragma once

#include "pch.h"

namespace practice
{
    enum class DamageLevel
    {
        None = 0,
        Minor = 1,
        Noticeable = 2,
        Major = 3
    };

    class Incident
    {
    public:
        Incident() = delete;
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

        const std::wstring& GetId() const;
        const std::wstring& GetTitle() const;
        const std::wstring& GetDescription() const;
        DamageLevel GetDamage() const;
        const std::wstring& GetDateTime() const;
        const std::wstring& GetLocation() const;

        static bool ValidateId(const std::wstring& id);
        static bool ValidateTitle(const std::wstring& title);
        static bool ValidateDescription(const std::wstring& description);
        static bool ValidateDateTime(const std::wstring& dateTime);
        static bool ValidateWindowsFileName(const std::wstring& fileName);
        static std::optional<DamageLevel> ParseDamage(const std::wstring& damageText);
        static std::wstring DamageToString(DamageLevel damage);

    private:
        std::wstring id_;
        std::wstring title_;
        std::wstring description_;
        DamageLevel damage_;
        std::wstring dateTime_;
        std::wstring location_;
    };

    class IncidentAnalyzer
    {
    public:
        std::size_t LoadIncidents(const std::vector<std::wstring>& fileNames, const std::filesystem::path& directory);
        std::vector<Incident> GetMostDangerous(std::size_t count = 3) const;
        std::vector<Incident> GetLeastDangerous(std::size_t count = 3) const;
        const std::vector<Incident>& GetIncidents() const;

    private:
        std::vector<Incident> incidents_;
    };

    class IncidentSearcher
    {
    public:
        explicit IncidentSearcher(const IncidentAnalyzer& analyzer);
        std::vector<Incident> SearchByKeywords(const std::vector<std::wstring>& keywords) const;

    private:
        const IncidentAnalyzer& analyzer_;
    };

    std::size_t LoadDemoIncidents(IncidentAnalyzer& analyzer);

    void RunPractice1();
    void RunPractice2();
    void RunPractice3();
}
