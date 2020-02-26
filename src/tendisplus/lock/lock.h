#ifndef SRC_TENDISPLUS_LOCK_LOCK_H__
#define SRC_TENDISPLUS_LOCK_LOCK_H__

#include <string>
#include <utility>
#include <memory>

#include "tendisplus/lock/mgl/mgl.h"
#include "tendisplus/server/session.h"
#include "tendisplus/network/session_ctx.h"
#include "tendisplus/lock/mgl/mgl_mgr.h"

namespace tendisplus {

class Session;

class ILock {
 public:
    ILock(ILock* parent, mgl::MGLock* lk, Session* sess);
    virtual ~ILock();
    mgl::LockMode getMode() const;
    virtual uint32_t getStoreId() const;
    virtual std::string getKey() const;

 protected:
    static mgl::LockMode getParentMode(mgl::LockMode mode);
    std::unique_ptr<ILock> _parent;
    std::unique_ptr<mgl::MGLock> _mgl;
    // not owned
    Session* _sess;
};

// TODO(takenliu) : delete StoresLock
class StoresLock: public ILock {
 public:
    explicit StoresLock(mgl::LockMode mode, Session* sess, mgl::MGLockMgr* mgr,
            uint64_t lockTimeoutMs = 3600000);
    virtual ~StoresLock() = default;

 private:
    static const char _target[];
};

class StoreLock: public ILock {
 public:
    StoreLock(uint32_t storeId, mgl::LockMode mode,
              Session* sess, mgl::MGLockMgr* mgr,
              uint64_t lockTimeoutMs = 3600000);
    uint32_t getStoreId() const final;
    virtual ~StoreLock() = default;

 private:
    uint32_t _storeId;
};

class KeyLock: public ILock {
 public:
    static std::unique_ptr<KeyLock> AquireKeyLock(uint32_t storeId, const std::string& key,
            mgl::LockMode mode, Session* sess, mgl::MGLockMgr* mgr,
            uint64_t lockTimeoutMs = 3600000);
    KeyLock(uint32_t storeId, const std::string& key,
            mgl::LockMode mode, Session* sess, mgl::MGLockMgr* mgr,
            uint64_t lockTimeoutMs = 3600000);
    uint32_t getStoreId() const final;
    std::string getKey() const final;
    // remove lock from session before that lock has really been unlocked in its parent's destructor.
    virtual ~KeyLock();

 private:
    const std::string _key;
};

}  // namespace tendisplus
#endif  //  SRC_TENDISPLUS_LOCK_LOCK_H__
