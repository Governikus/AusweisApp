import QtQuick 2.5

import ".."
import "../global"


Item {
	height: identifycolumn.height + Utils.dp(30)

	Column {
		id: identifycolumn
		anchors.top: parent.top
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.margins: Constants.component_spacing

		spacing: Constants.component_spacing

		Pane {

			Item {
				width: parent.width
				height: providerEntries.height

				Column {
					id: providerEntries
					anchors.top: parent.top
					anchors.left: parent.left
					anchors.right: forwardAction.left
					spacing: Constants.pane_spacing

					ProviderInfoSection {
						imageSource: "qrc:///images/provider/information.svg"
						title: qsTr("Service provider")
						name: certificateDescriptionModel.subjectName
					}
					ProviderInfoSection {
						imageSource: "qrc:///images/provider/purpose.svg"
						title: qsTr("Purpose for reading out requested data")
						name: certificateDescriptionModel.purpose
					}
				}

				Text {
					id: forwardAction
					anchors.right: parent.right
					anchors.verticalCenter: providerEntries.verticalCenter

					text: ">"
					font.pixelSize: Utils.sp(22)
					color: Constants.grey
				}

				MouseArea {
					anchors.fill: parent
					onClicked: push(certificateDescriptionPage)
				}

				CertificateDescriptionPage {
					id: certificateDescriptionPage
					name: certificateDescriptionModel.subjectName
					visible: false
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
					numberModel.continueWorkflow()
				}
			}
		}

		Pane {
			width: parent.width

			Column {
				id: transactionInfo

				width: parent.width
				visible: !!transactionInfoText.text

				Text {
					height: implicitHeight * 1.5
					verticalAlignment: Text.AlignTop
					text: qsTr("Transactional information")
					color: Constants.blue
					font.pixelSize: Constants.header_font_size
					elide: Text.ElideRight
				}

				Text {
					id: transactionInfoText

					width: parent.width
					font.pixelSize: Constants.normal_font_size
					text: authModel.transactionInfo
					wrapMode: Text.WordWrap
				}
			}

			DataGroup {
				title: qsTr("Required Data")
				chat: chatModel.required
			}

			DataGroup {
				title: qsTr("Optional Data")
				chat: chatModel.optional
			}
		}
	}
}
