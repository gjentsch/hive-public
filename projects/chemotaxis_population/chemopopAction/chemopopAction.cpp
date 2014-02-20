#include "chemopopAction.hh"

using namespace Hive;
using namespace ChemoPop;

UpdateWorldSeenCellsSwimmingStateAction::UpdateWorldSeenCellsSwimmingStateAction() :
	Action(ChemoPopActionIDs::UPDATE_WORLD_SEEN_CELLS_SWIMMING_STATE_ACTION_ID){}

UpdateWorldSeenCellsSwimmingStateAction::~UpdateWorldSeenCellsSwimmingStateAction() { }

void UpdateWorldSeenCellsSwimmingStateAction::prepare()
{
	if(this->agent->getDatabase()->getDataItem("swimmingstates")) {
	local_index_map = (MapIntIntData* ) this->agent->getDatabase()->getDataItem("local_index_map");
	swimstates   = (TVectorData<int> *) this->agent->getDatabase()->getDataItem("swimmingstates");
	oldstates    = (TVectorData<int> *) this->agent->getDatabase()->getDataItem("lastswimmingstates");
	} else {
		swimstates = 0;
		oldstates = 0;
		local_index_map = 0;
	}
}

void UpdateWorldSeenCellsSwimmingStateAction::fire(Data *d) {
	if (this->agent == NULL)
		throw HiveException("Action was not assigned to an agent", "UpdateWorldSeenCellsSwimmmingStateAction(Data *d)");
	if (d->getType().compare("tvector_integer")!=0)
		throw HiveException("wrong data passed to action", "UpdateWorldSeenCellsSwimmingStateAction::fire(Data *d)");

	TVectorData<int> *argument = (TVectorData<int> *) d;
	int cell_index             = argument->at(0);
	int newstate               = argument->at(1);

	int real_index                 = local_index_map->returnValueForKey(cell_index);
	oldstates->at(real_index)      = swimstates->at(real_index);
	swimstates->at(real_index)     = newstate;
}



UpdateWorldSeenCellSpeedAction::UpdateWorldSeenCellSpeedAction() :
	Action(ChemoPopActionIDs::UPDATE_WORLD_SEEN_CELL_SPEED_ACTION_ID) {}

UpdateWorldSeenCellSpeedAction::~UpdateWorldSeenCellSpeedAction() { }

void UpdateWorldSeenCellSpeedAction::prepare() {

	local_index_map    = (MapIntIntData*) this->agent->getDatabase()->getDataItem("local_index_map");
	cellspeeds   = (TVectorData<double>* ) this->agent->getDatabase()->getDataItem("cellspeeds");
}


void UpdateWorldSeenCellSpeedAction::fire(Data *d) {
	if (this->agent == NULL)
		throw HiveException("Action was not assigned to an agent", "UpdateWorldSeenCellSpeedAction(Data *d)");
	if (d->getType().compare("tvector_double")!=0)
		throw HiveException("wrong data passed to action", "UpdateWorldSeenCellSpeedAction::fire(Data *d)");

	TVectorData<double> *argument = (TVectorData<double>* ) d;
	int cell_position             = (int)argument->at(0);
	double newspeed				  = argument->at(1);

	int real_index                    = local_index_map->returnValueForKey(cell_position);
	cellspeeds->at(real_index)        = newspeed;

}

UpdateCellSeenLigandAction::UpdateCellSeenLigandAction() :
	Action(ChemoPopActionIDs::UPDATE_CELL_SEEN_LIGAND_ACTION_ID) { }

UpdateCellSeenLigandAction::~UpdateCellSeenLigandAction() { }

void UpdateCellSeenLigandAction::prepare() {
	Database *db      = agent->getDatabase();

	// must have ligand concentration
	ligand_conc       = (TVectorData<double>* ) db->getDataItem("ligands");
	dt                = (DoubleData*) db->getDataItem("dt");

	// if there is an essence, we have to keep track of the appetite and yield too
	if(db->existsDataItem("essence")) {
		essence           = (DoubleData*) db->getDataItem("essence");
		kcat              = (DoubleData*) db->getDataItem("kcat");
		Km                = (DoubleData*) db->getDataItem("Km");
		appetite          = (TVectorData<double>* ) db->getDataItem("appetite");
		yield             = (DoubleData*) db->getDataItem("yield");
		effeciency        = (DoubleData*) db->getDataItem("base_effeciency_of_conversion");
		cellaccligand     = 0;
	} else {
		essence = 0; kcat = 0; Km = 0;
		appetite = 0; yield = 0; effeciency = 0;
		cellaccligand = (TVectorData<double>* ) db->getDataItem("accumulated_ligands");
	}
}

void UpdateCellSeenLigandAction::fire(Data *d) {
	if (this->agent == NULL)
		throw HiveException("Action was not assigned to an agent", "UpdateCellSeenLigandAction(Data *d)");
	if (d->getType().compare("tvector_double")!=0)
		throw HiveException("wrong data passed to action", "UpdateCellSeenLigandAction::fire(Data *d)");


	// The cell responds to seen ligand here.  It does this by setting its local ligand concentration
	// and by setting its appetite (which is its rate of uptake, and is based on Km and kcat)

	// first grab the argument
	TVectorData<double> *argument          = (TVectorData<double>* ) d;

	// update the seen ligand value
	for(int i=0; i<ligand_conc->size(); i++) {
		ligand_conc->at(i) = argument->at(i);
		cellaccligand->at(i) += argument->at(i); // *dt->getDouble();
	}

//	cerr << "frank " << argument->size() << " " << argument->at(0) << endl;
	if(essence==0) return;

	cerr << "hans" << endl;

	// determine the uptake rate (the appetite) for ligand zero
	int lig_index = 0;
	appetite->at(lig_index) = effeciency->getDouble()*
			(1.0/yield->getDouble())*essence->getDouble() * (
			(kcat->getDouble()*ligand_conc->at(lig_index)) /
			(Km->getDouble() + ligand_conc->at(lig_index))  );

	if(appetite->at(lig_index)<=0)  appetite->at(lig_index)=0;
	//cerr<<agent->getAgentId()<<" seen:" << ligand_conc->at(0)<<endl;
	//cerr<<agent->getAgentId()<<" want:" << effeciency->getDouble()*
	//		essence->getDouble() * (
	//		(kcat->getDouble()*ligand_conc->at(lig_index)) /
	//		(Km->getDouble() + ligand_conc->at(lig_index))  )<<endl;
	//cerr<<agent->getAgentId()<<" ask :" << appetite->at(lig_index)<<endl;
}


SpecialAgentReceiveCellPositionsAction::SpecialAgentReceiveCellPositionsAction():
	Action(ChemoPopActionIDs::SPECIAL_AGENT_RECEIVE_CELL_POSITIONS_ACTION_ID)  {
	this->isbinary = false;
}

SpecialAgentReceiveCellPositionsAction::~SpecialAgentReceiveCellPositionsAction() {
	this->out->close();
	delete out;
}

void SpecialAgentReceiveCellPositionsAction::prepare() {

}

void SpecialAgentReceiveCellPositionsAction::fire(Data *d) {
	if (this->agent == NULL)
		throw HiveException("Action was not assigned to an agent", "SpecialAgentReceiveCellPositionsAction(Data *d)");
	if (d->getType().compare("tvector_doublematrix")!=0)
		throw HiveException("wrong data passed to action", "SpecialAgentReceiveCellPositionsAction::fire(Data *d)");

	// recieve the data
	TVectorData<TVectorData<double>* > *positions = (TVectorData<TVectorData<double>* >*) d;

	// first element is always the time (in a vector of length one)
	double time = ((TVectorData<double> *)positions->at(0))->at(0);

	// nothing to do with the data other than generating the output.
	if (!this->isbinary) {
		//*this->out << "# output at time t=" << ((SpecialAgent*) this->agent)->getInternalTime()  << endl;
		//*this->out  <<time << " " << 0 << " " << 0	<<" "<< 0 << endl;
		for (int i=1; i<positions->size(); i++)
			*this->out  <<time<<" "<<positions->at(i)->at(0) << " " << positions->at(i)->at(1) << " " << positions->at(i)->at(2)
				<<" "<< positions->at(i)->at(3) << endl;
		//*this->out << endl << endl;
	} else {
		double d;
		for (int i=1; i<positions->size(); i++) {
			d = positions->at(i)->at(0);
			this->out->write(reinterpret_cast<char *> (&d), sizeof(d));
			d = time;
			this->out->write(reinterpret_cast<char *> (&d), sizeof(d));
			d = positions->at(i)->at(1);
			this->out->write(reinterpret_cast<char *> (&d), sizeof(d));
			d = positions->at(i)->at(2);
			this->out->write(reinterpret_cast<char *> (&d), sizeof(d));
			d = positions->at(i)->at(3);
			this->out->write(reinterpret_cast<char *> (&d), sizeof(d));
		}
	}
}

void SpecialAgentReceiveCellPositionsAction::prepareOutputfile(string name, bool bin) {
	if (bin) {
		this->out = new ofstream(name.c_str(),ios_base::binary);
		this->isbinary = true;
	}
	else
		this->out = new ofstream(name.c_str());
}

SpecialAgentAccumulateCellDataAction::SpecialAgentAccumulateCellDataAction() :
	Action(ChemoPopActionIDs::SPECIAL_AGENT_ACCUMULATE_CELL_DATA_ACTION_ID) {
	this->cellcounter = 0;
	this->isbinary    = false;
}

SpecialAgentAccumulateCellDataAction::~SpecialAgentAccumulateCellDataAction() {
	this->out->close();
	delete out;
}

void SpecialAgentAccumulateCellDataAction::prepare() {
	cell_matrix = (TVectorData<TVectorData<double>* >* ) agent->getDatabase()->getDataItem("celldatamatrix");
	numberofcellsData =(IntegerData *)agent->getDatabase()->getDataItem("numberofcells");
}

void SpecialAgentAccumulateCellDataAction::fire(Data *d) {
	if (this->agent == NULL)
		throw HiveException("Action was not assigned to an agent", "SpecialAgentAccumulateCellDataAction(Data *d)");
	if (d->getType().compare("tvector_double")!=0)
		throw HiveException("wrong data passed to action", "SpecialAgentAccumulateCellDataAction::fire(Data *d)");

	// convert the received data
	TVectorData<double> *received_info = (TVectorData<double> *) d;

	// copy the data into the right position
	// needs to be done ...
	int index = received_info->at(0);
	for (int i=1; i<received_info->size(); i++)
		cell_matrix->at(index-1)->at(i-1) = received_info->at(i);

	this->cellcounter += 1;
	// do the output if all cells are added.
	if (cellcounter == (numberofcellsData)->getInteger() ) {
		// do the output
		if (!this->isbinary) {
//			*this->out << "# output at time t=" << ((SpecialAgent*) this->agent)->getInternalTime()  << endl;
//			*this->out << "# output at time t=" << cell_matrix->at(0)->at(0) << endl;
			for (int i=0; i<(numberofcellsData)->getInteger(); i++) {
				*this->out << i+1 ;
				for (int j=0; j<cell_matrix->at(i)->size(); j++)
					*this->out << " " << cell_matrix->at(i)->at(j);
				*this->out << endl;
			}
			this->cellcounter = 0;
			//*this->out << endl << endl;
		} else {
			double d;
			for (int i=0; i<(numberofcellsData)->getInteger(); i++) {

				double id = i+1;
				this->out->write(reinterpret_cast<char *> (&id), sizeof(double));

				for (int j=0; j<cell_matrix->at(i)->size(); j++) {
					double d = cell_matrix->at(i)->at(j);
					this->out->write(reinterpret_cast<char *> (&d), sizeof(double));
				}
			}
			this->cellcounter = 0;
		}
	}

}

void SpecialAgentAccumulateCellDataAction::prepareOutputfile(string filename, bool bin) {
	if (bin) {
		this->out = new ofstream(filename.c_str(),ios_base::binary);
		this->isbinary = true;
	}
	else
		this->out = new ofstream(filename.c_str());
}


SpecialAgentOutputAnalysedCellPositionsDataAction::SpecialAgentOutputAnalysedCellPositionsDataAction():
Action(ChemoPopActionIDs::SPECIAL_AGENT_OUTPUT_ANALYSED_CELL_POSITIONS_DATA_ACTION_ID) {
	this->isbinary = false;
	this->out = NULL;
}


SpecialAgentOutputAnalysedCellPositionsDataAction::~SpecialAgentOutputAnalysedCellPositionsDataAction() {
	this->out->close();
	delete out;
}

void SpecialAgentOutputAnalysedCellPositionsDataAction::prepare() {

}

void SpecialAgentOutputAnalysedCellPositionsDataAction::prepareOutputfile(string filename, bool bin) {
	if (bin) {
		this->out = new ofstream(filename.c_str(), ios_base::binary);
		this->isbinary = true;
	} else
		this->out = new ofstream(filename.c_str());
}

void SpecialAgentOutputAnalysedCellPositionsDataAction::fire(Data *d) {
	if (this->agent == NULL)
		throw HiveException("Action was not assigned to an agent", "SpecialAgentOutputAnalysedCellPositionsDataAction(Data *d)");
	if (d->getType().compare("tvector_double")!=0)
		throw HiveException("wrong data passed to action", "SpecialAgentOutputAnalysedCellPositionsDataAction::fire(Data *d)");

	TVectorData<double> *arg = (TVectorData<double> *) d;
	if (!this->isbinary) {
		for (int i=1; i<=arg->size(); i++)
		{
			*out << arg->at(i-1) << " ";
		}
		*out << endl;
	} else {
		for (int i=0; i<arg->size(); i++) {
			double d = arg->at(i);
			this->out->write(reinterpret_cast<char *> (&d), sizeof(double));
			//cout<<d<<" ";
		}
	}
	//cout<<endl;
}

SpecialAgentOutputAnalysedLigandDataAction::SpecialAgentOutputAnalysedLigandDataAction() :
		Action(ChemoPopActionIDs::SPECIAL_AGENT_OUTPUT_ANALYSED_LIGAND_DATA_ACTION_ID) {
	this->isbinary = false;
	this->out = NULL;
}

SpecialAgentOutputAnalysedLigandDataAction::~SpecialAgentOutputAnalysedLigandDataAction() {
	this->out->close();
	delete out;
}

void SpecialAgentOutputAnalysedLigandDataAction::prepare() {
	// prepare the data needed for running this action
	this->for_mean_acc_ligand                               = new DoubleData("for_mean_acc_ligand",0);
	this->for_mean_sq_acc_ligand                            = new DoubleData("for_mean_sq_acc_ligand", 0);
	this->messages_received_for_analysed_ligand_data_action = new IntegerData("messages_received_for_analysed_ligand_action", 0);

	this->numberofcells = ((IntegerData *) this->agent->getDatabase()->getDataItem("numberofcells"))->getInteger();
}

void SpecialAgentOutputAnalysedLigandDataAction::prepareOutputfile(string filename, bool bin) {
	if (bin) {
		this->out = new ofstream(filename.c_str(), ios_base::binary);
		this->isbinary = true;
	} else
		this->out = new ofstream(filename.c_str());
	// also prepare the action, i.e. connect the local variables with the relevant parts of the database.
	this->prepare();
}

void SpecialAgentOutputAnalysedLigandDataAction::fire(Data *d) {
	if (this->agent == NULL)
		throw HiveException("Action was not assigned to an agent", "SpecialAgentOutputAnalysedLigandDataAction(Data *d)");
	if (d->getType().compare("tvector_double")!=0)
		throw HiveException("wrong data passed to action", "SpecialAgentOutputAnalysedLigandDataAction::fire(Data *d)");

	TVectorData<double> *arg = (TVectorData<double> *) d;
	// the first argument is the agent id of the sending agent, the second argument is the accumulated ligand.
	this->messages_received_for_analysed_ligand_data_action->setInteger(messages_received_for_analysed_ligand_data_action->getInteger()+1);
	this->for_mean_acc_ligand->addToValue(arg->at(2));
	this->for_mean_sq_acc_ligand->addToValue(arg->at(2)*arg->at(2));

	// if the last message has been received do the output
	if (this->messages_received_for_analysed_ligand_data_action->getInteger() == this->numberofcells) {
		// first calculate the mean and the variance
		double mean = this->for_mean_acc_ligand->getDouble()/this->numberofcells;
		double var  = this->for_mean_sq_acc_ligand->getDouble()/this->numberofcells;
		// do the output
		if (!this->isbinary) {
			*out << arg->at(1) << " " << mean << " " << var << endl;
		} else {
			double d = arg->at(1);
			this->out->write(reinterpret_cast<char *> (&d), sizeof(double));
			this->out->write(reinterpret_cast<char *> (&mean), sizeof(double));
			this->out->write(reinterpret_cast<char *> (&var), sizeof(double));
		}
		// reset the values
		this->messages_received_for_analysed_ligand_data_action->setInteger(0);
		this->for_mean_acc_ligand->setDouble(0.0);
		this->for_mean_sq_acc_ligand->setDouble(0.0);
	}
}

SpecialAgentOutputAnalysedCellActivityAction::SpecialAgentOutputAnalysedCellActivityAction() :
		Action(ChemoPopActionIDs::SPECIAL_AGENT_OUTPUT_ANALYSED_CELL_ACTIVITY_ACTION_ID) {
	this->isbinary = false;
	this->out      = NULL;
}

SpecialAgentOutputAnalysedCellActivityAction::~SpecialAgentOutputAnalysedCellActivityAction() {
	delete mean_activity;
	delete mean_cheyp;
	delete mean_methyl;
	delete messages_received_for_analysed_cell_activity_action;
	this->out->close();
}

void SpecialAgentOutputAnalysedCellActivityAction::prepareOutputfile(string filename, bool bin) {
	if (bin) {
		this->out = new ofstream(filename.c_str(), ios_base::binary);
		this->isbinary = true;
	} else
		this->out = new ofstream(filename.c_str());
	// also prepare the action, i.e. connect the local variables with the relevant parts of the database.
	this->prepare();
}

void SpecialAgentOutputAnalysedCellActivityAction::prepare() {
	this->mean_activity = new DoubleData("mean_act",0);
	this->mean_cheyp    = new DoubleData("mean_cheyp",0);
	this->mean_methyl   = new DoubleData("mean_methyl",0);

	this->messages_received_for_analysed_cell_activity_action = new IntegerData("mes_rec", 0);

	this->numberofcells = ((IntegerData *) this->agent->getDatabase()->getDataItem("numberofcells"))->getInteger();
}

void SpecialAgentOutputAnalysedCellActivityAction::fire(Data *d) {
	if (this->agent == NULL)
		throw HiveException("Action was not assigned to an agent", "SpecialAgentOutputAnalysedCellActivityAction(Data *d)");
	if (d->getType().compare("tvector_double")!=0)
		throw HiveException("wrong data passed to action", "SpecialAgentOutputAnalysedCellActivityAction::fire(Data *d)");

	TVectorData<double> *arg = (TVectorData<double> *) d;
	// the first argument is the agent id of the sending agent, the second argument is the accumulated ligand.
	this->messages_received_for_analysed_cell_activity_action->setInteger(messages_received_for_analysed_cell_activity_action->getInteger()+1);
	this->mean_activity->addToValue(arg->at(1));
	this->mean_cheyp->addToValue(arg->at(2));
	this->mean_methyl->addToValue(arg->at(3));

	// if the last message has been received do the output
	if (this->messages_received_for_analysed_cell_activity_action->getInteger() == this->numberofcells) {
		// first calculate the mean and the variance
		double mean_act = this->mean_activity->getDouble()/this->numberofcells;
		double mean_che = this->mean_cheyp->getDouble()/this->numberofcells;
		double mean_met = this->mean_methyl->getDouble()/this->numberofcells;

		// do the output
		if (!this->isbinary) {
			*out << arg->at(0) << " " << mean_act << " " << mean_che << " " << mean_met << endl;
		} else {
			double d = arg->at(0);
			this->out->write(reinterpret_cast<char *> (&d), sizeof(double));
			this->out->write(reinterpret_cast<char *> (&mean_act), sizeof(double));
			this->out->write(reinterpret_cast<char *> (&mean_che), sizeof(double));
			this->out->write(reinterpret_cast<char *> (&mean_met), sizeof(double));
		}
		// reset the values
		this->messages_received_for_analysed_cell_activity_action->setInteger(0);
		this->mean_activity->setDouble(0.0);
		this->mean_cheyp->setDouble(0.0);
		this->mean_methyl->setDouble(0.0);
	}
}

