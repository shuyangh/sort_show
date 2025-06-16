#include "Slider.h"

Slider::Slider(const sf::Vector2f& position_ref, float width, float min_value, float max_value, float initial_value, sf::Font* font_ptr) : font_ptr(font_ptr), minimum_value(min_value), maximum_value(max_value), current_value(initial_value), label(*font_ptr), value_text(*font_ptr) {
    track.setPosition(sf::Vector2f(position_ref.x, position_ref.y + 30));
    track.setSize(sf::Vector2f(width, 6));
    track.setFillColor(sf::Color(130, 130, 130));
    
    handle.setRadius(12);
    handle.setFillColor(sf::Color(70, 130, 220));
    
    value_text.setCharacterSize(16);
    value_text.setFillColor(sf::Color(50, 50, 50));
    value_text.setStyle(sf::Text::Bold);
    value_text.setPosition(sf::Vector2f(position_ref.x + width - 40, position_ref.y));
    
    setValue(initial_value);
}

void Slider::setCallback(std::function<void(float)> callback_function) {
    callback = callback_function;
}

void Slider::setValue(float value) {
    current_value = std::max(minimum_value, std::min(maximum_value, value));
    
    float ratio = (current_value - minimum_value) / (maximum_value - minimum_value);
    sf::Vector2f track_position = track.getPosition();
    float track_width = track.getSize().x;
    
    handle.setPosition(sf::Vector2f(track_position.x + ratio * track_width - 12, track_position.y - 9));
    
    value_text.setString(std::to_string(static_cast<int>(current_value)));
}

void Slider::handleEvent(const sf::Event& event_ref, const sf::Vector2f& mouse_position_ref) {
    if (event_ref.is<sf::Event::MouseButtonPressed>()) {
        const auto* mouse_event = event_ref.getIf<sf::Event::MouseButtonPressed>();
        if (mouse_event && mouse_event->button == sf::Mouse::Button::Left) {
            if (handle.getGlobalBounds().contains(mouse_position_ref)) {
                is_dragging = true;
            }
        }
    } else if (event_ref.is<sf::Event::MouseButtonReleased>()) {
        const auto* mouse_event = event_ref.getIf<sf::Event::MouseButtonReleased>();
        if (mouse_event && mouse_event->button == sf::Mouse::Button::Left) {
            is_dragging = false;
        }
    }
}

void Slider::update(const sf::Vector2f& mouse_position_ref) {
    if (is_dragging) {
        sf::Vector2f track_position = track.getPosition();
        float track_width = track.getSize().x;
        
        float ratio = (mouse_position_ref.x - track_position.x) / track_width;
        ratio = std::max(0.0f, std::min(1.0f, ratio));
        
        float new_value = minimum_value + ratio * (maximum_value - minimum_value);
        setValue(new_value);
        
        if (callback) callback(current_value);
    }
}

void Slider::render(sf::RenderWindow& window) {
    window.draw(label);
    window.draw(value_text);
    window.draw(track);
    window.draw(handle);
}