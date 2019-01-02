#pragma once

#include <iostream>
#include <cmath>
#include <ctime>
#include <map>
#include <random>
#include <string>
#include <utility>
#include <vector>

#define tile std::pair<size_t, size_t>

class Board
{
private:
// Initialization section.

// Player name.
    std::string name_;
// My FOV.
    char** clear_;
// Enemy FOV.
    char** cloudy_;
// Positions of all battleships. Is not changed during combat for proper working of Destroy().
    std::map<std::string, tile*> positions_;
// Indicates status of all battleships. Ships with 0 health are deleted by Destroy().
    std::map<std::string, size_t> status_;
// As there are lots of random elements i have to make one rd gen to use all the time;
    std::mt19937 gen_;
// Private checks.

// Returns true if position is proper for ship placement. Is used in both random generation and manual placing.
    bool Check(const tile* pos);

// Random generation and ship building.

// Returns "beginning" of ship if poss are generated randomly.
    tile* BuildHeap(const tile* spos, const tile* epos);
// Returns "beginning" of ship if poss are chosen by player.
    tile* BuildStack(const tile& spos, const tile& epos);
// Throws a point on jump distance in a random direction. Returns nullptr if position cant be chosen.
    tile* ThrowRand(const tile* prev, const size_t& jump);
// Picks random position on the map until it is proper.
    tile* PickRand();
// Generate vector of pos + surroundings. Used in random shooting;
    std::vector<tile> GeneratePossible(const tile& pos);

// Utility

// Returns size of ship with beginning in spos and end in epos. Returns 0 if poss are not on the same line.
    size_t Size(const tile* spos, const tile* epos);
// Returns amount of same sized ships in positions_. Is used in maual placing.
    size_t Amount(const size_t& size);

// Marking

// Marks ALL battleships on clear_ matrix.
    void Mark();
// Marks named battleship on clear_ matrix.
    void MarkShip(const std::string& name);
// Destroys named battleship: marks positions around ship on cloudy_ as misses, erases ship from status_ map.
    void Destroy(const std::string& name);
public:
    Board(std::string name);
    Board();
    ~Board();

// Complex funtions

// GeneratesRandom battlefield;
    void RandomScenario();
// Returns true if shot hits or pos is invalid, false if shot misses;
    bool Shoot(const tile& pos);
// Manual placing for ships of any size (<5); Returns false if ship cannot be placed.
    bool Place(const tile& spos, const tile& epos);

// Show functions

// Shows clear_.
    void ShowMyBoard();
// Shows cloudy_.
    void ShowEnemyBoard();
// Shows clear_ with chosen position.
    void ShowTargeted(const tile& pos);
// Shows status. Is not currently used.
    void ShowStatus();
// Returns player name.
    std::string GetName();
// Returns name of the ship with a named tile. Otherwise returns empty string.
    std::string GetShipName(const tile* pos);
// Returns random generator. 
    std::mt19937 GetRdGen();
// Returns amount of ships in positions_.
    size_t ShipsAmount();
// Returms total amount of ship health
    size_t TotalHealth();

// Public checks

// Returns true if position was used before.
    bool WasShot(const tile& pos);
// Returns true is tile is inside matrix bounds.
    bool IsProper(const tile& pos);
// Returns true if health of named ship equals to 0.
    bool IsDead(const std::string& name);

// AI

// Random shooting for AI. Can seek and kill damaged ships. If misses damaged ship returns vector of possile positions.
    std::pair<std::vector<tile>, size_t> ShootRand();
// Randomly chooses position from vector. Can continue seeking after a miss.
    std::pair<std::vector<tile>, size_t> ContinueShooting(std::vector<tile>& positions, size_t& hits);

// Returns std::strings to be used by Alice
    std::string MyBoardToString();
    std::string EnemyBoardToString();
    std::string TargetedToString(const tile& pos);
    std::string StatusToString();
    std::string ComplexCheck(const tile& spos, const tile& epos);
};
