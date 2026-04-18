# Практика 4. Наследование

Исходные файлы:

- `PracticHub/PracticeTaxi.h`
- `PracticHub/PracticeTaxi.cpp`

Ключевой листинг показывает иерархию классов участников сервиса, автомобилей и заказов.

```cpp
class Participant
{
public:
    Participant(std::string name, std::string phone);
    virtual ~Participant() = default;
    virtual std::string DescribeRole() const = 0;
    virtual std::string DescribeProfile() const = 0;
};

class Person : public Participant
{
public:
    Person(std::string name, std::string phone, double rating);
    double GetRating() const;
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
};

class Driver : public Person
{
public:
    Driver(std::string name, std::string phone, double rating, std::string licenseNumber);
    std::string DescribeRole() const override;
    std::string DescribeProfile() const override;
    bool AcceptOrder(std::string orderId);
    bool CancelCurrentOrder();
};

class TaxiCar
{
public:
    TaxiCar(std::string model, std::string plateNumber);
    virtual ~TaxiCar() = default;
    virtual std::string ServiceLevel() const = 0;
    virtual int Capacity() const = 0;
    virtual bool SupportsCashlessPayment() const = 0;
    std::string DescribeCar() const;
};

class EconomyCar : public TaxiCar
{
public:
    using TaxiCar::TaxiCar;
    std::string ServiceLevel() const override { return "Economy"; }
    int Capacity() const override { return 4; }
    bool SupportsCashlessPayment() const override { return true; }
};

class ComfortCar : public TaxiCar
{
public:
    using TaxiCar::TaxiCar;
    std::string ServiceLevel() const override { return "Comfort"; }
    int Capacity() const override { return 4; }
    bool SupportsCashlessPayment() const override { return true; }
};

class OrderService
{
public:
    virtual ~OrderService() = default;
    virtual std::string DescribeService() const = 0;
    virtual std::string OrderPolicy() const = 0;

    bool SelectRoute(std::string route);
    bool CancelRoute();
    void SetPaymentMethod(std::string paymentMethod);
    bool CompleteOrder();
    bool RateQuality(int score, std::string comment);
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
};

void RunPractice4()
{
    Client client("Anna", "+79990001122", 4.9, "Cash");
    Driver driver("Oleg", "+79995554433", 4.8, "LIC-42");
    ComfortCar comfortCar("Skoda Octavia", "A123BC77");
    EconomyCar economyCar("Kia Rio", "B456DE77");
    PremiumOrder premiumOrder("Airport -> Center", 890.0, true);
    StandardOrder standardOrder("Office -> Station", 540.0);

    std::vector<Participant*> participants = { &client, &driver };
    std::vector<TaxiCar*> cars = { &economyCar, &comfortCar };
    std::vector<OrderService*> orders = { &standardOrder, &premiumOrder };

    client.ChoosePaymentMethod("Card");
    check(client.GetPaymentMethod() == "Card", "client can choose payment method");
    check(driver.AcceptOrder("ORD-4001"), "driver can accept an order");

    check(premiumOrder.SelectRoute("Airport -> Ring Road -> Center"),
          "order can select a route");
    check(premiumOrder.CancelRoute(), "selected route can be cancelled");
    check(standardOrder.SelectRoute("Office -> Metro -> Station"),
          "another order can select route");
    standardOrder.SetPaymentMethod(client.GetPaymentMethod());
    check(standardOrder.CompleteOrder(),
          "order can be completed after selecting route and payment");
}
```
