#include <SFML/Graphics.hpp>
#include <optional>
#include "components/ControlPanel.h"

int main()
{
    auto window = sf::RenderWindow(sf::VideoMode({1280u, 960u}), L"排序算法演示器");
    window.setFramerateLimit(60);

    ControlPanel control_panel;

    sf::RectangleShape algorithm_display_area;
    algorithm_display_area.setFillColor(sf::Color::White);
    algorithm_display_area.setOutlineColor(sf::Color(200, 200, 200));
    algorithm_display_area.setOutlineThickness(2.f);

    sf::RectangleShape control_panel_shape;
    control_panel_shape.setFillColor(sf::Color(240, 240, 240));
    control_panel_shape.setOutlineColor(sf::Color(200, 200, 200));
    control_panel_shape.setOutlineThickness(2.f);

    auto updateLayout = [&]() {
        sf::Vector2u window_size = window.getSize();
        float window_width = static_cast<float>(window_size.x);
        float window_height = static_cast<float>(window_size.y);
        
        float control_width = 350.0f;
        float display_width = window_width - control_width;
        
        algorithm_display_area.setSize(sf::Vector2f(display_width, window_height));
        algorithm_display_area.setPosition(sf::Vector2f(0.f, 0.f));
        
        control_panel_shape.setSize(sf::Vector2f(control_width, window_height));
        control_panel_shape.setPosition(sf::Vector2f(display_width, 0.f));
        
        sf::FloatRect panel_rect = sf::FloatRect(sf::Vector2f(display_width, 0.f), sf::Vector2f(control_width, window_height));
        control_panel.updateLayout(panel_rect);
    };

    updateLayout();
    
    sf::Vector2u window_size = window.getSize();
    float window_width = static_cast<float>(window_size.x);
    float control_width = 350.0f;
    float display_width = window_width - control_width;
    sf::FloatRect panel_rect = sf::FloatRect(sf::Vector2f(display_width, 0.f), sf::Vector2f(control_width, static_cast<float>(window_size.y)));
    control_panel.setupControls(panel_rect);

    while (window.isOpen()) {
        sf::Vector2f mouse_pos = sf::Vector2f(sf::Mouse::getPosition(window));
        
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            } else if (event->is<sf::Event::Resized>()) {
                const auto* resize_event = event->getIf<sf::Event::Resized>();
                if (resize_event) {
                    sf::FloatRect visible_area(sf::Vector2f(0.0f, 0.0f), 
                    sf::Vector2f(static_cast<float>(resize_event->size.x), static_cast<float>(resize_event->size.y)));
                    window.setView(sf::View(visible_area));
                    updateLayout();
                }
            }
            
            control_panel.handleEvent(*event, mouse_pos);
        }

        control_panel.update(mouse_pos);
        
        window.clear(sf::Color::White);
        
        window.draw(algorithm_display_area);
        window.draw(control_panel_shape);
        
        control_panel.render(window);
        
        window.display();
    }
    return 0;
}