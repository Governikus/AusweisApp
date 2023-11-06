/**
 * Copyright (c) 2015-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import Governikus.View
import Governikus.Type.AuthModel

SectionPage {
	id: root

	property bool autoInsertCard: false
	property bool hideTechnologySwitch: false
	property var initialPlugIn: null

	signal showChangePinView
	signal workflowFinished

	//: LABEL ANDROID IOS
	title: qsTr("Identify")

	Connections {
		function onFireWorkflowStarted() {
			authController.createObject(root);
			setLockedAndHidden(true);
		}

		enabled: visible
		target: AuthModel
	}
	Component {
		id: authController

		AuthController {
			autoInsertCard: root.autoInsertCard
			hideTechnologySwitch: root.hideTechnologySwitch
			initialPlugIn: root.initialPlugIn
			stackView: root.stackView
			title: root.title

			onShowChangePinView: {
				setLockedAndHidden(false);
				root.showChangePinView();
				this.destroy();
			}
			onWorkflowFinished: {
				setLockedAndHidden(false);
				root.workflowFinished();
				this.destroy();
			}
		}
	}
}
