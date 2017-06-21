#ifndef __SENSORCONFIGURATION_H__
#define __SENSORCONFIGURATION_H__

#include <string>

/*
	Configuration of a SensorTag
*/
class CSensorConfiguration {
	private:
		/*
			MAC address of the SensorTag
		*/
		std::string addr;
	public:
		/*
			Sets the MAC address of the SensorTag
			@param[in] addr	MAC addes
		*/
		void setAddr(std::string addr);

		/*
			Return the MAC addres of the SensorTag
			@return	MAC address as std::string
		*/
		std::string getAddr();

		/*
			Enables notification service for the SensorTag
			@return 0 for success, -1 for failure
		*/
		virtual int enableNotification();

		/*
			Disables notification service for the SensorTag
			@return 0 for success, -1 for failure
		*/
		virtual int disableNotification();
};

#endif /* __SENSORCONFIGURATION_H__ */
