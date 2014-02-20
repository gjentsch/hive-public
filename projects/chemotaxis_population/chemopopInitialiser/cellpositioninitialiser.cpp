#include "cellpositioninitialiser.hh"

using namespace std;
using namespace Hive;
using namespace ChemoPop;

CellPositionInitialiser::CellPositionInitialiser() {  }

CellPositionInitialiser::~CellPositionInitialiser() { }

void CellPositionInitialiser::initAllInSamePosition(Agent *ag, double x, double y, double z, int num) {
	cerr << "entering initAllInSamePosition: " << x << " "  << y  << " " << z << " " << num << endl;
	Database *db = ag->getDatabase();
	IntegerData *cellnumber = (IntegerData* ) db->getDataItem("numberofcells");
	cellnumber->setInteger(num);
	TVectorData<TVectorData<double>* > *positions = (TVectorData<TVectorData<double>* >*) db->getDataItem("cellpositions");
	positions->reserveSize(num);

	this->cellnumber = num;

	for (int i=0; i<positions->size(); i++) {
		positions->at(i) = new TVectorData<double> ("vec", "tvector_double");
		positions->at(i)->reserveSize(3);
		positions->at(i)->at(0) = x;
		positions->at(i)->at(1) = y;
		positions->at(i)->at(2) = z;

	}
	this->doTheRest(ag);
}


void CellPositionInitialiser::initUniformDistAlongXAxis(Agent *ag, double minX, double maxX, int num) {
	cerr << "entering initUniformDistAlongXAxis: " << minX << " to "  << maxX  << " " << num << endl;
	if (minX>=maxX)
	{
		cerr<<" when init cell positions uniformly, maxX must be larger than minX."<<endl; exit(1);
	}
	Database *db = ag->getDatabase();
	IntegerData *cellnumber = (IntegerData* ) db->getDataItem("numberofcells");
	cellnumber->setInteger(num);
	TVectorData<TVectorData<double>* > *positions = (TVectorData<TVectorData<double>* >*) db->getDataItem("cellpositions");
	positions->reserveSize(num);
//	TVectorData<int> *cell_voxel_positions;
//	TVectorData<int> *number_cells_per_voxel;
//	Grid *g;
//	int voxel_index;
	this->cellnumber = num;


	for (int i=0; i<positions->size(); i++) {
		positions->at(i) = new TVectorData<double> ("vec", "tvector_double");
		positions->at(i)->reserveSize(3);

		//generate random x-position, and set the others to zero
		positions->at(i)->at(0) = Util::RANDOM_CLOSED()*(maxX-minX)+minX;
		positions->at(i)->at(1) = 0;
		positions->at(i)->at(2) = 0;
//		if (this->isCubeGrid) {
//			cell_voxel_positions->at(i) = voxel_index;
//			number_cells_per_voxel->at(voxel_index) = number_cells_per_voxel->at(voxel_index) + 1;
//		}
	}
	this->doTheRest(ag);
}



void CellPositionInitialiser::initRandomlyAroundPosition(Agent *ag, double x, double y, double z, double r, int num) {
	Database *db = ag->getDatabase();
	IntegerData *numcells = (IntegerData*) db->getDataItem("numberofcells");
	numcells->setInteger(num);
	TVectorData<TVectorData<double>* > *positions = (TVectorData<TVectorData<double>* >*) db->getDataItem("cellpositions");
	positions->reserveSize(num);
	TVectorData<int> *cell_voxel_positions;
	TVectorData<int> *number_cells_per_voxel;

	double posx, posy, posz;
	for (int i=0; i<positions->size(); i++) {
		do {
			posx = x-r+2*r*Util::RANDOM_CLOSED();
			posy = y-r+2*r*Util::RANDOM_CLOSED();
			posz = z-r+2*r*Util::RANDOM_CLOSED();
		} while (((posx-x)*(posx-x)+(posy-y)*(posy-y)+(posz-z)*(posz-z))>r*r);
		positions->at(i) = new TVectorData<double> ("vec", "tvector_double");
		positions->at(i)->reserveSize(3);
		positions->at(i)->at(0) = posx;
		positions->at(i)->at(1) = posy;
		positions->at(i)->at(2) = posz;

	}
	this->doTheRest(ag);
}

void CellPositionInitialiser::initFromFile(Agent *ag, ifstream& input) {
	// some local variables
	Database *db = ag->getDatabase();
	TVectorData<TVectorData<double>* > *positions = (TVectorData<TVectorData<double>* >*) db->getDataItem("cellpositions");


	double x,y,z;
	istringstream iss;
	string line = "";
	this->cellnumber = 0;
	// do the parsing of the input file
	while (getline(input,line)) {
		iss.clear();
		iss.str(line);
		iss >> x;
		iss >> y;
		iss >> z;
		TVectorData<double> *vec = new TVectorData<double>("position", "tvector_double");
		vec->reserveSize(3);
		vec->at(0) = x; vec->at(1) = y; vec->at(2) = z;
		positions->addElementToEnd(vec);

		this->cellnumber += 1;
	}
	this->doTheRest(ag);
}



void CellPositionInitialiser::doTheRest(Agent *ag) {
	/// obtain the database of the agent
	Database *db = ag->getDatabase();

	// get all vectors from the agent of which the size depends on the
	// number of cells in the system and set the size accordingly
	// swimmingstates
	if (db->doesDataItemExist("swimmingstates")) {
		TVectorData<int> *swimmingstates = (TVectorData<int> *) db->getDataItem("swimmingstates");
		swimmingstates->reserveSize(this->cellnumber);
		// do the initialisation
		for (int i=0; i<this->cellnumber; i++)
			swimmingstates->at(i) = 0;
	}
	// lastswimmingstates
	if (db->doesDataItemExist("lastswimmingstates")) {
		TVectorData<int> *lastswimmingstates = (TVectorData<int> *) db->getDataItem("lastswimmingstates");
		lastswimmingstates->reserveSize(this->cellnumber);
		// do the initialisation
		for (int i=0; i<this->cellnumber; i++)
			lastswimmingstates->at(i) = 0;
	}
	// cellnextturnangle
	if (db->doesDataItemExist("cellnextturnangle")) {
		TVectorData<double> *cellnextturnangle = (TVectorData<double>* ) db->getDataItem("cellnextturnangle");
		cellnextturnangle->reserveSize(this->cellnumber);
		// do the initialisation
		for (int i=0; i<this->cellnumber; i++)
			cellnextturnangle->at(i) = 0.0;
	}
	// desired_cell_consumption
	if (db->doesDataItemExist("desired_cell_consumption")) {
		TVectorData<double> *desired_cell_consumption = (TVectorData<double>* ) db->getDataItem("desired_cell_consumption");
		desired_cell_consumption->reserveSize(this->cellnumber);
		// do the initialisation
		for (int i=0; i<this->cellnumber; i++)
			desired_cell_consumption->at(i) = 0;
	}
	// celldirvecs
	if (db->doesDataItemExist("celldirvecs")) {
		TVectorData<TVectorData<double>* > *cell_dir_vecs =
				(TVectorData<TVectorData<double>* >*) db->getDataItem("celldirvecs");
		cell_dir_vecs->reserveSize(this->cellnumber);
		// do the initialisation
		for (int i=0; i<this->cellnumber; i++) {
			cell_dir_vecs->at(i) = new TVectorData<double> ("vec", "tvector_double");
			cell_dir_vecs->at(i)->reserveSize(3);
			cell_dir_vecs->at(i)->at(0) = 1;
			cell_dir_vecs->at(i)->at(1) = 0;
			cell_dir_vecs->at(i)->at(2) = 0;
		}
	}
	// up vector
	if (db->doesDataItemExist("cellupvecs")) {
		TVectorData<TVectorData<double>* > *cell_up_vecs =
				(TVectorData<TVectorData<double>* >*) db->getDataItem("cellupvecs");
		cell_up_vecs->reserveSize(this->cellnumber);
		// do the initialisation
		for (int i=0; i<this->cellnumber; i++) {
			cell_up_vecs->at(i) = new TVectorData<double> ("vec", "tvector_double");
			cell_up_vecs->at(i)->reserveSize(3);
			cell_up_vecs->at(i)->at(0) = 0;
			cell_up_vecs->at(i)->at(1) = 1;
			cell_up_vecs->at(i)->at(2) = 0;
		}
	}
	// cellspeed
	if (db->doesDataItemExist("cellspeeds")) {
		TVectorData<double> *cell_speeds = (TVectorData<double>*) db->getDataItem("cellspeeds");
		cell_speeds->reserveSize(this->cellnumber);
		// do the initialisation
		for (int i=0; i<this->cellnumber; i++)
			cell_speeds->at(i) = 20.0;
	}
	// cellnumber
	if (db->doesDataItemExist("numberofcells")) {
		IntegerData *cellnum = (IntegerData *) db->getDataItem("numberofcells");
		cellnum->setInteger(this->cellnumber);
	}
}

