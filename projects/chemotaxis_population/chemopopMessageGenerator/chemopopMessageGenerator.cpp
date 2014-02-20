#include "chemopopMessageGenerator.hh"

using namespace Hive;
using namespace ChemoPop;

const int ChemoPopActionIDs::UPDATE_WORLD_SEEN_CELLS_SWIMMING_STATE_ACTION_ID = 0;
const int ChemoPopActionIDs::UPDATE_WORLD_SEEN_CELL_SPEED_ACTION_ID = 1;

const int ChemoPopActionIDs::UPDATE_CELL_SEEN_LIGAND_ACTION_ID = 0;

const int ChemoPopActionIDs::SPECIAL_AGENT_RECEIVE_CELL_POSITIONS_ACTION_ID = 0;
const int ChemoPopActionIDs::SPECIAL_AGENT_ACCUMULATE_CELL_DATA_ACTION_ID = 1;
const int ChemoPopActionIDs::SPECIAL_AGENT_OUTPUT_ANALYSED_CELL_POSITIONS_DATA_ACTION_ID=2;
const int ChemoPopActionIDs::SPECIAL_AGENT_OUTPUT_ANALYSED_LIGAND_DATA_ACTION_ID=3;
const int ChemoPopActionIDs::SPECIAL_AGENT_OUTPUT_ANALYSED_CELL_ACTIVITY_ACTION_ID=4;

WorldNotifyLigandToCellNewMessageGenerator::WorldNotifyLigandToCellNewMessageGenerator(Agent *ag) : MessageGenerator(ag, false) {
	this->db        = source->getDatabase();
	cell_positions  = (TVectorData<TVectorData<double>* >* ) db->getDataItem("cellpositions");
	environments    = (TVectorData<mu::Parser>* ) db->getDataItem("muparser_environments");
	local_index_map = (MapIntIntData*) db->getDataItem("local_index_map");
	t               = (DoubleData*) db->getDataItem("localworldtime");

	// define variables in the mu-parser objects
	for (int i=0; i<environments->size(); i++) {
		environments->at(i).DefineVar("x",&x);
		environments->at(i).DefineVar("y",&y);
		environments->at(i).DefineVar("z",&z);
		environments->at(i).DefineVar("t",&ti);
	}
}

WorldNotifyLigandToCellNewMessageGenerator::~WorldNotifyLigandToCellNewMessageGenerator() {
}

void WorldNotifyLigandToCellNewMessageGenerator::placeMessage(int destID) {
	Message *msg = new Message();
	msg->setAction(ChemoPopActionIDs::UPDATE_CELL_SEEN_LIGAND_ACTION_ID);
	msg->setDestinationID(destID);

	// mathematical description of the environment! there might be more than one!
	int index = local_index_map->returnValueForKey(destID);

	TVectorData<double> *argument = new TVectorData<double> ("argument","tvector_double");
	argument->reserveSize(1);
	argument->at(0) = 0;

	// assuming that the only unspecified parameters in the environment expressions are
	// x,y,z, and t
	for (int i=0; i<environments->size(); i++) {
		// write the correct values in the variables seen by the mu-parser object.
		ti = t->getDouble();
		x  = cell_positions->at(index)->at(0);
		y  = cell_positions->at(index)->at(1);
		z  = cell_positions->at(index)->at(2);
		// evaluate expression
		argument->at(0) += environments->at(i).Eval();
	}
	// add argument to message and sent it off
	msg->setArgument(argument);
	source->placeMessageInOutbox(msg);
}



WorldNotifyLigandInCapAssayToCellMessageGenerator::WorldNotifyLigandInCapAssayToCellMessageGenerator(Agent *ag) : MessageGenerator(ag, false) {
	this->db        = source->getDatabase();
	cell_positions  = (TVectorData<TVectorData<double>* >* ) db->getDataItem("cellpositions");
	local_index_map = (MapIntIntData*) db->getDataItem("local_index_map");
	t               = (DoubleData*) db->getDataItem("localworldtime");
	eqt             = (DoubleData*) db->getDataItem("eqTime");

	// variables for the capillary assay
	Nt[0]=0; Nt[1]=0; Nt[2]=-1;
	Nb[0]=0; Nb[1]=0; Nb[2]=1;
	this->initialCapillaryConc = 1e-2;
	this->diffusionConstant    = 890;
	Zt = 10000;
	Zc = 7000;
	Zb = 0;
	Re = 4000; //2525;
	Rc = 250;
}

WorldNotifyLigandInCapAssayToCellMessageGenerator::~WorldNotifyLigandInCapAssayToCellMessageGenerator() {
}

void WorldNotifyLigandInCapAssayToCellMessageGenerator::placeMessage(int destID) {
	Message *msg = new Message();
	msg->setAction(ChemoPopActionIDs::UPDATE_CELL_SEEN_LIGAND_ACTION_ID);
	msg->setDestinationID(destID);

	// mathematical description of the environment! there might be more than one!
	int index = local_index_map->returnValueForKey(destID);

	// do we have to consider the equilibration time?
	double time = t->getDouble();
	double xPos = cell_positions->at(index)->at(0);
	double yPos = cell_positions->at(index)->at(1);
	double zPos = cell_positions->at(index)->at(2);
	double c    = 0;

	// calculate concentration seen. (take equilibration time into account)
	if (time >= eqt->getDouble()) {
		double r0 = sqrt( xPos*xPos + yPos*yPos);
		if(r0<Rc && zPos>Zc)
			c = initialCapillaryConc;
		else {
			double diffX = xPos;
			double diffY = yPos;
			double diffZ = Zc-zPos;
			double r2 = diffX*diffX+diffY*diffY+diffZ*diffZ;
			double r = sqrt(r2);
			c = ( (initialCapillaryConc*Rc*Rc) /
				  (2.*r*(sqrt(3.14159265359*diffusionConstant*time))) )  *
					 	(exp(-r2/(4.*diffusionConstant*time)))  /
						(1.+((3.*Rc*r) /
						(4.*diffusionConstant*time)));
		}
	}

	TVectorData<double> *argument = new TVectorData<double> ("argument","tvector_double");
	argument->reserveSize(1);
	argument->at(0) = c;
	msg->setArgument(argument);
	source->placeMessageInOutbox(msg);
}


WorldTellSpecialAgentCellPositionsMessageGenerator::WorldTellSpecialAgentCellPositionsMessageGenerator(Agent *ag) : MessageGenerator(ag, false) {
	this->db = source->getDatabase();

	// begin output immediately, not after the first step
	nextOutputTime = ((DoubleData *) ag->getDatabase()->getDataItem("eqTime"))->getDouble();
	//((DoubleData *) ag->getDatabase()->getDataItem("localworldtime"))->getDouble() + ((DoubleData *) ag->getDatabase()->getDataItem("output_interval"))->getDouble();

	t  = (DoubleData *) db->getDataItem("localworldtime");
	ov = (DoubleData *) db->getDataItem("output_interval");
	positions          = (TVectorData<TVectorData<double>* >*) db->getDataItem("cellpositions");
	index_agentid_map  = (MapIntIntData*) db->getDataItem("indexagentidmap");

}

WorldTellSpecialAgentCellPositionsMessageGenerator::~WorldTellSpecialAgentCellPositionsMessageGenerator() { }


void WorldTellSpecialAgentCellPositionsMessageGenerator::placeMessage(int destID) {
	if(t->getDouble()>=nextOutputTime) {
		nextOutputTime += ov->getDouble();

		Message *msg = new Message();
		msg->setAction(ChemoPopActionIDs::SPECIAL_AGENT_RECEIVE_CELL_POSITIONS_ACTION_ID);
		msg->setDestinationID(destID);

		TVectorData<TVectorData<double>* > *argument = new TVectorData<TVectorData<double>* > ("argument", "tvector_doublematrix");
		argument->reserveSize(positions->size()+1);
		argument->at(0) = new TVectorData<double> ("time", "tvector_double");
		argument->at(0)->reserveSize(1);
		argument->at(0)->at(0) = t->getDouble();
		for (int i=0; i<argument->size()-1; i++) {
			argument->at(i+1) = new TVectorData<double> ("line", "tvector_double");
			argument->at(i+1)->reserveSize(4);
			argument->at(i+1)->at(0) = index_agentid_map->returnValueForKey(i);
			argument->at(i+1)->at(1) = positions->at(i)->at(0);
			argument->at(i+1)->at(2) = positions->at(i)->at(1);
			argument->at(i+1)->at(3) = positions->at(i)->at(2);
		}

		msg->setArgument(argument);
		source->placeMessageInOutbox(msg);
	}
}

WorldTellSpecialAgentAnalysedCellPositionsDataMessageGenerator::WorldTellSpecialAgentAnalysedCellPositionsDataMessageGenerator(Agent *ag) : MessageGenerator(ag, false) {
	this->db = source->getDatabase();
	nextOutputTime = ((DoubleData*) db->getDataItem("eqTime"))->getDouble();
	t  = (DoubleData *) db->getDataItem("localworldtime");
	ov = (DoubleData *) db->getDataItem("output_interval");

	positions = (TVectorData<TVectorData<double>* >* ) db->getDataItem("cellpositions");
	index_agentid_map              = (MapIntIntData*) db->getDataItem("indexagentidmap");
}

WorldTellSpecialAgentAnalysedCellPositionsDataMessageGenerator::~WorldTellSpecialAgentAnalysedCellPositionsDataMessageGenerator() { }

void WorldTellSpecialAgentAnalysedCellPositionsDataMessageGenerator::placeMessage(int destID) {
	if(t->getDouble()>=nextOutputTime) {
		// update nextoupttime
		nextOutputTime += ov->getDouble();

		// some local variables needed for analysing the data
		double avtrdist    = 0;
		double sqavtrdist  = 0;
		double stdtrdist   = 0;
		double comx        = 0;
		double com2x       = 0;
		double com3x       = 0;
		double com4x       = 0;
		double comy        = 0;
 		double comz        = 0;
		double tmp         = 0;
		double x           = 0;
		double y           = 0;
		double z           = 0;
		int num_cells      = positions->size();
		// iterate over all cell positions
		for (int i=0; i<num_cells; i++) {
			x           = positions->at(i)->at(0);
			y           = positions->at(i)->at(1);
			z           = positions->at(i)->at(2);
			// calcuate traveled distance of this cell
			tmp         = sqrt(x*x+y*y+z*z);
			// update average traveled distance
			avtrdist   += tmp;
			// for calculating the std
			sqavtrdist += tmp*tmp;
			// for calculating the center of mass
			comx  += x;
			com2x += x*x;
			com3x += x*x*x;
			com4x += x*x*x*x;
			comy  += y;
			comz  += z;
		}
		// get the mean quantities
		comx       /= num_cells;
		com2x      /= num_cells;
		com3x      /= num_cells;
		com4x      /= num_cells;
		comy       /= num_cells;
		comz       /= num_cells;
		avtrdist   /= num_cells;
		sqavtrdist /= num_cells;
		/// construct the message
		Message *msg = new Message();
		msg->setAction(ChemoPopActionIDs::SPECIAL_AGENT_OUTPUT_ANALYSED_CELL_POSITIONS_DATA_ACTION_ID);
		msg->setDestinationID(destID);
		// make a copy of the information ... message with argument is destroyed at the end of a communication step
		TVectorData<double> *argument = new TVectorData<double> ("argument", "tvector_double");
		// reserve a size 6
		argument->reserveSize(9);
		// pos 0: time
		// pos 1: average travelled distance
		// pos 2: average squared travelled distance
		// pos 3: center of mass x
		// pos 4: center of mass y
		// pos 5: center of mass z
		// pos 6: 2nd moment of x
		// pos 7: 3rd moment of x
		// pos 8: 4th moment of x
		argument->at(0) = t->getDouble();
		argument->at(1) = avtrdist;
		argument->at(2) = sqavtrdist;
		argument->at(3) = comx;
		argument->at(4) = comy;
		argument->at(5) = comz;
		argument->at(6) = com2x;
		argument->at(7) = com3x;
		argument->at(8) = com4x;
		// set argument of the message
		msg->setArgument(argument);
		// place message in outbox
		source->placeMessageInOutbox(msg);
	}
}

CellNotifyWorldAboutSwimmingStateMessageGenerator::CellNotifyWorldAboutSwimmingStateMessageGenerator(Agent *ag) : MessageGenerator(ag, true) {
	this->db = source->getDatabase();
	swimmingstate = (IntegerData*) db->getDataItem("swimming_state");

}

CellNotifyWorldAboutSwimmingStateMessageGenerator::~CellNotifyWorldAboutSwimmingStateMessageGenerator() { }

void CellNotifyWorldAboutSwimmingStateMessageGenerator::placeMessage(int destID) {
	Message *msg = new Message();
	msg->setAction(ChemoPopActionIDs::UPDATE_WORLD_SEEN_CELLS_SWIMMING_STATE_ACTION_ID);
	msg->setDestinationID(destID);

	TVectorData<int> *argument =  new TVectorData<int>("argument", "tvector_integer");
	argument->reserveSize(2);
	argument->at(0) = source->getAgentId();
	argument->at(1) = swimmingstate->getInteger();

	msg->setArgument(argument);
	source->placeMessageInOutbox(msg);
}

CellNotifyWorldAboutNewSpeedMessageGenerator::CellNotifyWorldAboutNewSpeedMessageGenerator(Agent *ag) : MessageGenerator(ag, true) {
	this->db = source->getDatabase();
}

CellNotifyWorldAboutNewSpeedMessageGenerator::~CellNotifyWorldAboutNewSpeedMessageGenerator() { }

void CellNotifyWorldAboutNewSpeedMessageGenerator::placeMessage(int destID) {
	Message *msg = new Message();
	msg->setAction(ChemoPopActionIDs::UPDATE_WORLD_SEEN_CELL_SPEED_ACTION_ID);

	// get the argument and data that needs to broadcasted.
	TVectorData<double> *argument = new TVectorData<double>("argument", "tvector_double");
	argument->reserveSize(2);
	argument->at(0) = source->getAgentId();

	msg->setArgument(argument);
	source->placeMessageInOutbox(msg);

	cerr<<"CALLING BAD MESSAGE GENERATOR:: CellNotifyWorldAboutNewSpeedMessageGenerator"<<endl;
	exit(1);
}

CellRevealItselfToSpecialAgentMessageGenerator::CellRevealItselfToSpecialAgentMessageGenerator(Agent *ag) : MessageGenerator(ag, false) {
	this->db = source->getDatabase();
	nextOutputTime = ((DoubleData *) db->getDataItem("eqtime"))->getDouble();

	t  = (DoubleData *) db->getDataItem("celltime");
	ov = (DoubleData *) db->getDataItem("outputinterval");
	cheyp = (TVectorData<double>*) db->getDataItem("CheYp");
	methLevel = (TVectorData<double>*) db->getDataItem("methLevel");
	swimmingstate = (IntegerData* ) db->getDataItem("swimming_state");
	ligand = (TVectorData<double>* ) db->getDataItem("ligands");
	accligand = (TVectorData<double>* ) db->getDataItem("cell_acc_ligand");
	motorStates = (TVectorData<int>*) db->getDataItem("MotorStates");

	dLigand = (DoubleData *) db->getDataItem("dLigand");
	dCheYP = (DoubleData *) db->getDataItem("dCheYP");
	dActivity = (DoubleData *) db->getDataItem("dActivity");
	dMeth = (DoubleData *) db->getDataItem("dMeth");
}

CellRevealItselfToSpecialAgentMessageGenerator::~CellRevealItselfToSpecialAgentMessageGenerator() { }

void CellRevealItselfToSpecialAgentMessageGenerator::placeMessage(int destID) {
	if(t->getDouble()>=(nextOutputTime)) {
		nextOutputTime += ov->getDouble();

		Message *msg = new Message();
		msg->setAction(ChemoPopActionIDs::SPECIAL_AGENT_ACCUMULATE_CELL_DATA_ACTION_ID);
		msg->setDestinationID(destID);

		TVectorData<double> *argument = new TVectorData<double>("argument", "tvector_double");
		// Assumes that there is only one ligand and one accumulated ligand!
		// 0  AgentID
		// 1  Time
		// 2  Swimming State
		// 3  n_motors in CCW
		// 4  n_motors in CW
		// 5  cheYp (of motor zero if uncoordinated)
		// 6  meth  (of motor zero if uncoordinated)
		// 7  ligand
		// 8  accumulated ligand
		// 9  dLigand (OverLastDT)
		// 10 dCheYP (OverLastDT)
		// 11 dActivity (OverLastDT)
		// 12 dMeth (OverLastDT)

		argument->addElementToEnd(source->getAgentId());
		argument->addElementToEnd(t->getDouble());
		argument->addElementToEnd((double)swimmingstate->getInteger());

		int ccw_motors = 0; int cw_motors = 0;
		for(int m=0; m<motorStates->size(); m++) {
			if(motorStates->at(m)==0) ccw_motors++;
			else if(motorStates->at(m)==1) cw_motors++;
			else throw(HiveException("motor state was not zero or one!","CellRevealItselfToSpecialAgentMessageGenerator::placeMessage(int destID)"));
		}

		argument->addElementToEnd((double)ccw_motors);
		argument->addElementToEnd((double)cw_motors);
		argument->addElementToEnd((double)cheyp->at(0));
		argument->addElementToEnd((double)methLevel->at(0));
		argument->addElementToEnd((double)ligand->at(0));
		argument->addElementToEnd((double)accligand->at(0));
		argument->addElementToEnd((double)dLigand->getDouble());
		argument->addElementToEnd((double)dCheYP->getDouble());
		argument->addElementToEnd(dActivity->getDouble());
		argument->addElementToEnd(dMeth->getDouble());

		msg->setArgument(argument);
		source->placeMessageInOutbox(msg);
	}
}

CellTellSpecialAgentAboutSeenLigandMessageGenerator::CellTellSpecialAgentAboutSeenLigandMessageGenerator(Agent *ag) :
		MessageGenerator(ag, false) {
	this->db             = source->getDatabase();
	this->time           = (DoubleData *) this->db->getDataItem("celltime");
	this->ov             = (DoubleData *) this->db->getDataItem("outputinterval");
	this->ligand         = (TVectorData<double> *) this->db->getDataItem("ligands");
	this->accligand      = (TVectorData<double> *) this->db->getDataItem("accumulated_ligands");
	this->nextOutputTime = ((DoubleData *) db->getDataItem("eqtime"))->getDouble();
}

CellTellSpecialAgentAboutSeenLigandMessageGenerator::~CellTellSpecialAgentAboutSeenLigandMessageGenerator() { }

void CellTellSpecialAgentAboutSeenLigandMessageGenerator::placeMessage(int destID) {
	if (time->getDouble() >= nextOutputTime) {
		nextOutputTime += ov->getDouble();

		// prepare message
		Message *msg = new Message();
		msg->setAction(ChemoPopActionIDs::SPECIAL_AGENT_OUTPUT_ANALYSED_LIGAND_DATA_ACTION_ID);
		msg->setDestinationID(destID);

		// get the agent id of the sending agent
		int id = source->getAgentId();

		// set up the argument of the message
		TVectorData<double> *arg  = new TVectorData<double>("argument", "tvector_double");
		arg->reserveSize(3);
		arg->at(0) = id;
		arg->at(1) = time->getDouble();
		arg->at(2) = accligand->at(0);

		// set the message its argument and place it ins the outbox
		msg->setArgument(arg);
		source->placeMessageInOutbox(msg);
	}
}

CellTellSpecialAgentAboutActivityMessageGenerator::CellTellSpecialAgentAboutActivityMessageGenerator(Agent *ag) :
	MessageGenerator(ag, false) {
	this->db        = source->getDatabase();
	this->time      = (DoubleData *) this->db->getDataItem("celltime");
	this->ov        = (DoubleData *) this->db->getDataItem("outputinterval");

	this->activity  = (DoubleData *) this->db->getDataItem("aktivitaet");
	this->cheyp     = (DoubleData *) this->db->getDataItem("cheyphos");
	this->methyl    = (DoubleData *) this->db->getDataItem("methyl");

	this->nextOutputTime = 0;
}

CellTellSpecialAgentAboutActivityMessageGenerator::~CellTellSpecialAgentAboutActivityMessageGenerator() { }

void CellTellSpecialAgentAboutActivityMessageGenerator::placeMessage(int destID) {
	if (time->getDouble() >= nextOutputTime) {
		// update next output time
		nextOutputTime += this->ov->getDouble();

		// prepare the message
		Message *msg = new Message();
		msg->setAction(ChemoPopActionIDs::SPECIAL_AGENT_OUTPUT_ANALYSED_CELL_ACTIVITY_ACTION_ID);
		msg->setDestinationID(destID);

		// add the data to argument
		TVectorData<double> *arg  = new TVectorData<double>("argument", "tvector_double");
		arg->reserveSize(4);
		arg->at(0) = time->getDouble();
		arg->at(1) = activity->getDouble();
		arg->at(2) = cheyp->getDouble();
		arg->at(3) = methyl->getDouble();

		// set the message its argument and place it ins the outbox
		msg->setArgument(arg);
		source->placeMessageInOutbox(msg);
	}
}
