#pragma once
#include <algorithm>
#include <string>
#include <vector>
#include "Form.h"

class Renderer {
public:
    std::string render(const Form& form) const {
        int width = 0, height = 0;
        form.for_each_element([&](const UIElement& e) {
            width = std::max(width, e.area().right());
            height = std::max(height, e.area().bottom());
            });

        std::vector<std::string> canvas(height, std::string(width, ' '));

        const UIElement* focus = form.focused();
        form.for_each_element([&](const UIElement& e) {
            draw(canvas, e, &e == focus);
            });


        std::string result;
        for (std::size_t i = 0; i < canvas.size(); ++i) {
            if (i > 0) result += '\n';
            result += canvas[i];
        }
        return result;
    }

private:
    void put(std::vector<std::string>& canvas, int x, int y, char ch) const {
        if (y < 0 || y >= static_cast<int>(canvas.size())) return;
        if (x < 0 || x >= static_cast<int>(canvas[y].size())) return;
        canvas[y][x] = ch;
    }

    void draw(std::vector<std::string>& canvas, const UIElement& e, bool focused) const {
        const Rect& r = e.area();
        const int x0 = r.left();
        const int y0 = r.top();
        const int w = r.get_width();
        const int h = r.get_height();

        for (int dy = 0; dy < h; ++dy) {
            for (int dx = 0; dx < w; ++dx) {
                const bool vertEdge = (dx == 0 || dx == w - 1);
                const bool horEdge = (dy == 0 || dy == h - 1);
                char ch = ' ';
                if (vertEdge || horEdge) {
                    if (focused)                 ch = '#';
                    else if (vertEdge && horEdge) ch = '+';
                    else if (horEdge)             ch = '-';
                    else                          ch = '|';
                }
                put(canvas, x0 + dx, y0 + dy, ch);
            }
        }

        
        if (w > 2 && h > 2) {
            const std::string text = e.to_string();
            const int row = y0 + h / 2;
            for (int i = 0; i < w - 2 && i < static_cast<int>(text.size()); ++i) {
                put(canvas, x0 + 1 + i, row, text[i]);
            }
        }
    }
};