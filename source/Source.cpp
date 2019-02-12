#include "Header.hpp"
#include <Request.hpp>
#include <Response.hpp>
#include <Skill.hpp>

void BattleshipsCallback(const Alice::Request& request, Alice::Response& response) {
    const auto& session = request.Session();
    if (!session.IsNew() && request.Command().empty()) {
        ClearFiles(session.SessionId());
        response.SetText("Пока!");
        response.SetTts("Пока!");
        response.SetEndSession(true);
        return;
    }
    State core(session.SessionId());
    switch (core.GetStage()) {
    case NAME: {
        std::string phrase = ChooseRandomString(core.Gen(),
            { "Как вас зовут?",
            "Имя?",
            "Ваше имя?",
            "Как вас называть?",
            "Как к вам обращаться?",
            });
        response.SetText(phrase);
        response.SetTts(phrase);
        core.SetStage(GREET);
        break;
    }
    case GREET: {
        std::string phrase = Cut(request.Command());
        Case MyVariables(&core, phrase);
        phrase = "Приятно с вами познакомиться, " + phrase + '\n';
        phrase += ChooseRandomString(core.Gen(),
            { "Как будем расставлять корабли? Вручную или случайно?",
            "Выберите как расставлять корабли: вручную или случайно",
            "Я могу помочь расставить ваши корабли вручную или сделать это случайно. Что выберете?",
            "Вы можете расставить корабли вручную или позволить мне сделать это случайным образом. Что вы выберете?",
            "Корабли можно расставить вручную или случайно. Какой вариант вам больше подходит?"
            });
        response.SetText(phrase);
        response.SetTts(phrase);
        Alice::Button ButtonHard("Вручную", { "hard_method" }, true);
        response.PushButton(ButtonHard);
        Alice::Button ButtonEasy("Случайно", { "easy_method" }, true);
        response.PushButton(ButtonE);
        core.SetStage(PLACEMENT);
        break;
    }
    case PLACEMENT: {
        Case MyVariables(&core);
        std::string phrase;
        if (request.Payload().dump() == "hard_method") {
            phrase = ChooseRandomString(core.Gen(),
                { "Итак, вы выбрали сложный путь\n",
                "Вы скучный человек\n",
                "В следующий раз лучше расставьте вручную\n",
                "Если вы думаете, что так будет лучше, то вы ошибаетесь\n",
                "Времени мало, а работы много, приступим!\n"
                });
            phrase += "Вот ваше поле:\n";
            response.SetTts(phrase);
            MyVariables.User.RandomScenario();
            phrase += MyVariables.User.InnerToString();
            response.SetText(phrase);
            phrase = ChooseRandomString(core.Gen(),
                { "Эх!",
                "Простите",
                "В следующий раз я выберу случайную расстановку",
                ":("
                });
            Alice::Button ButtonUseless(phrase, { "useless_button" }, true);
            response.PushButton(ButtonUseless);
            core.SetStage(MANUAL);
            break;
        }
        if (request.Payload().dump() != "easy_method")
            phrase += ChooseRandomString(core.Gen(),
                { "Сделаем вид, что я вас поняла и расставим корабли случайно",
                "Что? Ладно, расставим кораблики случайно\n",
                "Полагаю, что мы расставим корабли случайно\n",
                "Я бы все равно расставила их случайно\n",
                "Ладно, у вас был шанс и вы его упустили. Я решу все сама\n"
                });
        else
            phrase += ChooseRandomString(core.Gen(),
                { "Итак, вы выбрали легкий путь. Спасибо\n",
                "Это экономит время и нервы, спасибо\n",
                "Правильное решение. Спасибо\n",
                "Так все становится проще. Спасибо\n",
                "На секунду я подумала что их придется ставить вручную. Спасибо\n"
                });
        phrase += "Вот ваше поле:\n";
        response.SetTts(phrase);
        MyVariables.User.RandomScenario();
        phrase += MyVariables.User.InnerToString();
        response.SetText(phrase);
        phrase = ChooseRandomString(core.Gen(),
            { "Не за что",
            "Всегда к вашим услугам",
            "Никто не стал бы ставить их вручную"
            });
        Alice::Button ButtonUseless(phrase, { "useless_button" }, true);
        response.PushButton(ButtonUseless);
        core.SetStage(DECIDE1);
        break;
    }
    case MANUAL: {
        std::string phrase = ChooseRandomString(core.Gen(),
            { "Выберите первую точку\n",
            "Нужно выбрать первую точку\n",
            "Первая точка:\n"
            "Назовите первую точку\n"
            });
        response.SetText(phrase);
        response.SetTts(phrase);
        core.SetStage(MANUAL1);
    }
    case MANUAL1: {
        auto position = GetPos(request.Command());
        std::string phrase;
        if (position.first != 10 && position.second != 10) {
            Case MyVariables(&core);
            if (MyVariables.User.IsProperForPlacement(position)) {
                MyVariables.Position = position;
                std::string field = MyVariables.User.TargetedToString(position);
                phrase = ChooseRandomString(core.Gen(),
                    { "Выберите вторую точку\n",
                    "Теперь нужно выбрать вторую точку\n",
                    "Вторая точка:\n"
                    "Теперь назовите вторую точку\n"
                    });
                response.SetText(field + phrase);
                response.SetTts(phrase);
                core.SetStage(MANUAL2);
                return;
            }
            phrase = ChooseRandomString(core.Gen(),
                { "Так не получится, рядом уже есть корабль. ",
                "Корабли не могут располагаться вплотную или пересекаться. ",
                "Слишком близко к другому кораблю. "
                });
            phrase += ChooseRandomString(core.Gen(),
                { "Попробуй еще раз",
                "Назовите еще раз ",
                "Стоит попробовать еще раз "
                });
            response.SetText(phrase);
            response.SetTts(phrase);
            break;
        }
        phrase = ChooseRandomString(core.Gen(),
            { "Еще раз?\n",
            "Очень сложно. Может еще раз?\n",
            "Что-что?\n",
            "Мне просто нужны буквы и цифры, можно еще раз?\n",
            "Еще разок можно?\n",
            "А? Я прослушала, простите\n",
            "Если у нас такие проблемы с первой, то что будет со второй? Попробуй еще раз\n"
            });
        response.SetText(phrase);
        response.SetTts(phrase);
        break;
    }
    case MANUAL2: {
        auto position = GetPos(request.Command());
        std::string phrase;
        if (position.first != 0 && position.second != 0) {
            Case MyVariables(&core);
            if (!IsProperPlaced(MyVariables.Position, position)) {
                phrase = ChooseRandomString(core.Gen(),
                    { "Точки должны быть на одной прямой. ",
                    "Точки располагаются не на одной прямой. ",
                    "Указанные позиции не располагаются на одной прямой. ",
                    "Указанные позиции располагаются не на одной прямой. "
                    });
                phrase += ChooseRandomString(core.Gen(),
                    { "Попробуй еще раз",
                    "Еще раз ",
                    "Стоит попробовать еще раз "
                    });
                response.SetText(phrase);
                response.SetTts(phrase);
                break;
            }
            if (!MyVariables.User.IsProperForPlacement(position)) {
                phrase = ChooseRandomString(core.Gen(),
                    { "Так не получится, рядом уже есть корабль. ",
                    "Корабли не могут располагаться вплотную или пересекаться. ",
                    "Слишком близко к другому кораблю. "
                    });
                phrase += ChooseRandomString(core.Gen(),
                    { "Попробуй еще раз",
                    "Еще раз ",
                    "Стоит попробовать еще раз "
                    });
                response.SetText(phrase);
                response.SetTts(phrase);
                break;
            }
            if (!IsProperSized(MyVariables.Position, position)) {
                phrase = ChooseRandomString(core.Gen(),
                    { "Корабль неподходящего размера. ",
                    "Корабль слишком большой. ",
                    "Таких кораблей не бывает. ",
                    "Не бывает таких кораблей. "
                    });
                phrase += ChooseRandomString(core.Gen(),
                    { "Попробуй еще раз",
                    "Еще раз ",
                    "Стоит попробовать еще раз "
                    });
                response.SetText(phrase);
                response.SetTts(phrase);
                break;
            }
            if (MyVariables.User.IsNeeded(MyVariables.Position, position)) {
                phrase = ChooseRandomString(core.Gen(),
                    { "Таких кораблей достаточно. ",
                    "Больше таких кораблей не нужно. ",
                    "Такие корабли кончились. ",
                    "Может хватит таких кораблей?. "
                    });
                phrase += ChooseRandomString(core.Gen(),
                    { "Попробуй еще раз",
                    "Еще раз ",
                    "Стоит попробовать еще раз "
                    });
                response.SetText(phrase);
                response.SetTts(phrase);
                return;
            }
            MyVariables.User.Place(MyVariables.Position, position);
            if (MyVariables.User.TotalShips() < 10) {
                phrase = "Осталось ";
                phrase += std::to_string(10 - MyVariables.User.TotalShips());
                phrase += " кораблей";
                response.SetTts(phrase);
                phrase += MyVariables.User.InnerToString();
                response.SetText(phrase);
                phrase = ChooseRandomString(core.Gen(),
                    { "Жуть!",
                    "Лучше бы я выбрал случайную расстановку",
                    "Как же долго",
                    "Скорее бы это кончилось"
                    });
                Alice::Button ButtonUseless(phrase, { "useless_button" }, true);
                response.PushButton(ButtonUseless);
                core.SetStage(MANUAL);
                break;
            }
            phrase = ChooseRandomString(core.Gen(),
                { "Кораблей хватит. ",
                "Все, корабли расставлены.",
                "Все корабли расставлены. ",
                "Корабли кончились. "
                });
            phrase += ChooseRandomString(core.Gen(),
                { "Отличная работа",
                "Хорошая работа",
                "Вашему упорству можно позавидовать"
                });
            response.SetText(phrase);
            response.SetTts(phrase);
            Alice::Button ButtonUseless("Спасибо", { "useless_button" }, true);
            response.PushButton(ButtonUseless);
            core.SetStage(DECIDE1);
            break;
        }
        phrase = ChooseRandomString(core.Gen(),
            { "Еще раз?\n",
            "Очень сложно. Может еще раз?\n",
            "Что-что?\n",
            "Мне просто нужны буквы и цифры\n",
            "Еще разок можно?\n",
            "А? Я прослушала, простите\n",
            "В первый раз лучше получилось\n",
            "Кажется я чего-то не поняла, можно еще раз?\n"
            });
        response.SetText(phrase);
        response.SetTts(phrase);
        break;
    }
    case DECIDE1: {
        std::string phrase = ChooseRandomString(core.Gen(),
            { "Я постараюсь не жульничать\n",
            "Обещаю что не буду жульничать\n",
            "Не беспокойтесь, я ничего не запомнила\n",
            "Сейчас я забуду где стоят все ваши корабли и мы начнем\n"
            });
        phrase += "\nДавайте решим кто будет ходить первым. Орел или решка?";
        response.SetText(phrase);
        response.SetTts(phrase);
        Alice::Button Bird("Орел", { "bird" }, true);
        response.PushButton(Bird);
        Alice::Button NotBird("Решка", { "not_bird" }, true);
        response.PushButton(NotBird);
        core.SetStage(BEGIN);
        break;
    }
    case BEGIN: {
        std::string phrase1 = request.Payload().dump();
        std::string phrase2 = "Тогда я выбираю ";
        if (phrase1 == "bird" || phrase1 == "not_bird") {
            if (phrase1 == "bird")
                phrase2 += "решку\n";
            else
                phrase2 += "орла\n";
            if (phrase1 == ChooseRandomString(core.Gen(), { "bird", "not_bird" })) {
                phrase1 = phrase2;
                phrase1 += "...\n";
                phrase1 += ChooseRandomString(core.Gen(),
                    { "Сегодня выиграл ты\n",
                    "Так не честно\n",
                    "В следующий раз выиграю я. Ходите\n",
                    "Закрою глаза на ваше жульничество. Ваш ход\n"
                    });
                std::string choise = ChooseRandomString(core.Gen(),
                    { "...Куда стрелять?",
                    "...Куда бить?",
                    "...Куда ударить?",
                    "...По какой позиции ударить?",
                    "...По какой позиции бить?",
                    "...По какой позиции стрелять?"
                    });
                response.SetTts(phrase1 + choise);
                Case MyVariables(&core);
                response.SetText(phrase1 + MyVariables.Bot.OuterToString() + choise);
                core.SetStage(PLAYER);
                break;
            }
            phrase1 = phrase2;
            phrase1 += "...\n";
            phrase1 += ChooseRandomString(core.Gen(),
                { "Я победила!",
                "Удача на моей стороне",
                "В следующий раз повезет",
                "Итак, я хожу"
                });
            response.SetTts(phrase1);
            response.SetText(phrase1);
            Alice::Button ButtonUseless("Эх!", { "useless_button" }, true);
            response.PushButton(ButtonUseless);
            core.SetStage(ALICE);
            break;
        }
        phrase1 = ChooseRandomString(core.Gen(),
            { "Что-что? Мне ходить? Ладно!",
            "Я похожу, а вы соберитесь с мыслями",
            "Ничего не поняла, а потому буду ходить",
            "Тогда я хожу, вы не против?"
            });
        response.SetTts(phrase1);
        response.SetText(phrase1);
        Alice::Button ButtonUseless("Эх!", { "useless_button" }, true);
        response.PushButton(ButtonUseless);
        core.SetStage(ALICE);
        break;
    }
    case ALICE: {
        Case MyVariables(&core);
        size_t health = MyVariables.User.TotalHealth();
        size_t amount = MyVariables.User.TotalShips();
        std::string phrase;
        if (request.RequestType() != Alice::Request::Type::ButtonPressed) {
            phrase = ChooseRandomString(core.Gen(),
                { "Моя очередь!\n",
                "Теперь я стреляю!\n",
                "Куда бы ударить?\n",
                "Наконец моя очередь\n",
                "Попаду или промажу?\n"
                });
        }
        auto possible = MyVariables.Possible;
        auto hits = MyVariables.Hits;
        if (MyVariables.Possible.size() == 0) {
            auto pair = MyVariables.User.ShootRand();
            MyVariables.Possible = pair.first;
            MyVariables.Hits = pair.second;
        } else {
            auto pair = MyVariables.User.ContinueShooting(MyVariables.Possible, MyVariables.Hits);
            MyVariables.Possible = pair.first;
            MyVariables.Hits = pair.second;
        }
        std::string field = MyVariables.User.OuterToString();
        std::string phrase2;
        if (MyVariables.User.TotalShips() < amount) {
            auto dif = amount - MyVariables.User.TotalShips();
            if (dif > 3) {
                phrase2 = ChooseRandomString(core.Gen(),
                    { "Я случайно\n",
                    "Ну, что, в лотерею?\n",
                    "Oof\n",
                    "( ͡° ͜ʖ ͡°)\n"
                    });
            }
            else if (dif == 3) {
                phrase2 = ChooseRandomString(core.Gen(),
                    { "Три!\n",
                    "Может сразу сдадитесь?\n",
                    "У меня бы отпало желание играть (Сдавайтесь!)\n",
                    "Мне неслыханно везет\n",
                    "Что за магия?\n"
                    });
            }
            else if (dif == 2) {
                phrase2 = ChooseRandomString(core.Gen(),
                    { "Два!\n",
                    "Я в ударе!n",
                    "Я ненарочно\n",
                    "Кажется я никогда не промахиваюсь\n"
                    });
            } else {
                phrase2 = ChooseRandomString(core.Gen(),
                    { "Ну, хоть один\n",
                    "Убила!\n",
                    "Без шансов\n",
                    "Удачно\n",
                    "Кораблик к кораблику...\n"
                    });
            }
        }
        else if (MyVariables.User.TotalHealth() < health) {
            phrase2 = ChooseRandomString(core.Gen(),
                { "Кажется, попала!\n",
                "Я попала!\n",
                "Попадание!\n",
                "В точку\n",
                "В яблочко\n",
                "Ранила!\n",
                "Прямое попадание!\n"
                });
        } else {
            phrase2 = ChooseRandomString(core.Gen(),
                { "\nПромах\n",
                "\nОбидно\n",
                "\nВ следующий раз попаду\n",
                "\nЭх\n",
                "\nОбидный промах\n"
                });
        }
        if (MyVariables.User.IsLoser()) {
            std::string phrase3 = ChooseRandomString(core.Gen(),
                { "Ты проиграл!\n",
                "Кораблей не осталось, я выиграла!\n",
                "Я выиграла!",
                "Хороший бой, но победа за мной\n",
                "Победа за мной!",
                "Это было просто. Я победила\n"
                });
            response.SetTts(phrase + phrase2 + phrase3);
            response.SetText(phrase + field + phrase2 + phrase3);
            Alice::Button ButtonUseless("Эх!", { "useless_button" }, true);
            response.PushButton(ButtonUseless);
            core.SetStage(TRYAGAIN);
            break;
        }
        std::string field2 = MyVariables.Bot.OuterToString();
        std::string phrase3 = ChooseRandomString(core.Gen(),
            { "Куда стрелять?",
            "Куда бить?",
            "Куда ударить?",
            "По какой позиции ударить?",
            "По какой позиции бить?",
            "По какой позиции стрелять?"
            });
        response.SetTts(phrase + phrase2 + phrase3);
        response.SetText(phrase + field + phrase2 + field2 + phrase3);
        core.SetStage(PLAYER);
        break;
    }
    case PLAYER: {
        auto position = GetPos(request.Command());
        std::string phrase;
        if (position.first != 10 && position.second != 10) {
            Case MyVariables(&core);
            phrase = MyVariables.Bot.ShipName(position);
            if (MyVariables.Bot.Shoot(position)) {
                std::string field = MyVariables.Bot.OuterToString();
                std::string phrase2;
                if (!phrase.empty() && MyVariables.Bot.IsDead(phrase)) {
                    phrase += " уничтожен!\n";
                    if (MyVariables.Bot.IsLoser()) {
                        phrase2 = ChooseRandomString(core.Gen(),
                            { "Хорошая игра. Поздравляю с победой",
                            "Вы победили, мои поздравления",
                            "Это было сильно",
                            });
                        response.SetText(phrase + field + phrase2);
                        response.SetTts(phrase + phrase2);
                        Alice::Button ButtonUseless("Спасибо!", { "useless_button" }, true);
                        response.PushButton(ButtonUseless);
                        core.SetStage(TRYAGAIN);
                        break;
                    }
                    phrase2 = ChooseRandomString(core.Gen(),
                        { "Куда стрелять?",
                        "Куда бить?",
                        "Куда ударить?",
                        "По какой позиции ударить?",
                        "По какой позиции бить?",
                        "По какой позиции стрелять?"
                        });
                    response.SetText(phrase + field + phrase2);
                    response.SetTts(phrase + phrase2);
                    break;
                }
                phrase = "Попадание!\n";
                phrase2 = ChooseRandomString(core.Gen(),
                    { "Куда стрелять?",
                    "Куда бить?",
                    "Куда ударить?",
                    "По какой позиции ударить?",
                    "По какой позиции бить?",
                    "По какой позиции стрелять?"
                    });
                response.SetText(phrase + field + phrase2);
                response.SetTts(phrase + phrase2);
                break;
            }
            phrase = "Промах!\n";
            std::string field = MyVariables.Bot.OuterToString();
            response.SetText(phrase + field);
            response.SetTts(phrase);
            Alice::Button ButtonUseless("Эх!", { "useless_button" }, true);
            response.PushButton(ButtonUseless);
            core.SetStage(ALICE);
            break;
        }
        phrase = ChooseRandomString(core.Gen(),
            { "Еще раз?\n",
            "Сложно. Еще раз?\n",
            "Что-что?\n",
            "А стрелять-то куда?\n",
            "Еще разок можно?\n",
            "А? Я прослушала, простите\n",
            "Что?\n"
            });
        response.SetText(phrase);
        response.SetTts(phrase);
        break;
    }
    case TRYAGAIN: {
        std::string phrase = ChooseRandomString(core.Gen(),
            { "Сыграем еще раз?\n",
            "Хочешь сыграть еще раз?\n",
            "Давай сыграем еще раз!\n",
            "Давай еще!\n",
            "Может еще разок?",
            "Давай еще разок. Ну пожалуйста!\n"
            });
        std::string yesResponse = ChooseRandomString(core.Gen(),
            { "Давай!",
            "Пойдем",
            "Сыграем еще раз"
            });
        std::string noResponse = ChooseRandomString(core.Gen(),
            { "Нет, спасибо",
            "Только не еще раз!",
            "Нет уж!"
            });
        Alice::Button YesButton(yesResponse, { "Y" }, true);
        response.PushButton(YesButton);
        Alice::Button NoButton(noResponse, { "N" }, true);
        response.PushButton(NoButton);
        response.SetText(phrase);
        response.SetTts(phrase);
        core.SetStage(DECIDE2);
        break;
    }
    case DECIDE2: {
        std::string phrase = request.Payload().dump();
        if (phrase == "Y") {
            phrase = ChooseRandomString(core.Gen(),
                { "Давай поменяем что-нибудь. Как тебя теперь будут звать?",
                "Выбирай новый псевдоним, таковы правила",
                "Нова игра - новое имя. Каким будет оно будте теперь?"
                });
            response.SetTts(phrase);
            response.SetText(phrase);
            ClearFiles(core.GetSessionId());
            core.SetStage(GREET);
            break;
        }
        phrase = ChooseRandomString(core.Gen(),
            { "Ну и ладно. Пока!",
            "Хорошо. До свидания",
            "Тогда до встречи"
            });
        response.SetTts(phrase);
        response.SetText(phrase);
        core.~core(); // файлы сохраняются в деструкторе. Необходим явный вызов, чтобы все-таки удалить файлы
        ClearFiles(session.SessionId());
        response.SetEndSession(true);
        break;
    }
    };
}

int main() {
    Skill battleships;
    battleships.SetCallback(BattleshipsCallback);
    battleships.Run();
    return 0;
}
