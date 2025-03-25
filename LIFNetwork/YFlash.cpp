#include"YFlash.h"


YFlash::YFlash(int x, int y, double req_, double gm_, double VDD_,
        double CGBr_, double CGBi_, double CGSr_, double CGSi_,
        double CGDr_, double CGDi_, double CDBr_, double CDBi_, double CMOS_)
    : x_size(x), y_size(y), req(req_), gm(gm_), VDD(VDD_),
        CGBr(CGBr_), CGBi(CGBi_), CGSr(CGSr_), CGSi(CGSi_),
        CGDr(CGDr_), CGDi(CGDi_), CDBr(CDBr_), CDBi(CDBi_), CMOS(CMOS_)
{
    // Initialize random resistances
    resistances.resize(y_size, std::vector<double>(x_size, 0.0));
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dist(1.0e3, 1.0e6);  // Resistance range (1kΩ to 1MΩ)
    for (int i = 0; i < y_size; ++i) 
    {
        for (int j = 0; j < x_size; ++j) 
        {
            resistances[i][j] = dist(gen);
        }
    }
}
// Compute Y-Flash current output for a given neuron
double YFlash::computeCurrent(double Vgs, double Vds, int row, int col)
{
    double charge_storage = (CGBr + CGBi + CGSr + CGSi + CGDr + CGDi + CDBr + CDBi + CMOS) * Vgs;
    double I_yflash = gm * Vgs + (Vds / resistances[row][col]);
    return I_yflash - charge_storage;
}
// Get resistance for a neuron connection
double YFlash::getResistance(int row, int col) 
{
    return resistances[row][col];
}
