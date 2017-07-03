import QtQuick 2.5

import "../global"

Column{
	spacing: Utils.dp(30)

	Column {
		width: parent.width
		ProviderInfoSection {
			imageSource: "qrc:///images/iOS/ProviderInformation.png"
			title: qsTr("Service provider")
			name: certificateDescriptionModel.subjectName
			SeparatorLine {}
		}
		ProviderInfoSection {
			imageSource: "qrc:///images/iOS/ProviderPurpose.png"
			title: qsTr("Purpose for reading out requested data")
			name: certificateDescriptionModel.purpose
			showForwardAction: false
			SeparatorLine { anchors.bottom: parent.bottom }
		}
	}
	Button {
		anchors.horizontalCenter: parent.horizontalCenter
		text: qsTr("Identify now")
		onClicked: {
			if (applicationModel.currentWorkflow === "") {
				selfAuthenticationModel.startWorkflow();
			}
			else if (applicationModel.currentWorkflow === "authentication") {
				chatModel.transferAccessRights()
				authModel.continueWorkflow()
			}
		}
	}

	DataGroup{
		title: qsTr("Required Data")
		chat: chatModel.required
	}
	DataGroup{
		title: qsTr("Optional Data")
		chat: chatModel.optional
	}
}
