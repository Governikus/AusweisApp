/*
 * \copyright Copyright (c) 2019-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtQuick.Layouts 1.12

import Governikus.Global 1.0
import Governikus.Style 1.0

RowLayout {
	id: baseItem

	property alias title: labeledText.label
	property alias description: labeledText.text
	property alias iconSource: icon.source

	property alias buttonText: button.text
	property alias buttonTooltip: button.enabledTooltipText
	property alias buttonIconSource: button.icon.source

	signal clicked()

	spacing: Constants.component_spacing

	TintableIcon {
		id: icon

		sourceSize.width: Style.dimens.icon_size

		tintColor: Style.color.accent
	}

	LabeledText {
		id: labeledText

		Layout.fillWidth: true
	}

	GButton {
		id: button

		tintIcon: true
		onClicked: baseItem.clicked()
	}
}
