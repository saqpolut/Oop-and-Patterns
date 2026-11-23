#include "game.h"

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <limits>
#include <string>

namespace{
    bool readInteger(const std::string& prompt, int& value){
        std::cout << prompt;

        if (std::cin >> value){
            return true;
        }

        std::cin.clear();
        std::cin.ignore(
            std::numeric_limits<std::streamsize>::max(),
            '\n'
        );

        std::cout << "Нужно ввести целое число.\n";
        return false;
    }

    bool readResourceType(ResourceType& type){
        std::string text;

        std::cout
            << "Введите название ресурса: "
            << "crystal, "
            << "metal, "
            << "oil, "
            << "diamond: ";

        if (!(std::cin >> text)){
            return false;
        }

        if (!parseResourceName(text, type)){
            std::cout << "Такого ресурса нет.\n";
            return false;
        }

        return true;
    }

    int inventoryAmount(const Inventory& inventory, ResourceType type){
        const auto it = inventory.find(type);

        if (it == inventory.end()){
            return 0;
        }

        return it->second;
    }
}

Game::Game(
    int planetCount,
    int asteroidCount,
    int maximumTurns,
    double startingCredits,
    const Inventory& startingInventory
)
    : galaxy_(planetCount, asteroidCount),
      inventory_(startingInventory),
      credits_(startingCredits),
      maximumTurns_(maximumTurns),
      currentTurn_(1),
      currentPlanetId_(0),
      running_(true)
{
    for (ResourceType type : allResourceTypes()){
        if (inventory_.find(type) == inventory_.end()){
            inventory_[type] = 0;
        }
    }

    // На стартовой планете три корпорации уже построены.
    auto* startingPlanet = dynamic_cast<Planets*>(galaxy_.body(0));

    if (startingPlanet){
        startingPlanet->installCorporation(
            createCorporation(CorporationType::Logistics)
        );

        startingPlanet->installCorporation(
            createCorporation(CorporationType::Miner)
        );

        startingPlanet->installCorporation(
            createCorporation(CorporationType::Trader)
        );
    }
}

bool Game::spend(double credits, const Inventory& materials){
    if (credits_ < credits){
        std::cout << "Недостаточно у.е.\n";
        return false;
    }

    for (const auto& [type, amount] : materials){
        if (inventoryAmount(inventory_, type) < amount){
            std::cout << "Недостаточно ресурса: "
                      << resourceName(type) << ".\n";
            return false;
        }
    }

    credits_ -= credits;

    for (const auto& [type, amount] : materials){
        inventory_[type] -= amount;
    }

    return true;
}

int Game::traderPower() const{
    int power = 0;

    for (const auto& body : galaxy_.bodies()){
        const auto* planet =
            dynamic_cast<const Planets*>(body.get());

        if (!planet){
            continue;
        }

        const Corporation* trader =
            planet->corporation(CorporationType::Trader);

        if (trader){
            power += trader->level();
        }
    }

    return power;
}

void Game::showWorld() const{
    std::cout << "\nОбъекты галактики:\n";

    for (const auto& body : galaxy_.bodies()){
        std::cout << "ID " << body->id()
                  << " — " << body->kind()
                  << " \"" << body->name() << "\""
                  << " (x=" << std::fixed << std::setprecision(1)
                  << body->x() << ", y=" << body->y() << ')';

        if (body->id() == currentPlanetId_){
            std::cout << " [текущая планета]";
        }

        std::cout << '\n';
    }
}

void Game::showStatus() const{
    std::cout << "\n--- Состояние империи ---\n"
              << "Ход: " << currentTurn_
              << " из " << maximumTurns_ << '\n'
              << "Казна: " << std::fixed << std::setprecision(1)
              << credits_ << " у.е.\n"
              << "Цель: " << TARGET_CREDITS << " у.е.\n";

    std::cout << "Общий склад:\n";

    for (ResourceType type : allResourceTypes()){
        std::cout << "  " << resourceName(type)
                  << ": " << inventoryAmount(inventory_, type)
                  << '\n';
    }

    const CelestialBody* currentBody = galaxy_.body(currentPlanetId_);

    if (currentBody){
        std::cout << "Текущий объект: "
                  << currentBody->name() << '\n';

        const auto* planet = dynamic_cast<const Planets*>(currentBody);

        if (planet){
            std::cout << "Доход золота за ход: "
                      << planet->goldIncome() << " у.е.\n";

            std::cout << "Корпорации на планете:\n";

            for (CorporationType type : {
                     CorporationType::Logistics,
                     CorporationType::Miner,
                     CorporationType::Trader})
            {
                const Corporation* corporation = planet->corporation(type);

                if (corporation){
                    std::cout << "  " << corporation->name()
                              << ", уровень "
                              << corporation->level() << '\n';
                }else{
                    std::cout << "  "
                              << corporationTypeName(type)
                              << " — не построена\n";
                }
            }
        }
    }

    std::cout << "Маршрутов в галактике: " << galaxy_.routes().size() << '\n';
}

void Game::showMenu() const{
    std::cout
        << "\nКоманды:\n"
        << "status        — показать состояние империи\n"
        << "switch        — переключиться на доступную планету\n"
        << "corporation   — построить или улучшить корпорацию\n"
        << "route         — проложить маршрут\n"
        << "upgrade_route — улучшить маршрут\n"
        << "sell          — продать ресурс\n"
        << "buy           — купить ресурс\n"
        << "end           — завершить ход\n"
        << "quit          — завершить игру\n"
        << "Введите команду: ";
}

void Game::switchPlanet(){
    const std::vector<bool> reachable = galaxy_.reachableFrom(currentPlanetId_);

    std::cout << "Доступные планеты:\n";

    for (const auto& body : galaxy_.bodies()){
        const auto* planet = dynamic_cast<const Planets*>(body.get());

        if (planet && reachable[planet->id()]){
            std::cout << "  ID " << planet->id() << " — " << planet->name() << '\n';
        }
    }

    int destinationId = -1;
    if (!readInteger("Введите ID планеты: ", destinationId)){
        return;
    }

    const auto* destination = dynamic_cast<const Planets*>(galaxy_.body(destinationId));

    if (!destination ||
        destinationId < 0 ||
        destinationId >= static_cast<int>(reachable.size()) ||
        !reachable[destinationId])
    {
        std::cout << "Планета недоступна: до неё нет цепочки маршрутов.\n";
        return;
    }

    currentPlanetId_ = destinationId;

    std::cout << "Текущая планета: " << destination->name() << '\n';
}

void Game::buildOrUpgradeCorporation(){
    if (corporationActionsThisTurn_.count(currentPlanetId_) > 0){
        std::cout
            << "На этой планете корпорация уже строилась "
            << "или улучшалась в текущем ходу.\n";
        return;
    }

    auto* planet = dynamic_cast<Planets*>(galaxy_.body(currentPlanetId_));

    if (!planet){
        std::cout << "Текущий объект не является планетой.\n";
        return;
    }

    std::string text;

    std::cout
        << "Введите корпорацию: logistics (логистика), "
        << "miner (добыча), trader (торговля): ";

    if (!(std::cin >> text)){
        return;
    }

    CorporationType type;

    if (!parseCorporationType(text, type)){
        std::cout << "Неизвестный тип корпорации.\n";
        return;
    }

    Corporation* existing = planet->corporation(type);

    if (existing){
        if (!spend(
                existing->upgradeCostCredits(),
                existing->upgradeCostResources())){
            return;
        }

        existing->upgrade();

        std::cout << existing->name()
                  << " улучшена до уровня "
                  << existing->level() << ".\n";
    }else{
        std::unique_ptr<Corporation> corporation =
            createCorporation(type);

        if (!corporation){
            std::cout << "Не удалось создать корпорацию.\n";
            return;
        }

        if (!spend(
                corporation->buildCostCredits(),
                corporation->buildCostResources())){
            return;
        }

        const std::string corporationName = corporation->name();

        if (!planet->installCorporation(std::move(corporation))){
            std::cout << "Не удалось установить корпорацию.\n";
            return;
        }

        std::cout << corporationName << " построена.\n";

    }

    corporationActionsThisTurn_.insert(currentPlanetId_);
}

void Game::buildRoute(){
    auto* planet = dynamic_cast<Planets*>(galaxy_.body(currentPlanetId_));

    if (!planet || !planet->hasCorporation(CorporationType::Logistics)){
        std::cout
            << "Для прокладки маршрута нужна логистическая "
            << "корпорация на текущей планете.\n";
        return;
    }

    showWorld();

    int destinationId = -1;

    if (!readInteger(
            "Введите ID планеты или астероида назначения: ",
            destinationId)){
        return;
    }

    const CelestialBody* destination =galaxy_.body(destinationId);

    if (!destination){
        std::cout << "Объект с таким ID не найден.\n";
        return;
    }

    if (destinationId == currentPlanetId_){
        std::cout << "Нельзя проложить маршрут к текущей планете: "
                    "это маршрут объекта к самому себе.\n";
        return;
    }

    if (galaxy_.hasRoute(currentPlanetId_, destinationId)){
        std::cout << "Между этими объектами маршрут уже существует.\n";
        return;
    }

    const double distance = galaxy_.distanceBetween(currentPlanetId_, destinationId);

    const int cost = 20 + static_cast<int>(std::ceil(distance * 2.0));

    std::cout << "Расстояние: " << std::fixed
              << std::setprecision(1) << distance
              << ". Стоимость прокладки: "
              << cost << " у.е.\n";

    if (credits_ < cost){
        std::cout << "Недостаточно у.е.\n";
        return;
    }

    if (!galaxy_.addRoute(currentPlanetId_, destinationId)){
        std::cout << "Не удалось добавить маршрут.\n";
        return;
    }

    credits_ -= cost;

    std::cout << "Маршрут проложен.\n";
}

void Game::upgradeRoute(){
    auto* planet = dynamic_cast<Planets*>(galaxy_.body(currentPlanetId_));

    if (!planet || !planet->hasCorporation(CorporationType::Logistics)){
        std::cout
            << "Для улучшения маршрута нужна логистическая "
            << "корпорация на текущей планете.\n";
        return;
    }

    std::vector<int> availableRoutes;

    const std::vector<Route>& routes = galaxy_.routes();

    for (int index = 0;
         index < static_cast<int>(routes.size());
         ++index)
    {
        const Route& route = routes[index];

        if (route.from != currentPlanetId_ && route.to != currentPlanetId_){
            continue;
        }

        availableRoutes.push_back(index);

        const CelestialBody* fromBody = galaxy_.body(route.from);

        const CelestialBody* toBody = galaxy_.body(route.to);

        std::cout
            << "Маршрут ID " << index << ": "
            << fromBody->name() << " — " << toBody->name()
            << ", уровень " << route.level
            << ", расход топлива за ход "
            << galaxy_.routeFuelCost(route) << " у.е.\n";
    }

    if (availableRoutes.empty()){
        std::cout << "У текущей планеты нет маршрутов.\n";
        return;
    }

    int routeIndex = -1;

    if (!readInteger("Введите ID маршрута: ", routeIndex)){
        return;
    }

    const bool routeIsAvailable =
        std::find(
            availableRoutes.begin(),
            availableRoutes.end(),
            routeIndex
        ) != availableRoutes.end();

    if (!routeIsAvailable){
        std::cout << "Такого маршрута у текущей планеты нет.\n";
        return;
    }

    if (routeUpgradesThisTurn_.count(routeIndex) > 0){
        std::cout
            << "Этот маршрут уже улучшался в текущем ходу.\n";
        return;
    }

    const Route& route = routes[routeIndex];
    const int cost = 50 * route.level;
    const Inventory materials = {
        {ResourceType::Metal, 1}
    };

    if (!spend(cost, materials)){
        return;
    }

    if (!galaxy_.upgradeRoute(routeIndex)){
        std::cout << "Не удалось улучшить маршрут.\n";
        return;
    }

    routeUpgradesThisTurn_.insert(routeIndex);

    std::cout << "Маршрут улучшен. Новый уровень: " << galaxy_.routes()[routeIndex].level << ".\n";

}

void Game::sellResource(){
    auto* planet =
        dynamic_cast<Planets*>(galaxy_.body(currentPlanetId_));

    if (!planet || !planet->hasCorporation(CorporationType::Trader)){
        std::cout
            << "Для торговли нужна торговая корпорация "
            << "на текущей планете.\n";
        return;
    }

    ResourceType type;

    if (!readResourceType(type)){
        return;
    }

    int amount = 0;

    if (!readInteger("Сколько единиц продать: ", amount) || amount <= 0){
        std::cout << "Количество должно быть положительным.\n";
        return;
    }

    if (inventoryAmount(inventory_, type) < amount){
        std::cout << "На складе недостаточно ресурса.\n";
        return;
    }

    const int power = traderPower();
    const double priceMultiplier = 1.0 + 0.05 * power;

    const int unitPrice = std::max(
        1,
        static_cast<int>(
            std::floor(resourceBasePrice(type) * priceMultiplier)
        )
    );

    const int income = unitPrice * amount;

    inventory_[type] -= amount;
    credits_ += income;

    std::cout << "Продано " << amount << " ед. ресурса \""
              << resourceName(type) << "\" по "
              << unitPrice << " у.е. за единицу. Получено "
              << income << " у.е.\n";
}

void Game::buyResource(){
    auto* planet = dynamic_cast<Planets*>(galaxy_.body(currentPlanetId_));

    if (!planet || !planet->hasCorporation(CorporationType::Trader)){
        std::cout
            << "Для торговли нужна торговая корпорация "
            << "на текущей планете.\n";
        return;
    }

    ResourceType type;

    if (!readResourceType(type)){
        return;
    }

    int amount = 0;

    if (!readInteger("Сколько единиц купить: ", amount) || amount <= 0){
        std::cout << "Количество должно быть положительным.\n";
        return;
    }

    const int power = traderPower();

    const double priceMultiplier = std::max(0.65, 1.25 - 0.05 * power);

    const int unitPrice = static_cast<int>( std::ceil(resourceBasePrice(type) * priceMultiplier)
    );

    const int totalCost = unitPrice * amount;

    if(credits_ < totalCost){
        std::cout << "Недостаточно у.е. Требуется: " << totalCost << ".\n";
        return;
    }

    credits_ -= totalCost;
    inventory_[type] += amount;

    std::cout << "Куплено " << amount << " ед. ресурса \""
              << resourceName(type) << "\" по "
              << unitPrice << " у.е. за единицу. Потрачено "
              << totalCost << " у.е.\n";
}

void Game::processTurn(){
    std::cout << "\nНачисление дохода и добыча ресурсов...\n";

    const std::vector<bool> reachable = galaxy_.reachableFrom(0);

    // Доступные планеты приносят золото.
    // Добывающая корпорация собирает ресурсы на своей планете.
    for (const auto& body : galaxy_.bodies()){
        const auto* planet = dynamic_cast<const Planets*>(body.get());

        if (!planet || !reachable[planet->id()]){
            continue;
        }

        credits_ += planet->goldIncome();

        const Corporation* miner = planet->corporation(CorporationType::Miner);

        if (!miner){
            continue;
        }

        for (const auto& [type, amount] : planet->deposits()){
            inventory_[type] += amount * miner->level();
        }
    }

    // Для добычи на астероиде должна существовать прямая связь
    // маршрутом с планетой, где построена добывающая корпорация.
    for (const Route& route : galaxy_.routes()){
        const CelestialBody* first = galaxy_.body(route.from);
        const CelestialBody* second = galaxy_.body(route.to);
        const auto* planet = dynamic_cast<const Planets*>(first);
        const auto* asteroid = dynamic_cast<const Asteroids*>(second);

        if (!planet || !asteroid) {
            planet = dynamic_cast<const Planets*>(second);
            asteroid = dynamic_cast<const Asteroids*>(first);
        }

        if (!planet || !asteroid || !reachable[planet->id()]){
            continue;
        }

        const Corporation* miner = planet->corporation(CorporationType::Miner);

        if (!miner){
            continue;
        }

        const int fuelCost = galaxy_.routeFuelCost(route);

        if (credits_ < fuelCost){
            std::cout << "Не хватило у.е. на топливо для маршрута "
                      << planet->name() << " — "
                      << asteroid->name()
                      << ". Добыча на астероиде пропущена.\n";
            continue;
        }

        credits_ -= fuelCost;

        for (const auto& [type, amount] : asteroid->deposits()){
            inventory_[type] += amount * miner->level();
        }

        std::cout << "На астероиде " << asteroid->name()
                  << " добыты ресурсы. Топливо: "
                  << fuelCost << " у.е.\n";
    }
}

void Game::finishTurn(){
    processTurn();

    corporationActionsThisTurn_.clear();
    routeUpgradesThisTurn_.clear();

    std::cout << "Ход завершён. Казна: "
              << std::fixed << std::setprecision(1)
              << credits_ << " у.е.\n";

    ++currentTurn_;
}

void Game::run(){
    std::cout
        << "\n=== Галактическая экономика "
        << "\"Империя звёзд\" ===\n"
        << "Цель: накопить " << TARGET_CREDITS << " у.е.\n"
        << "Для команды вводите её название, например status или sell.\n";

    showWorld();

    while(running_ && currentTurn_ <= maximumTurns_ && credits_ < TARGET_CREDITS){
        std::cout << "\n========== Ход "
                  << currentTurn_ << " из "
                  << maximumTurns_ << " ==========\n";

        while (running_ && credits_ < TARGET_CREDITS){
            showMenu();

            std::string command;

            if (!(std::cin >> command)){
                running_ = false;
                break;
            }

            if (command == "status"){
                showStatus();
            }
            else if (command == "switch"){
                switchPlanet();
            }
            else if (command == "corporation"){
                buildOrUpgradeCorporation();
            }
            else if (command == "route"){
                buildRoute();
            }
            else if (command == "upgrade_route"){
                upgradeRoute();
            }
            else if (command == "sell"){
                sellResource();
            }
            else if (command == "buy"){
                buyResource();
            }
            else if (command == "end"){
                finishTurn();
                break;
            }
            else if (command == "quit"){
                running_ = false;
                break;
            }
            else{
                std::cout
                    << "Неизвестная команда. "
                    << "Введите status, switch, corporation, route, "
                    << "upgrade_route, sell, buy, end или quit.\n";
            }
        }
    }

    if (credits_ >= TARGET_CREDITS){
        std::cout << "\nПобеда! В казне "
                  << std::fixed << std::setprecision(1)
                  << credits_ << " у.е.\n";
    }
    else if (currentTurn_ > maximumTurns_){
        std::cout << "\nХоды закончились. Победить не удалось.\n"
                  << "Итоговая казна: "
                  << std::fixed << std::setprecision(1)
                  << credits_ << " у.е.\n"
                  << "Необходимая цель: "
                  << TARGET_CREDITS << " у.е.\n";
    }
    else{
        std::cout << "\nИгра завершена пользователем.\n"
                  << "Итоговая казна: "
                  << std::fixed << std::setprecision(1)
                  << credits_ << " у.е.\n";
    }
}
