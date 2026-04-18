# Практика 6. Композиция и ассоциации

Исходные файлы:

- `PracticHub/PracticeTaxi.h`
- `PracticHub/PracticeTaxi.cpp`

Этот листинг показывает связку поездки, маршрута, платёжного шлюза и базы оплат.

```cpp
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
};

class TaxiTrip
{
public:
    TaxiTrip(std::string orderId, Route route, double price);
    const std::string& GetOrderId() const;
    const Route& GetRoute() const;
    double GetPrice() const;
};

class TaxiServiceCoordinator
{
public:
    explicit TaxiServiceCoordinator(PaymentGateway& gateway);
    void CompleteTrip(const TaxiTrip& trip, const Client& client) const;

private:
    PaymentGateway& gateway_;
};

void PaymentDatabase::Save(const PaymentRecord& record)
{
    records_.push_back(record);
}

void PaymentGateway::ProcessPayment(const PaymentRecord& record) const
{
    database_.Save(record);
}

void TaxiServiceCoordinator::CompleteTrip(const TaxiTrip& trip, const Client& client) const
{
    gateway_.ProcessPayment({ trip.GetOrderId(), trip.GetPrice(), client.GetPaymentMethod() });
}

void RunPractice6()
{
    Client client("Sergey", "+79991234567", 5.0, "Corporate account");
    PaymentDatabase database;
    PaymentGateway gateway(database);
    TaxiServiceCoordinator coordinator(gateway);

    TaxiTrip trip("ORDER-1001", Route("Office", "Airport"), 1350.0);
    coordinator.CompleteTrip(trip, client);

    std::cout << "Trip route: " << trip.GetRoute().Describe() << "\n";
    std::cout << "Saved payments: " << database.Records().size() << "\n";
}
```
