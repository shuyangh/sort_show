#include "MergeSortDemonstrator.h"
#include "../components/AlgorithmsDemonstrator.h"
#include <algorithm>

const sf::Color MergeSortDemonstrator::NORMAL_COLOR(135, 206, 235);
const sf::Color MergeSortDemonstrator::DIVIDING_COLOR(255, 215, 0);
const sf::Color MergeSortDemonstrator::MERGING_LEFT_COLOR(50, 205, 50);
const sf::Color MergeSortDemonstrator::MERGING_RIGHT_COLOR(255, 69, 0);
const sf::Color MergeSortDemonstrator::MERGING_TARGET_COLOR(255, 20, 147);
const sf::Color MergeSortDemonstrator::SORTED_COLOR(95, 102, 172);
const sf::Color MergeSortDemonstrator::COMPARING_LEFT_COLOR(0, 255, 0);
const sf::Color MergeSortDemonstrator::COMPARING_RIGHT_COLOR(255, 0, 0);
const sf::Color MergeSortDemonstrator::TEMP_ARRAY_COLOR(255, 255, 0);
const sf::Color MergeSortDemonstrator::SUBARRAY_BACKGROUND_COLOR(200, 200, 200, 100);

MergeSortDemonstrator::MergeSortDemonstrator() : current_step(0, 0, 0) {
    reset();
}

std::wstring MergeSortDemonstrator::getName() const {
    return L"归并排序";
}

void MergeSortDemonstrator::reset() {
    statistics.reset();
    animation_state.reset();
    
    while (!merge_queue.empty()) {
        merge_queue.pop();
    }
    
    current_step = MergeStep(-1, -1, -1);
    is_initialized = false;
    has_merge_finished = false;
    current_step_description = L"准备开始排序";
    animation_clock.restart();
    
    animation_state.indicators.clear();
}

bool MergeSortDemonstrator::step(std::vector<int>& data_ref, AlgorithmsDemonstrator& demonstrator_ref, float scaled_duration_since_animation_start) {
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
        performMergeSortStep(data_ref, demonstrator_ref);
        animation_clock.restart();
    } else if (current_step_description == L"准备开始排序") {
        current_step_description = L"开始归并排序：将数组递归分解为单个元素，然后合并";
    }
    
    updateStatistics(data_ref);
    return false;
}

bool MergeSortDemonstrator::isCompleted() const {
    return animation_state.completed;
}

const SortStatistics& MergeSortDemonstrator::getStatistics() const {
    return statistics;
}

const AnimationState& MergeSortDemonstrator::getAnimationState() const {
    return animation_state;
}

sf::Color MergeSortDemonstrator::getElementColor(const std::vector<int>& data_ref, int index) const {
    const auto& animation_state = getAnimationState();
    
    for (int animating_element : animation_state.animating_elements) {
        if (animating_element == index) {
            return MERGING_TARGET_COLOR;
        }
    }
    
    for (size_t highlight_index = 0; highlight_index < animation_state.special_highlight_indices.size(); ++highlight_index) {
        if (animation_state.special_highlight_indices[highlight_index] == index) {
            return animation_state.special_highlight_colors[highlight_index];
        }
    }
    
    if (!has_merge_finished && !current_step.temp_array.empty() && !current_step.is_dividing) {
        if (current_step.current_comparing_left == index) {
            return COMPARING_LEFT_COLOR;
        }
        if (current_step.current_comparing_right == index) {
            return COMPARING_RIGHT_COLOR;
        }
        
        if (current_step.current_target_position == index) {
            return TEMP_ARRAY_COLOR;
        }
        
        if (index >= current_step.left_index && index <= current_step.right_index) {
            if (index <= current_step.middle_index) {
                if (index >= current_step.merge_left_index) {
                    return sf::Color(MERGING_RIGHT_COLOR.r, MERGING_RIGHT_COLOR.g, MERGING_RIGHT_COLOR.b, 150);
                }
            } else {
                if (index >= current_step.merge_right_index) {
                    return sf::Color(MERGING_RIGHT_COLOR.r, MERGING_RIGHT_COLOR.g, MERGING_RIGHT_COLOR.b, 150);
                }
            }
        }
    }
    
    if (!has_merge_finished && current_step.is_dividing) {
        if (index >= current_step.left_index && index <= current_step.right_index) {
            return sf::Color(DIVIDING_COLOR.r, DIVIDING_COLOR.g, DIVIDING_COLOR.b, 180);
        }
    }
    
    if (animation_state.completed) {
        return SORTED_COLOR;
    }
    
    return NORMAL_COLOR;
}

std::wstring MergeSortDemonstrator::getCurrentStepExplaination() const {
    return current_step_description;
}

void MergeSortDemonstrator::initializeMergeSort(const std::vector<int>& data_ref) {
    generateMergeSteps(data_ref);
}

void MergeSortDemonstrator::generateMergeSteps(const std::vector<int>& data_ref) {
    int data_size = static_cast<int>(data_ref.size());
    
    if (data_size <= 1) {
        return;
    }
    
    merge_queue.push(MergeStep(0, 0, data_size - 1, true));
    
    for (int subarray_size = 1; subarray_size < data_size; subarray_size = subarray_size * 2) {
        for (int left_start_index = 0; left_start_index < data_size - 1; left_start_index += subarray_size * 2) {
            int middle_index = std::min(left_start_index + subarray_size - 1, data_size - 1);
            int right_end_index = std::min(left_start_index + subarray_size * 2 - 1, data_size - 1);
            
            if (middle_index < right_end_index) {
                merge_queue.push(MergeStep(left_start_index, middle_index, right_end_index, true));
                
                if (right_end_index - left_start_index == 1 && data_ref[left_start_index] > data_ref[right_end_index]) {
                    MergeStep swap_step(left_start_index, middle_index, right_end_index, false);
                    swap_step.is_swap_animation_needed = true;
                    swap_step.swap_source_index = left_start_index;
                    swap_step.swap_target_index = right_end_index;
                    merge_queue.push(swap_step);
                } else {
                    merge_queue.push(MergeStep(left_start_index, middle_index, right_end_index, false));
                }
            }
        }
    }
}

void MergeSortDemonstrator::performMergeSortStep(std::vector<int>& data_ref, AlgorithmsDemonstrator& demonstrator_ref) {
    if (!is_initialized) {
        initializeMergeSort(data_ref);
        is_initialized = true;
        if (merge_queue.empty()) {
            current_step_description = L"数组已经排序完成";
            animation_state.completed = true;
            return;
        }
        current_step_description = L"开始归并排序演示";
        return;
    }
    
    if (current_step.left_index < 0 || current_step.right_index < 0) {
        if (merge_queue.empty()) {
            current_step_description = L"排序完成！所有元素已按正确顺序排列";
            animation_state.completed = true;
            has_merge_finished = true;
            return;
        }
        
        current_step = merge_queue.front();
        merge_queue.pop();
    }
    
    if (current_step.is_dividing) {
        performDivideStep(data_ref, demonstrator_ref);
        current_step = MergeStep(-1, -1, -1);
        updateIndicators(demonstrator_ref, data_ref);
    } else {
        bool merge_complete = performMergeStepOnce(data_ref, demonstrator_ref);
        if (merge_complete) {
            current_step = MergeStep(-1, -1, -1);
            updateIndicators(demonstrator_ref, data_ref);
        }
    }
}

void MergeSortDemonstrator::performDivideStep(std::vector<int>& data_ref, AlgorithmsDemonstrator& demonstrator_ref) {
    if (current_step.left_index == 0 && current_step.right_index == static_cast<int>(data_ref.size()) - 1 && 
        current_step.middle_index == 0) {
        current_step_description = L"初始状态：数组包含 " + std::to_wstring(data_ref.size()) + L" 个元素，准备开始归并排序";
        updateIndicators(demonstrator_ref, data_ref);
        return;
    }
    
    if (current_step.left_index >= current_step.right_index) {
        current_step_description = L"单个元素 [" + std::to_wstring(current_step.left_index) + L"]，已经是有序的";
        return;
    }
    
    int size = current_step.right_index - current_step.left_index + 1;
    if (size == 2) {
        current_step_description = L"准备合并两个相邻元素：位置 " + std::to_wstring(current_step.left_index) + L" 和 " + std::to_wstring(current_step.right_index) + L"，值为 " + std::to_wstring(data_ref[current_step.left_index]) + L" 和 " + std::to_wstring(data_ref[current_step.right_index]);
    } else {
        current_step_description = L"准备合并两个有序子数组：\n左半部分 [" + std::to_wstring(current_step.left_index) + L", " + std::to_wstring(current_step.middle_index) + L"] " + L"和右半部分 [" + std::to_wstring(current_step.middle_index + 1) + L", " + std::to_wstring(current_step.right_index) + L"]";
    }
    
    updateIndicators(demonstrator_ref, data_ref);
}

bool MergeSortDemonstrator::performMergeStepOnce(std::vector<int>& data_ref, AlgorithmsDemonstrator& demonstrator_ref) {
    if (current_step.left_index >= current_step.right_index) {
        current_step_description = L"单个元素，无需合并";
        return true;
    }
    
    if (current_step.is_swap_animation_needed) {
        return handleSwapAnimation(data_ref, demonstrator_ref);
    }
    
    if (handlePairSwapCompletion(data_ref, demonstrator_ref)) {
        return false;
    }
    
    return performStandardMerge(data_ref, demonstrator_ref);
}

bool MergeSortDemonstrator::performStandardMerge(std::vector<int>& data_ref, AlgorithmsDemonstrator& demonstrator_ref) {
    if (current_step.temp_array.empty()) {
        current_step.temp_array.resize(current_step.right_index - current_step.left_index + 1);
        current_step.merge_left_index = current_step.left_index;
        current_step.merge_right_index = current_step.middle_index + 1;
        current_step.merge_target_index = 0;
        
        current_step_description = L"开始合并";
        updateIndicators(demonstrator_ref, data_ref);
        return false;
    }
    
    if (current_step.merge_left_index > current_step.middle_index && 
        current_step.merge_right_index > current_step.right_index) {
        
        if (!current_step.is_in_copy_back_phase) {
            return handleMergeCompletion(data_ref, demonstrator_ref);
        }
        
        return performCopyBackWithSwaps(data_ref, demonstrator_ref);
    }
    
    return performSingleMergeStep(data_ref, demonstrator_ref);
}

void MergeSortDemonstrator::generateCopyBackSwaps(std::vector<int>& data_ref, AlgorithmsDemonstrator& demonstrator_ref) {
    current_step.pending_swaps.clear();
    
    std::vector<int> current_data(data_ref.begin() + current_step.left_index, data_ref.begin() + current_step.right_index + 1);
    
    for (int array_index = 0; array_index < static_cast<int>(current_step.temp_array.size()); array_index++) {
        int target_position = current_step.left_index + array_index;
        int target_value = current_step.temp_array[array_index];
        
        if (current_data[array_index] != target_value) {
            for (int search_index = array_index + 1; search_index < static_cast<int>(current_data.size()); search_index++) {
                if (current_data[search_index] == target_value) {
                    current_step.pending_swaps.push_back({current_step.left_index + search_index, target_position});
                    
                    std::swap(current_data[array_index], current_data[search_index]);
                    break;
                }
            }
        }
    }
}

bool MergeSortDemonstrator::performCopyBackWithSwaps(std::vector<int>& data_ref, AlgorithmsDemonstrator& demonstrator_ref) {
    if (current_step.is_swap_animation_needed) {
        return false;
    }
    
    if (current_step.copy_back_index >= static_cast<int>(current_step.temp_array.size())) {
        std::wstring final_result = L"";
        for (int element_index = current_step.left_index; element_index <= current_step.right_index; element_index++) {
            if (!final_result.empty()) final_result += L", ";
            final_result += std::to_wstring(data_ref[element_index]);
        }
        
        current_step_description = L"合并完成！最终结果：[" + final_result + L"]";
        statistics.assignments += static_cast<int>(current_step.temp_array.size());
        statistics.passes++;
        
        current_step.temp_array.clear();
        current_step.pending_swaps.clear();
        current_step.is_in_copy_back_phase = false;
        return true;
    }
    
    int target_position = current_step.left_index + current_step.copy_back_index;
    int target_value = current_step.temp_array[current_step.copy_back_index];
    
    if (data_ref[target_position] == target_value) {
        demonstrator_ref.playNote(target_value);
        current_step_description = L"元素 " + std::to_wstring(target_value) + L" 已在正确位置（位置 " + std::to_wstring(target_position) + L"）";
        current_step.copy_back_index++;
        updateIndicators(demonstrator_ref, data_ref);
        return false;
    }
    
    for (int search_index = target_position + 1; search_index <= current_step.right_index; search_index++) {
        if (data_ref[search_index] == target_value) {
            current_step.is_swap_animation_needed = true;
            current_step.swap_source_index = search_index;
            current_step.swap_target_index = target_position;
            
            current_step_description = L"将元素 " + std::to_wstring(target_value) + L" 从位置 " + std::to_wstring(search_index) + L" 移动到位置 " + std::to_wstring(target_position);
            
            startSwapAnimation(search_index, target_position, demonstrator_ref);
            current_step.copy_back_index++;
            return false;
        }
    }
    
    current_step.copy_back_index++;
    return false;
}

void MergeSortDemonstrator::updateStatistics(const std::vector<int>& data_ref) {
    int inversions = 0;
    for (size_t first_index = 0; first_index < data_ref.size(); ++first_index) {
        for (size_t second_index = first_index + 1; second_index < data_ref.size(); ++second_index) {
            if (data_ref[first_index] > data_ref[second_index]) {
                inversions++;
            }
        }
    }
    
    int maximum_inversions_count = static_cast<int>(data_ref.size() * (data_ref.size() - 1) / 2);
    statistics.sortedness = maximum_inversions_count > 0 ? (1.0f - static_cast<float>(inversions) / maximum_inversions_count) * 100.0f : 100.0f;
}

void MergeSortDemonstrator::updateIndicators(AlgorithmsDemonstrator& demonstrator_ref, const std::vector<int>& data_ref) {
    animation_state.indicators.clear();
    
    if (has_merge_finished || current_step.left_index < 0 || current_step.right_index < 0 || current_step.left_index > current_step.right_index) {
        return;
    }
    
    float bar_width = demonstrator_ref.getBarWidth();
    sf::FloatRect content_area = demonstrator_ref.getContentArea();
    
    if (current_step.is_dividing) {
        addDividingIndicators(bar_width, content_area);
    } else if (!current_step.temp_array.empty()) {
        addMergingIndicators(bar_width, content_area, data_ref);
    }
    
    addTriangleIndicator(bar_width, content_area);
    addSpecialPairSwapIndicator(bar_width, content_area);
}

void MergeSortDemonstrator::startSwapAnimation(int source_index, int target_index, AlgorithmsDemonstrator& demonstrator_ref) {
    animation_state.is_animating = true;
    animation_state.animation_progress = 0.0f;
    animation_state.animating_elements = {source_index, target_index};
    
    animation_state.left_element_index = source_index;
    animation_state.right_element_index = target_index;
    
    float bar_width = demonstrator_ref.getBarWidth();
    sf::FloatRect content_area = demonstrator_ref.getContentArea();
    
    const auto& data_ref = demonstrator_ref.getData();
    float source_bar_height = (static_cast<float>(data_ref[source_index]) / data_ref.size()) * content_area.size.y;
    float target_bar_height = (static_cast<float>(data_ref[target_index]) / data_ref.size()) * content_area.size.y;
    
    sf::Vector2f source_element_start_position(
        content_area.position.x + source_index * bar_width,
        content_area.position.y + content_area.size.y - source_bar_height
    );
    sf::Vector2f source_element_target_position(
        content_area.position.x + target_index * bar_width,
        content_area.position.y + content_area.size.y - source_bar_height
    );
    
    sf::Vector2f target_element_start_position(
        content_area.position.x + target_index * bar_width,
        content_area.position.y + content_area.size.y - target_bar_height
    );
    sf::Vector2f target_element_target_position(
        content_area.position.x + source_index * bar_width,
        content_area.position.y + content_area.size.y - target_bar_height
    );
    
    animation_state.start_positions = {source_element_start_position, target_element_start_position};
    animation_state.target_positions = {source_element_target_position, target_element_target_position};
}

void MergeSortDemonstrator::updateSwapAnimation(std::vector<int>& data_ref, AlgorithmsDemonstrator& demonstrator_ref, float scaled_duration_since_animation_start) {
    animation_state.animation_progress += scaled_duration_since_animation_start / swap_animation_duration;
        
    if (animation_state.animation_progress >= 1.0f) {
        if (animation_state.left_element_index >= 0 && animation_state.right_element_index >= 0) {
            demonstrator_ref.playNote(data_ref[animation_state.left_element_index]);
            demonstrator_ref.playNote(data_ref[animation_state.right_element_index]);
        }
        
        current_step_description = L"元素交换完成：" + std::to_wstring(data_ref[animation_state.left_element_index]) + L" 和 " + std::to_wstring(data_ref[animation_state.right_element_index]) + L" 已交换位置";
        
        std::swap(data_ref[animation_state.left_element_index], data_ref[animation_state.right_element_index]);
        statistics.array_accesses += 4;
        statistics.assignments += 2;
        
        animation_state.is_animating = false;
        animation_state.animation_progress = 0.0f;
        animation_state.left_element_index = -1;
        animation_state.right_element_index = -1;
        animation_state.animating_elements.clear();
        animation_state.start_positions.clear();
        animation_state.target_positions.clear();
        
        current_step.is_swap_animation_needed = false;
        current_step.swap_source_index = -1;
        current_step.swap_target_index = -1;
        
        if (!current_step.is_in_copy_back_phase && current_step.right_index - current_step.left_index == 1) {
            current_step.is_swap_complete = true;
        }
        
        startPause(demonstrator_ref);
    }
}

void MergeSortDemonstrator::startPause(AlgorithmsDemonstrator& demonstrator_ref) {
    animation_state.is_pausing = true;
    animation_state.pause_timer = 0.0f;
    float base_pause_duration = 0.5f;
    animation_state.pause_duration = base_pause_duration / demonstrator_ref.getAnimationSpeed();
}

void MergeSortDemonstrator::updatePause(float scaled_duration_since_animation_start) {
    animation_state.pause_timer += scaled_duration_since_animation_start;
    if (animation_state.pause_timer >= animation_state.pause_duration) {
        animation_state.is_pausing = false;
        animation_state.pause_timer = 0.0f;
        
        if (current_step.is_swap_animation_needed && current_step.is_rectangle_swap_visible && 
            !current_step.is_swap_pause_completed && !current_step.is_in_copy_back_phase) {
            current_step.is_swap_pause_completed = true;
        }
    }
}

bool MergeSortDemonstrator::handleSwapAnimation(std::vector<int>& data_ref, AlgorithmsDemonstrator& demonstrator_ref) {
    if (!current_step.is_in_copy_back_phase && !current_step.is_rectangle_swap_visible) {
        if (current_step.temp_array.empty()) {
            current_step.temp_array.resize(2);
            current_step.merge_left_index = current_step.left_index;
            current_step.merge_right_index = current_step.middle_index + 1;
            current_step.merge_target_index = 0;
            
            current_step_description = L"开始比较相邻元素：" + std::to_wstring(data_ref[current_step.swap_source_index]) + L" 和 " + std::to_wstring(data_ref[current_step.swap_target_index]);
            updateIndicators(demonstrator_ref, data_ref);
            return false;
        }
        
        current_step_description = L"检测到需要交换：" + std::to_wstring(data_ref[current_step.swap_source_index]) + L" > " + std::to_wstring(data_ref[current_step.swap_target_index]);
        
        current_step.is_rectangle_swap_visible = true;
        updateIndicators(demonstrator_ref, data_ref);
        startPause(demonstrator_ref);
        return false;
    }
    
    if (!current_step.is_in_copy_back_phase && current_step.is_rectangle_swap_visible && 
        !current_step.is_swap_pause_completed) {
        return false;
    }
    
    if (current_step.swap_source_index >= 0 && current_step.swap_target_index >= 0) {
        if (!current_step.is_in_copy_back_phase) {
            current_step_description = L"执行交换：" + std::to_wstring(data_ref[current_step.swap_source_index]) + L" <-> " + std::to_wstring(data_ref[current_step.swap_target_index]);
        }
        
        startSwapAnimation(current_step.swap_source_index, current_step.swap_target_index, demonstrator_ref);
        return false;
    }
    return false;
}

bool MergeSortDemonstrator::handlePairSwapCompletion(std::vector<int>& data_ref, AlgorithmsDemonstrator& demonstrator_ref) {
    if (!current_step.is_in_copy_back_phase && current_step.is_swap_complete && 
        current_step.right_index - current_step.left_index == 1 && !current_step.showing_final_rectangle) {
        
        current_step.temp_array.clear();
        current_step.merge_left_index = current_step.middle_index + 1;
        current_step.merge_right_index = current_step.right_index + 1;
        
        std::wstring final_result = L"";
        for (int element_index = current_step.left_index; element_index <= current_step.right_index; element_index++) {
            if (!final_result.empty()) final_result += L", ";
            final_result += std::to_wstring(data_ref[element_index]);
        }
        current_step_description = L"配对交换完成！结果：[" + final_result + L"]";
        
        current_step.showing_final_rectangle = true;
        updateIndicators(demonstrator_ref, data_ref);
        startPause(demonstrator_ref);
        return true;
    }
    
    if (!current_step.is_in_copy_back_phase && current_step.showing_final_rectangle && 
        current_step.right_index - current_step.left_index == 1 && !animation_state.is_pausing) {
        
        resetPairSwapState();
        statistics.passes++;
        return true;
    }
    
    return false;
}

bool MergeSortDemonstrator::handleMergeCompletion(std::vector<int>& data_ref, AlgorithmsDemonstrator& demonstrator_ref) {
    bool is_copy_back_needed = false;
    for (int array_index = 0; array_index < static_cast<int>(current_step.temp_array.size()); array_index++) {
        if (data_ref[current_step.left_index + array_index] != current_step.temp_array[array_index]) {
            is_copy_back_needed = true;
            break;
        }
    }
    
    if (!is_copy_back_needed) {
        std::wstring final_result = L"";
        for (int element_index = current_step.left_index; element_index <= current_step.right_index; element_index++) {
            if (!final_result.empty()) final_result += L", ";
            final_result += std::to_wstring(data_ref[element_index]);
        }
        
        current_step_description = L"合并完成！元素已排序：[" + final_result + L"]";
        statistics.passes++;
        
        current_step.temp_array.clear();
        return true;
    }
    
    current_step.is_in_copy_back_phase = true;
    current_step.copy_back_index = 0;
    generateCopyBackSwaps(data_ref, demonstrator_ref);
    
    std::wstring merged_array_preview = L"";
    for (int array_index = 0; array_index < static_cast<int>(current_step.temp_array.size()); array_index++) {
        if (!merged_array_preview.empty()) merged_array_preview += L", ";
        merged_array_preview += std::to_wstring(current_step.temp_array[array_index]);
    }
    
    current_step_description = L"合并完成！目标序列：[" + merged_array_preview + L"]\n开始将结果应用到原数组";
    return false;
}

bool MergeSortDemonstrator::performSingleMergeStep(std::vector<int>& data_ref, AlgorithmsDemonstrator& demonstrator_ref) {
    int selected_value;
    
    if (current_step.merge_left_index <= current_step.middle_index && (current_step.merge_right_index > current_step.right_index || data_ref[current_step.merge_left_index] <= data_ref[current_step.merge_right_index])) {
        selected_value = data_ref[current_step.merge_left_index];
        current_step.temp_array[current_step.merge_target_index] = selected_value;
        current_step_description = L"选择左数组元素 " + std::to_wstring(selected_value);
        current_step.merge_left_index++;
        statistics.comparisons++;
        statistics.array_accesses += 2;
    } else {
        selected_value = data_ref[current_step.merge_right_index];
        current_step.temp_array[current_step.merge_target_index] = selected_value;
        current_step_description = L"选择右数组元素 " + std::to_wstring(selected_value);
        current_step.merge_right_index++;
        statistics.comparisons++;
        statistics.array_accesses += 2;
    }
    
    current_step.merge_target_index++;
    demonstrator_ref.playNote(selected_value);
    
    bool is_merge_about_to_complete = (current_step.merge_left_index > current_step.middle_index && current_step.merge_right_index > current_step.right_index);
    
    if (is_merge_about_to_complete) {
        bool is_copy_back_needed = false;
        for (int array_index = 0; array_index < static_cast<int>(current_step.temp_array.size()); array_index++) {
            if (data_ref[current_step.left_index + array_index] != current_step.temp_array[array_index]) {
                is_copy_back_needed = true;
                break;
            }
        }
        
        if (is_copy_back_needed) {
            current_step.is_in_copy_back_phase = true;
            current_step.copy_back_index = 0;
            generateCopyBackSwaps(data_ref, demonstrator_ref);
        }
    }
    
    updateIndicators(demonstrator_ref, data_ref);
    return false;
}

void MergeSortDemonstrator::addDividingIndicators(float bar_width, const sf::FloatRect& content_area) {
    float rectangle_height = content_area.size.y;
    
    sf::Vector2f dividing_rectangle_position(
        content_area.position.x + current_step.left_index * bar_width,
        content_area.position.y
    );
    sf::Vector2f dividing_rectangle_size(
        (current_step.right_index - current_step.left_index + 1) * bar_width,
        rectangle_height
    );
    
    animation_state.indicators.emplace_back(
        IndicatorType::Rectangle,
        dividing_rectangle_position,
        DIVIDING_COLOR
    );
    animation_state.indicators.back().size = dividing_rectangle_size;
    
    if (current_step.right_index - current_step.left_index > 1) {
        sf::Vector2f middle_line_position(
            content_area.position.x + (current_step.middle_index + 1) * bar_width - 1.5f,
            content_area.position.y
        );
        sf::Vector2f middle_line_size(3.0f, rectangle_height);
        
        animation_state.indicators.emplace_back(
            IndicatorType::Rectangle,
            middle_line_position,
            sf::Color::Black
        );
        animation_state.indicators.back().size = middle_line_size;
    }
}

void MergeSortDemonstrator::addMergingIndicators(float bar_width, const sf::FloatRect& content_area, const std::vector<int>& data_ref) {
    float rectangle_height = content_area.size.y;
    
    if (current_step.merge_left_index > current_step.middle_index && current_step.merge_right_index > current_step.right_index) {
        addCompletedMergeIndicator(bar_width, content_area, rectangle_height, data_ref);
    } else {
        addActiveMergeIndicators(bar_width, content_area, rectangle_height);
    }
}

void MergeSortDemonstrator::addCompletedMergeIndicator(float bar_width, const sf::FloatRect& content_area, float rectangle_height, const std::vector<int>& data_ref) {
    if (current_step.is_swap_animation_needed || animation_state.is_animating) {
        sf::Vector2f swap_rectangle_position(
            content_area.position.x + current_step.left_index * bar_width,
            content_area.position.y
        );
        sf::Vector2f swap_rectangle_size(
            (current_step.right_index - current_step.left_index + 1) * bar_width,
            rectangle_height
        );
        
        animation_state.indicators.emplace_back(
            IndicatorType::Rectangle,
            swap_rectangle_position,
            MERGING_RIGHT_COLOR
        );
        animation_state.indicators.back().size = swap_rectangle_size;
    } else {
        bool arrays_match = true;
        if (!current_step.temp_array.empty()) {
            for (int array_index = 0; array_index < static_cast<int>(current_step.temp_array.size()); array_index++) {
                if (data_ref[current_step.left_index + array_index] != current_step.temp_array[array_index]) {
                    arrays_match = false;
                    break;
                }
            }
        }
        
        bool show_green_rectangle = false;
        
        if (arrays_match && !current_step.is_in_copy_back_phase) {
            show_green_rectangle = true;
        } else if (current_step.is_in_copy_back_phase && 
                  current_step.copy_back_index >= static_cast<int>(current_step.temp_array.size())) {
            show_green_rectangle = true;
        }
        
        sf::Vector2f merged_rectangle_position(
            content_area.position.x + current_step.left_index * bar_width,
            content_area.position.y
        );
        sf::Vector2f merged_rectangle_size(
            (current_step.right_index - current_step.left_index + 1) * bar_width,
            rectangle_height
        );
        
        sf::Color rectangle_color = show_green_rectangle ? MERGING_LEFT_COLOR : MERGING_RIGHT_COLOR;
        
        animation_state.indicators.emplace_back(
            IndicatorType::Rectangle,
            merged_rectangle_position,
            rectangle_color
        );
        animation_state.indicators.back().size = merged_rectangle_size;
    }
}

void MergeSortDemonstrator::addActiveMergeIndicators(float bar_width, const sf::FloatRect& content_area, float rectangle_height) {
    if (current_step.left_index <= current_step.middle_index) {
        sf::Vector2f left_rectangle_position(
            content_area.position.x + current_step.left_index * bar_width,
            content_area.position.y
        );
        sf::Vector2f left_rectangle_size(
            (current_step.middle_index - current_step.left_index + 1) * bar_width,
            rectangle_height
        );
        
        animation_state.indicators.emplace_back(
            IndicatorType::Rectangle,
            left_rectangle_position,
            MERGING_RIGHT_COLOR
        );
        animation_state.indicators.back().size = left_rectangle_size;
    }
    
    if (current_step.middle_index + 1 <= current_step.right_index) {
        sf::Vector2f right_rectangle_position(
            content_area.position.x + (current_step.middle_index + 1) * bar_width,
            content_area.position.y
        );
        sf::Vector2f right_rectangle_size(
            (current_step.right_index - current_step.middle_index) * bar_width,
            rectangle_height
        );
        
        animation_state.indicators.emplace_back(
            IndicatorType::Rectangle,
            right_rectangle_position,
            MERGING_RIGHT_COLOR
        );
        animation_state.indicators.back().size = right_rectangle_size;
    }
    
    if (!current_step.is_in_copy_back_phase) {
        addComparisonArrows(bar_width, content_area);
        addTempArrayProgress(bar_width, content_area);
    }
}

void MergeSortDemonstrator::addComparisonArrows(float bar_width, const sf::FloatRect& content_area) {
    if (current_step.merge_left_index <= current_step.middle_index) {
        sf::Vector2f left_arrow_position(
            content_area.position.x + current_step.merge_left_index * bar_width + bar_width/2,
            content_area.position.y - 25.0f
        );
        
        animation_state.indicators.emplace_back(
            IndicatorType::Arrow,
            left_arrow_position,
            COMPARING_LEFT_COLOR
        );
    }
    
    if (current_step.merge_right_index <= current_step.right_index) {
        sf::Vector2f right_arrow_position(
            content_area.position.x + current_step.merge_right_index * bar_width + bar_width/2,
            content_area.position.y - 25.0f
        );
        
        animation_state.indicators.emplace_back(
            IndicatorType::Arrow,
            right_arrow_position,
            COMPARING_RIGHT_COLOR
        );
    }
}

void MergeSortDemonstrator::addTempArrayProgress(float bar_width, const sf::FloatRect& content_area) {
    if (current_step.merge_target_index > 0) {
        sf::Vector2f temp_progress_position(
            content_area.position.x + current_step.left_index * bar_width,
            content_area.position.y - 45.0f
        );
        sf::Vector2f temp_progress_size(
            current_step.merge_target_index * bar_width,
            8.0f
        );
        
        animation_state.indicators.emplace_back(
            IndicatorType::Rectangle,
            temp_progress_position,
            TEMP_ARRAY_COLOR
        );
        animation_state.indicators.back().size = temp_progress_size;
    }
}

void MergeSortDemonstrator::addTriangleIndicator(float bar_width, const sf::FloatRect& content_area) {
    if (!current_step.is_dividing && !current_step.temp_array.empty() && 
        !current_step.is_in_copy_back_phase && !has_merge_finished) {
        
        int target_position = current_step.left_index + current_step.merge_target_index;
        
        if (target_position >= current_step.left_index && target_position <= current_step.right_index &&
            (current_step.merge_left_index <= current_step.middle_index || 
             current_step.merge_right_index <= current_step.right_index)) {
            
            float indicator_x_position = content_area.position.x + target_position * bar_width + bar_width / 2.0f;
            
            sf::Vector2f triangle_position(
                indicator_x_position,
                content_area.position.y + content_area.size.y + 5.0f
            );
            
            VisualIndicator triangle(IndicatorType::Triangle, triangle_position, sf::Color(255, 140, 0));
            animation_state.indicators.push_back(triangle);
        }
    }
}

void MergeSortDemonstrator::addSpecialPairSwapIndicator(float bar_width, const sf::FloatRect& content_area) {
    if (!current_step.is_dividing && current_step.temp_array.empty() && 
        current_step.showing_final_rectangle && current_step.right_index - current_step.left_index == 1) {
        
        float rectangle_height = content_area.size.y;
        
        sf::Vector2f final_rectangle_position(
            content_area.position.x + current_step.left_index * bar_width,
            content_area.position.y
        );
        sf::Vector2f final_rectangle_size(
            (current_step.right_index - current_step.left_index + 1) * bar_width,
            rectangle_height
        );
        
        animation_state.indicators.emplace_back(
            IndicatorType::Rectangle,
            final_rectangle_position,
            MERGING_LEFT_COLOR
        );
        animation_state.indicators.back().size = final_rectangle_size;
    }
}

void MergeSortDemonstrator::resetPairSwapState() {
    current_step.temp_array.clear();
    current_step.is_rectangle_swap_visible = false;
    current_step.is_swap_pause_completed = false;
    current_step.is_swap_complete = false;
    current_step.showing_final_rectangle = false;
}
