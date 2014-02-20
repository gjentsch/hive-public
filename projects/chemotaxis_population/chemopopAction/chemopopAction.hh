/*
 * chemopopAction.hh
 *
 *  Created on: Apr 16, 2010
 *      Author: jentsch
 */

#ifndef CHEMOPOPACTION_HH_
#define CHEMOPOPACTION_HH_

#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <sstream>
#include <stdlib.h>

#include "../../../core/agent/action/arnie.hh"
#include "../chemopopMessageGenerator/chemopopMessageGenerator.hh"
#include "../../../core/agent/data/primitive/primitiveData.hh"
#include "../../../core/agent/data/template/tdata.hh"
#include "../../../core/agent/data/mapdata/mapintintdata.hh"
#include "../../../core/exception/exception.hh"
#include "../../../core/agent/special_agent.hh"
#include "../../../core/util/util.hh"

using namespace std;
using namespace Hive;

namespace ChemoPop {
	/// action which updates the swimming state of the cell inside the world
	class UpdateWorldSeenCellsSwimmingStateAction : public Action {
	public:
		UpdateWorldSeenCellsSwimmingStateAction();
		~UpdateWorldSeenCellsSwimmingStateAction();

		void fire(Data *d);
		void prepare();
	private:
		MapIntIntData *local_index_map;
		TVectorData<int> *swimstates;
		TVectorData<int> *oldstates;

	};

	/// action which updates the speed of cells inside the world
	class UpdateWorldSeenCellSpeedAction : public Action {
	public:
		UpdateWorldSeenCellSpeedAction();
		~UpdateWorldSeenCellSpeedAction();

		void fire(Data *d);
		void prepare();
	private:

		MapIntIntData *local_index_map;
		TVectorData<double> *cellspeeds;
	};

	/// action which updates the ligand concentration that a cell sees
	class UpdateCellSeenLigandAction : public Action {
	public:
		UpdateCellSeenLigandAction();
		~UpdateCellSeenLigandAction();
		void prepare();

		void fire(Data *d);
	private:
		DoubleData *essence;
		DoubleData *kcat;
		DoubleData *Km;
		TVectorData<double> *appetite;
		TVectorData<double> *ligand_conc;
		TVectorData<double> *cellaccligand;
		DoubleData *dt;
		DoubleData *yield;
		DoubleData *effeciency;
	};

	/// action by which the outputagent receives the positions of the cells
	class SpecialAgentReceiveCellPositionsAction : public Action {
	public:
		SpecialAgentReceiveCellPositionsAction();
		~SpecialAgentReceiveCellPositionsAction();

		void fire(Data*);
		void prepare();

		void prepareOutputfile(string, bool);

	private:
		ofstream *out;
		bool isbinary;
	};

	/// action with which the special agent accumulates data for generating output
	class SpecialAgentAccumulateCellDataAction : public Action {
	public:
		SpecialAgentAccumulateCellDataAction();
		~SpecialAgentAccumulateCellDataAction();

		void fire(Data *);
		void prepare();

		void prepareOutputfile(string, bool);

	private:
		ofstream *out;
		int cellcounter;
		bool isbinary;


		TVectorData<TVectorData<double>* > *cell_matrix;
		IntegerData *numberofcellsData;
	};

	class SpecialAgentOutputAnalysedCellPositionsDataAction : public Action {
	public:
		SpecialAgentOutputAnalysedCellPositionsDataAction();
		~SpecialAgentOutputAnalysedCellPositionsDataAction();

		void fire(Data *);
		void prepare();
		void prepareOutputfile(string, bool);
	private:
		ofstream *out;
		bool isbinary;
	};

	class SpecialAgentOutputAnalysedLigandDataAction : public Action {
	public:
		SpecialAgentOutputAnalysedLigandDataAction();
		~SpecialAgentOutputAnalysedLigandDataAction();

		void fire(Data *);
		void prepare();
		void prepareOutputfile(string, bool);
	private:
		ofstream *out;
		bool isbinary;

		// data needed for this action
		DoubleData *for_mean_acc_ligand;
		DoubleData *for_mean_sq_acc_ligand;
		IntegerData *messages_received_for_analysed_ligand_data_action;

		int numberofcells;
	};

	/// message with which the special agent outputs the mean activity and the mean
	/// cheyp level across the population.
	class SpecialAgentOutputAnalysedCellActivityAction : public Action {
	public:
		SpecialAgentOutputAnalysedCellActivityAction();
		~SpecialAgentOutputAnalysedCellActivityAction();

		void fire(Data *);
		void prepare();
		void prepareOutputfile(string, bool);
	private:
		ofstream *out;
		bool isbinary;

		// Data needed for this action
		DoubleData  *mean_activity;
		DoubleData  *mean_cheyp;
		DoubleData  *mean_methyl;
		IntegerData *messages_received_for_analysed_cell_activity_action;

		int numberofcells;
	};

}

#endif /* CHEMOPOPACTION_HH_ */
