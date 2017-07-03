import QtQuick 2.5

import ".."
import "../global"


Column{
	spacing: Utils.dp(30)

	Column {
		width: parent.width
		ProviderInfoSection {
			imageSource: "qrc:///images/provider/information.svg"
			title: qsTr("Service provider")
			name: certificateDescriptionModel.subjectName
			SeparatorLine {}
		}
		ProviderInfoSection {
			imageSource: "qrc:///images/provider/purpose.svg"
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
				numberModel.continueWorkflow()
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
