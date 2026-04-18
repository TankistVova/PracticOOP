#pragma once

#include "pch.h"

namespace practice
{
    class Participant
    {
    public:
        Participant(std::string name, std::string phone);
        virtual ~Participant() = default;
        virtual std::string DescribeRole() const = 0;
        virtual std::string DescribeProfile() const = 0;
        const std::string& GetName() const;
        const std::string& GetPhone() const;

    protected:
        std::string name_;
        std::string phone_;
    };

    class Person : public Participant
    {
    public:
        Person(std::string name, std::string phone, double rating);
        double GetRating() const;

    protected:
        double rating_;
    };

    class Client : public Person
    {
    public:
        Client(std::string name, std::string phone, double rating, std::string paymentMethod);
        std::string DescribeRole() const override;
        std::string DescribeProfile() const override;
        void ChoosePaymentMethod(std::string paymentMethod);
        bool RateService(int score, std::string comment);
        const std::string& GetPaymentMethod() const;
        int GetLastServiceScore() const;
        const std::string& GetLastServiceComment() const;

    private:
        std::string paymentMethod_;
        int lastServiceScore_;
        std::string lastServiceComment_;
    };

    class Driver : public Person
    {
    public:
        Driver(std::string name, std::string phone, double rating, std::string licenseNumber);
        std::string DescribeRole() const override;
        std::string DescribeProfile() const override;
        bool AcceptOrder(std::string orderId);
        bool CancelCurrentOrder();
        const std::string& GetLicenseNumber() const;
        const std::string& GetCurrentOrderId() const;

    private:
        std::string licenseNumber_;
        std::string currentOrderId_;
    };

    class TaxiCar
    {
    public:
        TaxiCar(std::string model, std::string plateNumber);
        virtual ~TaxiCar() = default;
        virtual std::string ServiceLevel() const = 0;
        virtual int Capacity() const = 0;
        virtual bool SupportsCashlessPayment() const = 0;
        const std::string& GetModel() const;
        const std::string& GetPlateNumber() const;
        std::string DescribeCar() const;

    protected:
        std::string model_;
        std::string plateNumber_;
    };

    class EconomyCar : public TaxiCar
    {
    public:
        EconomyCar(std::string model, std::string plateNumber);
        std::string ServiceLevel() const override;
        int Capacity() const override;
        bool SupportsCashlessPayment() const override;
    };

    class ComfortCar : public TaxiCar
    {
    public:
        ComfortCar(std::string model, std::string plateNumber);
        std::string ServiceLevel() const override;
        int Capacity() const override;
        bool SupportsCashlessPayment() const override;
    };

    class OrderService
    {
    public:
        enum class Status
        {
            Draft,
            RouteSelected,
            RouteCancelled,
            Completed
        };

        OrderService(std::string route, double basePrice);
        virtual ~OrderService() = default;
        virtual std::string DescribeService() const = 0;
        virtual std::string OrderPolicy() const = 0;
        bool SelectRoute(std::string route);
        bool CancelRoute();
        void SetPaymentMethod(std::string paymentMethod);
        bool CompleteOrder();
        bool RateQuality(int score, std::string comment);
        double GetBasePrice() const;
        const std::string& GetRoute() const;
        const std::string& GetPaymentMethod() const;
        int GetQualityScore() const;
        const std::string& GetQualityComment() const;
        Status GetStatus() const;
        static std::string StatusToString(Status status);
        std::string DescribeOrderState() const;

    protected:
        std::string route_;
        double basePrice_;
        std::string paymentMethod_;
        int qualityScore_;
        std::string qualityComment_;
        Status status_;
    };

    class StandardOrder : public OrderService
    {
    public:
        StandardOrder(std::string route, double basePrice);
        std::string DescribeService() const override;
        std::string OrderPolicy() const override;
    };

    class PremiumOrder : public OrderService
    {
    public:
        PremiumOrder(std::string route, double basePrice, bool childSeat);
        std::string DescribeService() const override;
        std::string OrderPolicy() const override;

    private:
        bool childSeat_;
    };

    enum class PaymentMethod
    {
        Cash,
        Card,
        CorporateAccount,
        FastPayment
    };

    std::string PaymentMethodToString(PaymentMethod method);

    class RouteOption
    {
    public:
        RouteOption(std::string name,
                    std::string from,
                    std::string to,
                    int estimatedMinutes,
                    double distanceKm,
                    bool tollRoad);

        std::string Describe() const;
        int GetEstimatedMinutes() const;

    private:
        std::string name_;
        std::string from_;
        std::string to_;
        int estimatedMinutes_;
        double distanceKm_;
        bool tollRoad_;
    };

    struct ServiceReview
    {
        int score;
        std::string comment;
    };

    class AggregationTaxiOrder
    {
    public:
        enum class State
        {
            Created,
            RouteSelected,
            RouteCancelled,
            Completed
        };

        AggregationTaxiOrder(std::string orderId, const Client& client, const OrderService& service);
        void AttachExecutor(const Driver& driver, const TaxiCar& car);
        void AddRouteOption(const RouteOption& routeOption);
        bool SelectRoute(std::size_t routeIndex);
        bool CancelRoute();
        void SetPaymentMethod(PaymentMethod method);
        bool CompleteWithReview(int score, std::string comment);
        std::string Summary() const;
        std::size_t RouteCount() const;
        bool HasSelectedRoute() const;
        State GetState() const;
        static std::string StateToString(State state);

    private:
        std::string orderId_;
        const Client* client_;
        const OrderService* service_;
        const Driver* driver_;
        const TaxiCar* car_;
        std::vector<const RouteOption*> routeOptions_;
        const RouteOption* selectedRoute_;
        PaymentMethod paymentMethod_;
        std::optional<ServiceReview> review_;
        State state_;
    };

    class DispatchCenter
    {
    public:
        void RegisterClient(const Client& client);
        void RegisterDriver(const Driver& driver);
        void RegisterCar(const TaxiCar& car);
        void RegisterOrder(const AggregationTaxiOrder& order);
        std::size_t ClientCount() const;
        std::size_t DriverCount() const;
        std::size_t CarCount() const;
        std::size_t OrderCount() const;
        std::string Summary() const;

    private:
        std::vector<const Client*> clients_;
        std::vector<const Driver*> drivers_;
        std::vector<const TaxiCar*> cars_;
        std::vector<const AggregationTaxiOrder*> orders_;
    };

    struct PaymentRecord
    {
        std::string orderId;
        double amount;
        std::string paymentMethod;
    };

    class PaymentDatabase
    {
    public:
        void Save(const PaymentRecord& record);
        const std::vector<PaymentRecord>& Records() const;

    private:
        std::vector<PaymentRecord> records_;
    };

    class PaymentGateway
    {
    public:
        explicit PaymentGateway(PaymentDatabase& database);
        void ProcessPayment(const PaymentRecord& record) const;

    private:
        PaymentDatabase& database_;
    };

    class Route
    {
    public:
        Route(std::string from, std::string to);
        std::string Describe() const;

    private:
        std::string from_;
        std::string to_;
    };

    class TaxiTrip
    {
    public:
        TaxiTrip(std::string orderId, Route route, double price);
        const std::string& GetOrderId() const;
        const Route& GetRoute() const;
        double GetPrice() const;

    private:
        std::string orderId_;
        Route route_;
        double price_;
    };

    class TaxiServiceCoordinator
    {
    public:
        explicit TaxiServiceCoordinator(PaymentGateway& gateway);
        void CompleteTrip(const TaxiTrip& trip, const Client& client) const;

    private:
        PaymentGateway& gateway_;
    };

    void RunPractice4();
    void RunPractice5();
    void RunPractice6();
}
