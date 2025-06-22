#include "QuickSortDemonstrator.h"
#include "../components/AlgorithmsDemonstrator.h"
#include <algorithm>

const sf::Color QuickSortDemonstrator::NORMAL_COLOR(135, 206, 235);
const sf::Color QuickSortDemonstrator::PIVOT_COLOR(255, 215, 0);
const sf::Color QuickSortDemonstrator::COMPARING_LEFT_COLOR(50, 205, 50);
const sf::Color QuickSortDemonstrator::COMPARING_RIGHT_COLOR(255, 69, 0);
const sf::Color QuickSortDemonstrator::SWAPPING_COLOR(255, 20, 147);
const sf::Color QuickSortDemonstrator::PARTITIONED_LESS_COLOR(144, 238, 144, 80);
const sf::Color QuickSortDemonstrator::PARTITIONED_GREATER_COLOR(255, 182, 193, 80);
const sf::Color QuickSortDemonstrator::SORTED_COLOR(95, 102, 172);
const sf::Color QuickSortDemonstrator::SUBARRAY_BACKGROUND_COLOR(255, 215, 0, 100);

QuickSortDemonstrator::QuickSortDemonstrator() : current_sorting_step(-1, -1) {
    reset();
}

std::wstring QuickSortDemonstrator::getName() const {
    return L"快速排序";
}

void QuickSortDemonstrator::reset() {
    statistics.reset();
    animation_state.reset();
    
    while (!quicksort_step_stack.empty()) {
        quicksort_step_stack.pop();
    }
    
    current_sorting_step = QuickSortStep(-1, -1);
    is_initialized = false;
    left_partition_boundary = -1;
    right_partition_boundary = -1;
    pivot_final_sorted_position = -1;
    current_step_description = L"准备开始排序";
    animation_clock.restart();
}

bool QuickSortDemonstrator::step(std::vector<int>& data_ref, AlgorithmsDemonstrator& demonstrator_ref, float scaled_duration_since_animation_start) {
    if (data_ref.empty()) {
        return true;
    }
    
    if (animation_state.completed) {
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
      if (!is_initialized) {
        initializeQuickSort(data_ref);
        return false;
    }
    
    if (animation_clock.getElapsedTime().asSeconds() >= time_per_step) {
        performQuickSortStep(data_ref, demonstrator_ref);
        animation_clock.restart();
    }
    
    updateStatistics(data_ref);
    return false;
}

bool QuickSortDemonstrator::isCompleted() const {
    return animation_state.completed;
}

const SortStatistics& QuickSortDemonstrator::getStatistics() const {
    return statistics;
}

const AnimationState& QuickSortDemonstrator::getAnimationState() const {
    return animation_state;
}

sf::Color QuickSortDemonstrator::getElementColor(const std::vector<int>& data_ref, int element_index) const {
    for (int animating_element : animation_state.animating_elements) {
        if (animating_element == element_index) {
            return SWAPPING_COLOR;
        }
    }
    
    for (size_t highlight_index = 0; highlight_index < animation_state.special_highlight_indices.size(); ++highlight_index) {
        if (animation_state.special_highlight_indices[highlight_index] == element_index) {
            return animation_state.special_highlight_colors[highlight_index];
        }
    }    
    if (element_index == current_sorting_step.pivot_element_index && current_sorting_step.pivot_element_index >= 0) {
        return PIVOT_COLOR;
    }
    
    if (element_index == animation_state.left_comparison_index) {
        return COMPARING_LEFT_COLOR;
    }
    if (element_index == animation_state.right_comparison_index) {
        return COMPARING_RIGHT_COLOR;
    }
    
    if (animation_state.completed) {
        return SORTED_COLOR;
    }
    
    return NORMAL_COLOR;
}

void QuickSortDemonstrator::initializeQuickSort(const std::vector<int>& data_ref) {
    if (data_ref.size() <= 1) {
        animation_state.completed = true;
        current_step_description = L"数组只有一个元素或为空，无需排序！";
        return;
    }
      quicksort_step_stack.push(QuickSortStep(0, static_cast<int>(data_ref.size()) - 1));
    is_initialized = true;
    current_step_description = L"开始快速排序：对数组进行分区排序";
}

void QuickSortDemonstrator::performQuickSortStep(std::vector<int>& data_ref, AlgorithmsDemonstrator& demonstrator_ref) {
    if (current_sorting_step.subarray_left_index == -1) {
        while (!quicksort_step_stack.empty()) {
            current_sorting_step = quicksort_step_stack.top();
            quicksort_step_stack.pop();
            
            if (current_sorting_step.subarray_left_index < current_sorting_step.subarray_right_index) {
                break;
            }
            current_sorting_step = QuickSortStep(-1, -1);
        }
        
        if (current_sorting_step.subarray_left_index == -1) {
            current_step_description = L"排序完成！所有元素都已正确排序";
            animation_state.completed = true;
            animation_state.left_comparison_index = -1;
            animation_state.right_comparison_index = -1;
            updateIndicators(demonstrator_ref, data_ref);
            return;
        }
        
        current_sorting_step.is_in_pivot_selection_phase = true;
        current_sorting_step.is_partitioning = false;
        current_sorting_step.is_in_recursive_call_phase = false;
    }
    
    if (current_sorting_step.is_in_pivot_selection_phase) {
        selectPivot(data_ref, demonstrator_ref);
    } else if (current_sorting_step.is_partitioning) {
        performPartitioning(data_ref, demonstrator_ref);
    }
}

void QuickSortDemonstrator::selectPivot(std::vector<int>& data_ref, AlgorithmsDemonstrator& demonstrator_ref) {
    current_sorting_step.pivot_element_index = current_sorting_step.subarray_right_index;
    current_sorting_step.partition_boundary_index = current_sorting_step.subarray_left_index;
    current_sorting_step.current_comparison_index = current_sorting_step.subarray_left_index;
    pivot_final_sorted_position = current_sorting_step.subarray_left_index;
    
    current_step_description = L"处理子数组 [" + std::to_wstring(current_sorting_step.subarray_left_index) + L", " + std::to_wstring(current_sorting_step.subarray_right_index) + L"]，选择元素 " + std::to_wstring(data_ref[current_sorting_step.pivot_element_index]) + L" 作为基准";
    
    current_sorting_step.is_in_pivot_selection_phase = false;
    current_sorting_step.is_partitioning = true;
    
    demonstrator_ref.playNote(data_ref[current_sorting_step.pivot_element_index]);
    updateIndicators(demonstrator_ref, data_ref);
    startPause(demonstrator_ref);
}

void QuickSortDemonstrator::performPartitioning(std::vector<int>& data_ref, AlgorithmsDemonstrator& demonstrator_ref) {
    if (current_sorting_step.current_comparison_index >= current_sorting_step.subarray_right_index) {
        finalizePivotPosition(data_ref, demonstrator_ref);
        return;
    }
    
    animation_state.left_comparison_index = current_sorting_step.current_comparison_index;
    animation_state.right_comparison_index = current_sorting_step.pivot_element_index;
    
    statistics.comparisons++;
    statistics.array_accesses += 2;
    
    demonstrator_ref.playNote(data_ref[current_sorting_step.current_comparison_index]);
    demonstrator_ref.playNote(data_ref[current_sorting_step.pivot_element_index]);
    
    if (data_ref[current_sorting_step.current_comparison_index] <= data_ref[current_sorting_step.pivot_element_index]) {
        if (current_sorting_step.current_comparison_index != pivot_final_sorted_position) {
            current_step_description = L"元素 " + std::to_wstring(data_ref[current_sorting_step.current_comparison_index]) + L" ≤ 基准 " + std::to_wstring(data_ref[current_sorting_step.pivot_element_index]) + L"，移动到左侧分区";
            
            updateIndicators(demonstrator_ref, data_ref);
            startSwapAnimation(current_sorting_step.current_comparison_index, pivot_final_sorted_position, demonstrator_ref);
        } else {
            current_step_description = L"元素 " + std::to_wstring(data_ref[current_sorting_step.current_comparison_index]) + L" ≤ 基准 " + std::to_wstring(data_ref[current_sorting_step.pivot_element_index]) + L"，扩大左侧分区以包含该元素";
            current_sorting_step.current_comparison_index++;
            updateIndicators(demonstrator_ref, data_ref);
            startPause(demonstrator_ref);
        }
        pivot_final_sorted_position++;
    } else {
        current_step_description = L"元素 " + std::to_wstring(data_ref[current_sorting_step.current_comparison_index]) + L" > 基准 " + std::to_wstring(data_ref[current_sorting_step.pivot_element_index]) + L"，保持在右侧分区";
        current_sorting_step.current_comparison_index++;
        updateIndicators(demonstrator_ref, data_ref);
        startPause(demonstrator_ref);
    }
}

void QuickSortDemonstrator::finalizePivotPosition(std::vector<int>& data_ref, AlgorithmsDemonstrator& demonstrator_ref) {
    animation_state.left_comparison_index = -1;
    animation_state.right_comparison_index = -1;
    
    if (pivot_final_sorted_position != current_sorting_step.pivot_element_index) {
        current_step_description = L"分区完成，将基准元素放到正确位置 " + std::to_wstring(pivot_final_sorted_position);
        
        updateIndicators(demonstrator_ref, data_ref);
        startSwapAnimation(pivot_final_sorted_position, current_sorting_step.pivot_element_index, demonstrator_ref);
        current_sorting_step.pivot_element_index = pivot_final_sorted_position;
    } else {
        current_step_description = L"基准元素已在正确位置，分区完成";
        completePartitioning(data_ref, demonstrator_ref);
    }
}

void QuickSortDemonstrator::addRecursiveCalls(std::vector<int>& data_ref) {
    if (current_sorting_step.pivot_element_index + 1 < current_sorting_step.subarray_right_index) {
        quicksort_step_stack.push(QuickSortStep(current_sorting_step.pivot_element_index + 1, current_sorting_step.subarray_right_index));
    }
    
    if (current_sorting_step.subarray_left_index < current_sorting_step.pivot_element_index - 1) {
        quicksort_step_stack.push(QuickSortStep(current_sorting_step.subarray_left_index, current_sorting_step.pivot_element_index - 1));
    }
}

void QuickSortDemonstrator::completePartitioning(std::vector<int>& data_ref, AlgorithmsDemonstrator& demonstrator_ref) {
    addRecursiveCalls(data_ref);
    current_sorting_step = QuickSortStep(-1, -1);
    statistics.passes++;
    updateIndicators(demonstrator_ref, data_ref);
    startPause(demonstrator_ref);
}

void QuickSortDemonstrator::startSwapAnimation(int left_element_index, int right_element_index, AlgorithmsDemonstrator& demonstrator_ref) {
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

void QuickSortDemonstrator::updateSwapAnimation(std::vector<int>& data_ref, AlgorithmsDemonstrator& demonstrator_ref, float scaled_duration_since_animation_start) {
    animation_state.animation_progress += scaled_duration_since_animation_start / swap_animation_duration;
    
    updateIndicators(demonstrator_ref, data_ref);
        
    if (animation_state.animation_progress >= 1.0f) {
        if (animation_state.left_element_index >= 0 && animation_state.right_element_index >= 0) {
            demonstrator_ref.playNote(data_ref[animation_state.left_element_index]);
            demonstrator_ref.playNote(data_ref[animation_state.right_element_index]);
        }
        std::swap(data_ref[animation_state.left_element_index], data_ref[animation_state.right_element_index]);
        statistics.assignments += 2;
        statistics.array_accesses += 4;
        
        current_step_description = L"交换完成";
        
        animation_state.is_animating = false;
        animation_state.animation_progress = 0.0f;
        animation_state.left_element_index = -1;
        animation_state.right_element_index = -1;
        animation_state.animating_elements.clear();
        animation_state.start_positions.clear();
        animation_state.target_positions.clear();
        
        updateIndicators(demonstrator_ref, data_ref);
          if (current_sorting_step.current_comparison_index < current_sorting_step.subarray_right_index) {
            current_sorting_step.current_comparison_index++;
            startPause(demonstrator_ref);
        } else {
            completePartitioning(data_ref, demonstrator_ref);
        }
    }
}

void QuickSortDemonstrator::startPause(AlgorithmsDemonstrator& demonstrator_ref) {
    animation_state.is_pausing = true;
    animation_state.pause_timer = 0.0f;
    float base_pause_duration = 0.5f;
    animation_state.pause_duration = base_pause_duration / demonstrator_ref.getAnimationSpeed();
}

void QuickSortDemonstrator::updatePause(float scaled_duration_since_animation_start) {
    animation_state.pause_timer += scaled_duration_since_animation_start;
    if (animation_state.pause_timer >= animation_state.pause_duration) {
        animation_state.is_pausing = false;
        animation_state.pause_timer = 0.0f;
    }
}

void QuickSortDemonstrator::updateIndicators(AlgorithmsDemonstrator& demonstrator_ref, const std::vector<int>& data_ref) {
    animation_state.indicators.clear();
    
    if (current_sorting_step.pivot_element_index >= 0 && !animation_state.completed) {
        float bar_width = demonstrator_ref.getBarWidth();
        sf::FloatRect content_area = demonstrator_ref.getContentArea();
        
        if (current_sorting_step.is_partitioning && pivot_final_sorted_position > current_sorting_step.subarray_left_index) {
            float left_partition_indicator_start_x = content_area.position.x + current_sorting_step.subarray_left_index * bar_width;
            float left_partition_indicator_width = (pivot_final_sorted_position - current_sorting_step.subarray_left_index) * bar_width;
            
            sf::Vector2f left_partition_indicator_position(left_partition_indicator_start_x, content_area.position.y);
            sf::Vector2f left_partition_size(left_partition_indicator_width, content_area.size.y);
            
            VisualIndicator left_partition_indicator(IndicatorType::Rectangle, left_partition_indicator_position, PARTITIONED_LESS_COLOR);
            left_partition_indicator.size = left_partition_size;
            animation_state.indicators.push_back(left_partition_indicator);
        }
        
        if (current_sorting_step.is_partitioning && pivot_final_sorted_position < current_sorting_step.pivot_element_index) {
            float right_partition_indicator_start_x = content_area.position.x + pivot_final_sorted_position * bar_width;
            float right_partition_indicator_width = (current_sorting_step.pivot_element_index - pivot_final_sorted_position) * bar_width;
            
            sf::Vector2f right_partition_indicator_position(right_partition_indicator_start_x, content_area.position.y);
            sf::Vector2f right_partition_indicator_size(right_partition_indicator_width, content_area.size.y);
            
            VisualIndicator right_partition_indicator(IndicatorType::Rectangle, right_partition_indicator_position, PARTITIONED_GREATER_COLOR);
            right_partition_indicator.size = right_partition_indicator_size;
            animation_state.indicators.push_back(right_partition_indicator);
        }

        if (current_sorting_step.is_partitioning && current_sorting_step.subarray_left_index < current_sorting_step.pivot_element_index) {
            float range_indicator_start_x = content_area.position.x + current_sorting_step.subarray_left_index * bar_width;
            float range_indicator_end_x = content_area.position.x + current_sorting_step.pivot_element_index * bar_width;
            float range_indicator_width = range_indicator_end_x - range_indicator_start_x;
            
            sf::Vector2f range_indicator_position(range_indicator_start_x, content_area.position.y + content_area.size.y + 5.0f);
            sf::Vector2f range_indicator_size(range_indicator_width, 3.0f);
            
            VisualIndicator range_indicator(IndicatorType::Range, range_indicator_position, sf::Color(100, 100, 100, 180));
            range_indicator.size = range_indicator_size;
            animation_state.indicators.push_back(range_indicator);
        }
        
        float pivot_indicator_x = content_area.position.x + current_sorting_step.pivot_element_index * bar_width + bar_width / 2.0f;
        sf::Vector2f pivot_position(pivot_indicator_x, content_area.position.y + content_area.size.y + 5.0f);
        VisualIndicator pivot_indicator(IndicatorType::Triangle, pivot_position, PIVOT_COLOR);
        animation_state.indicators.push_back(pivot_indicator);
    }
}

void QuickSortDemonstrator::updateStatistics(const std::vector<int>& data_ref) {
    int correct_positions = 0;
    for (size_t element_index = 0; element_index < data_ref.size(); ++element_index) {
        if (data_ref[element_index] == static_cast<int>(element_index + 1)) {
            correct_positions++;
        }
    }
    statistics.sortedness = (static_cast<float>(correct_positions) / data_ref.size()) * 100.0f;
}

std::wstring QuickSortDemonstrator::getCurrentStepExplaination() const {
    return current_step_description;
}
