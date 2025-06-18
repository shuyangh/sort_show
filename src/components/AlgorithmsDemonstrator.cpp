#include "AlgorithmsDemonstrator.h"
#include "../algorithms/IDemonstrator.h"
#include "../algorithms/BubbleSortDemonstrator.h"
#include <algorithm>
#include <iostream>
#include <set>
#include <cmath>

AlgorithmsDemonstrator::AlgorithmsDemonstrator() 
    : data_size(50), distribution_mode(DataDistributionMode::RANDOM), animation_speed(0.5f) {
    AlgorithmRegistry::registerAlgorithm(L"冒泡排序", []() {
        return std::make_unique<BubbleSortDemonstrator>();
    });

    if (!font.openFromFile("C:/Windows/Fonts/msyh.ttc")) {
        if (!font.openFromFile("C:/Windows/Fonts/segoeui.ttf")) {
            std::wcout << L"无法加载字体" << std::endl;
        }
    }
    
    midi_player = std::make_unique<MidiPlayer>();
    if (midi_player->isInitialized()) {
        midi_player->setInstrument(1);
        midi_player->setNoteRange(48, 48);
    }
    
    explaination = sf::Text(font);
    generateData();
    
    setAlgorithm(L"冒泡排序");
}

void AlgorithmsDemonstrator::setDataSize(int size) {
    data_size = std::max(5, std::min(400, size));
    generateData();
}

void AlgorithmsDemonstrator::setDataDistributionMode(DataDistributionMode mode) {
    distribution_mode = mode;
    generateData();
}

void AlgorithmsDemonstrator::setAlgorithm(const std::wstring& algorithm_name) {
    current_algorithm_ptr = AlgorithmRegistry::createAlgorithm(algorithm_name);
    resetAlgorithm();
}

void AlgorithmsDemonstrator::setAnimationSpeed(float speed) {
    animation_speed = std::max(0.1f, std::min(10.0f, speed));
}

void AlgorithmsDemonstrator::setMargins(float top, float bottom, float left, float right) {
    margin_top = std::max(0.0f, top);
    margin_bottom = std::max(0.0f, bottom);
    margin_left = std::max(0.0f, left);
    margin_right = std::max(0.0f, right);
}

void AlgorithmsDemonstrator::setMargin(float margin) {
    setMargins(margin, margin, margin, margin);
}

void AlgorithmsDemonstrator::generateData() {
    data.clear();
    data.reserve(data_size);
    bars.clear();
    
    switch (distribution_mode) {
        case DataDistributionMode::RANDOM:
            generateRandomData();
            break;
        case DataDistributionMode::ASCENDING:
            generateAscendingData();
            break;
        case DataDistributionMode::DESCENDING:
            generateDescendingData();
            break;
        case DataDistributionMode::NORMAL:
            generateNormalData();
            break;
    }
    
    bars.resize(data_size);
    resetAlgorithm();
}

void AlgorithmsDemonstrator::resetAlgorithm() {
    if (current_algorithm_ptr) {
        current_algorithm_ptr->reset();
    }
    is_running = false;
    is_paused = false;
}

void AlgorithmsDemonstrator::start() {
    if (current_algorithm_ptr && !data.empty()) {
        is_running = true;
        is_paused = false;
        animation_clock.restart();
    }
}

void AlgorithmsDemonstrator::pause() {
    is_paused = true;
}

void AlgorithmsDemonstrator::resume() {
    if (is_running) {
        is_paused = false;
        animation_clock.restart();
    }
}

void AlgorithmsDemonstrator::stop() {
    is_running = false;
    is_paused = false;
    resetAlgorithm();
}

void AlgorithmsDemonstrator::update(const sf::FloatRect& area_ref, float duration_since_animation_start) {
    display_area = area_ref;
    updateBars(area_ref);
    
    if (is_running && !is_paused && current_algorithm_ptr) {
        float scaled_duration_since_animation_start = duration_since_animation_start * animation_speed;
        
        bool completed = current_algorithm_ptr->step(data, *this, scaled_duration_since_animation_start);
        if (completed) {
            is_running = false;
        }
        
        updateExplaination();
    }
}

void AlgorithmsDemonstrator::render(sf::RenderWindow& window_ref) {
    if (data.empty()) return;
    
    std::set<int> animated_indices;
    if (current_algorithm_ptr) {
        const auto& animation_state_ref = current_algorithm_ptr->getAnimationState();
        for (int index : animation_state_ref.animating_elements) {
            animated_indices.insert(index);
        }
    }
    
    for (size_t bar_index = 0; bar_index < bars.size() && bar_index < data.size(); ++bar_index) {
        if (animated_indices.find(static_cast<int>(bar_index)) != animated_indices.end()) {
            continue;
        }
        
        sf::Color color = sf::Color(100, 149, 237);
        if (current_algorithm_ptr) {
            color = current_algorithm_ptr->getElementColor(data, static_cast<int>(bar_index));
        }
        
        bars[bar_index].setFillColor(color);
        window_ref.draw(bars[bar_index]);
    }
    
    if (current_algorithm_ptr) {
        const auto& animation_state_ref = current_algorithm_ptr->getAnimationState();
        
        if (animation_state_ref.is_animating && !animation_state_ref.animating_elements.empty()) {
            for (size_t bar_index = 0; bar_index < animation_state_ref.animating_elements.size(); ++bar_index) {
                int element_index = animation_state_ref.animating_elements[bar_index];
                if (element_index >= 0 && element_index < static_cast<int>(bars.size()) && 
                    bar_index < animation_state_ref.start_positions.size() && bar_index < animation_state_ref.target_positions.size()) {
                    
                    sf::Vector2f current_position = calculateAnimatedElementPosition(
                        animation_state_ref.start_positions[bar_index], 
                        animation_state_ref.target_positions[bar_index], 
                        animation_state_ref.animation_progress
                    );
                    
                    sf::RectangleShape animated_bar = bars[element_index];
                    animated_bar.setPosition(current_position);
                    animated_bar.setFillColor(sf::Color(255, 69, 0));
                    
                    window_ref.draw(animated_bar);
                }
            }
        }
        
        renderIndicators(window_ref, animation_state_ref);
    }
    
    if (explaination.has_value()) {
        window_ref.draw(*explaination);
    }
}

bool AlgorithmsDemonstrator::isCompleted() const {
    return current_algorithm_ptr ? current_algorithm_ptr->isCompleted() : false;
}

const SortStatistics& AlgorithmsDemonstrator::getStatistics() const {
    static SortStatistics empty_statistics;
    return current_algorithm_ptr ? current_algorithm_ptr->getStatistics() : empty_statistics;
}

std::wstring AlgorithmsDemonstrator::getCurrentStepExplaination() const {
    return current_algorithm_ptr ? current_algorithm_ptr->getCurrentStepExplaination() : L"无算法选择";
}

sf::Vector2f AlgorithmsDemonstrator::getElementPosition(int index) const {
    if (index >= 0 && index < static_cast<int>(bars.size())) {
        return bars[index].getPosition();
    }
    return sf::Vector2f(0, 0);
}

void AlgorithmsDemonstrator::playNote(int value) const {
    if (is_audio_enabled && midi_player && midi_player->isInitialized()) {
        midi_player->playNote(value, data_size, 150);
    }
}

void AlgorithmsDemonstrator::updateBars(const sf::FloatRect& area_ref) {
    if (data.empty()) return;
    
    float adjusted_bottom_margin = margin_bottom;
    int data_size = static_cast<int>(data.size());
    if (data_size < 32 && data_size > 0) {
        float margin_multiplier = 1.0f + 1.5f * (32 - data_size) / 31.0f;
        adjusted_bottom_margin = margin_bottom * margin_multiplier;
    }
    
    content_area.position.x = area_ref.position.x + margin_left;
    content_area.position.y = area_ref.position.y + margin_top;
    content_area.size.x = area_ref.size.x - margin_left - margin_right;
    content_area.size.y = area_ref.size.y - margin_top - adjusted_bottom_margin;
    
    if (content_area.size.x <= 0 || content_area.size.y <= 0) return;
    
    float bar_width = content_area.size.x / data.size();
    float max_height = content_area.size.y;
    
    for (size_t bar_index = 0; bar_index < bars.size() && bar_index < data.size(); ++bar_index) {
        float height = (static_cast<float>(data[bar_index]) / data_size) * max_height;
        
        bars[bar_index].setSize(sf::Vector2f(bar_width - 1.0f, height));
        
        sf::Vector2f normal_position(
            content_area.position.x + bar_index * bar_width, 
            content_area.position.y + content_area.size.y - height
        );
        
        bars[bar_index].setPosition(normal_position);
    }
}

void AlgorithmsDemonstrator::updateExplaination() {
    if (!explaination.has_value() || !current_algorithm_ptr) return;
    
    std::wstring current_step_explaination = current_algorithm_ptr->getCurrentStepExplaination();
    
    explaination->setString(current_step_explaination);
    explaination->setCharacterSize(18);
    explaination->setFillColor(sf::Color::Black);
    explaination->setOutlineColor(sf::Color::White);
    explaination->setOutlineThickness(0.5f);
    explaination->setPosition(sf::Vector2f(display_area.position.x + 10, display_area.position.y + 10));
}

sf::Vector2f AlgorithmsDemonstrator::calculateAnimatedElementPosition(const sf::Vector2f& start_position_ref, const sf::Vector2f& end_position_ref, float animation_progress) {
    float progress = std::max(0.0f, std::min(1.0f, animation_progress));

    return sf::Vector2f(
        start_position_ref.x + (end_position_ref.x - start_position_ref.x) * progress,
        start_position_ref.y + (end_position_ref.y - start_position_ref.y) * progress
    );
}

void AlgorithmsDemonstrator::generateAscendingData() {
    for (int element = 1; element <= data_size; ++element) {
        data.push_back(element);
    }
}

void AlgorithmsDemonstrator::generateDescendingData() {
    for (int element = data_size; element >= 1; --element) {
        data.push_back(element);
    }
}

void AlgorithmsDemonstrator::generateRandomData() {
    for (int element = 1; element <= data_size; ++element) {
        data.push_back(element);
    }
    
    auto random_engine = std::default_random_engine(std::chrono::steady_clock::now().time_since_epoch().count());
    std::shuffle(data.begin(), data.end(), random_engine);
}

void AlgorithmsDemonstrator::generateNormalData() {
    std::vector<std::pair<double, int>> height_position_pairs;
    
    double data_width = static_cast<double>(data_size);
    
    for (int index = 0; index < data_size; ++index) {
        double current_position = static_cast<double>(index);
        
        double cosine_value = std::cos(M_PI * (current_position - data_width / 2.0) / (data_width <= 25 ? data_width * 1.3 : data_width * 1.1));
        double bell_curve_height = data_width * cosine_value * cosine_value;
        
        bell_curve_height = std::max(0.0, bell_curve_height);
        
        data.push_back(static_cast<int>(std::round(bell_curve_height)));
    }
}

void AlgorithmsDemonstrator::renderIndicators(sf::RenderWindow& window_ref, const AnimationState& animation_state_ref) {
    for (const auto& indicator_ref : animation_state_ref.indicators) {
        if (!indicator_ref.visible) continue;
        
        switch (indicator_ref.type) {
            case IndicatorType::Triangle:
                renderTriangleIndicator(window_ref, indicator_ref);
                break;
            case IndicatorType::Rectangle:
                renderRectangleIndicator(window_ref, indicator_ref);
                break;
            case IndicatorType::Arrow:
                renderArrowIndicator(window_ref, indicator_ref);
                break;
            case IndicatorType::Range:
                renderRangeIndicator(window_ref, indicator_ref);
                break;
        }
    }
}

void AlgorithmsDemonstrator::renderTriangleIndicator(sf::RenderWindow& window_ref, const VisualIndicator& indicator_ref) {
    int data_size = static_cast<int>(data.size());
    float triangle_size = 8.0f;
    if (data_size < 32 && data_size > 0) {
        triangle_size = 8.0f + 24.0f * (32 - data_size) / 31.0f;
    }
    sf::CircleShape triangle(triangle_size, 3);
    triangle.setFillColor(indicator_ref.color);
    triangle.setPosition(sf::Vector2f(indicator_ref.position.x - triangle_size, indicator_ref.position.y));
    window_ref.draw(triangle);
}

void AlgorithmsDemonstrator::renderRectangleIndicator(sf::RenderWindow& window_ref, const VisualIndicator& indicator_ref) {
    sf::RectangleShape rectangle(indicator_ref.size);
    rectangle.setFillColor(sf::Color(indicator_ref.color.r, indicator_ref.color.g, indicator_ref.color.b, 50));
    rectangle.setOutlineThickness(2.0f);
    rectangle.setOutlineColor(indicator_ref.color);
    rectangle.setPosition(indicator_ref.position);
    window_ref.draw(rectangle);
}

void AlgorithmsDemonstrator::renderArrowIndicator(sf::RenderWindow& window_ref, const VisualIndicator& indicator_ref) {
    sf::RectangleShape arrow_pole(sf::Vector2f(3, 20));
    arrow_pole.setFillColor(indicator_ref.color);
    arrow_pole.setPosition(sf::Vector2f(indicator_ref.position.x - 1.5f, indicator_ref.position.y - 25));
    window_ref.draw(arrow_pole);
    
    sf::CircleShape arrow_head(8, 3);
    arrow_head.setFillColor(indicator_ref.color);
    arrow_head.setPosition(sf::Vector2f(indicator_ref.position.x - 8, indicator_ref.position.y - 8));
    arrow_head.rotate(sf::degrees(180));
    window_ref.draw(arrow_head);
}

void AlgorithmsDemonstrator::renderRangeIndicator(sf::RenderWindow& window_ref, const VisualIndicator& indicator_ref) {
    float width = indicator_ref.size.x;
    
    sf::RectangleShape line(sf::Vector2f(width, 3));
    line.setFillColor(indicator_ref.color);
    line.setPosition(indicator_ref.position);
    window_ref.draw(line);
    
    sf::RectangleShape left_boundary(sf::Vector2f(3, 15));
    left_boundary.setFillColor(indicator_ref.color);
    left_boundary.setPosition(sf::Vector2f(indicator_ref.position.x, indicator_ref.position.y - 6));
    window_ref.draw(left_boundary);
    
    sf::RectangleShape right_boundary(sf::Vector2f(3, 15));
    right_boundary.setFillColor(indicator_ref.color);
    right_boundary.setPosition(sf::Vector2f(indicator_ref.position.x + width - 3, indicator_ref.position.y - 6));
    window_ref.draw(right_boundary);
}

float AlgorithmsDemonstrator::getBarWidth() const {
    if (data.empty() || content_area.size.x <= 0) return 0.0f;
    return content_area.size.x / data.size();
}

float AlgorithmsDemonstrator::getBarHeight(int index) const {
    if (index < 0 || index >= static_cast<int>(data.size()) || index >= static_cast<int>(bars.size())) {
        return 0.0f;
    }
    return bars[index].getSize().y;
}

sf::Vector2f AlgorithmsDemonstrator::getElementTopCenter(int index) const {
    if (index < 0 || index >= static_cast<int>(bars.size())) {
        return sf::Vector2f(0, 0);
    }
    sf::Vector2f position = bars[index].getPosition();
    float width = bars[index].getSize().x;
    return sf::Vector2f(position.x + width / 2.0f, position.y);
}

sf::Vector2f AlgorithmsDemonstrator::getElementBottomCenter(int index) const {
    if (index < 0 || index >= static_cast<int>(bars.size())) {
        return sf::Vector2f(0, 0);
    }
    sf::Vector2f position = bars[index].getPosition();
    sf::Vector2f size = bars[index].getSize();
    return sf::Vector2f(position.x + size.x / 2.0f, position.y + size.y);
}

sf::FloatRect AlgorithmsDemonstrator::getElementBounds(int index) const {
    if (index < 0 || index >= static_cast<int>(bars.size())) {
        return sf::FloatRect();
    }
    return bars[index].getGlobalBounds();
}
