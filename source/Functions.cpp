#include "Header.hpp"

std::ostream& operator<<(std::ostream& os, const Board& Player)
{
    os << Player.name_ << std::endl;
    for (size_t i = 0; i < 11; ++i)
    {
        for (size_t j = 0; j < 11; ++j)
            os << ' ' << Player.clear_[i][j];
        os << std::endl;
    }
    for (size_t i = 0; i < 11; ++i)
    {
        for (size_t j = 0; j < 11; ++j)
            os << ' ' << Player.cloudy_[i][j];
        os << std::endl;
    }
    for (const auto& i : Player.positions_)
    {
        os << i.first << std::endl;
        for (size_t j = 0; j < i.first.size(); ++j)
            os << i.second[j].first << ' ' << i.second[j].second << std::endl;
    }
    for (const auto& i : Player.status_)
    {
        os << i.first << std::endl;
        os << i.second << std::endl;
    }
    os << Player.gen_.first << std::endl;
    return os;
}

std::istream& operator>>(std::istream& is, Board& Player)
{
    std::string tmp;
    std::string str;
    getline(is, Player.name_);
    for (size_t i = 0; i < 11; ++i)
    {
        getline(is, tmp);
        Trim(tmp);
        for (size_t j = 0; j < 11; ++j)
            Player.clear_[i][j] = tmp[j]; 
    }
    for (size_t i = 0; i < 11; ++i)
    {
        getline(is, tmp);
        Trim(tmp);
        for (size_t j = 0; j < 11; ++j)
            Player.cloudy_[i][j] = tmp[j];
    }
    for (size_t i = 0; i < 10; ++i)
    {
        getline(is, tmp);
        auto* arr = new std::pair<size_t, size_t>[tmp.size()];
        for (size_t k = 0; k < tmp.size(); ++k)
        {
            getline(is, str);
            arr[k] = DivideBySpace(str);
        }
        Player.positions_.emplace(tmp, arr);
    }
    for (size_t i = 0; i < 10; ++i)
    {
        getline(is, tmp);
        getline(is, str);
        Player.status_.emplace(tmp, std::stoi(str));
    }
    getline(is, str);
    Player.gen_.first = std::stoi(str);
    Player.gen_.second.seed(std::stoi(str));
    return is;
}

void Trim(std::string& str)
{
    std::remove(str.begin(), str.end(), ' ');
}

void SaveString(const std::string& str)
{
    std::ofstream out("TMP.txt");
    out << str;
    out << '$';
}

void SaveStageAndGen(const stages& stage, const size_t& seedy)
{
    std::ofstream out("STAGE.txt");
    out << stage << std::endl;
    out << seedy << std::endl;
}

void ClearFiles()
{
    remove("DATA.txt");
    remove("STAGE.txt");
    remove("TMP.txt");
}

std::string Cut(const std::string& str)
{
    return str.substr(str.find_first_not_of(' '), str.find(str.find_first_not_of(' '), ' '));
}

std::string LoadString()
{
    std::ifstream in("TMP.txt");
    std::string result;
    getline(in, result, '$');
    return result;
}

std::string ChooseRandomString(std::mt19937& gen, const std::vector<std::string>& quotes)
{
    std::uniform_int_distribution<> directions(0, quotes.size() - 1);
    auto num = directions(gen);
    return quotes[num];
}

std::pair<stages, size_t> ReadStageAndGen()
{
    std::ifstream in("STAGE.txt");
    if (in.peek() == std::ifstream::traits_type::eof())
        return{ NAME, static_cast<size_t>(time(0)) };
    std::string tmp1;
    getline(in, tmp1);
    std::string tmp2;
    return { static_cast<stages>(std::stoi(tmp1)), static_cast<size_t>(std::stoi(tmp2)) };
}

std::pair<size_t, size_t> GetPos(const std::string& request)
{
    std::string str = request;
    Trim(str);
    if (str.size() != 2)
        return { 0, 0 };
    if (str.front() < 'A' || str.front() > 'J' || str.back() < 'A' || str.back() > 'J')
        return { 0, 0 };
    return { str.front() - 64, str.back() - 64 };
}

std::pair<size_t, size_t> DivideBySpace(const std::string& str)
{
    std::pair <size_t, size_t> point = {
        std::stoi(str.substr(0, str.find(' '))),
        std::stoi(str.substr(str.find(' '), str.size()))
    };
    return point;
}

bool SameLine(const std::pair<size_t, size_t>& pos1, const std::pair<size_t, size_t>& pos2)
{
    return pos1.first == pos2.first || pos1.second == pos2.second;
}

bool CheckSize(const std::pair<size_t, size_t>& pos1, const std::pair<size_t, size_t>& pos2)
{
    if (pos1.first == pos2.first)
    {
        size_t size = std::abs(static_cast<int>(pos1.second - pos2.second)) + 1;
        return size > 0 && size < 5;
    }
    else if (pos1.second == pos2.second)
    {
        size_t size = std::abs(static_cast<int>(pos1.first - pos2.first)) + 1;
        return size > 0 && size < 5;
    }
    return false;
}
