#!/bin/sh

if [ -z "$REVIEWBOARD_USER" ]
then
	echo "REVIEWBOARD_USER is undefined"
	exit 1
fi

if [ -z "$REVIEWBOARD_TOKEN" ]
then
	echo "REVIEWBOARD_TOKEN is undefined"
	exit 2
fi

if [ -z "${REVIEWBOARD_SERVER}" ]
then
	echo "REVIEWBOARD_SERVER is undefined"
	exit 3
fi

if [ -z "${REVIEWBOARD_REVIEW_ID}" ]
then
	echo "REVIEWBOARD_REVIEW_ID is undefined... continue to allow custom build"
	exit 0
fi



cmdline="-r ${REVIEWBOARD_REVIEW_ID} --server ${REVIEWBOARD_SERVER} --username ${REVIEWBOARD_USER} --api-token ${REVIEWBOARD_TOKEN}"

if [ -n "${REVIEWBOARD_STATUS_UPDATE_ID}" ]
then
	echo "Update status..."

	if [ "${BUILD_RESULT}" = "SUCCESS" ]
	then
		rb_state="done-success"
		rb_desc="build succeeded."
	else
		if [ -z "${BUILD_RESULT}" ]
		then
			rb_state="pending"
			rb_desc="build started..."
		else
			rb_state="error"
			rb_desc="build failed."
		fi
	fi

	rbt status-update set \
		-s ${REVIEWBOARD_STATUS_UPDATE_ID} \
		--state ${rb_state} \
		--description "${rb_desc}" \
		--url ${BUILD_URL} \
		--url-text "See build" \
		${cmdline}
fi


if [ -n "${BUILD_RESULT}" ]
then
	echo "Add review comment: ${BUILD_RESULT}"

	if [ "${BUILD_RESULT}" = "SUCCESS" ]
	then
		cmdlineIssue=""
		cmdlineHeader=":+1: Congratulations, Jenkins successfully built the changes."
	else
		cmdlineIssue="--open-issue"

		if [ "${BUILD_RESULT}" = "FAILURE" ]
		then
			cmdlineHeader=":-1: Uh oh, Jenkins tried to build the changes and failed."
		elif [ "${BUILD_RESULT}" = "UNSTABLE" ]
		then
			cmdlineHeader=":hand: Jenkins tried to build the changes and noticed some problems."
		else
			cmdlineHeader=":bulb: Jenkins stopped build: ${BUILD_RESULT}."
		fi
	fi

	# Remove empty env variables to avoid ugly messages
	MSG=$(echo $REVIEWBOARD_MSG | sed -E "s/\\$\{[0-9A-Z_]+_BUILD_[A-Z]+\}//g")
	rbt review add-general-comment \
		-t "$MSG" \
		${cmdline} ${cmdlineIssue} --markdown

	rbt review edit \
	 --header "${cmdlineHeader} See [${JOB_NAME} ${BUILD_DISPLAY_NAME}](${BUILD_URL}) of revision: ${REVIEWBOARD_DIFF_REVISION}" \
	${cmdline} --markdown

	rbt review publish ${cmdline}
fi
