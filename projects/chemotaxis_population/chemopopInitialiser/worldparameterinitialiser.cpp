/*
 * worldparameterinitialiser.cpp
 *
 *  Created on: 4 Aug 2013
 *      Author: Garrit
 */

#include "worldparameterinitialiser.hh"

using namespace std;
using namespace Hive;
using namespace ChemoPop;

WorldParameterData::WorldParameterData() {
	this->readrotdiffconst     = false;
	this->readdimension        = false;
	this->iscapillary_assay    = false;
	this->cellnumber           = 0;
	this->fileextension        = "hans";
	this->generatecelldata     = false;
	this->generatecellposition = false;
	this->blockworldoutput     = false;
	this->isbinary             = false;
	this->generateruntumble	   = false;
}

WorldParameterData::~WorldParameterData() {}


WorldParameterInitialiser::WorldParameterInitialiser(string inputFileName) {
	// set agent counter to correct number
	this->currentAgent = 0;
	this->wpd          = new WorldParameterData();

	// parse file
	this->parseFile(inputFileName);
}

WorldParameterInitialiser::~WorldParameterInitialiser() {

}


void WorldParameterInitialiser::parseFile(string inputFileName) {
	input.open(inputFileName.c_str());
    if (!input.is_open()) {
            throw HiveException("cannot open file named:'"+inputFileName+"' given to WorldParameterInitializer",
            		"void WorldParameterInitializer::parseFile(string inputfile)");
    }

    while(!input.eof()) {
    	getline(input,line);
        Util::trim(line);
        if (line.substr(0,1) != ";" && line != "") {
        	iss.clear();
        	iss.str(line);
        	iss >> type;

        	// check type and call the parsing methods
        	if (type == "$ROTDIF") {
        		this->parseRotDiff();
        	} else if (type == "$CELLPOSITIONINIT") {
        		this->parseCellPositions();
        	} else if (type == "$ENVIRONMENT") {
        		this->parseEnvironment();
        	} else if (type == "$CELLTYPE") {
        		this->parseCellType();
        	} else if (type == "$DIMENSION") {
        		this->parseDimension();
        	} else if (type == "$OUTPUTFLAGS") {
        		this->parseOutputFlags();
        	} else if (type == "$TIMEINFO") {
        		this->parseTimeInformation();
        	} else if (type == "$SEED") {
        		this->parseSeed();
        	}
        }
    }

    // check whether rot-diff constant has been read
    // if not set it to default value
    if (!this->wpd->readrotdiffconst)
    	this->wpd->rotdiffconst = 0.062;

    // check whether dimension has been given, if not set
    // it to default
    if (!this->wpd->readdimension)
    	this->wpd->dimension = "3D";

}


void WorldParameterInitialiser::setNextAgentParameters(Agent *ag) {
	cerr << "# WorldParameterInitialiser adding DataItems to Agent" << endl;

	// check whether cell numbers are compatible
	if (wpd->cellnumber != wpd->cellpositions.size()) {
		throw HiveException("# incompatible cell-number between CELLTYPE and CELLPOSITION",
				"# in WorldParameterInitialiser::setNextAgentParameters(Agent *ag)");
	}

	// get agent's database
	Database *db = ag->getDatabase();
	// swimmingstates
	TVectorData<int> *swimmingstates =  new TVectorData<int> ("swimmingstates","tvector_int");
	swimmingstates->reserveSize(wpd->cellnumber);
	// do the initialisation
	for (int i=0; i<this->wpd->cellnumber; i++)
		swimmingstates->at(i) = 0;
	db->addData(swimmingstates);

	// lastswimmingstates
	TVectorData<int> *lastswimmingstates = new TVectorData<int> ("lastswimmingstates","tvector_int");
	lastswimmingstates->reserveSize(this->wpd->cellnumber);
	// do the initialisation
	for (int i=0; i<this->wpd->cellnumber; i++)
		lastswimmingstates->at(i) = 0;
	db->addData(lastswimmingstates);

	// cellnextturnangle
	TVectorData<double> *cellnextturnangle =  new TVectorData<double> ("cellnextturnangle","tvector_double");
	cellnextturnangle->reserveSize(this->wpd->cellnumber);
	// do the initialisation
	for (int i=0; i<this->wpd->cellnumber; i++)
		cellnextturnangle->at(i) = 0.0;
	db->addData(cellnextturnangle);

	// desired_cell_consumption ... we can possibly remove this
	TVectorData<double> *desired_cell_consumption =  new TVectorData<double>("desired_cell_consumption", "tvector_double");
	desired_cell_consumption->reserveSize(this->wpd->cellnumber);
	// do the initialisation
	for (int i=0; i<this->wpd->cellnumber; i++)
		desired_cell_consumption->at(i) = 0;
	db->addData(desired_cell_consumption);

	// celldirvecs
	TVectorData<TVectorData<double>* > *cell_dir_vecs = new TVectorData<TVectorData<double>* > ("celldirvecs", "matrix_double");
	cell_dir_vecs->reserveSize(this->wpd->cellnumber);

	// do the initialisation
	for (int i=0; i<this->wpd->cellnumber; i++) {
		cell_dir_vecs->at(i) = new TVectorData<double> ("vec", "tvector_double");
		cell_dir_vecs->at(i)->reserveSize(3);
		cell_dir_vecs->at(i)->at(0) = 1;
		cell_dir_vecs->at(i)->at(1) = 0;
		cell_dir_vecs->at(i)->at(2) = 0;
	}
	db->addData(cell_dir_vecs);

	// up vector
	TVectorData<TVectorData<double>* > *cell_up_vecs = new TVectorData<TVectorData<double>* > ("cellupvecs","matrix_double");
	cell_up_vecs->reserveSize(this->wpd->cellnumber);
	// do the initialisation
	for (int i=0; i<this->wpd->cellnumber; i++) {
		cell_up_vecs->at(i) = new TVectorData<double> ("vec", "tvector_double");
		cell_up_vecs->at(i)->reserveSize(3);
		cell_up_vecs->at(i)->at(0) = 0;
		cell_up_vecs->at(i)->at(1) = 1;
		cell_up_vecs->at(i)->at(2) = 0;
	}
	db->addData(cell_up_vecs);

	// cellspeed
	TVectorData<double> *cell_speeds = new TVectorData<double>("cellspeeds","tvector_double");
	cell_speeds->reserveSize(this->wpd->cellnumber);
	// do the initialisation
	for (int i=0; i<this->wpd->cellnumber; i++)
		cell_speeds->at(i) = 20.0;
	db->addData(cell_speeds);

	// cellnumber
	IntegerData *cellnum = new IntegerData ("numberofcells",0);
	cellnum->setInteger(this->wpd->cellnumber);
	db->addData(cellnum);

	// cell positions
	TVectorData<TVectorData<double>* > *positions = new TVectorData<TVectorData<double>* >("cellpositions", "matrix_double");
	positions->reserveSize(this->wpd->cellnumber);
	for (int i=0; i<this->wpd->cellnumber; i++) {
		positions->at(i) = new TVectorData<double> ("name","type");
		positions->at(i)->reserveSize(3);
		positions->at(i)->at(0) = wpd->cellpositions[i][0];
		positions->at(i)->at(1) = wpd->cellpositions[i][1];
		positions->at(i)->at(2) = wpd->cellpositions[i][2];
	}
	db->addData(positions);

	// rotational diffusion coefficient
	DoubleData *rot_diff = new DoubleData("rotational_diffusion_constant", wpd->rotdiffconst);
	db->addData(rot_diff);

	// environment
	TVectorData<mu::Parser> *mu_environments = new TVectorData<mu::Parser> ("muparser_environments","tvector_muparser");
	mu_environments->reserveSize(wpd->environments.size());
	for (int i=0; i<mu_environments->size(); i++)
		mu_environments->at(i) = wpd->environments[i];
	db->addData(mu_environments);

	// dimension what is this needed for?
	BoolData *dimension = new BoolData("isIn2D",false);
	if(wpd->dimension == "2D" || wpd->dimension =="2d") {
		dimension->setBool(true);
	} else if (wpd->dimension =="3D" || wpd->dimension == "3d") {
		dimension->setBool(false);
	} else {
		throw HiveException("# the DIMENSION flag must be either 2D or 3D", "WorldParameterInitialiser::setNextAgentParameters");
	}
	db->addData(dimension);

	// number of steps
	double n  = wpd->endtime/wpd->dt;
	IntegerData *nsteps         = new IntegerData("num_steps", (int) n);
	// length of timestep
	DoubleData *timestep        = new DoubleData("dt", wpd->dt);
	// start time
	DoubleData *startt          = new DoubleData("localworldtime", 0.0);
	// equilibration time
	DoubleData *eqt             = new DoubleData("eqTime", wpd->equilibrationtime);
	// output interval
	DoubleData *output_interval = new DoubleData("output_interval",wpd->outputinterval);

	/// add them to the database
	db->addData(nsteps);
	db->addData(timestep);
	db->addData(startt);
	db->addData(eqt);
	db->addData(output_interval);

	// ------------------ Data Structures for the registrar --------------------------------
	// data structure for knowing which cell is stored at which index in the various vectors
	// that the world agent has
	// maps agent_id (key) on to vector index (value)
	MapIntIntData *local_index_map = new MapIntIntData("local_index_map");
	for (int i=0; i<this->wpd->cellnumber; i++) {
		//assume agent_ids start at 1 (because the world is agent zero)
		local_index_map->insert(i+1,i);
	}
	db->addData(local_index_map->getName(),local_index_map);

	// maps vector_index (key) onto agent_id
	// this is needed for outputing the positions of the cells
	MapIntIntData *index_agentid_map = new MapIntIntData("indexagentidmap");
	for (int i=0; i<this->wpd->cellnumber; i++) {
		// again, agent_ids = i+1
		index_agentid_map->insert(i,i+1);
	}
	db->addData(index_agentid_map->getName(), index_agentid_map);

	cerr << "# WorldParameterInitialiser done adding DataItems to Agent" << endl;
}

// needed by movement simulator
void WorldParameterInitialiser::parseRotDiff() {
	string value;
	if (iss.eof()) {
		getline(input,line);
		Util::trim(line);
		iss.clear();
		iss.str(line);
	}
	iss >> value;
	this->wpd->rotdiffconst     = atof(value.c_str());
	this->wpd->readrotdiffconst = true;
	cerr << "# WorldParameterInitialiser: read rotational diffusion constant of " << wpd->rotdiffconst << endl;
}

// this simply parse an expression ... the world
// would contain a vector of mu-parsers. these would be used
// by the world to calculate the concentration that the ligand
// sees at any point in time. hence we will need
// to update the WorldNotifyLigandToCellMessageGenerator.
// for setting this up compare with nonmemSubjectInitialiser in
// CTS-Hive branch. for the capillary assay: wouldn't it be easier
// to use the cylindrical coordinates for simulating the
// ligand concentration. add a part to the movement simulator
// to correct for the boundaries ... ah well, too much work.
void WorldParameterInitialiser::parseEnvironment() {
	string first;

	// read until the start of the data
	do {
		getline(input,line);
		Util::trim(line);
		iss.clear();
		iss.str(line);
		first.clear();
		iss >> first;
	} while(line.substr(0,1)==";" || first.empty() );

	if (first == "CAPILLARY_ASSAY") {
		wpd->iscapillary_assay = true;
	} else if (first == "MATH") {
		unsigned start = 4;
		unsigned end;
	    if( line.find(';',0)!=string::npos)
	    	end = line.find(';',0) - 1;
	    else
	    	end = line.size();
	    string expression = line.substr(start,end);
	    Util::trim(expression);
	    if (expression[expression.size()-1]=='\r')
	    	expression.erase(expression.size()-1);
	    wpd->environments.resize(wpd->environments.size()+1);
	    wpd->environments[wpd->environments.size()-1].SetExpr(expression);
	}

}

void WorldParameterInitialiser::parseCellPositions() {
	string first;

	// read until the start of the data
	do {
		getline(input,line);
		Util::trim(line);
		iss.clear();
		iss.str(line);
		first.clear();
		iss >> first;
	} while(line.substr(0,1)==";" || first.empty() );
	// check what kind of cell-position init is to be used
	if (first == "ALL_SAME_POSITION") {
		double x, y, z;
		int n;
		iss >> x; iss >> y; iss >> z; iss >> n;
		for (int i=0; i<n; i++) {
			vector<double> tmp(3);
			tmp[0] = x; tmp[1] = y; tmp[2] = z;
			wpd->cellpositions.push_back(tmp);
		}
	} else if (first == "AROUND_POSITION") {
		double x, y, z, r;
		double posx, posy, posz;
		int n;
		iss >> x; iss >> y; iss >> z; iss >> r; iss >> n;
		for (int i=0; i<n; i++) {
			do {
				posx = x-r+2*r*Util::RANDOM_CLOSED();
				posy = y-r+2*r*Util::RANDOM_CLOSED();
				posz = z-r+2*r*Util::RANDOM_CLOSED();
			} while (((posx-x)*(posx-x)+(posy-y)*(posy-y)+(posz-z)*(posz-z))>r*r);
			vector<double> tmp(3);
			tmp[0] = posx; tmp[1] = posy; tmp[2] = posz;
			wpd->cellpositions.push_back(tmp);
		}
	} else if (first == "INDIVIDUALLY") {
		size_t pos;
		while (true) {
			line.clear();
			getline(input, line);
			Util::trim(line);
			// check whether you have gone too far
			if (line.substr(0,1)=="$") {
				input.seekg(pos);
				break;
			}
			// check for comment or empty line
			iss.clear();
			iss.str(line);
			first.clear();
			iss >> first;
			if (line.substr(0,1)!=";" && !first.empty() ) {
				// triplets code for cell position
				vector<double> tmp (3);
				tmp[0] = atof(first.c_str());
				iss >> first;
				tmp[1] = atof(first.c_str());
				iss >> first;
				tmp[2] = atof(first.c_str());
				wpd->cellpositions.push_back(tmp);
			}
			pos = input.tellg();
		}
	}
}

// the world parses the celltype block in order to
// determine the number of cells there will be in the
// simulation. this is the only reason for doing so.
void WorldParameterInitialiser::parseCellType() {
	const char *delimeter = ",";
	size_t lastFound, nextFound;
	string first;
	string s;
	int pos;

	// first get the which modeltype we are dealing with.
	while (getline(input,line)) {
		Util::trim(line);
		if (line.substr(0,1) != ";" && line != "") {
			iss.clear();
			iss.str(line);
			iss >> first;
			Util::trim(first);
			break;
		}
	}

	int paratoreaduntilcellnumber;
	if (first == "WILLS_MODEL")
		paratoreaduntilcellnumber = 13;
	else if (first == "OLD_MODEL")
		paratoreaduntilcellnumber = 10;
	else if (first == "WILLS_MODEL_WITH_VAR_MOTOR")
		paratoreaduntilcellnumber = 16;
	else
		throw HiveException("do not understand celltype", "WorldParameterInitialiser::parseCellType()");

	// now we need to find out, how many cells there are going to be.
	while (true) {
		line.clear();
		getline(input, line);
		Util::trim(line);
		// check whether you have gone too far
		if (line.substr(0,1)=="$") {
			input.seekg(pos);
			break;
		}
		iss.clear();
		iss.str(line);
		first.clear();
		iss >> first;
		if (line.substr(0,1)!=";" && !first.empty() ) {
			// set up reading the file
			lastFound=line.find(delimeter);
			nextFound=0;
			s = line.substr(0,lastFound); Util::trim(s);
			// fast forward to time
			for (int i=0; i<paratoreaduntilcellnumber; i++) {
				nextFound = line.find(delimeter,lastFound+1);
				s = line.substr(lastFound+1,nextFound-(lastFound+1));
				Util::trim(s);
				lastFound=nextFound;
			}
			wpd->cellnumber += atoi(s.c_str());
		}
		// remember position
		pos = input.tellg();
	}
}


// adds dimension to data. will be needed for setting
// up cell positions vector dimensions is suppose. not sure.
void WorldParameterInitialiser::parseDimension() {
	string value;
	if (iss.eof()) {
		getline(input,line);
		Util::trim(line);
		iss.clear();
		iss.str(line);
	}
	iss >> value;
	this->wpd->dimension = value;
}

void WorldParameterInitialiser::parseSeed() {
	string value;
	if (iss.eof()) {
		getline(input, line);
		Util::trim(line);
		iss.clear();
		iss.str(line);
	}
	iss >> value;
	Util::SEED_RANDOM(atoi(value.c_str()));
}


void WorldParameterInitialiser::parseTimeInformation() {
	string value;
	if (iss.eof()) {
		getline(input,line);
		Util::trim(line);
		iss.clear();
		iss.str(line);
	}
	double d;
	iss >> d;
	this->wpd->endtime = d;
	iss >> d;
	this->wpd->dt = d;
	iss >> d;
	this->wpd->starttime = d;
	iss >> d;
	this->wpd->equilibrationtime = d;
	iss >> d;
	this->wpd->outputinterval = d;

}

// reading the file extension name, binary flag, and blockworldoutput flag
// also run-tumble output; cell-position; and cell-data output
void WorldParameterInitialiser::parseOutputFlags() {
	string key;
	string value;
	size_t pos;

	while(true) {
		getline(input,line);
		Util::trim(line);
		if (line.substr(0,1)=="$") {
			input.seekg(pos);
			break;
		}
		iss.clear();
		iss.str(line);
		iss >> key;
		if (line.substr(0,1)!=";" && !key.empty() ) {
			iss >> value;
			if (key == "SUFFIX")
				wpd->fileextension = value;
			else if (key == "BINARY") {
				if (value == "true" || value == "TRUE")
					wpd->isbinary = true;
				else
					wpd->isbinary = false;
			} else if (key == "BLOCKWORLDOUTPUT") {
				if (value == "true" || value == "TRUE")
					wpd->blockworldoutput = true;
			} else if (key == "RUNTUMBLEOUTPUT") {
				if (value == "true" || value == "TRUE")
					wpd->generateruntumble= true;
			} else if (key == "CELLPOSITIONOUTPUT") {
				if (value == "true" || value == "TRUE")
					wpd->generatecellposition = true;
			} else if (key == "CELLDATA") {
				if (value == "true" || value == "TRUE")
					wpd->generatecelldata = true;
			} else
				throw HiveException("# don't understand value given in parseOutputFlags",
						"# WorldParaMeterInitialiser::parseOutputFlags");
		}
		pos = input.tellg();
	}
}

