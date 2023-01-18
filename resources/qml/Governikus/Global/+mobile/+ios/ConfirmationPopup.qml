/*
 * \copyright Copyright (c) 2018-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Governikus.Global 1.0
import Governikus.Style 1.0

BaseConfirmationPopup {
	id: root
	headerTextStyle: Style.text.header
	horizontalTextAlignment: Text.AlignHCenter

	buttons: ColumnLayout {
		spacing: 0
		width: parent.width

		GSeparator {
			Layout.fillWidth: true
			visible: style !== ConfirmationPopup.PopupStyle.NoButtons
		}
		RowLayout {
			Layout.fillWidth: true
			spacing: 0

			GButton {
				Layout.fillWidth: true
				buttonColor: Style.color.transparent
				text: root.cancelButtonText
				textStyle: Style.text.normal_accent
				visible: style & ConfirmationPopup.PopupStyle.CancelButton

				onClicked: root.cancel()
			}
			GSeparator {
				Layout.fillHeight: true
				orientation: Qt.Vertical
				visible: style & ConfirmationPopup.PopupStyle.CancelButton && style & ConfirmationPopup.PopupStyle.OkButton
			}
			GButton {
				Layout.fillWidth: true
				buttonColor: Style.color.transparent
				text: root.okButtonText
				textStyle: Style.text.normal_accent
				visible: style & ConfirmationPopup.PopupStyle.OkButton

				onClicked: root.accept()
			}
		}
	}
}
