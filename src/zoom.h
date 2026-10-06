#ifndef ZOOM_H
#define ZOOM_H

#include <memory>

#include "vector_util.h"
#include "waldo_data.h"
#include <SFML/Graphics.hpp>

class ZoomApp {
public:
    static constexpr float DEFAULT_MAX_WORLD_WIDTH = 800.f;
    static constexpr float DEFAULT_MAX_WORLD_HEIGHT = 600.f;
    static constexpr float ZOOM_FACTOR = 4.0f;

    ZoomApp(float maxWorldWidth = DEFAULT_MAX_WORLD_WIDTH,
            float maxWorldHeight = DEFAULT_MAX_WORLD_HEIGHT);

    void run();

private:
    // loadWorld loads the waldo image into the waldo texture, sets the waldo sprite,
    // and computs the world size based on the largest dimensions which fit into
    // { maxWorldWidth, maxWorldHeight } that maintain the texture's original aspect ratio.
    void loadWorld(float maxWorldWidth, float maxWorldHeight);

    void handleEvent(const sf::Event& event);

    // updateViewAfterResize updates BOTH world views so that it renders into the largest possible
    // centered rectangle with the same aspect ratio as mWorldSize, that still shows the entire
    // world when unzoomed.
    void updateViewAfterResize();

    // updateZoomView sets the center of mWorldViewZoomed such that the zoomed view would have the
    // user's cursor pointing to the same thing as when it is unzoomed.
    void updateZoomView(sf::Vector2i mousePos);

    void render();

    // MEMBER DATA FIELDS
    sf::RenderWindow mWindow;
    sf::View mWorldViewDefault;
    sf::View mWorldViewZoomed;

    // Image data
    sf::Texture mWaldoTexture;
    std::unique_ptr<sf::Sprite> mWaldoSprite;

    // World coordinate bounds
    sf::Vector2f mWorldSize;

    // Zoom state
    bool mIsZooming;
};

#endif  // ZOOM_H
