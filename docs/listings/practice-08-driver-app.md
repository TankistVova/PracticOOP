# Практика 8. Приложение водителя

Исходные файлы:

- `PracticHub/PracticeDriverApp.h`
- `PracticHub/PracticeDriverApp.cpp`

Ключевой листинг отражает поведение приложения водителя: регистрацию, выбор заказа, движение по маршруту и завершение смены.

```cpp
class DriverApp
{
public:
    enum class State
    {
        AwaitingRegistration,
        ReadyForOrders,
        OrderSelected,
        Driving,
        ShiftEnded
    };

    DriverApp();

    bool RegisterDriver(const std::wstring& driverName, const std::wstring& driverId);
    bool ShowIncomingOrders();
    bool SelectOrder(std::size_t index);
    bool AcceptSelectedOrder();
    bool ChooseRouteVariant(const std::wstring& variant);
    bool AdvanceRoute(int minutes);
    bool AdvanceShift(int minutes);
    bool FinishRoute();

    bool CanReceiveOrders() const;
    bool CanAcceptSelectedOrder() const;
    bool CanManipulateMap() const;
    bool CanChooseRouteVariant() const;
    bool CanSelectNextOrder() const;
    bool IsShiftEnded() const;
    int GetRemainingRouteMinutes() const;
};

bool DriverApp::RegisterDriver(const std::wstring& driverName, const std::wstring& driverId)
{
    if (registered_ || state_ == State::ShiftEnded)
    {
        return false;
    }

    if (!IsValidName(driverName) || !IsValidDriverId(driverId))
    {
        AppendEvent(L"Регистрация отклонена: проверьте ФИО и табельный номер формата TX-000.");
        return false;
    }

    registered_ = true;
    driverName_ = driverName;
    driverId_ = driverId;
    state_ = State::ReadyForOrders;
    AppendEvent(L"Водитель зарегистрирован. Приём заказов и их отображение разблокированы.");
    return true;
}

bool DriverApp::AcceptSelectedOrder()
{
    if (!CanAcceptSelectedOrder())
    {
        return false;
    }

    currentOrderIndex_ = selectedOrderIndex_;
    selectedOrderIndex_ = -1;
    orderStatuses_[currentOrderIndex_] = DriverOrderStatus::Current;
    remainingRouteMinutes_ = orders_[currentOrderIndex_].routeMinutes;
    routeVariant_ = L"Базовый маршрут";
    state_ = State::Driving;
    AppendEvent(L"Заказ принят к выполнению.");
    return true;
}

bool DriverApp::AdvanceRoute(int minutes)
{
    if (state_ != State::Driving || minutes <= 0 || state_ == State::ShiftEnded)
    {
        return false;
    }

    shiftMinutes_ += minutes;
    remainingRouteMinutes_ = std::max(0, remainingRouteMinutes_ - minutes);

    if (!nearFinishWindowOpened_ && remainingRouteMinutes_ > 0 && remainingRouteMinutes_ <= 10)
    {
        OpenNearFinishWindow();
    }

    if (shiftMinutes_ >= 14 * 60)
    {
        ForceShiftEnd();
    }

    return true;
}

bool DriverApp::AdvanceShift(int minutes)
{
    if (state_ == State::ShiftEnded || minutes <= 0)
    {
        return false;
    }

    shiftMinutes_ += minutes;
    if (shiftMinutes_ >= 14 * 60)
    {
        ForceShiftEnd();
    }

    return true;
}

void RunPractice8()
{
    DriverApp app;

    check(!app.CanReceiveOrders(), "orders are blocked before registration");
    check(app.RegisterDriver(L"Ivan Petrov", L"TX-101"), "driver registration succeeds");
    check(app.ShowIncomingOrders(), "incoming orders are displayed");
    check(app.SelectOrder(0), "driver can select one active order");
    check(app.AcceptSelectedOrder(), "driver can confirm the selected order");
    check(app.ChooseRouteVariant(L"Shortest route"), "route variant can be chosen");

    const int minutesToNearFinish = std::max(0, app.GetRemainingRouteMinutes() - 10);
    check(app.AdvanceRoute(minutesToNearFinish), "route can be progressed to the last 10 minutes");
    check(app.CanSelectNextOrder(), "next order can be selected 10 minutes before finish");
}
```
