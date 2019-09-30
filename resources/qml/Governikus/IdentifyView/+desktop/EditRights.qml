/*
 * \copyright Copyright (c) 2016-2019 Governikus GmbH & Co. KG, Germany
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

						anchors.left: parent.left
						anchors.bottom: parent.bottom

						source: "qrc:///images/npa.svg"
						sourceSize.height: providerText.height * 4
					}

					GText {
						id: providerText

						anchors.left: providerImage.right
						anchors.leftMargin: Constants.pane_spacing
						anchors.right: parent.right

						activeFocusOnTab: true
						Accessible.role: Accessible.Heading
						Accessible.name: providerText.text

						//: LABEL DESKTOP_QML
						text: qsTr("You are about to identify yourself towards the following service provider:") + SettingsModel.translationTrigger
						textStyle: Style.text.normal_inverse

						FocusFrame {
							border.color: Constants.black
							dynamic: false
						}
					}
				}

				Item {
					height: providerRow.height
					width: parent.width

					Row {
						id: providerRow

						spacing: Constants.component_spacing

						ProviderInfoSection {
							id: purposeInfo

							activeFocusOnTab: true

							image: "qrc:///images/provider/purpose.svg"
							//: LABEL DESKTOP_QML
							title: qsTr("Purpose for reading out requested data") + SettingsModel.translationTrigger
							name: certificateDescriptionModel.purpose
						}

						ProviderInfoSection {
							id: providerInfo

							activeFocusOnTab: true

							image: "qrc:///images/provider/information.svg"
							//: LABEL DESKTOP_QML
							title: qsTr("Service provider") + SettingsModel.translationTrigger
							name: certificateDescriptionModel.subjectName
						}
					}

					GButton {
						id: moreButton

						anchors.right: parent.right
						anchors.bottom: parent.bottom

						activeFocusOnTab: true

						//: LABEL DESKTOP_QML
						text: qsTr("more...") + SettingsModel.translationTrigger
						onClicked: showProviderInformation(true)
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
			Accessible.role: Accessible.Heading
			Accessible.name: dataIntroduction.text

			//: LABEL DESKTOP_QML
			text: qsTr("The following data will be transferred to the service provider when you enter the PIN:") + SettingsModel.translationTrigger
			textStyle: Style.text.normal_inverse

			FocusFrame {
				dynamic: false
			}
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
					Accessible.role: Accessible.Section
					Accessible.name: transactionHeading.text

					//: LABEL DESKTOP_QML
					text: qsTr("Transactional information") + SettingsModel.translationTrigger
					textStyle: Style.text.header_accent

					FocusFrame {
						border.color: Constants.black
						dynamic: false
					}
				}

				GText {
					id: transactionText

					width: parent.width

					activeFocusOnTab: true
					Accessible.role: Accessible.Paragraph
					Accessible.name: transactionText.text

					text: AuthModel.transactionInfo
					textStyle: Style.text.normal_inverse

					FocusFrame {
						border.color: Constants.black
						dynamic: false
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

			GButton {
				id: confirmButton

				anchors.right: parent.right

				activeFocusOnTab: true
				Accessible.name: confirmButton.text

				icon.source: "qrc:///images/npa.svg"
				//: LABEL DESKTOP_QML %1 can be CAN or PIN
				text: qsTr("Proceed to %1 entry").arg(NumberModel.isCanAllowedMode ? "CAN" : "PIN") + SettingsModel.translationTrigger
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
