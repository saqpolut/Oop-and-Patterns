#include "corporation.h"

Corporation::Corporation(int level) : level_(level) {}

int Corporation::level() const{
    return level_;
}

void Corporation::upgrade(){
    ++level_;
}

//стоимость для первого уровня в у.е.
int Corporation::upgradeCostCredits() const{
    return 80 * level_;
}

// Логистическая корпорация
CorporationType LogisticsCompany::type() const{
    return CorporationType::Logistics;
}

std::string LogisticsCompany::name() const{
    return "Логистическая корпорация";
}

int LogisticsCompany::buildCostCredits() const{
    return 120;
}

Inventory LogisticsCompany::buildCostResources() const{
    return {{ResourceType::Crystal, 2}};
}

Inventory LogisticsCompany::upgradeCostResources() const{
    return {{ResourceType::Crystal, 1}};
}

// Добывающая корпорация
CorporationType MinerCompany::type() const{
    return CorporationType::Miner;
}

std::string MinerCompany::name() const{
    return "Добывающая корпорация";
}

int MinerCompany::buildCostCredits() const{
    return 150;
}

Inventory MinerCompany::buildCostResources() const{
    return {{ResourceType::Metal, 2}};
}

Inventory MinerCompany::upgradeCostResources() const{
    return {{ResourceType::Metal, 1}};
}

// Торговая корпорация
CorporationType TechTrader::type() const{
    return CorporationType::Trader;
}

std::string TechTrader::name() const{
    return "Торговая корпорация";
}

int TechTrader::buildCostCredits() const{
    return 180;
}

Inventory TechTrader::buildCostResources() const{
    return {{ResourceType::Diamond, 2}};
}

Inventory TechTrader::upgradeCostResources() const{
    return {{ResourceType::Diamond, 1}};
}

// Фабрика корпораций
std::unique_ptr<Corporation> createCorporation(CorporationType type){
    switch (type){
    case CorporationType::Logistics:
        return std::make_unique<LogisticsCompany>();

    case CorporationType::Miner:
        return std::make_unique<MinerCompany>();

    case CorporationType::Trader:
        return std::make_unique<TechTrader>();
    }

    return nullptr;
}

std::string corporationTypeName(CorporationType type){
    switch (type){
    case CorporationType::Logistics:
        return "логистика";

    case CorporationType::Miner:
        return "добыча";

    case CorporationType::Trader:
        return "торговля";
    }

    return "неизвестная корпорация";
}

bool parseCorporationType(const std::string& text, CorporationType& type){
    if (text == "logistics"){
        type = CorporationType::Logistics;
        return true;
    }

    if (text == "miner"){
        type = CorporationType::Miner;
        return true;
    }

    if (text == "trader"){
        type = CorporationType::Trader;
        return true;
    }

    return false;
}
