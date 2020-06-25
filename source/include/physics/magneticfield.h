#ifndef MAGNETICFIELD_H
#define MAGNETICFIELD_H


#include "global.h"


#include <G4MagneticField.hh>

START_NAMESPACE
{
class MagneticField
{
public:
    MagneticField();
    virtual ~MagneticField();

//    virtual void GetFieldValue(const G4double point[4], double* field) const;
};
}

#endif // MAGNETICFIELD_H
