#include "chemotaxisMovementInWorld.hh"

using namespace std;
using namespace Hive;
using namespace ChemoPop;

ChemotaxisMovementSimulatorInWorld::ChemotaxisMovementSimulatorInWorld() {
	this->eqTime = 0;

	// initialise the rotation Matrix
	rotMat[0][0] = 1;	rotMat[0][1] = 0;	rotMat[0][2] = 0;
	rotMat[1][0] = 0;	rotMat[1][1] = 1; 	rotMat[1][2] = 0;
	rotMat[2][0] = 0;	rotMat[2][1] = 0; 	rotMat[2][2] = 1;

	double DefaultGammaAlpha = 4.;
	double DefaultGammaBeta = 1./18.32045567939674;
	double DefaultGammaOffset = -4.606176605609249;
	gs = new Util::GammaSampler(DefaultGammaAlpha, DefaultGammaBeta, DefaultGammaOffset);

	this->eqTime        = 0;
	this->numberofcells = 0;
	this->rotdiffconst  = 0;
	this->movement_dt   = 0;
	this->cellspeeds           = 0;
	this->last_swimming_states = 0;
	this->swimming_states      = 0;
	this->cell_positions = 0;
	this->cell_dir_vecs  = 0;
	this->cell_up_vecs   = 0;

	this->generate_run_tumble_output = false;

	// use when you want to record the run-length distribution
	out = new ofstream("file_recording_the_changes.bdat",ios_base::binary);
}

ChemotaxisMovementSimulatorInWorld::~ChemotaxisMovementSimulatorInWorld() {
	out->close(); // if you do not close this you risk of not clearing the buffer such that you may have an empty file.
	delete gs;
}

void ChemotaxisMovementSimulatorInWorld::setWorldAgent(Agent *ag) {
	this->agent        = ag;
	this->internalTime = 0;

	this->initialise();
}

void ChemotaxisMovementSimulatorInWorld::initialise() {
	// neglecting error checking ...
	Database *db = agent->getDatabase();

	// setting the equilibration time.
	this->eqTime        = ((DoubleData*) db->getDataItem("eqTime"))->getDouble();

	// i hope i didn't screw up on the names of the data items ...
	this->numberofcells = (IntegerData*) db->getDataItem("numberofcells");
	if(db->existsDataItem("rotational_diffusion_constant")) {
		this->rotdiffconst  = (DoubleData*)  db->getDataItem("rotational_diffusion_constant");
		cerr<<"# in ChemotaxisMovementSimulatorInWorld::initialize(), detected ROTDIF = "<<this->rotdiffconst->getDouble()<<endl;
	} else {
		cerr<<"# WARNING! : in ChemotaxisMovementSimulatorInWorld::initialize(), no ROTDIF flag found"<<endl;
		cerr<<"# WARNING! : for legacy reasons, the hive will assume cells have rotational diffusion"<<endl;
		cerr<<"# WARNING! : constants of 0.062 rad^2/s, but this should be made explicit!"<<endl;
		this->rotdiffconst = new DoubleData("rotational_diffusion_constant",0.062);
		db->addData(rotdiffconst);
	}
	this->movement_dt   = (DoubleData*)  db->getDataItem("dt");

	this->cellspeeds           = (TVectorData<double>*) db->getDataItem("cellspeeds");

	this->last_swimming_states = (TVectorData<int>* )   db->getDataItem("lastswimmingstates");
	this->swimming_states      = (TVectorData<int>* )   db->getDataItem("swimmingstates");

	this->cell_positions = (TVectorData<TVectorData<double>* >* ) db->getDataItem("cellpositions");
	this->cell_dir_vecs  = (TVectorData<TVectorData<double>* >* ) db->getDataItem("celldirvecs");
	this->cell_up_vecs   = (TVectorData<TVectorData<double>* >* ) db->getDataItem("cellupvecs");

	// check if movement is in 2d.  If no flag was given, we assume 3D
	if(db->existsDataItem("isIn2D")) {
		this->isIn2D         = (BoolData *) db->getDataItem("isIn2D");

	} else {
		cerr<<"# WARNING! : in ChemotaxisMovementSimulatorInWorld::initialize(), no DIMENSION flag found"<<endl;
		cerr<<"# WARNING! : for legacy reasons, the hive will assume cells move in 3D, but this should be explicit!"<<endl;
		this->isIn2D = new BoolData("isIn2D",false);
		db->addData(isIn2D);
	}

	//db->printDataBase(cerr);

    // We have to make sure the up vector is in Z and the direction vector is in X
	for (int i=0; i<this->numberofcells->getInteger(); i++) {
		//cerr<<"dir["<<i<<"]: "<<cell_dir_vecs->at(i)->at(0)<<" "<<cell_dir_vecs->at(i)->at(1)<<" "<<cell_dir_vecs->at(i)->at(2)<<endl;
		//cerr<<"up["<<i<<"]: "<<cell_up_vecs->at(i)->at(0)<<" "<<cell_up_vecs->at(i)->at(1)<<" "<<cell_up_vecs->at(i)->at(2)<<endl;
		cell_dir_vecs->at(i)->at(0)=1;
		cell_dir_vecs->at(i)->at(1)=0;
		cell_dir_vecs->at(i)->at(2)=0;
		cell_up_vecs->at(i)->at(0)=0;
		cell_up_vecs->at(i)->at(1)=0;
		cell_up_vecs->at(i)->at(2)=1;

	}

	for (int i=0; i<this->numberofcells->getInteger(); i++) {
		this->changeDirRandom(i);
	}

	//for (int i=0; i<this->numberofcells->getInteger(); i++) {
	//	cerr<<"dir["<<i<<"]: "<<cell_dir_vecs->at(i)->at(0)<<" "<<cell_dir_vecs->at(i)->at(1)<<" "<<cell_dir_vecs->at(i)->at(2)<<endl;
	//	cerr<<"up["<<i<<"]: "<<cell_up_vecs->at(i)->at(0)<<" "<<cell_up_vecs->at(i)->at(1)<<" "<<cell_up_vecs->at(i)->at(2)<<endl;
	//	}

}

void ChemotaxisMovementSimulatorInWorld::prepare() {
	// i think there is nothing to do here.
}

void ChemotaxisMovementSimulatorInWorld::synchroniseWithDatabase() {
	// i don't think there is anything to do here.
}

void ChemotaxisMovementSimulatorInWorld::step(double t) {
	if (this->internalTime >= this->eqTime) {
		// iterate over all the cells.
		for (int i=0; i<this->numberofcells->getInteger(); i++) {
			if (this->swimming_states->at(i) == CellParameterData::RUN) {

				// this is for recording the distribtuion of run-times to file
				if (this->last_swimming_states->at(i) == CellParameterData::TUMBLE) {
					// OUTPUT: time ID pos Run/Tumble

					if (this->generate_run_tumble_output) {
						double d;
						d = this->internalTime;
						this->out->write(reinterpret_cast<char*> (&d), sizeof(d));
						d = (double)i+1.0;
						this->out->write(reinterpret_cast<char*> (&d), sizeof(d));
						d = this->cell_positions->at(i)->at(0);
						this->out->write(reinterpret_cast<char*> (&d), sizeof(d));
						d = 1.0;
						this->out->write(reinterpret_cast<char*> (&d), sizeof(d));
					}
				}
				// do the swimming
				this->swimToNewPosition(i);
				this->last_swimming_states->at(i) = CellParameterData::RUN;

			} else if (this->swimming_states->at(i) == CellParameterData::TUMBLE
					&& this->last_swimming_states->at(i) == CellParameterData::RUN) {
//				this->changeDirDistribution(i);
				this->changeDirRandom(i);
				this->last_swimming_states->at(i) = CellParameterData::TUMBLE;
				// the following nine lines are for recording the distribution of run-times
				// to file
				if (this->generate_run_tumble_output) {
					double d;
					d = this->internalTime;
					this->out->write(reinterpret_cast<char*> (&d), sizeof(d));
					d = (double)i+1.0;
					this->out->write(reinterpret_cast<char*> (&d), sizeof(d));
					d = this->cell_positions->at(i)->at(0);
					this->out->write(reinterpret_cast<char*> (&d), sizeof(d));
					d = 0.0;
					this->out->write(reinterpret_cast<char*> (&d), sizeof(d));
				}
			}
		}
	}
	this->internalTime = this->internalTime + t;

}


void ChemotaxisMovementSimulatorInWorld::swimToNewPosition(int i) {
	this->moveToNewPosition(i);
	// account for rotational diffusion!
	this->changeDirRotDiffusion(i);
}

void ChemotaxisMovementSimulatorInWorld::moveToNewPosition(int i) {
	//cerr<<"query cell speed @ "<<i<<endl;
	//cerr<<"cellspeeds.size()  "<<cellspeeds->size()<<endl;
	//cerr<<"cellspeeds->at(i)  "<<cellspeeds->at(i)<<endl;
	//cerr<<"dir["<<i<<"]: "<<cell_dir_vecs->at(i)->at(0)<<" "<<cell_dir_vecs->at(i)->at(1)<<" "<<cell_dir_vecs->at(i)->at(2)<<endl;
	//cerr<<"up["<<i<<"]: "<<cell_up_vecs->at(i)->at(0)<<" "<<cell_up_vecs->at(i)->at(1)<<" "<<cell_up_vecs->at(i)->at(2)<<endl;


	double distanceTraveled = this->cellspeeds->at(i) * this->movement_dt->getDouble();

	double start[3];
	start[0] = this->cell_positions->at(i)->at(0);
	start[1] = this->cell_positions->at(i)->at(1);
	start[2] = this->cell_positions->at(i)->at(2);

	// the direction vector is always a unitvector, so this works!
	double predictedEnd[3];
	predictedEnd[0] = this->cell_positions->at(i)->at(0) + distanceTraveled*this->cell_dir_vecs->at(i)->at(0);
	predictedEnd[1] = this->cell_positions->at(i)->at(1) + distanceTraveled*this->cell_dir_vecs->at(i)->at(1);
	predictedEnd[2] = this->cell_positions->at(i)->at(2) + distanceTraveled*this->cell_dir_vecs->at(i)->at(2);

	//cerr<<this->cell_positions->at(i)->at(2)<<endl;
	//cerr<<this->cell_dir_vecs->at(i)->at(2)<<endl;
	double direction[3];
	direction[0] = this->cell_dir_vecs->at(i)->at(0);
	direction[1] = this->cell_dir_vecs->at(i)->at(1);
	direction[2] = this->cell_dir_vecs->at(i)->at(2);

    // update positions and directions
	this->cell_positions->at(i)->at(0) = predictedEnd[0];
	this->cell_positions->at(i)->at(1) = predictedEnd[1];
	this->cell_positions->at(i)->at(2) = predictedEnd[2];
	this->cell_dir_vecs->at(i)->at(0)  = direction[0];
	this->cell_dir_vecs->at(i)->at(1)  = direction[1];
	this->cell_dir_vecs->at(i)->at(2)  = direction[2];

	//cerr<<"dir["<<i<<"]: "<<cell_dir_vecs->at(i)->at(0)<<" "<<cell_dir_vecs->at(i)->at(1)<<" "<<cell_dir_vecs->at(i)->at(2)<<endl;
	//cerr<<"up["<<i<<"]: "<<cell_up_vecs->at(i)->at(0)<<" "<<cell_up_vecs->at(i)->at(1)<<" "<<cell_up_vecs->at(i)->at(2)<<endl;
	//if(abs(cell_dir_vecs->at(i)->at(2))>10e-6 ) {
	//exit(1);
	//}
}


void ChemotaxisMovementSimulatorInWorld::changeDirRandom(int i)
{

	double local_up[3];
	local_up[0] = this->cell_up_vecs->at(i)->at(0);
	local_up[1] = this->cell_up_vecs->at(i)->at(1);
	local_up[2] = this->cell_up_vecs->at(i)->at(2);
	double local_dir[3];
	local_dir[0] = this->cell_dir_vecs->at(i)->at(0);
	local_dir[1] = this->cell_dir_vecs->at(i)->at(1);
	local_dir[2] = this->cell_dir_vecs->at(i)->at(2);

	if(this->isIn2D->getBool()) {
		double angleInRad = Util::RANDOM_CLOSED()*2.*Util::PI;
		Util::genRot3dAboutAxis(rotMat,local_up,angleInRad);
	} else {
		Util::genUniformRandRotation3d(rotMat);
		Util::applyRotation(rotMat,local_up);
	}
	Util::applyRotation(rotMat,local_dir);

	this->cell_up_vecs->at(i)->at(0) = local_up[0];
	this->cell_up_vecs->at(i)->at(1) = local_up[1];
	this->cell_up_vecs->at(i)->at(2) = local_up[2];

	this->cell_dir_vecs->at(i)->at(0) = local_dir[0];
	this->cell_dir_vecs->at(i)->at(1) = local_dir[1];
	this->cell_dir_vecs->at(i)->at(2) = local_dir[2];

}


void ChemotaxisMovementSimulatorInWorld::changeDirDistribution(int i) {
	double local_up[3];
	local_up[0] = this->cell_up_vecs->at(i)->at(0);
	local_up[1] = this->cell_up_vecs->at(i)->at(1);
	local_up[2] = this->cell_up_vecs->at(i)->at(2);

	double local_dir[3];
	local_dir[0] = this->cell_dir_vecs->at(i)->at(0);
	local_dir[1] = this->cell_dir_vecs->at(i)->at(1);
	local_dir[2] = this->cell_dir_vecs->at(i)->at(2);

	double local_down[3];
	local_down[0]= 0;
	local_down[1]= 0;
	local_down[2]= -1;

	//	double lastDir[3];
	//	lastDir[0] = local_dir[0];
	//	lastDir[1] = local_dir[1];
	//	lastDir[2] = local_dir[2];

	if(this->isIn2D->getBool()) {
		// generate the angle
		double angleInRad = gs->gen(0.,180.)*Util::PI/180.;
		// rotate around the up direction, which should always be up (0,0,1)
		if(Util::RANDOM_CLOSED()>0.5){
		  Util::genRot3dAboutAxis(rotMat,local_up,angleInRad);}
		else {
		  Util::genRot3dAboutAxis(rotMat,local_down,angleInRad);}
		Util::applyRotation(rotMat,local_dir);
	} else {
		// First we have to rotate the up vector around the direction (because we
		// can rotate randomly into any dimension from the cells perspective)
		double angleInRad = Util::RANDOM_CLOSED()*2.*Util::PI;
		Util::genRot3dAboutAxis(rotMat,local_dir,angleInRad);
		Util::applyRotation(rotMat,local_up);
		// Then we can rotate the cell direction about the up vector
		angleInRad = gs->gen(0.,180.)*Util::PI/180.;
		Util::genRot3dAboutAxis(rotMat,local_up,angleInRad);
		Util::applyRotation(rotMat,local_dir);
	}

	this->cell_up_vecs->at(i)->at(0) = local_up[0];
	this->cell_up_vecs->at(i)->at(1) = local_up[1];
	this->cell_up_vecs->at(i)->at(2) = local_up[2];

	this->cell_dir_vecs->at(i)->at(0) = local_dir[0];
	this->cell_dir_vecs->at(i)->at(1) = local_dir[1];
	this->cell_dir_vecs->at(i)->at(2) = local_dir[2];

	//	double scalar = lastDir[0]*local_dir[0]+lastDir[1]*local_dir[1]+lastDir[2]*local_dir[2];
	//	double a = sqrt(lastDir[0]*lastDir[0]+lastDir[1]*lastDir[1]+lastDir[2]*lastDir[2]);
	//	double b = sqrt(local_dir[0]*local_dir[0] + local_dir[1]*local_dir[1]+local_dir[2]*local_dir[2]);
	//		cout << scalar/(a*b) << endl;

	//cerr<<"dir["<<i<<"]: "<<cell_dir_vecs->at(i)->at(0)<<" "<<cell_dir_vecs->at(i)->at(1)<<" "<<cell_dir_vecs->at(i)->at(2)<<endl;
	//cerr<<"up["<<i<<"]: "<<cell_up_vecs->at(i)->at(0)<<" "<<cell_up_vecs->at(i)->at(1)<<" "<<cell_up_vecs->at(i)->at(2)<<endl;
	//if(abs(cell_dir_vecs->at(i)->at(2))>10e-6 ) {
	//	exit(1);
	//}

}

void ChemotaxisMovementSimulatorInWorld::changeDirRotDiffusion(int i) {
	double local_up[3];
	local_up[0] = this->cell_up_vecs->at(i)->at(0);
	local_up[1] = this->cell_up_vecs->at(i)->at(1);
	local_up[2] = this->cell_up_vecs->at(i)->at(2);

	double local_dir[3];
	local_dir[0] = this->cell_dir_vecs->at(i)->at(0);
	local_dir[1] = this->cell_dir_vecs->at(i)->at(1);
	local_dir[2] = this->cell_dir_vecs->at(i)->at(2);

	//double lastDir[3];
	//lastDir[0] = local_dir[0];
	//lastDir[1] = local_dir[1];
	//lastDir[2] = local_dir[2];

	//cerr<<"\n"<<endl;
	//cerr<<"dir["<<i<<"]: "<<cell_dir_vecs->at(i)->at(0)<<" "<<cell_dir_vecs->at(i)->at(1)<<" "<<cell_dir_vecs->at(i)->at(2)<<endl;
	//cerr<<"up["<<i<<"]: "<<cell_up_vecs->at(i)->at(0)<<" "<<cell_up_vecs->at(i)->at(1)<<" "<<cell_up_vecs->at(i)->at(2)<<endl;

	double rotDiffTerm = sqrt(this->movement_dt->getDouble() * 2. * this->rotdiffconst->getDouble());
	double anglePhi   = 0;
	double angleOmega = 0; // rotates on z/y plane after first rotation
	double anglePsi   = rotDiffTerm * Util::RANDOM_GAUSSIAN(); // rotates on x/y plane after first two rotations
	if(!this->isIn2D->getBool()) {  //if in 3D, we need to have rotations on two other axis as well
		anglePhi   = rotDiffTerm * Util::RANDOM_GAUSSIAN();;
		angleOmega = rotDiffTerm * Util::RANDOM_GAUSSIAN();
	}

	Util::genRotFromAngles(rotMat, anglePhi, angleOmega, anglePsi);
	Util::applyRotation(rotMat,local_dir);

	if(!this->isIn2D->getBool()) {  // save on some multiplications when we run in 2D
		Util::applyRotation(rotMat,local_up);
	}

	this->cell_dir_vecs->at(i)->at(0) = local_dir[0];
	this->cell_dir_vecs->at(i)->at(1) = local_dir[1];
	this->cell_dir_vecs->at(i)->at(2) = local_dir[2];

	this->cell_up_vecs->at(i)->at(0) = local_up[0];
	this->cell_up_vecs->at(i)->at(1) = local_up[1];
	this->cell_up_vecs->at(i)->at(2) = local_up[2];

	//debug the angle deviation
	//cout<<" "<<acos(lastDir[0]*local_dir[0] + lastDir[1]*local_dir[1] + lastDir[2]*local_dir[2] )<< ""<<endl;

//	double scalar = lastDir[0]*local_dir[0]+lastDir[1]*local_dir[1]+lastDir[2]*local_dir[2];
//	double a = sqrt(lastDir[0]*lastDir[0]+lastDir[1]*lastDir[1]+lastDir[2]*lastDir[2]);
//	double b = sqrt(local_dir[0]*local_dir[0] + local_dir[1]*local_dir[1]+local_dir[2]*local_dir[2]);
//	cout << scalar/(a*b) << endl;

/*	cerr<<"dir["<<i<<"]: "<<cell_dir_vecs->at(i)->at(0)<<" "<<cell_dir_vecs->at(i)->at(1)<<" "<<cell_dir_vecs->at(i)->at(2)<<endl;
	cerr<<"up["<<i<<"]: "<<cell_up_vecs->at(i)->at(0)<<" "<<cell_up_vecs->at(i)->at(1)<<" "<<cell_up_vecs->at(i)->at(2)<<endl;

	cerr<<anglePhi<<" "<<angleOmega<<" "<<anglePsi<<" "<<endl;
	cerr<<acos(lastDir[0]*local_dir[0] + lastDir[1]*local_dir[1] + lastDir[2]*local_dir[2] );
	if(abs(cell_dir_vecs->at(i)->at(2))>10e-6 ) {
		cerr<< cell_dir_vecs->at(i)->at(2)*cell_dir_vecs->at(i)->at(2) + cell_dir_vecs->at(i)->at(0)*cell_dir_vecs->at(i)->at(0)  +  cell_dir_vecs->at(i)->at(1)*cell_dir_vecs->at(i)->at(1) <<endl;
		exit(1);
	}
*/

}
