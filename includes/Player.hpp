#pragma once
#include <algorithm>
#include <array>
#include <ctime>
#include <iostream>
#include <map>
#include <random>
#include <string>
#include <vector>

#define Tile std::pair<size_t, size_t>
#define Board std::array<std::array<char, FIELD_SIZE>, FIELD_SIZE>

enum constants {FIELD_SIZE = 10};

struct Ship {
    std::vector<Tile> Positions;
    size_t Health;
};

std::string Trim(const std::string& str);
bool IsOutOfBorders(const Tile& pos);
bool IsProperSized(const Tile& spos, const Tile& epos);
bool IsProperPlaced(const Tile& spos, const Tile& epos);
size_t ShipSize(const Tile& spos, const Tile& epos);
Tile StrToPair(const std::string& str);

class Player {
private:
    std::string Name;
    struct Fields {
        Board Outer;
        Board Inner;
    } Boards;
    std::map<std::string, Ship> Ships;
    std::mt19937* Gen;

    void FillOuter(const char& symbol) {
        for (auto& i : Boards.Outer)
            i.fill(symbol);
    }
    void FillInner(const char& symbol) {
        for (auto& i : Boards.Inner)
            i.fill(symbol);
    }
    void MarkShip(const std::string& name) {
        for (const auto& i : Ships[name].Positions)
            Boards.Inner[i.first][i.second] = 'X';
    }
// GENERATION
    Tile PickRand() {
        std::uniform_int_distribution<> range(0, 9);
        Tile pos = { range(*Gen), range(*Gen) };
        while (!IsProperForPlacement(pos))
            pos = { range(*Gen), range(*Gen) };
        return pos;
    }
    Tile ThrowRand(const Tile& prev, const size_t& jump) {
        std::uniform_int_distribution<> directions(0, 3);
        Tile thrown = { 0, 0 };
        for (size_t k = 0, i = directions(*Gen); k < 4;
            ++k, ++i %= 4) {
            switch (i) {
            case 0:
                thrown = { prev.first - jump, prev.second };
                break;
            case 1:
                thrown = { prev.first + jump, prev.second };
                break;
            case 2:
                thrown = { prev.first, prev.second + jump };
                break;
            case 3:
                thrown = { prev.first, prev.second - jump };
                break;
            }
            if (IsProperForPlacement(thrown))
                return thrown;
        }
        return { 10, 10 };
    }
    Ship BuildShip(const Tile& spos, const Tile& epos) {
        std::vector<Tile> positions;
        size_t size = ShipSize(spos, epos);
        Tile lower = spos;
        if (spos.first == epos.first) {
            if (spos.second > epos.second)
                lower = epos;
            for (size_t i = 0; i < size; ++i)
                positions.push_back({lower.first, lower.second + i});
        }
        else if (spos.second == epos.second) {
            if (spos.first > epos.first)
                lower = epos;
            for (size_t i = 0; i < size; ++i)
                positions.push_back({ lower.first + i, lower.second });
        }
        Ship ship = { positions, size };
        return ship;
    }
// BATTLE
    void Destroy(const std::string& name) {
        Tile pos = Ships[name].Positions.front();
        --pos.first;
        for (size_t i = 0; i < 3; ++i, ++pos.first) {
            if (pos.first > 9)
                continue;
            if (pos.second - 1 < 10)
                Boards.Outer[pos.first][pos.second - 1] = 'x';
            Boards.Outer[pos.first][pos.second] = 'x';
            if (pos.second + 1 < 10)
                Boards.Outer[pos.first][pos.second + 1] = 'x';
        }
        if (Ships[name].Positions.size() != 1) {
            pos = Ships[name].Positions.back();
            --pos.first;
            for (size_t i = 0; i < 3; ++i, ++pos.first) {
                if (pos.first > 9)
                    continue;
                if (pos.second - 1 < 10)
                    Boards.Outer[pos.first][pos.second - 1] = 'x';
                Boards.Outer[pos.first][pos.second] = 'x';
                if (pos.second + 1 < 10)
                    Boards.Outer[pos.first][pos.second + 1] = 'x';
            }
        }
        for (const auto& i : Ships[name].Positions) {
            Boards.Outer[i.first][i.second] = 'D';
        }
    }
public:
    Player(const std::string& name, std::mt19937* generator) {
        Name = name;
        FillOuter('~');
        FillInner('~');
        Gen = generator;
    }
    Player(std::mt19937* generator) : Player("nan", generator) {}
    Player(const std::string& name) : Player(name, nullptr) {}
    Player() : Player("nan", nullptr) {} 
    ~Player() {}
    Player& operator=(Player& player)
    {
        Name = player.Name;
        Boards.Inner = player.Boards.Inner;
        Boards.Outer = player.Boards.Outer;
        Ships = player.Ships;
        Gen = player.Gen;
        return *this;
    }
    void SetShips(std::map<std::string, Ship> ships) {
        Ships = ships;
    }
    void SetInnerBoard(const Board& board) {
        Boards.Inner = board;
    }
    void SetOuterBoard(const Board& board) {
        Boards.Outer = board;
    }
    void SetName(const std::string& name) { Name = name; }
    void SetGen(std::mt19937* generator) { Gen = generator; }
    Board GetOuter() { return Boards.Outer; }
    Board GetInner() { return Boards.Inner; }
    std::string GetName() { return Name; }
    std::mt19937 GetGenerator() { return *Gen; }
    friend std::istream& operator>>(std::istream& is, Board& board);
    friend std::ostream& operator<<(std::ostream& os, const Board& board);
    friend std::istream& operator>>(std::istream& is, Player& player);
    friend std::ostream& operator<<(std::ostream& os, const Player& player);
// TO STRING
    std::string InnerToString() {
        char letter = '@';
        size_t number = 0;
        std::string result;
        while (letter < 'K') {
            result += letter;
            ++letter;
        }
        result += '\n';
        for (const auto& i : Boards.Inner) {
            result += static_cast<char>('0' + number);
            for (const auto& k : i) {
                result += k;
            }
            result += '\n';
            ++number;
        }
        return result;
    }
    std::string OuterToString() {
        char letter = '@';
        size_t number = 0;
        std::string result;
        while (letter < 'K') {
            result += letter;
            ++letter;
        }
        result += '\n';
        for (const auto& i : Boards.Outer) {
            result += static_cast<char>('0' + number);
            for (const auto& k : i) {
                result += k;
            }
            result += '\n';
            ++number;
        }
        return result;
    }
    std::string TargetedToString(const Tile& pos) {
        char letter = '@';
        size_t number = 0;
        std::string result;
        while (letter < 'K') {
            result += letter;
            ++letter;
        }
        result += '\n';
        for (size_t i = 0; i < Boards.Inner.size(); ++i) {
            result += static_cast<char>(number - '0');
            for (size_t k = 0; k < Boards.Inner[i].size(); ++k) {
                if (pos.first == i && pos.second == k) {
                    result += 'T';
                } else {
                    result += Boards.Inner[i][k];
                }
            }
            result += '\n';
            ++number;
        }
        return result;
    }
// UTILITY
    std::string ShipName(const Tile& pos) {
        for (const auto& i : Ships)
            for (const auto& k : i.second.Positions)
                if (k == pos)
                    return i.first;
        return "";
    }
    size_t ShipsAmount(const size_t& size) {
        size_t amount = 0;
        for (const auto& i : Ships)
            if (size == i.second.Positions.size())
                ++amount;
        return amount;
    }
    size_t TotalHealth() {
        size_t health = 0;
        for (const auto& i : Ships)
            health += i.second.Health;
        return health;
    }
    size_t TotalShips() {
        size_t amount = 0;
        for (const auto& i : Ships)
            if (i.second.Health != 0)
                ++amount;
        return amount;
    }
// GENERATION
    void RandomScenario() {
        std::string name = "aaaa";
        char edge = 'b';
        size_t distance = 3;
        Tile spos = { 0, 0 };
        Tile epos = { 0, 0 };
        do
        {
            do
            {
                do
                {
                    spos = PickRand();
                    epos = ThrowRand(spos, distance);
                } while (epos.first == 10 || epos.second == 10);
                Ships.emplace(name, BuildShip(spos, epos));
                MarkShip(name);
                ++name.back();
            } while (name.back() != edge);
            name.resize(distance);
            --distance;
            ++edge;
        } while (distance < 4);
    }
    void Place(const Tile& spos, const Tile& epos) {
        std::string name;
        auto size = ShipSize(spos, epos);
        name.resize(size, 'a');
        name.back() += ShipsAmount(size);
        Ships.emplace(name, BuildShip(spos, epos));
        MarkShip(name);
    }
// BATTLE
    bool Shoot(const Tile& pos) {
        if (Boards.Inner[pos.first][pos.second] == 'X') {
            Boards.Outer[pos.first][pos.second] = 'S';
            auto name = ShipName(pos);
            --Ships[name].Health;
            if (Ships[name].Health == 0)
                Destroy(name);
            return true;
        }
        Boards.Outer[pos.first][pos.second] = 'x';
        return false;
    }
// CHECKS
    bool IsProperForPlacement(const Tile& pos) {
        if (IsOutOfBorders(pos))
            return false;
        for (size_t i = 0, x = pos.first - 1; i < 3; ++i, ++x) {
            if (x > 9)
                continue;
            for (size_t k = 0, y = pos.second - 1; k < 3; ++k, ++y) {
                if (y > 9)
                    continue;
                if (Boards.Inner[x][y] == 'X')
                    return false;
            }
        }
        return true;
    }
    bool IsNeeded(const Tile& spos, const Tile& epos) {
        size_t size = ShipSize(spos, epos);
        size_t amount = ShipsAmount(size);
        return amount + size < 5;
    }
    bool IsDead(const std::string& name) {
        return Ships[name].Health == 0;
    }
    bool IsLoser() {
        for (const auto& i : Ships)
            if (i.second.Health != 0)
                return false;
        return true;
    }
    bool WasShot(const Tile& pos) {
        return Boards.Outer[pos.first][pos.second] == 'x' ||
            Boards.Outer[pos.first][pos.second] == 'S' ||
            Boards.Outer[pos.first][pos.second] == 'D';
    }
// AI
    std::vector<Tile> GeneratePossible(const Tile& pos) {
        std::vector<Tile> possible;
        if (pos.second - 1 < 10u && !WasShot({ pos.first, pos.second - 1 }))
            possible.push_back({ pos.first, pos.second - 1 });
        if (pos.first - 1 < 10u && !WasShot({ pos.first - 1, pos.second }))
            possible.push_back({ pos.first - 1, pos.second });
        if (pos.second + 1 < 10u && !WasShot({ pos.first, pos.second + 1 }))
            possible.push_back({ pos.first, pos.second + 1 });
        if (pos.first + 1 < 10u && !WasShot({ pos.first + 1, pos.second }))
            possible.push_back({ pos.first + 1, pos.second });
        return possible;
    }
    std::pair<std::vector<Tile>, std::vector<Tile>> ShootRand() {
        if (IsLoser())
            return { {}, {} };
        std::uniform_int_distribution<> range(0, 9);
        Tile pos = { range(*Gen), range(*Gen) };
        while (WasShot(pos)) {
            pos = { range(*Gen), range(*Gen) };
        }
        if (Shoot(pos)) {
            if (Ships[ShipName(pos)].Health == 0)
                return ShootRand();
            auto possible = GeneratePossible(pos);
            return ContinueShooting(possible, { pos });
        }
        return { {}, {} };
    }
    std::pair<std::vector<Tile>, std::vector<Tile>> ContinueShooting(
        const std::vector<Tile>& possible, const std::vector<Tile>& hits) {
        size_t num = 0;
        if (possible.size() > 1) {
            std::uniform_int_distribution<> range(0, possible.size() - 1);
            num = range(*Gen);
        }
        std::vector<Tile> result;
        if (Shoot(possible[num])) {
            if (Ships[ShipName(possible[num])].Health == 0)
                return ShootRand();
            std::vector<Tile> sucess = hits;
            if (hits.front().first == possible[num].first) {
                if (possible[num].second > hits.back().second)
                    sucess.push_back(possible[num]);
                if (possible[num].second < hits.front().second)
                    sucess.insert(sucess.begin(), possible[num]);
                if (sucess.front().second - 1 < 10 &&
                    !WasShot({ sucess.front().first, sucess.front().second - 1 }))
                    result.push_back({ sucess.front().first, sucess.front().second - 1 });
                if (sucess.back().second + 1 < 10 &&
                    !WasShot({ sucess.back().first, sucess.back().second + 1 }))
                    result.push_back({ sucess.back().first, sucess.back().second + 1 });
                return ContinueShooting(result, sucess);
            }
            if (hits.front().second == possible[num].second) {
                if (possible[num].first > hits.back().first)
                    sucess.push_back(possible[num]);
                if (possible[num].first < hits.front().first)
                    sucess.insert(sucess.begin(), possible[num]);
                if (sucess.front().first - 1 < 10 &&
                    !WasShot({ sucess.front().first - 1, sucess.front().second }))
                    result.push_back({ sucess.front().first - 1, sucess.front().second });
                if (sucess.back().first + 1 < 10 &&
                    !WasShot({ sucess.back().first + 1, sucess.back().second }))
                    result.push_back({ sucess.back().first + 1, sucess.back().second });
                return ContinueShooting(result, sucess);
            }
        }
        result = possible;
        std::swap(result[num], result.back());
        result.pop_back();
        return { result, hits };
    }
};
