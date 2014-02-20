/*
 * worldagentfactory.hh
 *
 *  Created on: Oct 1st, 2010
 *      Author: jentsch
 */

#ifndef WORLDAGENTFACTORY_HH_
#define WORLDAGENTFACTORY_HH_

#include "../../../core/agent/agentFactory/agent_factory.hh"
#include "../../../core/agent/data/data.hh"
#include "../../../core/agent/data/primitive/primitiveData.hh"
#include "../../../core/agent/data/template/tdata.hh"
#include "../../../core/agent/data/mapdata/mapintintdata.hh"
#include "../../../core/exception/exception.hh"
#include "../../../core/simulators/timekeeper/timekeeper.hh"

#include "../chemopopAction/chemopopAction.hh"
#include "../chemopopMessageGenerator/chemopopMessageGenerator.hh"
#include "../chemopopInitialiser/worldparameterinitialiser.hh"
#include "../chemopopSimulators/cellMovement/chemotaxisMovement/chemotaxisMovementInWorld.hh"
#include "../chemopopSimulators/cellMovement/chemotaxisMovement/chemotaxisMovementInCapAssay.hh"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

using namespace std;
using namespace Hive;

namespace ChemoPop {

	/**
	 * @brief world agent factory of the chemotaxis population simulation.
	 *
	 *
	 * @author Garrit Jentsch
	 *
	 * @date Oct 1st, 2010 last edited: 2010
	 */
	class WorldAgentFactory : public AgentFactory {
	public:
		WorldAgentFactory();
		~WorldAgentFactory();

		Agent* createAgent();
		Agent* createAgent(Action **as);

		Agent* duplicateAgent(Agent *ag);

		void finalise();

		void setWorldParameterInitialiser(WorldParameterInitialiser *wpi) { this->wpi = wpi; };

	protected:
		void addActionsToAgentsActionSet(Agent *ag);
		void addSimulatorToAgent(Agent *ag);
		void addMessageGeneratorsToAgent(Agent *ag);

	private:
		WorldParameterInitialiser *wpi;
	};


}


#endif /* WORLDAGENTFACTORY_HH_ */
