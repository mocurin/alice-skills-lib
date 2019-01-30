#include "Header.hpp"

void HandleEmpty(const Alice::Request& request, Alice::Response& response)
{
    ClearFiles();
    response.SetEndSession(true);
}

bool IsUnexpectedlyEmpty(const Alice::Request& request, Alice::Response& response) {
    if (!request.Session().IsNew())
        return request.Command() == "";
    return false;
}

void StName(const Alice::Request& request, Alice::Response& response,
    const std::pair<stages, size_t>& info) {
    std::mt19937 gen(info.second);
    std::string tmp = ChooseRandomString(gen,
        { "��� ��� �����?",
        "���?",
        "���, ���� ���?",
        "��� ��� ��������?",
        "��� � ��� ����������?",
        "���� ���?"
        });
    response.SetText(tmp);
    response.SetTts(tmp);
    SaveStageAndGen(GREET, info.second);
}

void StGreet(const Alice::Request& request, Alice::Response& response,
    const std::pair<stages, size_t>& info) {
    std::mt19937 gen(info.second);
    std::string tmp = Cut(request.Command());
    Case MyVariables(info, tmp);
    tmp = "������� � ���� �������������, " + tmp + "\n\n";
    tmp += ChooseRandomString(gen,
        { "��� ����� ����������� �������? ������� ��� ��������?",
        "�������� ��� ����������� �������: ������� ��� ��������",
        "� ���� ������ ���������� ���� ������� ������� ��� ������� ��� ��������. ��� ��������?",
        "�� ������ ���������� ������� ������� ��� ��������� ��� ������� ��� ��������� �������. ��� ��������?",
        "������� ����� ���������� ������� ��� ��������. ����� ������� ��� ������ ��������?"
        });
    response.SetText(tmp);
    response.SetTts(tmp);
    Alice::Button ButtonH("�������", { "hard_method" }, true);
    response.PushButton(ButtonH);
    Alice::Button ButtonE("��������", { "easy_method" }, true);
    response.PushButton(ButtonE);
    MyVariables.SetStage(PLACEMENT);
}

void StPlacement(const Alice::Request& request, Alice::Response& response,
    const std::pair<stages, size_t>& info) {
    Case MyVariables;
    std::string tmp;
    if (request.Payload().dump() == "easy_method") {
        tmp = ChooseRandomString(MyVariables.GetGen(),
            { "����, �� ������� ������ ����. �������\n",
            "��� �������� ����� � �����, �������\n",
            "���������� �������. �������\n",
            "��� ��� ���������� �����. �������\n",
            "�� ������� � �������� ��� �� �������� ������� �������. �������\n"
            });
        tmp += "\n��� ���� ����:\n\n";
        response.SetTts(tmp);
        MyVariables.GetUser().RandomScenario();
        tmp += MyVariables.GetUser().InnerToString();
        response.SetText(tmp);
        tmp = ChooseRandomString(MyVariables.GetGen(),
            { "�� �� ���",
            "������ � ����� �������",
            "����� �� ���� �� ������� �� �������"
            });
        Alice::Button Button(tmp, { "useless_button" }, true);
        response.PushButton(Button);
        MyVariables.SetStage(DECIDE);
    }
}

void StManual(const Alice::Request& request, Alice::Response& response,
    const std::pair<stages, size_t>& info) {
    auto pos = GetPos(request.Command());
    if (pos.first != 0 && pos.second != 0)
    {
        Case MyVariables;
        std::string tmp;
        if (MyVariables.GetUser().IsProperForPlacement(pos))
        {
            MyVariables.SetPos(pos);
            std::string tmp = MyVariables.GetUser().TargetedToString(pos);
            tmp += "\n������ ������";
            response.SetText(tmp);
            response.SetTts("������ ������");
            MyVariables.SetStage(MANUAL2);
            return;
        }
        tmp = ChooseRandomString(MyVariables.GetGen(),
            { "��� �� ���������, ����� ��� ���� �������. ",
            "������� �� ����� ������������� �������� ��� ������������. ",
            "������� ������ � ������� �������. "
            });
        tmp += ChooseRandomString(MyVariables.GetGen(),
            { "�������� ��� ���",
            "��� ��� ",
            "����� ����������� ��� ��� "
            });
        response.SetText(tmp);
        response.SetTts(tmp);
        return;
    }
    std::mt19937 gen(info.second);
    std::string tmp = ChooseRandomString(gen,
        { "��� ���?\n",
        "������. ��� ���?\n",
        "���-���?\n",
        "��� ������ ����� �����\n",
        "��� ����� �����?\n",
        "�? � ����������, ��������\n",
        "���� � ��� ����� �������� � ������, �� ��� ����� �� ������? �������� ��� ���\n"
        });
    response.SetText(tmp);
    response.SetTts(tmp);
}

void StManual2(const Alice::Request& request, Alice::Response& response,
    const std::pair<stages, size_t>& info) {
    auto pos = GetPos(request.Command());
    if (pos.first != 0 && pos.second != 0)
    {
        Case MyVariables;
        std::string tmp;
        if (!IsProperPlaced(MyVariables.GetPos(), pos))
        {
            tmp = ChooseRandomString(MyVariables.GetGen(),
                { "������ ������ ���� �� ����� ������. ",
                "������ ������������� �� �� ����� ������. ",
                "��������� ������� �� ������������� �� ����� ������. ",
                "��������� ������� ������������� �� �� ����� ������. "
                });
            tmp += ChooseRandomString(MyVariables.GetGen(),
                { "�������� ��� ���",
                "��� ��� ",
                "����� ����������� ��� ��� "
                });
            response.SetText(tmp);
            response.SetTts(tmp);
            return;
        }
        if (!MyVariables.GetUser().IsProperForPlacement(pos))
        {
            tmp = ChooseRandomString(MyVariables.GetGen(),
                { "��� �� ���������, ����� ��� ���� �������. ",
                "������� �� ����� ������������� �������� ��� ������������. ",
                "������� ������ � ������� �������. "
                });
            tmp += ChooseRandomString(MyVariables.GetGen(),
                { "�������� ��� ���",
                "��� ��� ",
                "����� ����������� ��� ��� "
                });
            response.SetText(tmp);
            response.SetTts(tmp);
            return;
        }
        if (!IsProperSized(MyVariables.GetPos(), pos))
        {
            tmp = ChooseRandomString(MyVariables.GetGen(),
                { "������� ������������� �������. ",
                "������� ������� �������. ",
                "����� �������� �� ������. ",
                "�� ������ ����� ��������. "
                });
            tmp += ChooseRandomString(MyVariables.GetGen(),
                { "�������� ��� ���",
                "��� ��� ",
                "����� ����������� ��� ��� "
                });
            response.SetText(tmp);
            response.SetTts(tmp);
            return;
        }
        if (MyVariables.GetUser().IsNeeded(MyVariables.GetPos(), pos))
        {
            tmp = ChooseRandomString(MyVariables.GetGen(),
                { "����� �������� ����������. ",
                "������ ����� �������� �� �����. ",
                "����� ������� ���������. ",
                "����� ������ ����� ��������?. "
                });
            tmp += ChooseRandomString(MyVariables.GetGen(),
                { "�������� ��� ���",
                "��� ��� ",
                "����� ����������� ��� ��� "
                });
            response.SetText(tmp);
            response.SetTts(tmp);
            return;
        }
        MyVariables.GetUser().Place(MyVariables.GetPos(), pos);
        if (MyVariables.GetUser().TotalShips() < 10)
        {
            tmp += ". ��������";
            tmp += std::to_string(10 - MyVariables.GetUser().TotalShips());
            tmp += " ��������\n";
            MyVariables.SetStage(MANUAL);
            response.SetText(tmp);
            response.SetTts(tmp);
            return;
        }
        tmp = ChooseRandomString(MyVariables.GetGen(),
            { "�������� ������. ",
            "���, ������� �����������.",
            "��� ������� ����������� ",
            "������� ���������. "
            });
        tmp += ChooseRandomString(MyVariables.GetGen(),
            { "�������-�� ��� ���������",
            "����� �� ���������� �������� ",
            "�������� ������",
            "������� ������"
            });
        response.SetText(tmp);
        response.SetTts(tmp);
        Alice::Button Button("�������", { "useless_button" }, true);
        response.PushButton(Button);
        MyVariables.SetStage(DECIDE);
        return;
    }
    std::mt19937 gen(info.second);
    std::string tmp = ChooseRandomString(gen,
        { "��� ���?\n",
        "������. ��� ���?\n",
        "���-���?\n",
        "��� ������ ����� �����\n",
        "��� ����� �����?\n",
        "�? � ����������, ��������\n",
        "� ������ ��� ����� ����������\n"
        });
    response.SetText(tmp);
    response.SetTts(tmp);
}

void StDecide(const Alice::Request& request, Alice::Response& response,
    const std::pair<stages, size_t>& info) {
    std::mt19937 gen(info.second);
    std::string tmp = ChooseRandomString(gen,
        { "� ���������� �� ����������\n",
        "������ ��� �� ���� ����������\n",
        "�� ����������, � ������ �� ���������\n",
        "������ � ������ ��� ����� ��� ���� ������� � �� ������\n"
        });
    tmp += "\n����� ����� ��� ����� ������ ������. ���� ��� �����?";
    response.SetText(tmp);
    response.SetTts(tmp);
    Alice::Button Bird("����", { "bird" }, true);
    response.PushButton(Bird);
    Alice::Button NotBird("�����", { "not_bird" }, true);
    response.PushButton(NotBird);
    SaveStageAndGen(BEGIN, info.second);
}

void StBegin(const Alice::Request& request, Alice::Response& response,
    const std::pair<stages, size_t>& info) {
    Case MyVariables;
    std::string tmp = request.Payload().dump();
    std::string tts = "����� � ������� ";
    if (tmp == "bird" || tmp == "not_bird")
    {
        if (tmp == "bird")
        {
            tts += "�����\n";
        }
        else
        {
            tts += "����\n";
        }
        if (tmp == ChooseRandomString(MyVariables.GetGen(), { "bird", "not_bird" }))
        {
            tmp = tts;
            tmp += ChooseRandomString(MyVariables.GetGen(),
                { "\n������� ������� ��\n\n",
                "\n��� �� ������\n\n",
                "\n� ��������� ��� ������� �\n\n",
                "\n����� ���\n\n"
                });
            std::string choise = ChooseRandomString(MyVariables.GetGen(),
                { "\n���� ��������?",
                "\n���� ����?",
                "\n���� �������?",
                "\n�� ����� ������� �������?",
                "\n�� ����� ������� ����?",
                "\n�� ����� ������� ��������?"
                });
            response.SetTts(tmp + choise);
            response.SetText(tmp + MyVariables.GetBot().OuterToString() + choise);
            MyVariables.SetStage(PLAYER);
            return;
        }
        tmp = tts;
        tmp += ChooseRandomString(MyVariables.GetGen(),
            { "� ��������!",
            "����� �� ���� �������",
            "� ��������� ��� �������",
            "� ����"
            });
        response.SetTts(tmp);
        response.SetText(tmp);
        Alice::Button Boop("��!", { "useless_button" }, true);
        response.PushButton(Boop);
        MyVariables.SetStage(ALICE);
        return;
    }
    tmp = ChooseRandomString(MyVariables.GetGen(),
        { "���-���? ��� ������? �����!",
        "� ������, � �� �������� � �������",
        "������ �� ������, � ������ ���� ������",
        "����� � ����, �� �� ������?"
        });
    response.SetTts(tmp);
    response.SetText(tmp);
    Alice::Button Boop("��!", { "useless_button" }, true);
    response.PushButton(Boop);
    MyVariables.SetStage(ALICE);
}

void StAlice(const Alice::Request& request, Alice::Response& response,
    const std::pair<stages, size_t>& info) {
    Case MyVariables;
    size_t health = MyVariables.GetUser().TotalHealth();
    size_t amount = MyVariables.GetUser().TotalShips();
    std::string tmp;
    if (request.RequestType() != Alice::Request::Type::ButtonPressed)
    {
        tmp = ChooseRandomString(MyVariables.GetGen(),
            { "��� �������!\n",
            "������ � �������!\n",
            "���� �� �������?\n",
            "������� ��� �������\n",
            "������ ��� �������?\n"
            });
    }
    auto seq = MyVariables.GetSequence();
    if (seq.second.size() == 0)
    {
        MyVariables.SetSequence(MyVariables.GetUser().ShootRand());
    }
    else
    {
        MyVariables.SetSequence(MyVariables.GetUser().ContinueShooting(seq.first, seq.second));
    }
    std::string field = MyVariables.GetUser().OuterToString();
    std::string tts;
    if (MyVariables.GetUser().TotalShips() < amount)
    {
        auto dif = amount - MyVariables.GetUser().TotalShips();
        if (dif > 3)
        {
            tts = ChooseRandomString(MyVariables.GetGen(),
                { "\n� ��������\n",
                "\n��, ���, � �������?\n",
                "\nEouf\n",
                "\n...\n"
                });
        }
        else if (dif == 3)
        {
            tts = ChooseRandomString(MyVariables.GetGen(),
                { "\n���!\n",
                "\n����� ����� �������?\n",
                "\n� ���� �� ������ ������� ������\n",
                "\n��� ���������� �����\n",
                "\n��� ��...\n"
                });
        }
        else if (dif == 2)
        {
            tts = ChooseRandomString(MyVariables.GetGen(),
                { "\n���!\n",
                "\n����!\n",
                "\n������\n",
                "\n� ���������\n",
                "\n������� � ������� �� ������������\n"
                });
        }
        else
        {
            tts = ChooseRandomString(MyVariables.GetGen(),
                { "\n��, ���� ����\n",
                "\n�����!\n",
                "\n��� ������\n",
                "\n������\n",
                "\n�������� � ���������...\n"
                });
        }
    }
    else if (MyVariables.GetUser().TotalHealth() < health)
    {
        tts = ChooseRandomString(MyVariables.GetGen(),
            { "\n�������, ������!\n",
            "\n� ������\n",
            "\n���������!\n",
            "\n� �����\n",
            "\n� �������\n",
            "\n������!\n",
            "\n������ ���������!\n"
            });
    }
    else
    {
        tts = ChooseRandomString(MyVariables.GetGen(),
            { "\n������\n",
            "\n������\n",
            "\n� ��������� ��� ������\n",
            "\n��\n",
            "\n������� ������\n"
            });
    }
    if (MyVariables.GetUser().IsLoser())
    {
        std::string ttsA = ChooseRandomString(MyVariables.GetGen(),
            { "\n�� ��������!\n",
            "\n�������� �� ��������, � ��������!\n",
            "\n� ��������!",
            "\n������� ���, �� ������ �� ����\n",
            "\n������ �� ����!",
            "\n��� ���� ������. � ��������\n"
            });
        ttsA += ChooseRandomString(MyVariables.GetGen(),
            { "\n������� ��� ���?\n",
            "\n������ ������� ��� ���?\n",
            "\n����� ������� ��� ���!\n",
            "\n����� ���!\n",
            "\n����� ��� �����?",
            "\n����� ��� �����. �� ����������!\n"
            });
        std::string resp = ChooseRandomString(MyVariables.GetGen(),
            { "\n��!\n",
            "\n�����!\n",
            "\n������\n"
            });
        Alice::Button Boop(resp, { "Y" }, true);
        response.PushButton(Boop);
        std::string noresp = ChooseRandomString(MyVariables.GetGen(),
            { "\n���!\n",
            "\n������ �� ��� ���!\n",
            "\n��� ��!\n"
            });
        Alice::Button NotBoop(noresp, { "N" }, true);
        response.PushButton(NotBoop);
        response.SetTts(tmp + tts + ttsA);
        response.SetText(tmp + field + tts + ttsA);
        MyVariables.SetStage(TRYAGAIN);
        return;
    }
    std::string fieldA = MyVariables.GetBot().OuterToString();
    std::string ttsA = ChooseRandomString(MyVariables.GetGen(),
        { "\n���� ��������?",
        "\n���� ����?",
        "\n���� �������?",
        "\n�� ����� ������� �������?",
        "\n�� ����� ������� ����?",
        "\n�� ����� ������� ��������?"
        });
    response.SetTts(tmp + tts + ttsA);
    response.SetText(tmp + field + tts + fieldA + ttsA);
    MyVariables.SetStage(PLAYER);
}

void StPlayer(const Alice::Request& request, Alice::Response& response,
    const std::pair<stages, size_t>& info) {
    auto pos = GetPos(request.Command());
    if (pos.first != 0 && pos.second != 0)
    {
        Case MyVariables;
        std::string tmp = MyVariables.GetBot().ShipName(pos);
        if (MyVariables.GetBot().Shoot(pos))
        {
            if (!tmp.empty() && MyVariables.GetBot().IsDead(tmp))
            {
                tmp += " ���������!\n";
                tmp += ChooseRandomString(MyVariables.GetGen(),
                    { "\n���� ��������?",
                    "\n���� ����?",
                    "\n���� �������?",
                    "\n�� ����� ������� �������?",
                    "\n�� ����� ������� ����?",
                    "\n�� ����� ������� ��������?"
                    });
                response.SetText(tmp);
                response.SetTts(tmp);
                return;
            }
            tmp = "���������!\n";
            tmp += ChooseRandomString(MyVariables.GetGen(),
                { "\n���� ��������?",
                "\n���� ����?",
                "\n���� �������?",
                "\n�� ����� ������� �������?",
                "\n�� ����� ������� ����?",
                "\n�� ����� ������� ��������?"
                });
            response.SetText(tmp);
            response.SetTts(tmp);
            return;
        }
        tmp = "������!\n";
        response.SetText(tmp);
        response.SetTts(tmp);
        Alice::Button Boop("��!", { "useless_button" }, true);
        response.PushButton(Boop);
        MyVariables.SetStage(ALICE);
        return;
    }
    std::mt19937 gen(info.second);
    std::string tmp = ChooseRandomString(gen,
        { "��� ���?\n",
        "������. ��� ���?\n",
        "���-���?\n",
        "� ��������-�� ����?\n",
        "��� ����� �����?\n",
        "�? � ����������, ��������\n",
        "���?\n"
        });
    response.SetText(tmp);
    response.SetTts(tmp);
}

void StTryAgain(const Alice::Request& request, Alice::Response& response,
    const std::pair<stages, size_t>& info) {
    std::mt19937 gen(info.second);
    std::string tmp = request.Payload().dump();
    if (tmp == "Y")
    {
        tmp = ChooseRandomString(gen,
            { "��� ���� ������ ����� �����?",
            "������� ����� ���������",
            "����� ����� ����� ���?"
            });
        response.SetTts(tmp);
        response.SetText(tmp);
        ClearFiles();
        SaveStageAndGen(GREET, info.second);
        return;
    }
    tmp = ChooseRandomString(gen,
        { "�� � �����. ����!",
        "�� ��������",
        "�� �������"
        });
    response.SetTts(tmp);
    response.SetText(tmp);
    ClearFiles();
    response.SetEndSession(true);
}

void MyCallback(const Alice::Request& request, Alice::Response& response)
{
    response.SetEndSession(false);
    auto info = ReadStageAndGen();
    if (IsUnexpectedlyEmpty(request, response)) {
        HandleEmpty(request, response);
        return;
    }
    switch (info.first)
    {
    case NAME:
        StName(request, response, info);
        break;
    case GREET:
        StGreet(request, response, info);
        break;
    case PLACEMENT:
        StPlacement(request, response, info);
        break;
    case MANUAL:
        StManual(request, response, info);
        break;
    case MANUAL2:
        StManual2(request, response, info);
        break;
    case DECIDE:
        StDecide(request, response, info);
        break;
    case BEGIN:
        StBegin(request, response, info);
        break;
    case ALICE:
        StAlice(request, response, info);
        break;
    case PLAYER:
        StPlayer(request, response, info);
        break;
    case TRYAGAIN:
        StTryAgain(request, response, info);
        break;
    }
}

int main()
{
    Skill battleships;
    battleships.SetCallback(MyCallback);
    battleships.Run();
    return 0;
}