/**
 * Copyright (c) 2016-2023 Governikus GmbH & Co. KG, Germany
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
		detailView = show;
		if (!detailView)
			onVisibleChanged();
		updateTitleBarActions();
	}

	Keys.onEnterPressed: event => d.onKeyboardConfirmPressed(event)
	Keys.onEscapePressed: event => {
		if (!detailView) {
			event.accepted = false;
			return;
		}
		showProviderInformation(false);
	}
	Keys.onReturnPressed: event => d.onKeyboardConfirmPressed(event)

	QtObject {
		id: d
		function onKeyboardConfirmPressed(event) {
			if (detailView) {
				showProviderInformation(false);
			} else {
				confirmButton.onClicked();
			}
		}
	}
	ColumnLayout {
		anchors.fill: parent
		anchors.topMargin: (providerImage.height / 2) + (Constants.pane_padding * 2)
		spacing: Constants.pane_spacing
		visible: !root.detailView

		Rectangle {
			id: providerRect
			Layout.fillWidth: true
			Layout.preferredHeight: providerColumn.height
			border.color: Style.color.high_contrast_item_border
			border.width: Style.dimens.high_contrast_item_border
			color: Style.color.background_pane

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
				bottomPadding: Constants.pane_padding
				spacing: Constants.pane_spacing
				topPadding: Constants.pane_padding

				anchors {
					left: providerImage.right
					margins: Constants.pane_padding
					right: parent.right
				}
				ProviderInfoSection {
					id: providerInfo
					activeFocusOnTab: true
					image: "qrc:///images/provider/information.svg"
					name: CertificateDescriptionModel.subjectName
					//: LABEL DESKTOP
					title: qsTr("You are about to identify yourself towards the following provider")

					anchors {
						left: parent.left
						right: parent.right
						rightMargin: Constants.component_spacing
					}
				}
				Item {
					height: Math.max(detailsButton.height, confirmButton.height)

					anchors {
						left: parent.left
						right: parent.right
					}
					GButton {
						id: detailsButton
						Accessible.description: qsTr("Show more information about the service provider")
						activeFocusOnTab: true
						icon.source: "qrc:///images/desktop/info_white.svg"

						//: LABEL DESKTOP
						text: qsTr("Details about the provider")

						onClicked: showProviderInformation(true)

						anchors {
							left: parent.left
							top: parent.top
						}
					}
					GButton {
						id: confirmButton
						Accessible.name: confirmButton.text
						activeFocusOnTab: true
						icon.source: "qrc:///images/identify.svg"
						//: LABEL DESKTOP %1 can be "CAN" or "PIN"
						text: qsTr("Proceed to %1 entry").arg(NumberModel.isCanAllowedMode ?
							//: LABEL DESKTOP Inserted into "Proceed to %1 entry"
							qsTr("CAN") :
							//: LABEL DESKTOP Inserted into "Proceed to %1 entry"
							qsTr("PIN"))
						tintIcon: true

						onClicked: {
							ChatModel.transferAccessRights();
							AuthModel.continueWorkflow();
						}

						anchors {
							bottom: parent.bottom
							right: parent.right
						}
					}
				}
			}
		}
		GText {
			id: dataIntroduction
			Accessible.name: dataIntroduction.text
			Layout.fillWidth: true
			Layout.leftMargin: Constants.pane_padding
			Layout.rightMargin: Constants.pane_padding
			activeFocusOnTab: true
			text: NumberModel.isCanAllowedMode ?
			//: LABEL DESKTOP
			qsTr("By entering the CAN, access to the following data of the ID card will be allowed to the mentioned provider:") :
			//: LABEL DESKTOP
			qsTr("By entering your PIN, access to the following data of your ID card will be allowed to the mentioned provider:")
			textStyle: Style.text.normal

			FocusFrame {
			}
		}
		ScrollablePane {
			Layout.fillHeight: true
			Layout.fillWidth: true
			Layout.leftMargin: Constants.pane_padding
			Layout.maximumHeight: implicitHeight
			Layout.rightMargin: Constants.pane_padding
			activeFocusOnTab: true

			//: LABEL DESKTOP
			title: qsTr("Transactional information")
			visible: !!AuthModel.transactionInfo || !requestedDataRow.visible

			GText {
				id: transactionText
				Accessible.name: transactionText.text
				activeFocusOnTab: true
				text: AuthModel.transactionInfo
				textStyle: Style.text.normal
				visible: !!text
				width: parent.width

				FocusFrame {
				}
			}
			GText {
				id: noAccessRightsText
				Accessible.name: noAccessRightsText.text
				activeFocusOnTab: true

				//: LABEL DESKTOP
				text: qsTr("The provider mentioned above does not require any data stored on your ID card, only confirmation of you possessing a valid ID card.")
				textStyle: Style.text.normal
				visible: !writeDataPane.visible && !readDataPane.visible
				width: parent.width

				FocusFrame {
				}
			}
		}
		Row {
			id: requestedDataRow

			readonly property int columnWidth: (width - spacing) / maxColumns
			readonly property int maxColumns: 3

			Layout.fillWidth: true
			Layout.leftMargin: Constants.pane_padding
			Layout.preferredHeight: Math.max(writeDataPane.implicitHeight, readDataPane.implicitHeight)
			Layout.rightMargin: Constants.pane_padding
			spacing: Constants.pane_spacing
			visible: writeData.count > 0 || requiredData.count > 0 || optionalData.count > 0

			GPane {
				id: writeDataPane
				height: parent.height
				visible: writeData.count > 0
				width: readDataPane.visible ? requestedDataRow.columnWidth : parent.width

				DataGroup {
					id: writeData
					chat: ChatModel.write
					columns: readDataPane.visible ? 1 : requestedDataRow.maxColumns

					//: LABEL DESKTOP
					title: qsTr("Write access (update)")
					titleStyle: Style.text.header_warning
					width: parent.width
				}
			}
			GPane {
				id: readDataPane
				height: parent.height
				visible: requiredData.count > 0 || optionalData.count > 0
				width: writeDataPane.visible ? requestedDataRow.columnWidth * 2 : parent.width

				Row {
					spacing: Constants.pane_spacing
					width: parent.width

					DataGroup {
						id: requiredData
						chat: ChatModel.required
						columns: Math.max(1, requestedDataRow.maxColumns - (writeData.visible ? writeData.columns : 0) - (optionalData.visible ? 1 : 0) - (count > optionalData.count ? 0 : 1))

						//: LABEL DESKTOP
						title: qsTr("Read access")
						width: optionalData.visible ? parent.width / 2 : parent.width
					}
					DataGroup {
						id: optionalData
						chat: ChatModel.optional
						columns: Math.max(1, requestedDataRow.maxColumns - (writeData.visible ? writeData.columns : 0) - (requiredData.visible ? requiredData.columns : 0))

						//: LABEL DESKTOP
						title: qsTr("Read access (optional)")
						width: requiredData.visible ? parent.width / 2 : parent.width
					}
				}
			}
		}
		GSpacer {
			Layout.fillHeight: true
			Layout.fillWidth: true
		}
	}
	CertificateDescriptionPage {
		id: certificateDescriptionPage
		visible: root.detailView

		onExit: showProviderInformation(false)
	}
}
