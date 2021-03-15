/*
 * \copyright Copyright (c) 2019-2021 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12

import Governikus.Type.ApplicationModel 1.0

Item {
	function open() {
		ApplicationModel.showAppStoreRatingDialog();
	}
}
