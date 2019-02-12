#pragma once
#include "Player.hpp"
#include <fstream>
#include <cstdio>

enum stages { NAME, GREET, PLACEMENT, MANUAL, MANUAL1, MANUAL2, DECIDE1, DECIDE2, BEGIN, PLAYER, ALICE, TRYAGAIN};

std::string ChooseRandomString(std::mt19937& gen, const std::vector<std::string>& replics);
std::string Cut(const std::string& str);
Tile GetPos(const std::string& request);

void ClearFiles(const std::string& sessionId) {
    std::string tmp = sessionId + "STATE.txt";
    std::remove(tmp.c_str());
    tmp = sessionId + "DATA.txt";
    std::remove(tmp.c_str());
}

class State {
private:
    size_t Stage;
    std::string SessionId;
    struct Engine {
        size_t Seed;
        std::mt19937 Body;
    } Generator;

    void InitiateBegin(const std::string& sessionId) {
        Stage = NAME;
        SessionId = sessionId;
        Generator.Seed = static_cast<size_t>(time(0));
        Generator.Body.seed(Generator.Seed);
    }

public:
    State() {}

    State(const std::string& sessionId) {
        std::ifstream in(sessionId + "STATE.txt");
        if (!in.is_open() || in.peek() == std::ifstream::traits_type::eof())
            InitiateBegin(sessionId);
        std::string tmp;
        getline(in, tmp);
        Stage = static_cast<stages>(std::stoul(tmp));
        getline(in, tmp);
        SessionId = tmp;
        getline(in, tmp);
        Generator.Seed = std::stoul(tmp);
        Generator.Body.seed(Generator.Seed);
        in.close();
    }

    State(const size_t& stage,
        const size_t& seed,
        const std::string& sessionId) {
        Stage = stage;
        SessionId = sessionId;
        Generator.Body.seed(seed);
    }

    ~State() {
        std::ofstream out(SessionId + "STATE.txt");
        if (out.is_open()) {
            out << Stage << std::endl;
            out << SessionId << std::endl;
            out << Generator.Seed << std::endl;
        }
        out.close();
    }

    std::mt19937& Gen() { return Generator.Body; }

    std::mt19937* GetGen() { return &Generator.Body; }

    std::string GetSessionId() { return SessionId; }

    void SetStage(const size_t& stage) { Stage = stage; }

    size_t GetStage() { return Stage; }
};

class Case {
private:
    State* Core;
public:
    Player User;
    Player Bot;
    std::vector<Tile> Possible;
    std::vector<Tile> Hits;
    Tile Position;

    Case(State* core,
        const std::string& username) {
        Core = core;
        User.SetName(username);
        User.SetGen(Core->GetGen());
        Bot.SetName("Alice");
        Bot.SetGen(Core->GetGen());
        Bot.RandomScenario();
    }

    Case(State* core) {
        Core = core;
        ReadFromFile();
    }

    Case() {}

    ~Case() {
        SaveToFile();
    }

    void SaveToFile() {
        std::ofstream out(Core->GetSessionId() + "DATA.txt");
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
        std::ifstream in(Core->GetSessionId() + "DATA.txt");
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
