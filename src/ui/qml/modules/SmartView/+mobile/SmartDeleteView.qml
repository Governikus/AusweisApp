/**
 * Copyright (c) 2021-2026 Governikus GmbH & Co. KG, Germany
 */
import QtQml
import Governikus.Type

SmartDeleteBaseView {
	id: root

	//: MOBILE
	buttonText: qsTr("Delete Smart-eID")
	//: MOBILE
	deleteDescriptionText: qsTr("You are about to delete the Smart-eID data that is currently stored on your device.")
	//: MOBILE
	popupButtonText: qsTr("Delete")
	//: MOBILE
	popupText: qsTr("Are you sure you want to delete the Smart-eID?")
	popupTitle: title
	//: MOBILE
	title: qsTr("Delete the Smart-eID")

	onClose: {
		show(UiModule.DEFAULT);
		popAll();
	}
	onDeleteConfirmed: {
		setLockedAndHidden();
		pushProgressView({
			title: root.title,
			//: MOBILE
			text: qsTr("Deleting Smart-eID")
		});
		SmartModel.deletePersonalization();
	}

	Connections {
		function onFireDeletePersonalizationDone(pSuccess) {
			root.pushResultView({
				success: pSuccess,
				title: root.title,
				text: (pSuccess ?
					//: MOBILE
					qsTr("You have successfully deleted your Smart-eID.") :
					//: MOBILE
					qsTr("The Smart-eID could not be successfully deleted from your device.")),
				//: MOBILE
				buttonText: qsTr("Back to start page")
			});
		}

		target: SmartModel
	}
}
