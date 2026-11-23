#pragma once

#include <map>
#include <memory> //unique_ptr
#include <string>
#include <vector> //для списка типов ресурсов

//перечисление всевозможных ресурсов
enum class ResourceType
{
    Crystal,
    Metal,
    Oil,
    Diamond
};

//склад ресурсов (тип ресурса, количество)
using Inventory = std::map<ResourceType, int>; 


class Resources{
public:
    //гарантирует корректный вызов деструктора производного класса при удалении 
    //объекта через указатель на базовый класс
    virtual ~Resources() = default;

    //метод без реализации по умолчанию
    //делают класс абстарктным
    //можно переопределить в дочерни классах
    virtual ResourceType type() const = 0;
    virtual std::string name() const = 0; //имя метода - возвращает название
    virtual int basePrice() const = 0;
};

//классы конктерных ресурсов

class CrystalResource : public Resources{
public:
    ResourceType type() const override;
    std::string name() const override;
    int basePrice() const override;
};

class MetalResource : public Resources{
public:
    ResourceType type() const override;
    std::string name() const override;
    int basePrice() const override;
};

class OilResource : public Resources{
public:
    ResourceType type() const override;
    std::string name() const override;
    int basePrice() const override;
};

class DiamondResource : public Resources{
public:
    ResourceType type() const override;
    std::string name() const override;
    int basePrice() const override;
};

//по типу ресурса создаёт объект
std::unique_ptr<Resources> createResource(ResourceType type);

//список всех типов ресурсов
std::vector<ResourceType> allResourceTypes();

//возвращает название для отображения
std::string resourceName(ResourceType type); 
//возвращает базовую цену
int resourceBasePrice(ResourceType type); 
//распознаёт введённый текст
bool parseResourceName(const std::string& text, ResourceType& type); 