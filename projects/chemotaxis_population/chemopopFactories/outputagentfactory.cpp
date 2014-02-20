#include "outputagentfactory.hh"

using namespace Hive;
using namespace ChemoPop;

OutputAgentFactory::OutputAgentFactory() {
		this->numbercells = 0;
		this->output_interval = 0;
		this->binaryOutput = false;
		this->world_has_grid = false;
		this->outputSuffix="";
}

OutputAgentFactory::~OutputAgentFactory() { }

Agent* OutputAgentFactory::createAgent() {
	cerr << "# creating output agent ... " << endl;
	SpecialAgent *ibn = new SpecialAgent();
	Database *db = ibn->getDatabase();

	IntegerData *numberofcells = new IntegerData("numberofcells",this->numbercells);
	db->addData(numberofcells->getName(), numberofcells);

	TVectorData<TVectorData<double>* > *cell_information = new TVectorData<TVectorData<double>* > ("celldatamatrix",
																								   "tvector_doublematrix" );
	cell_information->reserveSize(this->numbercells);
	for (int i=0; i<this->numbercells; i++) {
		cell_information->at(i) = new TVectorData<double> ("infoline", "tvector_double");
		/// at this point i use the information that there is only one ligand and one accumulated ligand !
		cell_information->at(i)->reserveSize(12);
	}
	db->addData(cell_information->getName(), cell_information);

	IntegerData *changeInBlindAgentNumberSinceLastOutput = new IntegerData("change_in_blind_agent_number_since_last_output", 0);
	db->addData(changeInBlindAgentNumberSinceLastOutput);

	this->addActionsToAgentsActionSet(ibn);
	cerr << "# done creating output agent ... returning it" << endl;
	return ibn;
}

Agent* OutputAgentFactory::createAgent(Action **as) {
	Agent *ibn = new Agent();
	return ibn;
}

Agent* OutputAgentFactory::duplicateAgent(Agent *ag) {
	Agent *ibn = new Agent();
	return ibn;
}

void OutputAgentFactory::finalise() {
	// i don't think there is anything to be done here ... brilliant!
}

void OutputAgentFactory::setNumberCells(int nc) {
	this->numbercells = nc;
}

void OutputAgentFactory::addActionsToAgentsActionSet(Agent *ag) {
	if (this->numbercells > 0) {
		SpecialAgentReceiveCellPositionsAction *sarcpa = new SpecialAgentReceiveCellPositionsAction();
		ag->addAction(sarcpa);
		sarcpa->setAgent(ag);
		/// binary = true / false
		sarcpa->prepareOutputfile("cell_positions"+this->outputSuffix+".dat", this->binaryOutput);

		SpecialAgentAccumulateCellDataAction *saacda = new SpecialAgentAccumulateCellDataAction();
		ag->addAction(saacda);
		saacda->setAgent(ag);
		saacda->prepareOutputfile("cell_data"+this->outputSuffix+".dat", this->binaryOutput);

		SpecialAgentOutputAnalysedCellPositionsDataAction *saoacpda = new SpecialAgentOutputAnalysedCellPositionsDataAction();
		ag->addAction(saoacpda);
		saoacpda->setAgent(ag);
		saoacpda->prepareOutputfile("analysed_cell_positions"+this->outputSuffix+".dat", this->binaryOutput);

		SpecialAgentOutputAnalysedLigandDataAction *saoalda = new SpecialAgentOutputAnalysedLigandDataAction();
		ag->addAction(saoalda);
		saoalda->setAgent(ag);
		saoalda->prepareOutputfile("analysed_ligand_data"+this->outputSuffix+".dat", this->binaryOutput);

		SpecialAgentOutputAnalysedCellActivityAction *saoacaa = new SpecialAgentOutputAnalysedCellActivityAction();
		ag->addAction(saoacaa);
		saoacaa->setAgent(ag);
		saoacaa->prepareOutputfile("analysed_cell_activity_data"+this->outputSuffix+".dat", this->binaryOutput);
	}
}


// it should be forbidden to add simulators and messagegenerators to an output-agent
void OutputAgentFactory::addSimulatorToAgent(Agent *ag) { }

void OutputAgentFactory::addMessageGeneratorsToAgent(Agent *ag) { }
