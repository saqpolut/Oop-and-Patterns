#pragma once

#include "corporation.h"

#include <map> //корпорации на планете
#include <memory>
#include <string>

//класс для планет и астероидов
class CelestialBody{
protected:
    int id_;
    std::string name_;
    double x_;
    double y_;
    Inventory deposits_; //запас ресурсов

public:
    CelestialBody(
        int id,
        const std::string& name,
        double x,
        double y,
        const Inventory& deposits
    );

    virtual ~CelestialBody() = default;

    int id() const;
    const std::string& name() const;
    double x() const;
    double y() const;
    const Inventory& deposits() const;

    virtual std::string kind() const = 0; //вид объекта: планета или астероид
    virtual int goldIncome() const = 0; //доход от залота на планете
};

class Planets : public CelestialBody{
private:
    int goldIncome_;
    std::map<CorporationType, std::unique_ptr<Corporation>> corporations_; //корпорации на планете

public:
    Planets(
        int id,
        const std::string& name,
        double x,
        double y,
        const Inventory& deposits,
        int goldIncome
    );

    std::string kind() const override;
    int goldIncome() const override;

    bool hasCorporation(CorporationType type) const;

    //возвращает указатель на корпорацию нужного типа
    Corporation* corporation(CorporationType type);
    const Corporation* corporation(CorporationType type) const;

    //добавить корпорацию на планету
    bool installCorporation(std::unique_ptr<Corporation> corporation);
};

class Asteroids final : public CelestialBody{
public:
    Asteroids(
        int id,
        const std::string& name,
        double x,
        double y,
        const Inventory& deposits
    );

    std::string kind() const override;
    int goldIncome() const override;
};
