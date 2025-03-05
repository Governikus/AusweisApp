/**
 * Copyright (c) 2016-2025 Governikus GmbH & Co. KG, Germany
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts

import Governikus.Global
import Governikus.Style
import Governikus.View
import Governikus.TitleBar
import Governikus.Type

FlickableSectionPage {
	id: root

	signal rightsAccepted

	fillWidth: true
	spacing: Style.dimens.pane_spacing

	Keys.onEnterPressed: confirmButton.clicked()
	Keys.onReturnPressed: confirmButton.clicked()

	RowLayout {
		Layout.alignment: Qt.AlignHCenter
		Layout.maximumWidth: Style.dimens.max_text_width * 1.2
		Layout.topMargin: Style.dimens.pane_padding
		spacing: Style.dimens.pane_spacing

		Image {
			id: npaIcon

			source: "qrc:///images/npa.svg"
			sourceSize.height: Style.dimens.huge_icon_size
		}
		ProviderInfo {
			Layout.fillWidth: true
			name: CertificateDescriptionModel.subjectName

			onClicked: root.push(certificateDescriptionPage)
		}
	}
	GButton {
		id: confirmButton

		Accessible.name: confirmButton.text
		Layout.alignment: Qt.AlignHCenter
		icon.source: "qrc:/images/identify.svg"
		//: LABEL DESKTOP %1 can be "CAN" or "PIN"
		text: qsTr("Proceed to %1 entry").arg((NumberModel.isCanAllowedMode ?
			//: LABEL DESKTOP Inserted into "Proceed to %1 entry"
			qsTr("CAN") :
			//: LABEL DESKTOP Inserted into "Proceed to %1 entry"
			qsTr("PIN")))
		tintIcon: true

		onClicked: root.rightsAccepted()
	}
	GText {
		Layout.alignment: Qt.AlignLeft
		text: NumberModel.isCanAllowedMode ?
		//: LABEL DESKTOP
		qsTr("By entering the CAN, access to the following data of the ID card will be allowed to the mentioned provider:") :
		//: LABEL DESKTOP
		qsTr("By entering your PIN, access to the following data of your ID card will be allowed to the mentioned provider:")
	}
	GPane {
		Layout.fillWidth: true
		color: Style.color.paneSublevel.background.basic

		//: LABEL DESKTOP
		title: qsTr("Transactional information")
		visible: !!AuthModel.transactionInfo || !requestedDataRow.visible

		onFocusChanged: if (focus)
			root.positionViewAtItem(this)

		GText {
			objectName: "transactionText"
			text: AuthModel.transactionInfo
			textFormat: Text.StyledText
			visible: !!text
		}
		GText {
			//: LABEL DESKTOP
			text: qsTr("The provider mentioned above does not require any data stored on your ID card, only confirmation of you possessing a valid ID card.")
			visible: !writeDataPane.visible && !readDataPane.visible
		}
	}
	RowLayout {
		id: requestedDataRow

		readonly property int maxColumns: 3

		Layout.alignment: Qt.AlignHCenter
		Layout.fillWidth: true
		spacing: Style.dimens.pane_spacing
		visible: writeData.count > 0 || requiredData.count > 0 || optionalData.count > 0

		GPane {
			id: writeDataPane

			Layout.alignment: Qt.AlignTop
			Layout.fillWidth: true
			border.color: Style.color.warning
			border.width: Style.dimens.pane_border_highlight_width
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

				spacing: Style.dimens.pane_spacing

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
	Component {
		id: certificateDescriptionPage

		CertificateDescriptionPage {
			titleBarSettings: TitleBarSettings {
				navigationAction: NavigationAction.Back

				onNavigationActionClicked: root.pop()
			}
		}
	}
}
