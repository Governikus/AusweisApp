import QtQuick 2.5

import "../global"

Item {
	height: Math.max(requireddata.height, optionaldata.height, providerinformation.height) + Utils.dp(40)

	Column {
		id: requireddata
		anchors.top: parent.top
		anchors.topMargin: Utils.dp(20)
		anchors.left: parent.left
		anchors.leftMargin: Utils.dp(20)
		width: (parent.width - Utils.dp(60)) / 3

		Pane {
			width: parent.width

			DataGroup{
				title: qsTr("Required Data")
				chat: chatModel.required
			}
		}
	}

	Column {
		id: optionaldata
		anchors.top: parent.top
		anchors.topMargin: Utils.dp(20)
		anchors.horizontalCenter: parent.horizontalCenter
		width: (parent.width - Utils.dp(60)) / 3

		spacing: Utils.dp(30)

		Pane {
			anchors.left: parent.left
			anchors.right: parent.right
			Column {
				anchors.left: parent.left
				anchors.right: parent.right

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

			DataGroup {
				title: qsTr("Optional Data")
				chat: chatModel.optional
			}
		}
	}

	Column {
		id: providerinformation
		anchors.top: parent.top
		anchors.topMargin: Utils.dp(20)
		anchors.right: parent.right
		anchors.rightMargin: Utils.dp(20)
		width: (parent.width - Utils.dp(60)) / 3

		Pane {
			id: pane

			Text {
				height: implicitHeight * 2
				verticalAlignment: Text.AlignVCenter
				text: qsTr("Provider Information")
				color: Constants.blue
				font.pixelSize: Constants.header_font_size
				elide: Text.ElideRight
			}

			Repeater {
				id: listView
				model: certificateDescriptionModel

				LabeledText {
					id: delegate
					label: model.label
					text: model.text
					width: parent.width
				}
			}
		}
	}
}

