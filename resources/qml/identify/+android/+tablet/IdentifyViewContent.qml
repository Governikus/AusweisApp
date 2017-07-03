import QtQuick 2.5

import ".."
import "../global"


Item {
	id: root
	height: infoPane.height + 2 * Constants.component_spacing

	Column {
		id: infoPane
		anchors.top: parent.top
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.margins: Constants.component_spacing

		spacing: Constants.pane_spacing

		Pane {

			Row {
				height: providerEntries.height
				width: parent.width
				spacing: Constants.pane_spacing

				Item {
					height: providerEntries.height
					width: (parent.width - Constants.pane_spacing) / 2

					Column {
						id: providerEntries
						anchors.top: parent.top
						anchors.left: parent.left
						anchors.right: parent.right
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


				Item {
					height: parent.height
					width: (parent.width - Constants.pane_spacing) / 2

					Button {
						id: button
						anchors.horizontalCenter: parent.horizontalCenter
						anchors.verticalCenter: parent.verticalCenter

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
				}
			}
		}

		Pane {
			Column {
				height: childrenRect.height
				width: parent.width
				spacing: Utils.dp(30)

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

				Row {
					width: parent.width
					spacing: Constants.pane_spacing

					DataGroup {
						id: requiredData
						width: optionalData.visible ? parent.width * 0.63 : parent.width

						title: qsTr("Required Data")
						columns: optionalData.visible ? 2 : 3
						chat: chatModel.required
					}

					DataGroup {
						id: optionalData
						width: parent.width * 0.37 - Constants.pane_spacing

						title: qsTr("Optional Data")
						chat: chatModel.optional
					}
				}
			}
		}
	}
}
