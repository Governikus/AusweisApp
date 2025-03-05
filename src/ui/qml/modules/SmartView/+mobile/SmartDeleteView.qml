/**
 * Copyright (c) 2021-2025 Governikus GmbH & Co. KG, Germany
 */
import QtQml
import Governikus.Type

SmartDeleteBaseView {
	id: root

	//: LABEL ANDROID IOS
	buttonText: qsTr("Delete Smart-eID")
	//: LABEL ANDROID IOS
	deleteDescriptionText: qsTr("You are about to delete the Smart-eID data that is currently stored on your device.")
	//: LABEL ANDROID IOS
	popupButtonText: qsTr("Delete")
	//: LABEL ANDROID IOS
	popupText: qsTr("Are you sure you want to delete the Smart-eID?")
	popupTitle: title
	//: LABEL ANDROID IOS
	title: qsTr("Delete the Smart-eID")

	onClose: {
		show(UiModule.DEFAULT);
		popAll();
	}
	onDeleteConfirmed: {
		setLockedAndHidden();
		pushProgressView({
			title: root.title,
			//: LABEL ANDROID IOS
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
					//: LABEL ANDROID IOS
					qsTr("You have successfully deleted your Smart-eID.") :
					//: LABEL ANDROID IOS
					qsTr("The Smart-eID could not be successfully deleted from your device.")),
				//: LABEL ANDROID IOS
				buttonText: qsTr("Back to start page")
			});
		}

		target: SmartModel
	}
}
