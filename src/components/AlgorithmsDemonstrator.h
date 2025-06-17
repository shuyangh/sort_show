#ifndef ALGORITHMS_DEMONSTRATOR_H
#define ALGORITHMS_DEMONSTRATOR_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <random>
#include <chrono>
#include <optional>
#include "../algorithms/IDemonstrator.h"

enum DataDistributionMode {
    RANDOM,
    ASCENDING,
    DESCENDING,
    NORMAL
};

class AlgorithmsDemonstrator {
private:
    std::vector<int> data;
    std::vector<sf::RectangleShape> bars;
    std::unique_ptr<IDemonstrator> current_algorithm_ptr;
    int data_size;
    DataDistributionMode distribution_mode;
    float animation_speed;
    sf::Clock animation_clock;
    
    sf::Font font;
    std::optional<sf::Text> explaination;

    sf::FloatRect display_area;
    sf::FloatRect content_area;
    bool is_running = false;
    bool is_paused = false;
    
    float margin_top = 20.0f;
    float margin_bottom = 30.0f;
    float margin_left = 10.0f;
    float margin_right = 10.0f;
    
    bool is_audio_enabled = false;
    
public:
    AlgorithmsDemonstrator();
    ~AlgorithmsDemonstrator() = default;

    void setDataSize(int size);
    void setDataDistributionMode(DataDistributionMode mode);
    void setAlgorithm(const std::wstring& algorithm_name);
    void setAnimationSpeed(float speed);
    void setAudioStatus(bool enabled) { is_audio_enabled = enabled; }
    void setMargins(float top, float bottom, float left, float right);
    void setMargin(float margin);
    
    void generateData();
    void resetAlgorithm();
    void start();
    void pause();
    void resume();
    void stop();
    
    void update(const sf::FloatRect& area_ref, float duration_since_animation_start);
    void render(sf::RenderWindow& window_ref);
    
    bool isCompleted() const;
    bool isRunning() const { return is_running && !is_paused; }
    bool isPaused() const { return is_paused; }
    const SortStatistics& getStatistics() const;
    
    std::wstring getCurrentStepExplaination() const;
    sf::Vector2f getElementPosition(int index) const;
    float getBarWidth() const;
    float getBarHeight(int index) const;
    sf::Vector2f getElementTopCenter(int index) const;
    sf::Vector2f getElementBottomCenter(int index) const;
    sf::FloatRect getElementBounds(int index) const;
    const std::vector<int>& getData() const { return data; }
    const sf::FloatRect& getDisplayArea() const { return display_area; }
    const sf::FloatRect& getContentArea() const { return content_area; }
    void playNote(int value) const;
    
private:
    void updateBars(const sf::FloatRect& area_ref);
    void updateExplaination();
    sf::Vector2f calculateAnimatedElementPosition(const sf::Vector2f& start_position_ref, const sf::Vector2f& end_position_ref, float animation_progress);
    
    void renderIndicators(sf::RenderWindow& window_ref, const AnimationState& animation_state_ref);
    void renderTriangleIndicator(sf::RenderWindow& window_ref, const VisualIndicator& indicator_ref);
    void renderRectangleIndicator(sf::RenderWindow& window_ref, const VisualIndicator& indicator_ref);
    void renderArrowIndicator(sf::RenderWindow& window_ref, const VisualIndicator& indicator_ref);
    void renderRangeIndicator(sf::RenderWindow& window_ref, const VisualIndicator& indicator_ref);

    void generateAscendingData();
    void generateDescendingData();
    void generateRandomData();
    void generateNormalData();
};

#endif