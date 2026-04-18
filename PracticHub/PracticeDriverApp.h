#pragma once

#include "pch.h"

namespace practice
{
    struct DriverOrder
    {
        std::wstring id;
        std::wstring pickup;
        std::wstring destination;
        int routeMinutes;
        int mapX;
        int mapY;
    };

    enum class DriverOrderStatus
    {
        Available,
        Current,
        Reserved,
        Completed
    };

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

        State GetState() const;
        static std::wstring StateToString(State state);

        bool IsRegistered() const;
        bool AreOrdersVisible() const;
        bool CanReceiveOrders() const;
        bool CanAcceptSelectedOrder() const;
        bool CanManipulateMap() const;
        bool CanChooseRouteVariant() const;
        bool CanSelectNextOrder() const;
        bool IsShiftEnded() const;

        int GetShiftMinutes() const;
        int GetRemainingRouteMinutes() const;
        int GetSelectedOrderIndex() const;
        int GetCurrentOrderIndex() const;
        int GetReservedOrderIndex() const;
        const std::wstring& GetRouteVariant() const;

        const std::vector<DriverOrder>& GetOrders() const;
        const std::vector<DriverOrderStatus>& GetOrderStatuses() const;
        const std::vector<std::wstring>& GetEvents() const;

        std::wstring GetStateSummary() const;
        std::wstring GetSelectedOrderSummary() const;

    private:
        static bool IsValidName(const std::wstring& value);
        static bool IsValidDriverId(const std::wstring& value);
        static std::vector<DriverOrder> CreateOrders();
        static std::wstring FormatOrder(const DriverOrder& order);

        bool HasAvailableOrders() const;
        void AppendEvent(std::wstring message);
        void ForceShiftEnd();
        void OpenNearFinishWindow();

        State state_;
        bool registered_;
        bool ordersVisible_;
        bool nearFinishWindowOpened_;
        int shiftMinutes_;
        int remainingRouteMinutes_;
        int selectedOrderIndex_;
        int currentOrderIndex_;
        int reservedOrderIndex_;
        std::wstring routeVariant_;
        std::wstring driverName_;
        std::wstring driverId_;
        std::vector<DriverOrder> orders_;
        std::vector<DriverOrderStatus> orderStatuses_;
        std::vector<std::wstring> events_;
    };

    void RunPractice8();
}
