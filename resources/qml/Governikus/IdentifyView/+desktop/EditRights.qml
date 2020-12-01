/*
 * \copyright Copyright (c) 2016-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtQuick.Controls 2.12

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Provider 1.0
import Governikus.TitleBar 1.0
import Governikus.View 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.AuthModel 1.0
import Governikus.Type.NumberModel 1.0
import Governikus.Type.CertificateDescriptionModel 1.0
import Governikus.Type.ChatModel 1.0


SectionPage {
	id: root

	property bool detailView: false

	function showProviderInformation(show) {
		detailView = show
		if (!detailView) onVisibleChanged()
		ApplicationWindow.menuBar.updateActions()
	}

	Accessible.name: qsTr("Edit rights view")
	Accessible.description: qsTr("This is the edit rights view of the AusweisApp2.")
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
						title: qsTr("You are about to identify yourself towards the following provider")
						name: CertificateDescriptionModel.subjectName
					}

					GButton {
						id: moreButton

						anchors.right: parent.right
						anchors.top: parent.top

						activeFocusOnTab: true
						Accessible.description: qsTr("Show more information about the service provider")

						//: LABEL DESKTOP_QML
						text: qsTr("more...")
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
						title: qsTr("Purpose for reading out requested data")
						name: CertificateDescriptionModel.purpose
					}

					GButton {
						id: confirmButton

						anchors.right: parent.right
						anchors.bottom: parent.bottom

						activeFocusOnTab: true
						Accessible.name: confirmButton.text

						icon.source: "qrc:///images/identify.svg"
						tintIcon: true
						//: LABEL DESKTOP_QML %1 can be "CAN" or "PIN"
						text: qsTr("Proceed to %1 entry").arg(
																NumberModel.isCanAllowedMode ?
																//: LABEL DESKTOP_QML Inserted into "Proceed to %1 entry"
																qsTr("CAN") :
																//: LABEL DESKTOP_QML Inserted into "Proceed to %1 entry"
																qsTr("PIN")
															)
						onClicked: {
							ChatModel.transferAccessRights()
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

			text: NumberModel.isCanAllowedMode
				  //: LABEL DESKTOP_QML
				  ? qsTr("By entering the CAN, access to the following data of the ID card will be allowed to the mentioned provider:")
				  //: LABEL DESKTOP_QML
				  : qsTr("By entering your PIN, access to the following data of your ID card will be allowed to the mentioned provider:")
			textStyle: Style.text.normal_inverse

			FocusFrame {}
		}

		GPane {
			visible: !!transactionText.text
			anchors {
				left: parent.left
				right: parent.right
				margins: Constants.pane_padding
			}

			Column {
				id: transactionInfo

				width: parent.width

				spacing: Constants.pane_spacing

				GText {
					id: transactionHeading

					width: parent.width

					activeFocusOnTab: true
					Accessible.name: transactionHeading.text

					//: LABEL DESKTOP_QML
					text: qsTr("Transactional information")
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

			GText {
				visible: !writeData.visible && !requiredData.visible && !optionalData.visible
				anchors.horizontalCenter: parent.horizontalCenter

				activeFocusOnTab: true

				//: LABEL DESKTOP_QML
				text: qsTr("No data requested")
				textStyle: Style.text.normal
			}
		}

		Row {
			id: requestedDataRow

			readonly property int maxColumns: 3
			readonly property int columnWidth: (width - spacing) / maxColumns

			anchors {
				left: parent.left
				right: parent.right
				margins: Constants.pane_padding
			}
			height: Math.max(writeDataPane.implicitHeight, readDataPane.implicitHeight)

			spacing: Constants.pane_spacing

			GPane {
				id: writeDataPane

				visible: writeData.count > 0
				width: readDataPane.visible ? requestedDataRow.columnWidth : parent.width
				height: parent.height

				DataGroup {
					id: writeData

					width: parent.width

					//: LABEL DESKTOP_QML
					title: qsTr("Write access (update)")
					titleStyle: Style.text.header_warning
					columns: readDataPane.visible ? 1 : requestedDataRow.maxColumns
					chat: ChatModel.write
				}
			}

			GPane {
				id: readDataPane

				visible: requiredData.count > 0 || optionalData.count > 0
				width: writeDataPane.visible ? requestedDataRow.columnWidth * 2 : parent.width
				height: parent.height

				Row {
					width: parent.width

					spacing: Constants.pane_spacing

					DataGroup {
						id: requiredData

						width: optionalData.visible ? parent.width / 2 : parent.width

						//: LABEL DESKTOP_QML
						title: qsTr("Read access")
						columns: Math.max(1, requestedDataRow.maxColumns - (writeData.visible ? writeData.columns : 0) - (optionalData.visible ? 1 : 0) - (count > optionalData.count ? 0 : 1))
						chat: ChatModel.required
					}

					DataGroup {
						id: optionalData

						width: requiredData.visible ? parent.width / 2 : parent.width

						//: LABEL DESKTOP_QML
						title: qsTr("Read access (optional)")
						columns: Math.max(1, requestedDataRow.maxColumns - (writeData.visible ? writeData.columns : 0) - (requiredData.visible ? requiredData.columns : 0))
						chat: ChatModel.optional
					}
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
