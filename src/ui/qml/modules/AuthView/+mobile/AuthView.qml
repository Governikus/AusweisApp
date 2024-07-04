/**
 * Copyright (c) 2015-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import Governikus.View
import Governikus.Type

SectionPage {
	id: root

	property bool autoInsertCard: false
	property bool hideTechnologySwitch: false
	property var initialPlugin: null

	signal requestBack
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
			initialPlugin: root.initialPlugin
			stackView: root.stackView
			title: root.title

			onRequestBack: root.requestBack()
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
