/*
 * \copyright Copyright (c) 2019-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQml 2.15
import Governikus.Type.ApplicationModel 1.0

Dimensions {
	readonly property int high_contrast_item_border: Math.max(ApplicationModel.scaleFactor * 2, 1)
	readonly property int tabbed_pane_separator_size: 0
}
