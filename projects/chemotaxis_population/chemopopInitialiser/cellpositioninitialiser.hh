/*
 * cellpositioninitialiser.hh
 *
 *  Created on: Apr 18, 2010
 *      Author: jentsch
 */

#ifndef CELLPOSITIONINITIALISER_HH_
#define CELLPOSITIONINITIALISER_HH_

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "../../../core/agent/agent.hh"
#include "../../../core/agent/data/primitive/primitiveData.hh"
#include "../../../core/agent/data/template/tdata.hh"
#include "../../../core/util/util.hh"
#include "../../../core/exception/exception.hh"

using namespace std;
using namespace Hive;


namespace ChemoPop {
	/**
	 * @brief just a class to initialise cell positions
	 *
	 * @author garrit
	 *
	 * @date 4-18-10
	 */
	class CellPositionInitialiser {
	public:
		/// constructor
		CellPositionInitialiser();

		/// destructor
		~CellPositionInitialiser();

		void initAllInSamePosition(Agent *ag, double x, double y, double z, int num);

		void initUniformDistAlongXAxis(Agent *ag, double minX, double maxX, int num);

		void initRandomlyAroundPosition(Agent *ag, double x, double y, double z, double radius, int num);

		void initFromFile(Agent *ag, ifstream& input);


	private:
		// for initialising the direction and up vecs as well as the cellspeed vector
		// this should also do the initialisation of all vectors of which the length has to be equal to
		// number of cells in the system.
		void doTheRest(Agent *ag);

		/// cell number ... is it needed?
		int  cellnumber;
	};
}

#endif /* CELLPOSITIONINITIALISER_HH_ */
