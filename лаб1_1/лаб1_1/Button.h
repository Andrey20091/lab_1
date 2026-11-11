#pragma once
#include <string>
#include "UIElement.h"

class Button : public UIElement {
private:
    int click_count_;

protected:
    void on_click() override {
        ++click_count_;
    }
public:
    Button(const Rect& area, bool enabled = true)
        : UIElement(area, enabled), click_count_(0) {}

    int click_count() const { return click_count_; }

    std::string to_string() const override {
        return "Button:" + std::to_string(click_count_);
    }
};
