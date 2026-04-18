#pragma once

#include "pch.h"

namespace practice
{
    class Controller
    {
    public:
        enum class State
        {
            S0,
            S1,
            S2,
            S3,
            S4,
            S5
        };

        Controller();

        bool InputPhone(const std::string& phone);
        bool InputCard(const std::string& cardNumber);
        bool InputOwner(const std::string& ownerName);
        bool InputCVV(const std::string& cvv);
        bool ConfirmPayment();
        void Reset();

        State GetState() const;
        static std::string StateToString(State state);

    private:
        State state_;
    };

    void RunPractice7();
}
