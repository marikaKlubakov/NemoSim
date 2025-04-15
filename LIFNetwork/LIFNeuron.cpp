#include <iostream>
#include <vector>
#include <random>
#include <string>
#include "LIFNeuron.hpp"

   LIFNeuron::LIFNeuron(double Cm_, double Cf_, double Vth_, double VDD_, double dt_)
       : m_Cm(Cm_), m_Cf(Cf_), m_Vth(Vth_), m_VDD(VDD_), m_dt(dt_) 
   {
       m_Vm = 0.0;
       m_beta = m_Cm / (m_Cm + m_Cf);
       m_spiked = false;
       m_IR = 8*100e-12;
   }
   void LIFNeuron::update(double Iin)
   {
       double Vm_prime = m_Vm + (m_dt / m_Cm) * (Iin - m_IR * (m_Vm > m_Vth));
       if (Vm_prime >= m_Vth)
	   {
           m_Vm = m_beta * m_VDD;
           m_spiked = true;
       } 
       else
       {
           m_Vm = Vm_prime;
           m_spiked = false;
       }

       m_vms.emplace_back(m_Vm);
       m_Iin.emplace_back(Iin);
       m_vout.emplace_back(m_spiked);
   }
