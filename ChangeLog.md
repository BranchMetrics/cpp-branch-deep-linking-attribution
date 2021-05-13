## 2021-05-13  Version 1.2.2
  * [INTENG-12752] Fix long URL generation.
  * Added Get Long URL button to TestBed.
  * Added generated documentation (via Doxygen) to MSI distribution.
  * Removed all Unix support. Moved all examples to BranchSDK-Samples from BranchSDK-Samples/Windows.
  * Work on automation and distribution.

## 2021-03-16  Version 1.2.1
  * Updated to Poco 1.10.1.
  * Removed OpenSSL & zlib from distribution (unused on Windows).
  * Improved use of Poco::Net::HTTPSClient.
  * Added TestBed-Basic and Welcome.html to MSI.
  * Added TestBed-Conan example.

## 2021-02-15  Version 1.2.0
  * Corrected `link_identifier` in `/v1/open` requests.
  * Added `X-Branch-Request-Id` to debug logs.
  * Use api2.branch.io instead of api.branch.io for better security.
  * Corrected advertising identifiers in `/v1/open` requests.
  * Added support for `INITIATE_STREAM` and `COMPLETE_STREAM` standard v2 events.
  * Added MSI distribution.
  * Added multiple TestBed flavors to illustrate different modes of URI-scheme
    association.

  This release includes no breaking API changes.

  In addition to Conan, the SDK is also now distributed via MSI. This is attached
  to each GitHub release tag. This integration method is
  [thorougly documented](https://help.branch.io/developers-hub/docs/windows-cpp-basic-integration#1-download--install-the-branch-sdk)
  and much easier than using Conan with Visual Studio. Conan users may continue
  using Conan for integration.

  Several flavors of TestBed are available to
  [illustrate methods of integration and URI association](https://help.branch.io/developers-hub/docs/windows-cpp-testing#sample-apps).

  **Known issues**
  - The MSI has been signed with a self-signed certificate. This may result in
    Windows Defender generating an alert on first install of a new release.
    Select "More Info" to install the MSI. This will be addressed in a
    subsequent release.

## 2020-10-07  Version 1.1.2
  * Fixed deep linking when tracking disabled
  * Fixed developer identity (`setIdentity`/`logout`)
  * Removed all `/v1/close` traffic from the SDK
  * Implemented per-app Registry storage
  * Fixed bad OS Version sent to server
  * Introduced TestBed QA app

  This release includes no breaking API changes.

  Deep linking was incorrectly disabled in previous releases when tracking is
  disabled. In this release, open events will be sent to the server without any
  identifiable fields. The response will be received normally by the caller.
  Only opening links and generating short URLs will work with tracking disabled.
  All other Branch functionality will return errors.

  The `Branch::closeSession` method no longer does anything of value and may be
  removed. Session closes are automatically inferred by the server.

  **Per-app Storage**

  Storage of Branch-related settings is usually per app in all other existing
  SDKs, often because of app sandboxing. Win32 is a rare environment where apps
  frequently operate outside a sandbox and can collide. Most of the information
  Branch stores must be per app. This release scopes all Registry-based storage
  to the app key in use. As of this release, only one app key may be used at a
  time (only one `Branch` instance at a time).

  Previous builds erroneously stored information under
  `HKEY_CURRENT_USER\Software\BranchIO`. Now each app will use
  `HKCU\Software\BranchIO\<branch-key>`, e.g.
  `HKCU\Software\BranchIO\key_live_xyz`. Any setting from a previous version of
  the SDK not scoped to a Branch key, in particular
  `HKCU\Software\BranchIO\advertiser\trackingDisabled`, will be assumed to have
  come from the current app. These settings will be migrated to
  `HKCU\Software\BranchIO\<branch-key>`
  (`HKCU\Software\BranchIO\<branch-key>\advertiser\trackingDisabled`).

  **Developer Identity**

  The `AppInfo::setDeveloperIdentity` method no longer does anything. The way
  it was used in certain examples was misleading. "Developer Identity" is an
  _identity_ meaningful to the _developer_ of the app. That is usually a user
  ID in the app developer's environment. It simply identifies the user in events
  recorded by Branch in whatever way is convenient for the developer. This is
  distinct from Branch's own notion of an Identity, represented by the
  `identity_id`. Any calls to the `AppInfo::setDeveloperIdentity` method
  should be removed.

  To associate a developer identity with the current app session, use
  `Branch::setIdentity`. The `Branch::logout` function removes that association.
  A new function `Branch::getIdentity` returns the current developer identity.

  Note that `setIdentity` and `logout` must be called after opening a session.
  The identity is persistent through multiple sessions once the association is
  made or removed.

  **TestBed**

  The repo now includes a `TestBed` app for QA that represents a standard
  Win32 implementation of the SDK.

## 2020-08-12  Version 1.1.1
  * Added support for wide strings via BranchIO::String adapter class.

  All public API methods that accept a string now take a BranchIO::String,
  which can be constructed automatically from std::string, std::wstring,
  const char\* or const wchar_t\*. Two new getters, Branch::getBranchKeyW()
  and Branch::getVersionW(), were also introduced.

## 2020-05-13  Version 1.1.0
  * Improvements to thread safety of LinkInfo and its ancestors.
  * Eliminate queueing and retry of /v1/url requests. A long link will now be
    generated immediately on any request failure.

  This required some changes to the public interface of `BaseEvent`. The return
  types of `getCustomData()` and `name()` have changed to avoid returning
  pointers or references to unsynchronized objects from synchronized getters.

  ```c++
  BranchIO::JSONObject getCustomData() const;
  std::string name() const;
  ```

  In addition, the destructor of `LinkInfo` will now block while any background
  request is in progress. Once `createUrl()` is called, the `LinkInfo` object
  is guaranteed to outlive the thread that executes the request. To force
  immediate termination of the background thread, use `LinkInfo::cancel()`.

  ~~Also note that each `LinkInfo` instance can currently only be used once to
  generate a URL. For now, create a new `LinkInfo` instance for each URL
  request. This will be improved in a future release.~~ Each 'LinkInfo' instance
  is reusable.

## 2020-05-01  Version 1.0.1
  * Updated Poco dependency to ~=1.9.4, allowing patch revisions via `conan install --update`.
    This addresses an HTTPS failure on Windows.

## 2019-11-22  Version 1.0.0
  * _*Master Release*_
  * Initial Revision
