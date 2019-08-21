#include <iostream>

#include <BranchIO/Branch.h>
#include <BranchIO/Event/CustomEvent.h>
#include <BranchIO/Event/StandardEvent.h>
#include <BranchIO/Log.h>

#include "HelloCallback.h"

using BranchIO::Log;

int main(int argc, char *argv[])
{
    // Debug and Verbose compiled out in release builds
    Log::setLevel(Log::Verbose);
    std::cout << "Hello " << BranchIO::Branch::getVersion() << std::endl;

    std::string branchKey;

    if (argc > 1) {
        branchKey = argv[1];
    }
    if (branchKey.empty()) {
        std::cerr << "Missing BranchKey" << std::endl;
        return -1;
    }

    BranchIO::AppInfo appInfo;
    appInfo
        .setAppVersion("1.0")
        .setCountryCode("US")
        .setDeveloperIdentity("Branch Metrics")
        .setEnvironment("FULL_APP")
        .setLanguage("en");

    std::unique_ptr<BranchIO::Branch> branch(BranchIO::Branch::create(branchKey, &appInfo));

    HelloCallback callback;

    // Open a Branch Session
    branch->openSession("https://hello-branch.app.link/0urbx9YcQR", &callback);

    // Send a Branch Standard Event
    BranchIO::StandardEvent testEvent1(BranchIO::StandardEvent::Type::PURCHASE);
    testEvent1.setTransactionId("1234");
    branch->sendEvent(testEvent1, &callback);

    // Send a Custom Event
    BranchIO::CustomEvent testEvent2("Test2");
    testEvent2.setAdType(BranchIO::Event::AdType::BANNER);
    branch->sendEvent(testEvent2, &callback);

    callback.waitForResponses(3, 370000); // ms
    BRANCH_LOG_I("Expected 3 responses. Got " << callback.getResponseCount() << ".");

    return 0;
}
