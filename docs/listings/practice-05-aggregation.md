# Практика 5. Агрегация

Исходные файлы:

- `PracticHub/PracticeTaxi.h`
- `PracticHub/PracticeTaxi.cpp`

Листинг показывает агрегирование клиента, водителя, автомобиля, маршрутов и заказа в диспетчерской системе.

```cpp
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
};

class AggregationTaxiOrder
{
public:
    AggregationTaxiOrder(std::string orderId, const Client& client, const OrderService& service);
    void AttachExecutor(const Driver& driver, const TaxiCar& car);
    void AddRouteOption(const RouteOption& routeOption);
    bool SelectRoute(std::size_t routeIndex);
    bool CancelRoute();
    void SetPaymentMethod(PaymentMethod method);
    bool CompleteWithReview(int score, std::string comment);
    std::size_t RouteCount() const;
    std::string Summary() const;
};

class DispatchCenter
{
public:
    void RegisterClient(const Client& client);
    void RegisterDriver(const Driver& driver);
    void RegisterCar(const TaxiCar& car);
    void RegisterOrder(const AggregationTaxiOrder& order);
    std::string Summary() const;
};

bool AggregationTaxiOrder::SelectRoute(std::size_t routeIndex)
{
    if (routeIndex >= routeOptions_.size() || state_ == State::Completed)
    {
        return false;
    }

    selectedRoute_ = routeOptions_[routeIndex];
    state_ = State::RouteSelected;
    return true;
}

bool AggregationTaxiOrder::CompleteWithReview(int score, std::string comment)
{
    if (driver_ == nullptr || car_ == nullptr || selectedRoute_ == nullptr || state_ == State::Completed)
    {
        return false;
    }

    if (score < 1 || score > 5)
    {
        return false;
    }

    review_ = ServiceReview{ score, std::move(comment) };
    state_ = State::Completed;
    return true;
}

void RunPractice5()
{
    Client client("Ivan", "+79997770011", 4.7, "Card");
    Driver driver("Maria", "+79993332211", 4.95, "DRV-77");
    ComfortCar car("Skoda Octavia", "B777CC77");
    PremiumOrder orderService("Station -> University", 960.0, true);
    RouteOption fastRoute("Fastest route", "Station", "University", 18, 9.4, false);
    RouteOption bypassRoute("Traffic bypass", "Station", "University", 24, 11.8, true);

    DispatchCenter dispatch;
    dispatch.RegisterClient(client);
    dispatch.RegisterDriver(driver);
    dispatch.RegisterCar(car);

    AggregationTaxiOrder order("ORD-5001", client, orderService);
    order.AttachExecutor(driver, car);
    order.AddRouteOption(fastRoute);
    order.AddRouteOption(bypassRoute);
    order.SetPaymentMethod(PaymentMethod::Card);
    dispatch.RegisterOrder(order);

    check(order.RouteCount() == 2, "order aggregates available routes");
    check(order.SelectRoute(0), "client can choose a route");
    check(order.CancelRoute(), "selected route can be cancelled");
    check(order.SelectRoute(1), "client can choose another route after cancellation");
    check(order.CompleteWithReview(5, "Driver arrived on time and ride was comfortable"),
          "service can be rated after trip");
}
```
