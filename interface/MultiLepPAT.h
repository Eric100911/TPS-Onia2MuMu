/******************************************************************************
 *  [File] 
 *      MultiLepPAT.h 
 *  [Class]      
 *      MultiLepPAT 
 *  [Directory] 
 *      TPS-Onia2MuMu/interface/MultiLepPAT.h
 *  [Description]
 *      Make rootTuple for JPsi+Jpsi+Upsilon reconstruction
 *  [Implementation]
 *     <Notes on implementation>
 *  [Original Author]
 *  
 *  [Note]
 *      20240626 [Eric Wang]
 *          Upsilon is abbreviated as "Ups"
 *      20240705 [Eric Wang]
 *          Adding new member functions for the following purposes:
 *              Judging if the event 
******************************************************************************/



#ifndef _MultiLepPAT_h
#define _MultiLepPAT_h

// system include files
#include <memory>
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/MuonReco/interface/MuonSelectors.h"
#include "DataFormats/PatCandidates/interface/PackedCandidate.h" // xining MINIAODtest

// user include files
#include "../interface/VertexReProducer.h"

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/Math/interface/Error.h"
#include "DataFormats/Math/interface/LorentzVector.h"
#include "DataFormats/Math/interface/Point3D.h"
#include "DataFormats/Math/interface/Vector3D.h"


#include "SimDataFormats/GeneratorProducts/interface/HepMCProduct.h"
#include "MagneticField/Records/interface/IdealMagneticFieldRecord.h"
#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"
#include "TrackingTools/Records/interface/TransientTrackRecord.h"
#include "RecoVertex/KinematicFit/interface/KinematicParticleVertexFitter.h"
#include "RecoVertex/KinematicFit/interface/KinematicParticleFitter.h"
#include "RecoVertex/KinematicFit/interface/MassKinematicConstraint.h"
#include "RecoVertex/KinematicFitPrimitives/interface/KinematicParticle.h"
#include "RecoVertex/KinematicFitPrimitives/interface/RefCountedKinematicParticle.h"
#include "RecoVertex/KinematicFitPrimitives/interface/TransientTrackKinematicParticle.h"
#include "RecoVertex/KinematicFitPrimitives/interface/KinematicParticleFactoryFromTransientTrack.h"
#include "TrackingTools/TransientTrack/interface/TransientTrackFromFTSFactory.h"

#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/Candidate/interface/Candidate.h"
#include "DataFormats/Candidate/interface/CompositeCandidate.h"
#include "DataFormats/Candidate/interface/VertexCompositeCandidate.h"
#include "DataFormats/V0Candidate/interface/V0Candidate.h"
#include "DataFormats/RecoCandidate/interface/RecoChargedCandidate.h"
//#include "RecoVertex/V0Producer/interface/V0Producer.h"
#include "RecoVertex/VertexTools/interface/VertexDistanceXY.h"

#include "Geometry/Records/interface/TrackerDigiGeometryRecord.h"

#include "CondFormats/L1TObjects/interface/L1GtTriggerMenu.h"
#include "CondFormats/DataRecord/interface/L1GtTriggerMenuRcd.h"
#include "DataFormats/L1GlobalTrigger/interface/L1GlobalTriggerReadoutSetupFwd.h"
#include "DataFormats/L1GlobalTrigger/interface/L1GlobalTriggerObjectMapRecord.h"
#include "DataFormats/L1GlobalTrigger/interface/L1GlobalTriggerReadoutSetup.h"
#include "DataFormats/L1GlobalTrigger/interface/L1GlobalTriggerReadoutRecord.h"

#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/MuonReco/interface/MuonFwd.h"

#include "HLTrigger/HLTcore/interface/HLTConfigProvider.h"

#include "DataFormats/Common/interface/ValueMap.h"
#include "DataFormats/TrackReco/interface/DeDxData.h"

#include "DataFormats/Luminosity/interface/LumiSummary.h"
#include "DataFormats/Luminosity/interface/LumiDetails.h"
#include "PhysicsTools/RecoUtils/interface/CheckHitPattern.h"

#include "TFile.h"
#include "TTree.h"
#include "TVector3.h"

#include <vector>
#include <utility>

#include "CLHEP/Matrix/Vector.h"
#include "CLHEP/Matrix/Matrix.h"
#include "CLHEP/Matrix/SymMatrix.h"

#include "DataFormats/MuonReco/interface/MuonSelectors.h"

//
// class decleration
//

using std::vector;
using std::string;
using namespace edm;
using namespace reco;

class MultiLepPAT : public edm::one::EDAnalyzer<edm::one::SharedResources> {
public:
    explicit MultiLepPAT(const ParameterSet&);
    ~MultiLepPAT();

    /***
     * [Class]
     *    MuPairInfo
    */
    struct MuPairInfo {
        std::unique_ptr<pat::Muon> muPtr1, muPtr2;
        RefCountedKinematicParticle parent;
        RefCountedKinematicVertex   vertex;
        double_t vtxChi2, vtxDOF, vtxProb;
    }
  
private:
    // Framework methods
    virtual void beginJob() ;
    virtual void beginRun(Run const & iRun, EventSetup const& iSetup);
    virtual void analyze(const Event&, const EventSetup&);
    virtual void endJob() ;

    // Essential methods (ctau)
    virtual double GetcTau( RefCountedKinematicVertex&   decayVrtx, 
                            RefCountedKinematicParticle& kinePart, 
                            Vertex&                             bs );
    virtual double GetcTauErr( RefCountedKinematicVertex& decayVrtx, 
                               RefCountedKinematicParticle& kinePart, 
                               Vertex& bs                              );
    
    // Essential methods (deltaR)
    double deltaR(double eta1, double phi1, double eta2, double phi2);
    



 
    //add token here
    edm::EDGetTokenT< L1GlobalTriggerReadoutRecord    > gtRecordToken_;
    edm::EDGetTokenT< BeamSpot                        > gtbeamspotToken_;
    edm::EDGetTokenT< VertexCollection                > gtprimaryVtxToken_;
    edm::EDGetTokenT< edm::View<pat::Muon>            > gtpatmuonToken_; // MINIAOD
    edm::EDGetTokenT< edm::TriggerResults             > gttriggerToken_;
    edm::EDGetTokenT< edm::View<pat::PackedCandidate> > trackToken_; // MINIAOD
    edm::EDGetTokenT< reco::GenParticleCollection     > genParticlesToken_;

    // ----------member data ---------------------------

    virtual int GetHitsBefore(const EventSetup& setup, const vector<reco::Track>& trks, RefCountedKinematicTree& fitTr){
        return -1;
    }

    virtual int GetMissesAfter(const EventSetup& setup, const vector<reco::Track>& trks, RefCountedKinematicTree& VrtxTree){
        return -1;
    }

    
    virtual bool tracksToMuonPair(vector<RefCountedKinematicParticle>&        arg_MuonResults,
                                  KinematicParticleFactoryFromTransientTrack& arg_MuFactory,
                                  const MagneticField&                        arg_bField,
                                  const TrackRef& arg_Trk1,   const TrackRef& arg_Trk2       );

    
    // Member data

    // Essentials [Annotation by Eric Wang, 20240626]
    string proccessName_;
    HLTConfigProvider hltConfig_;
    
    InputTag hlTriggerResults_;
    InputTag inputGEN_;
    edm::ESGetToken<MagneticField, IdealMagneticFieldRecord>  magneticFieldToken_;
    edm::ESGetToken<TransientTrackBuilder, TransientTrackRecord>  theTTBuilderToken_;
    string  vtxSample;
    bool    doMC;
    int     MCParticle;
    bool    doJPsiMassCost;
    int     MuPixHits_c;
    int     MuSiHits_c;
    double  MuNormChi_c;
    double  MuD0_c;

    // Limits for secondary particles [Annotation by Eric Wang, 20240626]
    
    double JMaxM_c;
    double JMinM_c;
    int PiSiHits_c;
    double MuPt_c;
    double JPiPiDR_c;
    double XPiPiDR_c;
    bool UseXDr_c;	
    double JPiPiMax_c;
    double JPiPiMin_c;

    // Trigger info [Annotation by Eric Wang, 20240626]
    
    bool resolveAmbiguity_; 
    bool addXlessPrimaryVertex_;

    // Using enumeration to define trigger types [Annotation by Eric Wang, 20240626] 
    const unsigned int trigCount = 50;
    const unsigned int trigTypeCount = 3;

    enum class trigType{JPSI, UPS, PHI};

    // Identifying triggers and filters with their name [Annotation by Eric Wang, 20240626]
    vector<string>      TriggersFor_[trigTypeCount];
    vector<string>      FiltersFor_[trigTypeCount];
    
    int matchTrigRes_Type[trigCount][trigTypeCount];
    int matchTrigRes_All[trigCount]


    virtual void getAllTriggers(   const edm::Handle<edm::TriggerResults>&     HLTresult);
    virtual bool muonMatchTrigType(const edm::View<pat::Muon>::const_iterator& muonIter
                                   const vector<string>& trigNames, 
                                         trigType        type                          );
    
    vector<string>      TriggersForMatching_;
    vector<string>      FiltersForMatching_;
    int  MatchingTriggerResult[50];
    bool Debug_;
    double Chi_Track_;

    // Constructing TTree object [Annotation by Eric Wang, 20240626]
    
    TTree* X_One_Tree_;
    
    unsigned int        runNum, evtNum, lumiNum;
    unsigned int        nGoodPrimVtx;
    
    vector<unsigned int>*   trigRes;
    vector<std::string>*    trigNames;
    vector<unsigned int>*   L1TT;
    vector<std::string>*    MatchTriggerNames;

    // primary vertices [Annotation by Eric Wang, 20240626]
    float               priVtxX,    priVtxY,    priVtxZ, 
                        priVtxXE,   priVtxYE,   priVtxZE, 
                        priVtxChiNorm, priVtxChi, priVtxCL;
    vector<float>       *PriVtxXCorrX, *PriVtxXCorrY, *PriVtxXCorrZ;
    vector<double>      *PriVtxXCorrEX, *PriVtxXCorrEY, *PriVtxXCorrEZ;
    vector<float>	    *PriVtxXCorrC2, *PriVtxXCorrCL;
    
    // all muons: kinematic and other information [Annotation by Eric Wang, 20240626]
    unsigned int        nMu;
    vector<float>       *muPx, *muPy, *muPz, 
                        *muD0, *muD0E, *muDz, 
                        *muChi2, *muGlChi2,  *mufHits;   
    vector<bool>        *muFirstBarrel, *muFirstEndCap;
    vector<float>       *muDzVtx, *muDxyVtx;   
    vector<int>         *muNDF, *muGlNDF, *muPhits, *muShits, *muGlMuHits, *muType, *muQual;
    vector<int>         *muTrack;
    vector<float>       *muCharge;
    vector<float>       *muIsoratio;

    // all muons: selection result [Annotation by Eric Wang, 20240626]
    vector<int>         *muIsGoodLooseMuon,         *muIsGoodLooseMuonNew, 
                        *muIsGoodSoftMuonNewIlse,   *muIsGoodSoftMuonNewIlseMod, 
                        *muIsGoodTightMuon,         *muIsJpsiTrigMatch,         
                        *muIsUpsTrigMatch,          *munMatchedSeg;
    vector<int>         *muIsPatLooseMuon, *muIsPatTightMuon, *muIsPatSoftMuon, *muIsPatMediumMuon;
    
    //for Maksat trigger match [Annotation by Eric Wang, 20240626]
    vector<int> *muUpsVrtxMatch, *muL3TriggerMatch;
    
    //added by zhenhu for MuonID
    vector<float>  *muMVAMuonID, *musegmentCompatibility; 
    //for Stoyan slope pull
    vector<float>  *mupulldXdZ_pos_noArb, *mupulldYdZ_pos_noArb;
    //addition 3,4,5
    vector<float>  *mupulldXdZ_pos_ArbDef, *mupulldYdZ_pos_ArbDef;
    vector<float>  *mupulldXdZ_pos_ArbST, *mupulldYdZ_pos_ArbST;
    vector<float>  *mupulldXdZ_pos_noArb_any, *mupulldYdZ_pos_noArb_any;


    // Index of selected muons in one event [Added by Eric Wang, 20240626]
    // Reference: code from Wang Xining
    vector<float> *Jpsi_mu1_Idx,    *Jpsi_mu2_Idx;
    vector<float> *Ups_mu1_Idx,     *Ups_mu2_Idx;

    // Vertex fitting [Modified by Eric Wang, 20240626]
    vector<float> *VtxProb,      *Chi2,           *ndof,        *VtxPt2;

    // Secondary Jpsi's and Upsilon's kinematics [Modified by Eric Wang, 20240626]
    vector<float> *Jpsi_mass,   *Jpsi_VtxProb,  *Jpsi_Chi2, *Jpsi_ndof,
                  *Jpsi_px,     *Jpsi_py,       *Jpsi_pz,   *Jpsi_massErr;
    vector<float> *Ups_mass,    *Ups_VtxProb,   *Ups_Chi2,  *Ups_ndof,
                  *Ups_px,      *Ups_py,        *Ups_pz,    *Ups_massErr;

    // Secondary Jpsi's and Upsilon's kinematics fitted under mass constraints 
    //      [Modified by Eric Wang, 20240626]
    vector<float> *CS_Jpsi_mass,    *CS_Jpsi_VtxProb,   *CS_Jpsi_Chi2,   *CS_Jpsi_ndof,
                  *CS_Jpsi_px,      *CS_Jpsi_py,        *CS_Jpsi_pz,     *CS_Jpsi_massErr;
    vector<float> *CS_Ups_mass,     *CS_Ups_VtxProb,    *CS_Ups_Chi2,    *CS_Ups_ndof,
                  *CS_Ups_px,       *CS_Ups_py,         *CS_Ups_pz,      *CS_Ups_massErr;

    // MC results [Modified by Eric Wang, 20240626]
    // Kinematics
    vector<float> 
        *MC_G1_Jpsi1_px, *MC_G1_Jpsi1_py, *MC_G1_Jpsi1_pz, *MC_G1_Jpsi1_mass,
        *MC_G1_Jpsi2_px, *MC_G1_Jpsi2_py, *MC_G1_Jpsi2_pz, *MC_G1_Jpsi2_mass,
        *MC_G1_Ups1_px,  *MC_G1_Ups1_py,  *MC_G1_Ups1_pz,  *MC_G1_Ups1_mass;

    // For Muon pairs, always keep the order of mu1 = mu+ and mu2 = mu-; 
    vector<float> 
        *MC_G2_Mu1_px, *MC_G2_Mu1_py, *MC_G2_Mu1_pz, *MC_G2_Mu1_mass,
        *MC_G2_Mu2_px, *MC_G2_Mu2_py, *MC_G2_Mu2_pz, *MC_G2_Mu2_mass ; 

    // Particle ID

    vector<float> 
        *MC_G1_Jpsi_PDG_ID,
        *MC_G1_Ups_PDG_ID;
    vector<float> 
        *MC_G2_Mu1_PDG_ID, 
        *MC_G2_Mu2_PDG_ID ; 
    vector<float> 
        *Match_mu1px, *Match_mu1py, *Match_mu1pz,
        *Match_mu2px, *Match_mu2py, *Match_mu2pz ; 

  
};

#endif
