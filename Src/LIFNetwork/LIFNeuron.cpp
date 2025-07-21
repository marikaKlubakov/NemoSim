#include <iostream>
#include <vector>
#include <random>
#include <string>
#include "LIFNeuron.hpp"
LIFNeuron::LIFNeuron(double Cm_, double Cf_, double Vth_, double VDD_, double dt_, double IR_)
    : m_Cm(Cm_), m_Cf(Cf_), m_Vth(Vth_), m_VDD(VDD_), m_dt(dt_)
{
    m_Vm = 0.0;
    m_beta = m_Cm / (m_Cm + m_Cf);
    m_spiked = false;
    m_IR = 8 * IR_;
    m_lastVout = 0.0;
}
void LIFNeuron::update(double Iin)
{
    // Determine f(Vth) based on current Vm
    double f_Vth = (m_Vm > m_Vth) ? 1.0 : 0.0;
    // Calculate output voltage
    double Vout = m_VDD * f_Vth;
    // Calculate V_f
    double Vf = Vout - m_Vm;
    // Approximate dVf/dt
    double dVf_dt = (Vout - m_lastVout) / m_dt;
    m_lastVout = Vout; // Update last output
    // Update Vm'
    double Vm_prime = m_Vm + (m_dt / m_Cm) * (Iin - m_IR * f_Vth + m_Cf * dVf_dt);
    // Save previous Vm to check for threshold crossings
    double Vm_prior = m_Vm;
    // Spike/reset logic according to threshold crossing
    if ((Vm_prime > m_Vth) && (Vm_prior <= m_Vth))
    {
        // Upward crossing
        m_Vm = m_Vth + m_beta * m_VDD;
        m_spiked = true;
    }
    else if ((Vm_prime < m_Vth) && (Vm_prior >= m_Vth))
    {
        // Downward crossing
        m_Vm = m_Vth - m_beta * m_VDD;
        m_spiked = true;
    }
    else
    {
        // No crossing, continue evolving
        m_Vm = Vm_prime;
        m_spiked = false;
    }
    // Store data for logging
    m_vms.emplace_back(m_Vm);
    m_Iin.emplace_back(Iin);
    m_vout.emplace_back(Vout);
}
