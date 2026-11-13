//
// Created by csobe on 2026-11-09.
//

#ifndef PLATFORMER_MAPPARSER_H
#define PLATFORMER_MAPPARSER_H

//
// Created by csobe on 2026-11-09.
//

#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "Entity.h"
#include "MapParser.h"
#include "MathUtil.h"

namespace ECSEngine {


struct MapEntry {
    std::string path;
    Rect source;
    Rect bounds;
    EntityID id;
};

struct MapData {
    //For the dictonary type
    int dicType = 0;
    //Map for our entries
    std::unordered_map<char, MapEntry> entries;
    //Map Origin info
    float otlx, otly, ow, oh, ogw, ogh;
    //Stores map layout
    std::vector<std::string> rows;
};

bool ParseMap(const std::string& dir, const std::string source, MapData& map);
}

#endif  // PLATFORMER_MAPPARSER_H
