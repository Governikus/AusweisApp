/**
 * Copyright (c) 2018-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Governikus.Global
import Governikus.Style

BaseConfirmationPopup {
	id: root

	buttons: Row {
		layoutDirection: Qt.RightToLeft
		spacing: Constants.component_spacing
		width: parent.width

		GButton {
			text: root.okButtonText
			visible: style & ConfirmationPopup.PopupStyle.OkButton

			onClicked: root.accept()
		}
		GButton {
			text: root.cancelButtonText
			visible: style & ConfirmationPopup.PopupStyle.CancelButton

			onClicked: root.cancel()
		}
	}
}
