#ifndef __MOCK_CLIENT_SESSION_H__
#define __MOCK_CLIENT_SESSION_H__

#include <gmock/gmock.h>

#include <BranchIO/Util/IClientSession.h>

struct MockClientSession : public virtual BranchIO::IClientSession
{
    void stop() {}
    MOCK_METHOD4(post, bool(const std::string&, const BranchIO::JSONObject&, BranchIO::IRequestCallback&, BranchIO::JSONObject&));
};

#endif // __MOCK_CLIENT_SESSION_H__
