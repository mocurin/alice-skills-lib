#pragma once
#include "Board.hpp"
#include <Request.hpp>
#include <Response.hpp>
#include <Skill.hpp>
#include <algorithm>
#include <fstream>

enum stages { NAME, GREET, PLACEMENT, MANUAL, MANUAL2, INVALID, INVALID2, DECIDE, BEGIN, PLAYER, ALICE, TRYAGAIN };

std::pair<stages, size_t> ReadStageAndGen();
void SaveStageAndGen(const stages& stage, const size_t& seedy);
void SaveString(const std::string& str);
void ClearFiles();
void Trim(std::string& str);
std::string LoadString();
std::string Cut(const std::string& str);
std::string ChooseRandomString(const std::mt19937& gen, const std::vector<std::string>& quotes);
std::pair<size_t, size_t> DivideBySpace(const std::string& str);
std::pair<size_t, size_t> GetPos(const std::string& request);
bool SameLine(const std::pair<size_t, size_t>& pos1, const std::pair<size_t, size_t>& pos2);
bool CheckSize(const std::pair<size_t, size_t>& pos1, const std::pair<size_t, size_t>& pos2);

class Case
{
private:
    std::size_t stage_;
    Board player_;
    Board bot_;
    std::pair<size_t, size_t> pos_;
    std::pair<size_t, std::mt19937> gen_;
    std::pair<std::vector<std::pair<size_t, size_t>>, size_t> tracking_;
public:
    Case(const std::pair<stages, size_t>& info, const std::string& tmp)
    {
        Board Player(tmp, info.second);
        player_ = Player;
        Board Bot("Alice", info.second);
        Bot.RandomScenario();
        bot_ = Bot;
        pos_ = { 0, 0 };
        stage_ = info.first;
        SetGen(info.second);
        tracking_ = { {}, 0 };
    }

    Case()
    {
        auto pair = ReadStageAndGen();
        stage_ = pair.first;
        SetGen(pair.second);
        ReadFromFile();
    }

    ~Case()
    {
        SaveStageAndGen();
        SaveToFile();
    }

    Board& GetPlayer() { return player_; }

    Board& GetBot() { return bot_; }

    std::pair<std::vector<std::pair<size_t, size_t>>, size_t>& GetTracking() { return tracking_; }

    std::pair<size_t, size_t>& GetPos() { return pos_; }

    std::mt19937& GetGen() { return gen_.second; }

    void SetStage(const stages& stage)
    {
        stage_ = stage;
    }

    void SetGen(const size_t& seedy)
    {
        std::mt19937 gen(seedy);
        gen_ = { seedy, gen };
    }

    void SetPos(const std::pair<size_t, size_t>& pos) { pos_ = pos; }

    void SaveStageAndGen()
    {
        std::ofstream out("STAGE.txt");
        out << stage_ << std::endl;
        out << gen_.first << std::endl;
    }

    void SaveToFile()
    {
        std::ofstream out("DATA.txt");
        out << player_;
        out << bot_;
        out << pos_.first << ' ' << pos_.second << std::endl;
        out << tracking_.first.size() << std::endl;
        for (const auto& i : tracking_.first)
        {
            out << i.first << ' ' << i.second << std::endl;
        }
        out << tracking_.second << std::endl;
    }

    void ReadFromFile()
    {
        std::ifstream in("DATA.txt");
        in >> player_;
        in >> bot_;
        std::string str;
        getline(in, str);
        pos_ = DivideBySpace(str);
        getline(in, str);
        size_t size = std::stoi(str);
        if (size == 0)
        {
            tracking_.first = {};
        }
        for (size_t i = 0; i < size; ++i)
        {
            getline(in, str);
            tracking_.first.push_back(DivideBySpace(str));
        }
        getline(in, str);
        tracking_.second = std::stoi(str);
    }
};
