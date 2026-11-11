#pragma once
#include <algorithm>
#include <cstddef>
#include <memory>
#include <stdexcept>
#include <vector>
#include "UIElement.h"

class Form {
private:

    std::vector<std::unique_ptr<UIElement>> elements_;
    UIElement* focus_ = nullptr;
    std::vector<std::unique_ptr<UIElement>>::iterator find_element(const UIElement* target) {
        return std::find_if(elements_.begin(), elements_.end(),
            [target](const std::unique_ptr<UIElement>& e) { return e.get() == target; });
    }

public:
    Form() = default;

    Form(const Form&) = delete;
    Form& operator=(const Form&) = delete;

    std::size_t size() const { return elements_.size(); }

    UIElement* focused() const { return focus_; }

    void add(std::unique_ptr<UIElement> element) {
        if (!element) {
            throw std::invalid_argument("Form::add: null element");
        }
        elements_.push_back(std::move(element));
    }


    UIElement* element_at(const Point2D& p) const {
        for (auto it = elements_.rbegin(); it != elements_.rend(); ++it) {
            if ((*it)->contains(p)) {
                return it->get();
            }
        }
        return nullptr;
    }

    bool remove(UIElement* target) {
        auto it = find_element(target);
        if (it == elements_.end()) {
            return false;
        }
        if (focus_ == target) {
            focus_ = nullptr;  
        }
        elements_.erase(it);   
        return true;
    }


    bool set_enabled(UIElement* target, bool value) {
        auto it = find_element(target);
        if (it == elements_.end()) {
            return false;
        }
        (*it)->set_enabled(value);
        if (!value && focus_ == target) {
            focus_ = nullptr;
        }
        return true;
    }
    
    bool click_at(const Point2D& p) {
        UIElement* target = element_at(p);

        if (target == nullptr) {    
            focus_ = nullptr;
            return false;
        }
        if (!target->click()) {  
            return false;
        }
        focus_ = target->can_focus() ? target : nullptr;
        return true;
    }

    bool type_text(const std::string& text) {
        if (focus_ == nullptr) {
            return false;
        }
        focus_->input_text(text);
        return true;
    }


    bool focus_next() {
        const std::size_t n = elements_.size();
        if (n == 0) {
            return false;
        }

        std::size_t start = n - 1;
        if (focus_ != nullptr) {
            start = static_cast<std::size_t>(find_element(focus_) - elements_.begin());
        }
        for (std::size_t step = 1; step <= n; ++step) {
            UIElement* candidate = elements_[(start + step) % n].get();
            if (candidate->is_enabled() && candidate->can_focus()) {
                focus_ = candidate;
                return true;
            }
        }
        return false;
    }

    template <class Visitor>
    void for_each_element(Visitor&& visit) const {
        for (const auto& e : elements_) {
            visit(*e);
        }
    }
};