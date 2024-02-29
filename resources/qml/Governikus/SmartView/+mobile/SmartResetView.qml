/**
 * Copyright (c) 2021-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQml
import Governikus.AuthView
import Governikus.Type.ConnectivityManager
import Governikus.Type.SmartModel

SmartDeleteBaseView {
	id: root

	//: LABEL ANDROID IOS
	buttonText: qsTr("Reset Smart-eID")
	//: LABEL ANDROID IOS
	deleteDescriptionText: qsTr("You are about to reset your Smart-eID data. This can also be used for troubleshooting as well.")
	//: LABEL ANDROID IOS
	popupButtonText: qsTr("Reset")
	//: LABEL ANDROID IOS
	popupText: qsTr("Are you sure you want to reset the Smart-eID?")
	popupTitle: title
	//: LABEL ANDROID IOS
	title: qsTr("Reset the Smart-eID")

	onClose: {
		setLockedAndHidden(false);
		popAll();
	}
	onDeleteConfirmed: {
		setLockedAndHidden();
		pushProgressView({
				"title": root.title,
				//: LABEL ANDROID IOS
				"text": qsTr("Resetting Smart-eID"),
				"progressBarVisible": true,
				//: LABEL ANDROID IOS
				"progressText": qsTr("Resetting Smart-eID")
			});
		if (connectivityManager.checkConnectivity()) {
			SmartModel.deleteSmart();
		} else {
			push(checkConnectivityView);
		}
	}

	ConnectivityManager {
		id: connectivityManager

		onNetworkInterfaceActiveChanged: {
			if (networkInterfaceActive) {
				pop();
				SmartModel.deleteSmart();
			}
		}
	}
	Connections {
		function onFireDeleteSmartDone() {
			pushResultView({
					"success": SmartModel.errorString === "",
					"title": root.title,
					//: LABEL ANDROID IOS
					"text": SmartModel.errorString === "" ? qsTr("You have successfully reset your Smart-eID.") : SmartModel.errorString
				});
		}

		target: SmartModel
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
				pop(root);
			}
		}
	}
}
