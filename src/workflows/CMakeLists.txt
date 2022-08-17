#####################################################################
# The module workflow is responsible to manage possible workflows
# like Remote-, Auth-, SelfAuth-, Smart- or ChangePin-Workflow.
#####################################################################

add_subdirectory(base)
add_subdirectory(selfauth)

if(TARGET AusweisAppCardSmart)
	add_subdirectory(personalization)
endif()

if(TARGET ${Qt}::WebSockets)
	add_subdirectory(ifd)
endif()
