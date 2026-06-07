#include "plugin.hpp"


Plugin* pluginInstance;


void init(Plugin* p) {
	pluginInstance = p;

	// Add modules here
	p->addModel(modelTL_tester);
	p->addModel(modelTL_Drum5);
	p->addModel(modelTL_Seq4);
	p->addModel(modelTL_Reseter);
	p->addModel(modelTL_Bass);
	p->addModel(modelTL_Mixes);
	p->addModel(modelTL_Odd5);
	p->addModel(modelTL_Bool);
	p->addModel(modelTL_Pump);
	p->addModel(modelTL_VULR);
	p->addModel(modelTL_LowF);
	p->addModel(modelTL_Shape);
	p->addModel(modelTL_Deck);
	p->addModel(modelTL_Treex);
	
	// Any other plugin initialization may go here.
	// As an alternative, consider lazy-loading assets and lookup tables when your module is created to reduce startup times of Rack.
}
