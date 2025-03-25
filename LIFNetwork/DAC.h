#ifndef DAC_H
#define DAC_H

class DAC {
public:
    DAC(float referenceCurrent);
    float convert(int digitalInput);

private:
    float referenceCurrent;
};

#endif // DAC_H