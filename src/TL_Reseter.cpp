#include "plugin.hpp"
#include "../helpers/widgets/switches.hpp"
#include "../helpers/messages.hpp"

extern Model* modelTL_Seq4;  // declaración externa

// General structure.
struct TL_Reseter : Module {
// --------------------   Visual components namespace  ---------------------------
	enum ParamId {
		PUSH_A_PARAM,
		SIDE_A_PARAM,
		PUSH_B_PARAM,
		SIDE_B_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		IN_A_INPUT,
		IN_B_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		OUTPUTS_LEN
	};
	enum LightId {
		PUSH_A_LED,
		PUSH_B_LED,
		LIGHTS_LEN
	};

// --------------------   Set initial values  ------------------------------------
    float aLightIntensity = 0.f;
    float bLightIntensity = 0.f;
    bool aPressed = false;
    bool bPressed = false;
	bool lastAPressed = false;
	bool lastBPressed = false;

	ReseterMessage leftBuf[2];
	ReseterMessage rightBuf[2];

// --------------------   Config module  -----------------------------------------
	TL_Reseter() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configButton(PUSH_A_PARAM, "Push A");
		configButton(PUSH_B_PARAM, "Push B");
		configSwitch(SIDE_A_PARAM, 0.f, 1.f, 0.f, "Side A", {"Left", "Right"});
		configSwitch(SIDE_B_PARAM, 0.f, 1.f, 0.f, "Side B", {"Left", "Right"});
		configInput(IN_A_INPUT, "Gate A");
		configInput(IN_B_INPUT, "Gate B");
		// Expander: asigna buffers para este módulo
		leftExpander.producerMessage  = &leftBuf[0];
		leftExpander.consumerMessage  = &leftBuf[1];
		rightExpander.producerMessage = &rightBuf[0];
		rightExpander.consumerMessage = &rightBuf[1];
	}

// --------------------   Functions  ---------------------------------------------

    void updateLightsAndTriggers(float deltaTime) {
        // Leer entradas + botones
        aPressed = (params[PUSH_A_PARAM].getValue() > 0.f) 
                || (inputs[IN_A_INPUT].isConnected() && std::fabs(inputs[IN_A_INPUT].getVoltage()) > 1.f);
        bPressed = (params[PUSH_B_PARAM].getValue() > 0.f) 
                || (inputs[IN_B_INPUT].isConnected() && std::fabs(inputs[IN_B_INPUT].getVoltage()) > 1.f);

        // Aumenta intensidades
        aLightIntensity += (aPressed ? (1.f - aLightIntensity) : -5.f * deltaTime);
        bLightIntensity += (bPressed ? (1.f - bLightIntensity) : -5.f * deltaTime);

        // Limita intensidades
        aLightIntensity = clamp(aLightIntensity, 0.f, 1.f);
        bLightIntensity = clamp(bLightIntensity, 0.f, 1.f);

        // Asigna intensidades a LEDs
        lights[PUSH_A_LED].setBrightness(aLightIntensity);
        lights[PUSH_B_LED].setBrightness(bLightIntensity);
    }

	void sendToExpander() {
		bool sendA = (!lastAPressed && aPressed);
		bool sendB = (!lastBPressed && bPressed);
		lastAPressed = aPressed;
		lastBPressed = bPressed;

		bool aToLeft  = params[SIDE_A_PARAM].getValue() == 0 && sendA;
		bool aToRight = params[SIDE_A_PARAM].getValue() == 1 && sendA;
		bool bToLeft  = params[SIDE_B_PARAM].getValue() == 0 && sendB;
		bool bToRight = params[SIDE_B_PARAM].getValue() == 1 && sendB;

		// Si hay vecino, escribe; si no, asegúrate de dejar en false
		auto* l = (ReseterMessage*) leftExpander.producerMessage;
		auto* r = (ReseterMessage*) rightExpander.producerMessage;

		if (leftExpander.module) {
			l->aGate = aToLeft;
			l->bGate = bToLeft;
			leftExpander.requestMessageFlip();
		} else {
			l->aGate = false;
			l->bGate = false;
		}

		if (rightExpander.module) {
			r->aGate = aToRight;
			r->bGate = bToRight;
			rightExpander.requestMessageFlip();
		} else {
			r->aGate = false;
			r->bGate = false;
		}
	}



// --------------------   Main cycle logic  --------------------------------------
	void process(const ProcessArgs& args) override {
        updateLightsAndTriggers(args.sampleTime);
		sendToExpander();
    }
};


// --------------------   Visual components  -------------------------------------
struct TL_ReseterWidget : ModuleWidget {
	TL_ReseterWidget(TL_Reseter* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/TL_Reseter.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		// Push.
		addParam(createLightParamCentered<VCVLightButton<LargeSimpleLight<WhiteLight>>>(mm2px(Vec(7.629, 42.405)), module, TL_Reseter::PUSH_A_PARAM, TL_Reseter::PUSH_A_LED));
		addParam(createLightParamCentered<VCVLightButton<LargeSimpleLight<WhiteLight>>>(mm2px(Vec(7.664, 90.533)), module, TL_Reseter::PUSH_B_PARAM, TL_Reseter::PUSH_B_LED));
		
		// Side.
		addParam(createParamCentered<Switch2Horiz>(mm2px(Vec(7.673, 55.553)), module, TL_Reseter::SIDE_A_PARAM));
		addParam(createParamCentered<Switch2Horiz>(mm2px(Vec(7.676, 103.721)), module, TL_Reseter::SIDE_B_PARAM));
		
		// Input.
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.724, 28.584)), module, TL_Reseter::IN_A_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.7, 76.748)), module, TL_Reseter::IN_B_INPUT));

	}
};


Model* modelTL_Reseter = createModel<TL_Reseter, TL_ReseterWidget>("TL_Reseter");
