#ifndef SRCIONGENERATOR_H
#define SRCIONGENERATOR_H
#include "Rtypes.h" //for Double_t, Int_t etc.
#include "FairGenerator.h"
#include "FairIon.h"

class FairIon;
class FairPrimaryGenerator;

class MpdSRCIonGenerator : public FairGenerator
{
	public:
		MpdSRCIonGenerator();
		MpdSRCIonGenerator(Int_t q,  Int_t a,   Int_t mult,  //Q, A, multiplicity of Ion
		 	Double_t pmin, Double_t pmax,//min and max total momentum 
		 	Double_t x, Double_t y, Double_t z, // mean value for the X,Y,Z ov the vertex in cm
		 	Double_t x_sigma,  Double_t y_sigma,  //sigma of X and Y spread in cm 
		 	Double_t xa,   Double_t ya,	 // mean value of X angle and Y angle in deg
		 	Double_t xa_sigma, Double_t ya_sigma); 	 //sigma of XA and YA spread in deg 

		~MpdSRCIonGenerator();
		// Create an Event
		Bool_t ReadEvent(FairPrimaryGenerator* primGen);
		virtual FairGenerator* CloneGenerator() const;


	private:    
	  static Int_t fgNIon;
		FairIon*   fIon;
		Int_t fQ;
		Int_t fA;
		Int_t fMult;
		Double_t fPmin; 
		Double_t fPmax; 
		Double_t fP_rnd; 
		Double_t fPx_rnd; 
		Double_t fPy_rnd; 
		Double_t fPz_rnd; 
		Double_t fX;
		Double_t fY;
		Double_t fZ;
		Double_t fX_rnd;
		Double_t fY_rnd;
		Double_t fZ_rnd;
		Double_t fXsigma;
		Double_t fYsigma;
		Double_t fXA;
		Double_t fYA;
		Double_t fXA_rnd;
		Double_t fYA_rnd;
		Double_t fXAsigma;
		Double_t fYAsigma;

		MpdSRCIonGenerator(const MpdSRCIonGenerator&);
		MpdSRCIonGenerator& operator=(const MpdSRCIonGenerator&);


		ClassDef(MpdSRCIonGenerator,1);

};
#endif


