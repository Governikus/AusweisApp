/**
 * Copyright (c) 2019-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Controls
import Governikus.Global
import Governikus.Style

BaseConfirmationPopup {
	id: root

	buttons: Flow {
		layoutDirection: Qt.RightToLeft
		spacing: 0
		width: parent.width

		GButton {
			background: null
			text: root.okButtonText
			textStyle: Style.text.normal
			visible: style & ConfirmationPopup.PopupStyle.OkButton

			onClicked: root.accept()
		}
		GButton {
			background: null
			text: root.cancelButtonText
			textStyle: Style.text.normal
			visible: style & ConfirmationPopup.PopupStyle.CancelButton

			onClicked: root.cancel()
		}
	}
}
