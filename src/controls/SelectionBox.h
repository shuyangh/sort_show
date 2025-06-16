#ifndef SELECTION_BOX_H
#define SELECTION_BOX_H

#include <SFML/Graphics.hpp>
#include <functional>

class SelectionBox {
private:
    sf::RectangleShape shape;
    sf::Font* font_ptr;
    std::vector<std::wstring> options;
    sf::Text selected_text;
    std::vector<sf::RectangleShape> option_boxes;
    std::vector<sf::Text> option_texts;
    
    int selected_index = 0;
    bool is_open = false;
    
    std::function<void(int)> callback;
    
public:
    SelectionBox(const sf::Vector2f& position_ref, float width, const std::vector<std::wstring>& option_list_ref, sf::Font* font_ptr);
    void setCallback(std::function<void(int)> callback);
    void handleEvent(const sf::Event& event_ref, const sf::Vector2f& mouse_position_ref);
    void update(const sf::Vector2f& mouse_position_ref);
    void render(sf::RenderWindow& window);
    
    int getSelectedIndex() const { return selected_index; }
    void setSelectedIndex(int index);
};

#endif