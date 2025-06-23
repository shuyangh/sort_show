#ifndef BUBBLE_SORT_DEMONSTRATOR_H
#define BUBBLE_SORT_DEMONSTRATOR_H

#include "IDemonstrator.h"
#include <SFML/System/Clock.hpp>

class AlgorithmsDemonstrator;

class BubbleSortDemonstrator : public IDemonstrator {
private:
    SortStatistics statistics;
    AnimationState animation_state;

    int current_comparison_index = 0;
    int bubble_pass = 0;
    bool bubble_finished = false;
    bool swapped_in_current_pass = false;
    int largest_element_index = -1;
    bool is_indicator_update_needed = false;
    std::wstring current_step_description = L"准备开始排序";
    sf::Clock animation_clock;
    float swap_animation_duration = 0.4f;
    
    static const sf::Color NORMAL_COLOR;
    static const sf::Color COMPARING_COLOR;
    static const sf::Color SWAPPING_COLOR;
    static const sf::Color SORTED_COLOR;
    
public:
    BubbleSortDemonstrator();
    
    std::wstring getName() const override;
    void reset() override;
    bool step(std::vector<int>& data_ref, AlgorithmsDemonstrator& demonstrator_ref, float scaled_duration_since_animation_start) override;
    bool isCompleted() const override;
    const SortStatistics& getStatistics() const override;
    const AnimationState& getAnimationState() const override;
    
    sf::Color getElementColor(const std::vector<int>& data_ref, int index) const override;
    
    std::wstring getCurrentStepExplaination() const override;
    
private:
    void performBubbleSortStep(std::vector<int>& data_ref, AlgorithmsDemonstrator& demonstrator_ref);
    void endCurrentPass(std::vector<int>& data_ref, AlgorithmsDemonstrator& demonstrator_ref);
    void updateStatistics(const std::vector<int>& data_ref);
    void updateIndicators(AlgorithmsDemonstrator& demonstrator_ref, const std::vector<int>& data_ref);
    void startSwapAnimation(int left_element_index, int right_element_index, AlgorithmsDemonstrator& demonstrator_ref);
    void updateSwapAnimation(std::vector<int>& data_ref, AlgorithmsDemonstrator& demonstrator_ref, float scaled_duration_since_animation_start);
    void startPause(AlgorithmsDemonstrator& demonstrator_ref);
    void updatePause(float scaled_duration_since_animation_start);
};

#endif
