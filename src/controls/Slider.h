#ifndef SLIDER_H
#define SLIDER_H

#include <SFML/Graphics.hpp>
#include <functional>

class Slider {
private:
    sf::RectangleShape track;
    sf::CircleShape handle;
    sf::Font* font_ptr;
    float minimum_value;
    float maximum_value;
    float current_value;
    sf::Text label;
    sf::Text value_text;
    bool is_dragging = false;
    
    std::function<void(float)> callback;
    
public:
    Slider(const sf::Vector2f& position_ref, float width, float minumum_value, float maximum_value, float initial_value, sf::Font* font_ptr);
    void setCallback(std::function<void(float)> callback);
    void handleEvent(const sf::Event& event_ref, const sf::Vector2f& mouse_position_ref);
    void update(const sf::Vector2f& mouse_position_ref);
    void render(sf::RenderWindow& window_ref);
    
    float getValue() const { return current_value; }
    void setValue(float value);
    bool isDragging() const { return is_dragging; }
};

#endif