#include "Board.hpp"
#include <Request.hpp>
#include <Response.hpp>
#include <Skill.hpp>

std::string Cut(const std::string& request)
{
    auto tmp = request.substr(0, request.find(' '));
    return tmp;
}

std::pair<size_t, size_t> GetPos(const std::string& request)
{
    size_t flag = 0;
    std::pair<size_t, size_t> done = { 10, 10 };
    for (const auto& i : request)
    {
        if (isdigit(i))
        {
            ++flag;
            done.first = i - '0';
        }
        if (flag == 2)
            break;
    }
    return done;
}

bool IsPlaced(Board& Player, Alice::Response& response, const std::pair<size_t, size_t>& pos1, const std::pair<size_t, size_t>& pos2)
{
    std::string check = Player.ComplexCheck(pos1, pos2);
    if (check.empty())
    {
        Player.Place(pos1, pos2);
        return true;
    }
    response.SetText(check);
    response.SetTts(check);
    return false;
}

bool IsProperPos(Board& Player, Alice::Response& response, const std::pair<size_t, size_t>& pos)
{
    if (!Player.IsProper(pos))
    {
        response.SetText("Позиция находится вне карты\n");
        response.SetTts("Позиция находится вне карты\n");
        return true;
    }
    if (Player.WasShot(pos))
    {
        response.SetText("В эту позицию уже стреляли\n");
        response.SetTts("В эту позицию уже стреляли\n");
        return true;
    }
    return false;
}

bool IsShot(Board& Player, Alice::Response& response, const std::pair<size_t, size_t>& pos)
{
    if (Player.Shoot(pos))
    {
        if (Player.IsDead(Player.GetShipName(&pos)))
        {
            response.SetText("Корабль уничтожен!\n");
            response.SetTts("Корабль уничтожен!\n");
            return true;
        }
        response.SetText("Попадание!\n");
        response.SetTts("Попадание!\n");
        return true;
    }
    response.SetText("Промах!\n");
    response.SetTts("Промах!\n");
    return false;
}

void MyCallback(const Alice::Request& request, Alice::Response& response)
{
    std::string tmp;
    do
    {
        response.SetText("Как вас зовут?\n");
        response.SetTts("Как вас зовут?\n");
        tmp = Cut(request.Command());
        tmp.front() = toupper(tmp.front());
        Board Player(tmp);
        std::pair<size_t, size_t> pos1 = { 0, 0 };
        auto pos2 = pos1;
        response.SetText("Выберите как расставлять корабли:\n0 - Вручную\n1 - Случайно\n");
        response.SetTts("Выберите как расставлять корабли:\n0 - Вручную\n1 - Случайно\n");
        do
            tmp = Cut(request.Command());
        while (tmp != "вручную" && tmp != "случайно");
        switch (tmp.front())
        {
        case 'в':
            response.SetText("Введите первую и последнюю позицию корабля\n");
            response.SetTts("Введите первую и последнюю позицию корабля\n");
            do
            {
                do
                {
                    response.SetText(Player.MyBoardToString());
                    response.SetText("Первая позиция:");
                    response.SetTts("Первая позиция:");
                    pos1 = GetPos(request.Command());
                    response.SetText(Player.TargetedToString(pos1));
                    response.SetText("Вторая позиция:");
                    response.SetTts("Вторая позиция:");
                    pos2 = GetPos(request.Command());
                } while (!IsPlaced(Player, response, pos1, pos2));
            } while (!Player.IsFull());
            break;
        case 'с':
            Player.RandomScenario();
            response.SetText(Player.MyBoardToString());
        }
        Board Bot("Alice");
        Bot.RandomScenario();
        std::pair<std::vector<std::pair<size_t, size_t>>, size_t> tmp_positions = { {}, 0 };
        size_t Health = Player.TotalHealth();
        do
        {
            do
            {
                do
                {
                    response.SetText(Bot.EnemyBoardToString());
                    response.SetText("Куда стрелять?\n");
                    response.SetTts("Куда стрелять?\n");
                    pos1 = GetPos(request.Command());
                } while (IsProperPos(Bot, response, pos1));
            } while (IsShot(Bot, response, pos1));
            response.SetText("Моя очередь!\n");
            response.SetTts("Моя очередь!\n");
            if (tmp_positions.second == 0)
            {
                tmp_positions = Player.ShootRand();
                response.SetText(Player.EnemyBoardToString());
            }
            else
            {
                tmp_positions = Player.ContinueShooting(tmp_positions.first, tmp_positions.second);
                Player.ShowEnemyBoard();
            }
            if (Health - Player.TotalHealth() == 0)
            {
                response.SetText("Обидно\n");
                response.SetTts("Обидно\n");
            }
            else if (Health - Player.TotalHealth() > 0)
            {
                response.SetText("Кажется попала!\n");
                response.SetTts("Кажется попала!\n");
            }
        } while (!Bot.IsLooser() || !Player.IsLooser());
        if (Bot.IsLooser())
            tmp = Player.GetName() + " wins!\n";
        else
            tmp = "Я победила!\n";
        response.SetText(tmp);
        response.SetTts(tmp);
        response.SetText("Хочешь сыграем еще?\n");
        response.SetTts("Хочешь сыграем еще?\n");
        tmp = request.Command();
    } while (tmp == "го" || tmp == "хочу");
    response.SetEndSession(true);
}

int main()
{
    Skill battleships;
    battleships.SetCallback(MyCallback);
    battleships.Run();
    return 0;
}
