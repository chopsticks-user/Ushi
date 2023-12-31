#ifndef USHI_INCLUDE_USHI_HPP
#define USHI_INCLUDE_USHI_HPP

#if __cplusplus >= 201402L
#include "Base.hpp"
#include "ContractManager.hpp"
#include "JobContract.hpp"
#include "Worker.hpp"
#include "WorkerManager.hpp"

namespace ushi {

class ThreadPool {
public:
  static u32 nConcurrentThreads() noexcept {
    return std::thread::hardware_concurrency();
  }

public:
  ThreadPool() = delete;
  ThreadPool(const ThreadPool &) = delete;
  ThreadPool(ThreadPool &&) noexcept = default;
  ThreadPool &operator=(const ThreadPool &) = delete;
  ThreadPool &operator=(ThreadPool &&) noexcept = default;
  ~ThreadPool() noexcept { this->clear(); }

  explicit ThreadPool(u64 nThreads) noexcept
      : mContractManager{std::make_unique<ContractManager>()},
        mWorkerManager{std::make_unique<WorkerManager>(nThreads)}, mMutex{} {}

  u64 nThreads() const noexcept { return this->mWorkerManager->nWorkers(); }

  u64 nAvailableThreads() const noexcept {
    return this->mWorkerManager->nAvailableWorkers();
  }

  bool busy() const noexcept {
    return this->mWorkerManager->nAvailableWorkers() == 0;
  }

  ContractStatusPtr submitJob(JobType job) noexcept {
    ScopedLockType lock{this->mMutex};

    this->mContractManager->addContract(std::move(job));

    return this->mWorkerManager->assignJob(
        this->mContractManager->getContract());
  }

  // TODO: submitJob with a user-defined contract status
  // template <typename CustomStatusType> //
  // ContractStatusPtr
  // submitJob(std::function<void(CustomStatusType &)> job) noexcept {
  //   ScopedLockType lock{this->mMutex};

  //   this->mContractManager->addContract([&job](ContractStatus &status) {
  //     return job(static_cast<CustomStatusType &>(status));
  //   });

  //   return this->mWorkerManager->assignJob(
  //       this->mContractManager->getContract());
  // }

  void clear() noexcept {
    this->mWorkerManager->clear();
    this->mContractManager->clear();
  }

private:
  std::unique_ptr<ContractManager> mContractManager;
  std::unique_ptr<WorkerManager> mWorkerManager;
  std::mutex mMutex;
};

} // namespace ushi
#else  // C++11 or older
static_assert(__cplusplus >= 201402L, "Ushi library requires C++14 or newer");
#endif // C++14 or later

#endif // USHI_INCLUDE_USHI_HPP