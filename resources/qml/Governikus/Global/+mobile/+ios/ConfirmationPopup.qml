/*
 * \copyright Copyright (c) 2018-2021 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

import Governikus.Global 1.0
import Governikus.Style 1.0


BaseConfirmationPopup {
	id: root

	headerTextStyle: Style.text.header
	horizontalTextAlignment: Text.AlignHCenter

	buttons: ColumnLayout {
		width: parent.width

		spacing: 0

		GSeparator {
			Layout.fillWidth: true
			visible: style !== ConfirmationPopup.PopupStyle.NoButtons
		}

		RowLayout {
			Layout.fillWidth: true

			spacing: 0

			GButton {
				visible: style & ConfirmationPopup.PopupStyle.CancelButton

				Layout.fillWidth: true

				buttonColor: Style.color.transparent
				text: root.cancelButtonText
				textStyle: Style.text.normal_accent

				onClicked: root.cancel()
			}

			GSeparator {
				visible: style & ConfirmationPopup.PopupStyle.CancelButton && style & ConfirmationPopup.PopupStyle.OkButton
				Layout.fillHeight: true
				orientation: Qt.Vertical
			}

			GButton {
				visible: style & ConfirmationPopup.PopupStyle.OkButton

				Layout.fillWidth: true

				buttonColor: Style.color.transparent
				text: root.okButtonText
				textStyle: Style.text.normal_accent

				onClicked: root.accept()
			}
		}
	}
}

