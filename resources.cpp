#include "resources.h"

//реализация кристаллов
ResourceType CrystalResource::type() const{
    return ResourceType::Crystal;
}

std::string CrystalResource::name() const{
    return "кристаллы";
}

int CrystalResource::basePrice() const{
    return 30;
}

//реализация металла
ResourceType MetalResource::type() const{
    return ResourceType::Metal;
}

std::string MetalResource::name() const{
    return "металл";
}

int MetalResource::basePrice() const{
    return 20;
}

//реализация нефти
ResourceType OilResource::type() const{
    return ResourceType::Oil;
}

std::string OilResource::name() const{
    return "нефть";
}

int OilResource::basePrice() const{
    return 25;
}

//реализация альмазов
ResourceType DiamondResource::type() const{
    return ResourceType::Diamond;
}

std::string DiamondResource::name() const{
    return "алмазы";
}

int DiamondResource::basePrice() const{
    return 60;
}

//функция получает тип и создаёт соответствующий объект
std::unique_ptr<Resources> createResource(ResourceType type){
    switch (type){
    case ResourceType::Crystal:
        return std::make_unique<CrystalResource>();

    case ResourceType::Metal:
        return std::make_unique<MetalResource>();

    case ResourceType::Oil:
        return std::make_unique<OilResource>();

    case ResourceType::Diamond:
        return std::make_unique<DiamondResource>();
    }

    return nullptr;
}

//список всех типов ресурсов
std::vector<ResourceType> allResourceTypes(){
    return{
        ResourceType::Crystal,
        ResourceType::Metal,
        ResourceType::Oil,
        ResourceType::Diamond
    };
}

//отображение имени объекта
std::string resourceName(ResourceType type){
    std::unique_ptr<Resources> resource = createResource(type);

    if (!resource){
        return "неизвестный ресурс";
    }

    return resource->name();
}

//получение базовой цены
int resourceBasePrice(ResourceType type){
    std::unique_ptr<Resources> resource = createResource(type);

    if (!resource){
        return 0;
    }

    return resource->basePrice();
}

//разбор введённого названия
bool parseResourceName(const std::string& text, ResourceType& type){
    if (text == "crystal"){
        type = ResourceType::Crystal;
        return true;
    }

    if (text == "metal"){
        type = ResourceType::Metal;
        return true;
    }

    if (text == "oil"){
        type = ResourceType::Oil;
        return true;
    }

    if (text == "diamond"){
        type = ResourceType::Diamond;
        return true;
    }

    return false;
}