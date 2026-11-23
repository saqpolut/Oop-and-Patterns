#pragma once

#include "galaxy.h"

#include <set> //для учёта действий за ход

class Game{
private:
    static constexpr double TARGET_CREDITS = 3000.0;

    Galaxy galaxy_; //люъекты галактики
    Inventory inventory_; // общий склад ресурсов
    double credits_; // деньги в казне

    int maximumTurns_; //максимальное количесвто ходов
    int currentTurn_; //номер текущего хода
    int currentPlanetId_; //ID-планеты, на которой сейчас игрок

    std::set<int> corporationActionsThisTurn_; //ID планеты, на которой добавили или улучшили корпорация
    std::set<int> routeUpgradesThisTurn_; //хранит индексы маршрутов, которые уже уличшили в этом ходе

    bool running_; //флаг, показывает продолжается ли игра

    bool spend(double credits, const Inventory& materials); //хватает ли денег и ресурсов для операции
    int traderPower() const; //считает суммарную силу торговых корпораций - сумма уровней корпораций

    void showWorld() const; //планеты и астероиды
    void showStatus() const; // казна, ресурсы, текущая планета, корпорации
    void showMenu() const; //печатает список команд

    void switchPlanet(); //переключится на доступную планету
    void buildOrUpgradeCorporation(); // строительство или улучшение корпорации
    void buildRoute(); // прокладка маршрута
    void upgradeRoute(); // абгрейд маршрута
    void sellResource(); // продажа ресурсов
    void buyResource(); // покупка ресурсов

    void processTurn(); //обработка дохода, добычи ресурсов и расходов на топливо
    void finishTurn(); // завершает ход

public:
    Game(
        int planetCount, //количесвто планет
        int asteroidCount, // количество астероидов
        int maximumTurns, //длительность игры в ходах
        double startingCredits, // стартовая сумма денег
        const Inventory& startingInventory // стартовый набор ресурсов
    );

    void run(); //запускает основной цикл игры
};
