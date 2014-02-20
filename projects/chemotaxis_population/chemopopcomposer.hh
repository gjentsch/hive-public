/**
 *  @file chemopopcomposer.hh
 *
 *  @brief  interface to the
 *
 * Created on: April 16, 2010
 * author : garrit
 */

#ifndef CHEMOPOPCOMPOSER_HH_
#define CHEMOPOPCOMPOSER_HH_

#include "../../core/composer/composer.hh"
#include "../../core/communicator/serial/serialcommunicator.hh"
#include "../../core/input/commandLineParser/commandLineParser.hh"

#include "chemopopFactories/cellagentfactory.hh"
#include "chemopopFactories/outputagentfactory.hh"
#include "chemopopFactories/worldagentfactory.hh"

#include "chemopopInitialiser/agentinitializer.hh"
#include "chemopopInitialiser/cellparameterinitializer.hh"
#include "chemopopInitialiser/worldparameterinitialiser.hh"

#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <stdlib.h>
#include <string>
#include <vector>

using namespace std;
using namespace Hive;

namespace ChemoPop {

	/**
	 * @brief composer for setting up the chemotaxis population simulations
	 *
	 * @author Garrit
	 *
	 * @date April 16, 2010 last edited: 4-16-2010 by garrit
	 */
	class ChemoPopComposer : public Composer {
	public:
		ChemoPopComposer();

		ChemoPopComposer(int argn, char *argv[]);

		~ChemoPopComposer();

		int getNumSteps();

		double getTimeIncrement();

		void setNumberOfCells(int);

		void setupSimulation();

	protected:
		void initializeAgentFactories();

		void setupAgentHierarchy();

		void addSerialCommunicator();

		Agent* createAgent(AgentFactory& af);

		int number_of_cells;
		int num_steps;

		double dt;
		double output_interval;

		SerialCommunicator *post;

		map<string,string> commandline;

		AgentInitializer *cpi; 			// cell parameter initializer
		AgentInitializer *wpi;			// world parameter initializer

		string outputSuffix;

		/// some flags
		bool binaryOutput;               // determines output mode
		bool blockWorldOutput;           // suppresses output of world information!

		/// some more flags
		bool generate_cell_data_output;
		bool generate_cell_position_output;
		bool generate_run_tumble_output;

	};
}


#endif // CHEMOPOPCOMPOSER_HH_
