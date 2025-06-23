#include <SFML/Graphics.hpp>
#include <optional>
#include "components/ControlPanel.h"
#include "components/AlgorithmsDemonstrator.h"

int main()
{
    auto window = sf::RenderWindow(sf::VideoMode({1280u, 960u}), L"排序算法演示器");
    window.setFramerateLimit(60);
    
    const unsigned int MIN_WINDOW_WIDTH = 800u;
    const unsigned int MIN_WINDOW_HEIGHT = 600u;

    ControlPanel control_panel;
    AlgorithmsDemonstrator algorithms_demonstrator;

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
        
        sf::FloatRect panel_shape = sf::FloatRect(sf::Vector2f(display_width, 0.f), sf::Vector2f(control_width, window_height));
        control_panel.updateLayout(panel_shape);
    };

    updateLayout();
    
    sf::Vector2u initial_window_size = window.getSize();
    float initial_window_width = static_cast<float>(initial_window_size.x);
    float initial_window_height = static_cast<float>(initial_window_size.y);
    float initial_control_width = std::max(300.0f, std::min(350.0f, initial_window_width * 0.3f));
    float initial_display_width = initial_window_width - initial_control_width;
    
    sf::FloatRect panel_shape = sf::FloatRect(sf::Vector2f(initial_display_width, 0.f), sf::Vector2f(initial_control_width, initial_window_height));
    control_panel.setupControls(panel_shape);

    control_panel.on_start_sort_clicked = [&]() {
        algorithms_demonstrator.start();
    };
    
    control_panel.on_reset_clicked = [&]() {
        algorithms_demonstrator.stop();
        algorithms_demonstrator.generateData();
    };
    
    control_panel.on_pause_resume_clicked = [&]() {
        if (control_panel.isPaused()) {
            algorithms_demonstrator.pause();
        } else {
            algorithms_demonstrator.resume();
        }
    };
    
    control_panel.on_data_size_changed = [&](int size) {
        algorithms_demonstrator.setDataSize(size);
    };
    
    control_panel.on_algorithm_changed = [&](int index) {
        std::vector<std::wstring> algorithms = {L"冒泡排序", L"快速排序", L"归并排序"};
        if (index >= 0 && index < static_cast<int>(algorithms.size())) {
            algorithms_demonstrator.setAlgorithm(algorithms[index]);
        }
    };
    
    control_panel.on_shuffle_type_changed = [&](int index) {
        DataDistributionMode modes[] = {DataDistributionMode::RANDOM, DataDistributionMode::ASCENDING, DataDistributionMode::DESCENDING, DataDistributionMode::NORMAL};
        if (index >= 0 && index < 4) {
            algorithms_demonstrator.setDataDistributionMode(modes[index]);
        }
    };
    
    control_panel.on_speed_changed = [&](float speed) {
        algorithms_demonstrator.setAnimationSpeed(speed);
    };
    
    control_panel.on_audio_toggled = [&](bool enabled) {
        algorithms_demonstrator.setAudioStatus(enabled);
    };

    sf::Clock delta_clock;

    while (window.isOpen()) {
        sf::Vector2f mouse_position = sf::Vector2f(sf::Mouse::getPosition(window));
        float duration_since_animation_start = delta_clock.restart().asSeconds();
        
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            } else if (event->is<sf::Event::Resized>()) {
                const auto* resize_event = event->getIf<sf::Event::Resized>();
                if (resize_event) {
                    unsigned int new_width = std::max(MIN_WINDOW_WIDTH, resize_event->size.x);
                    unsigned int new_height = std::max(MIN_WINDOW_HEIGHT, resize_event->size.y);
                    
                    if (new_width != resize_event->size.x || new_height != resize_event->size.y) {
                        window.setSize(sf::Vector2u(new_width, new_height));
                    }
                    
                    sf::FloatRect visible_area(sf::Vector2f(0.0f, 0.0f), 
                    sf::Vector2f(static_cast<float>(new_width), static_cast<float>(new_height)));
                    window.setView(sf::View(visible_area));
                    updateLayout();
                }
            }
            
            control_panel.handleEvent(*event, mouse_position);
        }

        control_panel.update(mouse_position);
        
        sf::Vector2u current_window_size = window.getSize();
        float current_window_width = static_cast<float>(current_window_size.x);
        float current_window_height = static_cast<float>(current_window_size.y);
        
        float current_control_width = std::max(300.0f, std::min(350.0f, current_window_width * 0.3f));
        float current_display_width = current_window_width - current_control_width;
        
        sf::FloatRect demonstrator_area = sf::FloatRect(sf::Vector2f(0.f, 0.f), sf::Vector2f(current_display_width, current_window_height));
        algorithms_demonstrator.update(demonstrator_area, duration_since_animation_start);
        
        const auto& statistics_ref = algorithms_demonstrator.getStatistics();
        control_panel.updateStatistics(statistics_ref.comparisons, statistics_ref.array_accesses, statistics_ref.assignments, statistics_ref.passes, statistics_ref.sortedness);
        
        window.clear(sf::Color::White);
        
        window.draw(algorithm_display_area);
        window.draw(control_panel_shape);
        
        algorithms_demonstrator.render(window);
        
        control_panel.render(window);
        
        window.display();
    }
    return 0;
}