/**
 * Copyright (c) 2015-2025 Governikus GmbH & Co. KG, Germany
 */

pragma ComponentBehavior: Bound

import QtQuick
import Governikus.View
import Governikus.Type

SectionPage {
	id: root

	property bool autoInsertCard: false
	property bool hideTechnologySwitch: false
	property var initialPlugin: null
	property bool startedByOnboarding: false

	signal changeTransportPin
	signal requestBack
	signal workflowFinished(bool pSuccess)

	//: LABEL ANDROID IOS
	title: qsTr("Identify")

	QtObject {
		id: d

		property bool oldLockedAndHiddenStatus
	}
	Connections {
		function onFireWorkflowStarted() {
			authController.createObject(root);
			d.oldLockedAndHiddenStatus = root.getLockedAndHidden();
			root.setLockedAndHidden(true);
		}

		enabled: root.visible
		target: AuthModel
	}
	Component {
		id: authController

		AuthController {
			autoInsertCard: root.autoInsertCard
			hideTechnologySwitch: root.hideTechnologySwitch
			initialPlugin: root.initialPlugin
			stackView: root.stackView
			startedByOnboarding: root.startedByOnboarding
			title: root.title

			onChangeTransportPin: {
				root.pop(root);
				root.changeTransportPin();
				this.destroy();
			}
			onRequestBack: root.requestBack()
			onWorkflowFinished: pSuccess => {
				root.pop(root);
				setLockedAndHidden(d.oldLockedAndHiddenStatus);
				root.workflowFinished(pSuccess);
				this.destroy();
			}
		}
	}
}
