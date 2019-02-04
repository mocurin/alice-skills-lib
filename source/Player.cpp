#include "Player.hpp"

std::string Trim(const std::string& str) {
    std::string result = str;
    std::remove(result.begin(), result.end(), ' ');
    return result;
}

std::array<char, FIELD_SIZE> StrToArr(const std::string& str) {
    std::array<char, FIELD_SIZE> arr;
    if (str.size() != FIELD_SIZE)
        return {};
    for (size_t i = 0; i < FIELD_SIZE; ++i) {
        arr[i] = str[i];
    }
    return arr;
}

std::ostream& operator<<(std::ostream& os, const Board& board) {
    for (auto& i : board) {
        for (auto& k : i) {
            os << k;
        }
        os << std::endl;
    }
    return os;
}

std::istream& operator>>(std::istream& is, Board& board) {
    std::string tmp;
    for (auto& i : board) {
        getline(is, tmp);
        i = StrToArr(tmp);
    }
    return is;
}

std::ostream& operator<<(std::ostream& os, const Player& player) {
    os << player.Name << std::endl;
    os << player.Boards.Outer;
    os << player.Boards.Inner;
    os << player.Ships.size() << std::endl;
    for (const auto& i : player.Ships) {
        os << i.first << std::endl;
        os << i.second.Positions.size() << std::endl;
        for (const auto& k : i.second.Positions)
            os << k.first << k.second << std::endl;
        os << i.second.Health << std::endl;
    }
    return os;
}

std::istream& operator>>(std::istream& is, Player& player) {
    getline(is, player.Name);
    is >> player.Boards.Outer;
    is >> player.Boards.Inner;
    std::string tmp;
    getline(is, tmp);
    size_t shipsSize = std::stoi(tmp);
    for (size_t i = 0; i < shipsSize; ++i) {
        std::string name;
        getline(is, name);
        getline(is, tmp);
        size_t positionsSize = std::stoi(tmp);
        std::vector<Tile> positions;
        for (size_t k = 0; k < positionsSize; ++k) {
            getline(is, tmp);
            positions.push_back(StrToPair(tmp));
        }
        getline(is, tmp);
        size_t health = std::stoi(tmp);
        Ship ship = { positions, health };
        player.Ships.emplace(name, ship);
    }
    return is;
}

bool IsOutOfBorders(const Tile& pos) {
    return pos.first > 9 || pos.second > 9;
}

bool IsProperSized(const Tile& spos, const Tile& epos) {
    return ShipSize(spos, epos) < 5;
}

bool IsProperPlaced(const Tile& spos, const Tile& epos) {
    return ShipSize(spos, epos) != 0;
}

size_t ShipSize(const Tile& spos, const Tile& epos) {
    size_t size = 1;
    if (spos.first == epos.first) {
        size += std::abs(
            static_cast<int>(spos.second) - static_cast<int>(epos.second));
        return size;
    }
    if (spos.second == epos.second) {
        size += std::abs(
            static_cast<int>(spos.first) - static_cast<int>(epos.first));
        return size;
    }
    return 0;
}

Tile StrToPair(const std::string& str) {
    if (str.length() == 2) {
        return { str.front() - '0', str.back() - '0' };
    }
	return { 0, 0 };
}
