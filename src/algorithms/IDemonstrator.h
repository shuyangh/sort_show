#ifndef DEMONSTRATOR_INTERFACE_H
#define DEMONSTRATOR_INTERFACE_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <memory>
#include <functional>
#include <map>

class AlgorithmsDemonstrator;

struct SortStatistics {
    int comparisons = 0; 
    int array_accesses = 0;
    int assignments = 0;
    int passes = 0;
    float sortedness = 0.0f;
    
    void reset() {
        comparisons = 0;
        array_accesses = 0;
        assignments = 0;
        passes = 0;
        sortedness = 0.0f;
    }
};

enum class IndicatorType {
    Triangle,
    Rectangle,
    Arrow,
    Range
};

struct VisualIndicator {
    IndicatorType type;
    sf::Vector2f position;
    sf::Vector2f size;
    sf::Color color;
    int target_index = -1;
    int secondary_index = -1;
    bool visible = true;
    
    VisualIndicator(IndicatorType type, sf::Vector2f position, sf::Color color) : type(type), position(position), color(color) {}
};

struct AnimationState {
    int left_comparison_index = -1;
    int right_comparison_index = -1;

    bool is_animating = false;
    float animation_progress = 0.0f;
    std::vector<int> animating_elements;
    std::vector<sf::Vector2f> start_positions;
    std::vector<sf::Vector2f> target_positions;
    
    int left_element_index = -1;
    int right_element_index = -1;
    
    std::vector<int> special_highlight_indices;
    std::vector<sf::Color> special_highlight_colors;
    
    std::vector<VisualIndicator> indicators;
    
    bool is_pausing = false;
    float pause_timer = 0.0f;
    float pause_duration = 0.2f;
    
    bool completed = false;
    void reset() {
        left_comparison_index = -1;
        right_comparison_index = -1;
        is_animating = false;
        animation_progress = 0.0f;
        animating_elements.clear();
        start_positions.clear();
        target_positions.clear();
        left_element_index = -1;
        right_element_index = -1;
        special_highlight_indices.clear();
        special_highlight_colors.clear();
        indicators.clear();
        is_pausing = false;
        pause_timer = 0.0f;
        completed = false;
    }
};

class IDemonstrator {
public:
    virtual ~IDemonstrator() = default;
    
    virtual std::wstring getName() const = 0;
    
    virtual void reset() = 0;
    
    virtual bool step(std::vector<int>& data_ref, AlgorithmsDemonstrator& demonstrator_ref, float duration_since_animation_start) = 0;
    
    virtual bool isCompleted() const = 0;
    
    virtual const SortStatistics& getStatistics() const = 0;
      virtual const AnimationState& getAnimationState() const = 0;
    
    virtual sf::Color getElementColor(const std::vector<int>& data_ref, int index) const = 0;
    
    virtual std::wstring getCurrentStepExplaination() const = 0;
};

class AlgorithmRegistry {
public:
    static void registerAlgorithm(const std::wstring& name_ref, std::function<std::unique_ptr<IDemonstrator>()> demonstrator_constructor);
    static std::vector<std::wstring> getAvailableAlgorithms();
    static std::unique_ptr<IDemonstrator> createAlgorithm(const std::wstring& name_ref);
    
private:
    static std::map<std::wstring, std::function<std::unique_ptr<IDemonstrator>()>> demonstrator_constructors;
};

#endif