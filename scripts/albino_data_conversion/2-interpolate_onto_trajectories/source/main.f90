!GLOBAL VARIBLE USING MODULE yzhu14
!      MODULE FILENUMBER
!        character(len=100) :: fileNumber 
!      END MODULE

      program main

      use nud_module
      use parameter_module
      use units_module
      implicit none

      integer, parameter :: num=400           !index of the file to be read
      integer :: n, nin
      character(len=100) :: fileNumber
      COMMON /YZHU14/ fileNumber
!   ..input file........................................................
     ! character(len=120) :: filename_in = './data/trajectory'//trim(fileNumber)//'.txt'
    
      integer, parameter :: npt=800           !number of points along the trajectory
    
!..output file.......................................................
      !character(len=120) :: filename_out1 = './output/v_potential'//trim(fileNumber)//'.txt'  !neutrino potentials [\propto R^(-4)]
     ! character(len=120) :: filename_out2 = './output/v_density'//trim(fileNumber)//'.txt'    !neutrino densitites [\propto R^(-2)]
    
      real(r_kind), dimension(3,npt) :: x
      real(r_kind), dimension(3) :: qx
      real(r_kind) :: d
      real(r_kind) :: t
      real(r_kind) :: y
      real(r_kind) :: qx_mod
      real(r_kind), dimension(ne_h,nut_h) :: v_nu
      real(r_kind), dimension(ne_h,nut_h) :: d_nu
    
      integer :: i
      character(len=100) :: format_str 
      character(len=100) :: filename_out
      nin=19
      do n=19,nin
        if (n < 10) then
            format_str = "(I1.1)"
        else if (n<100) then
            format_str = "(I2.2)"
        else
            format_str = "(I3.3)"
        endif
        write (fileNumber,format_str) n
        write(6,*) trim(fileNumber),fileNumber
!.....read-in the points of the trajectory..............................
        open(8,file='../1-generate_trajectory/output/trajectory'//trim(fileNumber)//'.txt',status='old')
  
!.....the point coordinates are assumed to be in km ....................
        do i=1,npt
          read(8,*)x(1,i)
          read(8,*)x(2,i)
          read(8,*)x(3,i)
        end do
  
        close(8)
  
!.....convert the coordinates in code units.............................
        x = x*1.e+5/dx
  
        open(9,file='./output/v_potential'//trim(fileNumber)//'.txt',status='unknown')
        open(10,file='./output/v_density'//trim(fileNumber)//'.txt',status='unknown')
  
!.....write the header..................................................
!        write(9,29)'x [km]','y [km]','z [km]','d [g/cm^3]','t [MeV]',     &
!       &           'ye [-]','V_nu [1/cm^3]','...'
  
!        write(10,29)'x [km]','y [km]','z [km]','d [g/cm^3]','t [MeV]',    &
!       &           'ye [-]','d_nu [1/cm^3]','...'
  29    format(8a14)
  
        do i=1,npt
!.....here we compute the neutrino direction. Assuming that
!the....neutrino is moving on a straight line, we simply take the ........
!     difference between two consecutive points on the trajectory.......

          if (i.eq.1) then
            qx(:) = x(:,2)-x(:,1)
          else
            qx(:) = x(:,i)-x(:,i-1)
          end if

!....and we normalize it................................................
          qx_mod = sqrt(sum(qx*qx))
          if (qx_mod.gt.0.) then
            qx = qx/qx_mod
          else
            write(6,*)'Problem with x coordinates'
            write(6,*)'x=(0,0,0)'
            stop
          end if
  
          call nu_potential(num,x(:,i),qx,d,t,y,d_nu,v_nu)
  
!....write out the potentials...........................................
          write(9,19)x(:,i)*dx/1.e+5,d,t,y,                               &
       &                v_nu(1:ne_h,1),v_nu(1:ne_h,2),v_nu(1:ne_h,3)
  
!....write out the densities............................................
          write(10,19)x(:,i)*dx/1.e+5,d,t,y,                              &
       &                d_nu(1:ne_h,1),d_nu(1:ne_h,2),d_nu(1:ne_h,3)
  
        end do
        close(9)
        close(10)
  
  19    format(100es14.4)
      end do
      end program main
