#ifndef MATLABCONNECTOR_H
#define MATLABCONNECTOR_H

//MatlabConnector
// MATLAB
//#include "MatlabEngine.hpp"

// /usr/local/MATLAB/R2018b/extern/bin/glnxa64/libMatlabEngine.so
// /usr/local/MATLAB/R2018b/extern/bin/glnxa64/libMatlabDataArray.so
// pthread

// Include:
// /usr/local/MATLAB/R2018b/extern/include/
// Linker:
// /usr/local/MATLAB/R2018b/extern/bin/glnxa64/

// LD_LIBRARY_PATH
// /usr/local/MATLAB/R2018b/extern/bin/glnxa64:/usr/local/MATLAB/R2018b/sys/os/glnxa64


namespace matlabConnectorFunctions {

    void callFevalgcd();
    void seekToTime(double secondsOffset);

}

#endif // MATLABCONNECTOR_H
