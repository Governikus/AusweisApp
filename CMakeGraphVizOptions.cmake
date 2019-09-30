########################################
# Generate build dependency
#
# cmake --build . --target architecture
########################################

SET(GRAPHVIZ_IGNORE_TARGETS AusweisAppGlobal;AusweisAppExternal;cvc;fuzz;OpenSsl;tlscheck;Test;Script)
SET(GRAPHVIZ_EXTERNAL_LIBS OFF)
SET(GRAPHVIZ_EXECUTABLES ON)
SET(GRAPHVIZ_GENERATE_PER_TARGET OFF)
SET(GRAPHVIZ_GENERATE_DEPENDERS OFF)
SET(GRAPHVIZ_GRAPH_NAME AusweisApp2)
