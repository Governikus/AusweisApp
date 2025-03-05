/**
 * Copyright (c) 2021-2025 Governikus GmbH & Co. KG, Germany
 */

import QtQuick

import Governikus.ProgressView
import Governikus.TitleBar
import Governikus.Type

ProgressView {
	id: root

	property int workflowState: PersonalizationController.WorkflowStates.Initial

	signal abortWorkflow

	headline: {
		switch (workflowState) {
		case PersonalizationController.WorkflowStates.CheckStatus:
			//: LABEL ANDROID IOS
			return qsTr("Checking Smart-eID status");
		case PersonalizationController.WorkflowStates.InstallApplet:
			//: LABEL ANDROID IOS
			return qsTr("Smart-eID is being prepared");
		case PersonalizationController.WorkflowStates.TcToken:
			//: LABEL ANDROID IOS
			return qsTr("Acquiring provider certificate");
		case PersonalizationController.WorkflowStates.ProcessingPhysicalEid:
			//: LABEL ANDROID IOS
			return qsTr("Retrieving information from ID card");
		case PersonalizationController.WorkflowStates.Personalization:
			//: LABEL ANDROID IOS
			return qsTr("Smart-eID is created");
		case PersonalizationController.WorkflowStates.Abort:
			//: LABEL ANDROID IOS
			return qsTr("Aborting Smart-eID personalization");
		default:
			//: LABEL ANDROID IOS
			return qsTr("Waiting for start of workflow...");
		}
	}
	progressText: PersonalizationModel.progressMessage
	progressValue: PersonalizationModel.progressValue
	smartEidUsed: workflowState !== PersonalizationController.WorkflowStates.ProcessingPhysicalEid
	text: {
		switch (workflowState) {
		case PersonalizationController.WorkflowStates.CheckStatus:
		case PersonalizationController.WorkflowStates.InstallApplet:
		case PersonalizationController.WorkflowStates.TcToken:
			//: LABEL ANDROID IOS
			return qsTr("Please wait a moment.");
		case PersonalizationController.WorkflowStates.Abort:
			//: LABEL ANDROID IOS
			return qsTr("Please wait a moment, the current process is being finished.");
		case PersonalizationController.WorkflowStates.ProcessingPhysicalEid:
			return PersonalizationModel.isBasicReader ?
			//: LABEL ANDROID IOS
			qsTr("Please do not move the ID card.") :
			//: LABEL ANDROID IOS
			qsTr("Please observe the display of your card reader.");
		default:
			return "";
		}
	}

	//: LABEL ANDROID IOS
	title: qsTr("Smart-eID")

	navigationAction: NavigationAction {
		action: root.workflowState === PersonalizationController.WorkflowStates.Abort ? NavigationAction.Action.None : NavigationAction.Action.Cancel

		onClicked: root.abortWorkflow()
	}
}
