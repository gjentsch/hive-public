#include "worldagentfactory.hh"

using namespace ChemoPop;

WorldAgentFactory::WorldAgentFactory() {
	cerr << "# hello, i am the WorldAgentFactory " << endl;
}

WorldAgentFactory::~WorldAgentFactory() { }

Agent* WorldAgentFactory::createAgent() {
	cerr << "# creating worldagent" << endl;
	// create world agent and get his database
	Agent *world = new Agent();

	// using the WorldParameterInitialiser to fill the database of the world agent
	this->wpi->setNextAgentParameters(world);

	/// now we add the remaining things to the world agent
	/// add simulators to agent
	this->addSimulatorToAgent(world);
	cerr << "# added simulators to the world agent" << endl;

	/// add actions to agents action set
	this->addActionsToAgentsActionSet(world);
	cerr << "# added actions to the world agent" << endl;

	/// add message generators to action set
	this->addMessageGeneratorsToAgent(world);
	cerr << "# added message generators to the world agent" << endl;

	cerr<<"# done creating world"<<endl;
	return world;
}


Agent* WorldAgentFactory::createAgent(Action **as) {
	// should throw an exception, not implemented
	throw HiveException("not implemented", "Agent* WorldAgentFactoryII:createAgent(Action **as)");

	Agent *frank = new Agent();
	return frank;
}


Agent* WorldAgentFactory::duplicateAgent(Agent *ag) {
	// should throw an exception, not implemented
	throw HiveException("not implemented", "Agent* WorldAgentFactoryII::duplicateAgent(Agent *ag)");

	Agent *frank = new Agent();
	return frank;
}


void WorldAgentFactory::finalise() { }


void WorldAgentFactory::addSimulatorToAgent(Agent* ag) {
	cerr << "# adding chemotaxis movement simulator to the world" << endl;
	if (!this->wpi->isCapillaryAssay()) {
		// add chemotaxis movement simulator to the world that deals with math-environments
		ChemotaxisMovementSimulatorInWorld *cmsiw = new ChemotaxisMovementSimulatorInWorld();
		cmsiw->setGenerateRunTumbleOutput(this->wpi->generateRunTumble());
		cmsiw->setWorldAgent(ag);
		ag->addSimulator(cmsiw);
	} else {
		// add chemotaxis movement simulator to the world that deals with capillary assay
		ChemotaxisMovementSimulatorInCapAssay *cmsica = new ChemotaxisMovementSimulatorInCapAssay();
		cmsica->setGenerateRunTumbleOutput(this->wpi->generateRunTumble());
		cmsica->setWorldAgent(ag);
		ag->addSimulator(cmsica);
	}

	// add a celltimekeeper to the agent
	TimeKeeper *ctk = new TimeKeeper(ag,"localworldtime");
	ag->addSimulator(ctk);
}


void WorldAgentFactory::addMessageGeneratorsToAgent(Agent *ag) {
	// adding message generators to agent.
	if (!this->wpi->isCapillaryAssay()) {
		WorldNotifyLigandToCellNewMessageGenerator *wnltcmg = new WorldNotifyLigandToCellNewMessageGenerator(ag);
		ag->addMessageGenerator(wnltcmg);
	} else {
		WorldNotifyLigandInCapAssayToCellMessageGenerator *wnlicatcmg = new WorldNotifyLigandInCapAssayToCellMessageGenerator(ag);
		ag->addMessageGenerator(wnlicatcmg);
	}

	if (this->wpi->generateCellPosition()) {
		WorldTellSpecialAgentCellPositionsMessageGenerator *wtsacpmg = new WorldTellSpecialAgentCellPositionsMessageGenerator(ag);
		ag->addMessageGenerator(wtsacpmg);
	}

	WorldTellSpecialAgentAnalysedCellPositionsDataMessageGenerator *wtsaacpdm = new WorldTellSpecialAgentAnalysedCellPositionsDataMessageGenerator(ag);
	ag->addMessageGenerator(wtsaacpdm);
}


void WorldAgentFactory::addActionsToAgentsActionSet(Agent *ag) {
	UpdateWorldSeenCellsSwimmingStateAction *uwscssa = new UpdateWorldSeenCellsSwimmingStateAction();
	uwscssa->setAgent(ag);
	ag->addAction(uwscssa);
	cerr << "# added UpdateWorldSeenCellsSwimmingStateAction" << endl;

	/// no idea for what we need this action. do we change the speed of the cells
	/// during the simulation?
	UpdateWorldSeenCellSpeedAction *uwscsa = new UpdateWorldSeenCellSpeedAction();
	uwscsa->setAgent(ag);
	ag->addAction(uwscsa);
	cerr << "# added UpdateWorldSeenCellSpeedAction" << endl;

}
