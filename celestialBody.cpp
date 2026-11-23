#include "celestialBody.h"

#include <utility> //std::move

CelestialBody::CelestialBody(int id, const std::string& name, double x, double y, const Inventory& deposits)
    : id_(id), name_(name), x_(x), y_(y), deposits_(deposits) {}

int CelestialBody::id() const{
    return id_;
}

const std::string& CelestialBody::name() const{
    return name_;
}

double CelestialBody::x() const{
    return x_;
}

double CelestialBody::y() const{
    return y_;
}

const Inventory& CelestialBody::deposits() const{
    return deposits_;
}

//конструктор планет
Planets::Planets(int id, const std::string& name, double x, double y, const Inventory& deposits, int goldIncome)
    : CelestialBody(id, name, x, y, deposits), goldIncome_(goldIncome) {}

//методы планет
std::string Planets::kind() const{
    return "планета";
}

int Planets::goldIncome() const{
    return goldIncome_;
}

bool Planets::hasCorporation(CorporationType type) const{
    return corporations_.find(type) != corporations_.end();
}

//получение корпорации
Corporation* Planets::corporation(CorporationType type){
    auto it = corporations_.find(type);

    if (it == corporations_.end()){
        return nullptr;
    }

    return it->second.get();
}

const Corporation* Planets::corporation(CorporationType type) const
{
    auto it = corporations_.find(type);

    if (it == corporations_.end()){
        return nullptr;
    }

    return it->second.get();
}

//установка корпорации
bool Planets::installCorporation(std::unique_ptr<Corporation> corporation){
    if (!corporation){
        return false;
    }

    const CorporationType type = corporation->type();

    if (hasCorporation(type)){
        return false;
    }
    corporations_.emplace(type, std::move(corporation)); //добавляет новую пару тип-корпорация
    return true;
}

//конструктор астеройда
Asteroids::Asteroids(int id, const std::string& name, double x, double y, const Inventory& deposits)
    : CelestialBody(id, name, x, y, deposits) {}

std::string Asteroids::kind() const{
    return "астероид";
}

int Asteroids::goldIncome() const{
    return 0;
}
