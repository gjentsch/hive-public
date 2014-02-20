#include "chemotaxisMovementInCapAssay.hh"

using namespace std;
using namespace Hive;
using namespace ChemoPop;

// a set of functions follows that are needed for the
// simulator to work. they account for the different intersections.
void norm(double v[3]) {
	double mag = sqrt(v[0]*v[0]+v[1]*v[1]+v[2]*v[2]);
	v[0]/=mag; v[1]/=mag; v[2]/=mag;
}

double dist(double p1[3], double p2[3]) {
	double dx = p1[0]-p2[0];
	double dy = p1[1]-p2[1];
	double dz = p1[2]-p2[2];
	return sqrt(dx*dx+dy*dy+dz*dz);
}

void translate(double p[3], double v[3], double d)	{
	p[0] = p[0]+d*v[0];
	p[1] = p[1]+d*v[1];
	p[2] = p[2]+d*v[2];
}

void cout_v(const char *name, double v[3])	{
	cout<<name<<":\t"<<v[0]<<"\t"<<v[1]<<"\t"<<v[2]<<endl;
}

void cout_v(double v[3]) {
	cout<<v[0]<<","<<v[1]<<","<<v[2]<<";"<<endl;
}

bool intersectPlane(double p[3], double u[3], double n[3], double d, double q[3]) {
	const int x=0,y=1,z=2;
	double c = u[x]*n[x]+u[y]*n[y]+u[z]*n[z];
	if(fabs(c)<=0.00001) {
		cout<<"Near parallel"<<endl;
		return false;
	}
	double alpha = (d-(p[x]*n[x]+p[y]*n[y]+p[z]*n[z])) / c;
	if(alpha<0) {
		cout<<"No intersection"<<endl;
		return false;
	}
	q[x] = p[x]+alpha*u[x];
	q[y] = p[y]+alpha*u[y];
	q[z] = p[z]+alpha*u[z];
	return true;
}

bool intersectTube(double p[3], double u[3], double Re, double q[3], double tubeNormal[3])	{
	const int x=0,y=1,z=2;

	// Solution to the quadradic equation intersecting with an infinite
	// sphere of radius Rc.  The equation:
	// (p[x]+t*u[x])^2 + (p[y]+t*u[y])^2 = r^2

	double a = (u[x]*u[x]+u[y]*u[y]);
	double b = 2.*p[x]*u[x]+2.*p[y]*u[y];
	double c = p[x]*p[x]+p[y]*p[y]-Re*Re;
	double rootTerm = b*b-4*a*c;

	//Make sure the root term is positive, otherwise we don't intersect
	if(rootTerm<0)
		return false;

	// We use this better alternate and equivalent formula for the quadradic equation
	// [ instead of: t1=(-b+sqrt(b*b-4*a*c))/(2*a);]
	// that does not make the round off errors like the usual equation may sometimes do
	// (see any version of Numerical Recipies for the explanation)
	double sign = 0; if(b>=0) sign=1; else sign=-1;
	double temp = -0.5*(b+sign*sqrt(rootTerm));
	if(fabs(a)<=0.0000001 || fabs(temp)<=0.0000001) {
		cout<<"here\n";
		return false;
	} // return false if we find only one intersection
	double t1 = temp/a;
	double t2 = c/temp;

	// Based on the sign of our parameters t, we can determine if the intersection is in front, behind,
	// and based on its value, we can determine which came first....
	if(fabs(t1)<0.00001 && t2>=0) {
		q[x] = p[x]+t2*u[x];
		q[y] = p[y]+t2*u[y];
		q[z] = p[z]+t2*u[z];
	} else if (fabs(t2)<0.00001 && t1>=0) {
		q[x] = p[x]+t1*u[x];
		q[y] = p[y]+t1*u[y];
		q[z] = p[z]+t1*u[z];
	} else if(t1<0 && t2<0) {
		// we should never get here, but this is the case that the tube is completely
		// behind us  - again, which should never happen as we are in the tube
		cout<<"\n\n!!! Error in intersectTube: the tube should never be behind us!\n\n";
		return false;
	} else if(t1>0) {
		q[x] = p[x]+t1*u[x];
		q[y] = p[y]+t1*u[y];
		q[z] = p[z]+t1*u[z];
	} else if(t2>0) {
		q[x] = p[x]+t2*u[x];
		q[y] = p[y]+t2*u[y];
		q[z] = p[z]+t2*u[z];
	} else {
		cout<<"\n\nSomething unexpected happened when trying to find the intersection with the tube!!\n";
		cout<<"We should have collided with it, but didn't find the proper intersections!!\n\n";
		return false;
	}

	tubeNormal[x]=-q[x];
	tubeNormal[y]=-q[y];
	tubeNormal[z]=0;  //should always be zero, by orientation of the tube
	norm(tubeNormal);

	return true;
}

bool intersectCap(double p[3], double u[3], double p2[3], double Rc, double Zc, double q[3], double capNormal[3]) {
	const int x=0,y=1,z=2;

	// Solution to the quadradic equation intersecting with an infinite
	// sphere of radius Rc.  The equation:
	// (p[x]+t*u[x])^2 + (p[y]+t*u[y])^2 = r^2

	double a = (u[x]*u[x]+u[y]*u[y]);
	double b = 2.*p[x]*u[x]+2.*p[y]*u[y];
	double c = p[x]*p[x]+p[y]*p[y]-Rc*Rc;
	double rootTerm = b*b-4*a*c;

	// Make sure the root term is positive, otherwise we don't intersect
	if(rootTerm<0) return false;

	// We use this better alternate and equivalent formula for the quadradic equation
	// that does not make the round off errors like the usual equation may sometimes do
	// (see any version of Numerical Recipies for the explanation)
	double sign = 0; if(b>=0) sign=1; else sign=-1;
	double temp = -0.5*(b+sign*sqrt(rootTerm));
	if(fabs(a)<=0.0000001 || fabs(temp)<=0.0000001)
		return false;  // return false if we find only one intersection
	double t1 = temp/a;
	double t2 = c/temp;

	// Right away, let us calculate the two intersection points;
	double q1[3];
	q1[x] = p[x]+t1*u[x];
	q1[y] = p[y]+t1*u[y];
	q1[z] = p[z]+t1*u[z];
	double q2[3];
	q2[x] = p[x]+t2*u[x];
	q2[y] = p[y]+t2*u[y];
	q2[z] = p[z]+t2*u[z];

	// if both intersections are further than how far we traveled, then we clearly
	// are not intersecting on this segment
	double d1 = dist(p,q1);
	double d2 = dist(p,q2);
	double maxD = dist(p,p2);
	if(d1>maxD && d2>maxD) return false;

	// if both intersections are behind us, then we must be outside the capillary facing
	// away from the capillary so any intersection does not matter
	if(t1<0.0000001 && t2<0.0000001) return false;

	// if both intersections are in front of us, then we must be outside of the
	// capillary, so handle that case...
	if(t1>0.0000001 && t2>0.0000001) {
		if(q1[z]<Zc && q2[z]<Zc) {
			// if both intersections lie below the plane, then we didn't hit a thing!
			return false;
		}
		if(q1[z]<Zc && q2[z]>=Zc && p2[z]>Zc) {
			// Here there are two intersections, both in front of us, so we must be outside the cap
			// Also, one of the intersections crosses below the plane of the mouth (so it touches nothing)
			// and the second point where we are moving to lies above the plane of the mouth.  Therefore,
			// we are outside the cap and are going at an upwards angle into the cap.  That is fine!
			// We are in!!
			if(d2<maxD) {
				q[x]=q2[x]; q[y]=q2[y]; q[z]=q2[z];
				capNormal[x]=-q[x]; capNormal[y]=-q[y]; capNormal[z]=0; norm(capNormal);
				return true;
			}
		}
		if(q2[z]<Zc && q1[z]>=Zc && p2[z]>Zc) {
			// Here there are two intersections, both in front of us, so we must be outside the cap
			// Also, one of the intersections crosses below the plane of the mouth (so it touches nothing)
			// and the second point where we are moving to lies above the plane of the mouth.  Therefore,
			// we are outside the cap and are going at an upwards angle into the cap.  That is fine!
			// We are in!!
			if(d1<maxD) {
				q[x]=q1[x]; q[y]=q1[y]; q[z]=q1[z];
				capNormal[x]=-q[x]; capNormal[y]=-q[y]; capNormal[z]=0; norm(capNormal);
				return true;
			}
		}
		// Now, whichever intersection is closer is the one we have to find.  Since
		// we are outside the cap, we assume the normal of the cap is facing out
		if(d1<d2 && d1<maxD) {
			q[x]=q1[x]; q[y]=q1[y]; q[z]=q1[z];
			capNormal[x]=q[x]; capNormal[y]=q[y]; capNormal[z]=0; norm(capNormal);
			return true;
		} else if(d2<=d1 && d2<maxD) {
			q[x]=q2[x]; q[y]=q2[y]; q[z]=q2[z];
			capNormal[x]=q[x]; capNormal[y]=q[y]; capNormal[z]=0; norm(capNormal);
			return true;
		} else {
			// This is the case that the intersection was further than our next point, so
			// we don't actually intersect yet
			return false;
		}
	}
	// If one intersection is clearly in front, and the other behind, then we must be in the
	// capillary, so handle that case (note that normals are now pointing in!
	else if(t1>0.0000001 && t2<0.0000001) {
		if(q1[z]>Zc && d1<maxD) {  // remember to check that the intersection was in the capillary!
			q[x]=q1[x]; q[y]=q1[y]; q[z]=q1[z];
			capNormal[x]=-q[x]; capNormal[y]=-q[y]; capNormal[z]=0; norm(capNormal);
			return true;
		} else { // Getting here means we are swimming out of the cap... oh my!
			return false;
		}
	} else if(t2>0.0000001 && t1<0.0000001) {
		if(q2[z]>Zc && d2<maxD) {  // remember to check that the intersection was in the capillary!
			q[x]=q2[x]; q[y]=q2[y]; q[z]=q2[z];
			capNormal[x]=-q[x]; capNormal[y]=-q[y]; capNormal[z]=0; norm(capNormal);
			return true;
		} else { // Getting here means we are swimming out of the cap... oh my!
			return false;
		}
	}
	// This is where things get interesting...  Imagine that we are a point sitting on the
	// wall of a capillary (meaning one of the t values ~0).  We are pointing in the direction
	// of another side of the capillary (the other t is >0).  That must mean we are inside the
	// capillary (otherwise we would be pointing away from the capillary) and we want to make sure
	// we intersect with the right one.  This shouldn't really ever happen if our steps are small,
	// but in general we must account for this.  So let's check this case out...
	else if(fabs(t1)<0.00001 && t2>0 && d2<maxD) {
		if(q2[z]>Zc) {  // remember to check that the intersection was in the capillary!
			q[x]=q2[x]; q[y]=q2[y]; q[z]=q2[z];
			capNormal[x]=-q[x]; capNormal[y]=-q[y]; capNormal[z]=0; norm(capNormal);
			return true;
		} else { // Getting here means we are swimming out of the cap again... oh my!
			return false;
		}
	} else if(fabs(t2)<0.00001 && t1>0 && d1<maxD) {
		if(q1[z]>Zc) {  // remember to check that the intersection was in the capillary!
			q[x]=q1[x]; q[y]=q1[y]; q[z]=q1[z];
			capNormal[x]=-q[x]; capNormal[y]=-q[y]; capNormal[z]=0; norm(capNormal);
			return true;
		} else { // Getting here means we are swimming out of the cap again... oh my!
			return false;
		}
	} else {
		cout<<"Didn't catch something!!"<<endl;
		cout<<t1<<"\t"<<t2<<endl;
		cout<<d1<<"\t"<<d2<<"\t"<<maxD<<endl;
		exit(0);
	}

	q[0]=0; q[1]=0; q[2]=0;
	return false;
}

	// p: orignal point
	// u: original direction that got us to p2
	// p2: location of next point beyond the reflecting plane
	// q: intersection point of line segment p2-p with plane
	// up: some up vector perpindicular to u, but must be updated
	// n: normal of the plane
	/*
	 *    ^                        /
	 *    |                       /
	 *   (up)                    /
	 *    |                     /
	 *   [p]----(u)-->--------[q]-------[p2]
	 *                        /
	 *                       /
	 *              <--(n)--/
	 *                     /
	 */
void reflect(double p[3], double u[3], double p2[3], double up[3], double q[3], const double n[3], double rv[3]) {
	const int x=0,y=1,z=2;

	// create vector from intersection point to original point
	double v[3];
	v[x]=p[x]-q[x]; v[y]=p[y]-q[y]; v[z]=p[z]-q[z];
	norm(v);  //normalize v;

	// use equation 2*(v dot n)*n - v to get reflection vector
	double v_dot_n = v[x]*n[x]+v[y]*n[y]+v[z]*n[z];
	rv[x] = 2.*(v_dot_n)*n[x]-v[x];
	rv[y] = 2.*(v_dot_n)*n[y]-v[y];
	rv[z] = 2.*(v_dot_n)*n[z]-v[z];

	// set the direction to be the new direction
	u[x]=rv[x];
	u[y]=rv[y];
	u[z]=rv[z];

	// set the last point to be the intersection point
	cout_v(p);
	p[x]=q[x];
	p[y]=q[y];
	p[z]=q[z];
	norm(u);      //normalize the new direction

	// We have to choose the new up vector (which doesn't matter in this case, it
	// just has to be orthogonal to the direction.  That is why we just change
	// one of the coordinates so that we are now in the correct direction
	if(u[z]>u[x] && u[z]>u[y])
		up[z]=-(up[x]*u[x]+up[y]*u[y])/u[z];
	else if(u[y]>u[x])
		up[y]=-(up[x]*u[x]+up[z]*u[z])/u[y];
	else
		up[x]=-(up[y]*u[y]+up[z]*u[z])/u[x];
	norm(up);

	// Finally, we have to set the final point (p2).  We must, however, conserve
	// the displacement, so that we don't go on forever and travel the correct distance

	// To do that, we first find distance between intersection and p2, then
	// translate the intersection by the new direction to get the new p2
	double dis = dist(q,p2);
	p2[0]=q[0]; p2[1]=q[1]; p2[2]=q[2];
	translate(p2, rv, dis);
}



ChemotaxisMovementSimulatorInCapAssay::ChemotaxisMovementSimulatorInCapAssay() {
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

	// data that is needed for the capillary assay
	// note, that we hard code the capillary assay. it should be part of the
	// database to be honest.
	Nt[0]=0; Nt[1]=0; Nt[2]=-1;
	Nb[0]=0; Nb[1]=0; Nb[2]=1;
	Zt = 10000;
	Zc = 7000;
	Zb = 0;
	Re = 4000; //2525;
	Rc = 250;
}

ChemotaxisMovementSimulatorInCapAssay::~ChemotaxisMovementSimulatorInCapAssay() {
	out->close(); // if you do not close this you risk of not clearing the buffer such that you may have an empty file.
	delete gs;
}

void ChemotaxisMovementSimulatorInCapAssay::setWorldAgent(Agent *ag) {
	this->agent        = ag;
	this->internalTime = 0;

	this->initialise();
}

void ChemotaxisMovementSimulatorInCapAssay::initialise() {
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

	this->movement_dt  		   = (DoubleData*)  db->getDataItem("dt");
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
    // We have to make sure the up vector is in Z and the direction vector is in X
	for (int i=0; i<this->numberofcells->getInteger(); i++) {
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
}

void ChemotaxisMovementSimulatorInCapAssay::prepare() {
	// i think there is nothing to do here.
}

void ChemotaxisMovementSimulatorInCapAssay::synchroniseWithDatabase() {
	// i don't think there is anything to do here.
}

void ChemotaxisMovementSimulatorInCapAssay::step(double t) {
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


void ChemotaxisMovementSimulatorInCapAssay::swimToNewPosition(int i) {
	this->moveToNewPosition(i);
	// account for rotational diffusion!
	this->changeDirRotDiffusion(i);
}

void ChemotaxisMovementSimulatorInCapAssay::moveToNewPosition(int i) {
	double distanceTraveled = this->cellspeeds->at(i) * this->movement_dt->getDouble();

	// get the start position
	double start[3];
	start[0] = this->cell_positions->at(i)->at(0);
	start[1] = this->cell_positions->at(i)->at(1);
	start[2] = this->cell_positions->at(i)->at(2);

	// get the predicted end
	// the direction vector is always a unitvector, so this works!
	double predictedEnd[3];
	predictedEnd[0] = this->cell_positions->at(i)->at(0) + distanceTraveled*this->cell_dir_vecs->at(i)->at(0);
	predictedEnd[1] = this->cell_positions->at(i)->at(1) + distanceTraveled*this->cell_dir_vecs->at(i)->at(1);
	predictedEnd[2] = this->cell_positions->at(i)->at(2) + distanceTraveled*this->cell_dir_vecs->at(i)->at(2);

	// get the direction vector
	double direction[3];
	direction[0] = this->cell_dir_vecs->at(i)->at(0);
	direction[1] = this->cell_dir_vecs->at(i)->at(1);
	direction[2] = this->cell_dir_vecs->at(i)->at(2);

	// get the ups vector
	double ups[3];
	ups[0] = this->cell_up_vecs->at(i)->at(0);
	ups[1] = this->cell_up_vecs->at(i)->at(1);
	ups[2] = this->cell_up_vecs->at(i)->at(2);

    // check whether the position found above is admissible, i.e.
	// does it obey the boundaries of the capillary assay?
	while(!this->updatePosition(start,predictedEnd,direction,ups));

	// update relevant data-items on the database
	this->cell_positions->at(i)->at(0) = predictedEnd[0];
	this->cell_positions->at(i)->at(1) = predictedEnd[1];
	this->cell_positions->at(i)->at(2) = predictedEnd[2];

	this->cell_dir_vecs->at(i)->at(0)  = direction[0];
	this->cell_dir_vecs->at(i)->at(1)  = direction[1];
	this->cell_dir_vecs->at(i)->at(2)  = direction[2];

	this->cell_up_vecs->at(i)->at(0) = ups[0];
	this->cell_up_vecs->at(i)->at(1) = ups[1];
	this->cell_up_vecs->at(i)->at(2) = ups[2];
}

void ChemotaxisMovementSimulatorInCapAssay::changeDirRandom(int i) {
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


void ChemotaxisMovementSimulatorInCapAssay::changeDirDistribution(int i) {
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
}

void ChemotaxisMovementSimulatorInCapAssay::changeDirRotDiffusion(int i) {
	double local_up[3];
	local_up[0] = this->cell_up_vecs->at(i)->at(0);
	local_up[1] = this->cell_up_vecs->at(i)->at(1);
	local_up[2] = this->cell_up_vecs->at(i)->at(2);

	double local_dir[3];
	local_dir[0] = this->cell_dir_vecs->at(i)->at(0);
	local_dir[1] = this->cell_dir_vecs->at(i)->at(1);
	local_dir[2] = this->cell_dir_vecs->at(i)->at(2);

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
}

bool ChemotaxisMovementSimulatorInCapAssay::updatePosition(double p[3], double p2[3], double u[3], double up[3]) {
	int x = 0; int y = 1; int z = 2;
	double nRefPlane[3]; nRefPlane[x]=0; nRefPlane[y]=0; nRefPlane[z]=0;
	double qFinal[3]; qFinal[x]=0; qFinal[y]=0; qFinal[z]=0;
	bool intersects = false; double minDist = 0;

	//Calculate the radius from the center of the
	double r = sqrt( p2[x]*p2[x] + p2[y]*p2[y]);

	//This block will always keep us below the top of the chamber
	if(p2[z]>=Zt && r<Rc) {
		double q[3]; q[0]=-1; q[1]=-1; q[2]=-1;
		if(!intersectPlane(p,u,Nt,-Zt,q)) {
			cout<<"Error!  There should be an intersection with the top!!"<<endl;
			cout_v("p:",p);cout_v("u:",u);cout_v("p2:",p2);cout_v("up:",up);cout_v("q:",q);
			exit(1);
		}
		if(!intersects) {
			intersects = true;
			qFinal[x]=q[x]; qFinal[y]=q[y]; qFinal[z]=q[z];
			nRefPlane[x]=Nt[x]; nRefPlane[y]=Nt[y]; nRefPlane[z]=Nt[z];
			minDist = dist(p,q);
		} else {
			double cDist= dist(p,q);
			if(cDist<minDist) {
				qFinal[x]=q[x]; qFinal[y]=q[y]; qFinal[z]=q[z];
				nRefPlane[x]=Nt[x]; nRefPlane[y]=Nt[y]; nRefPlane[z]=Nt[z];
				minDist = cDist;
			}
		}
	}

	//This block always keeps us above the bottom of the chamber
	if(p2[z]<=Zb) {
		double q[3]; q[0]=-1; q[1]=-1; q[2]=-1;
		if(!intersectPlane(p,u,Nb,Zb,q)) {
			cout<<"Error!  There should be an intersection with the base!!"<<endl;
			cout_v("p:",p);cout_v("u:",u);cout_v("p2:",p2);cout_v("up:",up);cout_v("q:",q);
			exit(1);
		}
		if(!intersects) {
			intersects = true;
			qFinal[x]=q[x]; qFinal[y]=q[y]; qFinal[z]=q[z];
			nRefPlane[x]=Nb[x]; nRefPlane[y]=Nb[y]; nRefPlane[z]=Nb[z];
			minDist = dist(p,q);
		} else {
			double cDist= dist(p,q);
			if(cDist<minDist) {
				qFinal[x]=q[x]; qFinal[y]=q[y]; qFinal[z]=q[z];
				nRefPlane[x]=Nb[x]; nRefPlane[y]=Nb[y]; nRefPlane[z]=Nb[z];
				minDist = cDist;
			}
		}
	}

	//This block always keeps us inside the sides of the cylindrical chamber
	if(r>Re) {
		double q[3]; q[0]=-1; q[1]=-1; q[2]=-1;
		double tubeNormal[3];tubeNormal[0]=-1; tubeNormal[1]=-1; tubeNormal[2]=-1;
		if(!intersectTube(p,u,Re,q,tubeNormal))	{
			cout<<"Error!  There should be an intersection with the tube!!"<<endl;
			cout_v("p:",p);cout_v("u:",u);cout_v("p2:",p2);cout_v("up:",up);cout_v("q:",q);
			exit(1);
		}
		if(!intersects)	{
			intersects = true;
			qFinal[x]=q[x]; qFinal[y]=q[y]; qFinal[z]=q[z];
			nRefPlane[x]=tubeNormal[x]; nRefPlane[y]=tubeNormal[y]; nRefPlane[z]=tubeNormal[z];
			minDist = dist(p,q);
		} else {
			double cDist= dist(p,q);
			if(cDist<minDist) {
				qFinal[x]=q[x]; qFinal[y]=q[y]; qFinal[z]=q[z];
				nRefPlane[x]=tubeNormal[x]; nRefPlane[y]=tubeNormal[y]; nRefPlane[z]=tubeNormal[z];
				minDist = cDist;
			}
		}
	}

	double r0 = sqrt( p[x]*p[x] + p[y]*p[y]);

	// We only check for intersection with the capillary if it
	// a) is not completely contained inside the capillary radius
	// b) is not completely below the plane of the mouth of the capillary
	if(!(r0<Rc && r<Rc) && !(p2[z]<Zc && p[z]<Zc)) {
		double q[3]; q[0]=-1; q[1]=-1; q[2]=-1;
		double capNormal[3];capNormal[0]=-1; capNormal[1]=-1; capNormal[2]=-1;
		if(intersectCap(p,u,p2,Rc,Zc,q,capNormal)) {
			if(!intersects) {
				intersects = true;
				qFinal[x]=q[x]; qFinal[y]=q[y]; qFinal[z]=q[z];
				nRefPlane[x]=capNormal[x]; nRefPlane[y]=capNormal[y]; nRefPlane[z]=capNormal[z];
				minDist = dist(p,q);
			} else {
				double cDist= dist(p,q);
				if(cDist<minDist) {
					qFinal[x]=q[x]; qFinal[y]=q[y]; qFinal[z]=q[z];
					nRefPlane[x]=capNormal[x]; nRefPlane[y]=capNormal[y]; nRefPlane[z]=capNormal[z];
					minDist = cDist;
				}
			}
		}
	}

	if(intersects) {
		double rv[3]; rv[0]=0; rv[1]=0;rv[2]=0;
		reflect(p,u,p2,up,qFinal,nRefPlane,rv);
		return false;
	}
	return true;
}
