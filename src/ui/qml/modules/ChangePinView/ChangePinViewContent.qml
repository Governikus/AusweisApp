/**
 * Copyright (c) 2016-2026 Governikus GmbH & Co. KG, Germany
 */

import QtQuick
import QtQuick.Layouts

import Governikus.Global
import Governikus.MultiInfoView
import Governikus.Style

GFlickableColumnLayout {
	id: root

	readonly property alias pinInfo: infoData
	property double titleTopMargin: Style.dimens.pane_spacing

	signal changePin
	signal changePinInfoRequested
	signal changeTransportPin
	signal noPinAvailable

	maximumContentWidth: Style.dimens.max_text_width
	spacing: Style.dimens.pane_spacing

	Heading {
		Layout.topMargin: root.titleTopMargin

		//: ALL_PLATFORMS
		text: qsTr("What kind of PIN do you have?")
		wrapMode: Text.WordWrap
	}
	MoreInformationLink {
		Layout.alignment: Qt.AlignHCenter
		text: infoData.linkText

		onClicked: root.changePinInfoRequested()

		MultiInfoData {
			id: infoData

			contentType: MultiInfoData.Type.CHANGE_PIN
		}
	}
	PinSelectionButtons {
		Layout.alignment: Qt.AlignHCenter

		onFiveDigitPin: root.changeTransportPin()
		onNoPinAvailable: root.noPinAvailable()
		onSixDigitPin: root.changePin()
	}
}
