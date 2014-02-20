/*
 * worldparameterinitialiser.hh
 *
 *  Created on: 4 Aug 2013
 *      Author: Garrit
 */

#ifndef WORLDPARAMETERINITIALISER_HH_
#define WORLDPARAMETERINITIALISER_HH_

#include <exception>
#include <stdexcept>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "agentinitializer.hh"
#include "../../../core/agent/agent.hh"
#include "../../../core/agent/data/primitive/primitiveData.hh"
#include "../../../core/agent/data/template/tdata.hh"
#include "../../../core/agent/data/mapdata/mapintintdata.hh"
#include "../../../core/agent/data/boundary/boundarydata.hh"
#include "../../../core/exception/exception.hh"
#include "../../../core/util/util.hh"

// include muParser
#include "../../../core/util/function/muParser/muParser.h"
#include "../../../core/util/function/muParser/muParserBase.h"
#include "../../../core/util/function/muParser/muParserBytecode.h"
#include "../../../core/util/function/muParser/muParserCallback.h"
#include "../../../core/util/function/muParser/muParserDef.h"
#include "../../../core/util/function/muParser/muParserError.h"
#include "../../../core/util/function/muParser/muParserFixes.h"
#include "../../../core/util/function/muParser/muParserStack.h"
#include "../../../core/util/function/muParser/muParserToken.h"
#include "../../../core/util/function/muParser/muParserTokenReader.h"


using namespace Hive;
using namespace std;


namespace ChemoPop {

class WorldParameterData : public ParameterData {
public:
	WorldParameterData();
	~WorldParameterData();

	int    cellnumber;
	double outputinterval;
	double equilibrationtime;
	double dt;
	double endtime;
	double starttime;
	int    number_steps;
	double rotdiffconst; // rotational diffusion constant

	string dimension;

	bool readrotdiffconst;
	bool readdimension;

	vector<mu::Parser> environments;

	string fileextension;
	bool   isbinary;
	bool   blockworldoutput;
	bool   generatecelldata;
	bool   generateruntumble;
	bool   generatecellposition;

	vector<vector<double> > cellpositions;

	bool iscapillary_assay;
};


class WorldParameterInitialiser: public Hive::AgentInitializer {
public:
	WorldParameterInitialiser(string inputFileName);
	~WorldParameterInitialiser();

	void parseFile(string inputFileName);

	void setNextAgentParameters(Agent *ag);

	// for returning information needed by the chemopop-composer and others
	bool   blockWorldOutput()     { return wpd->blockworldoutput;};
	double getDt()                { return wpd->dt; };
	double getEquilibrationTime() { return wpd->equilibrationtime; };
	string getFileExtensionName() { return wpd->fileextension; };
	int    getNumberCells()       { return wpd->cellnumber; };
	int    getNumberSteps()       { return wpd->number_steps; };
	double getOutputInterval()    { return wpd->outputinterval; };
	bool   isCapillaryAssay()     { return wpd->iscapillary_assay; };
	bool   isOutputBinary()       { return wpd->isbinary; };
	bool   generateCellData()     { return wpd->generatecelldata; };
	bool   generateRunTumble()    { return wpd->generateruntumble; };
	bool   generateCellPosition() { return wpd->generatecellposition; };

protected:
	void parseRotDiff();
	void parseEnvironment();
	void parseCellPositions();
	void parseCellType();
	void parseDimension();
	void parseTimeInformation();
	void parseOutputFlags();
	void parseSeed();

	WorldParameterData *wpd;

private:
	string line;
	string type;
	istringstream iss;
	ifstream input;
};

} /* namespace Hive */
#endif /* WORLDPARAMETERINITIALISER_HH_ */
