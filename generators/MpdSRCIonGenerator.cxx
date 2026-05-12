
#include "MpdSRCIonGenerator.h"
#include "FairPrimaryGenerator.h"
#include "FairIon.h"
#include "FairParticle.h"
#include "FairRunSim.h"
#include "FairLogger.h"
#include <iosfwd> 
#include "TRandom.h"
#include "TDatabasePDG.h"
#include "TObjArray.h"
#include "TParticle.h"
#include "TParticlePDG.h"
#include <stdio.h>
#include <iostream>

Int_t MpdSRCIonGenerator::fgNIon = 0;

//Defaul constructor 
MpdSRCIonGenerator::MpdSRCIonGenerator():
	FairGenerator(),
	fIon(NULL),fQ(0), fA(0), fMult(0), fPmin(0), fPmax(0),
	fX(0), fY(0), fZ(0),fXsigma(0), fYsigma(0),
	fXA(0), fYA(0), fXAsigma(0), fYAsigma(0)
{
 
}

//Main constructor
MpdSRCIonGenerator::MpdSRCIonGenerator(Int_t q,  Int_t a,   Int_t mult,
		Double_t pmin, Double_t pmax,
		Double_t x, Double_t y,	Double_t z,  Double_t x_sigma,  Double_t y_sigma,
		Double_t xa, Double_t ya, Double_t xa_sigma, Double_t ya_sigma):
	FairGenerator(),
	fQ(q), fA(a), fMult(mult), fIon(NULL), fPmin(pmin), fPmax(pmax),
	fX(x), fY(y), fZ(z), fXsigma(x_sigma), fYsigma(y_sigma),
	fXA(xa), fYA(ya), fXAsigma(xa_sigma), fYAsigma(ya_sigma)
{
	fgNIon++;
	char buffer[20];
	sprintf(buffer, "FairIon%d", fgNIon);
	fIon= new FairIon(buffer, fQ, fA, fQ);
	FairRunSim* run = FairRunSim::Instance();

	if ( ! run )
	{
		std::cout << "\nNo FairRun instantised!";
	}
	else
	{
		run->AddNewIon(fIon);
	}
}

MpdSRCIonGenerator::MpdSRCIonGenerator(const MpdSRCIonGenerator& rhs):
	FairGenerator(rhs),
	fQ(rhs.fQ), fA(rhs.fA),
	fMult(rhs.fMult),
	fPmin(rhs.fPmin), fPmax(rhs.fPmax),
	fX(rhs.fX), fY(rhs.fY), fZ(rhs.fZ),
	fXsigma(rhs.fXsigma), fYsigma(rhs.fYsigma),
	fXA(rhs.fXA), fYA(rhs.fYA),
	fXAsigma(rhs.fXAsigma), fYAsigma(rhs.fYAsigma),
	fIon(rhs.fIon) // CHECK
{
	// fIon= new FairIon(buffer, z, a, q);
	FairRunSim* run = FairRunSim::Instance();
	if ( ! run ) {
		std::cout << "\n\nNo FairRun instantised!\n\n";
	} else {
		run->AddNewIon(fIon);
	}
}

MpdSRCIonGenerator::~MpdSRCIonGenerator()
{
	if(fIon) delete fIon;
}

FairGenerator* MpdSRCIonGenerator::CloneGenerator() const
{
	// Clone for worker (used in MT mode only)
	return new MpdSRCIonGenerator(*this);
}


Bool_t MpdSRCIonGenerator::ReadEvent(FairPrimaryGenerator* primGen)
{
	TParticlePDG* thisPart =
		TDatabasePDG::Instance()->GetParticle(fIon->GetName());
	if ( ! thisPart ) {
		std::cout << "\nSRCIonGenerator: Ion " << fIon->GetName()
			<< " not found in database!";
		return kFALSE;
	}

	int pdgType = thisPart->PdgCode();

	std::cout << "\nSRCIonGenerator: Generating " << fMult << " ions of type "
		<< fIon->GetName() << " (PDG code " << pdgType << ")";
	std::cout << "\t [Min,Max] Momentum = ["<< fPmin << "," << fPmax << "] Gev/c from vertex (" << fX << ", " << fY
		<< ", " << fZ << ") cm";


	for(Int_t i=0; i<fMult; i++)
	{
		//fP_rnd = gRandom->Uniform(fPmin, fPmax);
		fP_rnd = gRandom->Gaus((fPmin+fPmax)/2., (-fPmin+fPmax)/2.);	

		fX_rnd = gRandom->Gaus(fX,fXsigma);
		fY_rnd = gRandom->Gaus(fY,fYsigma);

		fXA_rnd = gRandom->Gaus(fXA, fXAsigma)*TMath::DegToRad();//random X angle in radians
		fYA_rnd = gRandom->Gaus(fYA, fYAsigma)*TMath::DegToRad();//random Y angle in radians

		fPx_rnd = fP_rnd*TMath::Sin(fXA_rnd);
		fPy_rnd = fP_rnd*TMath::Sin(fYA_rnd);
		fPz_rnd = sqrt( pow(fP_rnd, 2.) - pow(fPx_rnd, 2.) - pow(fPy_rnd, 2.) );

		primGen->AddTrack(pdgType, fPx_rnd, fPy_rnd, fPz_rnd, fX_rnd, fY_rnd, fZ);
	}

	return kTRUE;

}

ClassImp(MpdSRCIonGenerator)
