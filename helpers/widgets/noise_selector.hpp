#pragma once
#include <functional>
#include "../dsp_utils.hpp"

struct XYPadWidget : Widget {
    widget::SvgWidget* border = nullptr;
    widget::SvgWidget* handle = nullptr;
    std::function<void(float, float)> onChange;
    std::function<void(float&, float&)> syncSource;

    Vec minHandlePos;
    Vec maxHandlePos;
    Vec fallbackSize;
    float x01 = 0.5f;
    float y01 = 0.5f;
    float handlePaddingRatio = 0.06f;
    float handleScale = 0.28f;

    XYPadWidget(std::shared_ptr<window::Svg> borderSvg,
        std::shared_ptr<window::Svg> handleSvg,
        Vec fallbackSize = Vec(54.f, 54.f)) {

        this->fallbackSize = fallbackSize;

        border = new widget::SvgWidget();
        border->setSvg(borderSvg);
        addChild(border);

        handle = new widget::SvgWidget();
        handle->setSvg(handleSvg);
        addChild(handle);

        box.size = border->box.size;
        if (box.size.isZero())
            box.size = fallbackSize;

        border->box.pos = Vec();
        border->box.size = box.size;

        if (handle->box.size.isZero())
            handle->box.size = box.size.mult(handleScale);

        updateBounds();
        updateHandleFromNormalized();
    }

    void updateBounds() {
        float handlePadding = std::min(box.size.x, box.size.y) * handlePaddingRatio;
        minHandlePos = Vec(handlePadding, handlePadding);
        maxHandlePos = box.size.minus(handle->box.size).minus(Vec(handlePadding, handlePadding));

        if (maxHandlePos.x < minHandlePos.x)
            maxHandlePos.x = minHandlePos.x;
        if (maxHandlePos.y < minHandlePos.y)
            maxHandlePos.y = minHandlePos.y;
    }

    void setNormalizedPosition(float x, float y) {
        x01 = clamp(x, 0.f, 1.f);
        y01 = clamp(y, 0.f, 1.f);
        updateHandleFromNormalized();

        if (onChange)
            onChange(x01, y01);
    }

    void updateHandleFromNormalized() {
        handle->box.pos.x = rescale(x01, 0.f, 1.f, minHandlePos.x, maxHandlePos.x);
        handle->box.pos.y = rescale(y01, 0.f, 1.f, minHandlePos.y, maxHandlePos.y);
    }

    void updateNormalizedFromHandle() {
        float x = 0.f;
        float y = 0.f;

        if (maxHandlePos.x > minHandlePos.x)
            x = rescale(handle->box.pos.x, minHandlePos.x, maxHandlePos.x, 0.f, 1.f);
        if (maxHandlePos.y > minHandlePos.y)
            y = rescale(handle->box.pos.y, minHandlePos.y, maxHandlePos.y, 0.f, 1.f);

        x01 = clamp(x, 0.f, 1.f);
        y01 = clamp(y, 0.f, 1.f);

        if (onChange)
            onChange(x01, y01);
    }

    void setPositionFromLocal(Vec localPos) {
        Vec handlePos = localPos.minus(handle->box.size.mult(0.5f));
        handlePos.x = clamp(handlePos.x, minHandlePos.x, maxHandlePos.x);
        handlePos.y = clamp(handlePos.y, minHandlePos.y, maxHandlePos.y);
        handle->box.pos = handlePos;
        updateNormalizedFromHandle();
    }

    void onButton(const event::Button& e) override {
        if (e.button != GLFW_MOUSE_BUTTON_LEFT) {
            Widget::onButton(e);
            return;
        }

        if (e.action == GLFW_PRESS) {
            setPositionFromLocal(e.pos);
            e.consume(this);
            return;
        }

        Widget::onButton(e);
    }

    void onDragMove(const event::DragMove& e) override {
        handle->box.pos = handle->box.pos.plus(e.mouseDelta);
        handle->box.pos.x = clamp(handle->box.pos.x, minHandlePos.x, maxHandlePos.x);
        handle->box.pos.y = clamp(handle->box.pos.y, minHandlePos.y, maxHandlePos.y);
        updateNormalizedFromHandle();
        e.consume(this);
    }

    void step() override {
        if (syncSource) {
            float syncedX = x01;
            float syncedY = y01;
            syncSource(syncedX, syncedY);

            if (DSPUtils::changedEnough(x01, syncedX) || DSPUtils::changedEnough(y01, syncedY)) {
                x01 = clamp(syncedX, 0.f, 1.f);
                y01 = clamp(syncedY, 0.f, 1.f);
                updateHandleFromNormalized();
            }
        }

        Widget::step();
    }
};
