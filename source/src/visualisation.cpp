#include "visualisation.h"

Visualisation::Visualisation()
    : m_file { m_filename.c_str(), H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT }
{
    H5::DataType compound{};
    H5::DataSpace dataspace{};
    H5::DataSet dataset { m_file.createDataSet("test", compound, dataspace) };
}

Visualisation::~Visualisation() = default;
