/*
 * \copyright Copyright (c) 2016-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtGraphicalEffects 1.0

import Governikus.Global 1.0
import Governikus.Style 1.0

ListItem {
	implicitHeight: Constants.is_layout_ios ? 44 : Style.dimens.list_item_height
	implicitWidth: parent.width
}
