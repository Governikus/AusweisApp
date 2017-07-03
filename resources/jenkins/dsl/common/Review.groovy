package common

import common.Trigger

class Review extends Trigger
{
	String namePrefix = 'Review_'
	String label = 'Review'
	String trigger = null
	List oldBuilds = [14, -1]
	int timeout = 60
	boolean review = true
}
