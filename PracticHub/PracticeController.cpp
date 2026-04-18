#include "pch.h"
#include "PracticeController.h"

namespace practice
{
    namespace
    {
        bool Matches(const std::string& value, const std::regex& pattern)
        {
            return std::regex_match(value, pattern);
        }
    }

    Controller::Controller()
        : state_(State::S0)
    {
    }

    bool Controller::InputPhone(const std::string& phone)
    {
        static const std::regex pattern(R"(^\+?\d{11,12}$)");
        if (state_ != State::S0 || !Matches(phone, pattern))
        {
            return false;
        }

        state_ = State::S1;
        return true;
    }

    bool Controller::InputCard(const std::string& cardNumber)
    {
        static const std::regex pattern(R"(^\d{16}$)");
        if (state_ != State::S0 || !Matches(cardNumber, pattern))
        {
            return false;
        }

        state_ = State::S2;
        return true;
    }

    bool Controller::InputOwner(const std::string& ownerName)
    {
        static const std::regex pattern(R"(^[A-Za-zА-Яа-яЁё]+(?:\s[A-Za-zА-Яа-яЁё]+)+$)");
        if (state_ != State::S2 || !Matches(ownerName, pattern))
        {
            return false;
        }

        state_ = State::S3;
        return true;
    }

    bool Controller::InputCVV(const std::string& cvv)
    {
        static const std::regex pattern(R"(^\d{3}$)");
        if (state_ != State::S3 || !Matches(cvv, pattern))
        {
            return false;
        }

        state_ = State::S4;
        return true;
    }

    bool Controller::ConfirmPayment()
    {
        if (state_ != State::S1 && state_ != State::S4)
        {
            return false;
        }

        state_ = State::S5;
        return true;
    }

    void Controller::Reset()
    {
        state_ = State::S0;
    }

    Controller::State Controller::GetState() const
    {
        return state_;
    }

    std::string Controller::StateToString(State state)
    {
        switch (state)
        {
        case State::S0:
            return "S0";
        case State::S1:
            return "S1";
        case State::S2:
            return "S2";
        case State::S3:
            return "S3";
        case State::S4:
            return "S4";
        case State::S5:
            return "S5";
        }

        return "Unknown";
    }

    void RunPractice7()
    {
        std::cout << "\n=== Practice 7: Controller ===\n";

        Controller controller;
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

        check(controller.InputPhone("+79991234567"), "valid phone switches S0 -> S1");
        check(controller.GetState() == Controller::State::S1, "state is S1");
        check(controller.ConfirmPayment(), "payment switches S1 -> S5");
        check(controller.GetState() == Controller::State::S5, "state is S5 after phone branch");

        controller.Reset();
        check(controller.InputCard("1111222233334444"), "valid card switches S0 -> S2");
        check(controller.GetState() == Controller::State::S2, "state is S2");
        check(controller.InputOwner("IVAN IVANOV"), "valid owner switches S2 -> S3");
        check(controller.InputCVV("123"), "valid cvv switches S3 -> S4");
        check(controller.ConfirmPayment(), "payment switches S4 -> S5");
        check(controller.GetState() == Controller::State::S5, "state is S5 after card branch");

        controller.Reset();
        check(controller.GetState() == Controller::State::S0, "reset returns to S0");
        check(!controller.InputPhone("123"), "invalid phone is rejected");

        std::cout << "Passed " << passed << " of " << total << " tests.\n";
    }
}
