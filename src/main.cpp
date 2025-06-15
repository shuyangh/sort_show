#include <SFML/Graphics.hpp>
#include <optional>

int main()
{
    auto window = sf::RenderWindow(sf::VideoMode({1080u, 720u}), "Sort Show");
    window.setFramerateLimit(60);

    sf::RectangleShape algorithm_display_area;
    algorithm_display_area.setSize(sf::Vector2f(720.f, 720.f));
    algorithm_display_area.setPosition(sf::Vector2f(0.f, 0.f));
    algorithm_display_area.setFillColor(sf::Color::White);
    algorithm_display_area.setOutlineColor(sf::Color::Black);
    algorithm_display_area.setOutlineThickness(2.f);

    sf::RectangleShape control_panel;
    control_panel.setSize(sf::Vector2f(360.f, 720.f));
    control_panel.setPosition(sf::Vector2f(720.f, 0.f));
    control_panel.setFillColor(sf::Color(240, 240, 240));
    control_panel.setOutlineColor(sf::Color::Black);
    control_panel.setOutlineThickness(2.f);

    sf::RectangleShape separator;
    separator.setSize(sf::Vector2f(2.f, 720.f));
    separator.setPosition(sf::Vector2f(720.f, 0.f));
    separator.setFillColor(sf::Color(180, 180, 180));

    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
        }

        window.clear(sf::Color::White);
        
        window.draw(algorithm_display_area);
        window.draw(control_panel);
        window.draw(separator);
        
        window.display();
    }
    return 0;
}