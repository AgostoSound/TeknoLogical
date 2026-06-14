#pragma once

#include "../plugin.hpp"
#include <app/SvgSlider.hpp>

struct SmallHSlider : rack::app::SvgSlider {
	SmallHSlider(float margin = 2.f) {
		auto bg = APP->window->loadSvg(asset::plugin(pluginInstance, "res/custom_components/MasterHSlider-bg.svg"));
		auto hd = APP->window->loadSvg(asset::plugin(pluginInstance, "res/custom_components/MasterHSlider-handle.svg"));
		setBackgroundSvg(bg);
		setHandleSvg(hd);

		box.size = background->box.size;
		if (box.size.isZero())
			box.size = rack::math::Vec(70.4f, 12.8f);

		const float w = box.size.x;
		const float h = box.size.y;
		const float hw = handle ? handle->box.size.x : h * 0.6f;
		setHandlePosCentered(
			rack::math::Vec(margin + hw * 0.1f, h * 0.5f),
			rack::math::Vec(w - margin - hw * 0.1f, h * 0.5f)
		);
		horizontal = true;
	}
};

struct SmallHSliderBool : rack::app::SvgSlider {
	SmallHSliderBool(float margin = 2.f) {
		auto bg = APP->window->loadSvg(asset::plugin(pluginInstance, "res/custom_components/MasterHSliderBool-bg.svg"));
		auto hd = APP->window->loadSvg(asset::plugin(pluginInstance, "res/custom_components/MasterHSliderBool-handle.svg"));
		setBackgroundSvg(bg);
		setHandleSvg(hd);

		box.size = background->box.size;
		if (box.size.isZero())
			box.size = rack::math::Vec(47.f, 12.8f);

		const float w = box.size.x;
		const float h = box.size.y;
		const float hw = handle ? handle->box.size.x : h * 0.6f;
		setHandlePosCentered(
			rack::math::Vec(margin + hw * 0.1f, h * 0.5f),
			rack::math::Vec(w - margin - hw * 0.1f, h * 0.5f)
		);
		horizontal = true;
	}
};

struct MiniVSlider : rack::app::SvgSlider {
	MiniVSlider(float margin = 1.f) {
		auto bg = APP->window->loadSvg(asset::plugin(pluginInstance, "res/custom_components/MiniSliderVertical-bg.svg"));
		auto hd = APP->window->loadSvg(asset::plugin(pluginInstance, "res/custom_components/MiniSliderVertical-handler.svg"));
		setBackgroundSvg(bg);
		setHandleSvg(hd);

		box.size = background->box.size;
		if (box.size.isZero())
			box.size = rack::math::Vec(12.2f, 40.85f);

		const float w = box.size.x;
		const float h = box.size.y;
		const float hh = handle ? handle->box.size.y : w * 0.45f;
		setHandlePosCentered(
			rack::math::Vec(w * 0.5f, h - margin - hh * 0.5f),
			rack::math::Vec(w * 0.5f, margin + hh * 0.5f)
		);
		horizontal = false;
	}
};
