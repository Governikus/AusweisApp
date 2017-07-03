import QtQuick 2.5

import "../global"

Item {
	height: identifycolumn.height + Utils.dp(30)

	Column {
		id: identifycolumn
		anchors.top: parent.top
		anchors.topMargin: Utils.dp(20)
		anchors.left: parent.left
		anchors.right: parent.right

		spacing: Utils.dp(30)

		Pane {
			width: parent.width

			Column {
				anchors.left: parent.left
				anchors.right: parent.right

				ProviderInfoSection {
					imageSource: "qrc:///images/iOS/ProviderInformation.png"
					title: qsTr("Service provider")
					name: certificateDescriptionModel.subjectName
				}
				ProviderInfoSection {
					imageSource: "qrc:///images/iOS/ProviderPurpose.png"
					title: qsTr("Purpose for reading out requested data")
					name: certificateDescriptionModel.purpose
					showForwardAction: false
				}
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

		Pane {
			width: parent.width

			DataGroup{
				title: qsTr("Required Data")
				chat: chatModel.required
			}
		}

		Pane {
			width: parent.width

			DataGroup{
				title: qsTr("Optional Data")
				chat: chatModel.optional
			}
		}
	}
}

