#include "pch.h"
#include "PracticeDriverApp.h"

namespace practice
{
    namespace
    {
        constexpr int kShiftLimitMinutes = 14 * 60;
    }

    DriverApp::DriverApp()
        : state_(State::AwaitingRegistration),
          registered_(false),
          ordersVisible_(false),
          nearFinishWindowOpened_(false),
          shiftMinutes_(0),
          remainingRouteMinutes_(0),
          selectedOrderIndex_(-1),
          currentOrderIndex_(-1),
          reservedOrderIndex_(-1),
          orders_(CreateOrders()),
          orderStatuses_(orders_.size(), DriverOrderStatus::Available)
    {
        AppendEvent(L"Приложение запущено: дорожная сеть показана, действия заблокированы до регистрации.");
    }

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
        AppendEvent(L"Водитель зарегистрирован. Прием заказов и их отображение разблокированы.");
        return true;
    }

    bool DriverApp::ShowIncomingOrders()
    {
        if (!CanReceiveOrders() || !HasAvailableOrders())
        {
            AppendEvent(L"Нет доступных заказов для отображения.");
            return false;
        }

        ordersVisible_ = true;
        AppendEvent(L"На схеме отображены активные заказы.");
        return true;
    }

    bool DriverApp::SelectOrder(std::size_t index)
    {
        if (state_ == State::ShiftEnded || index >= orders_.size())
        {
            return false;
        }

        if (orderStatuses_[index] == DriverOrderStatus::Completed || static_cast<int>(index) == currentOrderIndex_)
        {
            return false;
        }

        if (state_ == State::ReadyForOrders || state_ == State::OrderSelected)
        {
            selectedOrderIndex_ = static_cast<int>(index);
            state_ = State::OrderSelected;
            AppendEvent(L"Выбран заказ " + orders_[index].id + L". Остальные заказы заблокированы.");
            return true;
        }

        if (state_ == State::Driving && CanSelectNextOrder())
        {
            if (reservedOrderIndex_ != -1 && reservedOrderIndex_ != static_cast<int>(index))
            {
                orderStatuses_[reservedOrderIndex_] = DriverOrderStatus::Available;
            }

            reservedOrderIndex_ = static_cast<int>(index);
            orderStatuses_[index] = DriverOrderStatus::Reserved;
            AppendEvent(L"Выбран следующий заказ " + orders_[index].id + L".");
            return true;
        }

        return false;
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
        nearFinishWindowOpened_ = false;
        state_ = State::Driving;
        AppendEvent(L"Заказ " + orders_[currentOrderIndex_].id + L" принят к выполнению.");
        return true;
    }

    bool DriverApp::ChooseRouteVariant(const std::wstring& variant)
    {
        if (!CanChooseRouteVariant() || variant.empty())
        {
            return false;
        }

        routeVariant_ = variant;
        AppendEvent(L"Выбран вариант пути: " + routeVariant_ + L".");
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

        if (shiftMinutes_ >= kShiftLimitMinutes)
        {
            ForceShiftEnd();
        }
        else if (remainingRouteMinutes_ == 0)
        {
            AppendEvent(L"Маршрут завершен. Можно закрыть текущий рейс.");
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
        if (shiftMinutes_ >= kShiftLimitMinutes)
        {
            ForceShiftEnd();
        }
        else
        {
            AppendEvent(L"Смена продвинута. Текущее время работы: " + std::to_wstring(shiftMinutes_ / 60) +
                L" ч " + std::to_wstring(shiftMinutes_ % 60) + L" мин.");
        }

        return true;
    }

    bool DriverApp::FinishRoute()
    {
        if (state_ != State::Driving || currentOrderIndex_ == -1)
        {
            return false;
        }

        const int finishedOrderIndex = currentOrderIndex_;
        orderStatuses_[finishedOrderIndex] = DriverOrderStatus::Completed;
        currentOrderIndex_ = -1;
        remainingRouteMinutes_ = 0;
        routeVariant_.clear();
        nearFinishWindowOpened_ = false;
        AppendEvent(L"Рейс по заказу " + orders_[finishedOrderIndex].id + L" завершен.");

        if (reservedOrderIndex_ != -1)
        {
            selectedOrderIndex_ = reservedOrderIndex_;
            reservedOrderIndex_ = -1;
            state_ = State::OrderSelected;
            AppendEvent(L"Следующий заказ уже выбран: " + orders_[selectedOrderIndex_].id + L".");
        }
        else
        {
            selectedOrderIndex_ = -1;
            state_ = State::ReadyForOrders;
        }

        return true;
    }

    DriverApp::State DriverApp::GetState() const
    {
        return state_;
    }

    std::wstring DriverApp::StateToString(State state)
    {
        switch (state)
        {
        case State::AwaitingRegistration:
            return L"Ожидание регистрации";
        case State::ReadyForOrders:
            return L"Готов к приему заказов";
        case State::OrderSelected:
            return L"Заказ выбран";
        case State::Driving:
            return L"Ведение машины";
        case State::ShiftEnded:
            return L"Смена завершена";
        }

        return L"Неизвестное состояние";
    }

    bool DriverApp::IsRegistered() const
    {
        return registered_;
    }

    bool DriverApp::AreOrdersVisible() const
    {
        return ordersVisible_;
    }

    bool DriverApp::CanReceiveOrders() const
    {
        if (!registered_ || state_ == State::ShiftEnded)
        {
            return false;
        }

        return state_ == State::ReadyForOrders || CanSelectNextOrder();
    }

    bool DriverApp::CanAcceptSelectedOrder() const
    {
        return state_ == State::OrderSelected && selectedOrderIndex_ != -1 && state_ != State::ShiftEnded;
    }

    bool DriverApp::CanManipulateMap() const
    {
        return (state_ == State::OrderSelected || state_ == State::Driving) && state_ != State::ShiftEnded;
    }

    bool DriverApp::CanChooseRouteVariant() const
    {
        return state_ == State::Driving && state_ != State::ShiftEnded;
    }

    bool DriverApp::CanSelectNextOrder() const
    {
        return state_ == State::Driving && remainingRouteMinutes_ > 0 && remainingRouteMinutes_ <= 10;
    }

    bool DriverApp::IsShiftEnded() const
    {
        return state_ == State::ShiftEnded;
    }

    int DriverApp::GetShiftMinutes() const
    {
        return shiftMinutes_;
    }

    int DriverApp::GetRemainingRouteMinutes() const
    {
        return remainingRouteMinutes_;
    }

    int DriverApp::GetSelectedOrderIndex() const
    {
        return selectedOrderIndex_;
    }

    int DriverApp::GetCurrentOrderIndex() const
    {
        return currentOrderIndex_;
    }

    int DriverApp::GetReservedOrderIndex() const
    {
        return reservedOrderIndex_;
    }

    const std::wstring& DriverApp::GetRouteVariant() const
    {
        return routeVariant_;
    }

    const std::vector<DriverOrder>& DriverApp::GetOrders() const
    {
        return orders_;
    }

    const std::vector<DriverOrderStatus>& DriverApp::GetOrderStatuses() const
    {
        return orderStatuses_;
    }

    const std::vector<std::wstring>& DriverApp::GetEvents() const
    {
        return events_;
    }

    std::wstring DriverApp::GetStateSummary() const
    {
        std::wstring summary = L"Состояние: " + StateToString(state_);
        summary += L" | Прием заказов: ";
        summary += CanReceiveOrders() ? L"доступен" : L"заблокирован";
        summary += L" | Масштаб/панорама: ";
        summary += CanManipulateMap() ? L"доступны" : L"заблокированы";
        summary += L" | Вариант пути: ";
        summary += CanChooseRouteVariant() ? L"доступен" : L"заблокирован";
        return summary;
    }

    std::wstring DriverApp::GetSelectedOrderSummary() const
    {
        if (currentOrderIndex_ != -1)
        {
            return L"Текущий заказ: " + FormatOrder(orders_[currentOrderIndex_]) +
                L" | Осталось " + std::to_wstring(remainingRouteMinutes_) + L" мин.";
        }

        if (selectedOrderIndex_ != -1)
        {
            return L"Выбран заказ: " + FormatOrder(orders_[selectedOrderIndex_]);
        }

        if (reservedOrderIndex_ != -1)
        {
            return L"Следующий заказ: " + FormatOrder(orders_[reservedOrderIndex_]);
        }

        return L"Заказ пока не выбран.";
    }

    bool DriverApp::IsValidName(const std::wstring& value)
    {
        if (value.size() < 5 || value.find(L' ') == std::wstring::npos)
        {
            return false;
        }

        return std::any_of(value.begin(), value.end(), [](wchar_t ch) {
            return std::iswalpha(ch) != 0;
            });
    }

    bool DriverApp::IsValidDriverId(const std::wstring& value)
    {
        static const std::wregex pattern(LR"(^TX-\d{3}$)");
        return std::regex_match(value, pattern);
    }

    std::vector<DriverOrder> DriverApp::CreateOrders()
    {
        return {
            { L"ORD-201", L"North Gate", L"City Center", 30, 120, 70 },
            { L"ORD-202", L"River Park", L"Central Station", 25, 280, 150 },
            { L"ORD-203", L"University", L"Airport", 35, 420, 95 }
        };
    }

    std::wstring DriverApp::FormatOrder(const DriverOrder& order)
    {
        return order.id + L" (" + order.pickup + L" -> " + order.destination + L")";
    }

    bool DriverApp::HasAvailableOrders() const
    {
        return std::any_of(orderStatuses_.begin(), orderStatuses_.end(), [](DriverOrderStatus status) {
            return status == DriverOrderStatus::Available || status == DriverOrderStatus::Reserved;
            });
    }

    void DriverApp::AppendEvent(std::wstring message)
    {
        events_.push_back(std::move(message));
    }

    void DriverApp::ForceShiftEnd()
    {
        shiftMinutes_ = kShiftLimitMinutes;
        state_ = State::ShiftEnded;
        selectedOrderIndex_ = -1;
        reservedOrderIndex_ = -1;
        routeVariant_.clear();
        remainingRouteMinutes_ = 0;
        AppendEvent(L"Смена достигла 14 часов. Приложение принудительно завершило работу.");
    }

    void DriverApp::OpenNearFinishWindow()
    {
        nearFinishWindowOpened_ = true;
        AppendEvent(L"До окончания маршрута осталось 10 минут. Прием заказов снова открыт.");
    }

    void RunPractice8()
    {
        std::cout << "\n=== Practice 8: Taxi driver app ===\n";

        DriverApp app;
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

        check(!app.CanReceiveOrders(), "orders are blocked before registration");
        check(!app.CanManipulateMap(), "zoom and pan are blocked at start");
        check(app.RegisterDriver(L"Ivan Petrov", L"TX-101"), "driver registration succeeds");
        check(app.CanReceiveOrders(), "orders are unlocked after registration");
        check(app.ShowIncomingOrders(), "incoming orders are displayed");
        check(app.SelectOrder(0), "driver can select one active order");
        check(app.CanManipulateMap(), "zoom and pan unlock after order selection");
        check(app.AcceptSelectedOrder(), "driver can confirm the selected order");
        check(app.CanChooseRouteVariant(), "route selection unlocks in driving mode");
        check(app.ChooseRouteVariant(L"Shortest route"), "route variant can be chosen");

        const int minutesToNearFinish = std::max(0, app.GetRemainingRouteMinutes() - 10);
        check(app.AdvanceRoute(minutesToNearFinish), "route can be progressed to the last 10 minutes");
        check(app.CanSelectNextOrder(), "next order can be selected 10 minutes before finish");
        check(app.SelectOrder(1), "next order can be reserved near the end of route");

        const int minutesToShiftEnd = (14 * 60) - app.GetShiftMinutes();
        check(app.AdvanceShift(minutesToShiftEnd), "shift time can reach the 14 hour limit");
        check(app.IsShiftEnded(), "app stops after 14 hours");
        check(!app.CanReceiveOrders(), "orders are blocked after forced shutdown");

        std::cout << "Passed " << passed << " of " << total << " checks.\n";
    }
}
