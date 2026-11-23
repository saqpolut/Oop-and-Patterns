#include "galaxy.h"

#include <algorithm>
#include <cmath>
#include <random>

namespace{
    //перебирает все типы ресурсов и выбирает количество
    Inventory generateDeposits(
        std::mt19937& engine,
        int min,
        int max
    )
    {
        std::uniform_int_distribution<int> amount(min, max);

        Inventory deposits;

        for (ResourceType type : allResourceTypes()){
            deposits[type] = amount(engine);
        }

        return deposits;
    }
}

//создание галактики
Galaxy::Galaxy(int planetCount, int asteroidCount){
    std::random_device device;
    std::mt19937 engine(device());

    //диапазоны случайных чисел
    std::uniform_real_distribution<double> coordinate(0.0, 100.0);
    std::uniform_int_distribution<int> goldIncome(25, 50);

    //создаётся планета и получает ID, название, координаты, запасы ресурсов, доходы от золота
    for (int index = 0; index < planetCount; ++index){
        const int id = static_cast<int>(bodies_.size());

        bodies_.push_back(std::make_unique<Planets>(
            id,
            "Планета-" + std::to_string(index + 1),
            coordinate(engine),
            coordinate(engine),
            generateDeposits(engine, 1, 3),
            goldIncome(engine)
        ));
    }

    //создаётся астероид
    for (int index = 0; index < asteroidCount; ++index){
        const int id = static_cast<int>(bodies_.size());

        bodies_.push_back(std::make_unique<Asteroids>(
            id,
            "Астероид-" + std::to_string(index + 1),
            coordinate(engine),
            coordinate(engine),
            generateDeposits(engine, 2, 5)
        ));
    }
}

int Galaxy::bodyCount() const{
    return static_cast<int>(bodies_.size());
}

//получение объека по ID
CelestialBody* Galaxy::body(int id){
    if (id < 0 || id >= bodyCount()){
        return nullptr;
    }

    return bodies_[id].get();
}

const CelestialBody* Galaxy::body(int id) const{
    if (id < 0 || id >= bodyCount()){
        return nullptr;
    }

    return bodies_[id].get();
}

const std::vector<std::unique_ptr<CelestialBody>>& Galaxy::bodies() const{
    return bodies_;
}

const std::vector<Route>& Galaxy::routes() const{
    return routes_;
}

//растояние между объектами
double Galaxy::distanceBetween(int firstId, int secondId) const{
    const CelestialBody* first = body(firstId);
    const CelestialBody* second = body(secondId);

    if (!first || !second){
        return 0.0;
    }

    const double dx = first->x() - second->x();
    const double dy = first->y() - second->y();

    return std::sqrt(dx * dx + dy * dy);
}

//проверка маршрута
bool Galaxy::hasRoute(int firstId, int secondId) const{
    for (const Route& route : routes_){
        const bool sameDirection = route.from == firstId && route.to == secondId;
        const bool reverseDirection = route.from == secondId && route.to == firstId;

        if (sameDirection || reverseDirection){
            return true;
        }
    }

    return false;
}

//добавление маршрута
bool Galaxy::addRoute(int planetId, int destinationId){
    //проеряем, что оба объекта существуют, они не совпадают, и маршрута между ними нет
    if (!body(planetId) 
        || !body(destinationId) 
        || planetId == destinationId 
        || hasRoute(planetId, destinationId)){
        return false;
    }

    const auto* planet = dynamic_cast<const Planets*>(body(planetId)); //первы объект - планета

    if (!planet || !planet->hasCorporation(CorporationType::Logistics)){
        return false;
    }

    routes_.push_back({
        planetId,
        destinationId,
        distanceBetween(planetId, destinationId),
        1
    });

    return true;
}

//улучшение маршрута
bool Galaxy::upgradeRoute(int routeIndex){
    if (routeIndex < 0 || routeIndex >= static_cast<int>(routes_.size())){
        return false;
    }

    ++routes_[routeIndex].level;
    return true;
}

//поиск достижимых объектов
std::vector<bool> Galaxy::reachableFrom(int startId) const{
    std::vector<bool> visited(bodyCount(), false); 

    if (!body(startId)){
        return visited;
    }

    std::vector<int> queue;
    queue.push_back(startId);
    visited[startId] = true;

    for (std::size_t position = 0; position < queue.size(); ++position){
        const int currentId = queue[position];

        for (const Route& route : routes_){
            int nextId = -1;

            if (route.from == currentId){
                nextId = route.to;
            }else if (route.to == currentId){
                nextId = route.from;
            }

            if (nextId >= 0 && nextId < bodyCount() && !visited[nextId]){
                visited[nextId] = true;
                queue.push_back(nextId);
            }
        }
    }

    return visited;
}

bool Galaxy::isReachable(int fromId, int toId) const{
    if (!body(toId)){
        return false;
    }

    const std::vector<bool> reachable = reachableFrom(fromId);
    return reachable[toId];
}

//расход топлива
int Galaxy::routeFuelCost(const Route& route) const{
    //чем выше уровень маршрута тем выше его эффективность
    const double efficiency = 1.0 + 0.5 * (route.level - 1);

    //результат округляется вверх но не может быть меньше единицы:
    const double distanceCost = route.distance / 10.0 / efficiency;

    return std::max(1, static_cast<int>(std::ceil(distanceCost)));
}
