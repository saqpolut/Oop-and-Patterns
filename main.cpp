#include "game.h"
#include "resources.h"

#include <iostream>
#include <limits>

namespace{
    bool readInteger(const std::string& prompt, int& value){
        std::cout << prompt;

        if (std::cin >> value){
            return true;
        }

        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        std::cout << "Ошибка: нужно ввести целое число.\n";
        return false;
    }

    bool readCredits(const std::string& prompt, double& value){
        std::cout << prompt;

        if (std::cin >> value){
            return true;
        }

        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        std::cout << "Ошибка: нужно ввести число.\n";
        return false;
    }
}

int main(){
    int planetCount = 0;
    int asteroidCount = 0;
    int turns = 0;
    double startingCredits = 0.0;

    if (!readInteger("Количество планет: ", planetCount) ||
        !readInteger("Количество астероидов: ", asteroidCount) ||
        !readInteger("Количество ходов: ", turns) ||
        !readCredits("Начальное количество у.е.: ", startingCredits))
    {
        return 1;
    }

    if (planetCount < 1 || asteroidCount < 0 || turns < 1 ||  startingCredits < 0.0){
        std::cerr << "Некорректные исходные данные.\n";
        return 1;
    }

    Inventory startingInventory;

    for (ResourceType type : allResourceTypes()){
        int amount = 0;

        const std::string prompt =
            "Начальное количество ресурса \"" +
            resourceName(type) + "\": ";

        if (!readInteger(prompt, amount) || amount < 0){
            std::cerr
                << "Количество ресурса должно быть "
                << "неотрицательным целым числом.\n";
            return 1;
        }

        startingInventory[type] = amount;
    }

    Game game(
        planetCount,
        asteroidCount,
        turns,
        startingCredits,
        startingInventory
    );

    game.run();

    return 0;
}
