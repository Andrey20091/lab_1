#pragma once
#include <string>
#include "UIElement.h"

class CheckBox : public UIElement {
private:
    bool checked_;

protected:
    void on_click() override {
        checked_ = !checked_;
    }

public:
    CheckBox(const Rect& area, bool enabled = true)
        : UIElement(area, enabled), checked_(false) {}

    bool is_checked() const { return checked_; }

    std::string to_string() const override {
        return checked_ ? "[x] CheckBox" : "[ ] CheckBox";
    }
};