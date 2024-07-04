/**
 * Copyright (c) 2021-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import Governikus.AuthView
import Governikus.ChangePinView
import Governikus.Global
import Governikus.SelfAuthenticationView
import Governikus.Style
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

		onClicked: show(UiModule.DEFAULT)
	}

	PersonalizationController {
		id: controller

		stackView: root.stackView
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
			onWorkflowFinished: popAll()
		}
	}
	Component {
		id: selfAuthView

		SelfAuthenticationView {
			autoInsertCard: true
			hideTechnologySwitch: true
			initialPlugin: ReaderManagerPluginType.SMART

			onBack: pop()
			onWorkflowFinished: _ => popAll()
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
				setLockedAndHidden(false);
				popAll();
			}
		}
	}
	Component {
		id: checkSmartResultView

		CheckSmartResultView {
			result: SmartModel.state

			onCancelClicked: {
				setLockedAndHidden(false);
				popAll();
			}
			onCheckDevice: {
				show(UiModule.CHECK_ID_CARD);
				popAll();
			}
			onRunSmartSetup: push(setupStartView)
		}
	}
	ConnectivityManager {
		id: connectivityManager

		onNetworkInterfaceActiveChanged: {
			if (networkInterfaceActive) {
				pop();
				SmartModel.updateSupportInfo();
			}
		}
	}
	SmartMainView {
		id: smartMainView

		onChangePin: push(changePinView)
		onDeletePersonalization: push(deletePersonalizationStartview)
		onReceivedFocus: pItem => root.positionViewAtItem(pItem)
		onShowCheckResult: {
			setLockedAndHidden();
			push(checkSmartResultView);
			if (connectivityManager.checkConnectivity()) {
				SmartModel.updateSupportInfo();
			} else {
				push(checkConnectivityView);
			}
		}
		onStartSelfAuth: push(selfAuthView)
		onUpdateSmart: push(updateStartView)
	}
}
