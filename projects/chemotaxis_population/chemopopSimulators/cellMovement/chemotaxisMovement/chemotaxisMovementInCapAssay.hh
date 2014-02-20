/*
 * chemotaxisMovementInCapAssay.hh
 *
 *  Created on: Oct 23, 2013
 *      Author: jentsch
 */

#ifndef CHEMOTAXISMOVEMENTINCAPASSAY_HH_
#define CHEMOTAXISMOVEMENTINCAPASSAY_HH_

#include <cmath>
#include <fstream>

#include "../../../../../core/agent/data/data.hh"
#include "../../../../../core/agent/data/template/tdata.hh"
#include "../../../../../core/agent/data/primitive/primitiveData.hh"
#include "../../../../../core/agent/data/boundary/boundarydata.hh"
#include "../../../../../core/simulators/simulator.hh"
#include "../../../../../core/util/util.hh"

#include "../../../chemopopInitialiser/cellparameterinitializer.hh"

using namespace std;
using namespace Hive;

namespace ChemoPop {

	/*
	 *@brief this is michael's chemotaxismovementsimulator that has been adapted such that
	 *       it runs on the world level instead of the cell level
	 */
	class ChemotaxisMovementSimulatorInCapAssay : public Simulator {
	public:
		ChemotaxisMovementSimulatorInCapAssay();
		virtual ~ChemotaxisMovementSimulatorInCapAssay();

		void setWorldAgent(Agent *ag);

		void step(double t); // t is the timestep that we need to
		void prepare();
		void synchroniseWithDatabase();

		void setGenerateRunTumbleOutput(bool b) { this->generate_run_tumble_output = b; };

	protected:
		void initialise();

	private:
		// not quite sure what these do, i assume
		void changeDirRandom(int i);
		void changeDirDistribution(int i);
		void changeDirRotDiffusion(int i);
		void swimToNewPosition(int i); // what are the parameters ?!
		void moveToNewPosition(int i);

		// new method to account for the boundaries of the
		// capillary assay (call-by-reference)
		bool updatePosition(double p[3], double p2[3], double u[3], double up[3]);

		double eqTime;
		double internalTime;

		// here we declare all the pointers to the data-structures of the world that are needed for the simulation
		IntegerData *numberofcells;
		DoubleData  *rotdiffconst;
		DoubleData  *movement_dt;
		BoolData    *isIn2D;

		TVectorData<double> *cellspeeds;
		TVectorData<int>    *last_swimming_states;
		TVectorData<int>    *swimming_states;

		TVectorData<TVectorData<double>* > *cell_positions;
		TVectorData<TVectorData<double>* > *cell_dir_vecs;
		TVectorData<TVectorData<double>* > *cell_up_vecs;

		double rotMat[3][3];
		Util::GammaSampler *gs;

		ofstream *out;

		bool generate_run_tumble_output;

		// variables and parameters needed for capillary
		double Zt;      // Z coordinate of the top of the tube
		double Nt[3];
		double Zb;      // Z coordinate of the base of the tube
		double Nb[3];
		double Zc;      // Z coordinate of the mouth of the capillary
		double Re;      // Radius of the entire environment
		double Rc;      // Radius of the capillary

	};

}

#endif /* CHEMOTAXISMOVEMENTINCAPASSAY_HH_ */
