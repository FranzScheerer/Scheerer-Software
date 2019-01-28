#include <stdio.h>
#include <math.h>
#include <time.h>

/*#
# Sonnenstandsberechnung nach Artikel Sonnenstand
# aus de.wikipedia.org
#
# geographische Koordinaten (minutengenau) hier eintragen     
#

#Beispiel Mainz: 

#50 Grad, 0 Minuten noerdliche Breite
*/
double geo_b = 50.0 + 0.0/60.0;

/*
#8 Grad + 16 Minute oestliche Laenge
*/
double geo_l = 8.0 + 16.0/60.0;

/*#
# Tage seit fiktivem Datum 0.0.0000
# (Gregorianischer Kalender)
#*/

int tage(int Y,int M, int T){
  int rm;
  int mon[12] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};
  int ym = Y - 1;
  rm = 365*ym + ym/4 - ym/100 + ym/400; 
  rm = rm + mon[M-1];
  if (M > 2 && Y % 4 == 0 && ( (Y % 100 != 0) || (Y % 400 == 0) ))
    rm = rm + 1;
  return rm + T;
}
/*#
# Julianisches Datum 
#*/
double JD(int Y,int M,int T, double gmt){
  return (gmt-6)/24.0 + tage(Y,M,T) - tage(2006,8,6) + 2453953.75;
}
/*
#
# Das "n" aus dem Artikel Sonnenstand 
#
*/
double n(int Y,int M, int T, double gmt){
  return JD(Y,M,T,gmt) - 2451545.0;
}
/*#
# Das "L" aus dem Artikel
#*/
double L(int Y,int M,int T, double gmt){
  return 280.460 + n(Y,M,T,gmt) * 0.9856474;
}
double g(int Y,int M,int T,double gmt){
  return 357.528 + n(Y,M,T,gmt) * 0.9856003; 
}
double lmb(int Y,int M,int T,double gmt){
  double gg = g(Y,M,T,gmt) * (M_PI/180.0);
  return L(Y,M,T,gmt) + 1.915 * sin(gg) + 0.02 * sin(2*gg);
}
double delta(int Y,int M,int T,double gmt){
  double lx = lmb(Y,M,T,gmt) * (M_PI/180.0);
  double eps = (23.439 + 0.0000004 * n(Y,M,T,gmt)) * (M_PI/180.0);
  return asin( sin(lx) * sin(eps) );
}
double alpha(int Y,int M,int T,double gmt){ 
  double lx = lmb(Y,M,T,gmt) * (M_PI/180.0);
  double eps = (23.439 + 0.0000004 * n(Y,M,T,gmt)) * (M_PI/180.0);
  return atan2( sin(lx) * cos(eps), cos(lx) );
}
double sternzeit(int Y,int M,int T, double gmt){
  double T0 = n(Y,M,T,0)/36525.0;  
  return (M_PI/12.0)*(6.697376 + 2400.05134*T0 + 1.002738 * gmt);
}
double a(double b,double lg,int Y,int M,int T, double gmt){
  double x,y,tau,td;
  b = b * M_PI/(180.0);
  tau = lg*M_PI/180.0 + sternzeit(Y,M,T,gmt) - alpha(Y,M,T,0);
  td = tan( delta(Y,M,T,gmt) );
  y =  sin(tau);
  x =  cos(tau)*sin(b) - td * cos(b);
  return (180.0/M_PI) * atan2( y, x );
}
double h(double b,double lg,int Y,int M,int T,double gmt){
  double tmp1,tmp2,d,tau;
  b = b * M_PI/(180.0);
  tau = lg*M_PI/180.0 + sternzeit(Y,M,T,gmt) - alpha(Y,M,T,0);
  d = delta(Y,M,T,gmt);
  tmp1 =  sin(b) * sin(d);
  tmp2 =  cos(tau) * cos(b) * cos(d);
  return (180.0/M_PI) * asin(tmp1 + tmp2);
}

main(){
  int Y,M,T;
  double gmt;
  time_t rawtime;
  struct tm *ptm;

  time ( &rawtime );

  ptm = gmtime ( &rawtime );

  Y  = ptm->tm_year + 1900;
  M  = ptm->tm_mon + 1;
  T  = ptm->tm_mday;
  gmt = ptm->tm_hour + ptm->tm_min/60.0;

  printf("Azimut %5.2f Grad\n", a(geo_b, geo_l, Y,M,T,gmt));
  printf(" Hoehenwinkel %5.2f Grad\n", h(geo_b, geo_l, Y,M,T,gmt));
}
