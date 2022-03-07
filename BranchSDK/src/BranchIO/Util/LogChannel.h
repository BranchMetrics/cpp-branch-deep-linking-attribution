#pragma once
#include <string>


namespace BranchIO {

	/**
	 * (Internal) The base class for all Log Channel classes.
	 *
	 **/

	class LogChannel
	{
	public:

		/**
		 * Creates the channel and initializes it
		 */
		LogChannel();

		/**
		 * Opens the channel.
		 */
		virtual bool open();

		/**
		 * Closes the channel.
		 */
		virtual void close();

		/**
		 * Logs messages to the channel.
		 */
		virtual void log(const std::string& message) = 0;

	protected:
		virtual ~LogChannel();

	private:
		LogChannel(const LogChannel&);
	};
}


