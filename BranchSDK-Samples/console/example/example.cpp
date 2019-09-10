#include <Poco/Util/Application.h>

#include <Poco/Util/Option.h>
#include <Poco/Util/OptionSet.h>
#include <Poco/Util/OptionCallback.h>
#include "Poco/Util/AbstractConfiguration.h"
#include <Poco/Util/HelpFormatter.h>

#include <iostream>
#include <BranchIO/Branch.h>
#include <BranchIO/AppInfo.h>
#include <BranchIO/Event/Event.h>
#include <BranchIO/Event/CustomEvent.h>
#include <BranchIO/Event/StandardEvent.h>
#include <BranchIO/Request.h>
#include <BranchIO/IRequestCallback.h>
#include <BranchIO/Log.h>

using Poco::Util::Application;
using Poco::Util::HelpFormatter;
using Poco::Util::Option;
using Poco::Util::OptionCallback;
using Poco::Util::OptionSet;
using Poco::Util::HelpFormatter;
using Poco::Util::AbstractConfiguration;
using Poco::Util::AbstractOptionCallback;

using namespace std;
using namespace BranchIO;

// Configuration File Key for the Branch Credentials
#define KEY_BRANCH "branch_key"
#define KEY_LINK "branch_link"

class ExampleApp : public Application, public IRequestCallback
{
public:
    ExampleApp() : _helpRequested(false), _shouldExit(false)
    {
    }

private:
	bool _helpRequested;
    bool _shouldExit;

    OptionSet _menuOptions;

    Branch *_branchInstance;
    AppInfo _appInfo;

    void initializeBranch()
    {
        if (!config().hasOption(KEY_BRANCH)) {
            cout << "Missing Branch Key, check your configuration" << endl;
            _shouldExit = true;
            return;
        }
        auto branch_key = config().getString(KEY_BRANCH);

        createBranchMenuOptions();

        // TODO: These should be "Configuration" pieces, not DeviceInfo pieces.
        _appInfo
            .setAppVersion("1.0")
            .setCountryCode("US")
            .setDeveloperIdentity("Branch Metrics")
            .setEnvironment("FULL_APP")
            .setLanguage("en");

        _branchInstance = Branch::create(branch_key, &_appInfo);
    }

    void uninitializeBranch()
    {
        delete _branchInstance;
    }

protected:
    // Override
    void initialize(Application& self)
    {
        this->loadConfiguration();
        Application::initialize(self);

        initializeBranch();
    }

    // Override
    void uninitialize()
    {
        uninitializeBranch();

        Application::uninitialize();
    }

    // Override
    void reinitialize(Application &self)
    {
    }

    // Override
    void defineOptions(OptionSet& optionSet) {
        Application::defineOptions(optionSet);

        optionSet.addOption(
            Option("help", "h", "Display help information on command line arguments")
                .required(false)
                .repeatable(false)
                .callback(OptionCallback<ExampleApp>(this, &ExampleApp::handleHelp)));

        optionSet.addOption(
            Option("version", "v", "Display version information")
                .required(false)
                .repeatable(false)
                .callback(OptionCallback<ExampleApp>(this, &ExampleApp::handleVersion)));

        optionSet.addOption(
            Option("config-file", "f", "Load configuration data from a file")
                .required(false)
                .repeatable(false)
                .argument("file")
                .callback(OptionCallback<ExampleApp>(this, &ExampleApp::handleConfig)));

        optionSet.addOption(
            Option("key", "k", "Branch key")
                .required(false)
                .repeatable(false)
                .argument("key")
                .callback(OptionCallback<ExampleApp>(this, &ExampleApp::handleBranchKey)));

        optionSet.addOption(
            Option("test", "t", "Run the test suite")
                .required(false)
                .repeatable(true)
                .argument("testName")
                .callback(OptionCallback<ExampleApp>(this, &ExampleApp::handleTest)));

        optionSet.addOption(
            Option("properties", "p", "Print the application properties")
                .required(false)
                .repeatable(false)
                .callback(OptionCallback<ExampleApp>(this, &ExampleApp::handleProperties)));
    }

    void createBranchMenuOptions()
    {
        _menuOptions.addOption(
            Option("1", "1", "Send Session Install Event")
                .callback(OptionCallback<ExampleApp>(this, &ExampleApp::handleSendInstallEvent)));

        _menuOptions.addOption(
            Option("2", "2", "Send Session Open Event")
                .callback(OptionCallback<ExampleApp>(this, &ExampleApp::handleSendOpenEvent)));

        _menuOptions.addOption(
            Option("3", "3", "Send Session Close Event")
                .callback(OptionCallback<ExampleApp>(this, &ExampleApp::handleSendCloseEvent)));

        _menuOptions.addOption(
            Option("4", "4", "Send Add To Cart Event")
                .callback(OptionCallback<ExampleApp>(this, &ExampleApp::handleSendAddToCartEvent)));

        _menuOptions.addOption(
            Option("5", "5", "Send Purchase Event")
                .callback(OptionCallback<ExampleApp>(this, &ExampleApp::handleSendPurchaseEvent)));

        _menuOptions.addOption(
            Option("6", "6", "Send Custom Event")
                .callback(OptionCallback<ExampleApp>(this, &ExampleApp::handleSendCustomEvent)));

        _menuOptions.addOption(
            Option("7", "7", "Send Multiple Events (async)")
                .callback(OptionCallback<ExampleApp>(this, &ExampleApp::handleSendMultipleEvents)));
    }

    void handleHelp(const std::string& name, const std::string& value)
	{
		_helpRequested = true;
		displayHelp();
		stopOptionsProcessing();
	}

    void handleVersion(const std::string& name, const std::string& value)
    {
        cout << "SDK Version: " << Branch::getVersion() << endl;
    }

    void handleProperties(const std::string &name, const std::string &value)
    {
        printProperties("");
    }

    void handleTest(const std::string &name, const std::string &value)
    {
        cout << "SDK Key: ";
        if (config().has("branch_key"))
        {
            cout << config().getString("branch_key");
        }
        else
        {
            cout << "Not Found";
        }
        cout << endl;

        if (config().has("debug"))
        {
            cout << "Debug: " << config().getBool("debug") << endl;
        }
    }

    void handleConfig(const std::string &name, const std::string &value) {
        cout << "Load Configuration: " << value << endl;
        loadConfiguration(value);
    }

    void handleBranchKey(const std::string &name, const std::string &value) {
        cout << "Branch Key: " << value << endl;
        config().setString(KEY_BRANCH, value);
    }

    void handleSendInstallEvent(const std::string &name, const std::string &value) {
        auto branch_link = config().getString(KEY_LINK);

        cout << "handleSendInstallEvent()" << endl;
        _branchInstance->openSession(branch_link, this);
    }

    void handleSendOpenEvent(const std::string &name, const std::string &value) {
        cout << "handleSendOpenEvent()" << endl;
        _branchInstance->openSession("", this);
    }

    void handleSendCloseEvent(const std::string &name, const std::string &value) {
        cout << "handleSendCloseEvent()" << endl;
        _branchInstance->closeSession(this);
    }

    void handleSendAddToCartEvent(const std::string &name, const std::string &value) {
        cout << "handleSendAddToCartEvent()" << endl;

        StandardEvent event(StandardEvent::Type::ADD_TO_CART);
        event
            .setDescription("Test Description")
            .addCustomDataProperty("product_id", "123")
            .addCustomDataProperty("foo", "bar");

        _branchInstance->sendEvent(event, this);
    }

    void handleSendPurchaseEvent(const std::string &name, const std::string &value) {
        cout << "handleSendPurchaseEvent()" << endl;

        StandardEvent event(StandardEvent::Type::PURCHASE);
        event
            .setCoupon("TestCoupon")
            .setCurrency("USD")
            .setDescription("Test Description")
            .setRevenue(99.99)
            .setSearchQuery("Some Search Query")
            .setShipping(1.99)
            .setTax(.99)
            .setTransactionId("Transaction123");

        _branchInstance->sendEvent(event, this);
    }

    void handleSendCustomEvent(const std::string &name, const std::string &value) {
        cout << "handleSendCustomEvent()" << endl;

        CustomEvent event("handleSendCustomEvent");
        event.setAdType(Event::AdType::BANNER);

        _branchInstance->sendEvent(event, this);
    }

    /**
     * Send a typical set of events that might happen during the lifecycle of an app.
     */
    void handleSendMultipleEvents(const std::string &name, const std::string &value) {
        cout << "handleSendMultipleEvents()" << endl;

        // Not really using name/value correctly, but this will work for now.
        handleSendInstallEvent(name, value);
        handleSendAddToCartEvent(name, value);
        handleSendPurchaseEvent(name, value);
        handleSendCustomEvent(name, value);
    }

    void displayHelp()
	{
		HelpFormatter helpFormatter(options());
		helpFormatter.setCommand(commandName());
		helpFormatter.setUsage("OPTIONS");
		helpFormatter.setHeader("A Branch sample application.");
		helpFormatter.format(cout);
	}

    void printProperties(const std::string& base)
	{
		AbstractConfiguration::Keys keys;
		config().keys(base, keys);
		if (keys.empty())
		{
			if (config().hasProperty(base))
			{
				std::string msg;
				msg.append(base);
				msg.append(" = ");
				msg.append(config().getString(base));
				logger().information(msg);
			}
		}
		else
		{
			for (AbstractConfiguration::Keys::const_iterator it = keys.begin(); it != keys.end(); ++it)
			{
				std::string fullKey = base;
				if (!fullKey.empty()) fullKey += '.';
				fullKey.append(*it);
				printProperties(fullKey);
			}
		}
    }

    // Interface IRequestCallback
    virtual void onSuccess(int id, JSONObject jsonResponse)
    {
        cout << "Callback Success!" << endl;
        cout << "onSuccess(): " << jsonResponse.stringify() << endl;
    }

    // Interface IRequestCallback
    virtual void onError(int id, int error, std::string description)
    {
        cout << "Callback Failed..." << endl;
    }

    // Interface IRequestCallback
    virtual void onStatus(int id, int error, std::string descirption)
    {
        cout << "Status updated" << endl;
    }

    bool tryProcessChoice(const Option &option, const std::string &choice)
    {
        bool match = option.matchesShort(choice);
        if (match)
        {
            AbstractOptionCallback *cb = option.callback();
            if (cb != NULL)
            {
                cb->invoke(option.fullName(), option.argumentName());
            }
        }

        return match;
    }

    bool processChoice(const std::string &choice)
    {
        bool processed = false;
        for (OptionSet::Iterator it1 = _menuOptions.begin(); it1 != _menuOptions.end() && !processed; ++it1)
        {
            processed = tryProcessChoice(*it1, choice);
        }
        for (OptionSet::Iterator it2 = options().begin(); it2 != options().end() && !processed; ++it2)
        {
            processed = tryProcessChoice(*it2, choice);
        }

        return processed;
    }

    void runMenu()
    {
        std::string choice;

        while (!_shouldExit)
        {
            cout << endl;
            cout << "Branch Menu" << endl;
            cout << "-------------------------" << endl;
            cout << "0. Exit" << endl;

            for (OptionSet::Iterator it1 = _menuOptions.begin(); it1 != _menuOptions.end(); ++it1)
            {
                const Option &o = *it1;
                cout << o.shortName() << ". " << o.description() << endl;
            }
            for (OptionSet::Iterator it2 = options().begin(); it2 != options().end(); ++it2)
            {
                const Option &o = *it2;
                cout << o.shortName() << ". " << o.description() << endl;
            }

            cout << endl << "Select Option >> ";
            std::getline(std::cin, choice);

            if (choice.compare("0") == 0)
            {
                _shouldExit = true;
                break;
            }
            else if (!processChoice(choice))
            {
                cout << "Invalid choice, try again" << endl;
            }

            cout << endl << "Press <enter> to continue... ";
            std::getline(std::cin, choice); // throw choice away
        }
    }

    // Override
    int main(const std::vector<std::string> &arguments)
    {
        // Debug and Verbose compiled out in release builds
        Log::setLevel(Log::Verbose);

        // Determine if help was requested from the commandline.  If so, we have
        // already shown help, so just exit.  Otherwise drop into the menu loop.
        if (!_helpRequested)
        {
            runMenu();
        }

        return Application::EXIT_OK;
    }

};

POCO_APP_MAIN(ExampleApp)
