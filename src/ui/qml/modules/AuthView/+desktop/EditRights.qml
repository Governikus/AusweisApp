/**
 * Copyright (c) 2016-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Governikus.Global
import Governikus.Style
import Governikus.TitleBar
import Governikus.View
import Governikus.Type

SectionPage {
	id: root

	function showProviderInformation(show) {
		d.detailView = show;
		if (!d.detailView) {
			root.visibleChanged();
		}
		root.updateTitleBarActions();
	}

	Keys.onEnterPressed: event => d.onKeyboardConfirmPressed(event)
	Keys.onEscapePressed: event => {
		if (!d.detailView) {
			event.accepted = false;
			return;
		}
		showProviderInformation(false);
	}
	Keys.onReturnPressed: event => d.onKeyboardConfirmPressed(event)

	FlickableSectionPage {
		anchors.fill: parent
		fillWidth: true
		spacing: Constants.pane_spacing
		visible: !d.detailView

		RowLayout {
			Layout.alignment: Qt.AlignHCenter
			Layout.maximumWidth: Style.dimens.max_text_width * 1.2
			Layout.topMargin: Constants.pane_padding
			spacing: Constants.pane_spacing

			Image {
				id: npaIcon

				source: "qrc:///images/npa.svg"
				sourceSize.height: Style.dimens.huge_icon_size
			}
			ProviderInfo {
				Layout.fillWidth: true
				activeFocusOnTab: true
				name: CertificateDescriptionModel.subjectName

				onClicked: showProviderInformation(true)
			}
		}
		GButton {
			id: confirmButton

			Accessible.name: confirmButton.text
			Layout.alignment: Qt.AlignHCenter
			activeFocusOnTab: true
			icon.source: "qrc:/images/identify.svg"
			//: LABEL DESKTOP %1 can be "CAN" or "PIN"
			text: qsTr("Proceed to %1 entry").arg((NumberModel.isCanAllowedMode ?
				//: LABEL DESKTOP Inserted into "Proceed to %1 entry"
				qsTr("CAN") :
				//: LABEL DESKTOP Inserted into "Proceed to %1 entry"
				qsTr("PIN")))
			tintIcon: true

			onClicked: {
				ChatModel.transferAccessRights();
				AuthModel.continueWorkflow();
			}
		}
		GText {
			id: dataIntroduction

			Accessible.name: dataIntroduction.text
			Layout.alignment: Qt.AlignLeft
			activeFocusOnTab: true
			text: NumberModel.isCanAllowedMode ?
			//: LABEL DESKTOP
			qsTr("By entering the CAN, access to the following data of the ID card will be allowed to the mentioned provider:") :
			//: LABEL DESKTOP
			qsTr("By entering your PIN, access to the following data of your ID card will be allowed to the mentioned provider:")

			FocusFrame {
			}
		}
		GPane {
			Layout.fillWidth: true
			activeFocusOnTab: true
			color: Style.color.paneSublevel.background.basic

			//: LABEL DESKTOP
			title: qsTr("Transactional information")
			visible: !!AuthModel.transactionInfo || !requestedDataRow.visible

			GText {
				id: transactionText

				Accessible.name: transactionText.text
				activeFocusOnTab: true
				text: AuthModel.transactionInfo
				textFormat: Text.PlainText
				visible: !!text

				FocusFrame {
				}
			}
			GText {
				id: noAccessRightsText

				Accessible.name: noAccessRightsText.text
				activeFocusOnTab: true

				//: LABEL DESKTOP
				text: qsTr("The provider mentioned above does not require any data stored on your ID card, only confirmation of you possessing a valid ID card.")
				visible: !writeDataPane.visible && !readDataPane.visible

				FocusFrame {
				}
			}
		}
		RowLayout {
			id: requestedDataRow

			readonly property int maxColumns: 3

			Layout.alignment: Qt.AlignHCenter
			Layout.fillWidth: true
			spacing: Constants.pane_spacing
			visible: writeData.count > 0 || requiredData.count > 0 || optionalData.count > 0

			GPane {
				id: writeDataPane

				Layout.alignment: Qt.AlignTop
				Layout.fillWidth: true
				border.color: Style.color.warning
				border.width: Constants.pane_border_highlight_width
				color: Style.color.paneSublevel.background.basic
				visible: writeData.count > 0

				DataGroup {
					id: writeData

					Layout.fillWidth: true
					chat: ChatModel.write
					columns: readDataPane.visible ? 1 : requestedDataRow.maxColumns

					//: LABEL DESKTOP
					title: qsTr("Write access (update)")
					titleStyle: Style.text.headline
					writeAccess: true
				}
			}
			GPane {
				id: readDataPane

				Layout.alignment: Qt.AlignTop
				Layout.fillWidth: true
				color: Style.color.paneSublevel.background.basic
				visible: requiredData.count > 0 || optionalData.count > 0

				RowLayout {
					id: readRow

					readonly property real columnWidth: Math.max(requiredData.implicitWidth / requiredData.columns, optionalData.implicitWidth / optionalData.columns)

					spacing: Constants.pane_spacing

					DataGroup {
						id: requiredData

						Layout.alignment: Qt.AlignTop | Qt.AlignRight
						Layout.fillWidth: true
						Layout.preferredWidth: readRow.columnWidth * columns
						chat: ChatModel.required
						columns: Math.max(1, requestedDataRow.maxColumns - (writeData.visible ? writeData.columns : 0) - (optionalData.visible ? 1 : 0) - (count > optionalData.count ? 0 : 1))

						//: LABEL DESKTOP
						title: qsTr("Read access")
					}
					DataGroup {
						id: optionalData

						Layout.alignment: Qt.AlignTop | Qt.AlignLeft
						Layout.fillWidth: true
						Layout.preferredWidth: readRow.columnWidth * columns
						chat: ChatModel.optional
						columns: Math.max(1, requestedDataRow.maxColumns - (writeData.visible ? writeData.columns : 0) - (requiredData.visible ? requiredData.columns : 0))

						//: LABEL DESKTOP
						title: qsTr("Read access (optional)")
					}
				}
			}
		}
	}
	CertificateDescriptionPage {
		anchors.fill: parent
		visible: d.detailView
	}
	QtObject {
		id: d

		property bool detailView: false

		function onKeyboardConfirmPressed(event) {
			if (d.detailView) {
				showProviderInformation(false);
			} else {
				confirmButton.clicked();
			}
		}
	}
}
