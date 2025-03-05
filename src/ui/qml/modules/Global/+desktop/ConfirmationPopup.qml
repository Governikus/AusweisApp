/**
 * Copyright (c) 2018-2025 Governikus GmbH & Co. KG, Germany
 */

import QtQuick

import Governikus.Global
import Governikus.Style

BaseConfirmationPopup {
	id: root

	buttons: Row {
		layoutDirection: Qt.RightToLeft
		spacing: Style.dimens.pane_spacing
		width: parent.width

		GButton {
			text: root.okButtonText
			visible: root.style & ConfirmationPopup.PopupStyle.OkButton

			onClicked: root.accept()
		}
		GButton {
			text: root.cancelButtonText
			visible: root.style & ConfirmationPopup.PopupStyle.CancelButton

			onClicked: root.cancel()
		}
	}
}
