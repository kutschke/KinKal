#ifndef KinKal_Config_hh
#define KinKal_Config_hh
//
//  This class defines the configuration of the KKTrk Fit.  It provides the external information needed by
//  the fit (BFieldMap maps, material model), the iteration schedule through which the fit should proceed,
//  and the convergence criteria for that.
//
// struct to define a single meta-iteration of the KKTrk fit.  Each meta-iteration configuration is held
// constant until the algebraic iteration implicit in the extended Kalman fit methodology converges.
//
#include "KinKal/Detector/BFieldMap.hh"

#include <vector>
#include <memory>
#include <algorithm>
#include <any>
#include <ostream>
#include <istream>

namespace KinKal {
  struct MetaIterConfig {
    bool updatemat_; // update material effects
    bool updatebfcorr_; // update magnetic field inhomogeneity effects
    bool updatehits_; // update the internal state of the hits (activity, LR ambiguity) 
    double temp_; // 'temperature' to use in the simulated annealing (dimensionless, roughly equivalent to 'sigma')
    double convdchisq_; // maximum change in chisquared/dof for convergence
    double divdchisq_; // minimum change in chisquared/dof for divergence
    double oscdchisq_; // maximum change in chisquared/dof for oscillation
    double tprec_; // time precision for TOCA calculations
    int miter_; // count of meta-iteration
    // payload for hit updating; specific hit classes should find their particular payload inside the vector
    std::vector<std::any> hitupdaters_;
    MetaIterConfig() : updatemat_(false), updatebfcorr_(false), updatehits_(false), temp_(0.0), convdchisq_(0.01), divdchisq_(10.0), oscdchisq_(1.0), tprec_(1e-6), miter_(-1) {}
    MetaIterConfig(std::istream& is) : miter_(-1) {
      is >> updatemat_ >> updatebfcorr_ >> updatehits_ >> temp_ >> convdchisq_ >> divdchisq_ >> oscdchisq_ >> tprec_;
    }
    double varianceScale() const { return (1.0+temp_)*(1.0+temp_); } // variance scale so that temp=0 means no additional variance
  };

  struct Config {
    enum printLevel{none=0,minimal, basic, complete, detailed, extreme};
    enum BFCorr {nocorr=0, fixed, variable, both };
    typedef std::vector<MetaIterConfig> MetaIterConfigCol;
    Config(BFieldMap const& bfield,std::vector<MetaIterConfig>const& schedule) : Config(bfield) { schedule_ = schedule; }
    Config(BFieldMap const& bfield) : bfield_(bfield),  maxniter_(10), dwt_(1.0e6),  tbuff_(1.0), tol_(0.1), minndof_(5), addmat_(true), bfcorr_(fixed), plevel_(none) {} 
    BFieldMap const& bfield() const { return bfield_; }
    MetaIterConfigCol const& schedule() const { return schedule_; }
    BFieldMap const& bfield_;
    // algebraic iteration parameters
    int maxniter_; // maximum number of algebraic iterations for this config
    double dwt_; // dweighting of initial seed covariance
    double tbuff_; // time buffer for final fit (ns)
    double tol_; // tolerance on position change in BFieldMap integration (mm)
    unsigned minndof_; // minimum number of DOFs to continue fit
    bool addmat_; // add material effects in the fit
    BFCorr bfcorr_; // how to make BFieldMap corrections in the fit
    VEC3 origin_; // nominal origin for defining BNom
    printLevel plevel_; // print level
    // schedule of meta-iterations.  These will be executed sequentially until completion or failure
    MetaIterConfigCol schedule_; 
  };
  std::ostream& operator <<(std::ostream& os, Config kkconfig );
  std::ostream& operator <<(std::ostream& os, MetaIterConfig miconfig );
}
#endif
