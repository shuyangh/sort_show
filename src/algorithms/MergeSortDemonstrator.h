#ifndef MERGE_SORT_DEMONSTRATOR_H
#define MERGE_SORT_DEMONSTRATOR_H

#include "IDemonstrator.h"
#include <SFML/System/Clock.hpp>
#include <vector>
#include <queue>

class AlgorithmsDemonstrator;

struct MergeStep {
    int left_index;
    int middle_index;
    int right_index;
    bool is_dividing;
    bool is_merging;
    int merge_left_index;
    int merge_right_index;
    int merge_target_index;
    std::vector<int> temp_array;
    
    bool is_merge_animating = false;
    int current_comparing_left = -1;
    int current_comparing_right = -1;
    int current_target_position = -1;
    bool is_temp_array_visible = false;
    
    bool is_swap_animation_needed = false;
    int swap_source_index = -1;
    int swap_target_index = -1;
    bool is_rectangle_swap_visible = false;
    bool is_swap_pause_completed = false;
    bool is_swap_complete = false;
    bool showing_final_rectangle = false;
    
    bool is_in_copy_back_phase = false;
    int copy_back_index = -1;
    std::vector<std::pair<int, int>> pending_swaps;
    MergeStep(int left, int middle, int right, bool is_dividing_step = true) : left_index(left), middle_index(middle), right_index(right), is_dividing(is_dividing_step), is_merging(false), merge_left_index(left), merge_right_index(middle + 1), merge_target_index(0) {}
};

class MergeSortDemonstrator : public IDemonstrator {
private:
    SortStatistics statistics;
    AnimationState animation_state;
    
    std::queue<MergeStep> merge_queue;
    MergeStep current_step;
    bool is_initialized = false;
    bool has_merge_finished = false;
    std::wstring current_step_description = L"准备开始排序";
    
    sf::Clock animation_clock;
    float time_per_step = 1.0f;
    float merge_animation_duration = 0.8f;
    float swap_animation_duration = 0.4f;
    
    static const sf::Color NORMAL_COLOR;
    static const sf::Color DIVIDING_COLOR;
    static const sf::Color MERGING_LEFT_COLOR;
    static const sf::Color MERGING_RIGHT_COLOR;
    static const sf::Color MERGING_TARGET_COLOR;
    static const sf::Color SORTED_COLOR;
    static const sf::Color COMPARING_LEFT_COLOR;
    static const sf::Color COMPARING_RIGHT_COLOR;
    static const sf::Color TEMP_ARRAY_COLOR;
    static const sf::Color SUBARRAY_BACKGROUND_COLOR;
    
public:
    MergeSortDemonstrator();
    
    std::wstring getName() const override;
    void reset() override;
    bool step(std::vector<int>& data_ref, AlgorithmsDemonstrator& demonstrator_ref, float scaled_duration_since_animation_start) override;
    bool isCompleted() const override;
    const SortStatistics& getStatistics() const override;
    const AnimationState& getAnimationState() const override;
    
    sf::Color getElementColor(const std::vector<int>& data_ref, int index) const override;
    
    std::wstring getCurrentStepExplaination() const override;
    
private:
    void initializeMergeSort(const std::vector<int>& data_ref);
    void generateMergeSteps(const std::vector<int>& data_ref);
    void performMergeSortStep(std::vector<int>& data_ref, AlgorithmsDemonstrator& demonstrator_ref);
    void performDivideStep(std::vector<int>& data_ref, AlgorithmsDemonstrator& demonstrator_ref);
    bool performMergeStepOnce(std::vector<int>& data_ref, AlgorithmsDemonstrator& demonstrator_ref);
    bool handleSwapAnimation(std::vector<int>& data_ref, AlgorithmsDemonstrator& demonstrator_ref);
    bool handlePairSwapCompletion(std::vector<int>& data_ref, AlgorithmsDemonstrator& demonstrator_ref);
    bool performStandardMerge(std::vector<int>& data_ref, AlgorithmsDemonstrator& demonstrator_ref);
    bool handleMergeCompletion(std::vector<int>& data_ref, AlgorithmsDemonstrator& demonstrator_ref);
    bool performSingleMergeStep(std::vector<int>& data_ref, AlgorithmsDemonstrator& demonstrator_ref);
    void generateCopyBackSwaps(std::vector<int>& data_ref, AlgorithmsDemonstrator& demonstrator_ref);
    bool performCopyBackWithSwaps(std::vector<int>& data_ref, AlgorithmsDemonstrator& demonstrator_ref);
    void updateStatistics(const std::vector<int>& data_ref);
    void updateIndicators(AlgorithmsDemonstrator& demonstrator_ref, const std::vector<int>& data_ref);
    void addDividingIndicators(float bar_width, const sf::FloatRect& content_area);
    void addMergingIndicators(float bar_width, const sf::FloatRect& content_area, const std::vector<int>& data_ref);
    void addCompletedMergeIndicator(float bar_width, const sf::FloatRect& content_area, float rectangle_height, const std::vector<int>& data_ref);
    void addActiveMergeIndicators(float bar_width, const sf::FloatRect& content_area, float rectangle_height);
    void addComparisonArrows(float bar_width, const sf::FloatRect& content_area);
    void addTempArrayProgress(float bar_width, const sf::FloatRect& content_area);
    void addTriangleIndicator(float bar_width, const sf::FloatRect& content_area);
    void addSpecialPairSwapIndicator(float bar_width, const sf::FloatRect& content_area);
    void resetPairSwapState();
    void startSwapAnimation(int source_index, int target_index, AlgorithmsDemonstrator& demonstrator_ref);
    void updateSwapAnimation(std::vector<int>& data_ref, AlgorithmsDemonstrator& demonstrator_ref, float scaled_duration_since_animation_start);
    void startPause(AlgorithmsDemonstrator& demonstrator_ref);
    void updatePause(float scaled_duration_since_animation_start);
};

#endif
