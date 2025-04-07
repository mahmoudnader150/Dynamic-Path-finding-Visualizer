#include <SFML/Graphics.hpp>
#include <iostream>
#include <queue>
#include <vector>
#include <chrono>
#include <climits>
#include <string>
#include <algorithm>
#include <functional>
#include <stack>

// Constants
const int ROWS = 16;
const int COLS = 16;
const int TILE_SIZE = 32;
const int TOP_BAR_HEIGHT = 60; // Top bar for buttons
const int BOTTOM_BAR_HEIGHT = 40; // New bottom bar for text
const int WINDOW_WIDTH = COLS * TILE_SIZE;
const int WINDOW_HEIGHT = ROWS * TILE_SIZE + TOP_BAR_HEIGHT + BOTTOM_BAR_HEIGHT; // Adjusted for bottom bar

enum CellType {
    EMPTY = 0,
    WALL = 1,
    START = 2,
    GOAL = 3,
    PATH = 4
};

enum AlgorithmType {
    DFS,
    BFS,
    DIJKSTRA
};

// Directions: up, right, down, left
const std::vector<sf::Vector2i> DIRECTIONS = {
    {0, -1}, {1, 0}, {0, 1}, {-1, 0}
};

class PathfindingVisualizer {
private:
    int grid[ROWS][COLS];
    sf::Vector2i startCell;
    sf::Vector2i goalCell;
    std::vector<sf::Vector2i> path;
    int stage;
    bool pathFound;
    std::string statusMessage;
    sf::Font font;
    AlgorithmType currentAlgorithm;
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

    bool isValidCell(int x, int y) {
        return x >= 0 && x < COLS && y >= 0 && y < ROWS;
    }

    bool isButtonClicked(const sf::RectangleShape& button, int mouseX, int mouseY) {
        sf::FloatRect bounds = button.getGlobalBounds();
        return bounds.contains(mouseX, mouseY);
    }

    void setupUI() {
        // Status text - positioned in bottom bar
        stageText.setFont(font);
        stageText.setCharacterSize(18);
        stageText.setFillColor(sf::Color::Black);
        stageText.setPosition(10, TOP_BAR_HEIGHT + ROWS * TILE_SIZE + 10); // In bottom bar

        // Time text - positioned in bottom bar
        timeText.setFont(font);
        timeText.setCharacterSize(16);
        timeText.setFillColor(sf::Color::Black);
        timeText.setPosition(WINDOW_WIDTH - 250, TOP_BAR_HEIGHT + ROWS * TILE_SIZE + 10); // In bottom bar

        // Find Path button
        findPathButton.setSize(sf::Vector2f(130, 40));
        findPathButton.setFillColor(sf::Color::Green);
        findPathButton.setPosition(10, 10); // Top left
        findPathButton.setOutlineThickness(2);
        findPathButton.setOutlineColor(sf::Color(0, 100, 0));

        findPathButtonText.setFont(font);
        findPathButtonText.setString("Find Path");
        findPathButtonText.setCharacterSize(16);
        findPathButtonText.setFillColor(sf::Color::White);
        findPathButtonText.setPosition(30, 20); // Centered in button

        // Reset button
        resetButton.setSize(sf::Vector2f(110, 40));
        resetButton.setFillColor(sf::Color::Red);
        resetButton.setPosition(WINDOW_WIDTH / 2 - 55, 10); // Top middle
        resetButton.setOutlineThickness(2);
        resetButton.setOutlineColor(sf::Color(139, 0, 0));

        resetButtonText.setFont(font);
        resetButtonText.setString("Reset");
        resetButtonText.setCharacterSize(16);
        resetButtonText.setFillColor(sf::Color::White);
        resetButtonText.setPosition(WINDOW_WIDTH / 2 - 30, 20); // Centered in button

        // Algorithm selection button
        algorithmButton.setSize(sf::Vector2f(150, 40));
        algorithmButton.setFillColor(sf::Color::Blue);
        algorithmButton.setPosition(WINDOW_WIDTH - 160, 10); // Top right
        algorithmButton.setOutlineThickness(2);
        algorithmButton.setOutlineColor(sf::Color(0, 0, 139));

        algorithmButtonText.setFont(font);
        algorithmButtonText.setString("Algorithm: DFS");
        algorithmButtonText.setCharacterSize(16);
        algorithmButtonText.setFillColor(sf::Color::White);
        algorithmButtonText.setPosition(WINDOW_WIDTH - 150, 20); // Centered in button
    }

    void updateUI() {
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
            case DFS: algoName = "DFS"; break;
            case BFS: algoName = "BFS"; break;
            case DIJKSTRA: algoName = "Dijkstra"; break;
        }
        algorithmButtonText.setString("Algorithm: " + algoName);

        // Update time text
        if (stage == 4) {
            timeText.setString("Execution time: " + std::to_string(executionTime) + " ms");
        } else {
            timeText.setString("");
        }
    }

    void reset() {
        for (int y = 0; y < ROWS; ++y) {
            for (int x = 0; x < COLS; ++x) {
                grid[y][x] = EMPTY;
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

    bool depthFirstSearch() {
        path.clear();
        std::vector<std::vector<bool>> visited(ROWS, std::vector<bool>(COLS, false));
        std::stack<sf::Vector2i> stack;
        std::vector<std::vector<sf::Vector2i>> parent(ROWS, std::vector<sf::Vector2i>(COLS, {-1, -1}));

        stack.push(startCell);
        visited[startCell.y][startCell.x] = true;

        while (!stack.empty()) {
            sf::Vector2i current = stack.top();
            stack.pop();

            if (current.x == goalCell.x && current.y == goalCell.y) {
                // Reconstruct path
                sf::Vector2i pos = goalCell;
                while (pos.x != startCell.x || pos.y != startCell.y) {
                    path.push_back(pos);
                    pos = parent[pos.y][pos.x];
                }
                path.push_back(startCell);
                std::reverse(path.begin(), path.end());
                return true;
            }

            // Try all possible directions
            for (auto& dir : DIRECTIONS) {
                int newX = current.x + dir.x;
                int newY = current.y + dir.y;

                if (isValidCell(newX, newY) && !visited[newY][newX] && grid[newY][newX] != WALL) {
                    stack.push({newX, newY});
                    visited[newY][newX] = true;
                    parent[newY][newX] = current;
                }
            }
        }

        return false;
    }

    bool breadthFirstSearch() {
        path.clear();
        std::vector<std::vector<bool>> visited(ROWS, std::vector<bool>(COLS, false));
        std::vector<std::vector<sf::Vector2i>> parent(ROWS, std::vector<sf::Vector2i>(COLS, {-1, -1}));
        std::queue<sf::Vector2i> queue;

        queue.push(startCell);
        visited[startCell.y][startCell.x] = true;

        while (!queue.empty()) {
            sf::Vector2i current = queue.front();
            queue.pop();

            if (current.x == goalCell.x && current.y == goalCell.y) {
                // Reconstruct path
                sf::Vector2i pos = goalCell;
                while (pos.x != startCell.x || pos.y != startCell.y) {
                    path.push_back(pos);
                    pos = parent[pos.y][pos.x];
                }
                path.push_back(startCell);
                std::reverse(path.begin(), path.end());
                return true;
            }

            for (auto& dir : DIRECTIONS) {
                int newX = current.x + dir.x;
                int newY = current.y + dir.y;

                if (isValidCell(newX, newY) && !visited[newY][newX] && grid[newY][newX] != WALL) {
                    queue.push({newX, newY});
                    visited[newY][newX] = true;
                    parent[newY][newX] = current;
                }
            }
        }

        return false;
    }

    bool dijkstraAlgorithm() {
        path.clear();

        // Node struct for priority queue
        struct Node {
            int x, y, cost;
            bool operator>(const Node& other) const {
                return cost > other.cost;
            }
        };

        std::priority_queue<Node, std::vector<Node>, std::greater<Node>> pq;
        std::vector<std::vector<int>> distance(ROWS, std::vector<int>(COLS, INT_MAX));
        std::vector<std::vector<sf::Vector2i>> parent(ROWS, std::vector<sf::Vector2i>(COLS, {-1, -1}));

        pq.push({startCell.x, startCell.y, 0});
        distance[startCell.y][startCell.x] = 0;

        while (!pq.empty()) {
            Node current = pq.top();
            pq.pop();

            // If we've reached the goal
            if (current.x == goalCell.x && current.y == goalCell.y) {
                // Reconstruct path
                sf::Vector2i pos = goalCell;
                while (pos.x != startCell.x || pos.y != startCell.y) {
                    path.push_back(pos);
                    pos = parent[pos.y][pos.x];
                }
                path.push_back(startCell);
                std::reverse(path.begin(), path.end());
                return true;
            }

            // If we've already found a better path
            if (current.cost > distance[current.y][current.x]) {
                continue;
            }

            for (auto& dir : DIRECTIONS) {
                int newX = current.x + dir.x;
                int newY = current.y + dir.y;

                if (isValidCell(newX, newY) && grid[newY][newX] != WALL) {
                    int newCost = current.cost + 1;

                    if (newCost < distance[newY][newX]) {
                        distance[newY][newX] = newCost;
                        parent[newY][newX] = {current.x, current.y};
                        pq.push({newX, newY, newCost});
                    }
                }
            }
        }

        return false;
    }

    void findPath() {
        if (startCell.x == -1 || goalCell.x == -1) {
            statusMessage = "Set start and goal cells first!";
            return;
        }

        path.clear();
        auto startTime = std::chrono::steady_clock::now();

        switch (currentAlgorithm) {
            case DFS:
                pathFound = depthFirstSearch();
                break;
            case BFS:
                pathFound = breadthFirstSearch();
                break;
            case DIJKSTRA:
                pathFound = dijkstraAlgorithm();
                break;
        }

        auto endTime = std::chrono::steady_clock::now();
        std::chrono::duration<float, std::milli> duration = endTime - startTime;
        executionTime = duration.count();

        statusMessage = pathFound ? "Path found!" : "No path exists!";
        stage = 4;
        updateUI();
    }

    void cycleAlgorithm() {
        switch (currentAlgorithm) {
            case DFS: currentAlgorithm = BFS; break;
            case BFS: currentAlgorithm = DIJKSTRA; break;
            case DIJKSTRA: currentAlgorithm = DFS; break;
        }
        updateUI();
    }

public:
    PathfindingVisualizer() :
        startCell(-1, -1),
        goalCell(-1, -1),
        stage(1),
        pathFound(false),
        currentAlgorithm(DFS),
        executionTime(0) {

        // Initialize grid
        for (int y = 0; y < ROWS; ++y) {
            for (int x = 0; x < COLS; ++x) {
                grid[y][x] = EMPTY;
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

    void handleMouseClick(int mouseX, int mouseY) {
        // Check if buttons were clicked
        if (isButtonClicked(resetButton, mouseX, mouseY)) {
            reset();
            return;
        }

        if (isButtonClicked(findPathButton, mouseX, mouseY)) {
            findPath();
            return;
        }

        if (isButtonClicked(algorithmButton, mouseX, mouseY)) {
            cycleAlgorithm();
            return;
        }

        // Handle grid cell clicks - only if within the grid area (not in top or bottom bars)
        if (mouseY >= TOP_BAR_HEIGHT && mouseY < (TOP_BAR_HEIGHT + ROWS * TILE_SIZE)) {
            int gridX = mouseX / TILE_SIZE;
            int gridY = (mouseY - TOP_BAR_HEIGHT) / TILE_SIZE;

            if (isValidCell(gridX, gridY)) {
                switch (stage) {
                    case 1: // Set start cell
                        if (startCell.x != -1) {
                            grid[startCell.y][startCell.x] = EMPTY;
                        }
                        grid[gridY][gridX] = START;
                        startCell = {gridX, gridY};
                        stage = 2;
                        break;

                    case 2: // Set goal cell
                        if (gridY == startCell.y && gridX == startCell.x) {
                            return; // Can't set goal same as start
                        }
                        if (goalCell.x != -1) {
                            grid[goalCell.y][goalCell.x] = EMPTY;
                        }
                        grid[gridY][gridX] = GOAL;
                        goalCell = {gridX, gridY};
                        stage = 3;
                        break;

                    case 3: // Add/remove walls
                    case 4: // After path finding
                        if ((gridY == startCell.y && gridX == startCell.x) ||
                            (gridY == goalCell.y && gridX == goalCell.x)) {
                            return; // Can't modify start or goal
                        }
                        grid[gridY][gridX] = (grid[gridY][gridX] == WALL) ? EMPTY : WALL;
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

    void render(sf::RenderWindow& window) {
        window.clear(sf::Color(240, 240, 240)); // Light gray background

        // Draw the top bar background
        sf::RectangleShape topBar(sf::Vector2f(WINDOW_WIDTH, TOP_BAR_HEIGHT));
        topBar.setFillColor(sf::Color(220, 220, 220)); // Slightly darker gray for top bar
        topBar.setPosition(0, 0);
        window.draw(topBar);

        // Draw the bottom bar background
        sf::RectangleShape bottomBar(sf::Vector2f(WINDOW_WIDTH, BOTTOM_BAR_HEIGHT));
        bottomBar.setFillColor(sf::Color(220, 220, 220)); // Same color as top bar
        bottomBar.setPosition(0, TOP_BAR_HEIGHT + ROWS * TILE_SIZE);
        window.draw(bottomBar);

        // Draw grid cells with borders
        for (int y = 0; y < ROWS; ++y) {
            for (int x = 0; x < COLS; ++x) {
                sf::RectangleShape cell(sf::Vector2f(TILE_SIZE - 2, TILE_SIZE - 2)); // Slightly smaller to show borders
                cell.setPosition(x * TILE_SIZE + 1, y * TILE_SIZE + 1 + TOP_BAR_HEIGHT); // +1 for border
                cell.setOutlineThickness(1); // Add border
                cell.setOutlineColor(sf::Color::Black); // Black border

                // Set cell color based on type
                switch (grid[y][x]) {
                    case EMPTY: cell.setFillColor(sf::Color::White); break;
                    case WALL: cell.setFillColor(sf::Color::Black); break;
                    case START: cell.setFillColor(sf::Color::Green); break;
                    case GOAL: cell.setFillColor(sf::Color::Red); break;
                    case PATH: cell.setFillColor(sf::Color::Blue); break;
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

            sf::RectangleShape pathCell(sf::Vector2f(TILE_SIZE - 2, TILE_SIZE - 2)); // Slightly smaller to show borders
            pathCell.setPosition(p.x * TILE_SIZE + 1, p.y * TILE_SIZE + 1 + TOP_BAR_HEIGHT); // +1 for border
            pathCell.setFillColor(sf::Color(100, 149, 237)); // Cornflower blue for path
            pathCell.setOutlineThickness(1); // Keep border
            pathCell.setOutlineColor(sf::Color::Black); // Black border
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
};

int main() {
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Pathfinding Visualizer", sf::Style::Close);
    window.setFramerateLimit(60);

    PathfindingVisualizer visualizer;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                visualizer.handleMouseClick(event.mouseButton.x, event.mouseButton.y);
            }
        }

        visualizer.render(window);
        window.display();
    }

    return 0;
}