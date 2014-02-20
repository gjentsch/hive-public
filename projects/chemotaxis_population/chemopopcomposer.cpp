/**
 * chemopopcomposer.cc
 *
 */

#include "chemopopcomposer.hh"


using namespace std;
using namespace Hive;
using namespace ChemoPop;

ChemoPopComposer::ChemoPopComposer () {
	this->dt = 0;
	this->num_steps = 0;
	this->blockWorldOutput = false;

	/// flags for recording the special output
	/// default: turn that output off as it generates lots of data!
	this->generate_cell_data_output     = false;
	this->generate_cell_position_output = false;
	this->generate_run_tumble_output    = false,

	cerr << "# calling without command line arguments. i can not tolerate that." << endl;
	exit(1);
}


ChemoPopComposer::ChemoPopComposer(int argn, char *argv[]) {
	cerr << "# hello, i am the ChemoPopComposer " << endl;

	/// pointer to cell parameter initialiser
	this->cpi = NULL;

	/// we are setting up dt and num_steps here and in the worldagentfactory not good!
	this->dt = 0;
	this->num_steps = 0;

    /// ideally we would have a command line parser for this
	/// parse the command line
	int index = 1;
	string first, second;
	while (index < argn) {
		first = argv[index];
		index++;
		if (index == argn) {
			cerr << "wrong command-line" << endl;
			exit(1);
		}
		second = argv[index];
		commandline[first] = second;
		index++;
	}

	if (commandline.find("-init") != commandline.end()) {
		cerr << "# calling CellParameterInitializer to init cells" << endl;
		this->cpi = new CellParameterInitializer(commandline["-init"].c_str());
		cerr << "# calling WorldParameterInitialiser to init world" << endl;
		this->wpi = new WorldParameterInitialiser(commandline["-init"].c_str());
	} else
		throw HiveException("need following commandline in order to run -init initfile ",
				"ChemoPopComposer::ChemoPopComposer()");

	this->output_interval               = ((WorldParameterInitialiser *) this->wpi)->getOutputInterval();
	this->outputSuffix 					= ((WorldParameterInitialiser *) this->wpi)->getFileExtensionName();
	this->binaryOutput 					= ((WorldParameterInitialiser *) this->wpi)->isOutputBinary();
	this->blockWorldOutput 				= ((WorldParameterInitialiser *) this->wpi)->blockWorldOutput();
	this->generate_cell_data_output 	= ((WorldParameterInitialiser *) this->wpi)->generateCellData();
	this->generate_cell_position_output = ((WorldParameterInitialiser *) this->wpi)->generateCellPosition();
	this->generate_run_tumble_output 	= ((WorldParameterInitialiser *) this->wpi)->generateRunTumble();

}

ChemoPopComposer::~ChemoPopComposer() {
	if (this->cpi != NULL)
		delete cpi;
	if (this->wpi != NULL)
		delete wpi;
	delete post;
}

void ChemoPopComposer::setNumberOfCells(int num) {
	this->number_of_cells = num;
}


int ChemoPopComposer::getNumSteps()
{
	// in maestro:
	//IntegerData *nsteps  = new IntegerData("num_steps", number);
	//DoubleData *timestep = new DoubleData("dt", dt);
	//DoubleData *startt   = new DoubleData("localworldtime", startTime);
	//DoubleData *eqt      = new DoubleData("eqTime", eqTime);

	num_steps = ((IntegerData *) this->maestro->getDatabase()->getDataItem("num_steps"))->getInteger();

	return num_steps;
}

double ChemoPopComposer::getTimeIncrement()
{
	return dt;
}

void ChemoPopComposer::setupSimulation() {
	this->initializeAgentFactories();

	Registrar *r = Registrar::getSystemRegistrar();

	maestro = this->createAgent(*factories[0]);

	// ask the worldagentfactory for the equilibration time
	double local_equilibration_time = ((WorldParameterInitialiser *) wpi)->getEquilibrationTime();
	// and  tell the equilibration time to the cell
	((CellAgentFactory*)factories[1])->setEquilibrationTime(local_equilibration_time);

	orchestra.push_back(maestro);
	r->registerNewAgent(maestro);

	/// error checking before initialising the number of cells
	if (  ((WorldParameterInitialiser *) wpi)->getNumberCells() != cpi->getNumberAgents() ) {

		std::stringstream outx, outy;
		outx <<  ((WorldParameterInitialiser *) wpi)->getNumberCells(); string x; x = outx.str();
		outy << cpi->getNumberAgents(); string y; y = outy.str();

		throw HiveException("this one will be fatal! world factory and cell factories operate with different cell numbers ("+x+" in worldFactory vs. "+y+" in cellFactory)",
				"ChemoPopComposer::setupSimulation()");
	}

	// get the number of cells
	this->number_of_cells = cpi->getNumberAgents();

	cerr <<  "# creating cells " << endl;
	// create the cells of the orchestra
	for (int i =0; i<this->number_of_cells; i++) {
		Agent *cell = factories[1]->createAgent();
		orchestra.push_back(cell);
		r->registerNewAgent(cell);
	}

	// set the number of cells that the output agent will have to deal with
	((OutputAgentFactory*) factories.at(2))->setNumberCells(number_of_cells);

	// create the output agent
	SpecialAgent *ibn_abi_sarh = (SpecialAgent* ) factories.at(2)->createAgent();
	ibn_abi_sarh->setOutputInterval(this->output_interval);
	special_agents.push_back(ibn_abi_sarh);
	r->registerNewAgent(ibn_abi_sarh);
	r->registerNewSpecialAgent(ibn_abi_sarh);

	this->setupAgentHierarchy();

	this->addSerialCommunicator();

	this->num_steps = ((WorldParameterInitialiser *) wpi)->getNumberSteps();
	this->dt        = ((WorldParameterInitialiser *) wpi)->getDt();

	// we do not need to send an initialisation messages.
	cerr << "# done setting up the simulation\n" << endl;
}

void ChemoPopComposer::initializeAgentFactories() {
	// the world factory will now make use of the new worldparameter-initialiser.

	// initialise the world agent factory: factory which uses an external parser.
	this->factories.push_back(new WorldAgentFactory());
	// add world parameter initialiser to factory
	((WorldAgentFactory *) this->factories.at(factories.size()-1))->setWorldParameterInitialiser((WorldParameterInitialiser*) this->wpi);
	this->factories.at(factories.size()-1)->finalise();

	// initialise cell factory
	this->factories.push_back(new CellAgentFactory());
	((CellAgentFactory*)this->factories.at(factories.size()-1))->setInitializer((CellParameterInitializer*) this->cpi);
	((CellAgentFactory*)this->factories.at(factories.size()-1))->setOutputInterval(this->output_interval);
	((CellAgentFactory*)this->factories.at(factories.size()-1))->setGenerateCellDataOutput(this->generate_cell_data_output);
	this->factories.at(factories.size()-1)->finalise();


	// initialise the outputagent factory
	this->factories.push_back(new OutputAgentFactory());
	((OutputAgentFactory *)this->factories.at(factories.size()-1))->setOutputInterval(this->output_interval);
	((OutputAgentFactory *)this->factories.at(factories.size()-1))->setOutputStyle(this->binaryOutput);
	((OutputAgentFactory *)this->factories.at(factories.size()-1))->setOutputFileNameSuffix(this->outputSuffix);
	this->factories.at(factories.size()-1)->finalise();

	//Register all the factories
	for(int k=0; k<factories.size(); k++)
	{
		Registrar::getSystemRegistrar()->registerAgentFactory(factories.at(k));
	}
}

void ChemoPopComposer::setupAgentHierarchy() {
	// set children of maestro
	if (this->number_of_cells > 0) {
		for (int i=1;i<=this->number_of_cells; i++) {
			Agent *cell = orchestra.at(i);
			maestro->addChild(cell->getAgentId());
			cell->setParent(maestro->getAgentId());
			cell->addSpecialAgent(this->special_agents[0]->getAgentId());
		}
	}
	maestro->addSpecialAgent(this->special_agents[0]->getAgentId());
}

void ChemoPopComposer::addSerialCommunicator() {
	post = new SerialCommunicator();
	for (unsigned int i=0; i<orchestra.size(); i++) {
		post->addAgent(orchestra[i]);
		orchestra[i]->addCommunicator(post);
	}

	for (unsigned int i=0; i<this->special_agents.size(); i++) {
		post->addAgent(special_agents[i]);
		special_agents[i]->addCommunicator(post);
	}

}

Agent* ChemoPopComposer::createAgent(AgentFactory& af) {
	return af.createAgent();
}
