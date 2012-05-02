/*
 * referralTracker.cpp
 *
 *  Created on: May 1, 2012
 *      Author: rebecca
 */
#include <stdio.h>
#include <string>
using namespace std;

#include "../common/referral_tracker.h"

ReferralTracker::ReferralTracker()
{
	numAddresses = 0;
}

void ReferralTracker::addIPAddress(string ipAddress)
{
	numAddresses++;
  	if (numAddresses > MAX_ADDRESSES)
   		throw "Exceeded ReferralTracker capacity";

  	referredIPs[numAddresses-1]= ipAddress;

//  	printf("\n*** Added IP %s", ipAddress.c_str());
}

bool ReferralTracker::containsIPAddress(string ipAddress)
{

	for(int i=0; i<numAddresses; i++)
	{
		if (ipAddress == referredIPs[i])
			return true;
	}
	return false;
}

int ReferralTracker::getNumberOfAddresses()
{
	return numAddresses;
}
