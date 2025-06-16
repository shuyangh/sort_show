#ifndef BUTTON_H
#define BUTTON_H

#include <SFML/Graphics.hpp>
#include <functional>

class Button {
private:
    sf::RectangleShape shape;
    sf::Font* font_ptr;
    sf::Text text;
    std::function<void()> callback;
    bool is_mouse_over = false;
    bool is_pressed = false;

    sf::Color normal_color = sf::Color(240, 240, 240);
    sf::Color hover_color = sf::Color(220, 220, 220);
    sf::Color pressed_color = sf::Color(200, 200, 200);
    
public:
    Button(const sf::Vector2f& position_ref, const sf::Vector2f& size_ref, const std::wstring& label_ref, sf::Font* font_ptr);
    
    void setCallback(std::function<void()> callback);
    void setLabel(const std::wstring& label_ref);
    void handleEvent(const sf::Event& event_ref, const sf::Vector2f& mouse_position_ref);
    void update(const sf::Vector2f& mouse_position_ref);
    void render(sf::RenderWindow& window_ref);
};

#endif