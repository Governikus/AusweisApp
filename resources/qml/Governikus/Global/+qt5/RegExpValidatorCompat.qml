/*
 * \copyright Copyright (c) 2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.15

RegExpValidator {
	property var expression
	regExp: expression
}
