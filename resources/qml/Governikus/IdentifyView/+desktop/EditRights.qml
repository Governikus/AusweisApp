/*
 * \copyright Copyright (c) 2016-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Controls 2.3

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Provider 1.0
import Governikus.TitleBar 1.0
import Governikus.View 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.SettingsModel 1.0
import Governikus.Type.AuthModel 1.0
import Governikus.Type.NumberModel 1.0


SectionPage {
	id: root

	property bool detailView: false

	function showProviderInformation(show) {
		detailView = show
		if (!detailView) onVisibleChanged()
		ApplicationWindow.menuBar.updateActions()
	}

	Accessible.name: qsTr("Edit rights view") + SettingsModel.translationTrigger
	Accessible.description: qsTr("This is the edit rights view of the AusweisApp2.") + SettingsModel.translationTrigger
	Keys.onReturnPressed: d.onKeyboardConfirmPressed(event)
	Keys.onEnterPressed: d.onKeyboardConfirmPressed(event)
	Keys.onEscapePressed: {
		if (!detailView) {
			event.accepted = false
			return
		}

		showProviderInformation(false)
	}

	QtObject {
		id: d

		function onKeyboardConfirmPressed(event) {
			if (detailView) {
				showProviderInformation(false)
			} else {
				confirmButton.onClicked()
			}
		}
	}

	Column {
		visible: !root.detailView
		anchors.left: parent.left
		anchors.right: parent.right

		spacing: Constants.pane_spacing
		topPadding: (providerImage.height / 2) + (Constants.pane_padding * 2)

		Rectangle {
			id: providerRect

			height: providerColumn.height
			anchors.left: parent.left
			anchors.right: parent.right

			color: Style.color.background_pane
			border.width: Style.dimens.high_contrast_item_border
			border.color: Style.color.high_contrast_item_border

			Image {
				id: providerImage

				anchors.left: parent.left
				anchors.leftMargin: Constants.pane_padding
				anchors.verticalCenter: parent.top

				source: "qrc:///images/npa.svg"
				sourceSize.height: providerColumn.height
			}

			Column {
				id: providerColumn

				anchors.left: providerImage.right
				anchors.right: parent.right
				anchors.margins: Constants.pane_padding

				topPadding: Constants.pane_padding
				bottomPadding: Constants.pane_padding
				spacing: Constants.pane_spacing

				Item {
					height: moreButton.height
					anchors.left: parent.left
					anchors.right: parent.right

					ProviderInfoSection {
						id: providerInfo1

						anchors.left: parent.left
						anchors.right: moreButton.left
						anchors.rightMargin: Constants.component_spacing
						anchors.top: parent.top

						activeFocusOnTab: true

						image: "qrc:///images/provider/information.svg"
						//: LABEL DESKTOP_QML
						title: qsTr("You are about to identify yourself towards the following provider") + SettingsModel.translationTrigger
						name: certificateDescriptionModel.subjectName
					}

					GButton {
						id: moreButton

						anchors.right: parent.right
						anchors.top: parent.top

						activeFocusOnTab: true
						Accessible.description: qsTr("Show more information about the service provider") + SettingsModel.translationTrigger

						//: LABEL DESKTOP_QML
						text: qsTr("more...") + SettingsModel.translationTrigger
						onClicked: showProviderInformation(true)
					}
				}

				Item {
					height: purposeInfo.height
					anchors.left: parent.left
					anchors.right: parent.right

					ProviderInfoSection {
						id: purposeInfo

						anchors.left: parent.left
						anchors.right: confirmButton.left
						anchors.rightMargin: Constants.component_spacing
						activeFocusOnTab: true

						image: "qrc:///images/provider/purpose.svg"
						//: LABEL DESKTOP_QML
						title: qsTr("Purpose for reading out requested data") + SettingsModel.translationTrigger
						name: certificateDescriptionModel.purpose
					}

					GButton {
						id: confirmButton

						anchors.right: parent.right
						anchors.bottom: parent.bottom

						activeFocusOnTab: true
						Accessible.name: confirmButton.text

						icon.source: "qrc:///images/npa.svg"
						//: LABEL DESKTOP_QML %1 can be "card access number (CAN)" or "PIN"
						text: qsTr("Proceed to %1 entry").arg(
																NumberModel.isCanAllowedMode ?
																//: LABEL DESKTOP_QML Inserted into "Proceed to %1 entry"
																qsTr("card access number (CAN)") :
																//: LABEL DESKTOP_QML Inserted into "Proceed to %1 entry"
																qsTr("PIN")
															) + SettingsModel.translationTrigger
						onClicked: {
							chatModel.transferAccessRights()
							AuthModel.continueWorkflow()
						}
					}
				}
			}
		}

		GText {
			id: dataIntroduction

			anchors.left: parent.left
			anchors.right: parent.right
			anchors.margins: Constants.pane_padding

			activeFocusOnTab: true
			Accessible.name: dataIntroduction.text

			//: LABEL DESKTOP_QML
			text: qsTr("The following data will be transferred to the provider when you enter the PIN:") + SettingsModel.translationTrigger
			textStyle: Style.text.normal_inverse

			FocusFrame {}
		}

		Pane {
			anchors.margins: Constants.pane_padding
			anchors.left: parent.left
			anchors.right: parent.right

			Column {
				id: transactionInfo

				visible: !!transactionText.text
				width: parent.width

				spacing: Constants.pane_spacing

				GText {
					id: transactionHeading

					width: parent.width

					activeFocusOnTab: true
					Accessible.name: transactionHeading.text

					//: LABEL DESKTOP_QML
					text: qsTr("Transactional information") + SettingsModel.translationTrigger
					textStyle: Style.text.header_accent

					FocusFrame {
						borderColor: Style.color.focus_indicator
					}
				}

				GText {
					id: transactionText

					width: parent.width

					activeFocusOnTab: true
					Accessible.name: transactionText.text

					text: AuthModel.transactionInfo
					textStyle: Style.text.normal

					FocusFrame {
						borderColor: Style.color.focus_indicator
					}
				}
			}

			Row {
				spacing: Constants.pane_spacing

				readonly property int columnWidth: (parent.width - 2 * Constants.pane_spacing) / 3

				DataGroup {
					id: requiredData

					width: columns * parent.columnWidth + ((columns - 1) * Constants.pane_spacing)

					//: LABEL DESKTOP_QML
					title: qsTr("Required Data") + SettingsModel.translationTrigger
					columns: !optionalData.visible ? 3
						   : count > optionalData.count ? 2
						   : 1
					chat: chatModel.required
				}

				DataGroup {
					id: optionalData

					width: columns * parent.columnWidth + ((columns - 1) * Constants.pane_spacing)

					//: LABEL DESKTOP_QML
					title: qsTr("Optional Data") + SettingsModel.translationTrigger
					columns: 3 - (requiredData.visible ? requiredData.columns : 0)
					chat: chatModel.optional
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
