#####################################################################
# The module workflow is responsible to manage possible workflows
# like Remote-, Auth-, SelfAuth- or ChangePin-Workflow.
#####################################################################

add_subdirectory(base)

if(TARGET ${Qt}::WebSockets)
	add_subdirectory(remote_device)
endif()
