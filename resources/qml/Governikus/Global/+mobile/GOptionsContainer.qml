/**
 * Copyright (c) 2023 Governikus GmbH & Co. KG, Germany
 */

import QtQuick
import QtQuick.Layouts
import Governikus.Global
import Governikus.Style

ColumnLayout {
	default property alias containerData: pane.paneData
	property alias containerPadding: pane.padding
	property alias containerSpacing: pane.spacing
	property alias title: titleText.text

	spacing: Constants.component_spacing

	GText {
		id: titleText

		Accessible.role: Accessible.Heading
		textStyle: Style.text.headline
		visible: text !== ""
	}
	GPane {
		id: pane

		Layout.fillWidth: true
		color: Style.color.pane
		padding: 0
		spacing: 0
	}
}
