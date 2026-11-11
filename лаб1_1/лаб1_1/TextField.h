#pragma once
#include <string>
#include "UIElement.h"

class TextField : public UIElement {
private:
    std::string text_; 

protected:
    void on_click() override {}

public:
    TextField(const Rect& area, bool enabled = true)
        : UIElement(area, enabled), text_() {}

    const std::string& text() const { return text_; }

    bool can_focus() const override { return true; }

    void input_text(const std::string& text) override {
        text_ += text;
    }

    std::string to_string() const override {
        return text_.empty() ? "TextField" : text_;
    }
};