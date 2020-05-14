2020-05-13  Version 1.1.0
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

2020-05-01  Version 1.0.1
  * Updated Poco dependency to ~=1.9.4, allowing patch revisions via `conan install --update`.
    This addresses an HTTPS failure on Windows.

2019-11-22  Version 1.0.0
  * _*Master Release*_
  * Initial Revision
