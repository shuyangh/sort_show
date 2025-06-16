#include "SelectionBox.h"

SelectionBox::SelectionBox(const sf::Vector2f& position_ref, float width, const std::vector<std::wstring>& option_list_ref, sf::Font* font_ptr) : font_ptr(font_ptr), options(option_list_ref), selected_text(*font_ptr) {
    shape.setPosition(position_ref);
    shape.setSize(sf::Vector2f(width, 35));
    shape.setFillColor(sf::Color::White);
    shape.setOutlineColor(sf::Color(100, 100, 100));
    shape.setOutlineThickness(2);
    selected_text.setCharacterSize(16);
    selected_text.setFillColor(sf::Color(50, 50, 50));
    selected_text.setStyle(sf::Text::Bold);
    selected_text.setPosition(sf::Vector2f(position_ref.x + 8, position_ref.y + 8));
    option_boxes.resize(options.size());
    option_texts.clear();
    option_texts.reserve(options.size());
    for (size_t option_index = 0; option_index < options.size(); ++option_index) {
        option_boxes[option_index].setPosition(sf::Vector2f(position_ref.x, position_ref.y + 35 * (option_index + 1)));
        option_boxes[option_index].setSize(sf::Vector2f(width, 35));
        option_boxes[option_index].setFillColor(sf::Color::White);
        option_boxes[option_index].setOutlineColor(sf::Color(100, 100, 100));
        option_boxes[option_index].setOutlineThickness(2);
        
        option_texts.emplace_back(*font_ptr);
        option_texts[option_index].setString(options[option_index]);
        option_texts[option_index].setCharacterSize(16);
        option_texts[option_index].setFillColor(sf::Color(50, 50, 50));
        option_texts[option_index].setStyle(sf::Text::Bold);
        option_texts[option_index].setPosition(sf::Vector2f(position_ref.x + 8, position_ref.y + 35 * (option_index + 1) + 8));
    }
    
    setSelectedIndex(0);
}

void SelectionBox::setCallback(std::function<void(int)> callback_function) {
    callback = callback_function;
}

void SelectionBox::setSelectedIndex(int index) {
    if (index >= 0 && index < static_cast<int>(options.size())) {
        selected_index = index;
        selected_text.setString(options[index]);
    }
}

void SelectionBox::handleEvent(const sf::Event& event_ref, const sf::Vector2f& mouse_position_ref) {
    if (event_ref.is<sf::Event::MouseButtonPressed>()) {
        const auto* mouse_event = event_ref.getIf<sf::Event::MouseButtonPressed>();
        if (mouse_event && mouse_event->button == sf::Mouse::Button::Left) {
            if (shape.getGlobalBounds().contains(mouse_position_ref)) {
                is_open = !is_open;
            } else if (is_open) {
                for (size_t option_index = 0; option_index < option_boxes.size(); ++option_index) {
                    if (option_boxes[option_index].getGlobalBounds().contains(mouse_position_ref)) {
                        setSelectedIndex(static_cast<int>(option_index));
                        if (callback) callback(selected_index);
                        is_open = false;
                        break;
                    }
                }
                is_open = false;
            }
        }
    }
}

void SelectionBox::update(const sf::Vector2f& mouse_position_ref) {
    if (shape.getGlobalBounds().contains(mouse_position_ref)) {
        shape.setFillColor(sf::Color(220, 220, 220));
    } else {
        shape.setFillColor(sf::Color::White);
    }
    
    if (is_open) {
        for (size_t option_index = 0; option_index < option_boxes.size(); ++option_index) {
            if (option_boxes[option_index].getGlobalBounds().contains(mouse_position_ref)) {
                option_boxes[option_index].setFillColor(sf::Color(240, 240, 240));
            } else {
                option_boxes[option_index].setFillColor(sf::Color::White);
            }
        }
    }
}

void SelectionBox::render(sf::RenderWindow& window) {
    window.draw(shape);
    window.draw(selected_text);
    
    if (is_open) {
        for (size_t option_index = 0; option_index < option_boxes.size(); ++option_index) {
            window.draw(option_boxes[option_index]);
            window.draw(option_texts[option_index]);
        }
    }
}