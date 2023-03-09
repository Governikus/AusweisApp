/**
 * Copyright (c) 2021-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import Governikus.Global 1.0
import Governikus.ProgressView 1.0
import Governikus.Style 1.0
import Governikus.TitleBar 1.0
import Governikus.View 1.0
import Governikus.Type.AuthModel 1.0
import Governikus.Type.PersonalizationModel 1.0

ProgressView {
	id: root

	property int workflowState: PersonalizationController.WorkflowStates.Initial

	signal abortWorkflow

	progressText: PersonalizationModel.progressMessage
	progressValue: PersonalizationModel.progressValue
	subText: {
		switch (workflowState) {
		case PersonalizationController.WorkflowStates.CheckStatus:
		case PersonalizationController.WorkflowStates.InstallApplet:
		case PersonalizationController.WorkflowStates.TcToken:
		case PersonalizationController.WorkflowStates.Abort:
			//: LABEL ANDROID IOS
			return qsTr("Please wait a moment.");
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
	text: {
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

	//: LABEL ANDROID IOS
	title: qsTr("Smart-eID")
	titleBarColor: workflowState === PersonalizationController.WorkflowStates.ProcessingPhysicalEid ? Style.color.accent : Style.color.accent_smart

	navigationAction: NavigationAction {
		action: workflowState === PersonalizationController.WorkflowStates.Abort ? NavigationAction.Action.None : NavigationAction.Action.Cancel

		onClicked: root.abortWorkflow()
	}
}
