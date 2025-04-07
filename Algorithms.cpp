#include "Algorithms.h"

bool Algorithms::isValidCell(int x, int y) {
    return x >= 0 && x < Constants::COLS && y >= 0 && y < Constants::ROWS;
}

bool Algorithms::depthFirstSearch(int grid[Constants::ROWS][Constants::COLS], 
                                 sf::Vector2i startCell, 
                                 sf::Vector2i goalCell, 
                                 std::vector<sf::Vector2i>& path) {
    path.clear();
    std::vector<std::vector<bool>> visited(Constants::ROWS, std::vector<bool>(Constants::COLS, false));
    std::stack<sf::Vector2i> stack;
    std::vector<std::vector<sf::Vector2i>> parent(Constants::ROWS, std::vector<sf::Vector2i>(Constants::COLS, {-1, -1}));

    // Directions: up, right, down, left
    const std::vector<sf::Vector2i> DIRECTIONS = {
        {0, -1}, {1, 0}, {0, 1}, {-1, 0}
    };

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

            if (isValidCell(newX, newY) && !visited[newY][newX] && grid[newY][newX] != Constants::WALL) {
                stack.push({newX, newY});
                visited[newY][newX] = true;
                parent[newY][newX] = current;
            }
        }
    }

    return false;
}

bool Algorithms::breadthFirstSearch(int grid[Constants::ROWS][Constants::COLS], 
                                   sf::Vector2i startCell, 
                                   sf::Vector2i goalCell, 
                                   std::vector<sf::Vector2i>& path) {
    path.clear();
    std::vector<std::vector<bool>> visited(Constants::ROWS, std::vector<bool>(Constants::COLS, false));
    std::vector<std::vector<sf::Vector2i>> parent(Constants::ROWS, std::vector<sf::Vector2i>(Constants::COLS, {-1, -1}));
    std::queue<sf::Vector2i> queue;

    // Directions: up, right, down, left
    const std::vector<sf::Vector2i> DIRECTIONS = {
        {0, -1}, {1, 0}, {0, 1}, {-1, 0}
    };

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

            if (isValidCell(newX, newY) && !visited[newY][newX] && grid[newY][newX] != Constants::WALL) {
                queue.push({newX, newY});
                visited[newY][newX] = true;
                parent[newY][newX] = current;
            }
        }
    }

    return false;
}

bool Algorithms::dijkstraAlgorithm(int grid[Constants::ROWS][Constants::COLS], 
                                  sf::Vector2i startCell, 
                                  sf::Vector2i goalCell, 
                                  std::vector<sf::Vector2i>& path) {
    path.clear();

    // Node struct for priority queue
    struct Node {
        int x, y, cost;
        bool operator>(const Node& other) const {
            return cost > other.cost;
        }
    };

    // Directions: up, right, down, left
    const std::vector<sf::Vector2i> DIRECTIONS = {
        {0, -1}, {1, 0}, {0, 1}, {-1, 0}
    };

    std::priority_queue<Node, std::vector<Node>, std::greater<Node>> pq;
    std::vector<std::vector<int>> distance(Constants::ROWS, std::vector<int>(Constants::COLS, INT_MAX));
    std::vector<std::vector<sf::Vector2i>> parent(Constants::ROWS, std::vector<sf::Vector2i>(Constants::COLS, {-1, -1}));

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

            if (isValidCell(newX, newY) && grid[newY][newX] != Constants::WALL) {
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