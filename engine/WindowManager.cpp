#include "WindowManager.h"

namespace ECSEngine {

WindowManager::WindowManager(unsigned int width, unsigned int height, const std::string& title)
    : mWindow(std::make_shared<sf::RenderWindow>(sf::VideoMode({width, height}), title)) {
    mWindow->setFramerateLimit(60);

    mWorldUnitsPerPixel = 1;
    mWorldCenter = Point2D(static_cast<int>(width / 2), static_cast<int>(height / 2));
}

std::shared_ptr<sf::RenderWindow> WindowManager::GetWindow() const { return mWindow; }

Rect WindowManager::GetWindowBounds() const {
    const Point2D tl = Point2D(0.f, 0.f);
    return Rect{tl, static_cast<float>(mWindow->getSize().x), static_cast<float>(mWindow->getSize().y)};
}

void WindowManager::DebugDrawRect(const Rect& r, int red, int green, int blue, int alpha) {}

void WindowManager::DebugFrameRect(const Rect& r, int red, int green, int blue, int alpha) {}

Rect WindowManager::WindowToWorld(const Rect& rect) const { return Rect(
    WindowToWorld(rect.topLeft), WindowToWorld(rect.width), WindowToWorld(rect.height)); }

Point2D WindowManager::WindowToWorld(const Point2D& pt) const {
    return {
        mWorldCenter.x + (pt.x - mWindow->getSize().x / 2.f) * mWorldUnitsPerPixel,
        mWorldCenter.y + (pt.y - mWindow->getSize().y / 2.f) * mWorldUnitsPerPixel};
}

Rect WindowManager::WorldToWindow(const Rect& rect) const { return Rect(
    WorldToWindow(rect.topLeft), WorldToWindow(rect.width), WorldToWindow(rect.height)); }

Point2D WindowManager::WorldToWindow(const Point2D& pt) const {
    return {
        mWindow->getSize().x / 2.f + (pt.x - mWorldCenter.x) / mWorldUnitsPerPixel,
        mWindow->getSize().y / 2.f + (pt.y - mWorldCenter.y) / mWorldUnitsPerPixel};
}

void WindowManager::SetCamera(const Point2D& worldPt, const Point2D& screenPt) {
    mWorldCenter.x = worldPt.x - (screenPt.x - mWindow->getSize().x / 2.f) * mWorldUnitsPerPixel;
    mWorldCenter.y = worldPt.y - (screenPt.y - mWindow->getSize().y / 2.f) * mWorldUnitsPerPixel;
}

Point2D WindowManager::GetWorldCenter() const { return mWorldCenter; }

float WindowManager::GetWorldScale() const { return mWorldUnitsPerPixel; }

void WindowManager::SetWorldCenter(const Point2D& worldCenter) {
    mWorldCenter = worldCenter;
}

void WindowManager::SetWorldScale(float worldUnitsPerPixel) {
    mWorldUnitsPerPixel = worldUnitsPerPixel;
}

void WindowManager::SetWorldRect(const Rect& worldCameraRect) {
    mWorldCenter = Point2D(worldCameraRect.topLeft.x + worldCameraRect.width / 2, worldCameraRect.topLeft.y + worldCameraRect.height / 2);
    mWorldUnitsPerPixel = std::max(worldCameraRect.width / mWindow->getSize().x, worldCameraRect.height / mWindow->getSize().y);
}

float WindowManager::WindowToWorld(float x) const {
    return x * mWorldUnitsPerPixel;
}

float WindowManager::WorldToWindow(float x) const {
    return x / mWorldUnitsPerPixel;
}

}  // namespace ECSEngine
