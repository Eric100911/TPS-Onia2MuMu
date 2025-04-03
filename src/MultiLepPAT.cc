/******************************************************************************
 *  [File] 
 *      MultiLepPAT.cc 
 *  [Class]      
 *      MultiLepPAT 
 *  [Directory] 
 *      TPS-Onia2MuMu/src/MultiLepPAT.cc
 *  [Description]
 *      Make rootTuple for Jpsi+Jpsi+Upsilon reconstruction from 6 muons
 *  [Implementation]
 *     <Notes on implementation>
 *  [Note]
 *      20240704 [Eric Wang]
 *          Upsilon is abbreviated as "Ups"
 *          PDG ID : Mu -> 13,  K -> 321, Pi -> 211, 
 *                   Jpsi -> 443, Ups -> 553, Phi -> 333
 *          Refraction was conducted, especially to the boolean variables.
 *          Room for improvement: (already tagged)
 *              "TO_ENC"            Encapsulation required
 *              "TO_IMPR_CPP11/17"  Use C++11/17 features for better 
 *                                  readability and efficiency
 *                                  (e.g. auto, range-based for loop)
 *          Github copilot is used for code and annotation completion.
 *      
 *      20240811 [Eric Wang]
 *       - The underlying core of edm::View<T> is actually a std::vector<T>.
 *         This fact may help with writing more efficient code.
 *       - It took me some time to realize that I must sort out the muon pairs
 *         before I can proceed with the JPsi and Upsilon reconstruction and 
 *         vertex matching. It would prove too troublesome to cover all 
 *         combinations of muon pairs in the "multi-layer-for-loop" structure.
 *       
 *      20241029 [Eric Wang and Shi Zhenpeng]
 *       - The code is branched from Eric-dev-run-loose.
 *         This will deal with combined production of Jpsi, Upsilon and Phi.
 *       - Relevant changes will be made at spots marked as "[J-U-P]".
******************************************************************************/

// system include files
#include "TLorentzVector.h"
// user include files
#include "../interface/MultiLepPAT.h"
#include "../interface/VertexReProducer.h"
#include <memory>
#include <regex>

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Common/interface/TriggerNames.h"

#include "CommonTools/Statistics/interface/ChiSquaredProbability.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/L1GlobalTrigger/interface/L1GlobalTriggerReadoutRecord.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/L1GlobalTrigger/interface/L1GtFdlWord.h"
#include "DataFormats/Candidate/interface/VertexCompositeCandidate.h"
#include "DataFormats/Common/interface/RefToBase.h"
#include "DataFormats/Candidate/interface/ShallowCloneCandidate.h"
#include "DataFormats/Candidate/interface/CandMatchMap.h"
#include "DataFormats/Math/interface/Error.h"
#include "DataFormats/Math/interface/LorentzVector.h"
#include "DataFormats/Math/interface/Point3D.h"
#include "DataFormats/Math/interface/Vector3D.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/MuonReco/interface/MuonSelectors.h"
#include "DataFormats/PatCandidates/interface/GenericParticle.h"
#include "DataFormats/Common/interface/ValueMap.h"
#include "DataFormats/TrackReco/interface/DeDxData.h"
#include "DataFormats/Math/interface/Point3D.h"
#include "DataFormats/GeometryCommonDetAlgo/interface/DeepCopyPointer.h"
#include "DataFormats/GeometryVector/interface/GlobalPoint.h"
#include "DataFormats/CLHEP/interface/AlgebraicObjects.h"
#include "DataFormats/CLHEP/interface/Migration.h"

#include "RecoVertex/KinematicFitPrimitives/interface/MultiTrackKinematicConstraint.h"
#include "RecoVertex/KinematicFit/interface/KinematicConstrainedVertexFitter.h"
#include "RecoVertex/AdaptiveVertexFit/interface/AdaptiveVertexFitter.h"
#include "RecoVertex/KinematicFit/interface/TwoTrackMassKinematicConstraint.h"
#include "RecoVertex/VertexTools/interface/VertexDistanceXY.h"
#include "Geometry/Records/interface/TrackerDigiGeometryRecord.h"

#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"
#include "TrackingTools/Records/interface/TransientTrackRecord.h"
#include "TrackingTools/TransientTrack/interface/TransientTrack.h"

#include "MagneticField/Engine/interface/MagneticField.h"

//////////This is necessary for lumicalc///////
#include "DataFormats/Luminosity/interface/LumiSummary.h"
#include "DataFormats/Luminosity/interface/LumiDetails.h"
#include "FWCore/Framework/interface/LuminosityBlock.h"
#include "RecoLuminosity/LumiProducer/interface/LumiCorrectionParam.h"
#include "RecoLuminosity/LumiProducer/interface/LumiCorrectionParamRcd.h"

#include "PhysicsTools/RecoUtils/interface/CheckHitPattern.h"

#include "TFile.h"
#include "TTree.h"
#include "TVector3.h"

#include <algorithm>
#include <memory>
#include <vector>
#include <utility>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_set>
#include <regex>
#include <iterator>

#include "DataFormats/MuonReco/interface/MuonSelectors.h"

#include "CLHEP/Matrix/Vector.h"
#include "CLHEP/Matrix/Matrix.h"
#include "CLHEP/Matrix/SymMatrix.h"

#include "TrackingTools/IPTools/interface/IPTools.h"

#include "../data/TMVAClassification_BDT.class.C"

// about photon
#include "DataFormats/EgammaCandidates/interface/PhotonFwd.h"
#include "DataFormats/EgammaCandidates/interface/Photon.h"
#include "DataFormats/EgammaCandidates/interface/Conversion.h"
#include "DataFormats/PatCandidates/interface/CompositeCandidate.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include <boost/foreach.hpp>

#include "DataFormats/PatCandidates/interface/PackedCandidate.h" // MINIAOD

// #define DISPLAY_STAGE

typedef math::Error<3>::type CovarianceMatrix;
typedef ROOT::Math::SVector<double, 3> SVector3;
typedef ROOT::Math::SMatrix<double, 3, 3, ROOT::Math::MatRepSym<double, 3>> SMatrixSym3D;

// constructors and destructor
MultiLepPAT::MultiLepPAT(const edm::ParameterSet &iConfig)
	: hlTriggerResults_(iConfig.getUntrackedParameter<edm::InputTag>("HLTriggerResults", edm::InputTag("TriggerResults::HLT"))),
	  inputGEN_(iConfig.getUntrackedParameter<edm::InputTag>("inputGEN", edm::InputTag("genParticles"))),
	  magneticFieldToken_(esConsumes<MagneticField, IdealMagneticFieldRecord>()),
	  theTTBuilderToken_(esConsumes<TransientTrackBuilder, TransientTrackRecord>(edm::ESInputTag("", "TransientTrackBuilder"))),
	  vtxSample(iConfig.getUntrackedParameter<std::string>("VtxSample", std::string("offlinePrimaryVertices"))),
	  doMC(iConfig.getUntrackedParameter<bool>("DoMonteCarloTree", false)),
	  MCParticle(iConfig.getUntrackedParameter<int>("MonteCarloParticleId", 20443)), // 20443 X, 100443 Psi(2S), 9120443  // X from B
	  doJPsiMassCost(iConfig.getUntrackedParameter<bool>("DoJPsiMassConstraint")),
	  MuPixHits_c(iConfig.getUntrackedParameter<int>("MinNumMuPixHits", 0)),
	  MuSiHits_c(iConfig.getUntrackedParameter<int>("MinNumMuSiHits", 0)),
	  MuNormChi_c(iConfig.getUntrackedParameter<double>("MaxMuNormChi2", 1000)),
	  MuD0_c(iConfig.getUntrackedParameter<double>("MaxMuD0", 1000)),
	  JMaxM_c(iConfig.getUntrackedParameter<double>("MaxJPsiMass", 4)),
	  JMinM_c(iConfig.getUntrackedParameter<double>("MinJPsiMass", 2.2)),
	  PiSiHits_c(iConfig.getUntrackedParameter<int>("MinNumTrSiHits", 0)),
	  MuPt_c(iConfig.getUntrackedParameter<double>("MinMuPt", 0)),
	  JPiPiDR_c(iConfig.getUntrackedParameter<double>("JPsiKKKMaxDR", 1)),
	  XPiPiDR_c(iConfig.getUntrackedParameter<double>("XCandPiPiMaxDR", 1.1)),
	  UseXDr_c(iConfig.getUntrackedParameter<bool>("UseXDr", false)),
	  JPiPiMax_c(iConfig.getUntrackedParameter<double>("JPsiKKKMaxMass", 50)),
	  JPiPiMin_c(iConfig.getUntrackedParameter<double>("JPsiKKKMinMass", 0)),
	  resolveAmbiguity_(iConfig.getUntrackedParameter<bool>("resolvePileUpAmbiguity", true)),
	  addXlessPrimaryVertex_(iConfig.getUntrackedParameter<bool>("addXlessPrimaryVertex", true)),
	  TriggersForJpsi_(iConfig.getUntrackedParameter<std::vector<std::string>>("TriggersForJpsi")),
	  FiltersForJpsi_(iConfig.getUntrackedParameter<std::vector<std::string>>("FiltersForJpsi")),
	  TriggersForUpsilon_(iConfig.getUntrackedParameter<std::vector<std::string>>("TriggersForUpsilon")),
	  FiltersForUpsilon_(iConfig.getUntrackedParameter<std::vector<std::string>>("FiltersForUpsilon")),
	  Debug_(iConfig.getUntrackedParameter<bool>("Debug_Output", false)),
	  Chi_Track_(iConfig.getUntrackedParameter<double>("Chi2NDF_Track", 10)),
	  X_One_Tree_(0),

	  runNum(0), evtNum(0), lumiNum(0), nGoodPrimVtx(0),
	  trigRes(0), trigNames(0), L1TT(0), MatchJpsiTrigNames(0), MatchUpsTrigNames(0),

	  priVtxX(0), priVtxY(0), priVtxZ(0), priVtxXE(0), priVtxYE(0), priVtxZE(0), priVtxChiNorm(0), priVtxChi(0), priVtxCL(0),
	  PriVtxXCorrX(0), PriVtxXCorrY(0), PriVtxXCorrZ(0),
	  PriVtxXCorrEX(0), PriVtxXCorrEY(0), PriVtxXCorrEZ(0), PriVtxXCorrC2(0), PriVtxXCorrCL(0),

	  nMu(0),
	  muPx(0), muPy(0), muPz(0), muD0(0), muD0E(0), muDz(0), muChi2(0), muGlChi2(0), mufHits(0),
	  muFirstBarrel(0), muFirstEndCap(0), muDzVtx(0), muDxyVtx(0),
	  muNDF(0), muGlNDF(0), muPhits(0), muShits(0), muGlMuHits(0), muType(0), muQual(0),
	  muTrack(0), muCharge(0), muIsoratio(0), muIsGoodLooseMuon(0), muIsGoodLooseMuonNew(0),
	  muIsGoodSoftMuonNewIlse(0), muIsGoodSoftMuonNewIlseMod(0), muIsGoodTightMuon(0), muIsJpsiTrigMatch(0), muIsUpsTrigMatch(0), munMatchedSeg(0),
	  muIsJpsiFilterMatch(0), muIsUpsFilterMatch(0),

	  muIsPatLooseMuon(0), muIsPatTightMuon(0), muIsPatSoftMuon(0), muIsPatMediumMuon(0),

	  muMVAMuonID(0), musegmentCompatibility(0),
	  mupulldXdZ_pos_noArb(0), mupulldYdZ_pos_noArb(0),
	  mupulldXdZ_pos_ArbDef(0), mupulldYdZ_pos_ArbDef(0),
	  mupulldXdZ_pos_ArbST(0), mupulldYdZ_pos_ArbST(0),
	  mupulldXdZ_pos_noArb_any(0), mupulldYdZ_pos_noArb_any(0),

	  Jpsi_cand_mass_p4(0), Jpsi_cand_mass_fit(0),

      Jpsi_mu_1_Idx(0), Jpsi_mu_2_Idx(0),
       Ups_mu_1_Idx(0),    Ups_mu_2_Idx(0),
	    Phi_K_1_Idx(0),     Phi_K_2_Idx(0),

      Jpsi_mass(0), Jpsi_massErr(0), Jpsi_massDiff(0),
        Ups_mass(0),    Ups_massErr(0),    Ups_massDiff(0),
         Phi_mass(0),    Phi_massErr(0),    Phi_massDiff(0),

      Jpsi_ctau(0), Jpsi_ctauErr(0), Jpsi_Chi2(0), Jpsi_ndof(0), Jpsi_VtxProb(0),
         Ups_ctau(0),    Ups_ctauErr(0),    Ups_Chi2(0),    Ups_ndof(0),    Ups_VtxProb(0),
         Phi_ctau(0),    Phi_ctauErr(0),    Phi_Chi2(0),    Phi_ndof(0),    Phi_VtxProb(0),
      
      Jpsi_phi(0), Jpsi_eta(0), Jpsi_pt(0),
         Ups_phi(0), Ups_eta(0), Ups_pt(0),
         Phi_phi(0),    Phi_eta(0),    Phi_pt(0),

      Jpsi_px(0), Jpsi_py(0), Jpsi_pz(0),
      Ups_px(0), Ups_py(0), Ups_pz(0),
         Phi_px(0),    Phi_py(0),    Phi_pz(0),
      
        Pri_mass(0),  Pri_massErr(0),
        Pri_ctau(0),  Pri_ctauErr(0), Pri_Chi2(0), Pri_ndof(0), Pri_VtxProb(0),
        Pri_px(0),    Pri_py(0),    Pri_pz(0), 
        Pri_phi(0),   Pri_eta(0),   Pri_pt(0),

        Phi_K_1_px(0),  Phi_K_1_py(0), Phi_K_1_pz(0),
        Phi_K_2_px(0),  Phi_K_2_py(0), Phi_K_2_pz(0),
        Phi_K_1_eta(0), Phi_K_1_phi(0), Phi_K_1_pt(0),
        Phi_K_2_eta(0), Phi_K_2_phi(0), Phi_K_2_pt(0),
      
	  // doMC
	  MC_X_py(0),
	  MC_X_pz(0),
	  MC_X_mass(0),
	  MC_Dau_Jpsipx(0),
	  MC_Dau_Jpsipy(0),
	  MC_Dau_Jpsipz(0),
	  MC_Dau_Jpsimass(0),
	  MC_Dau_psi2spx(0),
	  MC_Dau_psi2spy(0),
	  MC_Dau_psi2spz(0),
	  MC_Dau_psi2smass(0),
	  MC_Granddau_mu1px(0),
	  MC_Granddau_mu1py(0),
	  MC_Granddau_mu1pz(0),
	  MC_Granddau_mu2px(0),
	  MC_Granddau_mu2py(0),
	  MC_Granddau_mu2pz(0),
	  MC_Granddau_Jpsipx(0),
	  MC_Granddau_Jpsipy(0),
	  MC_Granddau_Jpsipz(0),
	  MC_Granddau_Jpsimass(0),
	  MC_Granddau_pi1px(0),
	  MC_Granddau_pi1py(0),
	  MC_Granddau_pi1pz(0),
	  MC_Granddau_pi2px(0),
	  MC_Granddau_pi2py(0),
	  MC_Granddau_pi2pz(0),
	  MC_Grandgranddau_mu3px(0),
	  MC_Grandgranddau_mu3py(0),
	  MC_Grandgranddau_mu3pz(0),
	  MC_Grandgranddau_mu4px(0),
	  MC_Grandgranddau_mu4py(0),
	  MC_Grandgranddau_mu4pz(0),

	  MC_X_chg(0),
	  MC_Dau_JpsipdgId(0),
	  MC_Dau_psi2spdgId(0),
	  MC_Granddau_mu1pdgId(0),
	  MC_Granddau_mu2pdgId(0),
	  MC_Granddau_JpsipdgId(0),
	  MC_Granddau_pi1pdgId(0),
	  MC_Granddau_pi2pdgId(0),
	  MC_Grandgranddau_mu3pdgId(0),
	  MC_Grandgranddau_mu4pdgId(0)
	  // mybxlumicorr(0), myrawbxlumi(0)
{
	// get token here for four-muon;
	gtRecordToken_     = consumes<L1GlobalTriggerReadoutRecord>(edm::InputTag("gtDigis"));
	gtbeamspotToken_   = consumes<BeamSpot>(edm::InputTag("offlineBeamSpot"));
	gtprimaryVtxToken_ = consumes<VertexCollection>(edm::InputTag("offlineSlimmedPrimaryVertices")); //  MINIAOD
	gtpatmuonToken_    = consumes<edm::View<pat::Muon>>(edm::InputTag("slimmedMuons"));				 //  MINIAOD
	gttriggerToken_    = consumes<edm::TriggerResults>(edm::InputTag("TriggerResults::HLT"));
	trackToken_        = consumes<edm::View<pat::PackedCandidate>>(edm::InputTag("packedPFCandidates")); //  MINIAOD
	genParticlesToken_ = consumes<reco::GenParticleCollection>(edm::InputTag("genParticles"));
}

MultiLepPAT::~MultiLepPAT()
{
	// do anything here that needs to be done at desctruction time
	// (e.g. close files, deallocate resources etc.)
}
// member functions

//    ofstream myfile("comparison.txt");
// ------------ method called to for each event  ------------
void MultiLepPAT::analyze(const edm::Event &iEvent, const edm::EventSetup &iSetup)
{
	

    std::cout<<"Load the MC results [Annotated by Eric Wang, 20240704]"<<std::endl;

	TLorentzVector MC_mu1p4, MC_mu2p4, MC_mu3p4, MC_mu4p4, MC_pi1p4, MC_pi2p4;
	if (doMC)
	{
		edm::Handle<reco::GenParticleCollection> genParticles;
		iEvent.getByToken(genParticlesToken_, genParticles);

		for (const auto &particle : *(genParticles.product()))
		{
			if (std::abs(particle.pdgId()) == 35 && particle.numberOfDaughters() >= 2)
			{
				MC_X_px->push_back(particle.px());
				MC_X_py->push_back(particle.py());
				MC_X_pz->push_back(particle.pz());
				MC_X_mass->push_back(particle.mass());
				MC_X_chg->push_back(particle.charge());
				// particle.daughter(0)->pdgId() == 443 Jpsi
				MC_Dau_JpsipdgId->push_back(particle.daughter(0)->pdgId());
				MC_Dau_Jpsipx->push_back(particle.daughter(0)->px());
				MC_Dau_Jpsipy->push_back(particle.daughter(0)->py());
				MC_Dau_Jpsipz->push_back(particle.daughter(0)->pz());
				MC_Dau_Jpsimass->push_back(particle.daughter(0)->mass());
				MC_Dau_psi2spdgId->push_back(particle.daughter(1)->pdgId());
				MC_Dau_psi2spx->push_back(particle.daughter(1)->px());
				MC_Dau_psi2spy->push_back(particle.daughter(1)->py());
				MC_Dau_psi2spz->push_back(particle.daughter(1)->pz());
				MC_Dau_psi2smass->push_back(particle.daughter(1)->mass());
				// particle.daughter(0)->daughter(1)->pdgId() == -13 mu+
				MC_Granddau_mu1pdgId->push_back(particle.daughter(0)->daughter(1)->pdgId());
				MC_Granddau_mu1px->push_back(particle.daughter(0)->daughter(1)->px());
				MC_Granddau_mu1py->push_back(particle.daughter(0)->daughter(1)->py());
				MC_Granddau_mu1pz->push_back(particle.daughter(0)->daughter(1)->pz());
				MC_Granddau_mu2pdgId->push_back(particle.daughter(0)->daughter(0)->pdgId());
				MC_Granddau_mu2px->push_back(particle.daughter(0)->daughter(0)->px());
				MC_Granddau_mu2py->push_back(particle.daughter(0)->daughter(0)->py());
				MC_Granddau_mu2pz->push_back(particle.daughter(0)->daughter(0)->pz());
				// particle.daughter(1)->daughter(0)->pdgId() == 443 Jpsi from psi2s
				MC_Granddau_JpsipdgId->push_back(particle.daughter(1)->daughter(0)->pdgId());
				MC_Granddau_Jpsipx->push_back(particle.daughter(1)->daughter(0)->px());
				MC_Granddau_Jpsipy->push_back(particle.daughter(1)->daughter(0)->py());
				MC_Granddau_Jpsipz->push_back(particle.daughter(1)->daughter(0)->pz());
				MC_Granddau_Jpsimass->push_back(particle.daughter(1)->daughter(0)->mass());
				// particle.daughter(1)->daughter(1)->pdgId() == 211 pi+ from psi2s
				MC_Granddau_pi1pdgId->push_back(particle.daughter(1)->daughter(1)->pdgId());
				MC_Granddau_pi1px->push_back(particle.daughter(1)->daughter(1)->px());
				MC_Granddau_pi1py->push_back(particle.daughter(1)->daughter(1)->py());
				MC_Granddau_pi1pz->push_back(particle.daughter(1)->daughter(1)->pz());
				MC_Granddau_pi2pdgId->push_back(particle.daughter(1)->daughter(2)->pdgId());
				MC_Granddau_pi2px->push_back(particle.daughter(1)->daughter(2)->px());
				MC_Granddau_pi2py->push_back(particle.daughter(1)->daughter(2)->py());
				MC_Granddau_pi2pz->push_back(particle.daughter(1)->daughter(2)->pz());
				// particle.daughter(1)->daughter(0)->daughter(1)->pdgId() == -13 mu+ from Jpsi from psi2s
				MC_Grandgranddau_mu3pdgId->push_back(particle.daughter(1)->daughter(0)->daughter(1)->pdgId());
				MC_Grandgranddau_mu3px->push_back(particle.daughter(1)->daughter(0)->daughter(1)->px());
				MC_Grandgranddau_mu3py->push_back(particle.daughter(1)->daughter(0)->daughter(1)->py());
				MC_Grandgranddau_mu3pz->push_back(particle.daughter(1)->daughter(0)->daughter(1)->pz());
				MC_Grandgranddau_mu4pdgId->push_back(particle.daughter(1)->daughter(0)->daughter(0)->pdgId());
				MC_Grandgranddau_mu4px->push_back(particle.daughter(1)->daughter(0)->daughter(0)->px());
				MC_Grandgranddau_mu4py->push_back(particle.daughter(1)->daughter(0)->daughter(0)->py());
				MC_Grandgranddau_mu4pz->push_back(particle.daughter(1)->daughter(0)->daughter(0)->pz());

				MC_mu1p4.SetXYZM((*MC_Granddau_mu1px)[0], (*MC_Granddau_mu1py)[0], (*MC_Granddau_mu1pz)[0], myMuMass);
				MC_mu2p4.SetXYZM((*MC_Granddau_mu2px)[0], (*MC_Granddau_mu2py)[0], (*MC_Granddau_mu2pz)[0], myMuMass);
				MC_mu3p4.SetXYZM((*MC_Grandgranddau_mu3px)[0], (*MC_Grandgranddau_mu3py)[0], (*MC_Grandgranddau_mu3pz)[0], myMuMass);
				MC_mu4p4.SetXYZM((*MC_Grandgranddau_mu4px)[0], (*MC_Grandgranddau_mu4py)[0], (*MC_Grandgranddau_mu4pz)[0], myMuMass);

				MC_pi1p4.SetXYZM((*MC_Granddau_pi1px)[0], (*MC_Granddau_pi1py)[0], (*MC_Granddau_pi1pz)[0], myKMass);
				MC_pi2p4.SetXYZM((*MC_Granddau_pi2px)[0], (*MC_Granddau_pi2py)[0], (*MC_Granddau_pi2pz)[0], myKMass);
			} // for (const auto& particle: *(genParticles.product()))
		}	  // if ( std::abs(particle.pdgId())  == 35 && particle.numberOfDaughters() ==2 )
	}		  // doMC

	// get event content information

	using std::vector;
	using namespace edm;
	using namespace reco;
	using namespace std;

	runNum = iEvent.id().run();
	evtNum = iEvent.id().event();
	lumiNum = iEvent.id().luminosityBlock();

	const MagneticField &bFieldHandle = iSetup.getData(magneticFieldToken_);

    /**************************************************************************
     * [Section]
     *      HLT Trigger Info
     * [Implementation]
     *      - Call getByToken() to acquire HLT results
     *      - Categorize the 
     * 
    **************************************************************************/

	edm::Handle<edm::TriggerResults> hltresults;
	bool Error_t = false;
	unsigned int nJpsitrigger = TriggersForJpsi_.size();
    unsigned int nUpstrigger  = TriggersForUpsilon_.size();
	try
	{
		iEvent.getByToken(gttriggerToken_, hltresults);
	}
	catch (...)
	{	
		Error_t = true;
		cout << "Couldn't get handle on HLT Trigger!" << endl;
	}
	if (Error_t || !hltresults.isValid())
	{
		cout << "No Trigger Results!" << endl;
	}
	else
	{
		int ntrigs = hltresults->size();
		if (ntrigs == 0)
		{
			cout << "No trigger name given in TriggerResults of the input " << endl;
		}

		edm::TriggerNames triggerNames_;
		triggerNames_ = iEvent.triggerNames(*hltresults);

		for (unsigned int JpsiTrig = 0; JpsiTrig < nJpsitrigger; JpsiTrig++){
			JpsiMatchTrig[JpsiTrig] = 0;
		} // Jpsi trigger

        for (unsigned int UpsTrig = 0; UpsTrig < nUpstrigger; UpsTrig++){
            UpsilonMatchTrig[UpsTrig] = 0;
        } // Initiating Upsilon trigger

		for (int itrig = 0; itrig < ntrigs; itrig++)            // Loop over all triggers
		{
			string trigName = triggerNames_.triggerName(itrig); // Extracting HLT trigger name
			int hltflag = (*hltresults)[itrig].accept();        // Check if accepted by this trigger
            #ifdef DISPLAY_STAGE
            cout << "Trigger Name: " << trigName << " [ Flag " << hltflag << "]" << std::endl;
            #endif        
			trigRes->push_back(hltflag);
			trigNames->push_back(trigName);
			// Checking if match any of the Jpsi triggers
            #ifdef DISPLAY_STAGE
            puts("Looking for Jpsi trigger match.");
            #endif
			for (unsigned int JpsiTrig = 0; JpsiTrig < nJpsitrigger; JpsiTrig++)
			{
				// regex matching: "containing the trigger name as substring"
				std::regex pattern(".*"+TriggersForJpsi_[JpsiTrig]+".*");
				if (std::regex_search(trigName, pattern))
				{
					JpsiMatchTrig[JpsiTrig] = hltflag;
					bool isDuplicate = false;                   // Flag for duplicate trigger names
					if(hltflag)
					{
						for(unsigned int MatchTrig = 0; MatchTrig < MatchJpsiTrigNames->size(); MatchTrig ++)
						{
							if(trigName == MatchJpsiTrigNames->at(MatchTrig))
							{
								isDuplicate = true;
								break;
							}
						}
					}
					if(!isDuplicate)
					{
						MatchJpsiTrigNames->push_back(trigName); // "Triggers that have not appeared in the event"
					}		
					break;
				}
			} // Jpsi Trigger

            #ifdef DISPLAY_STAGE
            puts("Looking for Upsilon trigger match.");
            #endif

            // Check if match any of the Upsilon triggers
            for (unsigned int UpsTrig = 0; UpsTrig < TriggersForUpsilon_.size(); UpsTrig++){
                // regex matching: "containing the trigger name as substring"
                std::regex pattern(".*"+TriggersForUpsilon_[UpsTrig]+".*");
                if (std::regex_search(trigName, pattern)){
                    #ifdef DISPLAY_STAGE
                    puts("Matched Upsilon Trigger");
                    #endif

                    UpsilonMatchTrig[UpsTrig] = hltflag;

                    #ifdef DISPLAY_STAGE
                    puts("Add if not duplicate.");
                    #endif

                    bool isDuplicate = false;                   // Flag for duplicate trigger names
                    if(hltflag){
                        for(unsigned int MatchTrig = 0; MatchTrig < MatchUpsTrigNames->size(); MatchTrig++){
                            if(trigName == MatchUpsTrigNames->at(MatchTrig)){
                                isDuplicate = true;

                                #ifdef DISPLAY_STAGE
                                puts("Well, is duplicate.");
                                #endif

                                break;
                            
                            }
                        }
                    }
                    if(!isDuplicate){
                        #ifdef DISPLAY_STAGE
                        puts("Not duplicate, add to list.");
                        #endif
                        MatchUpsTrigNames->push_back(trigName); // "Triggers that have not appeared in the event"
                    }		
                    break;
                }
            } // Upsilon Trigger
		}
	} // end of HLT trigger info

    #ifdef DISPLAY_STAGE
    puts("HLT Trigger Info is done.");
    #endif

	std::string vrtxFilter("hltVertexmumuFilterUpsilonMuon");
	std::string L3Filter("hltTripleMuL3PreFiltered0");

	edm::ESHandle<TransientTrackBuilder> theB = iSetup.getHandle(theTTBuilderToken_);

	edm::Handle<L1GlobalTriggerReadoutRecord> gtRecord;
	iEvent.getByToken(gtRecordToken_, gtRecord);
	const DecisionWord dWord = gtRecord->decisionWord();

	const TechnicalTriggerWord ttWord = gtRecord->technicalTriggerWord();
	for (unsigned int l1i = 0; l1i != ttWord.size(); ++l1i)
	{
		L1TT->push_back(ttWord.at(l1i));
	}

    // Get the primary vertex information [Annotated by Eric Wang, 20240704]
    //      Initialization
	Vertex thePrimaryV;
	Vertex theRecoVtx;
	Vertex theBeamSpotV;
	BeamSpot beamSpot;
	math::XYZPoint RefVtx;

	// get BeamSplot
	edm::Handle<reco::BeamSpot> beamSpotHandle;
	iEvent.getByToken(gtbeamspotToken_, beamSpotHandle);
	if (beamSpotHandle.isValid())
	{
		beamSpot = *beamSpotHandle;
		theBeamSpotV = Vertex(beamSpot.position(), beamSpot.covariance3D());
	}
	else
	{
		cout << "No beam spot available from EventSetup" << endl;
	}

	Handle<VertexCollection> recVtxs;
	iEvent.getByToken(gtprimaryVtxToken_, recVtxs);
	unsigned int nVtxTrks = 0;
	///////////////////////////////////////////////////////////////////////
	////////////////Check Lines below for Primary Vertex///////////////////
	///////////////////////////////////////////////////////////////////////

    // Determine the good vertices [Annotated by Eric Wang, 20240704]

	int myNGoodPrimVtx = 0;
	for (unsigned myi = 0; myi < recVtxs->size(); myi++)
	{
        //Selection criteria:  ndof    -> "enough particle info"(?)     [Annotated by Eric Wang, 20240704]
        //                      z, rhp  -> "not too far off"            [Annotated by Eric Wang, 20240704]
		if ((*recVtxs)[myi].ndof() >= 5 
                && ( fabs((*recVtxs)[myi].z()) <= 24 && fabs((*recVtxs)[myi].position().rho())) <= 2.0)
		{
			myNGoodPrimVtx++;
		}
	}
	nGoodPrimVtx = myNGoodPrimVtx;

	if (recVtxs->begin() != recVtxs->end())  // At least one vertex [Annotated by Eric Wang, 20240704]
	{
		if (addXlessPrimaryVertex_ || resolveAmbiguity_)  // What criterion? [Question from Eric Wang, 20240704]
		{
			thePrimaryV = Vertex(*(recVtxs->begin()));
		}
		else
		{
            // TO_IMPR_CPP11 (for(auto ...)) [Tagged by Eric Wang, 20240704]
			for (reco::VertexCollection::const_iterator vtx = recVtxs->begin(); vtx != recVtxs->end(); ++vtx)
			{
                // Choose the vertex with the most tracks [Annotated by Eric Wang, 20240704]
				if (nVtxTrks < vtx->tracksSize())   
				{
					nVtxTrks = vtx->tracksSize();
					thePrimaryV = Vertex(*vtx);
				}
			}
		}
	}
	else
	{
        // If no vertex is found, use the beam spot as the primary vertex [Annotated by Eric Wang, 20240704]
		thePrimaryV = Vertex(beamSpot.position(), beamSpot.covariance3D());
	}

    // Vertex reconstruction results [Annotated by Eric Wang, 20240704]
	RefVtx = thePrimaryV.position();
	priVtxX = (thePrimaryV.position().x());
	priVtxY = (thePrimaryV.position().y());
	priVtxZ = (thePrimaryV.position().z());
	priVtxXE = (thePrimaryV.xError());
	priVtxYE = (thePrimaryV.yError());
	priVtxZE = (thePrimaryV.zError());
	priVtxChiNorm = (thePrimaryV.normalizedChi2());
	priVtxChi = thePrimaryV.chi2();
	try{
		priVtxCL = ChiSquaredProbability((double)(thePrimaryV.chi2()), (double)(thePrimaryV.ndof()));
	}
	catch (...){
		priVtxCL = -9;
	}
	// priVtxCL = ChiSquaredProbability((double)(thePrimaryV.chi2()), (double)(thePrimaryV.ndof()));
	// 这里去掉了XYZ两两的协方差

	edm::Handle<edm::View<pat::Muon> > thePATMuonHandle; //  MINIAOD
	iEvent.getByToken(gtpatmuonToken_, thePATMuonHandle);
	edm::Handle<edm::View<pat::PackedCandidate> > theTrackHandle;   //  MINIAOD
	iEvent.getByToken(trackToken_,                theTrackHandle);  //  MINIAOD
	std::vector<edm::View<pat::PackedCandidate>::const_iterator> nonMuonKaonTrack;

	// Copy tracks iterators
 	for (edm::View<pat::PackedCandidate>::const_iterator iTrackc = theTrackHandle->begin(); // MINIAOD 
	            iTrackc != theTrackHandle->end(); ++iTrackc                                 )
	{
		nonMuonKaonTrack.push_back(iTrackc);
	}

    // Initialize the muon track block [Annotated by Eric Wang, 20240704]

    #ifdef DISPLAY_STAGE
    puts("Begin muons.");
    #endif
	if (thePATMuonHandle->size() >= 2) // Require at least 4 muons present [Annotated by Eric Wang, 20241214]
	// JUP->4mu+2K, 改回4
	{
		vector<std::string> theInputVariables;
		theInputVariables.push_back("validFrac");
		theInputVariables.push_back("globalChi2");
		theInputVariables.push_back("pt");
		theInputVariables.push_back("eta");
		theInputVariables.push_back("segComp");
		theInputVariables.push_back("chi2LocMom");
		theInputVariables.push_back("chi2LocPos");
		theInputVariables.push_back("glbTrackProb");
		theInputVariables.push_back("NTrkVHits");
		theInputVariables.push_back("NTrkEHitsOut");
		ReadBDT muonID(theInputVariables);
		vector<double> inputValues;
		inputValues.resize(10, 0.);

		// fill muon track block
        // TO_IMPR_CPP11 (for(auto ...)) [Tagged by Eric Wang, 20240704]
		for (edm::View<pat::Muon>::const_iterator iMuonP =  thePATMuonHandle->begin(); //  MINIAOD
			                                      iMuonP != thePATMuonHandle->end(); ++iMuonP)
		{
			// push back all muon information
			++nMu;
			muIsPatLooseMuon->push_back(iMuonP->isLooseMuon());
			muIsPatTightMuon->push_back(iMuonP->isTightMuon(thePrimaryV));
			muIsPatSoftMuon->push_back(iMuonP->isSoftMuon(thePrimaryV));
			muIsPatMediumMuon->push_back(iMuonP->isMediumMuon());

			muPx->push_back(iMuonP->px());
			muPy->push_back(iMuonP->py());
			muPz->push_back(iMuonP->pz());
			muCharge->push_back(iMuonP->charge());
			// 没有引入d0和TrackIso等信息

			int goodSoftMuonNewIlseMod = 0;
			int goodSoftMuonNewIlse = 0;
			int goodLooseMuonNew = 0;
			int goodLooseMuon = 0;
			int goodTightMuon = 0;
			
			
			// Find and delete muon Tracks in Tracks
			for (std::vector<edm::View<pat::PackedCandidate>::const_iterator>::const_iterator iTrackfID  = nonMuonKaonTrack.begin(); // MINIAOD
			                                                                                  iTrackfID != nonMuonKaonTrack.end(); 

                                                                                            ++iTrackfID                             )
			{
				if(iMuonP->track().isNull())
				{
					continue;
				}
				edm::View<pat::PackedCandidate>::const_iterator iTrackf = *(iTrackfID);		

                // Why call the function outside? [Question from Eric Wang, 20240704]
				iMuonP->track()->px();

                // Match using the momentum. [Annotated by Eric Wang, 20240704]                  
				if (   iTrackf->px() == iMuonP->track()->px() 
                    && iTrackf->py() == iMuonP->track()->py() 
                    && iTrackf->pz() == iMuonP->track()->pz())
				{
					nonMuonKaonTrack.erase(iTrackfID);
					iTrackfID = iTrackfID - 1;
				}
			}
			// float mymuMVABs = -1;

            // Check if match any HLT for Jpsi and Upsilon [Annotated by Eric Wang, 20240704]
            // TO_ENC [Tagged by Eric Wang, 20240704]

			bool isJpsiTrigMatch = false;
			bool isJpsiFilterMatch = false;

			for (unsigned int JpsiTrig = 0; JpsiTrig < TriggersForJpsi_.size(); JpsiTrig++)
			{
				if (JpsiMatchTrig[JpsiTrig] != 0)
				{
					isJpsiTrigMatch = true;
				}
			}



			muIsJpsiTrigMatch->push_back(isJpsiTrigMatch);

			for (unsigned int JpsiFilter = 0; JpsiFilter < FiltersForJpsi_.size(); JpsiFilter++){
				if(isJpsiTrigMatch){ // remove hltresults.isValid() for now
					pat::TriggerObjectStandAlone *tempTriggerObject = nullptr;
					for (auto it = iMuonP->triggerObjectMatches().begin(); it != iMuonP->triggerObjectMatches().end(); ++it){
						tempTriggerObject = new pat::TriggerObjectStandAlone(*it);
						tempTriggerObject->unpackFilterLabels(iEvent, *hltresults);
						if(tempTriggerObject->hasFilterLabel(FiltersForJpsi_[JpsiFilter])){
							isJpsiFilterMatch = true;
						}
						delete tempTriggerObject;
					}
				}
			}
			muIsJpsiFilterMatch->push_back(isJpsiFilterMatch);

            // TO_ENC [Tagged by Eric Wang, 20240704]
		    bool isUpsTrigMatch = false;
            bool isUpsFilterMatch = false;

		    for (unsigned int UpsTrig = 0; UpsTrig < TriggersForUpsilon_.size(); UpsTrig++)
		    {
                if (UpsilonMatchTrig[UpsTrig] != 0)
                {
                    isUpsTrigMatch = true;
                }
            }
            muIsUpsTrigMatch->push_back(isUpsTrigMatch);

            for (unsigned int UpsFilter = 0; UpsFilter < FiltersForUpsilon_.size(); UpsFilter++){
                if(isUpsTrigMatch && hltresults.isValid()){
                    pat::TriggerObjectStandAlone *tempTriggerObject = nullptr;
                    for (auto it = iMuonP->triggerObjectMatches().begin(); it != iMuonP->triggerObjectMatches().end(); ++it){
                        tempTriggerObject = new pat::TriggerObjectStandAlone(*it);
                        tempTriggerObject->unpackFilterLabels(iEvent, *hltresults);
                        if(tempTriggerObject->hasFilterLabel(FiltersForUpsilon_[UpsFilter])){
                            isUpsFilterMatch = true;
                        }
                        delete tempTriggerObject;
                    }
                }
            }
            muIsUpsFilterMatch->push_back(isUpsFilterMatch);

			munMatchedSeg->push_back(-1);
		}
	} // if two muons

    #ifdef DISPLAY_STAGE
    puts("Settled muons.");
    #endif

	if (doMC)
	{
		// pion loop

        // Compare direct MC results and RECO results [Annotated by Eric Wang, 20240704]
        //      For those that matches within the precision limit, store the momentum 

        // TO_IMPR_CPP11 (for(auto ...)) [Tagged by Eric Wang, 20240704]
		for (edm::View<pat::PackedCandidate>::const_iterator iTrack = theTrackHandle->begin(); // MINIAOD
			                                                iTrack != theTrackHandle->end(); ++iTrack)
		{
			TLorentzVector RECO_pip4;
			RECO_pip4.SetXYZM(iTrack->px(), iTrack->py(), iTrack->pz(), myKMass);
			if (fabs(MC_pi1p4.Pt() - RECO_pip4.Pt()) < 0.08 * MC_pi1p4.Pt() && MC_pi1p4.DeltaR(RECO_pip4) < 0.1)
			{
				Match_pi1px->push_back(RECO_pip4.Px());
				Match_pi1py->push_back(RECO_pip4.Py());
				Match_pi1pz->push_back(RECO_pip4.Pz());
			}
			if ((fabs(MC_pi2p4.Pt() - RECO_pip4.Pt()) < 0.08 * MC_pi2p4.Pt() && MC_pi2p4.DeltaR(RECO_pip4) < 0.1))
			{
				Match_pi2px->push_back(RECO_pip4.Px());
				Match_pi2py->push_back(RECO_pip4.Py());
				Match_pi2pz->push_back(RECO_pip4.Pz());
			}
		}
	} // if(doMC)

    // Set the mass constraints for further reconstruction. [Annotated by Eric Wang, 20240704]

	// It takes a lot less memory out of the loop
	KinematicConstraint *Jpsi_cs = new MassKinematicConstraint(myJpsiMass, myJpsiMassErr);
	KinematicConstraint *Jpsi_cs34 = new MassKinematicConstraint(myJpsiMass, myJpsiMassErr);

	// 要做mass constrain的话这里要加phi的相关变量
	double pionPTcut = 2;
	double pionDRcut = 0.7;
	double vtxProbPreCut = 1.0e-7;

    // Eta restriction for the quarkonia [Annotated by Eric Wang, 20240704]
    double etaCut = 2.5;

    // booleans marking whether the muon pair satisfies the mass constraint
    bool isJpsiMuPair = false;
    bool isUpsMuPair  = false;
    bool isGoodVtxFit = false;
    bool isInOniaEtaRange = false;

    // Muon factory
    KinematicParticleFactoryFromTransientTrack muPairFactory;

    // [J-U-P] Will only need 4 muons.

    // Will be working reco from 3 pairs of muons. 
	if (thePATMuonHandle->size() < 2)//warning  //Different from JJP
	{
		return;
	}

    // Temporary storage for the muon pair [Annotated by Eric Wang, 20240704]
    std::vector<RefCountedKinematicParticle> transMuonPair;
    std::vector<uint>                        transMuPairId;
    ParticleMass muMass = myMuMass;
    float muMassSigma   = myMuMassErr;
	ParticleMass kaonMass = myKMass;
	float kaonMassSigma = myKMassErr;
    float chi2 = 0.;
	float ndof = 0.;

    /**************************************************************************
     * [Section]
     *      Muon pairing and preselection with vertex fitting
     * [Implementation]
     *      - Loop over all existing muon pairs.
     *      - Apply kinematic and mass window constraints.
     *      - Fit the vertex to judge.
     *      - Classify muons pairs as Jpsi or Upsilon candidates with mass.
     * [Note]
     *      The intermidiate storage for muon pair stores the muons as 
     *      RefCountedKinematicParticle. This saves repeated reco.
    **************************************************************************/

    // Candidates of muon pairs from Jpsi or Upsilon
    using muon_t   = RefCountedKinematicParticle;
    using muList_t = std::pair< vector<muon_t>, vector<uint> >;
    std::vector< muList_t > muPairCand_Jpsi, muPairCand_Ups;
    std::vector< std::pair< muList_t, muList_t > > muQuad_Jpsi_Ups;

	RefCountedKinematicTree     muVtxFitTree;
    RefCountedKinematicParticle muPair_noMC;
    RefCountedKinematicVertex   muVtxFit_noMC;

    #ifdef DISPLAY_STAGE
    puts("Begin muon pairing.");
    #endif

    // Selection for the muon candidates
    for(auto iMuon1 =  thePATMuonHandle->begin(); 
             iMuon1 != thePATMuonHandle->end(); ++iMuon1){
        TrackRef muTrack1 = iMuon1->track();
        if (muTrack1.isNull()){
            continue;
        }
        // Apply a general eta cut: 2.5 for muons.
        if (std::abs(iMuon1->eta()) > etaCut){
            continue;
        }
        // Build transient track and store.
        TransientTrack transTrk1(muTrack1, &(bFieldHandle));
        transMuonPair.push_back(muPairFactory.particle(transTrk1, muMass, chi2, ndof, muMassSigma));
        transMuPairId.push_back(iMuon1 - thePATMuonHandle->begin());

        // Next muon candidate.
        for(auto iMuon2  = iMuon1 + 1; 
                 iMuon2 != thePATMuonHandle->end(); ++iMuon2){
            // Build transient track and store.
            TrackRef muTrack2 = iMuon2->track();
            if (muTrack2.isNull()){
                continue;
            }
            // Apply a general eta cut: 2.5 for muons.
            if (std::abs(iMuon2->eta()) > etaCut){
                continue;
            }
            TransientTrack transTrk2(muTrack2, &(bFieldHandle));
            // Charge requirement.
            if ((iMuon1->charge() + iMuon2->charge()) != 0){
				continue;
			}
            // Dynamics selection. A very crude selection.
            // Involves more calculation and is therefore done after kinematics.
			double muPairMassFromP4 = (iMuon1->p4() + iMuon2->p4()).mass();
            double muPairEtaFromP4  = (iMuon1->p4() + iMuon2->p4()).eta();
            isUpsMuPair  = (8 < muPairMassFromP4 && muPairMassFromP4 < 12);   
            // Apply a general eta cut: 2.5 for quarkonia.
            isInOniaEtaRange = (std::abs(muPairEtaFromP4) < etaCut);

            transMuonPair.push_back(muPairFactory.particle(transTrk2,  muMass, 
                                                           chi2, ndof, muMassSigma) );
            transMuPairId.push_back(iMuon2 - thePATMuonHandle->begin());
            // Judging with vertex fitting.
            if(isInOniaEtaRange &&  isUpsMuPair){
                if(particlesToVtx(transMuonPair)){
                    // Having passed all the checks, store the muon pair.
                    particlesToVtx(muVtxFitTree, transMuonPair, "final muon pair");
                    if(isUpsMuPair){
                        muPairCand_Ups.push_back(
                            std::make_pair(transMuonPair, transMuPairId) );
                    }
                }
            }
            // Clear the transient muon pair for the next pair.
            transMuonPair.pop_back();
            transMuPairId.pop_back();
			// pop_back弹掉最后一个元素，不用重复加入iMuon1了
        }
        transMuonPair.pop_back();
        transMuPairId.pop_back();
    }

	

	/**************************************************************************
     * [Section]
     *      J/psi pairing and preselection with vertex fitting
     * [Implementation]
     *      - Loop over all existing J/psi candidates to create J/psi pairs.
     *      - Fit to same vertex to judge.
     *      - For those that pass, store the indices of the muons.
     * [Note]
     *      The intermidiate storage for muon pair stores the muons as 
     *      RefCountedKinematicParticle. This saves repeated reco.
    **************************************************************************/

	// Classes for the fitting process.
    RefCountedKinematicTree vtxFitTree_Jpsi;

    RefCountedKinematicTree vtxFitTree_Ups;
    RefCountedKinematicTree vtxFitTree_Phi;
    RefCountedKinematicTree vtxFitTree_Pri;

    // Classes for secondary particles (Jpsi and Upsilon)
    RefCountedKinematicParticle Jpsi_Fit_noMC, Ups_Fit_noMC, Phi_Fit_noMC, Pri_Fit_noMC;
    RefCountedKinematicVertex   Jpsi_Vtx_noMC, Ups_Vtx_noMC, Phi_Vtx_noMC, Pri_Vtx_noMC;
    KinematicParameters         Jpsi_Para,     Ups_Para,     Phi_Para,     Pri_Para;
    std::vector< RefCountedKinematicParticle >  interOnia;
	std::vector< RefCountedKinematicParticle >  interOniaJU;

    // Markers for fitting. Only marks if a result is constructed
    bool isValidJpsi, isValidUps, isValidPhi, isValidPri;
    // Fitted mass error is also stricter marker for fitting.
    double tmp_Jpsi_massErr, tmp_Ups_massErr, tmp_Phi_massErr, tmp_Pri_massErr;
    // Temporary storage for particle dynamics.
    double tmp_pt, tmp_eta, tmp_phi;

    #ifdef DISPLAY_STAGE
    puts("Producing Jpsi + Ups");
    #endif

        for(auto muPair_Ups  = muPairCand_Ups.begin();
                 muPair_Ups != muPairCand_Ups.end(); muPair_Ups++){
            // Check if the muon pairs overlap.
			// Start constructing the fit tree.
			// Use particlesToVtx() to fit the quarkonia once more.
                    isValidUps = particlesToVtx(vtxFitTree_Ups,  muPair_Ups->first,    "final Ups");
                    if(isValidUps){
                    // Extract the vertex and the particle parameters from valid results.
                    // Here, when an invalid fit is detected, the massErr is set to -9.
                    extractFitRes(vtxFitTree_Ups, Ups_Fit_noMC, Ups_Vtx_noMC, tmp_Ups_massErr);
                    // Look for "Good Fit". Judge by the massErr.
		    #ifdef DISPLAY_STAGE
                    puts("Found candidate!");
                    #endif

                    // Store the fitting results into temporary vectors for Jpsi 2.
                    getDynamics(Ups_Fit_noMC, tmp_pt, tmp_eta, tmp_phi);
                    Ups_mass->push_back(    Ups_Fit_noMC->currentState().mass());
                    Ups_massDiff->push_back(Ups_Fit_noMC->currentState().mass() - myUpsMass);
                    Ups_massErr->push_back( tmp_Ups_massErr);
                    Ups_ctau->push_back(   GetcTau(   Ups_Vtx_noMC, Ups_Fit_noMC, theBeamSpotV));
                    Ups_ctauErr->push_back(GetcTauErr(Ups_Vtx_noMC, Ups_Fit_noMC, theBeamSpotV));
                    Ups_Chi2->push_back(double(Ups_Vtx_noMC->chiSquared()));
                    Ups_ndof->push_back(double(Ups_Vtx_noMC->degreesOfFreedom()));
                    Ups_VtxProb->push_back(ChiSquaredProbability((double)(Ups_Vtx_noMC->chiSquared()), 
                                                                 (double)(Ups_Vtx_noMC->degreesOfFreedom())));
                    Ups_px->push_back(Ups_Fit_noMC->currentState().kinematicParameters().momentum().x());
                    Ups_py->push_back(Ups_Fit_noMC->currentState().kinematicParameters().momentum().y());
                    Ups_pz->push_back(Ups_Fit_noMC->currentState().kinematicParameters().momentum().z());
                    Ups_phi->push_back(tmp_phi);
                    Ups_eta->push_back(tmp_eta);
                    Ups_pt->push_back(tmp_pt);
			}
        }

	// std::cout << "Start the part of track pair."  << endl;
	// std::cout << "the number of track" << nonMuonKaonTrack.size() << endl;

    #ifdef DISPLAY_STAGE
    puts("Producing track pair.");
    #endif

    // Selection for the Phi candidates

            // Dynamics selection. A very crude s

	// std::cout << "Finish the part of track pair."  << endl;

    /**************************************************************************
    * [Section]
    *      Looking for the final candidates
    * [Implementation]
    *      - Loop over all existing non-muon pairs.
    *      - Assuming all tracks to be Kaons, reconstruct the Phi.
    *      - Judge the Phi candidates with mass window and vertex fitting.
    *      - For those that pass, find the double Jpsi candidates to match.
    *      - Judge the eleborate candidates with vertex fitting.
    * [Note]
    *       
    **************************************************************************/

	
    // Currently: Event
	// std::cout << "the big circle ended" << endl; //testing segmentation
	if(Ups_mass->size() != 0){
            X_One_Tree_->Fill();
	}
	// std::cout << "Finish the part of fitting."  << endl;

	if (Debug_)
	{
	}
    // Reset the vectors [Annotated by Eric Wang, 20240704]
	if (doMC)
	{
		MC_X_px->clear();
		MC_X_py->clear();
		MC_X_pz->clear();
		MC_X_mass->clear();
		MC_X_chg->clear();
		MC_Dau_JpsipdgId->clear();
		MC_Dau_Jpsipx->clear();
		MC_Dau_Jpsipy->clear();
		MC_Dau_Jpsipz->clear();
		MC_Dau_Jpsimass->clear();
		MC_Dau_psi2spdgId->clear();
		MC_Dau_psi2spx->clear();
		MC_Dau_psi2spy->clear();
		MC_Dau_psi2spz->clear();
		MC_Dau_psi2smass->clear();
		MC_Granddau_mu1pdgId->clear();
		MC_Granddau_mu1px->clear();
		MC_Granddau_mu1py->clear();
		MC_Granddau_mu1pz->clear();
		MC_Granddau_mu2pdgId->clear();
		MC_Granddau_mu2px->clear();
		MC_Granddau_mu2py->clear();
		MC_Granddau_mu2pz->clear();
		MC_Granddau_JpsipdgId->clear();
		MC_Granddau_Jpsipx->clear();
		MC_Granddau_Jpsipy->clear();
		MC_Granddau_Jpsipz->clear();
		MC_Granddau_Jpsimass->clear();
		MC_Granddau_pi1pdgId->clear();
		MC_Granddau_pi1px->clear();
		MC_Granddau_pi1py->clear();
		MC_Granddau_pi1pz->clear();
		MC_Granddau_pi2pdgId->clear();
		MC_Granddau_pi2px->clear();
		MC_Granddau_pi2py->clear();
		MC_Granddau_pi2pz->clear();
		MC_Grandgranddau_mu3pdgId->clear();
		MC_Grandgranddau_mu3px->clear();
		MC_Grandgranddau_mu3py->clear();
		MC_Grandgranddau_mu3pz->clear();
		MC_Grandgranddau_mu4pdgId->clear();
		MC_Grandgranddau_mu4px->clear();
		MC_Grandgranddau_mu4py->clear();
		MC_Grandgranddau_mu4pz->clear();

		Match_mu1px->clear();
		Match_mu1py->clear();
		Match_mu1pz->clear();
		Match_mu2px->clear();
		Match_mu2py->clear();
		Match_mu2pz->clear();
		Match_mu3px->clear();
		Match_mu3py->clear();
		Match_mu3pz->clear();
		Match_mu4px->clear();
		Match_mu4py->clear();
		Match_mu4pz->clear();

		Match_pi1px->clear();
		Match_pi1py->clear();
		Match_pi1pz->clear();
		Match_pi2px->clear();
		Match_pi2py->clear();
		Match_pi2pz->clear();
	}

	trigRes->clear();
	trigNames->clear();
	L1TT->clear();
	MatchJpsiTrigNames->clear();
    MatchUpsTrigNames->clear();
	muIsJpsiTrigMatch->clear();
	muIsJpsiFilterMatch->clear();
	muIsUpsTrigMatch->clear();
	muIsUpsFilterMatch->clear();
	runNum = 0;
	evtNum = 0;
	lumiNum = 0;
	nGoodPrimVtx = 0;
	priVtxX = 0;
	priVtxY = 0;
	priVtxZ = 0;
	priVtxXE = 0;
	priVtxYE = 0;
	priVtxZE = 0;
	priVtxChiNorm = 0;
	priVtxChi = 0;
	priVtxCL = 0;
	// mybxlumicorr = 0;
	// myrawbxlumi = 0;
	PriVtxXCorrX->clear();
	PriVtxXCorrY->clear();
	PriVtxXCorrZ->clear();
	PriVtxXCorrEX->clear();
	PriVtxXCorrEY->clear();
	PriVtxXCorrEZ->clear();
	PriVtxXCorrC2->clear();
	PriVtxXCorrCL->clear();

	nMu = 0;
	muPx->clear();
	muPy->clear();
	muPz->clear();
	muD0->clear();
	muD0E->clear();
	muDz->clear();
	muChi2->clear();
	muGlChi2->clear();
	mufHits->clear();
	muFirstBarrel->clear();
	muFirstEndCap->clear();
	muDzVtx->clear();
	muDxyVtx->clear();
	muNDF->clear();
	muGlNDF->clear();
	muPhits->clear();
	muShits->clear();
	muGlMuHits->clear();
	muType->clear();
	muQual->clear();
	muTrack->clear();
	muCharge->clear();
	muIsoratio->clear();
	muIsGoodLooseMuon->clear();
	muIsGoodLooseMuonNew->clear();
	muIsGoodSoftMuonNewIlse->clear();
	muIsGoodSoftMuonNewIlseMod->clear();
	muIsGoodTightMuon->clear();
	munMatchedSeg->clear();
	muMVAMuonID->clear();
	musegmentCompatibility->clear();

	mupulldXdZ_pos_noArb->clear();
	mupulldYdZ_pos_noArb->clear();
	mupulldXdZ_pos_ArbDef->clear();
	mupulldYdZ_pos_ArbDef->clear();
	mupulldXdZ_pos_ArbST->clear();
	mupulldYdZ_pos_ArbST->clear();
	mupulldXdZ_pos_noArb_any->clear();
	mupulldYdZ_pos_noArb_any->clear();

	muIsPatLooseMuon->clear();
	muIsPatTightMuon->clear();
	muIsPatSoftMuon->clear();
	muIsPatMediumMuon->clear();

	Jpsi_cand_mass_p4->clear();
    Jpsi_cand_mass_fit->clear();


    Pri_mass->clear();
    Pri_massErr->clear();
    Pri_ctau->clear();
    Pri_ctauErr->clear();
    Pri_Chi2->clear();
    Pri_ndof->clear();
    Pri_VtxProb->clear();
    Pri_px->clear();
    Pri_py->clear();
    Pri_pz->clear();
    Pri_phi->clear();
    Pri_eta->clear();
    Pri_pt->clear();


    Jpsi_mass->clear();
    Jpsi_massErr->clear();
    Jpsi_massDiff->clear();
    Jpsi_ctau->clear();
    Jpsi_ctauErr->clear();
    Jpsi_Chi2->clear();
    Jpsi_ndof->clear();
    Jpsi_VtxProb->clear();
    Jpsi_px->clear();
    Jpsi_py->clear();
    Jpsi_pz->clear();
    Jpsi_phi->clear();
    Jpsi_eta->clear();
    Jpsi_pt->clear();
    Jpsi_mu_1_Idx->clear();
    Jpsi_mu_2_Idx->clear();

    Ups_mass->clear();
    Ups_massErr->clear();
    Ups_massDiff->clear();
    Ups_ctau->clear();
    Ups_ctauErr->clear();
    Ups_Chi2->clear();
    Ups_ndof->clear();
    Ups_VtxProb->clear();
    Ups_px->clear();
    Ups_py->clear();
    Ups_pz->clear();
    Ups_phi->clear();
    Ups_eta->clear();
    Ups_pt->clear();
    Ups_mu_1_Idx->clear();
    Ups_mu_2_Idx->clear();


    Phi_mass->clear();
    Phi_massErr->clear();
    Phi_massDiff->clear();
	Phi_ctau->clear();
    Phi_ctauErr->clear();
    Phi_Chi2->clear();
    Phi_ndof->clear();
    Phi_VtxProb->clear();
    Phi_px->clear();
    Phi_py->clear();
    Phi_pz->clear();
    Phi_phi->clear();
    Phi_eta->clear();
    Phi_pt->clear();

    Phi_K_1_Idx->clear();

    Phi_K_1_px->clear();
    Phi_K_1_py->clear();
    Phi_K_1_pz->clear();
    Phi_K_1_phi->clear();
    Phi_K_1_eta->clear();
    Phi_K_1_pt->clear();

    Phi_K_2_Idx->clear();
    Phi_K_2_px->clear();
    Phi_K_2_py->clear();
    Phi_K_2_pz->clear();
    Phi_K_2_phi->clear();
    Phi_K_2_eta->clear();
    Phi_K_2_pt->clear();

} // analyze
// 

/******************************************************************************
 * [Name of function]  
 *      tracksToMuonPair
 * [Description]  
 *      Construct muons from tracks.
 *      Assuming muon mass and mass error as PDG 2023 values.
 *      Adds reconstructed muons to the arg_MuonResults.
 * [Parameters]
 *      vector<RefCountedKinematicParticle>&        arg_MuonResults
 *          - The vector to which reconstructed muons are added.
 *      KinematicParticleFactoryFromTransientTrack& arg_MuFactory
 *          - The class used to reconstruct muons.
 *      const MagneticField&                        arg_bField,
 *          - Magnetic field used in reconstruction.
 *      const TrackRef&                             arg_Trk1, arg_Trk2  
 *          - Tracks identified as muons.      
 * [Return value]
 *      (void)
 * [Note]
 *          
******************************************************************************/

void MultiLepPAT::getDynamics(double  arg_mass, double  arg_px,  double  arg_py, double arg_pz,
                              double& res_pt,   double& res_eta, double& res_phi){
    TLorentzVector myParticle;
    myParticle.SetXYZM(arg_px, arg_py, arg_pz, arg_mass);
    res_pt  = myParticle.Pt();
    res_eta = myParticle.Eta();
    res_phi = myParticle.Phi();
}

/******************************************************************************
 * [Name of function]  
 *      tracksToMuonPair
 * [Description]  
 *      Construct muons from tracks.
 *      Assuming muon mass and mass error as PDG 2023 values.
 *      Adds reconstructed muons to the arg_MuonResults.
 * [Parameters]
 *      vector<RefCountedKinematicParticle>&        arg_MuonResults
 *          - The vector to which reconstructed muons are added.
 *      KinematicParticleFactoryFromTransientTrack& arg_MuFactory
 *          - The class used to reconstruct muons.
 *      const MagneticField&                        arg_bField,
 *          - Magnetic field used in reconstruction.
 *      const TrackRef&                             arg_Trk1, arg_Trk2  
 *          - Tracks identified as muons.      
 * [Return value]
 *      (void)
 * [Note]
 *          
******************************************************************************/
void MultiLepPAT::getDynamics(const RefCountedKinematicParticle& arg_Part,
                              double& res_pt,   double& res_eta, double& res_phi){
    getDynamics(arg_Part->currentState().mass(), 
                arg_Part->currentState().kinematicParameters().momentum().x(),
                arg_Part->currentState().kinematicParameters().momentum().y(),
                arg_Part->currentState().kinematicParameters().momentum().z(),
                res_pt, res_eta, res_phi);
}

/******************************************************************************
 * [Name of function]  
 *      tracksToMuonPair
 * [Description]  
 *      Construct muons from tracks.
 *      Assuming muon mass and mass error as PDG 2023 values.
 *      Adds reconstructed muons to the arg_MuonResults.
 * [Parameters]
 *      vector<RefCountedKinematicParticle>&        arg_MuonResults
 *          - The vector to which reconstructed muons are added.
 *      KinematicParticleFactoryFromTransientTrack& arg_MuFactory
 *          - The class used to reconstruct muons.
 *      const MagneticField&                        arg_bField,
 *          - Magnetic field used in reconstruction.
 *      const TrackRef&                             arg_Trk1, arg_Trk2  
 *          - Tracks identified as muons.      
 * [Return value]
 *      (void)
 * [Note]
 *          
******************************************************************************/
void MultiLepPAT::tracksToMuonPair(vector<RefCountedKinematicParticle>&        arg_MuonResults,
                                   KinematicParticleFactoryFromTransientTrack& arg_MuFactory,
                                   const MagneticField&                        arg_bField,
                                   const TrackRef arg_Trk1,     const TrackRef arg_Trk2        ){
    TransientTrack transTrk1(arg_Trk1, &(arg_bField));
    TransientTrack transTrk2(arg_Trk2, &(arg_bField));
    // Parameters for muon
    ParticleMass muMass = myMuMass;
    float muMassSigma   = myMuMassErr;
    float chi2 = 0.;
	float ndof = 0.;
    arg_MuonResults.push_back(arg_MuFactory.particle(transTrk1, muMass, chi2, ndof, muMassSigma));
    arg_MuonResults.push_back(arg_MuFactory.particle(transTrk2, muMass, chi2, ndof, muMassSigma));
    return ;
}

/******************************************************************************
 * [Name of function]  
 *      particlesToVtx
 * [Description]  
 *      Construct muons from tracks.
 *      Assuming muon mass and mass error as PDG 2023 values.
 *      Adds reconstructed muons to the arg_FromParticles.
 *      Adds reconstructed muons to the arg_FromParticles.
 * [Parameters]
 *      vector<RefCountedKinematicParticle>&        arg_FromParticles
 *          - The vector to which reconstructed particles are added.
 *      vector<RefCountedKinematicParticle>&        arg_FromParticles
 *          - The vector to which reconstructed particles are added.
 * [Return value]
 *      (void)
 * [Note]
 *      A "silent" version of fitting particles to vertex. No error message
 *      will be printed in case of failed fitting.
******************************************************************************/

bool MultiLepPAT::particlesToVtx(const vector<RefCountedKinematicParticle>&  arg_FromParticles){
    KinematicParticleVertexFitter fitter;
    RefCountedKinematicTree vertexFitTree;
    bool fitError = false;
	double vtxprob = 0;
    try{
        vertexFitTree = fitter.fit(arg_FromParticles);
    }catch(...){
        fitError = true;
    }
	if (fitError || !vertexFitTree->isValid()){
        return false;
    }
	RefCountedKinematicVertex vFit_vertex_noMC = vertexFitTree->currentDecayVertex();
    try{
        vtxprob = ChiSquaredProbability((double)(vFit_vertex_noMC->chiSquared()), (double)(vFit_vertex_noMC->degreesOfFreedom()));
    }catch(...){
        vtxprob = 0.0;
    }   

    return (vtxprob >= VtxProbCut);
}

/******************************************************************************
 * [Name of function]  
 *      particlesToVtx
 * [Description]  
 *      Construct muons from tracks.
 *      Assuming muon mass and mass error as PDG 2023 values.
 *      Adds reconstructed muons to the arg_FromParticles.
 * [Parameters]
 *      vector<RefCountedKinematicParticle>&        arg_FromParticles
 *          - The vector to which reconstructed particles are added.
 *      const string&                               arg_Message  
 *          - The message to be displayed in case of error.
 * [Return value]
 *      (void)
 * [Note]
 *      This definition uses an "implicit" VertexFitter and KinematicTree. 
******************************************************************************/

bool MultiLepPAT::particlesToVtx(const vector<RefCountedKinematicParticle>&  arg_FromParticles,
                                 const string&                               arg_Message){
	KinematicParticleVertexFitter fitter;
    RefCountedKinematicTree vertexFitTree;
    bool fitError = false;
	double vtxprob = 0;
    try{
        vertexFitTree = fitter.fit(arg_FromParticles);
    }catch(...){
        fitError = true;
		std::cout << "[Fit Error] " << arg_Message <<  std::endl;
    }
	if (fitError || !vertexFitTree->isValid()){
        return false;
    }
    RefCountedKinematicVertex vFit_vertex_noMC = vertexFitTree->currentDecayVertex();
    try{
        vtxprob = ChiSquaredProbability((double)(vFit_vertex_noMC->chiSquared()), (double)(vFit_vertex_noMC->degreesOfFreedom()));
    }catch(...){
        vtxprob = 0.0;
    }   

    return (vtxprob >= VtxProbCut);
}

/******************************************************************************
 * [Name of function]  
 *      particlesToVtx
 * [Description]  
 *      Construct muons from tracks.
 *      Assuming muon mass and mass error as PDG 2023 values.
 *      Adds reconstructed muons to the arg_FromParticles.
 * [Parameters]
 *      vector<RefCountedKinematicParticle>&        arg_FromParticles
 *          - The vector to which reconstructed particles are added.
 *      const string&                               arg_Message  
 *          - The message to be displayed in case of error.
 *      RefCountedKinematicTree&                    arg_VertexFitTree
 *          - The KinematicTree to which the vertex fit is added.    
 * [Return value]
 *      (void)
 * [Note]
 *      This definition uses an "explicit" KinematicTree.
 *      The KinematicTree is passed as an argument and is modified after call.
******************************************************************************/

bool MultiLepPAT::particlesToVtx(RefCountedKinematicTree&                    arg_VertexFitTree,
                                 const vector<RefCountedKinematicParticle>&  arg_FromParticles,
                                 const string&                               arg_Message){
	KinematicParticleVertexFitter fitter;
	// std::cout<<"1"<<std::endl;
    bool fitError = false;

	double vtxprob = 0;
    try{
        arg_VertexFitTree = fitter.fit(arg_FromParticles);
	//	std::cout<<"3"<<std::endl;
    }catch(...){
        fitError = true;
	//	std::cout<<"4"<<std::endl;
	//	std::cout << "[Fit Error] " << arg_Message <<  std::endl;
    }
	if (fitError || !arg_VertexFitTree->isValid()){
        return false;
    }
    RefCountedKinematicVertex vFit_vertex_noMC = arg_VertexFitTree->currentDecayVertex();
    try{
        vtxprob = ChiSquaredProbability((double)(vFit_vertex_noMC->chiSquared()), (double)(vFit_vertex_noMC->degreesOfFreedom()));
    }catch(...){
        vtxprob = 0.0;
    }   

    return (vtxprob >= VtxProbCut);
}

/******************************************************************************
 * [Name of function]  
 *      extractFitRes
 * [Description]
 *      Extract kinematic parameters and other results from a KinematicTree.
 *      Calls movePointerToTheTop() .
 * [Parameters]
 *      RefCountedKinematicTree&     arg_VtxTree
 *          - The KinematicTree constructed from fitting.
 *      RefCountedKinematicParticle& res_Part
 *          - The mother particle extracted from arg_VtxTree.
 *      RefCountedKinematicVertex&   res_Vtx
 *          - The primary vertex extracted from arg_VtxTree.
 *      KinematicParameters&         res_Param
 *          - The kinematic parameters of the mother particle.
 *      double&                      res_MassErr
 *          - The mass error of the mother particle.
 * [Return value]
 *      (bool)
 *          - True if the mass error squared is non-negative.
 * [Note]
 *      - Used when the resulting dynamics is important.
 *      - Requires the KinematicTree to be valid.
 *      - Requires "explicit" particle and vertex.
 *      - The mass error is set to -9 if the mass error squared is negative.
******************************************************************************/

bool MultiLepPAT::extractFitRes(RefCountedKinematicTree&     arg_VtxTree,
                                RefCountedKinematicParticle& res_Part,
                                RefCountedKinematicVertex&   res_Vtx,
                                KinematicParameters&         res_Param,
                                double&                      res_MassErr){
    double tmp_MassErr2 = 0.0;
    arg_VtxTree->movePointerToTheTop();
    // Extract particle and vertex.
    res_Part  = arg_VtxTree->currentParticle();
    res_Vtx   = arg_VtxTree->currentDecayVertex();
    // Obtain mass error squared and other parameters for the vertex.
    res_Param    = res_Part->currentState().kinematicParameters();
    tmp_MassErr2 = res_Part->currentState().kinematicParametersError().matrix()(6, 6);
    // Judge if the fit have been a good fit.
    if(tmp_MassErr2 < 0.0){
        res_MassErr = -9;
    }
    else{
        res_MassErr = std::sqrt(tmp_MassErr2);
    }
    return (res_MassErr >= 0.0);
}

/******************************************************************************
 * [Name of function]  
 *      extractFitRes
 * [Description]
 *      Extract kinematic parameters and other results from a KinematicTree.
 *      Calls movePointerToTheTop() .
 * [Parameters]
 *      RefCountedKinematicTree&     arg_VtxTree
 *          - The KinematicTree constructed from fitting.
 *      RefCountedKinematicParticle& res_Part
 *          - The mother particle extracted from arg_VtxTree.
 *      RefCountedKinematicVertex&   res_Vtx
 *          - The primary vertex extracted from arg_VtxTree.
 *      double&                      res_MassErr
 *          - The mass error of the mother particle.
 * [Return value]
 *      (bool)
 *          - True if the mass error squared is non-negative.
 * [Note]
 *      - Used when the resulting dynamics is important.
 *      - Requires the KinematicTree to be valid.
 *      - Requires "explicit" particle and vertex.
 *      - The mass error is set to -9 if the mass error squared is negative.
******************************************************************************/

bool MultiLepPAT::extractFitRes(RefCountedKinematicTree&     arg_VtxTree,
                                RefCountedKinematicParticle& res_Part,
                                RefCountedKinematicVertex&   res_Vtx,
                                double&                      res_MassErr){
    double tmp_MassErr2 = 0.0;
    arg_VtxTree->movePointerToTheTop();
    // Extract particle and vertex.
    res_Part  = arg_VtxTree->currentParticle();
    res_Vtx   = arg_VtxTree->currentDecayVertex();
    // Obtain mass error squared and other parameters for the vertex.
    tmp_MassErr2 = res_Part->currentState().kinematicParametersError().matrix()(6, 6);
    // Judge if the fit have been a good fit.
    if(tmp_MassErr2 < 0.0){
        res_MassErr = -9;
    }
    else{
        res_MassErr = std::sqrt(tmp_MassErr2);
    }
    return (res_MassErr >= 0.0);
}

/******************************************************************************
 * [Name of function]  
 *      extractFitRes
 * [Description]
 *      Extract kinematic parameters and other results from a KinematicTree.
 *      Calls movePointerToTheTop() .
 * [Parameters]
 *      RefCountedKinematicTree&     arg_VtxTree
 *          - The KinematicTree constructed from fitting.
 *      RefCountedKinematicVertex&   res_Vtx
 *          - The primary vertex extracted from arg_VtxTree.
 *      double&                      res_VtxProb
 *          - The vertex probability deduced from res_Vtx parameters.
 * [Return value]
 *      (bool, always true)
 * [Note]
 *      - Used when only the vertex probability is important.
 *      - Requires the KinematicTree to be valid.
 *      - Requires "explicit" vertex.
******************************************************************************/

bool MultiLepPAT::extractFitRes(RefCountedKinematicTree&     arg_VtxTree,
                                RefCountedKinematicVertex&   res_Vtx,
                                double&                      res_VtxProb){
    arg_VtxTree->movePointerToTheTop();
    // Extract particle and vertex.
    res_Vtx   = arg_VtxTree->currentDecayVertex();
    // Obtain mass error squared and other parameters for the vertex.
	try{
    res_VtxProb = ChiSquaredProbability((double)(res_Vtx->chiSquared()), 
                                        (double)(res_Vtx->degreesOfFreedom()));
	}catch(...){
		res_VtxProb = -9;
	}
    return true;
}

/******************************************************************************
 * [Name of function]  
 *      isOverlapPair
 * [Description]
 *      Check if two muon pairs overlap from the muon indices.
 * [Parameters]
 *      const muList_t& arg_MuonPair1, arg_MuonPair2
 *          - The muon pairs to be compared.
 * [Return value]
 *      (bool)
 *          - True if the two muon pairs do overlap.
 * [Note]
 *      - Uses muList_t defined in the header.
******************************************************************************/

bool MultiLepPAT::isOverlapPair(const muList_t& arg_MuonPair1, 
                                const muList_t& arg_MuonPair2 ){
    return (arg_MuonPair1.second[0] == arg_MuonPair2.second[0] || 
            arg_MuonPair1.second[0] == arg_MuonPair2.second[1] || 
            arg_MuonPair1.second[1] == arg_MuonPair2.second[0] || 
            arg_MuonPair1.second[1] == arg_MuonPair2.second[1]   );
}

/******************************************************************************
 * [Name of function]  
 *      fitResEval
 * [Description]
 *      Evaluate a fitting result from the mass and mass error.
 * [Parameters]
 *      double arg_mass_Jpsi, arg_mass_Ups, arg_mass_Ups
 *          - The mass of the particles.
 *      double arg_massErr_Jpsi, arg_massErr_Ups, arg_massErr_Ups
 *          - The mass error of the particles.
 * [Return value]
 *      (double)
 *          - Evaluation result.
 * [Note]
 *      To be used in resolving "multi-candidate" events.
******************************************************************************/


double MultiLepPAT::fitResEval(double arg_massDiff_Jpsi, double arg_massErr_Jpsi,
                               double arg_massDiff_Ups,  double arg_massErr_Ups,
                               double arg_massDiff_Phi,  double arg_massErr_Phi   ){
    return arg_massDiff_Jpsi * arg_massDiff_Jpsi / (arg_massErr_Jpsi * arg_massErr_Jpsi) +
           arg_massDiff_Ups  * arg_massDiff_Ups  / (arg_massErr_Ups  * arg_massErr_Ups) +
           arg_massDiff_Phi  * arg_massDiff_Phi  / (arg_massErr_Phi  * arg_massErr_Phi) ;
}

// ------------ method called once each job just before starting event loop  ------------
void MultiLepPAT::beginRun(edm::Run const &iRun, edm::EventSetup const &iSetup)
{
	// bool changed = true;
	// proccessName_="HLT";
	// hltConfig_.init(iRun,iSetup,proccessName_,changed);
}

void MultiLepPAT::beginJob()
{
	edm::Service<TFileService> fs;

	// estree_ = fs->make<TTree>("eventSummary", "General Event Summary");
	X_One_Tree_ = fs->make<TTree>("X_data", "X(3872) Data");

	X_One_Tree_->Branch("TrigRes", &trigRes);
	X_One_Tree_->Branch("TrigNames", &trigNames);
	X_One_Tree_->Branch("MatchJpsiTriggerNames", &MatchJpsiTrigNames);
    X_One_Tree_->Branch("MatchUpsTriggerNames", &MatchUpsTrigNames);
	X_One_Tree_->Branch("L1TrigRes", &L1TT);

	X_One_Tree_->Branch("evtNum", &evtNum, "evtNum/i");
	X_One_Tree_->Branch("runNum", &runNum, "runNum/i");
	X_One_Tree_->Branch("lumiNum", &lumiNum, "lumiNum/i");
	X_One_Tree_->Branch("nGoodPrimVtx", &nGoodPrimVtx, "nGoodPrimVtx/i");

	// inst. lumi is here
	//X_One_Tree_->Branch("mybxlumicorr", &mybxlumicorr, "mybxlumicorr/f");
	//X_One_Tree_->Branch("myrawbxlumi", &myrawbxlumi, "myrawbxlumi/f");

	X_One_Tree_->Branch("priVtxX", &priVtxX, "priVtxX/f");
	X_One_Tree_->Branch("priVtxY", &priVtxY, "priVtxY/f");
	X_One_Tree_->Branch("priVtxZ", &priVtxZ, "priVtxZ/f");
	X_One_Tree_->Branch("priVtxXE", &priVtxXE, "priVtxXE/f");
	X_One_Tree_->Branch("priVtxYE", &priVtxYE, "priVtxYE/f");
	X_One_Tree_->Branch("priVtxZE", &priVtxZE, "priVtxZE/f");
	X_One_Tree_->Branch("priVtxChiNorm", &priVtxChiNorm, "priVtxChiNorm/f");
	X_One_Tree_->Branch("priVtxChi", &priVtxChi, "priVtxChi/f");
	X_One_Tree_->Branch("priVtxCL", &priVtxCL, "priVtxCL/f");

	X_One_Tree_->Branch("PriVtxXCorrX", &PriVtxXCorrX);
	X_One_Tree_->Branch("PriVtxXCorrY", &PriVtxXCorrY);
	X_One_Tree_->Branch("PriVtxXCorrZ", &PriVtxXCorrZ);
	X_One_Tree_->Branch("PriVtxXCorrEX", &PriVtxXCorrEX);
	X_One_Tree_->Branch("PriVtxXCorrEY", &PriVtxXCorrEY);
	X_One_Tree_->Branch("PriVtxXCorrEZ", &PriVtxXCorrEZ);
	X_One_Tree_->Branch("PriVtxXCorrC2", &PriVtxXCorrC2);
	X_One_Tree_->Branch("PriVtxXCorrCL", &PriVtxXCorrCL);

	X_One_Tree_->Branch("nMu", &nMu, "nMu/i");
	X_One_Tree_->Branch("muPx", &muPx);
	X_One_Tree_->Branch("muPy", &muPy);
	X_One_Tree_->Branch("muPz", &muPz);
	X_One_Tree_->Branch("muD0", &muD0);
	X_One_Tree_->Branch("muD0E", &muD0E);
	X_One_Tree_->Branch("muDz", &muDz);
	X_One_Tree_->Branch("muChi2", &muChi2);
	X_One_Tree_->Branch("muGlChi2", &muGlChi2);
	X_One_Tree_->Branch("mufHits", &mufHits);
	X_One_Tree_->Branch("muFirstBarrel", &muFirstBarrel);
	X_One_Tree_->Branch("muFirstEndCap", &muFirstEndCap);
	X_One_Tree_->Branch("muDzVtx", &muDzVtx);
	X_One_Tree_->Branch("muDxyVtx", &muDxyVtx);
	X_One_Tree_->Branch("muNDF", &muNDF);
	X_One_Tree_->Branch("muGlNDF", &muGlNDF);
	X_One_Tree_->Branch("muPhits", &muPhits);
	X_One_Tree_->Branch("muShits", &muShits);
	X_One_Tree_->Branch("muGlMuHits", &muGlMuHits);
	X_One_Tree_->Branch("muType", &muType);
	X_One_Tree_->Branch("muQual", &muQual);
	X_One_Tree_->Branch("muTrack", &muTrack);
	X_One_Tree_->Branch("muCharge", &muCharge);
	X_One_Tree_->Branch("muIsoratio", &muIsoratio);
	X_One_Tree_->Branch("munMatchedSeg", &munMatchedSeg);
	X_One_Tree_->Branch("muIsGoodSoftMuonNewIlseMod", &muIsGoodSoftMuonNewIlseMod);
	X_One_Tree_->Branch("muIsGoodSoftMuonNewIlse", &muIsGoodSoftMuonNewIlse);
	X_One_Tree_->Branch("muIsGoodLooseMuonNew", &muIsGoodLooseMuonNew);
	X_One_Tree_->Branch("muIsGoodLooseMuon", &muIsGoodLooseMuon);
	X_One_Tree_->Branch("muIsGoodTightMuon", &muIsGoodTightMuon);

	X_One_Tree_->Branch("muIsPatLooseMuon", &muIsPatLooseMuon);
	X_One_Tree_->Branch("muIsPatTightMuon", &muIsPatTightMuon);
	X_One_Tree_->Branch("muIsPatSoftMuon", &muIsPatSoftMuon);
	X_One_Tree_->Branch("muIsPatMediumMuon", &muIsPatMediumMuon);

	X_One_Tree_->Branch("muIsJpsiTrigMatch", &muIsJpsiTrigMatch);
	X_One_Tree_->Branch("muIsUpsTrigMatch", &muIsUpsTrigMatch);
	X_One_Tree_->Branch("muIsJpsiFilterMatch", &muIsJpsiFilterMatch);
    X_One_Tree_->Branch("muIsUpsFilterMatch", &muIsUpsFilterMatch);
	X_One_Tree_->Branch("muMVAMuonID", &muMVAMuonID);
	X_One_Tree_->Branch("musegmentCompatibility", &musegmentCompatibility);

	X_One_Tree_->Branch("mupulldXdZ_pos_noArb", &mupulldXdZ_pos_noArb);
	X_One_Tree_->Branch("mupulldYdZ_pos_noArb", &mupulldYdZ_pos_noArb);
	X_One_Tree_->Branch("mupulldXdZ_pos_ArbDef", &mupulldXdZ_pos_ArbDef);
	X_One_Tree_->Branch("mupulldYdZ_pos_ArbDef", &mupulldYdZ_pos_ArbDef);
	X_One_Tree_->Branch("mupulldXdZ_pos_ArbST", &mupulldXdZ_pos_ArbST);
	X_One_Tree_->Branch("mupulldYdZ_pos_ArbST", &mupulldYdZ_pos_ArbST);
	X_One_Tree_->Branch("mupulldXdZ_pos_noArb_any", &mupulldXdZ_pos_noArb_any);
	X_One_Tree_->Branch("mupulldYdZ_pos_noArb_any", &mupulldYdZ_pos_noArb_any);

	X_One_Tree_->Branch("Jpsi_cand_mass_p4", &Jpsi_cand_mass_p4);
    X_One_Tree_->Branch("Jpsi_cand_mass_fit", &Jpsi_cand_mass_fit);

    X_One_Tree_->Branch("Jpsi_mass", &Jpsi_mass);
    X_One_Tree_->Branch("Jpsi_massErr", &Jpsi_massErr);
    X_One_Tree_->Branch("Jpsi_massDiff", &Jpsi_massDiff);
    X_One_Tree_->Branch("Jpsi_ctau", &Jpsi_ctau);
    X_One_Tree_->Branch("Jpsi_ctauErr", &Jpsi_ctauErr);
    X_One_Tree_->Branch("Jpsi_Chi2", &Jpsi_Chi2);
    X_One_Tree_->Branch("Jpsi_ndof", &Jpsi_ndof);
    X_One_Tree_->Branch("Jpsi_VtxProb", &Jpsi_VtxProb);
    X_One_Tree_->Branch("Jpsi_px", &Jpsi_px);
    X_One_Tree_->Branch("Jpsi_py", &Jpsi_py);
    X_One_Tree_->Branch("Jpsi_pz", &Jpsi_pz);
    X_One_Tree_->Branch("Jpsi_phi", &Jpsi_phi);
    X_One_Tree_->Branch("Jpsi_eta", &Jpsi_eta);
    X_One_Tree_->Branch("Jpsi_pt", &Jpsi_pt);
    X_One_Tree_->Branch("Jpsi_mu_1_Idx", &Jpsi_mu_1_Idx);
    X_One_Tree_->Branch("Jpsi_mu_2_Idx", &Jpsi_mu_2_Idx);


    X_One_Tree_->Branch("Ups_mass", &Ups_mass);
    X_One_Tree_->Branch("Ups_massErr", &Ups_massErr);
    X_One_Tree_->Branch("Ups_massDiff", &Ups_massDiff);
    X_One_Tree_->Branch("Ups_ctau", &Ups_ctau);
    X_One_Tree_->Branch("Ups_ctauErr", &Ups_ctauErr);
    X_One_Tree_->Branch("Ups_Chi2", &Ups_Chi2);
    X_One_Tree_->Branch("Ups_ndof", &Ups_ndof);
    X_One_Tree_->Branch("Ups_VtxProb", &Ups_VtxProb);
    X_One_Tree_->Branch("Ups_px", &Ups_px);
    X_One_Tree_->Branch("Ups_py", &Ups_py);
    X_One_Tree_->Branch("Ups_pz", &Ups_pz);
    X_One_Tree_->Branch("Ups_phi", &Ups_phi);
    X_One_Tree_->Branch("Ups_eta", &Ups_eta);
    X_One_Tree_->Branch("Ups_pt", &Ups_pt);
    X_One_Tree_->Branch("Ups_mu_1_Idx", &Ups_mu_1_Idx);
    X_One_Tree_->Branch("Ups_mu_2_Idx", &Ups_mu_2_Idx);

    X_One_Tree_->Branch("Phi_mass", &Phi_mass);
    X_One_Tree_->Branch("Phi_massErr", &Phi_massErr);
    X_One_Tree_->Branch("Phi_massDiff", &Phi_massDiff);
	X_One_Tree_->Branch("Phi_ctau", &Phi_ctau);
    X_One_Tree_->Branch("Phi_ctauErr", &Phi_ctauErr);
    X_One_Tree_->Branch("Phi_Chi2", &Phi_Chi2);
    X_One_Tree_->Branch("Phi_ndof", &Phi_ndof);
    X_One_Tree_->Branch("Phi_VtxProb", &Phi_VtxProb);
    X_One_Tree_->Branch("Phi_px", &Phi_px);
    X_One_Tree_->Branch("Phi_py", &Phi_py);
    X_One_Tree_->Branch("Phi_pz", &Phi_pz);
    X_One_Tree_->Branch("Phi_phi", &Phi_phi);
    X_One_Tree_->Branch("Phi_eta", &Phi_eta);
    X_One_Tree_->Branch("Phi_pt", &Phi_pt);
    X_One_Tree_->Branch("Phi_K_1_Idx", &Phi_K_1_Idx);
    X_One_Tree_->Branch("Phi_K_2_Idx", &Phi_K_2_Idx);

    X_One_Tree_->Branch("Pri_mass", &Pri_mass);
    X_One_Tree_->Branch("Pri_massErr", &Pri_massErr);
    X_One_Tree_->Branch("Pri_ctau", &Pri_ctau);
    X_One_Tree_->Branch("Pri_ctauErr", &Pri_ctauErr);
    X_One_Tree_->Branch("Pri_Chi2", &Pri_Chi2);
    X_One_Tree_->Branch("Pri_ndof", &Pri_ndof);
    X_One_Tree_->Branch("Pri_VtxProb", &Pri_VtxProb);
    X_One_Tree_->Branch("Pri_px", &Pri_px);
    X_One_Tree_->Branch("Pri_py", &Pri_py);
    X_One_Tree_->Branch("Pri_pz", &Pri_pz);
    X_One_Tree_->Branch("Pri_phi", &Pri_phi);
    X_One_Tree_->Branch("Pri_eta", &Pri_eta);
    X_One_Tree_->Branch("Pri_pt", &Pri_pt);

    

    X_One_Tree_->Branch("Phi_K_1_px", &Phi_K_1_px);
    X_One_Tree_->Branch("Phi_K_1_py", &Phi_K_1_py);
    X_One_Tree_->Branch("Phi_K_1_pz", &Phi_K_1_pz);
    X_One_Tree_->Branch("Phi_K_1_phi", &Phi_K_1_phi);
    X_One_Tree_->Branch("Phi_K_1_eta", &Phi_K_1_eta);
    X_One_Tree_->Branch("Phi_K_1_pt", &Phi_K_1_pt);

    X_One_Tree_->Branch("Phi_K_2_px", &Phi_K_2_px);
    X_One_Tree_->Branch("Phi_K_2_py", &Phi_K_2_py);
    X_One_Tree_->Branch("Phi_K_2_pz", &Phi_K_2_pz);
    X_One_Tree_->Branch("Phi_K_2_phi", &Phi_K_2_phi);
    X_One_Tree_->Branch("Phi_K_2_eta", &Phi_K_2_eta);
    X_One_Tree_->Branch("Phi_K_2_pt", &Phi_K_2_pt);


	if (doMC)
	{
		X_One_Tree_->Branch("MC_X_px", &MC_X_px);
		X_One_Tree_->Branch("MC_X_py", &MC_X_py);
		X_One_Tree_->Branch("MC_X_pz", &MC_X_pz);
		X_One_Tree_->Branch("MC_X_mass", &MC_X_mass);
		X_One_Tree_->Branch("MC_X_chg", &MC_X_chg);
		X_One_Tree_->Branch("MC_Dau_JpsipdgId", &MC_Dau_JpsipdgId);
		X_One_Tree_->Branch("MC_Dau_Jpsipx", &MC_Dau_Jpsipx);
		X_One_Tree_->Branch("MC_Dau_Jpsipy", &MC_Dau_Jpsipy);
		X_One_Tree_->Branch("MC_Dau_Jpsipz", &MC_Dau_Jpsipz);
		X_One_Tree_->Branch("MC_Dau_Jpsimass", &MC_Dau_Jpsimass);
		X_One_Tree_->Branch("MC_Dau_psi2spdgId", &MC_Dau_psi2spdgId);
		X_One_Tree_->Branch("MC_Dau_psi2spx", &MC_Dau_psi2spx);
		X_One_Tree_->Branch("MC_Dau_psi2spy", &MC_Dau_psi2spy);
		X_One_Tree_->Branch("MC_Dau_psi2spz", &MC_Dau_psi2spz);
		X_One_Tree_->Branch("MC_Dau_psi2smass", &MC_Dau_psi2smass);
		X_One_Tree_->Branch("MC_Granddau_mu1pdgId", &MC_Granddau_mu1pdgId);
		X_One_Tree_->Branch("MC_Granddau_mu1px", &MC_Granddau_mu1px);
		X_One_Tree_->Branch("MC_Granddau_mu1py", &MC_Granddau_mu1py);
		X_One_Tree_->Branch("MC_Granddau_mu1pz", &MC_Granddau_mu1pz);
		X_One_Tree_->Branch("MC_Granddau_mu2pdgId", &MC_Granddau_mu2pdgId);
		X_One_Tree_->Branch("MC_Granddau_mu2px", &MC_Granddau_mu2px);
		X_One_Tree_->Branch("MC_Granddau_mu2py", &MC_Granddau_mu2py);
		X_One_Tree_->Branch("MC_Granddau_mu2pz", &MC_Granddau_mu2pz);
		X_One_Tree_->Branch("MC_Granddau_JpsipdgId", &MC_Granddau_JpsipdgId);
		X_One_Tree_->Branch("MC_Granddau_Jpsipx", &MC_Granddau_Jpsipx);
		X_One_Tree_->Branch("MC_Granddau_Jpsipy", &MC_Granddau_Jpsipy);
		X_One_Tree_->Branch("MC_Granddau_Jpsipz", &MC_Granddau_Jpsipz);
		X_One_Tree_->Branch("MC_Granddau_Jpsimass", &MC_Granddau_Jpsimass);
		X_One_Tree_->Branch("MC_Granddau_pi1pdgId", &MC_Granddau_pi1pdgId);
		X_One_Tree_->Branch("MC_Granddau_pi1px", &MC_Granddau_pi1px);
		X_One_Tree_->Branch("MC_Granddau_pi1py", &MC_Granddau_pi1py);
		X_One_Tree_->Branch("MC_Granddau_pi1pz", &MC_Granddau_pi1pz);
		X_One_Tree_->Branch("MC_Granddau_pi2pdgId", &MC_Granddau_pi2pdgId);
		X_One_Tree_->Branch("MC_Granddau_pi2px", &MC_Granddau_pi2px);
		X_One_Tree_->Branch("MC_Granddau_pi2py", &MC_Granddau_pi2py);
		X_One_Tree_->Branch("MC_Granddau_pi2pz", &MC_Granddau_pi2pz);
		X_One_Tree_->Branch("MC_Grandgranddau_mu3pdgId", &MC_Grandgranddau_mu3pdgId);
		X_One_Tree_->Branch("MC_Grandgranddau_mu3px", &MC_Grandgranddau_mu3px);
		X_One_Tree_->Branch("MC_Grandgranddau_mu3py", &MC_Grandgranddau_mu3py);
		X_One_Tree_->Branch("MC_Grandgranddau_mu3pz", &MC_Grandgranddau_mu3pz);
		X_One_Tree_->Branch("MC_Grandgranddau_mu4pdgId", &MC_Grandgranddau_mu4pdgId);
		X_One_Tree_->Branch("MC_Grandgranddau_mu4px", &MC_Grandgranddau_mu4px);
		X_One_Tree_->Branch("MC_Grandgranddau_mu4py", &MC_Grandgranddau_mu4py);
		X_One_Tree_->Branch("MC_Grandgranddau_mu4pz", &MC_Grandgranddau_mu4pz);
	} // if(doMC)
} // begin Job

// Moved some functions from the header file to the source file [Modified by Eric Wang, 20240705]

// CTau calculation from fitted vertex. [Annotated by Eric Wang, 20240705]
double MultiLepPAT::GetcTau(RefCountedKinematicVertex&   decayVrtx, 
                            RefCountedKinematicParticle& kinePart, 
                            Vertex&                             bs ){	
    TVector3 vtx;
    TVector3 pvtx;
    vtx.SetXYZ((*decayVrtx).position().x(), (*decayVrtx).position().y(), 0);
    pvtx.SetXYZ(bs.position().x(), bs.position().y(), 0);
    TVector3 pperp(kinePart->currentState().globalMomentum().x(),
        kinePart->currentState().globalMomentum().y(), 0);
    TVector3 vdiff = vtx - pvtx;


    //Jinfeng 10.3
    GlobalError DecayErr = (*decayVrtx).error();
    GlobalError PrimaryErr = bs.error();

    AlgebraicSymMatrix33 LxyErrMatrix = DecayErr.matrix() + PrimaryErr.matrix();
    AlgebraicVector3 vdiff_;
    vdiff_[0] = vdiff.x();
    vdiff_[1] = vdiff.y();
    vdiff_[2] = 0;

    double LxyErr2 = ROOT::Math::Similarity(LxyErrMatrix, vdiff_);
    double LxyPVErr = 0;
    // double LxyPVSig = 0;

    double cosAlpha = vdiff.Dot(pperp) / (vdiff.Perp() * pperp.Perp());
    double LxyPV = vdiff.Dot(pperp) / pperp.Mag();
    double Lxy = vdiff.Perp();
    double ctauPV = LxyPV * kinePart->currentState().mass() / pperp.Perp();

    if (Lxy != 0)
        LxyPVErr = TMath::Sqrt(LxyErr2) / Lxy * cosAlpha;

    if (LxyPVErr != 0)
        double LxyPVSig = LxyPV / LxyPVErr;

    //Jinfeng
    // if (MassConstraint) {
    //     temp_Lxy_MC = Lxy;
    //     temp_LxyPV_MC = LxyPV;
    //     temp_CosAlpha_MC = cosAlpha;
    //     temp_LxyPVErr_MC = LxyPVErr;
    //     temp_LxyPVSig_MC = LxyPVSig;
    // }
    // else {
    //     temp_Lxy_noMC = Lxy;
    //     temp_LxyPV_noMC = LxyPV;
    //     temp_CosAlpha_noMC = cosAlpha;
    //     temp_LxyPVErr_noMC = LxyPVErr;
    //     temp_LxyPVSig_noMC = LxyPVSig;
    // }

    return ctauPV;    
}

// CTau error calculation from fitted vertex. [Annotated by Eric Wang, 20240705]
double MultiLepPAT:: GetcTauErr( RefCountedKinematicVertex& decayVrtx, 
                                 RefCountedKinematicParticle& kinePart, 
                                 Vertex& bs                              ){       
    TVector3 pperp(kinePart->currentState().globalMomentum().x(),
	               kinePart->currentState().globalMomentum().y(), 
                   0                                              );
    AlgebraicVector vpperp(3);
    vpperp[0] = pperp.x();
    vpperp[1] = pperp.y();
    vpperp[2] = 0.;

    GlobalError v1e = (*decayVrtx).error();
    GlobalError v2e = bs.error();
    AlgebraicSymMatrix vXYe = asHepMatrix(v1e.matrix()) + asHepMatrix(v2e.matrix());
    double ctauErrPV = sqrt(vXYe.similarity(vpperp)) * kinePart->currentState().mass() / (pperp.Perp2());

    return ctauErrPV;    
}

// deltaR calculation from usual eta and phi [Annotated by Eric Wang, 20240705]
double MultiLepPAT::deltaR(double eta1, double phi1, double eta2, double phi2) {
    double deta = eta1 - eta2;
    double dphi = phi1 - phi2;
    while (dphi >   M_PI) dphi -= 2*M_PI;
    while (dphi <= -M_PI) dphi += 2*M_PI;
    return sqrt(deta*deta + dphi*dphi);
}


/******************************************************************************
 * [Name of function]  
 *      getAllTriggers
 * [Description]   
 *      Get all triggers from the trigger results and categorize them
 * [Parameters]
 *      HLTresult                       trigger results from EDM
 * [Return value]
 *      (void)
 * [Note]
 *      [Eric Wang, 20240705]
 *          
******************************************************************************/
void MultiLepPAT::getAllTriggers(const edm::Handle<edm::TriggerResults>&     HLTresult){
}



/******************************************************************************
 * [Name of function]  
 *      muonMatchTrigType
 * [Description]   
 *      
 * [Parameters]
 *      HLTresult                       trigger results from EDM
 * [Return value]
 *      (void)
 * [Note]
 *      [Eric Wang, 20240705]
 *          
******************************************************************************/
bool MultiLepPAT::muonMatchTrigType(const edm::View<pat::Muon>::const_iterator& muIter,
                                    const vector<string>& trigNames, 
                                          trigType        type                          ){
    return false;
}


// ------------ method called once each job just after ending the event loop  ------------
void MultiLepPAT::endJob()
{
	X_One_Tree_->GetDirectory()->cd();
	X_One_Tree_->Write();
}

// define this as a plug-in
DEFINE_FWK_MODULE(MultiLepPAT);
