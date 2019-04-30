#ifndef _NULIB_INTERFACE_H
#define _NULIB_INTERFACE_H

#include "H5Cpp.h"
#include <string>
#include <cstdlib>

// physical constants
const double clight = 2.99792458e10; // cm/s
const double hplanck = 1.0545716e-27; // erg.s
const double MeV_to_ergs = 1.60217646e-6;
const int NEUTRINO_SCHEME = 2;
const long int NS_NULIB = 4;

inline bool hdf5_dataset_exists(const char* filename, const char* datasetname){
  bool exists = true;

  // Temporarily turn off error printing                                                                                   
  H5E_auto2_t func;
  void* client_data;
  H5::Exception::getAutoPrint(func,&client_data);
  H5::Exception::dontPrint();

  // See if dataset exists                                                                                                 
  H5::H5File file(filename, H5F_ACC_RDONLY);
  H5::DataSet dataset;
  try{
    dataset = file.openDataSet(datasetname);
  }
  catch(H5::FileIException& exception){
    exists = false;
  }

  // Turn error printing back on                                                                                           
  H5::Exception::setAutoPrint(func,client_data);
  file.close();

  return exists;
}

// module variables set in fortran NuLib code
extern int     __nulib_MOD_total_eos_variables;
extern double* __nulib_MOD_energies;
extern double __nulib_MOD_m_ref;
extern const bool __nulib_MOD_do_weak_mag_corrections,
  __nulib_MOD_do_ionion_correlation,
  __nulib_MOD_do_heavyscat_formfactor,
  __nulib_MOD_do_electronpolarization_correction,
  __nulib_MOD_do_nc_virial_correction,
  __nulib_MOD_do_strange_coupling,
  __nulib_MOD_do_transport_opacities,
  __nulib_MOD_add_nue_absorption_on_n,
  __nulib_MOD_add_anue_absorption_on_p,
  __nulib_MOD_add_nue_absorption_on_a,
  __nulib_MOD_add_nux_absorption_on_n_and_p,
  __nulib_MOD_add_nue_scattering_n,
  __nulib_MOD_add_nue_scattering_p,
  __nulib_MOD_add_nue_scattering_heavies,
  __nulib_MOD_add_nue_scattering_electrons,
  __nulib_MOD_add_nue_scattering_alphas,
  __nulib_MOD_add_anue_scattering_n,
  __nulib_MOD_add_anue_scattering_p,
  __nulib_MOD_add_anue_scattering_heavies,
  __nulib_MOD_add_anue_scattering_electrons,
  __nulib_MOD_add_anue_scattering_alphas,
  __nulib_MOD_add_numu_scattering_n,
  __nulib_MOD_add_numu_scattering_p,
  __nulib_MOD_add_numu_scattering_heavies,
  __nulib_MOD_add_numu_scattering_electrons,
  __nulib_MOD_add_numu_scattering_alphas,
  __nulib_MOD_add_anumu_scattering_n,
  __nulib_MOD_add_anumu_scattering_p,
  __nulib_MOD_add_anumu_scattering_heavies,
  __nulib_MOD_add_anumu_scattering_electrons,
  __nulib_MOD_add_anumu_scattering_alphas,
  __nulib_MOD_add_nutau_scattering_n,
  __nulib_MOD_add_nutau_scattering_p,
  __nulib_MOD_add_nutau_scattering_heavies,
  __nulib_MOD_add_nutau_scattering_electrons,
  __nulib_MOD_add_nutau_scattering_alphas,
  __nulib_MOD_add_anutau_scattering_n,
  __nulib_MOD_add_anutau_scattering_p,
  __nulib_MOD_add_anutau_scattering_heavies,
  __nulib_MOD_add_anutau_scattering_electrons,
  __nulib_MOD_add_anutau_scattering_alphas,
  __nulib_MOD_add_nue_iscattering_electrons,
  __nulib_MOD_add_anue_iscattering_electrons,
  __nulib_MOD_add_numu_iscattering_electrons,
  __nulib_MOD_add_anumu_iscattering_electrons,
  __nulib_MOD_add_nutau_iscattering_electrons,
  __nulib_MOD_add_anutau_iscattering_electrons,
  __nulib_MOD_add_nue_emission_epannihil,
  __nulib_MOD_add_anue_emission_epannihil,
  __nulib_MOD_add_numu_emission_epannihil,
  __nulib_MOD_add_anumu_emission_epannihil,
  __nulib_MOD_add_nutau_emission_epannihil,
  __nulib_MOD_add_anutau_emission_epannihil,
  __nulib_MOD_add_nue_emission_nnbrems,
  __nulib_MOD_add_anue_emission_nnbrems,
  __nulib_MOD_add_numu_emission_nnbrems,
  __nulib_MOD_add_anumu_emission_nnbrems,
  __nulib_MOD_add_nutau_emission_nnbrems,
  __nulib_MOD_add_anutau_emission_nnbrems,	
  __nulib_MOD_add_nue_emission_weakinteraction_ecap,
  __nulib_MOD_add_anue_emission_weakinteraction_poscap,
  __nulib_MOD_apply_kirchoff_to_pair_creation,
  __nulib_MOD_add_nue_kernel_epannihil,
  __nulib_MOD_add_anue_kernel_epannihil,
  __nulib_MOD_add_numu_kernel_epannihil,
  __nulib_MOD_add_anumu_kernel_epannihil,
  __nulib_MOD_add_nutau_kernel_epannihil,
  __nulib_MOD_add_anutau_kernel_epannihil;
  
extern "C"{
  void set_eos_variables_(double* eos_variables);
  void read_eos_table_(char* filename);
  void total_absorption_opacities_(const int* neutrino_species,
				   const double* neutrino_energy,
				   double* absorption_opacity,
				   const double* eos_variables);
}


class EAS{
 public:

  vector<double> eos_variables;
  double munue_kT, eta;

  array<array<double,NE>,NS_NULIB> absorption_opacity, emissivities, scattering_opacity, delta;
  
  EAS(){
    int number_species = 6; // has to be 6 no matter how many are included in nux
    int number_groups = NE;
    eos_variables.resize(__nulib_MOD_total_eos_variables);
    
    // output all nulib parameters
    cout << endl;
    cout << "REQUESTED INTERACTIONS" << endl;
    cout << __nulib_MOD_do_weak_mag_corrections << " do_weak_mag_corrections" << endl;
    cout << __nulib_MOD_do_ionion_correlation << " do_ionion_correlation" << endl;
    cout << __nulib_MOD_do_heavyscat_formfactor << " do_heavyscat_formfactor" << endl;
    cout << __nulib_MOD_do_electronpolarization_correction << " do_electronpolarization_correction" << endl;
    cout << __nulib_MOD_do_nc_virial_correction << " do_nc_virial_correction" << endl;
    cout << __nulib_MOD_do_strange_coupling << " do_strange_coupling" << endl;
    cout << __nulib_MOD_do_transport_opacities << " do_transport_opacities" << endl;
    cout << __nulib_MOD_add_nue_absorption_on_n << " add_nue_absorption_on_n" << endl;
    cout << __nulib_MOD_add_anue_absorption_on_p << " add_anue_absorption_on_p" << endl;
    cout << __nulib_MOD_add_nue_absorption_on_a << " add_nue_absorption_on_A" << endl;
    cout << __nulib_MOD_add_nux_absorption_on_n_and_p << " add_nux_absorption_on_n_and_p" << endl;
    cout << __nulib_MOD_add_nue_scattering_n << " add_nue_scattering_n" << endl;
    cout << __nulib_MOD_add_nue_scattering_p << " add_nue_scattering_p" << endl;
    cout << __nulib_MOD_add_nue_scattering_heavies << " add_nue_scattering_heavies" << endl;
    cout << __nulib_MOD_add_nue_scattering_electrons << " add_nue_scattering_electrons" << endl;
    cout << __nulib_MOD_add_nue_scattering_alphas << " add_nue_scattering_alphas" << endl;
    cout << __nulib_MOD_add_anue_scattering_n << " add_anue_scattering_n" << endl;
    cout << __nulib_MOD_add_anue_scattering_p << " add_anue_scattering_p" << endl;
    cout << __nulib_MOD_add_anue_scattering_heavies << " add_anue_scattering_heavies" << endl;
    cout << __nulib_MOD_add_anue_scattering_electrons << " add_anue_scattering_electrons" << endl;
    cout << __nulib_MOD_add_anue_scattering_alphas << " add_anue_scattering_alphas" << endl;
    cout << __nulib_MOD_add_numu_scattering_n << " add_numu_scattering_n" << endl;
    cout << __nulib_MOD_add_numu_scattering_p << " add_numu_scattering_p" << endl;
    cout << __nulib_MOD_add_numu_scattering_heavies << " add_numu_scattering_heavies" << endl;
    cout << __nulib_MOD_add_numu_scattering_electrons << " add_numu_scattering_electrons" << endl;
    cout << __nulib_MOD_add_numu_scattering_alphas << " add_numu_scattering_alphas" << endl;
    cout << __nulib_MOD_add_anumu_scattering_n << " add_anumu_scattering_n" << endl;
    cout << __nulib_MOD_add_anumu_scattering_p << " add_anumu_scattering_p" << endl;
    cout << __nulib_MOD_add_anumu_scattering_heavies << " add_anumu_scattering_heavies" << endl;
    cout << __nulib_MOD_add_anumu_scattering_electrons << " add_anumu_scattering_electrons" << endl;
    cout << __nulib_MOD_add_anumu_scattering_alphas << " add_anumu_scattering_alphas" << endl;
    cout << __nulib_MOD_add_nutau_scattering_n << " add_nutau_scattering_n" << endl;
    cout << __nulib_MOD_add_nutau_scattering_p << " add_nutau_scattering_p" << endl;
    cout << __nulib_MOD_add_nutau_scattering_heavies << " add_nutau_scattering_heavies" << endl;
    cout << __nulib_MOD_add_nutau_scattering_electrons << " add_nutau_scattering_electrons" << endl;
    cout << __nulib_MOD_add_nutau_scattering_alphas << " add_nutau_scattering_alphas" << endl;
    cout << __nulib_MOD_add_anutau_scattering_n << " add_anutau_scattering_n" << endl;
    cout << __nulib_MOD_add_anutau_scattering_p << " add_anutau_scattering_p" << endl;
    cout << __nulib_MOD_add_anutau_scattering_heavies << " add_anutau_scattering_heavies" << endl;
    cout << __nulib_MOD_add_anutau_scattering_electrons << " add_anutau_scattering_electrons" << endl;
    cout << __nulib_MOD_add_anutau_scattering_alphas << " add_anutau_scattering_alphas" << endl;
    cout << __nulib_MOD_add_nue_iscattering_electrons << " add_nue_Iscattering_electrons" << endl;
    cout << __nulib_MOD_add_anue_iscattering_electrons << " add_anue_Iscattering_electrons" << endl;
    cout << __nulib_MOD_add_numu_iscattering_electrons << " add_numu_Iscattering_electrons" << endl;
    cout << __nulib_MOD_add_anumu_iscattering_electrons << " add_anumu_Iscattering_electrons" << endl;
    cout << __nulib_MOD_add_nutau_iscattering_electrons << " add_nutau_Iscattering_electrons" << endl;
    cout << __nulib_MOD_add_anutau_iscattering_electrons << " add_anutau_Iscattering_electrons" << endl;
    cout << __nulib_MOD_add_nue_emission_epannihil << " add_nue_emission_epannihil" << endl;
    cout << __nulib_MOD_add_anue_emission_epannihil << " add_anue_emission_epannihil" << endl;
    cout << __nulib_MOD_add_numu_emission_epannihil << " add_numu_emission_epannihil" << endl;
    cout << __nulib_MOD_add_anumu_emission_epannihil << " add_anumu_emission_epannihil" << endl;
    cout << __nulib_MOD_add_nutau_emission_epannihil << " add_nutau_emission_epannihil" << endl;
    cout << __nulib_MOD_add_anutau_emission_epannihil << " add_anutau_emission_epannihil" << endl;
    cout << __nulib_MOD_add_nue_emission_nnbrems << " add_nue_emission_NNBrems" << endl;
    cout << __nulib_MOD_add_anue_emission_nnbrems << " add_anue_emission_NNBrems" << endl;
    cout << __nulib_MOD_add_numu_emission_nnbrems << " add_numu_emission_NNBrems" << endl;
    cout << __nulib_MOD_add_anumu_emission_nnbrems << " add_anumu_emission_NNBrems" << endl;
    cout << __nulib_MOD_add_nutau_emission_nnbrems << " add_nutau_emission_NNBrems" << endl;
    cout << __nulib_MOD_add_anutau_emission_nnbrems << " add_anutau_emission_NNBrems" << endl;	
    cout << __nulib_MOD_add_nue_emission_weakinteraction_ecap << " add_nue_emission_weakinteraction_ecap" << endl;
    cout << __nulib_MOD_add_anue_emission_weakinteraction_poscap << " add_anue_emission_weakinteraction_poscap" << endl;
    cout << __nulib_MOD_apply_kirchoff_to_pair_creation << " apply_kirchoff_to_pair_creation" << endl;
    cout << __nulib_MOD_add_nue_kernel_epannihil << " add_nue_kernel_epannihil" << endl;
    cout << __nulib_MOD_add_anue_kernel_epannihil << " add_anue_kernel_epannihil" << endl;
    cout << __nulib_MOD_add_numu_kernel_epannihil << " add_numu_kernel_epannihil" << endl;
    cout << __nulib_MOD_add_anumu_kernel_epannihil << " add_anumu_kernel_epannihil" << endl;
    cout << __nulib_MOD_add_nutau_kernel_epannihil << " add_nutau_kernel_epannihil" << endl;
    cout << __nulib_MOD_add_anutau_kernel_epannihil << " add_anutau_kernel_epannihil" << endl;
    cout << endl;
  }

  void update(const double rho /* g/ccm */, const double T /*MeV*/, const double Ye){
    // set the EOS variables
    for(int i=0; i<__nulib_MOD_total_eos_variables; i++) eos_variables[i] = 0;
    eos_variables[0] = rho;
    eos_variables[1] = T;
    eos_variables[2] = Ye;
    set_eos_variables_(&eos_variables[0]);
    double mue = eos_variables[10];
    double muhat = eos_variables[13];
    munue_kT = (mue-muhat) / T;
    eta = mue / T;
  }

  /* int index(const int is,const int ig,const int iv) const{ */
  /*   return is + ig*ns + iv*ns*ng; */
  /* } */

  /* int kernel_index(const int is,const int igin, const int igout) const{ */
  /*   return is + igin*ns + igout*ns*ng; */
  /* } */

  /* inline int nu4_kernel_index(const int ik, const int i1, const int i3) const{ */
  /*   return i3 + ng*i1 + ng*ng*ik; */
  /* } */
  /* inline int nu4_bin2(const int ik, const int i1, const int i3) const{ */
  /*   return i1+i3-ik; */
  /* } */
  
  double abs(int s, double E /*erg*/) const{ // 1/cm
    double EMeV = E / (1e6*eV);
    int s_nulib = s+1;

    double absopac;
    total_absorption_opacities_(&s_nulib, &EMeV, &absopac, &eos_variables[0]);

    return absopac;
  }
  double scat(int s, int ig) const{ // 1/cm
    return 1;
  }
  /* double delta(const int is,const int ig) const{ // 1/cm */
  /*   if(do_delta){ */
  /*     int ind = index(is,ig,3); */
  /*     assert(ind < eas.size()); */
  /*     return eas[ind]; */
  /*   } */
  /*   else return 0; */
  /* } */
  double fermidirac(int s, double E_kT) const{
    double mu_kT;
    if     (s==0) mu_kT =  munue_kT;
    else if(s==1) mu_kT = -munue_kT;
    else          mu_kT = 0;
    double result = 1./(1. + exp(E_kT-mu_kT));
    return result;
  }
  /* double Phi0scat(const int is,const int igin, const int igout) const{ // cm^3/s/sr */
  /*   double result = 0; */
  /*   if(igin == igout) */
  /*     result += scat(is,igin) */
  /* 	/(4.*M_PI*nu[igin]*nu[igin]*dnu[igin]/cgs::constants::c4); */
  /*   if(do_iscat) */
  /*     result += escat_kernel0[kernel_index(is,igin,igout)]; */
  /*   return result; */
  /* } */
  /* double Phi1scat(const int is,const int igin, const int igout) const{ // cm^3/s/sr */
  /*   double result = 0; */
  /*   if(igin == igout) */
  /*     result += scat(is,igin)*delta(is,igin)/3. */
  /* 	/(4.*M_PI*nu[igin]*nu[igin]*dnu[igin]/cgs::constants::c4); */
  /*   if(do_iscat) */
  /*     result += escat_kernel1[kernel_index(is,igin,igout)]; */
  /*   else return 0; */
  /* } */
  /* double Phi0pair(const int is,const int igin, const int igout) const{ // cm^3/s/sr */
  /*   double result = 0; */
  /*   if(do_pair) */
  /*     result += pair_kernel0[kernel_index(is,igin,igout)]; */
  /*   return result; */
  /* } */
  /* double Phi1pair(const int is,const int igin, const int igout) const{ // cm^3/s/sr */
  /*   double result = 0; */
  /*   if(do_pair) */
  /*     result += pair_kernel1[kernel_index(is,igin,igout)]; */
  /*   else return 0; */
  /* } */

};
EAS eas;

#endif
