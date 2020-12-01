/*
 * \copyright Copyright (c) 2016-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtGraphicalEffects 1.12

import Governikus.Style 1.0

Rectangle {
	id: root

	color: Style.color.background_pane
	radius: Style.dimens.corner_radius
	border.width: Style.dimens.high_contrast_item_border
	border.color: Style.color.high_contrast_item_border

	layer.enabled: !Constants.is_desktop && Constants.is_layout_android && GraphicsInfo.api !== GraphicsInfo.Software
	layer.effect: DropShadow {
					radius: 8
					samples: 8
					source: root
					color: Style.color.shadow
					verticalOffset: 2
				}
}
