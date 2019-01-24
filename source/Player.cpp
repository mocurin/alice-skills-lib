#include "Player.hpp"

std::string Trim(const std::string& str) {
    std::string result = str;
    std::remove(result.begin(), result.end(), ' ');
    return result;
}

std::array<char, FIELD_SIZE> StrToArr(const std::string& str) {
    std::array<char, FIELD_SIZE> arr;
    std::string trimmed = Trim(str);
    if (trimmed.size() != FIELD_SIZE)
        return {};
    for (size_t i = 0; i < FIELD_SIZE; ++i) {
        if (trimmed[i] < '0' || trimmed[i] > '9')
            return {};
        arr[i] = trimmed[i] - '0';
    }
    return arr;
}

std::ostream& operator<<(std::ostream& os, const Board& board) {
    for (auto& i : board) {
        for (auto& k : i) {
            os << k;
            if (k != i.back())
                os << ' ';
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
    os << player.Generator.Seed << std::endl;
    return os;
}

std::istream& operator>>(std::istream& is, Player& player) {
    getline(is, player.Name);
    is >> player.Boards.Outer;
    is >> player.Boards.Inner;
    std::string tmp;
    getline(is, tmp);
    player.Generator.Seed = std::stoul(tmp);
    std::mt19937 generator(player.Generator.Seed);
    player.Generator.Body = generator;
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
    if (spos.first == epos.first) {
        size_t size = std::abs(
            static_cast<int>(spos.second) - static_cast<int>(epos.second));
        return size;
    }
    if (spos.second == epos.second) {
        size_t size = std::abs(
            static_cast<int>(spos.first) - static_cast<int>(epos.first));
        return size;
    }
    return 0;
}