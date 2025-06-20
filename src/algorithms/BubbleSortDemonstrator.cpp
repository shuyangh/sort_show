#include "BubbleSortDemonstrator.h"
#include "../components/AlgorithmsDemonstrator.h"
#include <algorithm>

const sf::Color BubbleSortDemonstrator::NORMAL_COLOR(135, 206, 235);
const sf::Color BubbleSortDemonstrator::COMPARING_COLOR(50, 205, 50);
const sf::Color BubbleSortDemonstrator::SWAPPING_COLOR(255, 69, 0);
const sf::Color BubbleSortDemonstrator::SORTED_COLOR(95,102,172);

BubbleSortDemonstrator::BubbleSortDemonstrator() {
    reset();
}

std::wstring BubbleSortDemonstrator::getName() const {
    return L"冒泡排序";
}

void BubbleSortDemonstrator::reset() {
    statistics.reset();
    animation_state.reset();
    current_comparison_index = 0;
    bubble_pass = 0;
    bubble_finished = false;
    swapped_in_current_pass = false;
    largest_element_index = -1;
    is_indicator_update_needed = false;
    current_step_description = L"准备开始排序";
    animation_clock.restart();
}

bool BubbleSortDemonstrator::step(std::vector<int>& data_ref, AlgorithmsDemonstrator& demonstrator_ref, float scaled_duration_since_animation_start) {
    if (animation_state.completed || data_ref.empty()) {
        return true;
    }
    
    if (animation_state.is_pausing) {
        updatePause(scaled_duration_since_animation_start);
        return false;
    }

    if (animation_state.is_animating) {
        updateSwapAnimation(data_ref, demonstrator_ref, scaled_duration_since_animation_start);
        return false;
    }
    if (animation_clock.getElapsedTime().asSeconds() >= time_per_step) {
        performBubbleSortStep(data_ref, demonstrator_ref);
        animation_clock.restart();
    } else if (current_step_description == L"准备开始排序") {
        current_step_description = L"开始冒泡排序：将相邻元素进行比较，大的元素向右移动";
    }
    
    updateStatistics(data_ref);
    return false;
}

bool BubbleSortDemonstrator::isCompleted() const {
    return animation_state.completed;
}

const SortStatistics& BubbleSortDemonstrator::getStatistics() const {
    return statistics;
}

const AnimationState& BubbleSortDemonstrator::getAnimationState() const {
    return animation_state;
}

sf::Color BubbleSortDemonstrator::getElementColor(const std::vector<int>& data_ref, int index) const {
    const auto& animation_state = getAnimationState();
    
    for (size_t i = 0; i < animation_state.special_highlight_indices.size(); ++i) {
        if (animation_state.special_highlight_indices[i] == index) {
            return animation_state.special_highlight_colors[i];
        }
    }
    
    for (int animating_element : animation_state.animating_elements) {
        if (animating_element == index) {
            return SWAPPING_COLOR;
        }
    }
    
    if (index == animation_state.left_comparison_index || index == animation_state.right_comparison_index) {
        return COMPARING_COLOR;
    }
    
    if (index >= static_cast<int>(data_ref.size()) - bubble_pass) {
        return SORTED_COLOR;
    }
    
    if (animation_state.completed) {
        return SORTED_COLOR;
    }
    
    return NORMAL_COLOR;
}

void BubbleSortDemonstrator::performBubbleSortStep(std::vector<int>& data_ref, AlgorithmsDemonstrator& demonstrator_ref) {
    if (bubble_finished) {
        current_step_description = L"排序完成！所有元素已按正确顺序排列";
        animation_state.completed = true;
        return;
    }
    
    animation_state.left_comparison_index = -1;
    animation_state.right_comparison_index = -1;
      if (current_comparison_index >= static_cast<int>(data_ref.size()) - bubble_pass - 1) {
        endCurrentPass(data_ref, demonstrator_ref);
        return;
    }
    
    animation_state.left_comparison_index = current_comparison_index;
    animation_state.right_comparison_index = current_comparison_index + 1;
    if (current_comparison_index == 0) {
        largest_element_index = 0;
    }
    statistics.comparisons++;
    statistics.array_accesses += 2;
    
    demonstrator_ref.playNote(data_ref[current_comparison_index]);
    demonstrator_ref.playNote(data_ref[current_comparison_index + 1]);
    
    if (data_ref[current_comparison_index] > data_ref[current_comparison_index + 1]) {
        swapped_in_current_pass = true;
        statistics.assignments += 2;
        
        largest_element_index = current_comparison_index + 1;
        
        current_step_description = L"第 " + std::to_wstring(bubble_pass + 1) + L" 轮：元素 " + std::to_wstring(data_ref[current_comparison_index]) + L" > " + std::to_wstring(data_ref[current_comparison_index + 1]) + L"，需要交换位置";
        
        updateIndicators(demonstrator_ref, data_ref);
        
        startSwapAnimation(current_comparison_index, current_comparison_index + 1, demonstrator_ref);
        return;
    } else {
        largest_element_index = current_comparison_index + 1;
        current_comparison_index++;
        current_step_description = L"第 " + std::to_wstring(bubble_pass + 1) + L" 轮：元素 " + std::to_wstring(data_ref[animation_state.left_comparison_index]) + L" ≤ " + std::to_wstring(data_ref[animation_state.right_comparison_index]) + L"，位置正确，继续下一对";
        
        updateIndicators(demonstrator_ref, data_ref);
        
        startPause(demonstrator_ref);
        return;
    }
}

void BubbleSortDemonstrator::endCurrentPass(std::vector<int>& data_ref, AlgorithmsDemonstrator& demonstrator_ref) {
    statistics.passes++;
    
    current_comparison_index = 0;
    bubble_pass++;
    largest_element_index = -1;
    
    if (!swapped_in_current_pass) {
        current_step_description = L"第 " + std::to_wstring(bubble_pass) + L" 轮完成：无交换发生，排序完成！";
        bubble_finished = true;
        animation_state.completed = true;
        animation_state.left_comparison_index = -1;
        animation_state.right_comparison_index = -1;
        return;
    } else if (bubble_pass >= static_cast<int>(data_ref.size()) - 1) {
        current_step_description = L"所有轮次完成，排序结束！";
        bubble_finished = true;
        animation_state.completed = true;
        animation_state.left_comparison_index = -1;
        animation_state.right_comparison_index = -1;
        return;    } else {
        current_step_description = L"第 " + std::to_wstring(bubble_pass) + L" 轮完成，最大元素已就位。开始第 " + std::to_wstring(bubble_pass + 1) + L" 轮";
        swapped_in_current_pass = false;
        startPause(demonstrator_ref);
    }
}

void BubbleSortDemonstrator::startSwapAnimation(int left_element_index, int right_element_index, AlgorithmsDemonstrator& demonstrator_ref) {
    animation_state.is_animating = true;
    animation_state.animation_progress = 0.0f;
    animation_state.animating_elements = {left_element_index, right_element_index};
    
    animation_state.left_element_index = left_element_index;
    animation_state.right_element_index = right_element_index;
    
    float bar_width = demonstrator_ref.getBarWidth();
    sf::FloatRect content_area = demonstrator_ref.getContentArea();
    
    const auto& data_ref = demonstrator_ref.getData();
    float left_bar_height = (static_cast<float>(data_ref[left_element_index]) / data_ref.size()) * content_area.size.y;
    float right_bar_height = (static_cast<float>(data_ref[right_element_index]) / data_ref.size()) * content_area.size.y;
    
    sf::Vector2f left_element_start_position(
        content_area.position.x + left_element_index * bar_width,
        content_area.position.y + content_area.size.y - left_bar_height
    );
    sf::Vector2f left_element_target_position(
        content_area.position.x + right_element_index * bar_width,
        content_area.position.y + content_area.size.y - left_bar_height
    );
    
    sf::Vector2f right_element_start_position(
        content_area.position.x + right_element_index * bar_width,
        content_area.position.y + content_area.size.y - right_bar_height
    );
    sf::Vector2f right_element_target_position(
        content_area.position.x + left_element_index * bar_width,
        content_area.position.y + content_area.size.y - right_bar_height
    );
    
    animation_state.start_positions = {left_element_start_position, right_element_start_position};
    animation_state.target_positions = {left_element_target_position, right_element_target_position};
}

void BubbleSortDemonstrator::updateSwapAnimation(std::vector<int>& data_ref, AlgorithmsDemonstrator& demonstrator_ref, float scaled_duration_since_animation_start) {
    animation_state.animation_progress += scaled_duration_since_animation_start / swap_animation_duration;
        
    if (animation_state.animation_progress >= 1.0f) {
        if (animation_state.left_element_index >= 0 && animation_state.right_element_index >= 0) {
            demonstrator_ref.playNote(data_ref[animation_state.left_element_index]);
            demonstrator_ref.playNote(data_ref[animation_state.right_element_index]);
        }
        current_step_description = L"交换完成：元素 " + std::to_wstring(data_ref[animation_state.left_element_index]) + L" 和 " + std::to_wstring(data_ref[animation_state.right_element_index]) + L" 已交换位置";
        
        std::swap(data_ref[animation_state.left_element_index], data_ref[animation_state.right_element_index]);
        statistics.array_accesses += 4;
        
        largest_element_index = current_comparison_index + 1;
        
        animation_state.is_animating = false;
        animation_state.animation_progress = 0.0f;
        animation_state.left_element_index = -1;
        animation_state.right_element_index = -1;
        animation_state.animating_elements.clear();
        animation_state.start_positions.clear();
        animation_state.target_positions.clear();
          current_comparison_index++;
        
        startPause(demonstrator_ref);
    }
}

void BubbleSortDemonstrator::startPause(AlgorithmsDemonstrator& demonstrator_ref) {
    animation_state.is_pausing = true;
    animation_state.pause_timer = 0.0f;
    float base_pause_duration = 0.5f;
    animation_state.pause_duration = base_pause_duration / demonstrator_ref.getAnimationSpeed();
}

void BubbleSortDemonstrator::updatePause(float scaled_duration_since_animation_start) {
    animation_state.pause_timer += scaled_duration_since_animation_start;
    if (animation_state.pause_timer >= animation_state.pause_duration) {
        animation_state.is_pausing = false;
        animation_state.pause_timer = 0.0f;
    }
}

void BubbleSortDemonstrator::updateIndicators(AlgorithmsDemonstrator& demonstrator_ref, const std::vector<int>& data_ref) {
    animation_state.indicators.clear();
    
    if (largest_element_index >= 0 && largest_element_index < static_cast<int>(data_ref.size()) && !animation_state.completed) {
        float bar_width = demonstrator_ref.getBarWidth();
        sf::FloatRect content_area = demonstrator_ref.getContentArea();
        float indicator_x = content_area.position.x + largest_element_index * bar_width + bar_width / 2.0f;
        
        sf::Vector2f triangle_pos(
            indicator_x,
            content_area.position.y + content_area.size.y + 5.0f
        );
        
        VisualIndicator triangle(IndicatorType::Triangle, triangle_pos, sf::Color(255, 140, 0));
        animation_state.indicators.push_back(triangle);
    }
}

void BubbleSortDemonstrator::updateStatistics(const std::vector<int>& data_ref) {
    int correct_positions = 0;
    for (size_t i = 0; i < data_ref.size(); ++i) {
        if (data_ref[i] == static_cast<int>(i + 1)) {
            correct_positions++;
        }
    }
    statistics.sortedness = (static_cast<float>(correct_positions) / data_ref.size()) * 100.0f;
}

std::wstring BubbleSortDemonstrator::getCurrentStepExplaination() const {
    return current_step_description;
}
