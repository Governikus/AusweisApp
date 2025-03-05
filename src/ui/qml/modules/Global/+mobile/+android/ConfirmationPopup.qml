/**
 * Copyright (c) 2019-2025 Governikus GmbH & Co. KG, Germany
 */

import QtQuick

import Governikus.Global

BaseConfirmationPopup {
	id: root

	buttons: Flow {
		layoutDirection: Qt.RightToLeft
		spacing: 0
		width: parent.width

		GLink {
			text: root.okButtonText
			visible: root.style & ConfirmationPopup.PopupStyle.OkButton

			onClicked: root.accept()
		}
		GLink {
			text: root.cancelButtonText
			visible: root.style & ConfirmationPopup.PopupStyle.CancelButton

			onClicked: root.cancel()
		}
	}
}
