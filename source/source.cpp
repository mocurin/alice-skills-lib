#include "Header.hpp"

bool HandleEmpty(const Alice::Request& request, Alice::Response& response)
{
    const auto& session = request.Session();
    if (!session.IsNew())
        if (request.Command() == "")
        {
            ClearFiles();
            response.SetText("bruh");
            response.SetEndSession(true);
            return true;
        }
    return false;
}

void MyCallback(const Alice::Request& request, Alice::Response& response)
{
    auto info = ReadStageAndGen();
    if (HandleEmpty(request, response))
        return;
    switch (info.first)
    {
    case NAME:
    {
        std::mt19937 gen(info.second);
        std::string tmp = ChooseRandomString(gen,
            { "Как вас зовут?",
            "Имя?",
            "Кэп, ваше имя?",
            "Как вас называть?",
            "Как к вам обращаться?",
            "Ваше имя?"
            });
        response.SetText(tmp);
        response.SetTts(tmp);
        SaveStageAndGen(GREET, info.second);
        break;
    }
    case GREET:
    {
        std::mt19937 gen(info.second);
        std::string tmp = Cut(request.Command());
        Case MyVariables(info, tmp);
        tmp = "Приятно с вами познакомиться, " + tmp + "\n\n";
        tmp += ChooseRandomString(gen,
            { "Как будем расставлять корабли? Вручную или случайно?",
            "Выберите как расставлять корабли: вручную или случайно",
            "Я могу помочь расставить ваши корабли вручную или сделать это случайно. Что выберете?",
            "Вы можете расставить корабли вручную или позволить мне сделать это случайным образом. Что выберете?",
            "Корабли можно расставить вручную или случайно. Какой вариант вам больше подходит?"
            });
        response.SetText(tmp);
        response.SetTts(tmp);
        Alice::Button ButtonH("Вручную", { "hard_method" }, true);
        response.PushButton(ButtonH);
        Alice::Button ButtonE("Случайно", { "easy_method" }, true);
        response.PushButton(ButtonE);
        MyVariables.SetStage(PLACEMENT);
        break;
    }
    case PLACEMENT:
    {
        Case MyVariables;
        std::string tmp;
        if (request.Payload().dump() == "easy_method")
        {
            tmp = ChooseRandomString(MyVariables.GetGen(),
                { "Итак, вы выбрали легкий путь. Спасибо\n",
                "Это экономит время и нервы, спасибо\n",
                "Правильное решение. Спасибо\n",
                "Так все становится проще. Спасибо\n",
                "На секунду я подумала что их придется ставить вручную. Спасибо\n"
                });
            tmp += "\nВот ваше поле:\n\n";
            response.SetTts(tmp);
            MyVariables.GetPlayer().RandomScenario();
            tmp += MyVariables.GetPlayer().MyBoardToString();
            response.SetText(tmp);
            tmp = ChooseRandomString(MyVariables.GetGen(),
                { "Не за что",
                "Всегда к вашим услугам",
                "Никто не стал бы ставить их вручную"
                });
            Alice::Button Button(tmp, { "useless_button" }, true);
            response.PushButton(Button);
            MyVariables.SetStage(DECIDE);
            break;
        }
        tmp = ChooseRandomString(MyVariables.GetGen(),
            { "Говорите позиции первой и последней клетки корабля пока не заполните поле. Если вдруг не знаете, кораблей должно быть десять\n",
            "Мне будут нужны первая и последняя клетка корабля. Нужно будет задать их десять раз чтобы заполнить поле\n",
            "Координат первой и последней клетки корабля будет достаточно чтобы создать его. Напоминаю, кораблей в конечном итоге будет 10\n"
            });
        tmp += "\nВот ваше поле:\n\n";
        std::string tts = tmp + "\nПервая клетка";
        tmp += MyVariables.GetPlayer().MyBoardToString();
        tmp += "\nПервая клетка";
        response.SetText(tmp);
        response.SetTts(tts);
        MyVariables.SetStage(MANUAL);
        break;
    }
    case MANUAL:
    {
        auto pos = GetPos(request.Command());
        if (pos.first != 0 && pos.second != 0)
        {
            Case MyVariables;
            std::string tmp;
            if (MyVariables.GetPlayer().Check(&pos))
            {
                MyVariables.SetPos(pos);
                std::string tmp = MyVariables.GetPlayer().TargetedToString(pos);
                tmp += "\nВторая клетка";
                response.SetText(tmp);
                response.SetTts("Вторая клетка");
                MyVariables.SetStage(MANUAL2);
                break;
            }
            tmp = ChooseRandomString(MyVariables.GetGen(),
                { "Так не получится, рядом уже есть корабль. ",
                "Корабли не могут располагаться вплотную или пересекаться. ",
                "Слишком близко к другому кораблю. "
                });
            tmp += ChooseRandomString(MyVariables.GetGen(),
                { "Попробуй еще раз",
                "Еще раз ",
                "Стоит попробовать еще раз "
                });
            response.SetText(tmp);
            response.SetTts(tmp);
            break;
        }
        std::mt19937 gen(info.second);
        std::string tmp = ChooseRandomString(gen,
            { "Еще раз?\n",
            "Сложно. Еще раз?\n",
            "Что-что?\n",
            "Мне просто нужны буквы\n",
            "Еще разок можно?\n",
            "А? Я прослушала, простите\n",
            "Если у нас такие проблемы с первой, то что будет со второй? Попробуй еще раз\n"
            });
        response.SetText(tmp);
        response.SetTts(tmp);
        break;
    }
    case MANUAL2:
    {
        auto pos = GetPos(request.Command());
        if (pos.first != 0 && pos.second != 0)
        {
            Case MyVariables;
            std::string tmp;
            if (!SameLine(MyVariables.GetPos(), pos))
            {
                tmp = ChooseRandomString(MyVariables.GetGen(),
                    { "Клетки должны быть на одной прямой. ",
                    "Клетки располагаются не на одной прямой. ",
                    "Указанные позиции не располагаются на одной прямой. ",
                    "Указанные позиции располагаются не на одной прямой. "
                    });
                tmp += ChooseRandomString(MyVariables.GetGen(),
                    { "Попробуй еще раз",
                    "Еще раз ",
                    "Стоит попробовать еще раз "
                    });
                response.SetText(tmp);
                response.SetTts(tmp);
                break;
            }
            if (!MyVariables.GetPlayer().Check(&pos))
            {
                tmp = ChooseRandomString(MyVariables.GetGen(),
                    { "Так не получится, рядом уже есть корабль. ",
                    "Корабли не могут располагаться вплотную или пересекаться. ",
                    "Слишком близко к другому кораблю. "
                    });
                tmp += ChooseRandomString(MyVariables.GetGen(),
                    { "Попробуй еще раз",
                    "Еще раз ",
                    "Стоит попробовать еще раз "
                    });
                response.SetText(tmp);
                response.SetTts(tmp);
                break;
            }
            if (!CheckSize(MyVariables.GetPos(), pos))
            {
                tmp = ChooseRandomString(MyVariables.GetGen(),
                    { "Корабль неподходящего размера. ",
                    "Корабль слишком большой. ",
                    "Таких кораблей не бывает. ",
                    "Не бывает таких кораблей. "
                    });
                tmp += ChooseRandomString(MyVariables.GetGen(),
                    { "Попробуй еще раз",
                    "Еще раз ",
                    "Стоит попробовать еще раз "
                    });
                response.SetText(tmp);
                response.SetTts(tmp);
                break;
            }
            if (!MyVariables.GetPlayer().AmountCheck(MyVariables.GetPos(), pos))
            {
                tmp = ChooseRandomString(MyVariables.GetGen(),
                    { "Таких кораблей достаточно. ",
                    "Больше таких кораблей не нужно. ",
                    "Такие корабли кончились. ",
                    "Может хватит таких кораблей?. "
                    });
                tmp += ChooseRandomString(MyVariables.GetGen(),
                    { "Попробуй еще раз",
                    "Еще раз ",
                    "Стоит попробовать еще раз "
                    });
                response.SetText(tmp);
                response.SetTts(tmp);
                break;
            }
            MyVariables.GetPlayer().Place(MyVariables.GetPos(), pos);
            if (MyVariables.GetPlayer().ShipsAmount() < 10)
            {
                tmp += ". Осталось";
                tmp += std::to_string(10 - MyVariables.GetPlayer().ShipsAmount());
                tmp += " кораблей\n";
                MyVariables.SetStage(MANUAL);
                response.SetText(tmp);
                response.SetTts(tmp);
                break;
            }
            tmp = ChooseRandomString(MyVariables.GetGen(),
                { "Кораблей хватит. ",
                "Все, корабли расставлены.",
                "Все корабли расставлены ",
                "Корабли кончились. "
                });
            tmp += ChooseRandomString(MyVariables.GetGen(),
                { "Наконец-то это кончилось",
                "Лучше бы расставили случайно ",
                "Отличная работа",
                "Хорошая работа"
                });
            response.SetText(tmp);
            response.SetTts(tmp);
            Alice::Button Button("Спасибо", { "useless_button" }, true);
            response.PushButton(Button);
            MyVariables.SetStage(DECIDE);
            break;
        }
        std::mt19937 gen(info.second);
        std::string tmp = ChooseRandomString(gen,
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
        break;
    }
    case DECIDE:
    {
        std::mt19937 gen(info.second);
        std::string tmp = ChooseRandomString(gen,
            { "Я постараюсь не жульничать\n",
            "Обещаю что не буду жульничать\n",
            "Не беспокойся, я ничего не запомнила\n",
            "Сейчас я забуду где стоят все твои корабли и мы начнем\n"
            });
        tmp += "\nДавай решим кто будет ходить первым. Орел или решка?";
        response.SetText(tmp);
        response.SetTts(tmp);
        Alice::Button Bird("Орел", { "bird" }, true);
        response.PushButton(Bird);
        Alice::Button NotBird("Решка", { "not_bird" }, true);
        response.PushButton(NotBird);
        SaveStageAndGen(BEGIN, info.second);
        break;
    }
    case BEGIN:
    {
        Case MyVariables;
        std::string tmp = request.Payload().dump();
        std::string tts = "Тогда я выбираю ";
        if (tmp == "bird" || tmp == "not_bird")
        {
            if (tmp == "bird")
            {
                tts += "решку\n";
            }
            else
            {
                tts += "орла\n";
            }
            if (tmp == ChooseRandomString(MyVariables.GetGen(), { "bird", "not_bird"}))
            {
                tmp = tts;
                tmp += ChooseRandomString(MyVariables.GetGen(),
                    { "\nСегодня выиграл ты\n\n",
                    "\nТак не честно\n\n",
                    "\nВ следующий раз выиграю я\n\n",
                    "\nВечно так\n\n"
                    });
                std::string choise = ChooseRandomString(MyVariables.GetGen(),
                    { "\nКуда стрелять?",
                    "\nКуда бить?",
                    "\nКуда ударить?",
                    "\nПо какой позиции ударить?",
                    "\nПо какой позиции бить?",
                    "\nПо какой позиции стрелять?"
                    });
                response.SetTts(tmp + choise);
                response.SetText(tmp + MyVariables.GetBot().EnemyBoardToString() + choise);
                MyVariables.SetStage(PLAYER);
                break;
            }
            tmp = tts;
            tmp += ChooseRandomString(MyVariables.GetGen(),
                { "Я победила!",
                "Удача на моей стороне",
                "В следующий раз повезет",
                "Я хожу"
                });
            response.SetTts(tmp);
            response.SetText(tmp);
            Alice::Button Boop("Эх!", { "useless_button" }, true);
            response.PushButton(Boop);
            MyVariables.SetStage(ALICE);
            break;
        }
        tmp = ChooseRandomString(MyVariables.GetGen(),
            { "Что-что? Мне ходить? Ладно!",
            "Я похожу, а ты соберись с мыслями",
            "Ничего не поняла, а потому буду ходить",
            "Тогда я хожу, ты не против?"
            });
        response.SetTts(tmp);
        response.SetText(tmp);
        Alice::Button Boop("Эх!", { "useless_button" }, true);
        response.PushButton(Boop);
        MyVariables.SetStage(ALICE);
        break;
    }
    case ALICE:
    {
        Case MyVariables;
        size_t health = MyVariables.GetPlayer().TotalHealth();
        size_t amount = MyVariables.GetPlayer().ShipsAmount();
        std::string tmp;
        if (request.RequestType() != Alice::Request::Type::ButtonPressed)
        {
            tmp = ChooseRandomString(MyVariables.GetGen(),
                { "Моя очередь!\n",
                "Теперь я стреляю!\n",
                "Куда бы ударить?\n",
                "Наконец моя очередь\n",
                "Попаду или промажу?\n"
                });
        }
        if (MyVariables.GetTracking().second == 0)
        {
            MyVariables.GetTracking() = MyVariables.GetPlayer().ShootRand();
        }
        else
        {
            MyVariables.GetTracking() = MyVariables.GetPlayer().ContinueShooting(MyVariables.GetTracking().first, MyVariables.GetTracking().second);
        }
        std::string field = MyVariables.GetPlayer().EnemyBoardToString();
        std::string tts;
        if (MyVariables.GetPlayer().ShipsAmount() < amount)
        {
            auto dif = amount - MyVariables.GetPlayer().ShipsAmount();
            if (dif > 3)
            {
                tts = ChooseRandomString(MyVariables.GetGen(),
                    { "\nСкрин сделай и отправь @mocurin\n",
                    "\nЯ случайно\n",
                    "\nНу, что, в лотерею?\n",
                    "\nEouf\n",
                    "\n...\n"
                    });
            }
            else if (dif == 3)
            {
                tts = ChooseRandomString(MyVariables.GetGen(),
                    { "\nТри!\n",
                    "\nМожет сразу сдашься?\n",
                    "\nУ меня бы отпало желание играть\n",
                    "\nМне неслыханно везет\n",
                    "\nЧто за...\n"
                    });
            }
            else if (dif == 2)
            {
                tts = ChooseRandomString(MyVariables.GetGen(),
                    { "\nДва!\n",
                    "\nДабл!\n",
                    "\nАхахах\n",
                    "\nЯ ненарочно\n",
                    "\nКажется я никогда не промахиваюсь\n"
                    });
            }
            else
            {
                tts = ChooseRandomString(MyVariables.GetGen(),
                    { "\nНу, хоть один\n",
                    "\nУбила!\n",
                    "\nБез шансов\n",
                    "\nУдачно\n",
                    "\nКораблик к кораблику...\n"
                    });
            }
        }
        else if (MyVariables.GetPlayer().TotalHealth() < health)
        {
            tts = ChooseRandomString(MyVariables.GetGen(),
                { "\nКажется, попала!\n",
                "\nЯ попала\n",
                "\nПопадание!\n",
                "\nВ точку\n",
                "\nВ яблочко\n",
                "\nРанила!\n",
                "\nПрямое попадание!\n"
                });
        }
        else
        {
            tts = ChooseRandomString(MyVariables.GetGen(),
                { "\nПромах\n",
                "\nОбидно\n",
                "\nВ следующий раз попаду\n",
                "\nЭх\n",
                "\nОбидный промах\n"
                });
        }
        if (MyVariables.GetPlayer().ShipsAmount() == 0)
        {
            std::string ttsA = ChooseRandomString(MyVariables.GetGen(),
                { "\nТы проиграл!\n",
                "\nКораблей не осталось, я выиграла!\n",
                "\nЯ выиграла!",
                "\nХороший бой, но победа за мной\n",
                "\nПобеда за мной!",
                "\nЭто было просто. Я победила\n"
                });
            ttsA += ChooseRandomString(MyVariables.GetGen(),
                { "\nСыграем еще раз?\n",
                "\nХочешь сыграть еще раз?\n",
                "\nДавай сыграем еще раз!\n",
                "\nДавай еще!\n",
                "\nМожет еще разок?",
                "\nДавай еще разок. Ну пожалуйста!\n"
                });
            std::string resp = ChooseRandomString(MyVariables.GetGen(),
                { "\nГо!\n",
                "\nДавай!\n",
                "\nПойдем\n"
                });
            Alice::Button Boop(resp, { "Y" }, true);
            response.PushButton(Boop);
            std::string noresp = ChooseRandomString(MyVariables.GetGen(),
                { "\nНет!\n",
                "\nТолько не еще раз!\n",
                "\nНет уж!\n"
                });
            Alice::Button NotBoop(noresp, { "N" }, true);
            response.PushButton(NotBoop);
            response.SetTts(tmp + tts + ttsA);
            response.SetText(tmp + field + tts + ttsA);
            MyVariables.SetStage(TRYAGAIN);
            break;
        }
        std::string fieldA = MyVariables.GetBot().EnemyBoardToString();
        std::string ttsA = ChooseRandomString(MyVariables.GetGen(),
            { "\nКуда стрелять?",
            "\nКуда бить?",
            "\nКуда ударить?",
            "\nПо какой позиции ударить?",
            "\nПо какой позиции бить?",
            "\nПо какой позиции стрелять?"
            });
        response.SetTts(tmp + tts + ttsA);
        response.SetText(tmp + field + tts + fieldA + ttsA);
        MyVariables.SetStage(PLAYER);
        break;
    }
    case PLAYER:
    {
        auto pos = GetPos(request.Command());
        if (pos.first != 0 && pos.second != 0)
        {
            Case MyVariables;
            std::string tmp = MyVariables.GetBot().GetShipName(&pos);
            if (MyVariables.GetBot().Shoot(pos))
            {
                if (!tmp.empty() && MyVariables.GetBot().IsDead(tmp))
                {
                    tmp += " уничтожен!\n";
                    tmp += ChooseRandomString(MyVariables.GetGen(),
                        { "\nКуда стрелять?",
                        "\nКуда бить?",
                        "\nКуда ударить?",
                        "\nПо какой позиции ударить?",
                        "\nПо какой позиции бить?",
                        "\nПо какой позиции стрелять?"
                        });
                    response.SetText(tmp);
                    response.SetTts(tmp);
                    break;
                }
                tmp = "Попадание!\n";
                tmp += ChooseRandomString(MyVariables.GetGen(),
                    { "\nКуда стрелять?",
                    "\nКуда бить?",
                    "\nКуда ударить?",
                    "\nПо какой позиции ударить?",
                    "\nПо какой позиции бить?",
                    "\nПо какой позиции стрелять?"
                    });
                response.SetText(tmp);
                response.SetTts(tmp);
                break;
            }
            tmp = "Промах!\n";
            response.SetText(tmp);
            response.SetTts(tmp);
            Alice::Button Boop("Эх!", { "useless_button" }, true);
            response.PushButton(Boop);
            MyVariables.SetStage(ALICE);
            break;
        }
        std::mt19937 gen(info.second);
        std::string tmp = ChooseRandomString(gen,
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
        break;
    }
    case TRYAGAIN:
    {
        std::mt19937 gen(info.second);
        std::string tmp = request.Payload().dump();
        if (tmp == "Y")
        {
            tmp = ChooseRandomString(gen,
                { "Как тебя теперь будут звать?",
                "Выбирай новый псевдоним",
                "Каким будет новое имя?"
                });
            response.SetTts(tmp);
            response.SetText(tmp);
            ClearFiles();
            SaveStageAndGen(GREET, info.second);
            break;
        }
        tmp = ChooseRandomString(gen,
            { "Ну и ладно. Пока!",
            "До свидания",
            "До встречи"
            });
        response.SetTts(tmp);
        response.SetText(tmp);
        ClearFiles();
        response.SetEndSession(true);
        break;
    }
    }
}

int main()
{
    Skill battleships;
    battleships.SetCallback(MyCallback);
    battleships.Run();
    return 0;
}
