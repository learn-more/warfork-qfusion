project (cgame)

file(GLOB CGAME_HEADERS
    "*.h"
	"../gameshared/*.h"

)

file(GLOB CGAME_SOURCES
    "*.cpp"
    "../gameshared/*.c"
   	"../qalgo/rng.cpp"

)

add_library(cgame SHARED ${CGAME_SOURCES} ${CGAME_HEADERS})
set_property(TARGET cgame PROPERTY CXX_STANDARD 11)
qf_set_output_dir(cgame ${QFUSION_GAME_DIR})