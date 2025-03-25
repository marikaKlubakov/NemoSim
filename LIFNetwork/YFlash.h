#ifndef YFLASH_H
#define YFLASH_H
#include <vector>
#include <random>
#include <cmath>
class YFlash {
private:
   int x_size, y_size;  // Size of Y-Flash array
   double req, gm, VDD;
   // Capacitance values
   double CGBr, CGBi, CGSr, CGSi, CGDr, CGDi, CDBr, CDBi, CMOS;
   std::vector<std::vector<double>> resistances; // Y-Flash weight storage
public:
   YFlash(int x, int y, double req_, double gm_, double VDD_,
          double CGBr_, double CGBi_, double CGSr_, double CGSi_,
          double CGDr_, double CGDi_, double CDBr_, double CDBi_, double CMOS_);
 
   // Compute Y-Flash current output for a given neuron
   double computeCurrent(double Vgs, double Vds, int row, int col);
   
   // Get resistance for a neuron connection
   double getResistance(int row, int col);
};
#endif // YFLASH_H