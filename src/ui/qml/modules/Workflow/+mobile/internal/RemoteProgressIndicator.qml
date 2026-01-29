/**
 * Copyright (c) 2017-2026 Governikus GmbH & Co. KG, Germany
 */

import QtQuick
import QtQuick.Layouts

import Governikus.Animations
import Governikus.Style

Item {
	id: root

	property bool foundSelectedReader: false

	Layout.maximumHeight: Style.dimens.workflow_progress_indicator_size
	Layout.minimumHeight: Style.dimens.workflow_progress_indicator_size
	Layout.preferredHeight: Style.dimens.workflow_progress_indicator_size

	AnimationLoader {
		anchors.centerIn: parent
		animated: root.foundSelectedReader
		type: root.foundSelectedReader ? AnimationLoader.Type.WAIT_FOR_CARD_SAC : AnimationLoader.Type.WAIT_FOR_SAC
	}
}
