#include "Board.h"

Board::Board(std::string name)
{
    name_ = name;
    char** first = new char*[10];
    char** second = new char*[10];
    for (size_t i = 0; i < 10; ++i)
    {
        first[i] = new char[10];
        second[i] = new char[10];
        for (size_t j = 0; j < 10; ++j)
        {
            first[i][j] = '~';
            second[i][j] = '~';
        }
    }
    clear_ = first;
    cloudy_ = second;
}

Board::Board() : Board("Player")
{
}

Board::~Board()
{
    for (size_t i = 0; i < 10; ++i)
    {
        delete[] clear_[i];
        delete[] cloudy_[i];
    }
    delete[] clear_;
    delete[] cloudy_;
    for (auto& i : positions_)
    {
        delete[] i.second;
    }

}

std::string Board::GetName()
{
    return name_;
}

void Board::ShowMyBoard()
{
    for (size_t i = 0; i < 10; ++i)
    {
        if (i == 0)
        {
            std::cout << 'N';
            for (size_t j = 0; j < 10; ++j)
                std::cout << ' ' << j;
            std::cout << std::endl;
        }
        std::cout << i;
        for (size_t j = 0; j < 10; ++j)
            std::cout << ' ' << clear_[i][j];
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

void Board::ShowTargeted(const tile& pos)
{
    for (size_t i = 0; i < 10; ++i)
    {
        if (i == 0)
        {
            std::cout << 'N';
            for (size_t j = 0; j < 10; ++j)
                std::cout << ' ' << j;
            std::cout << std::endl;
        }
        std::cout << i;
        for (size_t j = 0; j < 10; ++j)
        {
            if (pos.first == j && pos.second == i)
            {
                std::cout << " T";
                continue;
            }
            std::cout << ' ' << clear_[i][j];
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

void Board::ShowEnemyBoard()
{
    for (size_t i = 0; i < 10; ++i)
    {
        if (i == 0)
        {
            std::cout << 'N';
            for (size_t j = 0; j < 10; ++j)
                std::cout << ' ' << j;
            std::cout << std::endl;
        }
        std::cout << i;
        for (size_t j = 0; j < 10; ++j)
            std::cout << ' ' << cloudy_[i][j];
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

bool Board::Check(const tile* pos)
{
    if (!IsProper(*pos))
        return false;
    for (const auto& i : positions_)
    {
        for (unsigned it = 0; it < i.first.size(); ++it)
        {
            if (i.second[it].first == pos->first - 1 ||
                i.second[it].first == pos->first ||
                i.second[it].first == pos->first + 1)
                if (i.second[it].second == pos->second - 1 ||
                    i.second[it].second == pos->second ||
                    i.second[it].second == pos->second + 1)
                    return false;
        }
    }
    return true;
}

tile* Board::PickRand()
{
    std::mt19937 gen(time(0));
    std::uniform_int_distribution<> directions(0, 9);
    auto* pos = new tile;
    *pos = { directions(gen), directions(gen) };
    while (!Check(pos))
        *pos = { directions(gen), directions(gen) };
    return pos;
}

tile* Board::ThrowRand(const tile* prev, const size_t& jump)
{ 
    auto* thrown = new tile;
    auto k = 0u;
    {
        std::mt19937 gen(time(0));
        std::uniform_int_distribution<> directions(0, 3);
        k = directions(gen);
    }
    for (size_t i = 0; i < 4; ++i)
    {
        switch (k)
        {
        case 0:  // left
            *thrown = { prev->first - jump, prev->second };
            break;
        case 1:  // right
            *thrown = { prev->first + jump, prev->second };
            break;
        case 2:  // up
            *thrown = { prev->first, prev->second + jump };
            break;
        case 3:  // down
            *thrown = { prev->first, prev->second - jump };
            break;
        }
        if (Check(thrown))
        {
            return thrown;
        }
        else
        {
            k = (k + 1) % 4;
        }
    }
    delete prev;
    delete thrown;
    return nullptr;
}

size_t Board::Size(const tile* spos, const tile* epos) 
{
    if (spos->first == epos->first)
    {
        return std::abs(static_cast<int>(spos->second - epos->second)) + 1;
    }
    else if (spos->second == epos->second)
    {
        return std::abs(static_cast<int>(spos->first - epos->first)) + 1;
    }
    return 0;
}

tile* Board::BuildHeap(const tile* spos, const tile* epos)
{
    auto size = Size(spos, epos);
    if (size == 0 || size > 4)
    {
        return nullptr;
    }
    auto* begin = new tile[size];
    begin[0] = *spos;
    begin[size - 1] = *epos;
    if (size == 1 || size == 2)
    {
    }
    else if (spos->first == epos->first)
    {
        if (spos->second > epos->second)
        {
            begin[1] = { epos->first, epos->second + 1 };
            if (size == 4)
                begin[2] = { epos->first, epos->second + 2 };
        }
        else if (spos->second < epos->second)
        {
            begin[1] = { epos->first, spos->second + 1 };
            if (size == 4)
                begin[2] = { epos->first, spos->second + 2 };
        }
    }
    else if (spos->second == epos->second)
    {
        if (spos->first > epos->first)
        {
            begin[1] = { epos->first + 1, epos->second };
            if (size == 4)
                begin[2] = { epos->first + 2, epos->second };
        }
        else if (spos->first < epos->first)
        {
            begin[1] = { spos->first + 1, epos->second };
            if (size == 4)
                begin[2] = { spos->first + 2, epos->second };
        }
    }
    delete spos;
    delete epos;
    return begin;
}

tile* Board::BuildStack(const tile& spos, const tile& epos)
{
    auto size = Size(&spos, &epos);
    if (size == 0 || size > 4)
    {
        return nullptr;
    }
    auto* begin = new tile[size];
    begin[0] = spos;
    begin[size - 1] = epos;
    if (size == 1 || size == 2)
    {
    }
    else if (spos.first == epos.first)
    {
        if (spos.second > epos.second)
        {
            begin[1] = { epos.first, epos.second + 1 };
            if (size == 4)
                begin[2] = { epos.first, epos.second + 2 };
        }
        else if (spos.second < epos.second)
        {
            begin[1] = { epos.first, spos.second + 1 };
            if (size == 4)
                begin[2] = { epos.first, spos.second + 2 };
        }
    }
    else if (spos.second == epos.second)
    {
        if (spos.first > epos.first)
        {
            begin[1] = { epos.first + 1, epos.second };
            if (size == 4)
                begin[2] = { epos.first + 2, epos.second };
        }
        else if (spos.first < epos.first)
        {
            begin[1] = { spos.first + 1, epos.second };
            if (size == 4)
                begin[2] = { spos.first + 2, epos.second };
        }
    }
    return begin;
}

void Board::RandomScenario()
{
    std::string name = "aaaa";
    auto edge = 'b';
    auto dst = 3;
    tile* spos;
    tile* epos;
    do
    {
        do
        {
            do
            {
                spos = PickRand();
                epos = ThrowRand(spos, dst);
            } while (epos == nullptr);
            positions_.emplace(name, BuildHeap(spos, epos));
            status_.emplace(name, dst + 1);
            ++name[name.size() - 1];
        } while (name[name.size() - 1] != edge);
        name.resize(dst);
        --dst;
        ++edge;
    } while (dst >= 0);
    Mark();
}

void Board::Mark()
{
    for (const auto& i : positions_)
    {
        for (size_t k = 0; k < i.first.size(); ++k)
        {
            clear_[i.second[k].second][i.second[k].first] = 'X';
        }
    }
}

void Board::MarkShip(const std::string& name)
{
    for (size_t i = 0; i < name.size(); ++i)
    {
        clear_[positions_[name][i].second][positions_[name][i].first] = 'X';
    }
}


void Board::Destroy(const std::string& name)
{
    auto pair = std::make_pair(0u, 0u);
    tile* target = &pair;
// First point
    for (size_t times = 0; times < 3; ++times)
    {
        target->first = positions_[name][0].first - 1 + times;
        if (target->first > 9)
            continue;
        target->second = positions_[name][0].second - 1;
        if (target->second < 10)
            cloudy_[target->second][target->first] = 'x';
        ++(target->second);
        cloudy_[target->second][target->first] = 'x';
        ++(target->second);
        if (target->second < 10)
            cloudy_[target->second][target->first] = 'x';
    }
// Last point
    if (name.size() != 1)
    {
        const auto last = name.size() - 1;
        for (size_t times = 0; times < 3; ++times)
        {
            target->first = positions_[name][last].first - 1 + times;
            if (target->first > 9)
                continue;
            target->second = positions_[name][last].second - 1;
            if (target->second < 10)
                    cloudy_[target->second][target->first] = 'x';
            ++(target->second);
            cloudy_[target->second][target->first] = 'x';
            ++(target->second);
            if (target->second < 10)
                cloudy_[target->second][target->first] = 'x';
        }
    }
    for (size_t i = 0; i < name.size(); ++i)
    {
        cloudy_[positions_[name][i].second][positions_[name][i].first] = 'X';
    }
    delete[] positions_[name];
    positions_.erase(name);
}

bool operator==(const tile& first, const tile& second)
{
    return first.first == second.first && first.second == second.second;
}

bool Board::IsDead(const std::string& name)
{
    if (status_[name] == 0)
    {
        return true;
    }
    return false;
}

std::string Board::GetShipName(const tile* pos)
{
    for (const auto& i : positions_)
    {
        for (size_t k = 0; k < i.first.size(); ++k)
        {
            if (*pos == i.second[k])
            {
                return i.first;
            }
        }
    }
    return "";
}

void Board::ShowStatus()
{
    std::string name = "a";
    char edge = 'e';
    do
    {
        std::cout << name << ' ' << static_cast<bool>(status_[name]) << std::endl;
        ++name[name.size() - 1];
        if (name[name.size() - 1] == edge)
        {
            name[name.size() - 1] = 'a';
            name = name + 'a';
            --edge;
        }
    } while (edge != 'a');
    std::cout << std::endl;
}

bool Board::WasShot(const tile& pos)
{
    return cloudy_[pos.second][pos.first] != '~';
}

bool Board::IsProper(const tile& pos)
{
    return pos.second < 10 && pos.first < 10;
}

bool Board::Shoot(const tile& pos)
{
    if (clear_[pos.second][pos.first] == 'X')
    {
        std::cout << "Hit!" << std::endl;
        auto name = GetShipName(&pos);
        --status_[name];
        cloudy_[pos.second][pos.first] = 'S';
        if (IsDead(name))
        {
            std::cout << "Ship is destroyed!" << std::endl;
            Destroy(name);
        }
        return true;
    }
    else
    {
        std::cout << "Miss!" << std::endl;
        cloudy_[pos.second][pos.first] = 'x';
        return false;
    }
}

bool Board::IsLooser()
{
    return positions_.empty();
}

bool Board::IsFull()
{
    return positions_.size() == 10u;
}

size_t Board::Amount(const size_t& size)
{
    auto amount = 0u;
    for (const auto& i : positions_)
    {
        if (i.first.size() == size)
            ++amount;
    }
    return amount;
}

bool Board::Place(const tile& spos, const tile& epos)
{
// Check 
    auto size = Size(&spos, &epos);
    if (!Check(&spos) || !Check(&epos) || size == 0 || size > 4)
    {
        std::cout << "Invalid positions" << std::endl;
        return false;
    }
    auto edge = 5u;
    auto amount = Amount(size);
    if (amount >= edge - size)
    {
        std::cout << "There is enough " << size << "-sized ships" << std::endl;
        return false;
    }
    std::string name = "";
// Placing
    for (size_t i = 0; i < size - 1; ++i)
    {
        name += 'a';
    }
    name += static_cast<char>('a' + amount);
    positions_.emplace(name, BuildStack(spos, epos));
    status_.emplace(name, size);
    MarkShip(name);
    return true;
}

std::vector<tile> Board::GeneratePossible(const tile& pos)
{
    std::vector<tile> possible = { pos };
    if (pos.second - 1 < 10u )
        if (!WasShot({ pos.first, pos.second - 1 }))
            possible.push_back({ pos.first, pos.second - 1 });
    if (pos.first - 1 < 10u)
        if (!WasShot({ pos.first - 1, pos.second }))
            possible.push_back({ pos.first - 1, pos.second });
    if (pos.second + 1 < 10u)
        if (!WasShot({ pos.first, pos.second + 1 }))
            possible.push_back({ pos.first, pos.second + 1 });
    if (pos.first + 1 < 10u)
        if (!WasShot({ pos.first + 1, pos.second }))
            possible.push_back({ pos.first + 1, pos.second });
    return possible;
}

std::pair<std::vector<tile>, size_t> Board::ContinueShooting(std::vector<tile>& positions, size_t& hits)
{
    size_t num = positions.size() - 1;
    if (num != hits)
    {
        std::mt19937 gen(time(0));
        std::uniform_int_distribution<> directions(hits, num);
        num = directions(gen);
    }
    if (Shoot(positions[num]))
    {
        if (IsDead(GetShipName(&positions[num])))
        {
            positions.clear();
            hits = 0;
            return ShootRand();
        }
        std::vector<tile> copy;
        if (positions[hits - 1].first == positions[num].first)
        {
            for (size_t i = 0; i < hits; ++i)  // Copies successfull positions. Here happens stack overflow
                copy.push_back(positions[i]);
            if (positions.front().second > positions[num].second)  // Sorts new position
                copy.insert(copy.begin(), positions[num]);
            else
                copy.push_back(positions[num]);
            tile tmp = { 0, 0 };
            if (copy.front().second - 1 < 10u)  // Adds edges of the line
            {
                tmp = { copy.front().first, copy.front().second - 1 };
                if (!WasShot(tmp))
                    copy.push_back(tmp);
            }
            if (copy.back().second + 1 < 10u)
            {
                tmp = { copy.back().first, copy.back().second + 1 };
                if (!WasShot(tmp))
                    copy.push_back(tmp);
            }
            ++hits;
            return ContinueShooting(copy, hits);
        }
        if (positions[hits - 1].second == positions[num].second)
        {
            for (size_t i = 0; i < hits; ++i)
                copy.push_back(positions[i]);
            if (positions.front().first > positions[num].first)
                copy.insert(copy.begin(), positions[num]);
            else
                copy.push_back(positions[num]);
            tile tmp = { 0, 0 };
            if (copy.front().first - 1 < 10u)
            {
                tmp = { copy.front().first - 1, copy.front().second };
                if (!WasShot(tmp))
                    copy.push_back(tmp);
            }
            if (copy.back().first + 1 < 10u)
            {
                tmp = { copy.back().first + 1, copy.back().second };
                if (!WasShot(tmp))
                    copy.push_back(tmp);
            }
            ++hits;
            return ContinueShooting(copy, hits);
        }
    }
    auto copy = positions;
    std::swap(copy.back(), copy[num]);
    copy.pop_back();
    return { copy, hits };
}

std::pair<std::vector<tile>, size_t> Board::ShootRand()
{
    tile pos;
    {
        std::mt19937 gen(time(0));
        std::uniform_int_distribution<> row(0, 9);
        pos = { row(gen), row(gen) };
        while (WasShot(pos))
            pos = { row(gen), row(gen) };
    }
    std::vector <tile> possible;
    if (Shoot(pos))
    {
        if (IsDead(GetShipName(&pos)))
            return ShootRand();
        auto possible = GeneratePossible(pos);
        auto hits = 1u;
        return ContinueShooting(possible, hits);
    }
    else 
    {
        std::vector <tile> possible;
        auto hits = 0u;
        return { possible, hits };
    }
}
