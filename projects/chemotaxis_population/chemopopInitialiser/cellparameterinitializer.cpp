/*
 * cellparameterinitializer.cpp
 *
 *  Created on: Apr 19, 2010
 *      Author: msneddon
 */

#include "cellparameterinitializer.hh"


using namespace std;
using namespace Hive;
using namespace ChemoPop;

// init e.coli cells
CellParameterInitializer::CellParameterInitializer(string inputFileName)
{
	this->currentAgent = 0;

	// some local variables
	string line;
	string type;
	string first;
	istringstream iss;

	// open up the file
	ifstream input(inputFileName.c_str());
	if (!input.is_open()) {
		throw HiveException("cannot open file named:'"+inputFileName+"' given to CellParameterInitializer","CellParameterInitializer::CellParameterInitializer");
	}

	// read as long as you encounter the type
//	while (getline(input,line)) {
//		Util::trim(line);
//		if (line.substr(0,1) != "#" && line != "") {
//			iss.clear();
//			iss.str(line);
//			iss >> type;
//			break;
//		}
//	}

	// read until the celltype block is encountered
	while (getline(input,line)) {
		Util::trim(line);
		iss.clear();
		iss.str(line);
		iss >> first;
		if (first == "$CELLTYPE")
			break;
	}

	// read until celltype has been read
	while (getline(input,line)) {
			Util::trim(line);
			if (line.substr(0,1) != ";" && line != "") {
				iss.clear();
				iss.str(line);
				iss >> type;
				Util::trim(type);
				break;
			}
		}

	cerr<<"# detected ecoli cell type as model: '" <<type<<"'"<<endl;

	// evaluate which type is called for and call the respective function
	if (type == "OLD_MODEL") {
		// set flag
		this->is_wills_methylation_model.setBool(false);
		// call parsing method
		this->parseFileOldModel(input);
	} else if (type == "WILLS_MODEL") {
		// set flag
		this->is_wills_methylation_model.setBool(true);
		// call parsing method
		this->parseFileWillsModel(input);
	} else if (type == "WILLS_MODEL_WITH_VAR_MOTOR") {
		// set flag
		this->is_wills_methylation_model.setBool(true);
		// call parsing method
		this->parseFileWillsModelWithVariableMotors(input);
	} else {
		// throw exception as type was not understood
		throw HiveException("unknown cell type of the ecoli given to CellParameterInitializer","CellParameterInitializer::CellParameterInitializer");
	}
}

CellParameterInitializer::~CellParameterInitializer()
{
	cells.clear();
	for (unsigned int i=0; i<uniqueCPDobjects.size(); i++) {
		delete uniqueCPDobjects[i];
		uniqueCPDobjects[i] = NULL;
	}
	uniqueCPDobjects.clear();
}

void CellParameterInitializer::parseFileOldModel(ifstream& distFile) {
	// flags for the parsing
	bool outputLines = false;
	bool outputParse = false;
	size_t pos;

	cout<<"# Parsing input file in CellparameterInitializer::parseFileOldModel ...\n";
	try {
		const char* delimeter = ",";
		string line; int lineCounter=0;
		size_t lastFound=0, nextFound=0;

		//Then we loop through the file and read / parse each element as a double valued
		//number.  As we read them in, we'll immediately dump the double to the output
		//file stream.
		while (true) {
			getline(distFile,line);
			if (line.substr(0,1)=="$") {
						distFile.seekg(pos);
						break;
			}
			if(outputLines) cout<<"["<<lineCounter<<"]: "<<line<<"\n";
			lineCounter++;
			if(line.size()==0) continue;
			Util::trim(line);

			//skip comments
			if(line.at(0)==';'||line.at(0)=='%') continue;

			// local values saved as we read the file
			double meanCheYp;
			double tauAdapt;
			double tauNoise;
			double noise;
			int numOfMotors;
			bool isCoordinated;
			int flagellaModel;
			int minFlagellaToRun;
			double semiWaitingTime;
			double cellSpeed;
			double motorG;
			int numOfCells;

			//CheY-P level
			lastFound=line.find(delimeter); nextFound=0;
			string s = line.substr(0,lastFound); Util::trim(s);
			meanCheYp = Util::convertToDouble(s);
			if(outputParse) cerr<<"  Mean [CheYp] :: "<<meanCheYp<<endl;

			// Tau
			nextFound = line.find(delimeter,lastFound+1);
			s = line.substr(lastFound+1,nextFound-(lastFound+1)); Util::trim(s);
			tauAdapt = Util::convertToDouble(s);
			lastFound=nextFound;
			if(outputParse) cerr<<"  tau adapt :: "<<tauAdapt<<endl;

			nextFound = line.find(delimeter,lastFound+1);
			s = line.substr(lastFound+1,nextFound-(lastFound+1)); Util::trim(s);
			tauNoise = Util::convertToDouble(s);
			lastFound=nextFound;
			if(outputParse) cerr<<"  tau noise :: "<<tauNoise<<endl;

			// Noise
			nextFound = line.find(delimeter,lastFound+1);
			s = line.substr(lastFound+1,nextFound-(lastFound+1)); Util::trim(s);
			noise = Util::convertToDouble(s);
			lastFound=nextFound;
			if(outputParse) cerr<<"  noise :: "<<noise<<endl;

			//numOfMotors
			nextFound = line.find(delimeter,lastFound+1);
			s = line.substr(lastFound+1,nextFound-(lastFound+1)); Util::trim(s);
			numOfMotors = Util::convertToInt(s);
			lastFound=nextFound;
			if(outputParse) cerr<<"  numOfMotors :: "<<numOfMotors<<endl;

			//isCoordinated
			nextFound = line.find(delimeter,lastFound+1);
			s = line.substr(lastFound+1,nextFound-(lastFound+1)); Util::trim(s);
			if(Util::convertToInt(s)==0) isCoordinated = false; else isCoordinated = true;
			lastFound=nextFound;
			if(outputParse) cerr<<"  isCoordinated :: "<<isCoordinated<<endl;

			//flagellaModel
			nextFound = line.find(delimeter,lastFound+1);
			s = line.substr(lastFound+1,nextFound-(lastFound+1)); Util::trim(s);
			flagellaModel = Util::convertToInt(s);
			lastFound=nextFound;
			if(outputParse) cerr<<"  flagellaModel :: "<<flagellaModel<<endl;

			//minFlagellaToRun
			nextFound = line.find(delimeter,lastFound+1);
			s = line.substr(lastFound+1,nextFound-(lastFound+1)); Util::trim(s);
			minFlagellaToRun = Util::convertToInt(s);
			lastFound=nextFound;
			if(outputParse) cerr<<"  minFlagellaToRun :: "<<minFlagellaToRun<<endl;

			//semiWaitingTime
			nextFound = line.find(delimeter,lastFound+1);
			s = line.substr(lastFound+1,nextFound-(lastFound+1)); Util::trim(s);
			semiWaitingTime = Util::convertToDouble(s);
			lastFound=nextFound;
			if(outputParse) cerr<<"  semiWaitingTime :: "<<semiWaitingTime<<endl;

			//cellSpeed
			nextFound = line.find(delimeter,lastFound+1);
			s = line.substr(lastFound+1,nextFound-(lastFound+1)); Util::trim(s);
			cellSpeed = Util::convertToDouble(s);
			lastFound=nextFound;
			if(outputParse) cerr<<"  cellSpeed :: "<<semiWaitingTime<<endl;

			// number of Cells
			nextFound = line.find(delimeter,lastFound+1);
			s = line.substr(lastFound+1,nextFound-(lastFound+1)); Util::trim(s);
			numOfCells = Util::convertToInt(s);
			lastFound=nextFound;
			if(outputParse) cerr<<"  numOfCells :: "<<numOfCells<<endl;

			//Motor steepness
			nextFound = line.find(delimeter,lastFound+1);
			s = line.substr(lastFound+1,nextFound-(lastFound+1)); Util::trim(s);
			motorG = Util::convertToDouble(s);
			lastFound=nextFound;
			if(outputParse) cerr<<" motor parameter_g1: "<<motorG<<endl;

			// isChemotactic = if
			nextFound = line.find(delimeter,lastFound+1);
			int isChemotactic = 0;
			if(string::npos!=nextFound)
			{
				s = line.substr(lastFound+1,nextFound-(lastFound+1)); Util::trim(s);
				isChemotactic = Util::convertToInt(s);
				lastFound=nextFound;
				if(outputParse) {
					if(isChemotactic==0) cerr<<"  chemotaxis is off"<<endl;
					else if(isChemotactic==1) cerr<<"  chemotaxis is on"<<endl;
					else {
						cerr<<"  error!  I don't know whether chemotaxis should be on or off!"<<endl;
						cerr<<"  so by default, chemotaxis is on."<<endl;
						isChemotactic = 1;
					}
				}
			} else {
				isChemotactic = 1;
				if(outputParse) cout<<"  by default, chemotaxis is on :: "<<endl;
			}


			// Now create a cell parameter data object with the new information
			CellParameterData *cpd = new CellParameterData();
			cpd->cheyp_mean = meanCheYp; //cpd->meanCheYp = meanCheYp;  //change to cpd->cheyp_mean
			cpd->tauAdapt = tauAdapt;
			cpd->tauNoise = tauNoise;
			cpd->sigma = noise;
			cpd->n_motors = numOfMotors;
			cpd->isFlagellaCoordinated = isCoordinated;
			cpd->flagellaModelType = flagellaModel;
			cpd->minNumberOfMotorsInCCWneededToSwim = minFlagellaToRun;
			cpd->minTimeInSemi = semiWaitingTime;
			cpd->speed = cellSpeed;
			cpd->motor_g1 = motorG;
			cpd->isChemotactic = isChemotactic;

			//save the info for each cell in the cells array - no need to recreate
			//a cpd for each cell, just pass a pointer
			uniqueCPDobjects.push_back(cpd);
			for(int k=0; k<numOfCells; k++) {
				cells.push_back(cpd);
			}
			// remember position
			pos = distFile.tellg();
		}
	} catch (Hive::HiveException& e) {
		e.addTraceException("SimpleCellDefaultComposer::parseInputFile(string inputFileName)");
		throw(e);
	} catch (...) {
		throw(Hive::HiveException("Error in reading or processing input file ",
				"SimpleCellDefaultComposer::parseInputFile(string inputFileName)"));
	}
	this->number_agents = cells.size();
}

void CellParameterInitializer::parseFileWillsModel(ifstream& distFile) {
	// flags for the parsing
	bool outputLines = false;
	bool outputParse = false;
	size_t pos;

	cerr << "# Parsing input file in CellParameterInitializer::parseFileWillsModel..." << endl;
	try {
		//Open the population file and create the cells
		const char* delimeter = ",";
		string line; int lineCounter=0;
		size_t lastFound=0, nextFound=0;

		//Then we loop through the file and read / parse each element as a double valued
		//number.  As we read them in, we'll immediately dump the double to the output
		//file stream.
		while (true) {
			getline(distFile,line);
			if (line.substr(0,1)=="$") {
				distFile.seekg(pos);
				break;
			}
			if(outputLines) cout<<"["<<lineCounter<<"]: "<<line<<"\n";
			lineCounter++;
			if(line.size()==0) continue;
			Util::trim(line);

			//skip comments
			if(line.at(0)==';'||line.at(0)=='%') continue;

			// local values saved as we read the file
			double kr;
			double Kr;
			double kb;
			double Kb;
			double noise_parameter;
			double meth_init;
			double cheyp_mean;
			int numOfMotors;
			bool isCoordinated;
			int flagellaModel;
			int minFlagellaToRun;
			double semiWaitingTime;
			double cellSpeed;
			int numOfCells;

			// kr
			lastFound=line.find(delimeter); nextFound=0;
			string s = line.substr(0,lastFound); Util::trim(s);
			kr = Util::convertToDouble(s);
			if(outputParse) cerr<<"  kr :: "<< kr <<endl;

			// Kr
			nextFound=line.find(delimeter, lastFound+1);
			s = line.substr(lastFound+1,nextFound-(lastFound+1)); Util::trim(s);
			Kr = Util::convertToDouble(s);
			lastFound = nextFound;
			if(outputParse) cerr<<"  Kr :: "<< Kr <<endl;

			// kb
			nextFound=line.find(delimeter, lastFound+1);
			s = line.substr(lastFound+1,nextFound-(lastFound+1)); Util::trim(s);
			kb = Util::convertToDouble(s);
			lastFound = nextFound;
			if(outputParse) cerr<<"  kb :: "<< kb <<endl;

			// Kb
			nextFound = line.find(delimeter,lastFound+1);
			s = line.substr(lastFound+1,nextFound-(lastFound+1)); Util::trim(s);
			Kb = Util::convertToDouble(s);
			lastFound=nextFound;
			if(outputParse) cerr<<"  Kb :: "<< Kb <<endl;

			// Noise parameter
			nextFound = line.find(delimeter,lastFound+1);
			s = line.substr(lastFound+1,nextFound-(lastFound+1)); Util::trim(s);
			noise_parameter = Util::convertToDouble(s);
			lastFound=nextFound;
			if(outputParse) cerr<<"  noise_parameter :: "<< noise_parameter <<endl;

			// meth_init
			nextFound = line.find(delimeter, lastFound+1);
			s = line.substr(lastFound+1, nextFound-(lastFound+1)); Util::trim(s);
			meth_init = Util::convertToDouble(s);
			lastFound = nextFound;
			if (outputParse) cerr << "  meth_init :: " << meth_init << endl;

			// mean cheyp
			nextFound = line.find(delimeter, lastFound+1);
			s = line.substr(lastFound+1, nextFound-(lastFound+1)); Util::trim(s);
			cheyp_mean = Util::convertToDouble(s);
			lastFound = nextFound;
			if(outputParse) cerr << " cheyp_mean :: " << cheyp_mean << endl;

			//numOfMotors
			nextFound = line.find(delimeter,lastFound+1);
			s = line.substr(lastFound+1,nextFound-(lastFound+1)); Util::trim(s);
			numOfMotors = Util::convertToInt(s);
			lastFound=nextFound;
			if(outputParse) cerr<<"  numOfMotors :: "<<numOfMotors<<endl;

			//isCoordinated
			nextFound = line.find(delimeter,lastFound+1);
			s = line.substr(lastFound+1,nextFound-(lastFound+1)); Util::trim(s);
			if(Util::convertToInt(s)==0) isCoordinated = false; else isCoordinated = true;
			lastFound=nextFound;
			if(outputParse) cerr<<"  isCoordinated :: "<<isCoordinated<<endl;

			//flagellaModel
			nextFound = line.find(delimeter,lastFound+1);
			s = line.substr(lastFound+1,nextFound-(lastFound+1)); Util::trim(s);
			flagellaModel = Util::convertToInt(s);
			lastFound=nextFound;
			if(outputParse) cerr<<"  flagellaModel :: "<<flagellaModel<<endl;

			//minFlagellaToRun
			nextFound = line.find(delimeter,lastFound+1);
			s = line.substr(lastFound+1,nextFound-(lastFound+1)); Util::trim(s);
			minFlagellaToRun = Util::convertToInt(s);
			lastFound=nextFound;
			if(outputParse) cerr<<"  minFlagellaToRun :: "<<minFlagellaToRun<<endl;

			//semiWaitingTime
			nextFound = line.find(delimeter,lastFound+1);
			s = line.substr(lastFound+1,nextFound-(lastFound+1)); Util::trim(s);
			semiWaitingTime = Util::convertToDouble(s);
			lastFound=nextFound;
			if(outputParse) cerr<<"  semiWaitingTime :: "<<semiWaitingTime<<endl;

			//cellSpeed
			nextFound = line.find(delimeter,lastFound+1);
			s = line.substr(lastFound+1,nextFound-(lastFound+1)); Util::trim(s);
			cellSpeed = Util::convertToDouble(s);
			lastFound=nextFound;
			if(outputParse) cerr<<"  cellSpeed :: "<<cellSpeed<<endl;

			// number of Cells
			nextFound = line.find(delimeter,lastFound+1);
			s = line.substr(lastFound+1,nextFound-(lastFound+1)); Util::trim(s);
			numOfCells = Util::convertToInt(s);
			lastFound=nextFound;
			if(outputParse) cerr<<"  numOfCells :: "<<numOfCells<<endl;

			// isChemotactic = if
			nextFound = line.find(delimeter,lastFound+1);
			int isChemotactic = 0;
			if(string::npos!=nextFound)
			{
				s = line.substr(lastFound+1,nextFound-(lastFound+1)); Util::trim(s);
				isChemotactic = Util::convertToInt(s);
				lastFound=nextFound;
				if(outputParse) {
					if(isChemotactic==0) cerr<<"  chemotaxis is off"<<endl;
					else if(isChemotactic==1) cerr<<"  chemotaxis is on"<<endl;
					else {
						cerr<<"  error!  I don't know whether chemotaxis should be on or off!"<<endl;
						cerr<<"  so by default, chemotaxis is on."<<endl;
						isChemotactic = 1;
					}
				}
			} else {
				isChemotactic = 1;
				if(outputParse) cout<<"  by default, chemotaxis is on :: "<<endl;
			}


			// Now create a cell parameter data object with the new information
			CellParameterData *cpd = new CellParameterData();
			cpd->kr = kr;
			cpd->Kr = Kr;
			cpd->kb = kb;
			cpd->Kb = Kb;
			cpd->noise_parameter = noise_parameter;
			cpd->meth_init = meth_init;
			cpd->cheyp_mean = cheyp_mean;
			cpd->n_motors = numOfMotors;
			cpd->isFlagellaCoordinated = isCoordinated;
			cpd->flagellaModelType = flagellaModel;
			cpd->minNumberOfMotorsInCCWneededToSwim = minFlagellaToRun;
			cpd->minTimeInSemi = semiWaitingTime;
			cpd->speed = cellSpeed;
			cpd->isChemotactic = isChemotactic;

			//save the info for each cell in the cells array - no need to recreate
			//a cpd for each cell, just pass a pointer
			uniqueCPDobjects.push_back(cpd);
			for(int k=0; k<numOfCells; k++) {
				cells.push_back(cpd);
			}
			pos = distFile.tellg();
		} // while

	} catch (Hive::HiveException& e) {
		e.addTraceException("CellParameterInitializer::parseFileNewModel(string inputFileName)");
		throw(e);
	} catch (...) {
		throw(Hive::HiveException("Error in reading or processing input file ",
				"CellParameterInitializer::parseFileNewModelstring inputFileName)"));
	}
	this->number_agents = cells.size();
}


void CellParameterInitializer::parseFileWillsModelWithVariableMotors(ifstream& distFile) {
	// flags for the parsing
	bool outputLines = false;
	bool outputParse = false;
	size_t pos;

    cerr << "# Parsing input file in CellParameterInitializer::parseFileWillsModelWithVariableMotors..." << endl;
	try {
		//Open the population file and create the cells
		const char* delimeter = ",";
		string line; int lineCounter=0;
		size_t lastFound=0, nextFound=0;

		//Then we loop through the file and read / parse each element as a double valued
		//number.  As we read them in, we'll immediately dump the double to the output
		//file stream.
		while (true) {
			getline(distFile,line);
			if (line.substr(0,1)=="$") {
				distFile.seekg(pos);
				break;
			}
			if(outputLines) cout<<"["<<lineCounter<<"]: "<<line<<"\n";
			lineCounter++;
			if(line.size()==0) continue;
			Util::trim(line);

			//skip comments
			if(line.at(0)==';'||line.at(0)=='%') continue;

			// local values saved as we read the file
			double kr;
			double Kr;
			double kb;
			double Kb;
			double noise_parameter;
			double meth_init;
			double cheyp_mean;

			int numOfMotors;
			double motorKd;
			double motorW;
			double motorG;
			bool isCoordinated;

			int flagellaModel;
			int minFlagellaToRun;
			double semiWaitingTime;

			double cellSpeed;
			int numOfCells;

			// kr
			lastFound=line.find(delimeter); nextFound=0;
			string s = line.substr(0,lastFound); Util::trim(s);
			kr = Util::convertToDouble(s);
			if(outputParse) cerr<<"  kr :: "<< kr <<endl;

			// Kr
			nextFound=line.find(delimeter, lastFound+1);
			s = line.substr(lastFound+1,nextFound-(lastFound+1)); Util::trim(s);
			Kr = Util::convertToDouble(s);
			lastFound = nextFound;
			if(outputParse) cerr<<"  Kr :: "<< Kr <<endl;

			// kb
			nextFound=line.find(delimeter, lastFound+1);
			s = line.substr(lastFound+1,nextFound-(lastFound+1)); Util::trim(s);
			kb = Util::convertToDouble(s);
			lastFound = nextFound;
			if(outputParse) cerr<<"  kb :: "<< kb <<endl;

			// Kb
			nextFound = line.find(delimeter,lastFound+1);
			s = line.substr(lastFound+1,nextFound-(lastFound+1)); Util::trim(s);
			Kb = Util::convertToDouble(s);
			lastFound=nextFound;
			if(outputParse) cerr<<"  Kb :: "<< Kb <<endl;

			// Noise parameter
			nextFound = line.find(delimeter,lastFound+1);
			s = line.substr(lastFound+1,nextFound-(lastFound+1)); Util::trim(s);
			noise_parameter = Util::convertToDouble(s);
			lastFound=nextFound;
			if(outputParse) cerr<<"  noise_parameter :: "<< noise_parameter <<endl;

			// meth_init
			nextFound = line.find(delimeter, lastFound+1);
			s = line.substr(lastFound+1, nextFound-(lastFound+1)); Util::trim(s);
			meth_init = Util::convertToDouble(s);
			lastFound = nextFound;
			if (outputParse) cerr << "  meth_init :: " << meth_init << endl;

			// mean cheyp
			nextFound = line.find(delimeter, lastFound+1);
			s = line.substr(lastFound+1, nextFound-(lastFound+1)); Util::trim(s);
			cheyp_mean = Util::convertToDouble(s);
			lastFound = nextFound;
			if(outputParse) cerr <<"  cheyp_mean :: " << cheyp_mean << endl;

			//numOfMotors
			nextFound = line.find(delimeter,lastFound+1);
			s = line.substr(lastFound+1,nextFound-(lastFound+1)); Util::trim(s);
			numOfMotors = Util::convertToInt(s);
			lastFound=nextFound;
			if(outputParse) cerr<<"  numOfMotors :: "<<numOfMotors<<endl;

			//omega motor parameter
			nextFound = line.find(delimeter,lastFound+1);
			s = line.substr(lastFound+1,nextFound-(lastFound+1)); Util::trim(s);
			motorKd = Util::convertToDouble(s);
			lastFound=nextFound;
			if(outputParse) cerr<<"  motorParamater_Kd :: "<<motorKd<<endl;

			//omega motor parameter
			nextFound = line.find(delimeter,lastFound+1);
			s = line.substr(lastFound+1,nextFound-(lastFound+1)); Util::trim(s);
			motorW = Util::convertToDouble(s);
			lastFound=nextFound;
			if(outputParse) cerr<<"  motorParamater_w :: "<<motorW<<endl;

			//g motor parameter
			nextFound = line.find(delimeter,lastFound+1);
			s = line.substr(lastFound+1,nextFound-(lastFound+1)); Util::trim(s);
			motorG = Util::convertToDouble(s);
			lastFound=nextFound;
			if(outputParse) cerr<<"  motorParamater_g :: "<<motorG<<endl;

			//isCoordinated
			nextFound = line.find(delimeter,lastFound+1);
			s = line.substr(lastFound+1,nextFound-(lastFound+1)); Util::trim(s);
			if(Util::convertToInt(s)==0) isCoordinated = false; else isCoordinated = true;
			lastFound=nextFound;
			if(outputParse) cerr<<"  isCoordinated :: "<<isCoordinated<<endl;

			//flagellaModel
			nextFound = line.find(delimeter,lastFound+1);
			s = line.substr(lastFound+1,nextFound-(lastFound+1)); Util::trim(s);
			flagellaModel = Util::convertToInt(s);
			lastFound=nextFound;
			if(outputParse) cerr<<"  flagellaModel :: "<<flagellaModel<<endl;

			//minFlagellaToRun
			nextFound = line.find(delimeter,lastFound+1);
			s = line.substr(lastFound+1,nextFound-(lastFound+1)); Util::trim(s);
			minFlagellaToRun = Util::convertToInt(s);
			lastFound=nextFound;
			if(outputParse) cerr<<"  minFlagellaToRun :: "<<minFlagellaToRun<<endl;

			//semiWaitingTime
			nextFound = line.find(delimeter,lastFound+1);
			s = line.substr(lastFound+1,nextFound-(lastFound+1)); Util::trim(s);
			semiWaitingTime = Util::convertToDouble(s);
			lastFound=nextFound;
			if(outputParse) cerr<<"  semiWaitingTime :: "<<semiWaitingTime<<endl;

			//cellSpeed
			nextFound = line.find(delimeter,lastFound+1);
			s = line.substr(lastFound+1,nextFound-(lastFound+1)); Util::trim(s);
			cellSpeed = Util::convertToDouble(s);
			lastFound=nextFound;
			if(outputParse) cerr<<"  cellSpeed :: "<<cellSpeed<<endl;

			// number of Cells
			nextFound = line.find(delimeter,lastFound+1);
			s = line.substr(lastFound+1,nextFound-(lastFound+1)); Util::trim(s);
			numOfCells = Util::convertToInt(s);
			lastFound=nextFound;
			if(outputParse) cerr<<"  numOfCells :: "<<numOfCells<<endl;

			// isChemotactic = if
			nextFound = line.find(delimeter,lastFound+1);
			int isChemotactic = 0;
			if(string::npos!=nextFound) {
				s = line.substr(lastFound+1,nextFound-(lastFound+1)); Util::trim(s);
				isChemotactic = Util::convertToInt(s);
				lastFound=nextFound;
				if(outputParse) {
					if(isChemotactic==0) cerr<<"  chemotaxis is off"<<endl;
					else if(isChemotactic==1) cerr<<"  chemotaxis is on"<<endl;
					else {
						cerr<<"  error!  I don't know whether chemotaxis should be on or off!"<<endl;
						cerr<<"  so by default, chemotaxis is on."<<endl;
						isChemotactic = 1;
					}
				}
			} else {
				isChemotactic = 1;
				if(outputParse) cerr<<"  by default, chemotaxis is on :: "<<endl;
			}

			// Now create a cell parameter data object with the new information
			CellParameterData *cpd = new CellParameterData();
			cpd->kr = kr;
			cpd->Kr = Kr;
			cpd->kb = kb;
			cpd->Kb = Kb;
			cpd->noise_parameter = noise_parameter;
			cpd->meth_init = meth_init;
			cpd->cheyp_mean = cheyp_mean;
			cpd->n_motors = numOfMotors;
			cpd->motor_Kd = motorKd;
			cpd->motor_w = motorW;
			cpd->motor_g1 = motorG;
			cpd->isFlagellaCoordinated = isCoordinated;
			cpd->flagellaModelType = flagellaModel;
			cpd->minNumberOfMotorsInCCWneededToSwim = minFlagellaToRun;
			cpd->minTimeInSemi = semiWaitingTime;
			cpd->speed = cellSpeed;
			cpd->isChemotactic = isChemotactic;

			//save the info for each cell in the cells array - no need to recreate
			//a cpd for each cell, just pass a pointer
			uniqueCPDobjects.push_back(cpd);
			for(int k=0; k<numOfCells; k++) {
				cells.push_back(cpd);
			}
			pos = distFile.tellg();
		}
	} catch (Hive::HiveException& e) {
		e.addTraceException("CellParameterInitializer::parseFileNewModel(string inputFileName)");
		throw(e);
	} catch (...) {
		throw(Hive::HiveException("Error in reading or processing input file ",
				"CellParameterInitializer::parseFileNewModelstring inputFileName)"));
	}
	this->number_agents = cells.size(); // these are not the number of agents (cells) but the number of lines describing
									    // a population of identical cells each
}



double CellParameterInitializer::getEqTime() const { return CellParameterData::eqTime; }


double CellParameterInitializer::getOutputInterval() const { return CellParameterData::outputInterval; }


void CellParameterInitializer::setNextAgentParameters(Agent *cell)
{
	if(currentAgent>=this->number_agents) {
		throw HiveException("Cannot set parameters for another cell, as all cells that have been specified are already created.",
								"CellParameterInitializer::setNextCellParameters(Agent *cell)");
	}
	CellParameterData *cpd = this->cells.at(currentAgent);
	Database *db = cell->getDatabase();

	// add flag about methylation model
	BoolData *is_wills_methylation_model = new Hive::BoolData("is_wills_methylation_model",
				this->is_wills_methylation_model.getBool());
	db->addData(is_wills_methylation_model);


	// add special cell accumulated data
	TVectorData<double> *cell_acc_ligand = new TVectorData<double> ("cell_acc_ligand","tvector_double");
	cell_acc_ligand->reserveSize(1);
	for(int al=0; al<1; al++) {
		cell_acc_ligand->at(al)=0;
	}
	db->addData(cell_acc_ligand);

	// Set whether or not the cell is chemotactic
	IntegerData *isChemotacticData = new Hive::IntegerData("isChemotactic",cpd->isChemotactic);
	db->addData(isChemotacticData->getName(),isChemotacticData);

	// Set the swimming characteristics of the cell
	Data *speedData = new Hive::DoubleData("CellSpeed",cpd->speed);
	db->addData(speedData->getName(),speedData);

	// right now, for simplicity, all cells have the same rotational diffusion constant, so this
	// is not a cell parameter
	//Data *rotDiffConstData = new Hive::DoubleData("RotDiffConst",cpd->rotDiffConst);
	//db->addData(rotDiffConstData->getName(),rotDiffConstData);

	// Set motor parameters
	Data *n_motor_Data = new Hive::IntegerData("n_motors",cpd->n_motors);
	db->addData(n_motor_Data->getName(),n_motor_Data);

	Data *flagellaModelType_Data = new Hive::IntegerData("flagellaModelType",cpd->flagellaModelType);
	db->addData(flagellaModelType_Data->getName(),flagellaModelType_Data);

	Data *isFlagellaCoordinated_Data = new Hive::BoolData("isFlagellaCoordinated",cpd->isFlagellaCoordinated);
	db->addData(isFlagellaCoordinated_Data->getName(),isFlagellaCoordinated_Data);

	Data *motor_Kd_Data = new Hive::DoubleData("motor_Kd",cpd->motor_Kd);
	db->addData(motor_Kd_Data->getName(),motor_Kd_Data);

	Data *motor_g1_Data = new Hive::DoubleData("motor_g1",cpd->motor_g1);
	db->addData(motor_g1_Data->getName(),motor_g1_Data);

	Data *motor_w_Data = new Hive::DoubleData("motor_w",cpd->motor_w);
	db->addData(motor_w_Data->getName(),motor_w_Data);


	TVectorData<int> *motorState = new TVectorData<int> ("MotorStates","tvector_integer");
	motorState->reserveSize(cpd->n_motors);
	for(int m=0; m<cpd->n_motors; m++) {
		motorState->at(m)=CellParameterData::CW;
	}
	db->addData(motorState->getName(),motorState);

	TVectorData<int> *lastmotorState = new TVectorData<int> ("LastMotorStates","tvector_integer");
	lastmotorState->reserveSize(cpd->n_motors);
	for(int m=0; m<cpd->n_motors; m++) {
		lastmotorState->at(m)=CellParameterData::CW;
	}
	db->addData(lastmotorState->getName(),lastmotorState);

	// Set flagella output
	TVectorData<int> *flagellaState = new TVectorData<int> ("FlagellaStates","tvector_integer");
	flagellaState->reserveSize(cpd->n_motors);
	for(int m=0; m<cpd->n_motors; m++) {
		flagellaState->at(m)=CellParameterData::NORMAL;
	}
	db->addData(flagellaState->getName(),flagellaState);

	TVectorData<int> *lastFlagellaState = new TVectorData<int> ("LastFlagellaStates","tvector_integer");
	lastFlagellaState->reserveSize(cpd->n_motors);
	for(int m=0; m<cpd->n_motors; m++) {
		lastFlagellaState->at(m)=CellParameterData::NORMAL;
	}
	db->addData(lastFlagellaState->getName(),lastFlagellaState);


	//Add data objects needed by the flagella models
	Data *minNumberOfMotorsInCCWneededToSwim_Data = new Hive::IntegerData("minNumberOfMotorsInCCWneededToSwim",cpd->minNumberOfMotorsInCCWneededToSwim);
	db->addData(minNumberOfMotorsInCCWneededToSwim_Data->getName(),minNumberOfMotorsInCCWneededToSwim_Data);


	//Only add this data if we are using the conformation model
	//Intermediate flagella objects needed by the conformation model
	if(cpd->flagellaModelType==CellParameterData::FLAGELLA_CONFORMATION_MODEL)
	{

		Data *minTimeInSemiData = new Hive::DoubleData("minTimeInSemi",cpd->minTimeInSemi);
		db->addData(minTimeInSemiData->getName(),minTimeInSemiData);

		//flagella state information
		TVectorData<double> *intervalTime = new TVectorData<double> ("intervalTime","tvector_double");
		intervalTime->reserveSize(cpd->n_motors);
		for(int m=0; m<cpd->n_motors; m++) {
			intervalTime->at(m)=1;
		}
		db->addData(intervalTime->getName(),intervalTime);

		TVectorData<double> *timeInSemi= new TVectorData<double> ("timeInSemi","tvector_double");
		timeInSemi->reserveSize(cpd->n_motors);
		for(int m=0; m<cpd->n_motors; m++) {
			timeInSemi->at(m)=1;
		}
		db->addData(timeInSemi->getName(),timeInSemi);
	}


	// Set CheYp values for the cell (or multiple if we need multiple traces per cell)
	Data *meanCheYp_Data = new Hive::DoubleData("meanCheYp",cpd->cheyp_mean);
	db->addData(meanCheYp_Data->getName(),meanCheYp_Data);
	//cerr << "hier " << cpd->meanCheYp << endl;
	TVectorData<double> *ypConc;
	TVectorData<double> *methLevel;
	if(!cpd->isFlagellaCoordinated) {
		ypConc = new TVectorData<double>("CheYp","tvector_double");
		methLevel = new TVectorData<double>("methLevel","tvector_double");
		ypConc->reserveSize(cpd->n_motors);
		methLevel->reserveSize(cpd->n_motors);
		for (int i=0; i<cpd->n_motors; i++) {
			ypConc->at(i)=cpd->cheyp_mean;
			methLevel->at(i)=cpd->meth_init;
		}
	} else {
		ypConc = new TVectorData<double>("CheYp","tvector_double");
		ypConc->reserveSize(1);
		ypConc->at(0)=cpd->cheyp_mean;

		methLevel = new TVectorData<double>("methLevel","tvector_double");
		methLevel->reserveSize(1);
		methLevel->at(0)=cpd->meth_init;
	}
	db->addData(ypConc->getName(),ypConc);
	db->addData(methLevel->getName(),methLevel);


	// the following information has to be added, if we are dealing with the old model
	if (!this->is_wills_methylation_model.getBool()) {
		//Set Signaling parameters
		Data *tauAdapt_Data = new Hive::DoubleData("tauAdapt",cpd->tauAdapt);
		db->addData(tauAdapt_Data);

		Data *tauNoise_Data = new Hive::DoubleData("tauNoise",cpd->tauNoise);
		db->addData(tauNoise_Data);

		Data *sigma_Data = new Hive::DoubleData("sigma",cpd->sigma);
		db->addData(sigma_Data);

		Data *cheyp_mean = new Hive::DoubleData("cheyp_mean", cpd->cheyp_mean);
		db->addData(cheyp_mean);

	} else {						// end of data needed for old model
		// now it is the new model!
		// add specific data for r&b model
		Data *kr = new Hive::DoubleData("kr", cpd->kr);
		Data *Kr = new Hive::DoubleData("Kr", cpd->Kr);
		Data *kb = new Hive::DoubleData("kb", cpd->kb);
		Data *Kb = new Hive::DoubleData("Kb", cpd->Kb);
		Data *noise_parameter = new Hive::DoubleData("noise_parameter", cpd->noise_parameter);
		Data *meth_init = new Hive::DoubleData("meth_init", cpd->meth_init);
		Data *cheyp_mean = new Hive::DoubleData("cheyp_mean", cpd->cheyp_mean);
		db->addData(kr);
		db->addData(Kr);
		db->addData(kb);
		db->addData(Kb);
		db->addData(noise_parameter);
		db->addData(meth_init);
		db->addData(cheyp_mean);

		Data *tauNoise_Data = new Hive::DoubleData("tauNoise",cpd->tauNoise);
		db->addData(tauNoise_Data);
	}

	// Set other receptor parameters
	Data *alpha_Data = new Hive::DoubleData("alpha",cpd->alpha);
	db->addData(alpha_Data->getName(),alpha_Data);

	/// change 10-31-2010: in will's r&b model the alpha value is not 6.0 but 5.7.
	/// is this always the case? anyways if change it here to that value
	/// 2-14-2014: set back to alpha
	if (this->is_wills_methylation_model.getBool()) {
		((Hive::DoubleData *) alpha_Data)->setDouble(cpd->alpha);
	}

	Data *e0_Data = new Hive::DoubleData("e0",cpd->e0);
	db->addData(e0_Data->getName(),e0_Data);

	Data *e1_Data = new Hive::DoubleData("e1",cpd->e1);
	db->addData(e1_Data->getName(),e1_Data);

	Data *KoffTar_Data = new Hive::DoubleData("KoffTar",cpd->KoffTar);
	db->addData(KoffTar_Data->getName(),KoffTar_Data);

	Data *KonTar_Data = new Hive::DoubleData("KonTar",cpd->KonTar);
	db->addData(KonTar_Data->getName(),KonTar_Data);

	Data *KoffTsr_Data = new Hive::DoubleData("KoffTsr",cpd->KoffTsr);
	db->addData(KoffTsr_Data->getName(),KoffTsr_Data);

	Data *KonTsr_Data = new Hive::DoubleData("KonTsr",cpd->KonTsr);
	db->addData(KonTsr_Data->getName(),KonTsr_Data);

	Data *TarCount_Data = new Hive::IntegerData("TarCount",cpd->TarCount);
	db->addData(TarCount_Data->getName(),TarCount_Data);

	Data *TsrCount_Data = new Hive::IntegerData("TsrCount",cpd->TsrCount);
	db->addData(TsrCount_Data->getName(),TsrCount_Data);

	Data *a0_data = new Hive::DoubleData("a0", cpd->a0);
	db->addData(a0_data->getName(), a0_data);

	Data *y0_data = new Hive::DoubleData("y0", cpd->y0);
	db->addData(y0_data->getName(), y0_data);

	// advance to the next cell
	this->currentAgent++;
}

const int CellParameterData::CCW=0;
const int CellParameterData::CW=1;

const int CellParameterData::RUN=0;
const int CellParameterData::TUMBLE=1;

const int CellParameterData::NORMAL = 3;
const int CellParameterData::SEMI = 4;
const int CellParameterData::CURLY = 5;
const int CellParameterData::NONE = 6;


double CellParameterData::outputInterval = 10;
double CellParameterData::eqTime = 200;

CellParameterData::CellParameterData() {
	//Default Settings for All Cells
  
	// Motion parameters
	speed = 20;

	// Flagella Parameters
	flagellaModelType = CellParameterData::FLAGELLA_CONFORMATION_MODEL;
	isFlagellaCoordinated = true;
	n_motors = 1;
	motor_Kd = 3.06;
	motor_g1 = 40;
	motor_w  = 1.3;
	minNumberOfMotorsInCCWneededToSwim = 1  ;
	minTimeInSemi                      = 0.0;

	// Signaling and Noise Parameters
	tauAdapt     =  15 ;
	tauNoise     =  15 ;
	sigma        =  0.1;
	meanCheYp    =  2.7;


	// Receptor Cluster Parameters
	alpha 	=  6.0	   ;
	e0 		=  6.0     ;
	e1 		=  -1.0    ;
	KoffTar   = 	0.02e-3 ;
	KonTar    = 	0.4e-3    ;
	KoffTsr   = 	100.0e-3   ;
	KonTsr    = 	1000      ;
	TarCount  = 	6         ;
	TsrCount  = 	13        ;
	a0 = 0.165;
	y0 = 4.77;
}

CellParameterData::~CellParameterData() { }
