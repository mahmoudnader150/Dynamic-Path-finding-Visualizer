#include "PathfindingVisualizer.h"
#include <iostream>

PathfindingVisualizer::PathfindingVisualizer() :
    startCell(-1, -1),
    goalCell(-1, -1),
    stage(1),
    pathFound(false),
    currentAlgorithm(Constants::DFS),
    executionTime(0) {

    // Initialize grid
    for (int y = 0; y < Constants::ROWS; ++y) {
        for (int x = 0; x < Constants::COLS; ++x) {
            grid[y][x] = Constants::EMPTY;
        }
    }

    // Load font
    if (!font.loadFromFile("DejaVuSans-Bold.ttf")) {
        std::cerr << "Failed to load font!" << std::endl;
        // Fallback to system font or exit
        if (!font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf")) {
            std::cerr << "Failed to load system font as well!" << std::endl;
        }
    }

    setupUI();
    updateUI();
}

void PathfindingVisualizer::setupUI() {
    // Status text - positioned in bottom bar
    stageText = UIComponents::createText(font, "", 18, sf::Color::Black,
                                        10, Constants::TOP_BAR_HEIGHT + Constants::ROWS * Constants::TILE_SIZE + 10);

    // Time text - positioned in bottom bar
    timeText = UIComponents::createText(font, "", 16, sf::Color::Black,
                                       Constants::WINDOW_WIDTH - 250,
                                       Constants::TOP_BAR_HEIGHT + Constants::ROWS * Constants::TILE_SIZE + 10);

    // Find Path button
    findPathButton = UIComponents::createButton(10, 10, 130, 40, sf::Color::Green, sf::Color(0, 100, 0));
    findPathButtonText = UIComponents::createText(font, "Find Path", 16, sf::Color::White, 30, 20);

    // Reset button
    resetButton = UIComponents::createButton(Constants::WINDOW_WIDTH / 2 - 55, 10, 110, 40,
                                           sf::Color::Red, sf::Color(139, 0, 0));
    resetButtonText = UIComponents::createText(font, "Reset", 16, sf::Color::White,
                                             Constants::WINDOW_WIDTH / 2 - 30, 20);

    // Algorithm selection button
    algorithmButton = UIComponents::createButton(Constants::WINDOW_WIDTH - 160, 10, 150, 40,
                                               sf::Color::Blue, sf::Color(0, 0, 139));
    algorithmButtonText = UIComponents::createText(font, "Algorithm: DFS", 16, sf::Color::White,
                                                 Constants::WINDOW_WIDTH - 150, 20);
}

void PathfindingVisualizer::updateUI() {
    // Update stage text
    switch (stage) {
        case 1:
            stageText.setString("Select start cell");
            break;
        case 2:
            stageText.setString("Select goal cell");
            break;
        case 3:
            stageText.setString("Add walls, then find path");
            break;
        case 4:
            stageText.setString(statusMessage);
            break;
    }

    // Update algorithm text
    std::string algoName;
    switch (currentAlgorithm) {
        case Constants::DFS: algoName = "DFS"; break;
        case Constants::BFS: algoName = "BFS"; break;
        case Constants::DIJKSTRA: algoName = "Dijkstra"; break;
    }
    algorithmButtonText.setString("Algorithm: " + algoName);

    // Update time text
    if (stage == 4) {
        timeText.setString("Execution time: " + std::to_string(executionTime) + " ms");
    } else {
        timeText.setString("");
    }
}

void PathfindingVisualizer::reset() {
    for (int y = 0; y < Constants::ROWS; ++y) {
        for (int x = 0; x < Constants::COLS; ++x) {
            grid[y][x] = Constants::EMPTY;
        }
    }
    startCell = {-1, -1};
    goalCell = {-1, -1};
    path.clear();
    stage = 1;
    pathFound = false;
    statusMessage = "";
    executionTime = 0;
    updateUI();
}

void PathfindingVisualizer::findPath() {
    if (startCell.x == -1 || goalCell.x == -1) {
        statusMessage = "Set start and goal cells first!";
        return;
    }

    path.clear();
    auto startTime = std::chrono::steady_clock::now();

    switch (currentAlgorithm) {
        case Constants::DFS:
            pathFound = Algorithms::depthFirstSearch(grid, startCell, goalCell, path);
            break;
        case Constants::BFS:
            pathFound = Algorithms::breadthFirstSearch(grid, startCell, goalCell, path);
            break;
        case Constants::DIJKSTRA:
            pathFound = Algorithms::dijkstraAlgorithm(grid, startCell, goalCell, path);
            break;
    }

    auto endTime = std::chrono::steady_clock::now();
    std::chrono::duration<float, std::milli> duration = endTime - startTime;
    executionTime = duration.count();

    statusMessage = pathFound ? "Path found!" : "No path exists!";
    stage = 4;
    updateUI();
}

void PathfindingVisualizer::cycleAlgorithm() {
    switch (currentAlgorithm) {
        case Constants::DFS: currentAlgorithm = Constants::BFS; break;
        case Constants::BFS: currentAlgorithm = Constants::DIJKSTRA; break;
        case Constants::DIJKSTRA: currentAlgorithm = Constants::DFS; break;
    }
    updateUI();
}

void PathfindingVisualizer::handleMouseClick(int mouseX, int mouseY) {
    // Check if buttons were clicked
    if (UIComponents::isButtonClicked(resetButton, mouseX, mouseY)) {
        reset();
        return;
    }

    if (UIComponents::isButtonClicked(findPathButton, mouseX, mouseY)) {
        findPath();
        return;
    }

    if (UIComponents::isButtonClicked(algorithmButton, mouseX, mouseY)) {
        cycleAlgorithm();
        return;
    }

    // Handle grid cell clicks - only if within the grid area (not in top or bottom bars)
    if (mouseY >= Constants::TOP_BAR_HEIGHT && mouseY < (Constants::TOP_BAR_HEIGHT + Constants::ROWS * Constants::TILE_SIZE)) {
        int gridX = mouseX / Constants::TILE_SIZE;
        int gridY = (mouseY - Constants::TOP_BAR_HEIGHT) / Constants::TILE_SIZE;

        if (gridX >= 0 && gridX < Constants::COLS && gridY >= 0 && gridY < Constants::ROWS) {
            switch (stage) {
                case 1: // Set start cell
                    if (startCell.x != -1) {
                        grid[startCell.y][startCell.x] = Constants::EMPTY;
                    }
                    grid[gridY][gridX] = Constants::START;
                    startCell = {gridX, gridY};
                    stage = 2;
                    break;

                case 2: // Set goal cell
                    if (gridY == startCell.y && gridX == startCell.x) {
                        return; // Can't set goal same as start
                    }
                    if (goalCell.x != -1) {
                        grid[goalCell.y][goalCell.x] = Constants::EMPTY;
                    }
                    grid[gridY][gridX] = Constants::GOAL;
                    goalCell = {gridX, gridY};
                    stage = 3;
                    break;

                case 3: // Add/remove walls
                case 4: // After path finding
                    if ((gridY == startCell.y && gridX == startCell.x) ||
                        (gridY == goalCell.y && gridX == goalCell.x)) {
                        return; // Can't modify start or goal
                    }
                    grid[gridY][gridX] = (grid[gridY][gridX] == Constants::WALL) ? Constants::EMPTY : Constants::WALL;
                    if (stage == 4) {
                        path.clear(); // Clear the path if walls change
                        stage = 3;
                    }
                    break;
            }
            updateUI();
        }
    }
}

void PathfindingVisualizer::render(sf::RenderWindow& window) {
    window.clear(sf::Color(240, 240, 240)); // Light gray background

    // Draw the top bar background
    sf::RectangleShape topBar(sf::Vector2f(Constants::WINDOW_WIDTH, Constants::TOP_BAR_HEIGHT));
    topBar.setFillColor(sf::Color(220, 220, 220)); // Slightly darker gray for top bar
    topBar.setPosition(0, 0);
    window.draw(topBar);

    // Draw the bottom bar background
    sf::RectangleShape bottomBar(sf::Vector2f(Constants::WINDOW_WIDTH, Constants::BOTTOM_BAR_HEIGHT));
    bottomBar.setFillColor(sf::Color(220, 220, 220)); // Same color as top bar
    bottomBar.setPosition(0, Constants::TOP_BAR_HEIGHT + Constants::ROWS * Constants::TILE_SIZE);
    window.draw(bottomBar);

    // Draw grid cells with borders
    for (int y = 0; y < Constants::ROWS; ++y) {
        for (int x = 0; x < Constants::COLS; ++x) {
            sf::RectangleShape cell(sf::Vector2f(Constants::TILE_SIZE - 2, Constants::TILE_SIZE - 2)); // Slightly smaller to show borders
            cell.setPosition(x * Constants::TILE_SIZE + 1, y * Constants::TILE_SIZE + 1 + Constants::TOP_BAR_HEIGHT); // +1 for border
            cell.setOutlineThickness(1); // Add border
            cell.setOutlineColor(sf::Color::Black); // Black border

            // Set cell color based on type
            switch (grid[y][x]) {
                case Constants::EMPTY: cell.setFillColor(sf::Color::White); break;
                case Constants::WALL: cell.setFillColor(sf::Color::Black); break;
                case Constants::START: cell.setFillColor(sf::Color::Green); break;
                case Constants::GOAL: cell.setFillColor(sf::Color::Red); break;
                case Constants::PATH: cell.setFillColor(sf::Color::Blue); break;
            }

            window.draw(cell);
        }
    }

    // Draw path
    for (auto& p : path) {
        // Don't draw path over start or goal
        if ((p.x == startCell.x && p.y == startCell.y) ||
            (p.x == goalCell.x && p.y == goalCell.y)) {
            continue;
        }

        sf::RectangleShape pathCell(sf::Vector2f(Constants::TILE_SIZE - 2, Constants::TILE_SIZE - 2));
        pathCell.setPosition(p.x * Constants::TILE_SIZE + 1, p.y * Constants::TILE_SIZE + 1 + Constants::TOP_BAR_HEIGHT);
        pathCell.setFillColor(sf::Color(100, 149, 237)); // Cornflower blue for path
        pathCell.setOutlineThickness(1);
        pathCell.setOutlineColor(sf::Color::Black);
        window.draw(pathCell);
    }

    // Draw UI elements
    window.draw(findPathButton);
    window.draw(findPathButtonText);
    window.draw(resetButton);
    window.draw(resetButtonText);
    window.draw(algorithmButton);
    window.draw(algorithmButtonText);
    window.draw(stageText);
    window.draw(timeText);
}