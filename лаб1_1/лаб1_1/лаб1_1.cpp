#include <iostream>
#include "Geometry.h"
#include "UIElement.h"
#include "Button.h"
#include "CheckBox.h"
#include "TextField.h"
#include "Form.h"
#include "Renderer.h"
void check(bool condition, const char* name) {
    std::cout << (condition ? "[OK]   " : "[FAIL] ") << name << "\n";
}

int main() {
    // Vector2D
    Vector2D v1(1, 2), v2(3, 4);
    check(v1 + v2 == Vector2D(4, 6), "vector +");
    check(v2 - v1 == Vector2D(2, 2), "vector -");
    check(v1 != v2, "vector !=");
    check(v1 == Vector2D(1, 2), "vector ==");
    check(v1.get_x() == 1 && v1.get_y() == 2, "vector getters");

    // Point2D
    Point2D p(2, 3);
    check(p + Vector2D(1, 1) == Point2D(3, 4), "point + vector");
    check(Point2D(5, 5) - p == Vector2D(3, 2), "point - point");
    check(p == Point2D(2, 3), "point == (immutability: p not changed)");
    check(p != Point2D(3, 2), "point !=");
    check(p.get_x() == 2 && p.get_y() == 3, "point getters");

    // Rect
    Rect r(Point2D(2, 1), 10, 3);
    check(r.get_origin() == Point2D(2, 1) && r.get_width() == 10 && r.get_height() == 3,
        "rect getters");
    check(r.left() == 2 && r.top() == 1, "rect left and top");
    check(r.right() == 12 && r.bottom() == 4, "rect right and bottom (first cell outside)");
    check(r.contains(Point2D(r.right() - 1, r.bottom() - 1)), "last cell inside is right-1, bottom-1");
    check(r.contains(Point2D(5, 2)), "inside");
    check(!r.contains(Point2D(20, 2)), "outside right");
    check(!r.contains(Point2D(1, 2)), "outside left");
    check(r.contains(Point2D(2, 1)), "left-top corner included");
    check(!r.contains(Point2D(12, 2)), "right border excluded");
    check(!r.contains(Point2D(5, 4)), "bottom border excluded");

    check(r == Rect(Point2D(2, 1), 10, 3), "rect ==");
    check(r != Rect(Point2D(3, 1), 10, 3), "rect != (other origin)");
    check(r != Rect(Point2D(2, 1), 11, 3), "rect != (other width)");
    check(r != Rect(Point2D(2, 1), 10, 4), "rect != (other height)");

    Rect zero(Point2D(0, 0), 0, 0);
    check(!zero.contains(Point2D(0, 0)), "zero-size rect contains nothing");
    check(zero.left() == zero.right() && zero.top() == zero.bottom(),
        "zero-size rect: right == left, bottom == top");

    bool thrown = false;
    try { Rect bad(Point2D(0, 0), -1, 5); }
    catch (const std::invalid_argument&) { thrown = true; }
    check(thrown, "negative width throws");

    thrown = false;
    try { Rect bad(Point2D(0, 0), 5, -1); }
    catch (const std::invalid_argument&) { thrown = true; }
    check(thrown, "negative height throws");


    // Button
    Button btn(Rect(Point2D(0, 0), 10, 3));
    check(btn.is_enabled(), "button: enabled by default");
    check(btn.click_count() == 0, "button: starts at 0");
    check(btn.click() == true, "button: click returns true when enabled");
    check(btn.click_count() == 1, "button: count is 1 after one click");
    btn.click();
    check(btn.click_count() == 2, "button: count is 2 after two clicks");
    check(btn.to_string() == "Button:2", "button: to_string shows count");
    check(!btn.can_focus(), "button: cannot take focus");
    check(btn.contains(Point2D(5, 1)), "button: point inside");
    check(!btn.contains(Point2D(10, 1)), "button: right border excluded");

    // выключенный элемент создаём через конструктор: set_enabled доступен только форме
    Button offBtn(Rect(Point2D(0, 0), 10, 3), false);
    check(!offBtn.is_enabled(), "button: can be created disabled");
    check(offBtn.click() == false, "button: disabled click returns false");
    check(offBtn.click_count() == 0, "button: disabled does not count");


    // CheckBox
    CheckBox cb(Rect(Point2D(20, 0), 15, 3));
    check(!cb.is_checked(), "checkbox: starts unchecked");
    check(cb.to_string() == "[ ] CheckBox", "checkbox: unchecked text");

    check(cb.click() == true, "checkbox: click returns true when enabled");
    check(cb.is_checked(), "checkbox: checked after first click");
    check(cb.to_string() == "[x] CheckBox", "checkbox: checked text");

    cb.click();
    check(!cb.is_checked(), "checkbox: unchecked after second click");
    check(!cb.can_focus(), "checkbox: cannot take focus");

    CheckBox offCb(Rect(Point2D(20, 0), 15, 3), false);
    check(offCb.click() == false, "checkbox: disabled click returns false");
    check(!offCb.is_checked(), "checkbox: disabled does not toggle");


    // TextField
    TextField tf(Rect(Point2D(0, 5), 20, 3));
    check(tf.text().empty(), "textfield: starts empty");
    check(tf.can_focus(), "textfield: can take focus");
    check(tf.to_string() == "TextField", "textfield: empty shows placeholder");

    tf.input_text("Al");
    tf.input_text("ice");
    check(tf.text() == "Alice", "textfield: text is appended");
    check(tf.to_string() == "Alice", "textfield: to_string shows text");

    tf.input_text("");
    check(tf.text() == "Alice", "textfield: empty input changes nothing");

    check(tf.click() == true, "textfield: click returns true when enabled");
    check(tf.text() == "Alice", "textfield: click does not change text");

    check(tf.contains(Point2D(5, 6)), "textfield: point inside");
    check(!tf.contains(Point2D(20, 6)), "textfield: right border excluded");

    TextField offTf(Rect(Point2D(0, 5), 20, 3), false);
    check(offTf.click() == false, "textfield: disabled click returns false");

    // поведение по умолчанию из UIElement: input_text ничего не делает
    btn.input_text("abc");
    check(btn.to_string() == "Button:2", "button: input_text is ignored");


    // Form: хранение и поиск верхнего
    Form form;
    check(form.size() == 0, "form: starts empty");
    check(form.focused() == nullptr, "form: no focus at start");
    check(form.element_at(Point2D(1, 1)) == nullptr, "form: empty form has nothing under point");

    // указатели сохраняем заранее, потому что после add владеет форма
    auto lower = std::make_unique<Button>(Rect(Point2D(0, 0), 10, 5));
    UIElement* lowerPtr = lower.get();
    form.add(std::move(lower));

    auto upper = std::make_unique<CheckBox>(Rect(Point2D(5, 2), 10, 5));
    UIElement* upperPtr = upper.get();
    form.add(std::move(upper));

    check(form.size() == 2, "form: two elements added");
    check(form.element_at(Point2D(1, 1)) == lowerPtr, "form: only lower under point");
    check(form.element_at(Point2D(6, 3)) == upperPtr, "form: overlap gives the top one");
    check(form.element_at(Point2D(30, 30)) == nullptr, "form: empty place gives nullptr");

    // выключенный верхний всё равно возвращается (Вариант А)
    form.set_enabled(upperPtr, false);
    check(form.element_at(Point2D(6, 3)) == upperPtr, "form: disabled top still covers lower");

    // удаление
    check(form.remove(upperPtr) == true, "form: remove top returns true");
    check(form.size() == 1, "form: size after remove");
    check(form.element_at(Point2D(6, 3)) == lowerPtr, "form: lower is found after removing top");
    check(form.remove(nullptr) == false, "form: remove nullptr returns false");

    Button outsider(Rect(Point2D(0, 0), 10, 3));
    check(form.remove(&outsider) == false, "form: remove foreign element returns false");
    check(form.size() == 1, "form: foreign remove changes nothing");

    bool thrown2 = false;
    try { form.add(nullptr); }
    catch (const std::invalid_argument&) { thrown2 = true; }
    check(thrown2, "form: add(nullptr) throws");
    check(form.size() == 1, "form: failed add changes nothing");

    // Form: включение и выключение (единственный способ менять enabled)
    {
        Form f;
        auto b = std::make_unique<Button>(Rect(Point2D(0, 0), 10, 3), false);
        auto c = std::make_unique<CheckBox>(Rect(Point2D(12, 0), 10, 3), false);
        auto t = std::make_unique<TextField>(Rect(Point2D(0, 4), 10, 3), false);
        Button* bp = b.get(); CheckBox* cp = c.get(); TextField* tp = t.get();
        f.add(std::move(b)); f.add(std::move(c)); f.add(std::move(t));

        check(f.set_enabled(bp, true) && bp->is_enabled(), "enable: form turns button on");
        check(bp->click() && bp->click_count() == 1, "enable: button works after enable");

        check(f.set_enabled(cp, true) && cp->click() && cp->is_checked(),
            "enable: checkbox works after enable");

        check(f.set_enabled(tp, true) && tp->click(), "enable: textfield works after enable");

        check(f.set_enabled(bp, false) && !bp->is_enabled(), "disable: form turns button off");
        check(!bp->click() && bp->click_count() == 1, "disable: button stops counting");

        check(f.set_enabled(bp, false) && !bp->is_enabled(), "disable: twice is harmless");

        Button foreign(Rect(Point2D(0, 0), 10, 3));
        check(f.set_enabled(&foreign, false) == false && foreign.is_enabled(),
            "enable: foreign element is untouched");
        check(f.set_enabled(nullptr, false) == false, "enable: nullptr returns false");
    }

    // Form: клик, фокус, Tab, ввод
    {
        Form f;
        auto b = std::make_unique<Button>(Rect(Point2D(0, 0), 10, 3));
        auto c = std::make_unique<CheckBox>(Rect(Point2D(12, 0), 10, 3));
        auto t1 = std::make_unique<TextField>(Rect(Point2D(0, 4), 10, 3));
        auto t2 = std::make_unique<TextField>(Rect(Point2D(12, 4), 10, 3), false); // disabled
        auto t3 = std::make_unique<TextField>(Rect(Point2D(0, 8), 10, 3));
        Button* bp = b.get();   CheckBox* cp = c.get();
        TextField* t1p = t1.get(); TextField* t2p = t2.get(); TextField* t3p = t3.get();
        f.add(std::move(b)); f.add(std::move(c)); f.add(std::move(t1));
        f.add(std::move(t2)); f.add(std::move(t3));

        // клики
        check(f.click_at(Point2D(1, 1)) == true && bp->click_count() == 1, "click: button counted");
        check(f.focused() == nullptr, "click: button does not take focus");
        f.click_at(Point2D(13, 1));
        check(cp->is_checked(), "click: checkbox toggled");
        check(f.focused() == nullptr, "click: checkbox does not take focus");
        check(f.click_at(Point2D(50, 50)) == false, "click: empty place returns false");

        check(f.click_at(Point2D(1, 5)) == true && f.focused() == t1p, "click: textfield gets focus");
        check(f.type_text("Al") && t1p->text() == "Al", "input: goes to focused field");

        // disabled: ничего не меняется, фокус остаётся
        check(f.click_at(Point2D(13, 5)) == false, "click: disabled returns false");
        check(f.focused() == t1p, "click: disabled does not change focus");
        check(f.type_text("x") && t1p->text() == "Alx" && t2p->text().empty(),
            "input: after disabled click text still goes to old focus");

        // клик по другому полю переносит фокус: фокус всегда только у одного
        f.click_at(Point2D(1, 9));
        check(f.focused() == t3p, "focus: moves to clicked field");
        f.type_text("B");
        check(t3p->text() == "B" && t1p->text() == "Alx", "input: only new focus gets text");

        // клик по кнопке и в пустоту снимает фокус
        f.click_at(Point2D(1, 1));
        check(f.focused() == nullptr, "focus: button click clears focus");
        check(f.type_text("zzz") == false && t1p->text() == "Alx", "input: no focus changes nothing");
        f.click_at(Point2D(1, 5));
        f.click_at(Point2D(50, 50));
        check(f.focused() == nullptr, "focus: empty click clears focus");

        // Tab
        check(f.focus_next() && f.focused() == t1p, "tab: from no focus to first field");
        check(f.focus_next() && f.focused() == t3p, "tab: skips checkbox, button, disabled");
        check(f.focus_next() && f.focused() == t1p, "tab: wraps to first");

        // выключение снимает фокус, а включение его НЕ возвращает
        f.set_enabled(t1p, false);
        check(f.focused() == nullptr, "focus: cleared when focused element disabled");
        f.set_enabled(t1p, true);
        check(f.focused() == nullptr, "focus: re-enable does not return focus");

        // выключение элемента без фокуса фокус не трогает
        f.focus_next();
        check(f.focused() == t1p, "tab: re-enabled field is reachable again");
        f.set_enabled(t3p, false);
        check(f.focused() == t1p, "focus: disabling other element keeps focus");
        f.set_enabled(t3p, true);

        // удаление элемента с фокусом
        f.remove(f.focused());
        check(f.focused() == nullptr, "focus: cleared when focused element removed");

        // единственный подходящий: круг возвращает на него же
        check(f.focus_next() && f.focused() == t3p, "tab: only candidate");
        check(f.focus_next() && f.focused() == t3p, "tab: single candidate stays");

        // удаление элемента без фокуса фокус не трогает
        f.remove(bp);
        check(f.focused() == t3p, "focus: removing other element keeps focus");
    }

    // Form: перекрытие и формы без подходящих элементов
    {
        Form g;
        auto lo = std::make_unique<Button>(Rect(Point2D(0, 0), 10, 5));
        auto hi = std::make_unique<Button>(Rect(Point2D(5, 0), 10, 5));
        Button* lop = lo.get(); Button* hip = hi.get();
        g.add(std::move(lo)); g.add(std::move(hi));
        g.click_at(Point2D(6, 1));
        check(hip->click_count() == 1 && lop->click_count() == 0, "click: only top gets it");

        // disabled сверху не пропускает клик вниз (Вариант А)
        g.set_enabled(hip, false);
        g.click_at(Point2D(6, 1));
        check(hip->click_count() == 1 && lop->click_count() == 0, "click: disabled top covers lower");

        // поле под выключенной кнопкой фокус не получает
        Form h;
        auto field = std::make_unique<TextField>(Rect(Point2D(0, 0), 10, 3));
        auto cover = std::make_unique<Button>(Rect(Point2D(0, 0), 10, 3), false);
        h.add(std::move(field)); h.add(std::move(cover));
        check(h.click_at(Point2D(1, 1)) == false && h.focused() == nullptr,
            "click: field under disabled cover gets no focus");

        Form empty;
        check(empty.focus_next() == false, "tab: empty form");
        check(empty.type_text("a") == false, "input: empty form");
        check(empty.click_at(Point2D(0, 0)) == false, "click: empty form");
        check(g.focus_next() == false && g.focused() == nullptr, "tab: no focusable elements");

        Form onlyDisabled;
        onlyDisabled.add(std::make_unique<TextField>(Rect(Point2D(0, 0), 10, 3), false));
        check(onlyDisabled.focus_next() == false, "tab: only disabled fields");
    }


    // Renderer
    {
        Renderer renderer;

        Form empty2;
        check(renderer.render(empty2).empty(), "render: empty form is empty string");

        Form one;
        one.add(std::make_unique<Button>(Rect(Point2D(0, 0), 10, 3)));
        check(renderer.render(one) ==
            "+--------+\n"
            "|Button:0|\n"
            "+--------+", "render: single button");

        Form focusForm;
        focusForm.add(std::make_unique<TextField>(Rect(Point2D(0, 0), 7, 3)));
        focusForm.click_at(Point2D(1, 1));
        focusForm.type_text("Al");
        check(renderer.render(focusForm) ==
            "#######\n"
            "#Al   #\n"
            "#######", "render: focused field has # border");

        Form overlap;
        overlap.add(std::make_unique<Button>(Rect(Point2D(0, 0), 10, 3)));
        overlap.add(std::make_unique<TextField>(Rect(Point2D(2, 0), 4, 3)));
        check(renderer.render(overlap) ==
            "+-+--+---+\n"
            "|B|Te|n:0|\n"
            "+-+--+---+", "render: later element covers earlier one");

        // элемент не в начале координат: слева и сверху пустое место
        Form shifted;
        shifted.add(std::make_unique<Button>(Rect(Point2D(2, 1), 3, 3)));
        check(renderer.render(shifted) ==
            "     \n"
            "  +-+\n"
            "  |B|\n"
            "  +-+", "render: shifted element, text is cut to width");

        // слишком маленький элемент: только рамка, без текста
        Form tiny;
        tiny.add(std::make_unique<Button>(Rect(Point2D(0, 0), 2, 2)));
        check(renderer.render(tiny) ==
            "++\n"
            "++", "render: tiny element has no text");

        // элемент нулевого размера ничего не рисует
        Form zeroForm;
        zeroForm.add(std::make_unique<Button>(Rect(Point2D(0, 0), 0, 0)));
        check(renderer.render(zeroForm).empty(), "render: zero-size element draws nothing");
    }

    {
        Form demo;
        demo.add(std::make_unique<Button>(Rect(Point2D(0, 0), 16, 5)));
        demo.add(std::make_unique<CheckBox>(Rect(Point2D(20, 0), 18, 5)));
        demo.add(std::make_unique<TextField>(Rect(Point2D(0, 6), 22, 5)));
        demo.add(std::make_unique<TextField>(Rect(Point2D(26, 6), 20, 5)));

        Renderer renderer;
        demo.click_at(Point2D(1, 1));     // нажали кнопку
        demo.click_at(Point2D(21, 1));    // отметили флажок
        demo.click_at(Point2D(1, 7));     // фокус на первое поле
        demo.type_text("Alice");

        std::cout << "\n" << renderer.render(demo) << "\n\n";
        demo.focus_next();                // Tab: фокус на второе поле
        std::cout << renderer.render(demo) << "\n";
        demo.focus_next();
        std::cout << renderer.render(demo) << "\n";
    }
}
