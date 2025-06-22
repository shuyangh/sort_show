#ifndef QUICK_SORT_DEMONSTRATOR_H
#define QUICK_SORT_DEMONSTRATOR_H

#include "IDemonstrator.h"
#include <SFML/System/Clock.hpp>
#include <stack>

class AlgorithmsDemonstrator;

struct QuickSortStep {
    int subarray_left_index;
    int subarray_right_index;
    int pivot_element_index;
    int partition_boundary_index;
    int current_comparison_index;
    bool is_partitioning;
    bool is_in_pivot_selection_phase;
    bool is_in_recursive_call_phase;
    
    QuickSortStep(int left_index, int right_index) : subarray_left_index(left_index), subarray_right_index(right_index), pivot_element_index(-1), partition_boundary_index(left_index), current_comparison_index(left_index), is_partitioning(false), is_in_pivot_selection_phase(true), is_in_recursive_call_phase(false) {}
};

class QuickSortDemonstrator : public IDemonstrator {
private:
    SortStatistics statistics;
    AnimationState animation_state;
    std::stack<QuickSortStep> quicksort_step_stack;
    QuickSortStep current_sorting_step;
    bool is_initialized = false;
    int left_partition_boundary = -1;
    int right_partition_boundary = -1;
    int pivot_final_sorted_position = -1;
    std::wstring current_step_description = L"准备开始排序";
    
    sf::Clock animation_clock;
    float time_per_step = 1.0f;
    float swap_animation_duration = 0.4f;
    
    static const sf::Color NORMAL_COLOR;
    static const sf::Color PIVOT_COLOR;
    static const sf::Color COMPARING_LEFT_COLOR;
    static const sf::Color COMPARING_RIGHT_COLOR;
    static const sf::Color SWAPPING_COLOR;
    static const sf::Color PARTITIONED_LESS_COLOR;
    static const sf::Color PARTITIONED_GREATER_COLOR;
    static const sf::Color SORTED_COLOR;
    static const sf::Color SUBARRAY_BACKGROUND_COLOR;
    
public:
    QuickSortDemonstrator();
    std::wstring getName() const override;
    void reset() override;
    bool step(std::vector<int>& data_ref, AlgorithmsDemonstrator& demonstrator_ref, float scaled_duration_since_animation_start) override;
    bool isCompleted() const override;
    const SortStatistics& getStatistics() const override;
    const AnimationState& getAnimationState() const override;
    
    sf::Color getElementColor(const std::vector<int>& data_ref, int element_index) const override;
    
    std::wstring getCurrentStepExplaination() const override;
    
private:
    void initializeQuickSort(const std::vector<int>& data_ref);
    void performQuickSortStep(std::vector<int>& data_ref, AlgorithmsDemonstrator& demonstrator_ref);
    void selectPivot(std::vector<int>& data_ref, AlgorithmsDemonstrator& demonstrator_ref);
    void performPartitioning(std::vector<int>& data_ref, AlgorithmsDemonstrator& demonstrator_ref);    void finalizePivotPosition(std::vector<int>& data_ref, AlgorithmsDemonstrator& demonstrator_ref);
    void addRecursiveCalls(std::vector<int>& data_ref);
    void completePartitioning(std::vector<int>& data_ref, AlgorithmsDemonstrator& demonstrator_ref);
    
    void startSwapAnimation(int left_element_index, int right_element_index, AlgorithmsDemonstrator& demonstrator_ref);
    void updateSwapAnimation(std::vector<int>& data_ref, AlgorithmsDemonstrator& demonstrator_ref, float scaled_duration_since_animation_start);
    void startPause(AlgorithmsDemonstrator& demonstrator_ref);
    void updatePause(float scaled_duration_since_animation_start);
    
    void updateStatistics(const std::vector<int>& data_ref);
    void updateIndicators(AlgorithmsDemonstrator& demonstrator_ref, const std::vector<int>& data_ref);
};

#endif
