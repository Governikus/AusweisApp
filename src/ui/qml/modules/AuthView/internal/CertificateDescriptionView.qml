/**
 * Copyright (c) 2015-2026 Governikus GmbH & Co. KG, Germany
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts

import Governikus.Global
import Governikus.Style
import Governikus.Type
import Governikus.View

FlickableSectionPage {
	id: root

	spacing: Style.dimens.pane_spacing

	Repeater {
		model: CertificateDescriptionModel

		LabeledText {
			Layout.fillWidth: true
			textFormat: Text.PlainText

			onFocusChanged: if (focus)
				root.positionViewAtItem(this)
		}
	}
}
