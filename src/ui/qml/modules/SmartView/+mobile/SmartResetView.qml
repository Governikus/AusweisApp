/**
 * Copyright (c) 2021-2026 Governikus GmbH & Co. KG, Germany
 */

pragma ComponentBehavior: Bound

import QtQml
import Governikus.AuthView
import Governikus.Type

SmartDeleteBaseView {
	id: root

	//: MOBILE
	buttonText: qsTr("Reset Smart-eID")
	//: MOBILE
	deleteDescriptionText: qsTr("You are about to reset your Smart-eID data. This can also be used for troubleshooting as well.")
	//: MOBILE
	popupButtonText: qsTr("Reset")
	//: MOBILE
	popupText: qsTr("Are you sure you want to reset the Smart-eID?")
	popupTitle: title
	//: MOBILE
	title: qsTr("Reset the Smart-eID")

	onClose: {
		setLockedAndHidden(false);
		popAll();
	}
	onDeleteConfirmed: {
		root.setLockedAndHidden();
		root.pushProgressView({
			title: root.title,
			//: MOBILE
			text: qsTr("Resetting Smart-eID"),
			progressBarVisible: true,
			//: MOBILE
			progressText: qsTr("Resetting Smart-eID")
		});
		if (connectivityManager.checkConnectivity()) {
			SmartModel.deleteSmart();
		} else {
			root.push(checkConnectivityView);
		}
	}

	ConnectivityManager {
		id: connectivityManager

		onNetworkInterfaceActiveChanged: {
			if (networkInterfaceActive) {
				root.pop();
				SmartModel.deleteSmart();
			}
		}
	}
	Connections {
		function onFireDeleteSmartDone() {
			root.pushResultView({
				success: SmartModel.errorString === "",
				title: root.title,
				//: MOBILE
				text: SmartModel.errorString === "" ? qsTr("You have successfully reset your Smart-eID.") : SmartModel.errorString
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
				root.setLockedAndHidden(false);
				root.pop(root);
			}
		}
	}
}
