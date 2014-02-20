/*
 * chemopopMessageGenerator.hh
 *
 *  Created on: Apr 16, 2010
 *      Author: jentsch
 */

#ifndef CHEMOPOPMESSAGEGENERATOR_HH_
#define CHEMOPOPMESSAGEGENERATOR_HH_

#include <cstdlib>

#include "../../../core/agent/messageGenerator/messagegenerator.hh"
#include "../../../core/agent/data/template/tdata.hh"
#include "../../../core/agent/data/primitive/primitiveData.hh"
#include "../../../core/agent/data/mapdata/mapintintdata.hh"
#include "../../../core/agent/agentFactory/agent_factory.hh"
#include "../../../core/util/function/muParser/muParser.h"
#include "../../../core/util/function/muParser/muParserBase.h"
#include "../../../core/util/function/muParser/muParserBytecode.h"
#include "../../../core/util/function/muParser/muParserCallback.h"
#include "../../../core/util/function/muParser/muParserDef.h"
#include "../../../core/util/function/muParser/muParserError.h"
#include "../../../core/util/function/muParser/muParserFixes.h"
#include "../../../core/util/function/muParser/muParserStack.h"
#include "../../../core/util/function/muParser/muParserToken.h"
#include "../../../core/util/function/muParser/muParserTokenReader.h"

using namespace std;
using namespace Hive;


namespace ChemoPop {
	class ChemoPopActionIDs {
	public:
		// world actions
		static const int UPDATE_WORLD_SEEN_CELLS_SWIMMING_STATE_ACTION_ID;
		static const int UPDATE_WORLD_SEEN_CELL_SPEED_ACTION_ID;

		// cell actions
		static const int UPDATE_CELL_SEEN_LIGAND_ACTION_ID;

		// special agent action
		static const int SPECIAL_AGENT_RECEIVE_CELL_POSITIONS_ACTION_ID;
		static const int SPECIAL_AGENT_ACCUMULATE_CELL_DATA_ACTION_ID;
		static const int SPECIAL_AGENT_OUTPUT_ANALYSED_CELL_POSITIONS_DATA_ACTION_ID;
		static const int SPECIAL_AGENT_OUTPUT_ANALYSED_LIGAND_DATA_ACTION_ID;
		static const int SPECIAL_AGENT_OUTPUT_ANALYSED_CELL_ACTIVITY_ACTION_ID;
	};

	// for sending ligand information to cells, if environment is given as
	// as arbitrary mathematical objects that are specified using muParser objects.
	class WorldNotifyLigandToCellNewMessageGenerator : public MessageGenerator {
	public:
		WorldNotifyLigandToCellNewMessageGenerator(Agent *Ag);
		~WorldNotifyLigandToCellNewMessageGenerator();
		void placeMessage(int destID);

	private:
		Database *db;
		TVectorData<TVectorData<double>* > *cell_positions;
		TVectorData<mu::Parser>            *environments;
		MapIntIntData                      *local_index_map;
		DoubleData                         *t;

		double ti,x,y,z;
	};

	// for sending ligand information to cells, if environment is a capillary assay
	class WorldNotifyLigandInCapAssayToCellMessageGenerator : public MessageGenerator {
	public:
		WorldNotifyLigandInCapAssayToCellMessageGenerator(Agent *Ag);
		~WorldNotifyLigandInCapAssayToCellMessageGenerator();
		void placeMessage(int destID);

	private:
		Database *db;
		TVectorData<TVectorData<double>* > *cell_positions;
		MapIntIntData                      *local_index_map;
		DoubleData                         *t;
		DoubleData						   *eqt;

		// parameters of the capillary
		double initialCapillaryConc;
		double diffusionConstant;

		double Zt;      // Z coordinate of the top of the tube
		double Nt[3];
		double Zb;      // Z coordinate of the base of the tube
		double Nb[3];
		double Zc;   	// Z coordinate of the mouth of the capillary
		double Re;      // Radius of the entire environment
		double Rc;      // Radius of the capillary

	};

	// message with which the world tells the special agent about the positions of the cells for output
	class WorldTellSpecialAgentCellPositionsMessageGenerator : public MessageGenerator {
	public:
		WorldTellSpecialAgentCellPositionsMessageGenerator(Agent *ag);

		~WorldTellSpecialAgentCellPositionsMessageGenerator();

		bool isMessageForSpecialAgent() { return true; };

		void placeMessage(int destID);

	private:
		Database *db;
		double nextOutputTime;
		DoubleData *t ;
		DoubleData *ov;
		TVectorData<TVectorData<double>* > *positions;
		MapIntIntData *index_agentid_map;

	};


	// message generator in which the world already analyses the cell positions and sends the information to the output agent
	class WorldTellSpecialAgentAnalysedCellPositionsDataMessageGenerator : public MessageGenerator {
	public:
		WorldTellSpecialAgentAnalysedCellPositionsDataMessageGenerator(Agent *ag);

		~WorldTellSpecialAgentAnalysedCellPositionsDataMessageGenerator();

		bool isMessageForSpecialAgent() { return true; };

		void placeMessage(int destID);

	private:
		Database *db;
		double nextOutputTime;
		DoubleData *t;
		DoubleData *ov;
		TVectorData<TVectorData<double>* >* positions;
		MapIntIntData *index_agentid_map;
	};

	// message with which the cell agent tells the world agent about its swimming state
	class CellNotifyWorldAboutSwimmingStateMessageGenerator : public MessageGenerator {
	public:
		CellNotifyWorldAboutSwimmingStateMessageGenerator(Agent *ag);

		~CellNotifyWorldAboutSwimmingStateMessageGenerator();

		void placeMessage(int destID);

	private:
		Database *db;
		IntegerData *swimmingstate;
	};

	// message with which the cell agent tells the world agent about a change in its speed
	class CellNotifyWorldAboutNewSpeedMessageGenerator : public MessageGenerator {
		CellNotifyWorldAboutNewSpeedMessageGenerator(Agent *ag);

		~CellNotifyWorldAboutNewSpeedMessageGenerator();

		void placeMessage(int destID);

	private:
		Database *db;
	};

	// message with which a cell tells its internal state to the special agent for output
	class CellRevealItselfToSpecialAgentMessageGenerator : public MessageGenerator {
	public:
		CellRevealItselfToSpecialAgentMessageGenerator(Agent *ag);

		~CellRevealItselfToSpecialAgentMessageGenerator();

		bool isMessageForSpecialAgent() { return true; };

		void placeMessage(int destID);

	private:
		Database *db;
		double nextOutputTime;
		DoubleData *t;
		DoubleData *ov;
		TVectorData<double> *cheyp;
		TVectorData<double> *methLevel;
		IntegerData *swimmingstate;
		TVectorData<double> *ligand;
		TVectorData<double> *accligand;
		TVectorData<int> *motorStates;

		DoubleData *dLigand;
		DoubleData *dCheYP;
		DoubleData *dActivity;
		DoubleData *dMeth;


	};

	// message with which a cell tells the world about the amount of ligand it has seen
	class CellTellSpecialAgentAboutSeenLigandMessageGenerator : public MessageGenerator {
	public:
		CellTellSpecialAgentAboutSeenLigandMessageGenerator(Agent* ag);

		~CellTellSpecialAgentAboutSeenLigandMessageGenerator();

		bool isMessageForSpecialAgent() { return true; };

		void placeMessage(int destID);

	private:
		Database *db;
		DoubleData *time;
		double nextOutputTime;
		DoubleData *ov;
		TVectorData<double> *ligand;
		TVectorData<double> *accligand;
	};

	/// message generator with which a cell tells the special agent about the activity of its receptors
	/// and the cheyp level
	class CellTellSpecialAgentAboutActivityMessageGenerator : public MessageGenerator {
	public:
		CellTellSpecialAgentAboutActivityMessageGenerator(Agent *ag);

		~CellTellSpecialAgentAboutActivityMessageGenerator();

		bool isMessageForSpecialAgent() { return true; };

		void placeMessage(int destID);

	private:
		Database *db;
		DoubleData *time;
		double nextOutputTime;
		DoubleData *ov;
		DoubleData *activity;
		DoubleData *cheyp;
		DoubleData *methyl;
	};
}
#endif /* CHEMOPOPMESSAGEGENERATOR_HH_ */
