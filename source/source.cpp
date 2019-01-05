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
    std::pair<size_t, size_t> done = { 0, 0 };
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

std::string ChooseRandomString(std::mt19937& gen, const std::vector<std::string>& replics)
{
    std::uniform_int_distribution<> directions(0, replics.size() - 1);
    auto num = directions(gen);
    return replics[num];
}

void MyCallback(const Alice::Request& request, Alice::Response& response)
{
    std::mt19937 gen(time(0));
    std::string tmp;
    do
    {
        tmp = ChooseRandomString(gen,
            { "Как вас зовут?\n",
            "Имя?\n",
            "Кэп, ваше имя?\n",
            "Как вас называть?\n",
            "Как к вам обращаться?\n",
            "Ваше имя?\n"
            });
        response.SetText(tmp);
        response.SetTts(tmp);
        tmp = Cut(request.Command());
        tmp.front() = toupper(tmp.front());
        Board Player(tmp);
        tmp = "Приятно с вами познакомиться, " + tmp + '\n';
        response.SetText(tmp);
        response.SetTts(tmp);
        tmp = ChooseRandomString(gen,
            { "Как будем расставлять корабли? Вручную или случайно?\n",
            "Выберите как расставлять корабли:\n вручную или случайно\n",
            "Я могу помочь расставить ваши корабли вручную или сделать это случайно. Что выберете?\n",
            "Вы можете расставить корабли вручную или позволить мне сделать это случайным образом. Что выберете?\n",
            "Корабли можно расставить вручную или случайно. Какой вариант вам больше подходит?\n"
            });
        response.SetText(tmp);
        response.SetTts(tmp);
        std::pair<size_t, size_t> pos1 = { 0, 0 };
        auto pos2 = pos1;
        {
            size_t variant = 0;
            do
            {
                tmp = request.Command();
                if (tmp.find("вручн") != std::string::npos || tmp.find("перв") != std::string::npos)
                {
                    variant = 1;
                    break;
                }
                if (tmp.find("случайн") != std::string::npos || tmp.find("втор") != std::string::npos)
                {
                    variant = 2;
                    break;
                }
                tmp = ChooseRandomString(gen,
                    { "Ничего не поняла. Можно еще раз?\n",
                    "Сложно, ничего не понимаю. Повторите еще раз?\n",
                    "Что-что? Можно еще раз?\n",
                    "Так вручную или случайно?\n",
                    "Еще разок можно?\n",
                    "Кажется, я чего-то не понимаю, можно еще раз?\n"
                    });
                response.SetText(tmp);
                response.SetTts(tmp);
            } while (true);
            switch (variant)
            {
            case 1:
                tmp = ChooseRandomString(gen,
                    { "Говорите позиции первой и последней клетки корабля пока не заполните поле. Если вдруг не знаете, кораблей должно быть десять\n",
                    "Мне будут нужны первая и последняя клетка корабля. Нужно будет задать их десять раз чтобы заполнить поле\n"
                    "Координат первой и последней клетки корабля будет достаточно чтобы создать его. Напоминаю, кораблей в конечном итоге будет 10\n"
                    });
                response.SetText(tmp);
                response.SetTts(tmp);
                do
                {
                    tmp = 10 - Player.ShipsAmount();
                    tmp = "Осталось " + tmp + " кораблей\n";
                    response.SetText(tmp);
                    response.SetTts(tmp);
                    do
                    {
                        response.SetText(Player.MyBoardToString());
                        response.SetText("Первая позиция");
                        response.SetTts("Первая позиция");
                        do
                        {
                            pos1 = GetPos(request.Command());
                            if (pos1.first != 0 || pos1.second != 0)
                                break;
                            tmp = ChooseRandomString(gen,
                                { "Еще раз?\n",
                                "Сложно. Еще раз?\n",
                                "Что-что?\n",
                                "Мне просто нужны цифры\n",
                                "Еще разок можно?\n",
                                "А? Я прослушала, простите\n",
                                "Если у нас такие проблемы с первой, то что будет со второй?\n"
                                });
                            response.SetText(tmp);
                            response.SetTts(tmp);
                        } while (true);
                        response.SetText(Player.TargetedToString(pos1));
                        response.SetText("Вторая позиция");
                        response.SetTts("Вторая позиция");
                        do
                        {
                            pos2 = GetPos(request.Command());
                            if (pos2.first != 0 || pos2.second != 0)
                                break;
                            tmp = ChooseRandomString(gen,
                                { "Еще раз?\n",
                                "Сложно. Еще раз?\n",
                                "Что-что?\n",
                                "Мне просто нужны цифры\n",
                                "Еще разок можно?\n",
                                "А? Я прослушала, простите\n",
                                "В первый раз лучше получилось\n"
                                });
                            response.SetText(tmp);
                            response.SetTts(tmp);
                        } while (true);
                    } while (!IsPlaced(Player, response, pos1, pos2));
                } while (!(Player.ShipsAmount() == 10));
                break;
            case 2:
                tmp = ChooseRandomString(gen,
                    { "Итак, вы выбрали легкий путь\n",
                    "Это экономит время и нервы, спасибо\n",
                    "Правильное решение\n",
                    "Так все становится проще\n",
                    "На секунду я подумала что их придется ставить вручную\n"
                    });
                response.SetText(tmp);
                response.SetTts(tmp);
                Player.RandomScenario();
                response.SetText(Player.MyBoardToString());
            }
        }
        tmp = ChooseRandomString(gen,
            { "Я постараюсь не жульничать\n",
            "Обещаю что не буду жульничать\n",
            "Не беспокойтесь, я ничего не запомнила\n",
            "Сейчас я забуду где стоят все ваши корабли и мы начнем\n"
            });
        response.SetText(tmp);
        response.SetTts(tmp);
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
                    tmp = ChooseRandomString(gen,
                        { "Куда стрелять?\n",
                        "Куда стреляем?\n",
                        "На какую позицию стреляем?\n"
                        });
                    response.SetText(tmp);
                    response.SetTts(tmp);
                    do
                    {
                        pos1 = GetPos(request.Command());
                        if (pos1.first != 0 || pos1.second != 0)
                            break;
                        tmp = ChooseRandomString(gen,
                            { "Еще раз?\n",
                            "Сложно. Еще раз?\n",
                            "Что-что?\n",
                            "А стрелять-то куда?\n",
                            "Еще разок можно?\n",
                            "А? Я прослушала, простите\n",
                            "Что?\n"
                            });
                        response.SetText(tmp);
                        response.SetTts(tmp);
                    } while (true);
                } while (IsProperPos(Bot, response, pos1));
            } while (IsShot(Bot, response, pos1));
            tmp = ChooseRandomString(gen,
                { "Моя очередь!\n",
                "Теперь я стреляю!\n",
                "Куда бы ударить?\n",
                "Наконец моя очередь\n",
                "Попаду или промажу?\n"
                });
            response.SetText(tmp);
            response.SetTts(tmp);
            int amount = Player.ShipsAmount();
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
                tmp = ChooseRandomString(gen,
                    { "Как так?\n",
                    "Обидно\n",
                    ":(\n",
                    "Что ж такое-то\n",
                    "Промах\n"
                    "В следующий раз повезет. Наверное\n"
                    });
                response.SetText(tmp);
                response.SetTts(tmp);
            }
            else if (Health - Player.TotalHealth() > 0)
            {
                tmp = ChooseRandomString(gen,
                    { "Попала!\n",
                    "Кажется я попала!\n",
                    "Мне везет\n",
                    "Всегда бы так\n"
                    });
                response.SetText(tmp);
                response.SetTts(tmp);
                if (amount - Player.ShipsAmount() == 1)
                {
                    tmp = ChooseRandomString(gen,
                        { "Корабль уничтожен!\n",
                        "Даже один убит!\n",
                        "Один корабль разрушен\n"
                        });
                }
                else if (amount - Player.ShipsAmount() == 2)
                {
                    tmp = ChooseRandomString(gen,
                        { "Ого, целых два!\n",
                        "Да я в ударе!\n",
                        "Такое бывает?\n"
                        });
                }
                else if (amount - Player.ShipsAmount() > 2)
                {
                    tmp = ChooseRandomString(gen,
                        { "Я правда не жульничала!\n",
                        "Кто мне теперь поверит?\n",
                        "Кажется, я никогда не промахиваюсь\n"
                        });
                }
                response.SetText(tmp);
                response.SetTts(tmp);
            }
        } while (!Bot.ShipsAmount() == 0 || !Player.ShipsAmount() == 0);
        if (Bot.ShipsAmount() == 0)
            tmp = Player.GetName() + " побеждает! Поздравляю!\n";
        else
            tmp = ChooseRandomString(gen,
                { "Я победила\n",
                "В этот раз побеждаю я\n",
                "Ха-ха. Это было просто\n"
                });
        response.SetText(tmp);
        response.SetTts(tmp);
        tmp = ChooseRandomString(gen,
            { "Сыграем еще?\n",
            "Хочешь сыграем еще раз?\n",
            "Давай сыграем еще раз?\n",
            "Может сыграем еще раз?\n"
            });
        response.SetText(tmp);
        response.SetTts(tmp);
        tmp = request.Command();
        if (tmp.find("го") == std::string::npos && tmp.find("давай") == std::string::npos && tmp.find("хоч") == std::string::npos)
            break;
    } while (true);
    response.SetEndSession(true);
}

int main()
{
    Skill battleships;
    battleships.SetCallback(MyCallback);
    battleships.Run();
    return 0;
}

// TODO: Remake all "Show" functions