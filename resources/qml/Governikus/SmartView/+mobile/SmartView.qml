/**
 * Copyright (c) 2021-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import Governikus.AuthView
import Governikus.ChangePinView
import Governikus.Global
import Governikus.SelfAuthenticationView
import Governikus.Style
import Governikus.TitleBar
import Governikus.View
import Governikus.Type.ChangePinModel
import Governikus.Type.ConnectivityManager
import Governikus.Type.ReaderPlugIn
import Governikus.Type.SmartModel
import Governikus.Type.UiModule

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
			initialPlugIn: ReaderPlugIn.SMART

			Component.onCompleted: ChangePinModel.startWorkflow(false, false)
			onWorkflowFinished: popAll()
		}
	}
	Component {
		id: selfAuthView

		SelfAuthenticationView {
			autoInsertCard: true
			hideTechnologySwitch: true
			initialPlugIn: ReaderPlugIn.SMART

			onBack: pop()
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
			result: SmartModel.smartState

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
