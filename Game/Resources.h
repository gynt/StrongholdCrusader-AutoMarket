#pragma once

#include <xutility>

namespace Game::Resource
{

enum Values : size_t
{
    None,
    u1,

    Wood,
    Hops,
    Stone,

    u2,

    Iron,
    Pitch,

    u3,

    Wheat,
    Bread,
    Cheese,
    Meat,
    Apples,
    Ale,
    Gold,
    Flour,
    Bows,
    Crossbows,
    Spears,
    Pikes,
    Maces,
    Swords,
    LeatherArmor,
    MetalArmor,

    Max,
};

constexpr char const* names[] = {
    "",
    "",
    "Wood",
    "Hops",
    "Stone",
    "",
    "Iron",
    "Pitch",
    "",
    "Wheat",
    "Bread",
    "Cheese",
    "Meat",
    "Apples",
    "Ale",
    "Gold",
    "Flour",
    "Bows",
    "Crossbows",
    "Spears",
    "Pikes",
    "Maces",
    "Swords",
    "LeatherArmor",
    "MetalArmor"
};

inline bool IsValid(size_t resource)
{
    static constexpr size_t invalids[] = { None, u1, u2, u3, Gold };
    return std::find(std::begin(invalids), std::end(invalids), resource) == std::end(invalids);
}

}