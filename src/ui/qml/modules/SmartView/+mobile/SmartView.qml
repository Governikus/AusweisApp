/**
 * Copyright (c) 2021-2025 Governikus GmbH & Co. KG, Germany
 */

pragma ComponentBehavior: Bound

import QtQuick

import Governikus.AuthView
import Governikus.ChangePinView
import Governikus.SelfAuthenticationView
import Governikus.TitleBar
import Governikus.View
import Governikus.Type

FlickableSectionPage {
	id: root

	smartEidUsed: true
	title: smartMainView.visible ?
	//: LABEL ANDROID IOS
	qsTr("Smart-eID") :
	//: LABEL ANDROID IOS
	qsTr("Check Smart-eID")

	navigationAction: NavigationAction {
		action: NavigationAction.Action.Back

		onClicked: root.show(UiModule.DEFAULT)
	}

	PersonalizationController {
		id: controller

		smartEidUsed: root.smartEidUsed
		stackView: root.stackView
		title: root.title
	}
	Component {
		id: setupStartView

		SmartSetupStartView {
		}
	}
	Component {
		id: updateStartView

		SmartUpdateStartView {
		}
	}
	Component {
		id: deletePersonalizationStartview

		SmartDeleteView {
		}
	}
	Component {
		id: changePinView

		ChangePinView {
			autoInsertCard: true
			hidePinTypeSelection: true
			hideTechnologySwitch: true
			initialPlugin: ReaderManagerPluginType.SMART

			Component.onCompleted: ChangePinModel.startWorkflow(false, false)
			onWorkflowFinished: root.popAll()
		}
	}
	Component {
		id: selfAuthView

		SelfAuthenticationView {
			autoInsertCard: true
			hideTechnologySwitch: true
			initialPlugin: ReaderManagerPluginType.SMART

			onBack: root.pop()
			onWorkflowFinished: _ => root.popAll()
		}
	}
	Component {
		id: checkConnectivityView

		CheckConnectivityView {
			smartEidUsed: root.smartEidUsed
			title: root.title

			Component.onCompleted: connectivityManager.watching = true
			Component.onDestruction: connectivityManager.watching = false
			onCancel: {
				root.setLockedAndHidden(false);
				root.popAll();
			}
		}
	}
	Component {
		id: checkSmartResultView

		CheckSmartResultView {
			result: SmartModel.state

			onCancelClicked: {
				root.setLockedAndHidden(false);
				root.popAll();
			}
			onCheckDevice: {
				root.show(UiModule.CHECK_ID_CARD);
				root.popAll();
			}
			onRunSmartSetup: push(setupStartView)
		}
	}
	ConnectivityManager {
		id: connectivityManager

		onNetworkInterfaceActiveChanged: {
			if (networkInterfaceActive) {
				root.pop();
				SmartModel.updateSupportInfo();
			}
		}
	}
	SmartMainView {
		id: smartMainView

		onChangePin: root.push(changePinView)
		onDeletePersonalization: root.push(deletePersonalizationStartview)
		onShowCheckResult: {
			root.setLockedAndHidden();
			root.push(checkSmartResultView);
			if (connectivityManager.checkConnectivity()) {
				SmartModel.updateSupportInfo();
			} else {
				root.push(checkConnectivityView);
			}
		}
		onStartSelfAuth: root.push(selfAuthView)
		onUpdateSmart: root.push(updateStartView)
	}
}
