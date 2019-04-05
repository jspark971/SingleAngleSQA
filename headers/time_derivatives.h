#ifndef TIMEDERIVATIVES_H
#define TIMEDERIVATIVES_H

//===//
// K //
//===//
array<array<array<array<double,NY>,NS>,NE>,NM> K(const State& s){

  array<array<array<array<double,NY>,NS>,NE>,NM> K;

  // derivative of MSW potential
  array<MATRIX<complex<double>,NF,NF>,NM> dVfMSWdr;
  dVfMSWdr[matter][e][e]=dVedr(s.rho,s.drhodr,s.Ye,s.dYedr);
  dVfMSWdr[matter][mu][mu]=dVmudr(s.rho,s.drhodr,s.Ye,s.dYedr);
  dVfMSWdr[matter][e][mu]=0;
  dVfMSWdr[matter][mu][e]=0;
  dVfMSWdr[antimatter]=-Conjugate(dVfMSWdr[matter]);
  
#pragma omp parallel for collapse(2)
  for(int m=matter; m<=antimatter; m++){
    for(int i=0;i<=NE-1;i++){

      array<double,NF-1> phase;
      MATRIX<complex<double>,NF,NF> Ha,HB;
      phase[0] = M_2PI*(s.Y[m][i][msw][4]-s.Y[m][i][msw][5]);
      Ha[0][1]=0.;
      for(int j=0;j<=NF-2;j++)
	for(int k=j+1;k<=NF-1;k++)
	  for(flavour f=e;f<=mu;f++)
	    Ha[j][k]+= conj(s.UU[m][i][f][j])*dVfMSWdr[m][f][f]*s.UU[m][i][f][k];
    
      Ha[0][1] *= I*cgs::constants::hbarc/s.dkk[m][i][0]*exp(I*phase[0]);
      Ha[1][0] = conj(Ha[0][1]);
    
      // HB = -I/cgs::constants::hbarc*Ha*BB;
      HB[0][0]=-I/cgs::constants::hbarc*( Ha[0][1]*s.BB[m][i][msw][1][0] );
      HB[0][1]=-I/cgs::constants::hbarc*( Ha[0][1]*s.BB[m][i][msw][1][1] );

      array<double,4> dvdr;
      dvdr[0]=real(HB[0][1]);
      dvdr[1]=imag(HB[0][1]);
      dvdr[2]=real(HB[0][0]);
      dvdr[3]=imag(HB[0][0]);

      MATRIX<double,3,4> JI = JInverse(s.Y[m][i][msw]);
      
      array<double,NF> dkkdr = dkdr(s.UU[m][i],dVfMSWdr[m]);
      array<MATRIX<complex<double>,NF,NF>,NF> dCCdr = CofactorMatricesDerivatives(s.Hf[m][i],dVfMSWdr[m],dkkdr);
      array<double,NF> QQ =  Q(s.UU[m][i],s.dkk[m][i],s.CC[m][i],dCCdr);

      for(int j=0;j<=2;j++){
	K[m][i][msw][j]=0.;
	for(int k=j;k<=3;k++)
	  K[m][i][msw][j] += JI[j][k]*dvdr[k];
      }
      K[m][i][msw][3] = 0.;
      K[m][i][msw][4] = (s.kk[m][i][0]+QQ[0])/M_2PI/cgs::constants::hbarc;
      K[m][i][msw][5] = (s.kk[m][i][1]+QQ[1])/M_2PI/cgs::constants::hbarc;

      
      // *********************
      // SI part of solution *
      // *********************
      MATRIX<complex<double>,NF,NF> UWBW =
	s.UU[m][i]
	* s.WW[m][i][msw]
	* s.BB[m][i][msw]
	* s.WW[m][i][si];
      Ha = Adjoint(UWBW)*s.VfSI[m]*UWBW;

      K[m][i][si][4]=real(Ha[0][0])/(M_2PI*cgs::constants::hbarc);
      K[m][i][si][5]=real(Ha[1][1])/(M_2PI*cgs::constants::hbarc);
    
      HB[0][0]=-I/cgs::constants::hbarc*( Ha[0][1]*s.BB[m][i][si][1][0] );
      HB[0][1]=-I/cgs::constants::hbarc*( Ha[0][1]*s.BB[m][i][si][1][1] );
    
      //array<double,4> dvdr;
      dvdr[0]=real(HB[0][1]);
      dvdr[1]=imag(HB[0][1]);
      dvdr[2]=real(HB[0][0]);
      dvdr[3]=imag(HB[0][0]);
    
      //MATRIX<double,3,4>
      JI = JInverse(s.Y[m][i][si]);
    
      for(int j=0;j<=2;j++){
	K[m][i][si][j]=0.;
	for(int k=j;k<=3;k++) K[m][i][si][j]+=JI[j][k]*dvdr[k];
      }
    
      K[m][i][si][3]=0.;

      for(solution x=msw;x<=si;x++)
	for(int j=0;j<=NY-1;j++)
	  assert(K[m][i][x][j] == K[m][i][x][j]);
    }
  }
  
  return K;
}// end of K function


#endif
