/*
 * \copyright Copyright (c) 2016-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

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
		updateTitleBarActions()
	}

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

	ColumnLayout {
		visible: !root.detailView
		anchors.fill: parent
		anchors.topMargin: (providerImage.height / 2) + (Constants.pane_padding * 2)

		spacing: Constants.pane_spacing

		Rectangle {
			id: providerRect

			Layout.fillWidth: true
			Layout.preferredHeight: providerColumn.height

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

				anchors {
					left: providerImage.right
					right: parent.right
					margins: Constants.pane_padding
				}

				topPadding: Constants.pane_padding
				bottomPadding: Constants.pane_padding
				spacing: Constants.pane_spacing

				ProviderInfoSection {
					id: providerInfo

					anchors {
						right: parent.right
						rightMargin: Constants.component_spacing
						left: parent.left
					}

					activeFocusOnTab: true

					image: "qrc:///images/provider/information.svg"
					//: LABEL DESKTOP
					title: qsTr("You are about to identify yourself towards the following provider")
					name: CertificateDescriptionModel.subjectName
				}

				Item {
					height: Math.max(detailsButton.height, confirmButton.height)

					anchors {
						left: parent.left
						right: parent.right
					}

					GButton {
						id: detailsButton

						anchors {
							top: parent.top
							left: parent.left
						}

						activeFocusOnTab: true
						Accessible.description: qsTr("Show more information about the service provider")

						//: LABEL DESKTOP
						text: qsTr("Details about the provider")
						onClicked: showProviderInformation(true)
						icon.source: "qrc:///images/info.svg"
					}

					GButton {
						id: confirmButton

						anchors {
							right: parent.right
							bottom: parent.bottom
						}

						activeFocusOnTab: true
						Accessible.name: confirmButton.text

						icon.source: "qrc:///images/identify.svg"
						tintIcon: true
						//: LABEL DESKTOP %1 can be "CAN" or "PIN"
						text: qsTr("Proceed to %1 entry").arg(
																NumberModel.isCanAllowedMode ?
																//: LABEL DESKTOP Inserted into "Proceed to %1 entry"
																qsTr("CAN") :
																//: LABEL DESKTOP Inserted into "Proceed to %1 entry"
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

			Layout.fillWidth: true
			Layout.leftMargin: Constants.pane_padding
			Layout.rightMargin: Constants.pane_padding

			activeFocusOnTab: true
			Accessible.name: dataIntroduction.text

			text: NumberModel.isCanAllowedMode
				  //: LABEL DESKTOP
				  ? qsTr("By entering the CAN, access to the following data of the ID card will be allowed to the mentioned provider:")
				  //: LABEL DESKTOP
				  : qsTr("By entering your PIN, access to the following data of your ID card will be allowed to the mentioned provider:")
			textStyle: Style.text.normal

			FocusFrame {}
		}

		ScrollablePane {
			visible: !!AuthModel.transactionInfo || !requestedDataRow.visible
			Layout.fillWidth: true
			Layout.fillHeight: true
			Layout.leftMargin: Constants.pane_padding
			Layout.rightMargin: Constants.pane_padding
			Layout.maximumHeight: implicitHeight

			activeFocusOnTab: true

			//: LABEL DESKTOP
			title: qsTr("Transactional information")

			GText {
				id: transactionText

				visible: !!text
				width: parent.width

				activeFocusOnTab: true
				Accessible.name: transactionText.text

				text: AuthModel.transactionInfo
				textStyle: Style.text.normal

				FocusFrame {}
			}

			GText {
				id: noAccessRightsText

				visible: !writeDataPane.visible && !readDataPane.visible
				width: parent.width

				activeFocusOnTab: true
				Accessible.name: noAccessRightsText.text

				//: LABEL DESKTOP
				text: qsTr("The provider mentioned above does not require any data stored on your ID card, only confirmation of you possessing a valid ID card.")
				textStyle: Style.text.normal

				FocusFrame {}
			}
		}

		Row {
			id: requestedDataRow

			readonly property int maxColumns: 3
			readonly property int columnWidth: (width - spacing) / maxColumns

			visible: writeData.count > 0 || requiredData.count > 0 || optionalData.count > 0
			Layout.fillWidth: true
			Layout.leftMargin: Constants.pane_padding
			Layout.rightMargin: Constants.pane_padding
			Layout.preferredHeight: Math.max(writeDataPane.implicitHeight, readDataPane.implicitHeight)

			spacing: Constants.pane_spacing

			GPane {
				id: writeDataPane

				visible: writeData.count > 0
				width: readDataPane.visible ? requestedDataRow.columnWidth : parent.width
				height: parent.height

				DataGroup {
					id: writeData

					width: parent.width

					//: LABEL DESKTOP
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

						//: LABEL DESKTOP
						title: qsTr("Read access")
						columns: Math.max(1, requestedDataRow.maxColumns - (writeData.visible ? writeData.columns : 0) - (optionalData.visible ? 1 : 0) - (count > optionalData.count ? 0 : 1))
						chat: ChatModel.required
					}

					DataGroup {
						id: optionalData

						width: requiredData.visible ? parent.width / 2 : parent.width

						//: LABEL DESKTOP
						title: qsTr("Read access (optional)")
						columns: Math.max(1, requestedDataRow.maxColumns - (writeData.visible ? writeData.columns : 0) - (requiredData.visible ? requiredData.columns : 0))
						chat: ChatModel.optional
					}
				}
			}
		}

		GSpacer {
			Layout.fillWidth: true
			Layout.fillHeight: true
		}
	}

	CertificateDescriptionPage {
		id: certificateDescriptionPage

		visible: root.detailView

		onExit: showProviderInformation(false)
	}
}
