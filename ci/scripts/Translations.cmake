block()
	include(Libraries)
endblock()

step(${T_CFG} --preset ci-translations)
step(${T_BUILD} --target update.translations)
