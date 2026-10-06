#include <iostream>

#include "waldo_data.h"
#include "zoom.h"

ZoomApp::ZoomApp(float maxWorldWidth, float maxWorldHeight) : mIsZooming(false) {
    // Load the Waldo image and determine the world size (mWorldSize)
    loadWorld(maxWorldWidth, maxWorldHeight);
    // Set up the default world view
    mWorldViewDefault.setSize(mWorldSize);
    mWorldViewDefault.setCenter(mWorldSize * 0.5f);
    mWorldViewZoomed.setSize(mWorldSize / ZOOM_FACTOR);
    mWorldViewZoomed.setCenter(mWorldSize * 0.5f);
    // Set window
    mWindow = sf::RenderWindow(
        sf::VideoMode(sf::Vector2u(static_cast<int>(mWorldSize.x), static_cast<int>(mWorldSize.y))),
        "Zoom App");
    // Set initial viewport
    updateViewAfterResize();
    mWindow.setView(mWorldViewDefault);
}

void ZoomApp::run() {
    while (mWindow.isOpen()) {
        // handle inputs
        while (const std::optional<sf::Event> event = mWindow.pollEvent()) {
            handleEvent(*event);
        }
        // draw
        render();
    }
}

// loadWorld loads the waldo image into the waldo texture, sets the waldo sprite,
// and computs the world size based on the largest dimensions which fit into
// { maxWorldWidth, maxWorldHeight } that maintain the texture's original aspect ratio.
void ZoomApp::loadWorld(float maxWorldWidth, float maxWorldHeight) {
    if (!mWaldoTexture.loadFromMemory(EmbeddedImages::waldo_data, EmbeddedImages::waldo_size)) {
        throw std::runtime_error("Failed to load Waldo image from embedded data");
    }
    mWaldoSprite = std::make_unique<sf::Sprite>(mWaldoTexture);
    sf::Vector2u textureSize = mWaldoTexture.getSize();
    // Calculate scale to fit the image within the world bounds while maintaining aspect ratio
    float scaleX = maxWorldWidth / static_cast<float>(textureSize.x);
    float scaleY = maxWorldHeight / static_cast<float>(textureSize.y);
    float scale = std::min(scaleX, scaleY);
    // The size of the world is just the resulting size of the texture after rescaling it
    // to just fit within max world size.
    mWorldSize = {scale * textureSize.x, scale * textureSize.y};
    // Apply the scale transform that makes mWaldoSprite of size mWorldSize:
    mWaldoSprite->setScale(sf::Vector2f(scale, scale));
}

void ZoomApp::handleEvent(const sf::Event& event) {
    // Update ideally should be modularized from input handling, but keeping things simple for
    // this lab:
    if (event.is<sf::Event::Closed>()) {
        mWindow.close();
    } else if (event.is<sf::Event::Resized>()) {
        updateViewAfterResize();
    } else if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {
        if (keyPressed->code == sf::Keyboard::Key::Space) {
            mIsZooming = true;
        }
    } else if (const auto* keyReleased = event.getIf<sf::Event::KeyReleased>()) {
        if (keyReleased->code == sf::Keyboard::Key::Space) {
            mIsZooming = false;
        }
    } else if (const auto* mouseMoved = event.getIf<sf::Event::MouseMoved>()) {
        updateZoomView(mouseMoved->position);
    }
}

// updateViewAfterResize updates BOTH world views so that it renders into the largest possible
// centered rectangle with the same aspect ratio as mWorldSize, that still shows the entire
// world when unzoomed.
void ZoomApp::updateViewAfterResize() {
    // ====== ====== ======
    // TODO:
    //      Enforce original aspect ratio of mWorldSize in the viewports of
    //      mWorldViewDefault and mWorldViewZoomed, when window resizes.
    //      The world view should be expanded/shrinked uniformly to fit in and be centered in the
    //      window. This means that if the window has a larger aspect ratio than the
    //      world views, then vertical black bars are displayed on the left/right. If window has
    //      smaller aspect ratio than world views, horizontal black bars are on the top/bottom.
    // Hint:
    //      Black bars will automatically be drawn for you, when we clear the window with
    //      Color::Black in render()
    // ====== ====== ======

    // Get current aspect ratios
    sf::Vector2u windowSize = mWindow.getSize();
    float winRatio = (float)windowSize.x / (float)windowSize.y;
//    std::cout << "winRatio: " << winRatio << "\n";

    sf::Vector2u textureSize = mWaldoTexture.getSize();
    float texRatio = (float)textureSize.x / (float)textureSize.y;
//    std::cout << "texRatio: " << texRatio << "\n";

    sf::FloatRect newViewport;
    // Determine if scaling is limited on top/bottom or left/right
    if( winRatio > texRatio ) { // Top/bottom limit
        newViewport = sf::FloatRect(
            {(1.f - texRatio/winRatio) / 2.f, 0.f},
            {texRatio/winRatio, 1.f}
        );
    }
    else {  // Left/right limit
        newViewport = sf::FloatRect(
            {0.f, (1.f - winRatio/texRatio) / 2.f},
            {1.f, winRatio/texRatio}
        );
    }

//    std::cout << "newViewport pos: (" << newViewport.position.x << ',' << newViewport.position.y << ")\n";
//    std::cout << "newViewport siz: (" << newViewport.size.x << ',' << newViewport.size.y << ")\n";

    // Calculate newViewport based on aspect ratios of window and of mWorldSize. Then:

    // Apply appropriate view to window
    if(mIsZooming) {
        mWorldViewZoomed.setViewport(newViewport);
        mWindow.setView(mWorldViewZoomed);
    }
    else {
        mWorldViewDefault.setViewport(newViewport);
        mWindow.setView(mWorldViewDefault);
    }
}

// updateZoomView sets the center of mWorldViewZoomed such that the zoomed view would have the
// user's cursor pointing to the same thing as when it is unzoomed.
void ZoomApp::updateZoomView(sf::Vector2i mousePos) {
    // TODO: Implement this method. High-level outline/hints given.
    // (1) Get position of mouse relative to viewport's top-left in window pixel units.
    //     Remember that in SFML, viewports' position and size are both given as PERCENTAGES
    //     of the window size.
    // (2) Get position of mouse relative to viewport's top-left as percentage of original
    //     viewport.
    // (3) Then use that to get pos of mouse relative to world top-left in world units.
    // (4) Derive the new viewport center (in world units)
    //     which keeps the mouse position pointing at the same thing in
    //     original image but now within a world-space rectangle of size
    //     mWorldSize / ZOOM_FACTOR.
    //  HINT: Determine the steps to go from the desired center to the top-left of this new
    //        world-space rectangle, then from this top-left to the thing you're pointing at,
    //        using our above computed vars. Then solve the equation for the desired center.
}

void ZoomApp::render() {
    mWindow.clear(sf::Color::Black);
    if (!mIsZooming) {
        // TODO: If the user is not zooming in, use the default world view.
    } else {
        // TODO: If the user is zooming in, use the zoomed world view.
    }
    mWindow.draw(*mWaldoSprite);
    mWindow.display();
}

int main() {
    try {
        ZoomApp app;
        app.run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return -1;
    }
    return 0;
}
