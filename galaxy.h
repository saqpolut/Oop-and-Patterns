#pragma once

#include "celestialBody.h"

#include <memory>
#include <vector>

//структура маршрута
struct Route{
    int from;
    int to;
    double distance;
    int level; //уровень маршрута
};

class Galaxy{
private:
    std::vector<std::unique_ptr<CelestialBody>> bodies_; //планеты и астероиды
    std::vector<Route> routes_; //созданные маршруты

public:
    Galaxy(int planetCount, int asteroidCount);

    int bodyCount() const; //количество всех планет и астероидов

    CelestialBody* body(int id);
    const CelestialBody* body(int id) const;

    const std::vector<std::unique_ptr<CelestialBody>>& bodies() const; //список объектов
    const std::vector<Route>& routes() const; //список маршрутов

    double distanceBetween(int firstId, int secondId) const; //расстояние между объектами

    bool hasRoute(int firstId, int secondId) const; //есть маршрут или нет
    bool addRoute(int planetId, int destinationId); // добавить маршрут
    bool upgradeRoute(int routeIndex); //проабгрейдить маршрут

    //проерка, до каких объектов можно добраться
    std::vector<bool> reachableFrom(int startId) const; 
    bool isReachable(int fromId, int toId) const;

    int routeFuelCost(const Route& route) const; //рассчитать стоимоть прохождения маршрута
};
