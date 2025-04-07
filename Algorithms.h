#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <queue>
#include <stack>
#include <climits>
#include <algorithm>
#include <functional>
#include "Constants.h"

class Algorithms {
public:
    static bool depthFirstSearch(int grid[Constants::ROWS][Constants::COLS], 
                                sf::Vector2i startCell, 
                                sf::Vector2i goalCell, 
                                std::vector<sf::Vector2i>& path);
    
    static bool breadthFirstSearch(int grid[Constants::ROWS][Constants::COLS], 
                                  sf::Vector2i startCell, 
                                  sf::Vector2i goalCell, 
                                  std::vector<sf::Vector2i>& path);
    
    static bool dijkstraAlgorithm(int grid[Constants::ROWS][Constants::COLS], 
                                 sf::Vector2i startCell, 
                                 sf::Vector2i goalCell, 
                                 std::vector<sf::Vector2i>& path);

private:
    static bool isValidCell(int x, int y);
};