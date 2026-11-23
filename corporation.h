#pragma once

#include "resources.h"

#include <memory>
#include <string>

//типы корпораций
//Logistics - логистическая (торговые маршруты)
//Miner - добывающая
//Trader - торговая
enum class CorporationType{
    Logistics,
    Miner,
    Trader
};

class Corporation{
protected:
    int level_; //уровень корпорации

public:
    explicit Corporation(int level = 1);
    virtual ~Corporation() = default;

    int level() const; //возвращает текущий уровень
    void upgrade(); //повышает уровень на 1

    virtual CorporationType type() const = 0;
    virtual std::string name() const = 0;

    virtual int buildCostCredits() const = 0; //цена постройки
    virtual Inventory buildCostResources() const = 0; //ресурсы для построки
    virtual Inventory upgradeCostResources() const = 0; //ресурсы для улучшения

    //возвращает стоимость улучшение в у.е.
    int upgradeCostCredits() const;
};

class LogisticsCompany : public Corporation{
public:
    using Corporation::Corporation; //наследует конструктор базового класса

    CorporationType type() const override;
    std::string name() const override;

    int buildCostCredits() const override;
    Inventory buildCostResources() const override;
    Inventory upgradeCostResources() const override;
};

class MinerCompany : public Corporation{
public:
    using Corporation::Corporation;

    CorporationType type() const override;
    std::string name() const override;

    int buildCostCredits() const override;
    Inventory buildCostResources() const override;
    Inventory upgradeCostResources() const override;
};

class TechTrader : public Corporation{
public:
    using Corporation::Corporation;

    CorporationType type() const override;
    std::string name() const override;

    int buildCostCredits() const override;
    Inventory buildCostResources() const override;
    Inventory upgradeCostResources() const override;
};

//создаёт корпорацию по типу
std::unique_ptr<Corporation> createCorporation(CorporationType type);
//возвращает текстовое название типа
std::string corporationTypeName(CorporationType type);
//распознаёт название
bool parseCorporationType(const std::string& text, CorporationType& type);
