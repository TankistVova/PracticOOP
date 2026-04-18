# Практика 7. Контроллер

Исходные файлы:

- `PracticHub/PracticeController.h`
- `PracticHub/PracticeController.cpp`

Листинг демонстрирует конечный автомат оплаты с проверкой переходов между состояниями.

```cpp
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

bool Controller::InputPhone(const std::string& phone)
{
    static const std::regex pattern(R"(^\+?\d{11,12}$)");
    if (state_ != State::S0 || !std::regex_match(phone, pattern))
    {
        return false;
    }

    state_ = State::S1;
    return true;
}

bool Controller::InputCard(const std::string& cardNumber)
{
    static const std::regex pattern(R"(^\d{16}$)");
    if (state_ != State::S0 || !std::regex_match(cardNumber, pattern))
    {
        return false;
    }

    state_ = State::S2;
    return true;
}

bool Controller::InputOwner(const std::string& ownerName)
{
    static const std::regex pattern(R"(^[A-Za-zА-Яа-яЁё]+(?:\s[A-Za-zА-Яа-яЁё]+)+$)");
    if (state_ != State::S2 || !std::regex_match(ownerName, pattern))
    {
        return false;
    }

    state_ = State::S3;
    return true;
}

bool Controller::InputCVV(const std::string& cvv)
{
    static const std::regex pattern(R"(^\d{3}$)");
    if (state_ != State::S3 || !std::regex_match(cvv, pattern))
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

void RunPractice7()
{
    Controller controller;

    check(controller.InputPhone("+79991234567"), "valid phone switches S0 -> S1");
    check(controller.GetState() == Controller::State::S1, "state is S1");
    check(controller.ConfirmPayment(), "payment switches S1 -> S5");

    controller.Reset();
    check(controller.InputCard("1111222233334444"), "valid card switches S0 -> S2");
    check(controller.InputOwner("IVAN IVANOV"), "valid owner switches S2 -> S3");
    check(controller.InputCVV("123"), "valid cvv switches S3 -> S4");
    check(controller.ConfirmPayment(), "payment switches S4 -> S5");
}
```

