#include <string>

using namespace std;

#ifndef REFERRALTRACKER_H_
#define REFERRALTRACKER_H_

#define MAX_ADDRESSES 30

class ReferralTracker
{
	public:
		ReferralTracker();
		void addIPAddress(string ipAddress);
		bool containsIPAddress(string ipAddress);
		int getNumberOfAddresses();

	private:
		int numAddresses;
		string referredIPs[MAX_ADDRESSES];
};

#endif /* REFERRALTRACKER_H_ */
