#pragma once
#include <app/SvgSwitch.hpp>
#include <plugin.hpp>

struct Switch2Horiz : rack::app::SvgSwitch {
	Switch2Horiz() {
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/custom_components/CKSS_Horizontal_0.svg")));
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/custom_components/CKSS_Horizontal_1.svg")));
	}
};
