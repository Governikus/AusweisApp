/**
 * Copyright (c) 2015-2025 Governikus GmbH & Co. KG, Germany
 */

pragma ComponentBehavior: Bound

import QtQuick

import Governikus.Type
import Governikus.View

SectionPage {
	id: root

	property bool startedByOnboarding: false

	signal backToSelfAuthView
	signal backToStartPage(bool pWorkflowSuccess)
	signal changeTransportPin

	//: LABEL DESKTOP
	title: qsTr("Identify")

	Component {
		id: authController

		AuthController {
			startedByOnboarding: root.startedByOnboarding
			title: root.title

			onBackToSelfAuthView: {
				root.pop(root);
				root.backToSelfAuthView();
			}
			onBackToStartPage: pWorkflowSuccess => {
				root.pop(root);
				root.backToStartPage(pWorkflowSuccess);
			}
			onChangeTransportPin: {
				root.pop(root);
				root.changeTransportPin();
			}
		}
	}
	Connections {
		function onFireWorkflowStarted() {
			root.push(authController);
		}

		target: AuthModel
	}
}
