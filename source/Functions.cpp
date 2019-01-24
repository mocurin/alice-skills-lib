#include "Header.hpp"

void SaveStageAndGen(const stages& stage, const size_t& seedy) {
    std::ofstream out("data/STAGE.txt");
    if (out.is_open()) {
        out << stage << std::endl;
        out << seedy << std::endl;
    }
    out.close();
}

std::pair<stages, size_t> ReadStageAndGen() {
    std::ifstream in("data/STAGE.txt");
    if (!in.is_open())
    {
        std::random_device rd;
        return { NAME, static_cast<size_t>(rd()) };
    }
    if (in.peek() == std::ifstream::traits_type::eof())
    {
        std::random_device rd;
        return { NAME, static_cast<size_t>(rd()) };
    }
    std::string tmp1;
    getline(in, tmp1);
    std::string tmp2;
    in.close();
    return { static_cast<stages>(std::stoi(tmp1)), std::stoi(tmp2) };
}

std::string ChooseRandomString(std::mt19937& gen, const std::vector<std::string>& replics) {
    std::uniform_int_distribution<> directions(0, replics.size() - 1);
    auto num = directions(gen);
    return replics[num];
}

std::string Cut(const std::string& str)
{
    return str.substr(str.find_first_not_of(' '), str.find(str.find_first_not_of(' '), ' '));
}

Tile DivideBySpace(const std::string& str) {
    Tile point = {
        std::stoi(str.substr(0, str.find(' '))),
        std::stoi(str.substr(str.find(' ') + 1, str.size()))
    };
    return point;
}

Tile GetPos(const std::string& request)
{
    std::string str = Trim(request);
    if (str.size() != 2)
        return { 10, 10 };
    if (str.front() < 'A' || str.front() > 'J' || str.back() < '0' || str.back() > '9')
        return { 10, 10 };
    return { str.front() - 'A', str.back() - '0' };
}

void ClearFiles()
{
    std::ofstream out1("DATA.txt");
    out1.close();
    std::ofstream out2("STAGE.txt");
    out2.close();
    std::ofstream out3("TMP.txt");
    out3.close();
}
