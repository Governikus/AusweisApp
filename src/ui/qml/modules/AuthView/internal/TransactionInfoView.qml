/**
 * Copyright (c) 2026 Governikus GmbH & Co. KG, Germany
 */

pragma ComponentBehavior: Bound

import QtQuick

import Governikus.Global
import Governikus.Style
import Governikus.View

FlickableSectionPage {
	property alias transactionText: transactionText.text

	spacing: Style.dimens.text_spacing

	Subheading {
		//: ALL_PLATFORMS
		text: qsTr("Transactional information")
	}
	GText {
		id: transactionText

		textFormat: Text.StyledText
		visible: !!text
	}
}
