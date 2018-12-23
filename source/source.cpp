#include "Board.h"

bool Check(Board& target, const std::pair<size_t, size_t>& pos)
{
    if (!target.IsProper(pos) || target.WasShot(pos))
    {
        std::cout << "Position is invalid or was shot before" << std::endl;
        return true;
    }
    return false;
}

int main()
{
    std::string playername;
    std::cout << "Enter your name: ";
    std::getline(std::cin, playername);
    Board Player(playername);
    std::pair<size_t, size_t> pos1 = { 0, 0 };
    std::cout << "Choose how to place ships\n0 - Manual\n1 - Random" << std::endl;
    auto pos2 = pos1;
    char lever = '1';
    do
        std::cin >> lever;
    while (lever != '0' && lever != '1');
    switch (lever)
    {
    case '0':
        std::cout << std::endl << "Enter first and last positions of a ship" << std::endl;
        char ch = '0';
        do
        {
            do
            {
                Player.ShowMyBoard();
                std::cout << "First: ";
                std::cin >> pos1.first >> pos1.second;
                Player.ShowTargeted(pos1);
                std::cout << std::endl << "Second: ";
                std::cin >> pos2.first >> pos2.second;
                std::cout << std::endl;
            } while (!Player.Place(pos1, pos2));
        } while (!Player.IsFull());
        break;
    case '1':
        Player.RandomScenario();
        Player.ShowMyBoard();
    }
    Board Bot("Alice");
    Bot.RandomScenario();
    std::pair<std::vector<std::pair<size_t, size_t>>, size_t> tmp_positions = { {}, 0 };
    do
    {
        do
        {
            do
            {
                Bot.ShowEnemyBoard();
                std::cout << "Enter position to shoot!" << std::endl;
                std::cin >> pos1.first >> pos1.second;
            } while (Check(Bot, pos1));
        } while (Bot.Shoot(pos1));
        if (tmp_positions.second == 0)
        {
            tmp_positions = Player.ShootRand();
            Player.ShowEnemyBoard();
        }
        else
        {
            tmp_positions = Player.ContinueShooting(tmp_positions.first, tmp_positions.second);
            Player.ShowEnemyBoard();
        }
    } while (!Bot.IsLooser() || !Player.IsLooser());
    if (Bot.IsLooser())
        std::cout << Player.GetName() << " wins!" << std::endl;
    else
        std::cout << Bot.GetName() << " wins!" << std::endl;
    system("pause");
    return 0;
}
