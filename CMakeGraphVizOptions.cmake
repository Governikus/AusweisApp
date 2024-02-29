########################################
# Generate build dependency
#
# cmake --build . --target architecture
########################################

set(GRAPHVIZ_IGNORE_TARGETS AusweisAppGlobal;AusweisAppExternal;cvc;fuzz;OpenSsl;tlscheck;Test;Script;testbedtool;tsParserFU)
set(GRAPHVIZ_EXTERNAL_LIBS OFF)
set(GRAPHVIZ_EXECUTABLES ON)
set(GRAPHVIZ_GENERATE_PER_TARGET OFF)
set(GRAPHVIZ_GENERATE_DEPENDERS OFF)
