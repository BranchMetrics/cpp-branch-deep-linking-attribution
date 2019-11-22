#include <iostream>

#include <BranchIO/Branch.h>
#include <BranchIO/Event/CustomEvent.h>
#include <BranchIO/Event/StandardEvent.h>
#include <BranchIO/Util/Log.h>

#include "HelloCallback.h"

using BranchIO::Log;

int main(int argc, char *argv[])
{
    // Debug and Verbose compiled out in release builds
    Log::setLevel(Log::Verbose);
    std::cout << "Hello " << BranchIO::Branch::getVersion() << std::endl;

    // Branch Key
    std::string branchKey = "key_live_efTsR1fbTucbHvX3N5RsOaamDtlPFLap";

    // Branch link
    std::string branchLink;

    if (argc > 1) {
        branchLink = argv[1];
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
    branch->openSession(branchLink, &callback);

    // Send a Branch Standard Event
    BranchIO::StandardEvent testEvent1(BranchIO::StandardEvent::Type::PURCHASE);
    testEvent1.setTransactionId("1234");
    branch->sendEvent(testEvent1, &callback);

    // Send a Custom Event
    BranchIO::CustomEvent testEvent2("Test2");
    testEvent2.setAdType(BranchIO::Event::AdType::BANNER);
    branch->sendEvent(testEvent2, &callback);

    // Close the Branch Session
    branch->closeSession(&callback);

    callback.waitForResponses(4, 370000); // ms
    BRANCH_LOG_I("Expected 4 responses. Got " << callback.getResponseCount() << ".");

    return 0;
}
