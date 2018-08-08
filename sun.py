18accbfa02ef9ac94fe4bc6af3b877bc9954cdd657e9a3947751b2127a86a1b5  sun.py

#!/usr/bin/python
#
#       Suncalc algorithm
#
#       Copyright (c) 2013 iaktueller.de
#       Author: Franz Scheerer <scheerer.software@gmail.com>
#
#       This program is free software; you can redistribute it and/or modify
#       it under the terms of the GNU General Public License as published by
#       the Free Software Foundation; either version 2 of the License, or
#       (at your option) any later version.
#       
#       This program is distributed in the hope that it will be useful,
#       but WITHOUT ANY WARRANTY; without even the implied warranty of
#       MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#       GNU General Public License for more details.
#       

import math
import time

# 
# Sonnenstandsberechnung nach Artikel Sonnenstand
# aus de.wikipedia.org
# Siehe auch Formel (31) in 
# http://www.igep.tu-bs.de/lehre/skripten/moderne_physik_dynamik/dynamik.pdf
#
# geographische Koordinaten hier eintragen     

geo_b = 50.0
geo_l = 8.0 + 16.0/60.0

#
# Tage seit fiktivem Datum 0.0.0000
# (Gregorianischer Kalender)
#
def tage(Y,M,T):
  ym = Y - 1
  rm = 365*ym + ym/4 - ym/100 + ym/400 
  mon = [0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334]
  rm = rm + mon[M-1]
  if M > 2 and Y % 4 == 0 and ( (Y % 100 != 0) or (Y % 400 == 0) ):
    rm = rm + 1
  return rm + T
#
# Julianisches Datum 
#
def JD(Y,M,T,gmt):
  return (gmt-6)/24.0 + tage(Y,M,T) - tage(2006,8,6) + 2453953.75

#
# Das "n" aus dem Artikel Sonnenstand 
#
def n(Y,M,T,gmt):
  return JD(Y,M,T,gmt) - 2451545

#
# Das "L" aus dem Artikel
#
def L(Y,M,T,gmt):
  return 280.460 + n(Y,M,T,gmt) * 0.9856474 

def g(Y,M,T,gmt):
  return 357.528 + n(Y,M,T,gmt) * 0.9856003 

def lmb(Y,M,T,gmt):
  gg = g(Y,M,T,gmt) * (math.pi/180.0)
  return L(Y,M,T,gmt) + 1.915 * math.sin(gg) + 0.01997 * math.sin(2*gg)

def delta(Y,M,T,gmt):
  lx = lmb(Y,M,T,gmt) * (math.pi/180.0)
  eps = (23.439 + 0.0000004 * n(Y,M,T,gmt)) * (math.pi/180.0)
  return math.asin( math.sin(lx) * math.sin(eps) )

def alpha(Y,M,T,gmt): 
  lx = lmb(Y,M,T,gmt) * (math.pi/180.0)
  eps = (23.439 + 0.0000004 * n(Y,M,T,gmt)) * (math.pi/180.0)
  return math.atan2( math.sin(lx) * math.cos(eps), math.cos(lx) )

def sternzeit(Y,M,T,gmt):
  T0 = n(Y,M,T,0)/36525.0  
  return (math.pi/12.0)*(6.697376 + 2400.05134*T0 + 1.002738 * gmt)

def a(b,lg,Y,M,T,gmt):
  b = b * math.pi/(180.0)
  tau = lg*math.pi/180.0 + sternzeit(Y,M,T,gmt) - alpha(Y,M,T,0)
  td = math.tan( delta(Y,M,T,gmt) )
  y =  math.sin(tau)
  x =  math.cos(tau)*math.sin(b) - td * math.cos(b)
  return (180.0/math.pi) * math.atan2( y, x )

def h(b,lg,Y,M,T,gmt):
  b = b * math.pi/(180.0)
  tau = lg*math.pi/180.0 + sternzeit(Y,M,T,gmt) - alpha(Y,M,T,0)
  d = delta(Y,M,T,gmt)
  tmp1 =  math.sin(b) * math.sin(d)
  tmp2 =  math.cos(tau) * math.cos(b) * math.cos(d)
  return (180.0/math.pi) * math.asin(tmp1 + tmp2)

#
# Ausbeute fuer Sueddach (Azimut Dach = 0, Neigung = Dachneigung in Grad)
#
def watt_pro_qm_dach(neigung,b,lg,Y,M,T,gmt):
  if h(b,lg,Y,M,T,gmt) <= 0:
    return 0
  else:
    w_sonne = math.pi/180.0 * h(b,lg,Y,M,T,gmt)
    w_dach = (90.0 - neigung) * math.pi/180.0  
    az_rad = (math.pi/180.0) * a(b,lg,Y,M,T,gmt)

    tmp = math.cos(w_sonne) * math.cos(w_dach) * math.cos(az_rad)
    skp = math.sin(w_dach) * math.sin(w_sonne) + tmp
    if skp < 0:
       return 0
    else:
       return skp * 1000.0

#
# Ausbeute in Wattstunden (3600 Joule) pro Kilowattpeak (kWp) und Jahr 
# m Monat Januar:0, Dezember:11
# tm = 0 entspricht erster des Monats
# Berechnung erfolgt in Schritten von 12 Minuten  (60/5)
#
def wh_pro_jahr(neigung,b,lg,Y):
  mon = [31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31]
  if Y % 4 == 0 and (Y % 100 > 0 or Y % 400 == 0):
    mon[1] = 29
  sum = 0
  for m in range(12):
    for tm in range(mon[m]):
      for minx in range(24*5):
        gmt = minx/5.0
        sum = sum + watt_pro_qm_dach(neigung,b,lg,Y, (m+1), (tm+1), gmt)/5.0
  return sum


gt = time.gmtime()
Y  = gt.tm_year
M  = gt.tm_mon
T  = gt.tm_mday
gmt = gt.tm_hour + gt.tm_min/60.0

print "Azimut ", a(geo_b, geo_l, Y,M,T,gmt), ' Grad'
print "Hoehenwinkel ", h(geo_b, geo_l, Y,M,T,gmt), ' Grad'

print "Ertrag pro Jahr ", round(wh_pro_jahr(50,geo_b,geo_l,Y)/1000.0), " kwh pro kwp"