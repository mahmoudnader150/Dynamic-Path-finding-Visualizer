#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include "Constants.h"

class UIComponents {
public:
    static sf::RectangleShape createButton(float x, float y, float width, float height, sf::Color color, sf::Color outlineColor);
    static sf::Text createText(const sf::Font& font, const std::string& string, int size, sf::Color color, float x, float y);
    static bool isButtonClicked(const sf::RectangleShape& button, int mouseX, int mouseY);
};