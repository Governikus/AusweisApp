/**
 * Copyright (c) 2017-2025 Governikus GmbH & Co. KG, Germany
 */

pragma ComponentBehavior: Bound

import QtQuick

import Governikus.AuthView
import Governikus.Global
import Governikus.TitleBar
import Governikus.Type

SelfAuthStartView {
	id: root

	property bool autoInsertCard: false
	property bool hideTechnologySwitch: false
	property var initialPlugin

	signal back
	signal changeTransportPin
	signal workflowFinished(int pModuleToShow)

	disagreeButtonText: ""

	navigationAction: NavigationAction {
		action: NavigationAction.Action.Back

		onClicked: root.back()
	}

	onStartSelfAuth: root.push(authView)

	FadeInAnimation {
		target: root
	}
	Component {
		id: authView

		AuthView {
			property bool backRequested: false

			autoInsertCard: root.autoInsertCard
			hideTechnologySwitch: root.hideTechnologySwitch
			initialPlugin: root.initialPlugin

			Component.onCompleted: SelfAuthModel.startWorkflow(false)
			onChangeTransportPin: root.changeTransportPin()
			onRequestBack: backRequested = true
			onWorkflowFinished: root.workflowFinished(backRequested ? UiModule.SELF_AUTHENTICATION : UiModule.DEFAULT)
		}
	}
}
