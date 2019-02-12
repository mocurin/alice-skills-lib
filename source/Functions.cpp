#include "Header.hpp"

std::string ChooseRandomString(std::mt19937& gen, const std::vector<std::string>& replics) {
    std::uniform_int_distribution<> directions(0, replics.size() - 1);
    auto num = directions(gen);
    return replics[num];
}

std::string Cut(const std::string& str)
{
    return str.substr(str.find_first_not_of(' '), str.find(str.find_first_not_of(' '), ' '));
}

Tile GetPos(const std::string& request)
{
    std::string str = Trim(request);
    if (str.size() != 2 ||
        str.front() < 'A' || str.front() > 'J' || str.back() < '0' || str.back() > '9')
        return { 10, 10 };
    return { str.front() - 'A', str.back() - '0' };
}
