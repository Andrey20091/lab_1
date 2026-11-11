#pragma once
#include <string>
#include "Geometry.h"

class UIElement {
    // Менять enabled может только форма: она же следит, чтобы фокус
    // не остался на выключенном элементе (инвариант формы).
    friend class Form;

private:
    Rect area_;
    bool enabled_;

    void set_enabled(bool value) { enabled_ = value; }

protected:
    virtual void on_click() = 0;

public:
    UIElement(const Rect& area, bool enabled = true)
        : area_(area), enabled_(enabled) {}

    virtual ~UIElement() = default;
    UIElement(const UIElement&) = delete;
    UIElement& operator=(const UIElement&) = delete;
    const Rect& area() const { return area_; }
    bool contains(const Point2D& p) const { return area_.contains(p); }
    bool is_enabled() const { return enabled_; }

    bool click() {
        if (!enabled_) {
            return false;
        }
        on_click();
        return true;
    }
    virtual bool can_focus() const { return false; }
    virtual void input_text(const std::string& text) { (void)text; }
    virtual std::string to_string() const = 0;
};