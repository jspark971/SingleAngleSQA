#include "../nulib_interface.h"
#include "../isospin.h"


//=======//
// Ebins //
//=======//
void set_Ebins(vector<double>& E){
  const double NEP=8;
  cout<<"NE="<<NE << " NEP="<<NEP;
  E.resize(NE);
  for(int i=0;i<NE;i++){
    unsigned ind = i;
    if(NE==1||NE==2||NE==4) ind = i*NEP/NE+NEP/NE/2;

    E[i] = 1.e6*cgs::units::eV * exp(log(2*1000000) + ind*(log(37.48*1000000) - log(2*1000000))/(NE-1))/1000000;

    cout<<E[i]<<endl;
    cout.flush();
  }
  cout.flush();
}
double dE(const unsigned i){
  double dlogE = (log(E[NE-1]) - log(E[0])) / (NE-1.);
  double Elow = exp(log(E[0]) + (i-0.5)*dlogE);
  double Ehi  = exp(log(E[0]) + (i+0.5)*dlogE);
  return Ehi - Elow;
}
double phaseVolDensity(const double density, const unsigned i){
  double phaseSpaceVol = 4.*M_PI * E[i]*E[i]*dE(i) / pow(2.*M_PI*cgs::constants::hbarc,3);
  return density / phaseSpaceVol;
}

//============//
// Initialize //
//============//
void initialize(vector<vector<MATRIX<complex<double>,NF,NF> > >& fmatrixf,
		double r, double rho, double T, double Ye){
  // T should be MeV
  cout << "Setting initial data." << endl;
  cout << "rho = " << rho << " g/ccm" << endl;
  cout << "T = " << T << " MeV" << endl;
  cout << "Ye = " << Ye << endl;
  double Ye_in = max(Ye,__nulibtable_MOD_nulibtable_ye_min);
  nulibtable_range_species_range_energy_(&rho, &T, &Ye_in, &eas.storage.front(),
  					 &__nulibtable_MOD_nulibtable_number_species,
  					 &__nulibtable_MOD_nulibtable_number_groups,
  					 &__nulibtable_MOD_nulibtable_number_easvariables);
  
  for(int i=0; i<NE; i++){
    for(state m=matter; m<=antimatter; m++)
      for(flavour f1=e; f1<=mu; f1++)
	for(flavour f2=e; f2<=mu; f2++) 
	  fmatrixf[m][i][f1][f2] = 0;
    double De = phaseVolDensity(eD[i](r)   , i); //eas.emis(0,i) / eas.abs(0,i); //
    double Da = phaseVolDensity(eBarD[i](r), i); //eas.emis(1,i) / eas.abs(1,i); //
    double Dx = phaseVolDensity(xD[i](r)   , i); //eas.emis(2,i) / eas.abs(2,i); //
    
    fmatrixf[    matter][i][e ][e ] = De; 
    fmatrixf[    matter][i][mu][mu] = Dx;
    fmatrixf[antimatter][i][e ][e ] = Da;
    fmatrixf[antimatter][i][mu][mu] = Dx;
      
    cout << "GROUP " << i << endl;
    cout << "\teas.emis = {" << eas.emis(0,i) << ", " << eas.emis(1,i) << ", " << eas.emis(2,i) << "}" << endl;
    cout << "\teas.abs = {" << eas.abs(0,i) << ", " << eas.abs(1,i) << ", " << eas.abs(2,i) << "}" << endl;
    cout << "\tBB = {" << eas.emis(0,i)/eas.abs(0,i) << ", " << eas.emis(1,i)/eas.abs(1,i) << ", " << eas.emis(2,i)/eas.abs(2,i) << "}" << endl;
    cout << "\teas.scat = {" << eas.scat(0,i) << ", " << eas.scat(1,i) << ", " << eas.scat(2,i) << "}" << endl;

    cout << "\tf = {" << real(fmatrixf[matter][i][e][e]) << ", " << real(fmatrixf[antimatter][i][e][e]) << ", " << real(fmatrixf[matter][i][mu][mu]) << ", " << real(fmatrixf[antimatter][i][mu][mu]) << "}" << endl;
  }
  
  for(state m=matter; m<=antimatter; m++)
    for(int i=0; i<NE; i++)
      for(flavour f1=e; f1<=mu; f1++)
	for(flavour f2=e; f2<=mu; f2++)
	  assert(fmatrixf[m][i][f1][f2] == fmatrixf[m][i][f1][f2]);
}

double get_rho(const double r){
  return exp(lnrho(log(r)));
}
double get_drhodr(const double rrho, const double r){
  return rrho*lnrho.Derivative(log(r))/r;
}
double get_Ye(const double r){
  return Ye(r);
}
double get_dYedr(const double r){
  return Ye.Derivative(r);
}

//===================//
// Vacuum Potentials //
//===================//
double deltaV(const double E){ // erg
  return abs(dm21)*cgs::constants::c4 / (2.*E);
}

void set_kV(vector<vector<double> >& kV){
  assert(NF==2);
  for(int i=0;i<NE;i++){
    kV[i][0] = m1*m1 * cgs::constants::c4 /2./E[i];
    kV[i][1] = kV[i][0] + deltaV(E[i]);
  }
}

//===================//
// Matter Potentials //
//===================//
double Ve(double rho, double Ye){
  return (M_SQRT2*cgs::constants::GF/cgs::constants::Mp)*rho*Ye;
}

double dVedr(double rho, double drhodr, double Ye, double dYedr){
  return (M_SQRT2*cgs::constants::GF/cgs::constants::Mp) * (drhodr*Ye + rho*dYedr );
}

double Vmu(double rho, double Ye){ return 0.;}

double dVmudr(double rho, double drhodr, double Ye, double dYedr){ return 0.;}

//=============================//
// Self-Interaction Potentials //
//=============================//
void getPunosc(const double r, const state m, const unsigned ig,
	       MATRIX<complex<double>,NF,NF>& p_unosc){
  
  // decompose unoscillated potential
  double P0 = (m==matter ? eP[ig](r) : eBarP[ig](r));
  double P1 = xP[ig](r);
  p_unosc[e ][e ] = complex<double>(P0,0);
  p_unosc[mu][e ] = complex<double>(0,0);
  p_unosc[e ][mu] = complex<double>(0,0);
  p_unosc[mu][mu] = complex<double>(P1,0);
}


void my_interact(vector<vector<MATRIX<complex<double>,NF,NF> > >& fmatrixf,
	      double rho, double T, double Ye, double r, double dr){

  // set up rate matrix
  MATRIX<complex<double>,NF,NF> dfdr, dfbardr;

  // don't do anything if too sparse
  if(log10(rho) <= __nulibtable_MOD_nulibtable_logrho_min)
    return;

  // T should be MeV
  double T_tmp = 10.0;
  nulibtable_range_species_range_energy_(&rho, &T_tmp, &Ye, &eas.storage.front(),
  					 &__nulibtable_MOD_nulibtable_number_species,
  					 &__nulibtable_MOD_nulibtable_number_groups,
  					 &__nulibtable_MOD_nulibtable_number_easvariables);

  double tmp = 0;
  for(int i=0; i<NE; i++){
    // reset dfdr
    for(flavour f1=e; f1<=mu; f1++)
      for(flavour f2=e; f2<=mu; f2++){
	dfdr   [f1][f2] = 0;
	dfbardr[f1][f2] = 0;
      }

    // absorption and out-scattering
    double kappa_e    = eas.abs(0,i) + eas.scat(0,i);
    double kappa_ebar = eas.abs(1,i) + eas.scat(1,i);
    double kappa_mu   = eas.abs(2,i) + eas.scat(2,i);
    double kappa_avg    = 0.5*(kappa_e   +kappa_mu);
    double kappa_avgbar = 0.5*(kappa_ebar+kappa_mu);
    dfdr   [e ][e ] -= kappa_e      * fmatrixf[    matter][i][e ][e ];
    dfbardr[e ][e ] -= kappa_ebar   * fmatrixf[antimatter][i][e ][e ];
    dfdr   [mu][mu] -= kappa_mu     * fmatrixf[    matter][i][mu][mu];
    dfbardr[mu][mu] -= kappa_mu     * fmatrixf[antimatter][i][mu][mu];
    dfdr   [e ][mu] -= kappa_avg    * fmatrixf[    matter][i][e ][mu];
    dfbardr[e ][mu] -= kappa_avgbar * fmatrixf[antimatter][i][e ][mu];

    // emission
    dfdr   [e ][e ] += eas.emis(0,i);
    dfbardr[e ][e ] += eas.emis(1,i);
    dfdr   [mu][mu] += eas.emis(2,i);
    dfbardr[mu][mu] += eas.emis(2,i);
    
    // in-scattering (currently assumes charged-current scattering)
    dfdr   [e ][e ] += phaseVolDensity(eD[i](r)   , i) * eas.scat(0,i);
    dfbardr[e ][e ] += phaseVolDensity(eBarD[i](r), i) * eas.scat(1,i);

    // Make sure dfdr is Hermitian
    dfdr   [mu][e ] = conj(dfdr   [e][mu]);
    dfbardr[mu][e ] = conj(dfbardr[e][mu]);

    // update fmatrixf
    for(flavour f1=e; f1<=mu; f1++)
      for(flavour f2=e; f2<=mu; f2++){
	fmatrixf[    matter][i][f1][f2] += dfdr[f1][f2]    * dr;
	fmatrixf[antimatter][i][f1][f2] += dfbardr[f1][f2] * dr;
      }

    // check that everything makes sense
    for(state s=matter; s<=antimatter; s++)
      for(flavour f1=e; f1<=mu; f1++)
	for(flavour f2=e; f2<=mu; f2++){
	  assert(fmatrixf[s][i][f1][f2] == fmatrixf[s][i][f1][f2]);
      }
  }
}
