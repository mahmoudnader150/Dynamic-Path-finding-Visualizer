#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <chrono>
#include "Constants.h"
#include "Algorithms.h"
#include "UIComponents.h"

class PathfindingVisualizer {
private:
    int grid[Constants::ROWS][Constants::COLS];
    sf::Vector2i startCell;
    sf::Vector2i goalCell;
    std::vector<sf::Vector2i> path;
    int stage;
    bool pathFound;
    std::string statusMessage;
    sf::Font font;
    Constants::AlgorithmType currentAlgorithm;
    float executionTime;

    // UI Components
    sf::Text stageText;
    sf::Text timeText;
    sf::Text algorithmText;
    sf::RectangleShape findPathButton;
    sf::Text findPathButtonText;
    sf::RectangleShape resetButton;
    sf::Text resetButtonText;
    sf::RectangleShape algorithmButton;
    sf::Text algorithmButtonText;

    void setupUI();
    void updateUI();
    void reset();
    void findPath();
    void cycleAlgorithm();

public:
    PathfindingVisualizer();
    void handleMouseClick(int mouseX, int mouseY);
    void render(sf::RenderWindow& window);
};