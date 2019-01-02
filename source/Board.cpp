#include "Board.hpp"

Board::Board(std::string name)
{
    name_ = name;
    char** first = new char*[11];
    char** second = new char*[11];
    char ch = 'A';
    for (size_t i = 0; i < 11; ++i)
    {
        first[i] = new char[11];
        second[i] = new char[11];
        for (size_t j = 0; j < 11; ++j)
        {
            if (i == 0 && j == 0)
            {
                first[i][j] = 'N';
                second[i][j] = 'N';
            }
            else if (i == 0)
            {
                first[i][j] = ch;
                second[i][j] = ch;
                ++ch;
                if (j == 10)
                    ch = 'A';
            }
            else if (j == 0)
            {
                first[i][j] = ch;
                second[i][j] = ch;
                ++ch;
            }
            else
            {
                first[i][j] = '~';
                second[i][j] = '~';
            }
        }
    }
    clear_ = first;
    cloudy_ = second;
    std::mt19937 ngen(time(0));
    gen_ = ngen;
}

Board::Board() : Board("Player")
{
}

Board::~Board()
{
    for (size_t i = 0; i < 11; ++i)
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
    for (size_t i = 0; i < 11; ++i)
    {
        for (size_t j = 0; j < 11; ++j)
            std::cout << ' ' << clear_[i][j];
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

void Board::ShowTargeted(const tile& pos)
{
    for (size_t i = 0; i < 11; ++i)
    {
        for (size_t j = 0; j < 11; ++j)
        {
            if (pos.first == j && pos.second == i)
                std::cout << " T";
            else
                std::cout << ' ' << clear_[i][j];
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

void Board::ShowEnemyBoard()
{
    for (size_t i = 0; i < 11; ++i)
    {
        for (size_t j = 0; j < 11; ++j)
            std::cout << ' ' << cloudy_[i][j];
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

std::string Board::MyBoardToString()
{
    std::string matrix;
    for (size_t i = 0; i < 11; ++i)
    {
        for (size_t j = 0; j < 11; ++j)
            matrix += ' ' + clear_[i][j];
        matrix += '\n';
    }
    matrix += '\n';
    return matrix;
}

std::string Board::TargetedToString(const tile& pos)
{
    std::string matrix;
    for (size_t i = 0; i < 11; ++i)
    {
        for (size_t j = 0; j < 11; ++j)
        {
            if (pos.first == j && pos.second == i)
                matrix += " T";
            else
                matrix += ' ' + clear_[i][j];
        }
        matrix += '\n';
    }
    matrix += '\n';
    return matrix;
}

std::string Board::EnemyBoardToString()
{
    std::string matrix;
    for (size_t i = 0; i < 11; ++i)
    {
        for (size_t j = 0; j < 11; ++j)
            matrix += ' ' + cloudy_[i][j];
        matrix += '\n';
    }
    matrix += '\n';
    return matrix;
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
    std::uniform_int_distribution<> row(1, 10);
    auto* pos = new tile;
    *pos = { row(gen_), row(gen_) };
    while (!Check(pos))
        *pos = { row(gen_), row(gen_) };
    return pos;
}

tile* Board::ThrowRand(const tile* prev, const size_t& jump)
{ 
    auto* thrown = new tile;
    auto k = 0u;
    std::uniform_int_distribution<> directions(0, 3);
    k = directions(gen_);
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
    tile pair = { 0, 0 };
    // First point
    for (size_t times = 0; times < 3; ++times)
    {
        pair.first = positions_[name][0].first - 1 + times;
        // Checks if Y is out of borders
        if (pair.first < 1 || pair.first > 10)
            continue;
        pair.second = positions_[name][0].second - 1;
        // Checks if X is out of borders
        if (pair.second > 0 && pair.second < 11)
            cloudy_[pair.second][pair.first] = 'x';
        ++(pair.second);
        // If the row wasnt thrown out in the beiginnig middle position should always be OK, therefore no checks are needed;
        cloudy_[pair.second][pair.first] = 'x';
        ++(pair.second);
        if (pair.second > 0 && pair.second < 11)
            cloudy_[pair.second][pair.first] = 'x';
    }
    // Last point. Literally the same way as the first. Happens only if ship isn`t 1 sized
    if (name.size() != 1)
    {
        const size_t last = name.size() - 1;
        for (size_t times = 0; times < 3; ++times)
        {
            pair.first = positions_[name][last].first - 1 + times;
            if (pair.first < 1 || pair.first > 10)
                continue;
            pair.second = positions_[name][last].second - 1;
            if (pair.second > 0 && pair.second < 11)
                cloudy_[pair.second][pair.first] = 'x';
            ++(pair.second);
            cloudy_[pair.second][pair.first] = 'x';
            ++(pair.second);
            if (pair.second > 0 && pair.second < 11)
                cloudy_[pair.second][pair.first] = 'x';
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

std::string Board::StatusToString()
{
    std::string text;
    std::string name = "a";
    char edge = 'e';
    do
    {
        text += name;
        text += ' ';
        text += static_cast<bool>(status_[name]);
        text += '\n';
        ++name[name.size() - 1];
        if (name[name.size() - 1] == edge)
        {
            name[name.size() - 1] = 'a';
            name = name + 'a';
            --edge;
        }
    } while (edge != 'a');
    text += '\n';
    return text;
}

bool Board::WasShot(const tile& pos)
{
    return cloudy_[pos.second][pos.first] != '~';
}

bool Board::IsProper(const tile& pos)
{
    return pos.second < 11 && pos.second > 0 && pos.first < 11 && pos.first > 0;
}

bool Board::Shoot(const tile& pos)
{
    if (clear_[pos.second][pos.first] == 'X')
    {
        auto name = GetShipName(&pos);
        --status_[name];
        cloudy_[pos.second][pos.first] = 'S';
        if (IsDead(name))
        {
            Destroy(name);
        }
        return true;
    }
    else
    {
        cloudy_[pos.second][pos.first] = 'x';
        return false;
    }
}

size_t Board::ShipsAmount()
{
    return positions_.size();
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

std::string Board::ComplexCheck(const tile& spos, const tile& epos)
{
    std::string error_text;
    auto size = Size(&spos, &epos);
    if (!Check(&spos) || !Check(&epos) || size == 0 || size > 4)
    {
        error_text += "Неподходящая позиция\n";
        return error_text;
    }
    auto edge = 5u;
    auto amount = Amount(size);
    if (amount >= edge - size)
    {
        error_text += "Достаточно ";
        error_text += size;
        error_text += "-палубных кораблей\n";
        return error_text;
    }
    return error_text;
}

bool Board::Place(const tile& spos, const tile& epos)
{
    auto size = Size(&spos, &epos);
    auto amount = Amount(size);
    std::string name;
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
    if (pos.second - 1 > 0u)
        if (!WasShot({ pos.first, pos.second - 1 }))
            possible.push_back({ pos.first, pos.second - 1 });
    if (pos.first - 1 > 0u)
        if (!WasShot({ pos.first - 1, pos.second }))
            possible.push_back({ pos.first - 1, pos.second });
    if (pos.second + 1 < 11u)
        if (!WasShot({ pos.first, pos.second + 1 }))
            possible.push_back({ pos.first, pos.second + 1 });
    if (pos.first + 1 < 11u)
        if (!WasShot({ pos.first + 1, pos.second }))
            possible.push_back({ pos.first + 1, pos.second });
    return possible;
}

std::pair<std::vector<tile>, size_t> Board::ContinueShooting(std::vector<tile>& positions, size_t& hits)
{
    size_t num = positions.size() - 1;
    if (num != hits)
    {
        std::uniform_int_distribution<> directions(hits, num);
        num = directions(gen_);
    }
    auto name = GetShipName(&positions[num]);
    std::vector<tile> copy;
    if (Shoot(positions[num]))
    {
        if (IsDead(name))
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
    copy = positions;
    std::swap(copy.back(), copy[num]);
    copy.pop_back();
    return { copy, hits };
}

std::pair<std::vector<tile>, size_t> Board::ShootRand()
{
    tile pos;
    std::uniform_int_distribution<> row(1, 10);
    pos = { row(gen_), row(gen_) };
    while (WasShot(pos))
        pos = { row(gen_), row(gen_) };
    std::vector <tile> possible;
    if (Shoot(pos))
    {
        if (IsDead(GetShipName(&pos)))
            return ShootRand();
        auto possible = GeneratePossible(pos);
        size_t hits = 1;
        return ContinueShooting(possible, hits);
    }
    else 
    {
        std::vector <tile> possible;
        size_t hits = 0;
        return { possible, hits };
    }
}

size_t Board::TotalHealth()
{
    size_t total = 0;
    for (const auto& i : status_)
    {
        total += i.second;
    }
    return total;
}

std::mt19937 Board::GetRdGen()
{
    return gen_;
}
