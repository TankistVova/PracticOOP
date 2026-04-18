#include "pch.h"
#include "PracticeTaxi.h"

namespace practice
{
    Participant::Participant(std::string name, std::string phone)
        : name_(std::move(name)), phone_(std::move(phone))
    {
    }

    const std::string& Participant::GetName() const
    {
        return name_;
    }

    const std::string& Participant::GetPhone() const
    {
        return phone_;
    }

    Person::Person(std::string name, std::string phone, double rating)
        : Participant(std::move(name), std::move(phone)), rating_(rating)
    {
    }

    double Person::GetRating() const
    {
        return rating_;
    }

    Client::Client(std::string name, std::string phone, double rating, std::string paymentMethod)
        : Person(std::move(name), std::move(phone), rating),
          paymentMethod_(std::move(paymentMethod)),
          lastServiceScore_(0)
    {
    }

    std::string Client::DescribeRole() const
    {
        return "Client";
    }

    std::string Client::DescribeProfile() const
    {
        std::ostringstream stream;
        stream << "Client " << name_ << ", rating " << rating_
            << ", payment " << paymentMethod_;
        if (lastServiceScore_ > 0)
        {
            stream << ", last review " << lastServiceScore_ << "/5";
        }

        return stream.str();
    }

    void Client::ChoosePaymentMethod(std::string paymentMethod)
    {
        paymentMethod_ = std::move(paymentMethod);
    }

    bool Client::RateService(int score, std::string comment)
    {
        if (score < 1 || score > 5)
        {
            return false;
        }

        lastServiceScore_ = score;
        lastServiceComment_ = std::move(comment);
        return true;
    }

    const std::string& Client::GetPaymentMethod() const
    {
        return paymentMethod_;
    }

    int Client::GetLastServiceScore() const
    {
        return lastServiceScore_;
    }

    const std::string& Client::GetLastServiceComment() const
    {
        return lastServiceComment_;
    }

    Driver::Driver(std::string name, std::string phone, double rating, std::string licenseNumber)
        : Person(std::move(name), std::move(phone), rating), licenseNumber_(std::move(licenseNumber))
    {
    }

    std::string Driver::DescribeRole() const
    {
        return "Driver";
    }

    std::string Driver::DescribeProfile() const
    {
        std::ostringstream stream;
        stream << "Driver " << name_ << ", rating " << rating_
            << ", license " << licenseNumber_;
        if (!currentOrderId_.empty())
        {
            stream << ", current order " << currentOrderId_;
        }

        return stream.str();
    }

    bool Driver::AcceptOrder(std::string orderId)
    {
        if (!currentOrderId_.empty() || orderId.empty())
        {
            return false;
        }

        currentOrderId_ = std::move(orderId);
        return true;
    }

    bool Driver::CancelCurrentOrder()
    {
        if (currentOrderId_.empty())
        {
            return false;
        }

        currentOrderId_.clear();
        return true;
    }

    const std::string& Driver::GetLicenseNumber() const
    {
        return licenseNumber_;
    }

    const std::string& Driver::GetCurrentOrderId() const
    {
        return currentOrderId_;
    }

    TaxiCar::TaxiCar(std::string model, std::string plateNumber)
        : model_(std::move(model)), plateNumber_(std::move(plateNumber))
    {
    }

    const std::string& TaxiCar::GetModel() const
    {
        return model_;
    }

    const std::string& TaxiCar::GetPlateNumber() const
    {
        return plateNumber_;
    }

    std::string TaxiCar::DescribeCar() const
    {
        std::ostringstream stream;
        stream << ServiceLevel() << " car " << model_
            << " [" << plateNumber_ << "], capacity " << Capacity()
            << ", cashless " << (SupportsCashlessPayment() ? "yes" : "no");
        return stream.str();
    }

    EconomyCar::EconomyCar(std::string model, std::string plateNumber)
        : TaxiCar(std::move(model), std::move(plateNumber))
    {
    }

    std::string EconomyCar::ServiceLevel() const
    {
        return "Economy";
    }

    int EconomyCar::Capacity() const
    {
        return 4;
    }

    bool EconomyCar::SupportsCashlessPayment() const
    {
        return true;
    }

    ComfortCar::ComfortCar(std::string model, std::string plateNumber)
        : TaxiCar(std::move(model), std::move(plateNumber))
    {
    }

    std::string ComfortCar::ServiceLevel() const
    {
        return "Comfort";
    }

    int ComfortCar::Capacity() const
    {
        return 4;
    }

    bool ComfortCar::SupportsCashlessPayment() const
    {
        return true;
    }

    OrderService::OrderService(std::string route, double basePrice)
        : route_(std::move(route)),
          basePrice_(basePrice),
          qualityScore_(0),
          status_(Status::Draft)
    {
    }

    bool OrderService::SelectRoute(std::string route)
    {
        if (route.empty() || status_ == Status::Completed)
        {
            return false;
        }

        route_ = std::move(route);
        status_ = Status::RouteSelected;
        return true;
    }

    bool OrderService::CancelRoute()
    {
        if (status_ != Status::RouteSelected)
        {
            return false;
        }

        route_.clear();
        status_ = Status::RouteCancelled;
        return true;
    }

    void OrderService::SetPaymentMethod(std::string paymentMethod)
    {
        paymentMethod_ = std::move(paymentMethod);
    }

    bool OrderService::CompleteOrder()
    {
        if (status_ != Status::RouteSelected || paymentMethod_.empty())
        {
            return false;
        }

        status_ = Status::Completed;
        return true;
    }

    bool OrderService::RateQuality(int score, std::string comment)
    {
        if (status_ != Status::Completed || score < 1 || score > 5)
        {
            return false;
        }

        qualityScore_ = score;
        qualityComment_ = std::move(comment);
        return true;
    }

    double OrderService::GetBasePrice() const
    {
        return basePrice_;
    }

    const std::string& OrderService::GetRoute() const
    {
        return route_;
    }

    const std::string& OrderService::GetPaymentMethod() const
    {
        return paymentMethod_;
    }

    int OrderService::GetQualityScore() const
    {
        return qualityScore_;
    }

    const std::string& OrderService::GetQualityComment() const
    {
        return qualityComment_;
    }

    OrderService::Status OrderService::GetStatus() const
    {
        return status_;
    }

    std::string OrderService::StatusToString(Status status)
    {
        switch (status)
        {
        case Status::Draft:
            return "Draft";
        case Status::RouteSelected:
            return "Route selected";
        case Status::RouteCancelled:
            return "Route cancelled";
        case Status::Completed:
            return "Completed";
        }

        return "Unknown";
    }

    std::string OrderService::DescribeOrderState() const
    {
        std::ostringstream stream;
        stream << "route: " << (route_.empty() ? "not selected" : route_)
            << ", payment: " << (paymentMethod_.empty() ? "not selected" : paymentMethod_)
            << ", status: " << StatusToString(status_);
        if (qualityScore_ > 0)
        {
            stream << ", quality: " << qualityScore_ << "/5";
        }

        return stream.str();
    }

    StandardOrder::StandardOrder(std::string route, double basePrice)
        : OrderService(std::move(route), basePrice)
    {
    }

    std::string StandardOrder::DescribeService() const
    {
        return "Standard order: " + route_;
    }

    std::string StandardOrder::OrderPolicy() const
    {
        return "Base tariff, standard dispatching, no extra priority.";
    }

    PremiumOrder::PremiumOrder(std::string route, double basePrice, bool childSeat)
        : OrderService(std::move(route), basePrice), childSeat_(childSeat)
    {
    }

    std::string PremiumOrder::DescribeService() const
    {
        return std::string("Premium order: ") + route_ + (childSeat_ ? " with child seat" : "");
    }

    std::string PremiumOrder::OrderPolicy() const
    {
        return childSeat_
            ? "Priority dispatching, comfort support and child seat."
            : "Priority dispatching with increased service level.";
    }

    std::string PaymentMethodToString(PaymentMethod method)
    {
        switch (method)
        {
        case PaymentMethod::Cash:
            return "Cash";
        case PaymentMethod::Card:
            return "Card";
        case PaymentMethod::CorporateAccount:
            return "Corporate account";
        case PaymentMethod::FastPayment:
            return "Fast payment";
        }

        return "Unknown";
    }

    RouteOption::RouteOption(std::string name,
                             std::string from,
                             std::string to,
                             int estimatedMinutes,
                             double distanceKm,
                             bool tollRoad)
        : name_(std::move(name)),
          from_(std::move(from)),
          to_(std::move(to)),
          estimatedMinutes_(estimatedMinutes),
          distanceKm_(distanceKm),
          tollRoad_(tollRoad)
    {
    }

    std::string RouteOption::Describe() const
    {
        std::ostringstream stream;
        stream << name_ << ": " << from_ << " -> " << to_
            << ", " << estimatedMinutes_ << " min, "
            << std::fixed << std::setprecision(1) << distanceKm_ << " km";
        if (tollRoad_)
        {
            stream << ", toll road";
        }

        return stream.str();
    }

    int RouteOption::GetEstimatedMinutes() const
    {
        return estimatedMinutes_;
    }

    AggregationTaxiOrder::AggregationTaxiOrder(std::string orderId, const Client& client, const OrderService& service)
        : orderId_(std::move(orderId)),
          client_(&client),
          service_(&service),
          driver_(nullptr),
          car_(nullptr),
          selectedRoute_(nullptr),
          paymentMethod_(PaymentMethod::Cash),
          state_(State::Created)
    {
    }

    void AggregationTaxiOrder::AttachExecutor(const Driver& driver, const TaxiCar& car)
    {
        driver_ = &driver;
        car_ = &car;
    }

    void AggregationTaxiOrder::AddRouteOption(const RouteOption& routeOption)
    {
        routeOptions_.push_back(&routeOption);
    }

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

    bool AggregationTaxiOrder::CancelRoute()
    {
        if (selectedRoute_ == nullptr || state_ == State::Completed)
        {
            return false;
        }

        selectedRoute_ = nullptr;
        state_ = State::RouteCancelled;
        return true;
    }

    void AggregationTaxiOrder::SetPaymentMethod(PaymentMethod method)
    {
        paymentMethod_ = method;
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

    std::string AggregationTaxiOrder::Summary() const
    {
        std::ostringstream stream;
        stream << "Order " << orderId_
            << " | client: " << client_->GetName()
            << " | service: " << service_->DescribeService()
            << " | payment: " << PaymentMethodToString(paymentMethod_)
            << " | state: " << StateToString(state_);

        if (driver_ != nullptr && car_ != nullptr)
        {
            stream << " | driver: " << driver_->GetName()
                << " | car: " << car_->GetModel() << " (" << car_->ServiceLevel() << ")";
        }

        if (selectedRoute_ != nullptr)
        {
            stream << " | route: " << selectedRoute_->Describe();
        }

        if (review_.has_value())
        {
            stream << " | review: " << review_->score << "/5 - " << review_->comment;
        }

        return stream.str();
    }

    std::size_t AggregationTaxiOrder::RouteCount() const
    {
        return routeOptions_.size();
    }

    bool AggregationTaxiOrder::HasSelectedRoute() const
    {
        return selectedRoute_ != nullptr;
    }

    AggregationTaxiOrder::State AggregationTaxiOrder::GetState() const
    {
        return state_;
    }

    std::string AggregationTaxiOrder::StateToString(State state)
    {
        switch (state)
        {
        case State::Created:
            return "Created";
        case State::RouteSelected:
            return "Route selected";
        case State::RouteCancelled:
            return "Route cancelled";
        case State::Completed:
            return "Completed";
        }

        return "Unknown";
    }

    void DispatchCenter::RegisterClient(const Client& client)
    {
        clients_.push_back(&client);
    }

    void DispatchCenter::RegisterDriver(const Driver& driver)
    {
        drivers_.push_back(&driver);
    }

    void DispatchCenter::RegisterCar(const TaxiCar& car)
    {
        cars_.push_back(&car);
    }

    void DispatchCenter::RegisterOrder(const AggregationTaxiOrder& order)
    {
        orders_.push_back(&order);
    }

    std::size_t DispatchCenter::ClientCount() const
    {
        return clients_.size();
    }

    std::size_t DispatchCenter::DriverCount() const
    {
        return drivers_.size();
    }

    std::size_t DispatchCenter::CarCount() const
    {
        return cars_.size();
    }

    std::size_t DispatchCenter::OrderCount() const
    {
        return orders_.size();
    }

    std::string DispatchCenter::Summary() const
    {
        std::ostringstream stream;
        stream << "Dispatch center aggregates "
            << clients_.size() << " clients, "
            << drivers_.size() << " drivers, "
            << cars_.size() << " cars and "
            << orders_.size() << " orders";
        return stream.str();
    }

    void PaymentDatabase::Save(const PaymentRecord& record)
    {
        records_.push_back(record);
    }

    const std::vector<PaymentRecord>& PaymentDatabase::Records() const
    {
        return records_;
    }

    PaymentGateway::PaymentGateway(PaymentDatabase& database)
        : database_(database)
    {
    }

    void PaymentGateway::ProcessPayment(const PaymentRecord& record) const
    {
        database_.Save(record);
    }

    Route::Route(std::string from, std::string to)
        : from_(std::move(from)), to_(std::move(to))
    {
    }

    std::string Route::Describe() const
    {
        return from_ + " -> " + to_;
    }

    TaxiTrip::TaxiTrip(std::string orderId, Route route, double price)
        : orderId_(std::move(orderId)), route_(std::move(route)), price_(price)
    {
    }

    const std::string& TaxiTrip::GetOrderId() const
    {
        return orderId_;
    }

    const Route& TaxiTrip::GetRoute() const
    {
        return route_;
    }

    double TaxiTrip::GetPrice() const
    {
        return price_;
    }

    TaxiServiceCoordinator::TaxiServiceCoordinator(PaymentGateway& gateway)
        : gateway_(gateway)
    {
    }

    void TaxiServiceCoordinator::CompleteTrip(const TaxiTrip& trip, const Client& client) const
    {
        gateway_.ProcessPayment({ trip.GetOrderId(), trip.GetPrice(), client.GetPaymentMethod() });
    }

    void RunPractice4()
    {
        std::cout << "\n=== Practice 4: Inheritance ===\n";

        Client client("Anna", "+79990001122", 4.9, "Cash");
        Driver driver("Oleg", "+79995554433", 4.8, "LIC-42");
        ComfortCar comfortCar("Skoda Octavia", "A123BC77");
        EconomyCar economyCar("Kia Rio", "B456DE77");
        PremiumOrder premiumOrder("Airport -> Center", 890.0, true);
        StandardOrder standardOrder("Office -> Station", 540.0);

        std::vector<Participant*> participants = { &client, &driver };
        std::vector<TaxiCar*> cars = { &economyCar, &comfortCar };
        std::vector<OrderService*> orders = { &standardOrder, &premiumOrder };

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

        client.ChoosePaymentMethod("Card");
        check(client.GetPaymentMethod() == "Card", "client can choose payment method");
        check(driver.AcceptOrder("ORD-4001"), "driver can accept an order");
        check(!driver.GetCurrentOrderId().empty(), "driver stores current order id");

        check(premiumOrder.SelectRoute("Airport -> Ring Road -> Center"), "order can select a route");
        check(premiumOrder.CancelRoute(), "selected route can be cancelled");
        check(standardOrder.SelectRoute("Office -> Metro -> Station"), "another order can select route");
        standardOrder.SetPaymentMethod(client.GetPaymentMethod());
        check(standardOrder.CompleteOrder(), "order can be completed after selecting route and payment");
        check(standardOrder.RateQuality(5, "Everything was on time"), "completed order can be rated");
        check(client.RateService(5, "Comfortable trip"), "client can rate quality of service");
        check(driver.CancelCurrentOrder(), "driver can cancel current order");

        std::cout << "Participants:\n";
        for (const Participant* participant : participants)
        {
            std::cout << " - " << participant->DescribeRole() << " | " << participant->DescribeProfile() << "\n";
        }

        std::cout << "Cars:\n";
        for (const TaxiCar* car : cars)
        {
            std::cout << " - " << car->DescribeCar() << "\n";
        }

        std::cout << "Orders:\n";
        for (const OrderService* order : orders)
        {
            std::cout << " - " << order->DescribeService()
                << " | " << order->OrderPolicy()
                << " | " << order->DescribeOrderState() << "\n";
        }

        std::cout << "Passed " << passed << " of " << total << " checks.\n";
    }

    void RunPractice5()
    {
        std::cout << "\n=== Practice 5: Aggregation ===\n";

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

        check(dispatch.ClientCount() == 1, "dispatch center aggregates clients");
        check(dispatch.DriverCount() == 1, "dispatch center aggregates drivers");
        check(dispatch.CarCount() == 1, "dispatch center aggregates cars");
        check(dispatch.OrderCount() == 1, "dispatch center aggregates orders");
        check(order.RouteCount() == 2, "order aggregates available routes");
        check(order.SelectRoute(0), "client can choose a route");
        check(order.HasSelectedRoute(), "selected route is stored in order");
        check(order.CancelRoute(), "selected route can be cancelled");
        check(!order.HasSelectedRoute(), "route is cleared after cancellation");
        check(order.SelectRoute(1), "client can choose another route after cancellation");
        check(order.CompleteWithReview(5, "Driver arrived on time and ride was comfortable"), "service can be rated after trip");
        check(order.GetState() == AggregationTaxiOrder::State::Completed, "order state switches to completed");

        std::cout << dispatch.Summary() << "\n";
        std::cout << order.Summary() << "\n";
        std::cout << "Passed " << passed << " of " << total << " checks.\n";
    }

    void RunPractice6()
    {
        std::cout << "\n=== Practice 6: Composition and association ===\n";

        Client client("Sergey", "+79991234567", 5.0, "Corporate account");
        PaymentDatabase database;
        PaymentGateway gateway(database);
        TaxiServiceCoordinator coordinator(gateway);

        TaxiTrip trip("ORDER-1001", Route("Office", "Airport"), 1350.0);
        coordinator.CompleteTrip(trip, client);

        std::cout << "Trip route: " << trip.GetRoute().Describe() << "\n";
        std::cout << "Saved payments: " << database.Records().size() << "\n";

        if (!database.Records().empty())
        {
            const PaymentRecord& record = database.Records().front();
            std::cout << "Payment record -> id: " << record.orderId
                << ", amount: " << record.amount
                << ", method: " << record.paymentMethod << "\n";
        }
    }
}
