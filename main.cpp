#include <SFML/Graphics.hpp>
#include <iostream>
#include <queue>
#include <vector>
#include <stack>
#include <chrono>
#include <climits>
#include <string>
#include <algorithm>
#include <functional> // Added for std::function

// Constants
const int ROWS = 16;
const int COLS = 16;
const int TILE_SIZE = 32;
const int TOP_BAR_HEIGHT = 40;
const int WINDOW_WIDTH = COLS * TILE_SIZE;
const int WINDOW_HEIGHT = ROWS * TILE_SIZE + TOP_BAR_HEIGHT;

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
        // Status text
        stageText.setFont(font);
        stageText.setCharacterSize(16);
        stageText.setFillColor(sf::Color::Black);
        stageText.setPosition(10, 5);

        // Time text
        timeText.setFont(font);
        timeText.setCharacterSize(14);
        timeText.setFillColor(sf::Color::Black);
        timeText.setPosition(10, 22);

        // Find Path button
        findPathButton.setSize(sf::Vector2f(120, 30));
        findPathButton.setFillColor(sf::Color::Green);
        findPathButton.setPosition(WINDOW_WIDTH - 250, 5);

        findPathButtonText.setFont(font);
        findPathButtonText.setString("Find Path");
        findPathButtonText.setCharacterSize(14);
        findPathButtonText.setFillColor(sf::Color::White);
        findPathButtonText.setPosition(WINDOW_WIDTH - 232, 12);

        // Reset button
        resetButton.setSize(sf::Vector2f(100, 30));
        resetButton.setFillColor(sf::Color::Red);
        resetButton.setPosition(WINDOW_WIDTH - 120, 5);

        resetButtonText.setFont(font);
        resetButtonText.setString("Reset");
        resetButtonText.setCharacterSize(14);
        resetButtonText.setFillColor(sf::Color::White);
        resetButtonText.setPosition(WINDOW_WIDTH - 95, 12);

        // Algorithm selection button
        algorithmButton.setSize(sf::Vector2f(120, 30));
        algorithmButton.setFillColor(sf::Color::Blue);
        algorithmButton.setPosition(WINDOW_WIDTH - 390, 5);

        algorithmButtonText.setFont(font);
        algorithmButtonText.setString("Algorithm: DFS");
        algorithmButtonText.setCharacterSize(14);
        algorithmButtonText.setFillColor(sf::Color::White);
        algorithmButtonText.setPosition(WINDOW_WIDTH - 380, 12);
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
        std::vector<sf::Vector2i> pathStack;

        // Using iterative DFS instead of recursive to avoid function declaration issues
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

        // Handle grid cell clicks
        if (mouseY >= TOP_BAR_HEIGHT) {
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
        window.clear(sf::Color::White);

        // Draw grid cells
        for (int y = 0; y < ROWS; ++y) {
            for (int x = 0; x < COLS; ++x) {
                sf::RectangleShape cell(sf::Vector2f(TILE_SIZE - 1, TILE_SIZE - 1));
                cell.setPosition(x * TILE_SIZE, y * TILE_SIZE + TOP_BAR_HEIGHT);

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

            sf::RectangleShape pathCell(sf::Vector2f(TILE_SIZE - 1, TILE_SIZE - 1));
            pathCell.setPosition(p.x * TILE_SIZE, p.y * TILE_SIZE + TOP_BAR_HEIGHT);
            pathCell.setFillColor(sf::Color::Blue);
            window.draw(pathCell);
        }

        // Draw UI elements
        window.draw(stageText);
        window.draw(timeText);
        window.draw(findPathButton);
        window.draw(findPathButtonText);
        window.draw(resetButton);
        window.draw(resetButtonText);
        window.draw(algorithmButton);
        window.draw(algorithmButtonText);
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