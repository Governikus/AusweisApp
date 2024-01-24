/**
 * Copyright (c) 2018-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Governikus.Global
import Governikus.Style

BaseConfirmationPopup {
	id: root

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
				background: null
				text: root.cancelButtonText
				textStyle: Style.text.normal
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
				background: null
				text: root.okButtonText
				textStyle: Style.text.normal
				visible: style & ConfirmationPopup.PopupStyle.OkButton

				onClicked: root.accept()
			}
		}
	}
}
