#include <SFML/Graphics.hpp>
#include "PathfindingVisualizer.h"
#include "Constants.h"

int main() {
    sf::RenderWindow window(sf::VideoMode(Constants::WINDOW_WIDTH, Constants::WINDOW_HEIGHT),
                          "Pathfinding Visualizer", sf::Style::Close);
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