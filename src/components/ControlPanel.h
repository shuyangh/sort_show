#ifndef CONTROL_PANEL_H
#define CONTROL_PANEL_H

#include <SFML/Graphics.hpp>
#include <functional>
#include <string>
#include <vector>
#include <optional>

#include "../controls/SelectionBox.h"
#include "../controls/Slider.h"
#include "../controls/Button.h"

class ControlPanel {
private:
    sf::FloatRect shape;
    sf::Font font;
    
    std::optional<sf::Text> title;

    std::vector<Button> buttons;
    std::vector<Slider> sliders;
    std::vector<SelectionBox> selection_boxes;
    std::vector<sf::Text> labels;
    
    bool is_paused;
    
    std::vector<sf::Text> statistics_labels;
    std::vector<sf::Text> statistics;
    sf::RectangleShape statistics_panel;
    
public:    
    ControlPanel();
    bool loadFont(const std::string& font_path_ref = "C:/Windows/Fonts/msyh.ttc");
    
    void setupControls(const sf::FloatRect& panel_shape_ref);
    void updateLayout(const sf::FloatRect& panel_shape_ref);
    void updateStatistics(int comparisons, int array_accesses, int swaps, int passes, float sortedness);
    void handleEvent(const sf::Event& event_ref, const sf::Vector2f& mouse_position_ref);
    void update(const sf::Vector2f& mouse_position_ref);
    void render(sf::RenderWindow& window_ref);
    
    void setPaused(bool paused);
    bool isPaused() const;
    std::function<void()> on_start_sort_clicked;
    std::function<void()> on_reset_clicked;
    std::function<void()> on_pause_resume_clicked;
    std::function<void(int)> on_data_size_changed;
    std::function<void(int)> on_shuffle_type_changed;
    std::function<void(float)> on_speed_changed;
    
    void updatePauseResumeButton();
};

#endif