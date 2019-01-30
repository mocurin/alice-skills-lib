#pragma once
#include "Player.hpp"
#include <fstream>

enum stages { NAME, GREET, PLACEMENT, MANUAL, MANUAL2, DECIDE, BEGIN, PLAYER, ALICE, TRYAGAIN };

const std::string folder = "data/";

void SaveStageAndGen(const stages& stage, const size_t& seedy);
std::pair<stages, size_t> ReadStageAndGen();
std::string ChooseRandomString(std::mt19937& gen, const std::vector<std::string>& replics);
std::string Cut(const std::string& str);
Tile DivideBySpace(const std::string& str);
Tile GetPos(const std::string& request);
void ClearFiles();

class Case {
private:
    Player User;
    Player Bot;
    size_t Stage;
    Tile Position;
    struct Engine {
        size_t Seed;
        std::mt19937 Body;
    } Generator;
    std::vector<Tile> Possible;
    std::vector<Tile> Hits;
public:
    Case(const std::pair<stages, size_t>& info, const std::string& username) {
        Stage = info.first;
        SetGen(info.second);
        User.SetName(username);
        User.SetGen(&Generator.Body);
        Bot.SetName("Alice");
        Bot.SetGen(&Generator.Body);
        Bot.RandomScenario();
        Position = { 0u, 0u };
    }
    Case() {
        auto pair = ReadStageAndGen();
        Stage = pair.first;
        SetGen(pair.second);
        User.SetGen(&Generator.Body);
        Bot.SetGen(&Generator.Body);
        ReadFromFile();
    }
    ~Case() {
        SaveStageAndGen();
        SaveToFile();
    }
    Player& GetUser() { return User; }
    Player& GetBot() { return Bot; }
    Tile GetPos() { return Position; }
    std::mt19937& GetGen() { return Generator.Body; }
    std::vector<Tile> GetPossible() {
        return Possible;
    }
    std::vector<Tile> GetHits() {
        return Hits;
    }
    void SetSequence(const std::pair<std::vector<Tile>, std::vector<Tile>>& seq) {
        Possible = seq.first;
        Hits = seq.second;
    }
    void SetStage(const stages& stage) { Stage = stage; }
    void SetGen(const size_t& seed) {
        std::mt19937 generator(seed);
        Generator = { seed, generator };
    }
    void SetPos(const std::pair<size_t, size_t>& pos) { Position = pos; }
    void SaveStageAndGen()
    {
        std::ofstream out(folder + "STAGE.txt");
        out << Stage << std::endl;
        out << Generator.Seed << std::endl;
        out.close();
    }
    void SaveToFile() {
        std::ofstream out(folder + "DATA.txt");
        if (out.is_open()) {
            out << User;
            out << Bot;
            out << Possible.size() << std::endl;
            for (const auto& i : Possible)
            {
                out << i.first << i.second << std::endl;
            }
            out << Hits.size() << std::endl;
            for (const auto& i : Hits)
            {
                out << i.first << i.second << std::endl;
            }
            out << Position.first << Position.second << std::endl;
        }
        out.close();
    }
    void ReadFromFile() {
        std::ifstream in(folder + "DATA.txt");
        if (in.is_open()) {
            in >> User;
            in >> Bot;
            std::string str;
            getline(in, str);
            Possible.resize(std::stoi(str));
            for (auto& i : Possible)
            {
                getline(in, str);
                i = StrToPair(str);
            }
            getline(in, str);
            Hits.resize(std::stoi(str));
            for (auto& i : Hits)
            {
                getline(in, str);
                i = StrToPair(str);
            }
            getline(in, str);
            Position = StrToPair(str);
        }
        in.close();
    }
};
