#include "Button.h"

Button::Button(const sf::Vector2f& position_ref, const sf::Vector2f& size_ref, const std::wstring& label_ref, sf::Font* font_ptr) : font_ptr(font_ptr), text(*font_ptr) {
    shape.setPosition(position_ref);
    shape.setSize(size_ref);
    shape.setFillColor(normal_color);
    shape.setOutlineColor(sf::Color(100, 100, 100));
    shape.setOutlineThickness(2.f);
    text.setString(label_ref);
    text.setCharacterSize(18);
    text.setFillColor(sf::Color::Black);
    text.setStyle(sf::Text::Bold);
    sf::FloatRect text_bounds = text.getLocalBounds();
    text.setPosition(sf::Vector2f(
        position_ref.x + (size_ref.x - text_bounds.size.x) / 2,
        position_ref.y + (size_ref.y - text_bounds.size.y) / 2
    ));
}

void Button::setCallback(std::function<void()> callback_function) {
    callback = callback_function;
}

void Button::setLabel(const std::wstring& label_ref) {
    text.setString(label_ref);
    sf::FloatRect text_bounds = text.getLocalBounds();
    sf::Vector2f button_position = shape.getPosition();
    sf::Vector2f button_size = shape.getSize();
    text.setPosition(sf::Vector2f(
        button_position.x + (button_size.x - text_bounds.size.x) / 2,
        button_position.y + (button_size.y - text_bounds.size.y) / 2
    ));
}

void Button::handleEvent(const sf::Event& event_ref, const sf::Vector2f& mouse_position_ref) {
    if (event_ref.is<sf::Event::MouseButtonPressed>()) {
        const auto* mouse_event_ptr = event_ref.getIf<sf::Event::MouseButtonPressed>();
        if (mouse_event_ptr && mouse_event_ptr->button == sf::Mouse::Button::Left) {
            if (shape.getGlobalBounds().contains(mouse_position_ref)) {
                is_pressed = true;
            }
        }
    } else if (event_ref.is<sf::Event::MouseButtonReleased>()) {
        const auto* mouse_event_ptr = event_ref.getIf<sf::Event::MouseButtonReleased>();
        if (mouse_event_ptr && mouse_event_ptr->button == sf::Mouse::Button::Left) {
            if (is_pressed && shape.getGlobalBounds().contains(mouse_position_ref)) {
                if (callback) callback();
            }
            is_pressed = false;
        }
    }
}

void Button::update(const sf::Vector2f& mouse_position_ref) {
    is_mouse_over = shape.getGlobalBounds().contains(mouse_position_ref);
    
    if (is_pressed) {
        shape.setFillColor(pressed_color);
    } else if (is_mouse_over) {
        shape.setFillColor(hover_color);
    } else {
        shape.setFillColor(normal_color);
    }
}

void Button::render(sf::RenderWindow& window_ref) {
    window_ref.draw(shape);
    window_ref.draw(text);
}