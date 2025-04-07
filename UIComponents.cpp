#include "UIComponents.h"

sf::RectangleShape UIComponents::createButton(float x, float y, float width, float height, sf::Color color, sf::Color outlineColor) {
    sf::RectangleShape button(sf::Vector2f(width, height));
    button.setFillColor(color);
    button.setPosition(x, y);
    button.setOutlineThickness(2);
    button.setOutlineColor(outlineColor);
    return button;
}

sf::Text UIComponents::createText(const sf::Font& font, const std::string& string, int size, sf::Color color, float x, float y) {
    sf::Text text;
    text.setFont(font);
    text.setString(string);
    text.setCharacterSize(size);
    text.setFillColor(color);
    text.setPosition(x, y);
    return text;
}

bool UIComponents::isButtonClicked(const sf::RectangleShape& button, int mouseX, int mouseY) {
    sf::FloatRect bounds = button.getGlobalBounds();
    return bounds.contains(mouseX, mouseY);
}