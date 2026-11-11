//
// Created by csobe on 2026-11-09.
//

#include "MapParser.h"

#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "MathUtil.h"

namespace ECSEngine {

bool ParseMap(const std::string& path, MapData& map) {
    std::string line;
    std::ifstream file(path);
    auto next = [&]() {
        bool lineExists = static_cast<bool>(std::getline(file, line));
        if (lineExists && !line.empty() && line.back() == '\n') {
            line.pop_back();
        }
        return lineExists;
    };
    if (!file) {
        return false;
    }

    /*
    Order:
        First line separately
        Lines until map
        Map line
        lines after map
    */

    //First Line:
    if (!next()) {
        return false;
    }

    {
        std::istringstream stream(line);
        std::string word;
        stream >> word >> map.dicType;
        if (!stream || word != "dictionary") {
            return false;
        }
    }

    //Lines until "map"

    std::string symbol;
    while (next()) {
        std::istringstream stream(line);

        if (!(stream >> symbol)) {
            continue;
        }

        if (symbol == "map") {
            break;
        }

        MapEntry entry;
        float stlx, stly, sw, sh, btlx, btly, bw, bh; //Stores our sprite location and bounding boxes

        stream >> entry.path >> stlx >> stly >> sw >> sh;

        if (map.dicType == 2) {
            stream >> btlx >> btly >> bw >> bh;
        }

        entry.source = Rect({stlx, stly}, sw, sh);
        entry.bounds = Rect({btlx, btly}, bw, bh);
        map.entries[symbol[0]] = entry;
    }

    //The line that starts with map now
    {
        std::istringstream stream(line);
        std::string word;
        stream >> word >> word >> map.otlx >> map.otly >> word
        >> map.ow >> map.oh >> word >> map.ogw >> map.ogh;
    }

    //Lines after map
    for (int i = 0; i < map.ogh; i++) {
        map.rows.push_back(line);
    }
    return true;

}
}