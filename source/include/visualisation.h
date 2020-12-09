#ifndef VISUALISATION_H
#define VISUALISATION_H

#include "hdf5/serial/H5Cpp.h"

class Visualisation
{
public:
    struct {
        std::uint64_t m_id;
        std::uint64_t m_pdg;
        std::
    };
    Visualisation();
    ~Visualisation();

private:
    std::string m_filename { "test.h5" };
    hid_t m_file_handle;
    herr_t m_file_error;
    H5::H5File m_file;
};

#endif // VISUALISATION_H
