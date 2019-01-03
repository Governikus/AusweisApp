import QtQuick 2.10
import QtQuick.Controls 2.3

import Governikus.Global 1.0
import Governikus.Provider 1.0
import Governikus.TitleBar 1.0
import Governikus.View 1.0
import Governikus.Type.AuthModel 1.0
import Governikus.Type.ApplicationModel 1.0


SectionPage {
	id: root

	property bool detailView: false

	function showProviderInformation(show) {
		detailView = show
		ApplicationWindow.menuBar.updateActions()
	}

	Column {
		visible: !root.detailView

		anchors.left: parent.left
		anchors.right: parent.right
		spacing: Constants.pane_spacing
		topPadding: providerRect.height

		Rectangle {
			id: providerRect
			height: providerColumn.height
			anchors.left: parent.left
			anchors.right: parent.right
			color: Constants.white

			Column {
				id: providerColumn
				anchors.left: parent.left
				anchors.right: parent.right
				anchors.margins: Constants.pane_padding
				topPadding: Constants.pane_padding
				bottomPadding: Constants.pane_padding
				spacing: Constants.pane_spacing

				Item {
					height: providerText.height
					width: parent.width

					Image {
						id: providerImage
						source: "qrc:///images/npa.svg"
						sourceSize.height: providerText.height * 4
						anchors.left: parent.left
						anchors.bottom: parent.bottom
					}

					Text {
						id: providerText
						anchors.left: providerImage.right
						anchors.leftMargin: Constants.pane_spacing
						anchors.right: parent.right
						color: Constants.black
						font.pixelSize: Constants.normal_font_size
						wrapMode: Text.WordWrap
						text: qsTr("You are about to identify yourself towards the following service provider:") + settingsModel.translationTrigger
					}
				}

				Item {
					height: providerRow.height
					width: parent.width

					Row {
						id: providerRow
						spacing: Constants.component_spacing

						ProviderInfoSection {
							image: "qrc:///images/provider/purpose.svg"
							title: qsTr("Purpose for reading out requested data") + settingsModel.translationTrigger
							name: certificateDescriptionModel.purpose
						}

						ProviderInfoSection {
							image: "qrc:///images/provider/information.svg"
							title: qsTr("Service provider") + settingsModel.translationTrigger
							name: certificateDescriptionModel.subjectName
						}
					}

					GButton {
						anchors.right: parent.right
						anchors.bottom: parent.bottom
						text: qsTr("more...") + settingsModel.translationTrigger
						onClicked: showProviderInformation(true)
					}
				}
			}
		}

		Text {
			color: Constants.black
			font.pixelSize: Constants.normal_font_size
			anchors.left: parent.left
			anchors.right: parent.right
			anchors.margins: Constants.pane_padding
			wrapMode: Text.WordWrap
			text: qsTr("The following data will be transferred to the service provider when you enter the PIN:") + settingsModel.translationTrigger
		}

		Pane {
			anchors.margins: Constants.pane_padding

			Column {
				id: transactionInfo
				width: parent.width
				visible: !!transactionInfoText.text
				spacing: Constants.pane_spacing

				Text {
					width: parent.width
					text: qsTr("Transactional information") + settingsModel.translationTrigger
					color: Constants.blue
					font.pixelSize: Constants.pane_title_font_size
					wrapMode: Text.WordWrap
				}

				Text {
					id: transactionInfoText
					width: parent.width
					text: AuthModel.transactionInfo
					color: Constants.black
					font.pixelSize: Constants.normal_font_size
					wrapMode: Text.WordWrap
				}
			}

			Row {
				spacing: Constants.pane_spacing

				readonly property int columnWidth: (parent.width - 2 * Constants.pane_spacing) / 3

				DataGroup {
					id: requiredData
					width: columns * parent.columnWidth + ((columns - 1) * Constants.pane_spacing)

					title: qsTr("Required Data") + settingsModel.translationTrigger
					columns: !optionalData.visible ? 3
						   : count > optionalData.count ? 2
						   : 1
					chat: chatModel.required
				}

				DataGroup {
					id: optionalData
					width: columns * parent.columnWidth + ((columns - 1) * Constants.pane_spacing)

					title: qsTr("Optional Data") + settingsModel.translationTrigger
					columns: 3 - (requiredData.visible ? requiredData.columns : 0)
					chat: chatModel.optional
				}
			}

			GButton {
				icon.source: "qrc:///images/npa.svg"
				anchors.right: parent.right
				text: qsTr("Identify now") + settingsModel.translationTrigger
				onClicked: {
					chatModel.transferAccessRights()
					AuthModel.continueWorkflow()
				}
			}
		}
	}

	CertificateDescriptionPage {
		id: certificateDescriptionPage
		visible: root.detailView
		onExit: showProviderInformation(false)
	}
}
