#pragma once

#include "../plugin.hpp"
#include <app/SvgSwitch.hpp>

struct Switch2Horiz : rack::app::SvgSwitch {
	Switch2Horiz() {
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/custom_components/CKSS_Horizontal_0.svg")));
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/custom_components/CKSS_Horizontal_1.svg")));
	}
};
