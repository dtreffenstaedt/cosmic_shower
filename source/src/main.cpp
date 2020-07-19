#include "core.h"

auto main(int argc, char* argv[]) -> int
{
    Shower::Core core(argc, argv);

    return core.execute();
}
