#include "ControlPanel.h"
#include <iostream>
#include <cmath>

ControlPanel::ControlPanel() {
    loadFont();
    title = sf::Text(font);
    is_paused = false;
}

bool ControlPanel::loadFont(const std::string& font_path) {
    if (!font_path.empty()) {
        if (font.openFromFile(font_path)) {
            return true;
        }
    }
      
    std::vector<std::string> font_paths = {
        "C:/Windows/Fonts/msyh.ttc",
        "C:/Windows/Fonts/segoeui.ttf", 
        "C:/Windows/Fonts/arial.ttf"
    };
    
    for (const auto& path : font_paths) {
        if (font.openFromFile(path)) {
            return true;
        }
    }
    
    return false;
}

void ControlPanel::setupControls(const sf::FloatRect& panel_area) {
    shape = panel_area;
    
    buttons.clear();
    sliders.clear();
    selection_boxes.clear();
    labels.clear();
    
    float x = panel_area.position.x + 25;
    float y = panel_area.position.y + 25;
    float width = 300.0f;
    
    if (title.has_value()) {
        title->setString(L"排序算法演示器");
        title->setCharacterSize(24);
        title->setFillColor(sf::Color(40, 40, 40));
        title->setStyle(sf::Text::Bold);
        title->setPosition(sf::Vector2f(x, y));
    }
    y += 50;
    
    labels.emplace_back(font);
    labels.back().setString(L"排序算法:");
    labels.back().setCharacterSize(16);
    labels.back().setFillColor(sf::Color(60, 60, 60));
    labels.back().setStyle(sf::Text::Bold);
    labels.back().setPosition(sf::Vector2f(x, y));
    y += 25;

    selection_boxes.emplace_back(sf::Vector2f(x, y), width, std::vector<std::wstring>{L"冒泡排序", L"快速排序", L"归并排序"}, &font);
    y += 60;
    
    labels.emplace_back(font);
    labels.back().setString(L"数据集大小:");
    labels.back().setCharacterSize(16);
    labels.back().setFillColor(sf::Color(60, 60, 60));
    labels.back().setStyle(sf::Text::Bold);
    labels.back().setPosition(sf::Vector2f(x, y));
    y += 25;
    sliders.emplace_back(sf::Vector2f(x, y), width, 5, 400, 50, &font);
    y += 75;
    
    labels.emplace_back(font);
    labels.back().setString(L"数据分布方式:");
    labels.back().setCharacterSize(16);
    labels.back().setFillColor(sf::Color(60, 60, 60));
    labels.back().setStyle(sf::Text::Bold);
    labels.back().setPosition(sf::Vector2f(x, y));
    y += 25;

    selection_boxes.emplace_back(sf::Vector2f(x, y), width, std::vector<std::wstring>{L"均匀分布", L"正序", L"逆序", L"正态分布"}, &font);
    y += 60;
    
    labels.emplace_back(font);
    labels.back().setString(L"动画速度:");
    labels.back().setCharacterSize(16);
    labels.back().setFillColor(sf::Color(60, 60, 60));
    labels.back().setStyle(sf::Text::Bold);
    labels.back().setPosition(sf::Vector2f(x, y));
    y += 25;
    sliders.emplace_back(sf::Vector2f(x, y), width, 1.0f, 100.0f, 5.0f, &font);
    y += 75;
    
    buttons.emplace_back(sf::Vector2f(x, y), sf::Vector2f(width, 45), L"音频: 关闭", &font);
    y += 55;
    buttons.emplace_back(sf::Vector2f(x, y), sf::Vector2f(width, 45), L"开始排序", &font);
    y += 55;
    buttons.emplace_back(sf::Vector2f(x, y), sf::Vector2f(width, 45), L"重置", &font);
    y += 55;
    buttons.emplace_back(sf::Vector2f(x, y), sf::Vector2f(width, 45), L"暂停", &font);
    y += 65;
    
    statistics_labels.clear();
    statistics.clear();
    statistics_panel.setPosition(sf::Vector2f(x - 10, y));
    statistics_panel.setSize(sf::Vector2f(width + 20, 160));
    statistics_panel.setFillColor(sf::Color(250, 250, 250));
    statistics_panel.setOutlineColor(sf::Color(150, 150, 150));
    statistics_panel.setOutlineThickness(2);
    
    labels.emplace_back(font);
    labels.back().setString(L"排序统计:");
    labels.back().setCharacterSize(16);
    labels.back().setFillColor(sf::Color(40, 40, 40));
    labels.back().setStyle(sf::Text::Bold);
    labels.back().setPosition(sf::Vector2f(x, y + 10));
    y += 35;
    std::vector<std::wstring> statistic_labels = {
        L"比较次数:", L"数组访问次数:", L"赋值次数:", L"遍历轮数:", L"有序度:"
    };
    
    for (size_t statistic_index = 0; statistic_index < statistic_labels.size(); ++statistic_index) {
        statistics_labels.emplace_back(font);
        statistics_labels.back().setString(statistic_labels[statistic_index]);
        statistics_labels.back().setCharacterSize(14);
        statistics_labels.back().setFillColor(sf::Color(70, 70, 70));
        statistics_labels.back().setPosition(sf::Vector2f(x, y));

        statistics.emplace_back(font);
        statistics.back().setString(L"0");
        statistics.back().setCharacterSize(14);
        statistics.back().setFillColor(sf::Color(70, 130, 220));
        statistics.back().setStyle(sf::Text::Bold);
        statistics.back().setPosition(sf::Vector2f(x + 120, y));
        
        y += 25;
    }
    if (sliders.size() >= 2) {
        sliders[0].setCallback([this](float value) {
            if (on_data_size_changed) on_data_size_changed(static_cast<int>(value));
        });
        sliders[1].setCallback([this](float value) {
            if (on_speed_changed) on_speed_changed(value / 10.0f);
        });
    }
      if (selection_boxes.size() >= 2) {
        selection_boxes[0].setCallback([this](int index) {
            if (on_algorithm_changed) on_algorithm_changed(index);
        });
        selection_boxes[1].setCallback([this](int index) {
            if (on_shuffle_type_changed) on_shuffle_type_changed(index);
        });
    }
    if (buttons.size() >= 4) {
        buttons[0].setCallback([this]() {
            is_audio_enabled = !is_audio_enabled;
            updateAudioButton();
            if (on_audio_toggled) on_audio_toggled(is_audio_enabled);
        });
        
        buttons[1].setCallback([this]() {
            if (on_start_sort_clicked) on_start_sort_clicked();
        });
        
        buttons[2].setCallback([this]() {
            if (on_reset_clicked) on_reset_clicked();
        });
          buttons[3].setCallback([this]() {
            is_paused = !is_paused;
            updatePauseResumeButton();
            if (on_pause_resume_clicked) on_pause_resume_clicked();
        });
    }
}

void ControlPanel::handleEvent(const sf::Event& event, const sf::Vector2f& mouse_pos) {
    for (auto& button : buttons) {
        button.handleEvent(event, mouse_pos);
    }
    
    for (auto& slider : sliders) {
        slider.handleEvent(event, mouse_pos);
    }
    
    for (auto& SelectionBox : selection_boxes) {
        SelectionBox.handleEvent(event, mouse_pos);
    }
}

void ControlPanel::update(const sf::Vector2f& mouse_pos) {
    for (auto& button : buttons) {
        button.update(mouse_pos);
    }
    
    for (auto& slider : sliders) {
        slider.update(mouse_pos);
    }
    
    for (auto& SelectionBox : selection_boxes) {
        SelectionBox.update(mouse_pos);
    }
}

void ControlPanel::render(sf::RenderWindow& window) {
    if (title.has_value()) {
        window.draw(*title);
    }
    
    for (auto& label : labels) {
        window.draw(label);
    }
    
    for (auto& button : buttons) {
        button.render(window);
    }
    
    for (auto& slider : sliders) {
        slider.render(window);
    }
    
    for (auto& SelectionBox : selection_boxes) {
        SelectionBox.render(window);
    }
    
    window.draw(statistics_panel);
    for (auto& stat_label : statistics_labels) {
        window.draw(stat_label);
    }
    for (auto& stat_value : statistics) {
        window.draw(stat_value);
    }
}

void ControlPanel::updateLayout(const sf::FloatRect& panel_area) {
    bool needs_update = std::abs(shape.position.x - panel_area.position.x) > 1.0f || std::abs(shape.position.y - panel_area.position.y) > 1.0f || std::abs(shape.size.x - panel_area.size.x) > 1.0f || std::abs(shape.size.y - panel_area.size.y) > 1.0f;
    if (needs_update) {
        int data_size = 50;
        float speed = 5.0f;
        int shuffle_type = 0;
        bool is_slider_dragging = false;
        if (!sliders.empty()) {
            data_size = static_cast<int>(sliders[0].getValue());
            if (sliders.size() > 1) {
                speed = sliders[1].getValue();
                is_slider_dragging = sliders[1].isDragging();
            }
        }
        
        if (!selection_boxes.empty() && selection_boxes.size() > 1) {
            shuffle_type = selection_boxes[1].getSelectedIndex();
        }
        
        if (!is_slider_dragging) {
            setupControls(panel_area);
              
            if (!sliders.empty()) {
                sliders[0].setValue(static_cast<float>(data_size));
                if (sliders.size() > 1) {
                    sliders[1].setValue(speed);
                }
            }
            
            if (!selection_boxes.empty() && selection_boxes.size() > 1) {
                selection_boxes[1].setSelectedIndex(shuffle_type);
            }
        }
    }
}

void ControlPanel::updateStatistics(int comparisons, int array_accesses, int assignments, int passes, float sortedness) {
    if (statistics.size() >= 5) {
        statistics[0].setString(std::to_string(comparisons));
        statistics[1].setString(std::to_string(array_accesses));
        statistics[2].setString(std::to_string(assignments));
        statistics[3].setString(std::to_string(passes));
        statistics[4].setString(std::to_string(static_cast<int>(std::round(sortedness))) + "%");
    }
}

void ControlPanel::setPaused(bool paused) {
    is_paused = paused;
    updatePauseResumeButton();
}

bool ControlPanel::isPaused() const {
    return is_paused;
}

void ControlPanel::updatePauseResumeButton() {
    if (buttons.size() >= 4) {
        if (is_paused) {
            buttons[3].setLabel(L"继续");
        } else {
            buttons[3].setLabel(L"暂停");
        }
    }
}

void ControlPanel::updateAudioButton() {
    if (buttons.size() >= 1) {
        if (is_audio_enabled) {
            buttons[0].setLabel(L"音频: 开启");
        } else {
            buttons[0].setLabel(L"音频: 关闭");
        }
    }
}