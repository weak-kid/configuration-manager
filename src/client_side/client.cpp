#include <atomic>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <nlohmann/json.hpp>
#include <sdbus-c++/sdbus-c++.h>
#include <thread>
#include <unistd.h>

using namespace sdbus;
using namespace std::chrono_literals;
using json = nlohmann::json;

sdbus::ServiceName SERVICE_NAME{"com.system.configurationManager"};
sdbus::ObjectPath OBJECT_PATH{
    "/com/system/configurationManager/Application/confManagerApplication1"};
constexpr const char *INTERFACE_NAME =
    "com.system.configurationManager.Application.Configuration";

class ConfigClient {
public:
  ConfigClient()
      : proxy_(sdbus::createProxy(sdbus::ServiceName{SERVICE_NAME},
                                  sdbus::ObjectPath{OBJECT_PATH})) {
    proxy_->uponSignal("configurationChanged")
        .onInterface(INTERFACE_NAME)
        .call([this](const std::map<std::string, sdbus::Variant> &config) {
          updateConfiguration(config);
        });
    initialLoad();
  }

  void run() {
    worker_ = std::thread([this]() {
      while (!stop_) {
        std::unique_lock lock(mutex_);
        if (cv_.wait_for(lock, timeout_, [this] { return stop_.load(); }))
          break;
        std::cout << phrase_ << std::endl;
      }
    });
  }

  ~ConfigClient() {
    stop_ = true;
    cv_.notify_all();
    if (worker_.joinable())
      worker_.join();
  }

private:
  void initialLoad() {
    std::map<std::string, sdbus::Variant> config;
    proxy_->callMethod("GetConfiguration")
        .onInterface(INTERFACE_NAME)
        .storeResultsTo(config);
    updateConfiguration(config);
  }

  void
  updateConfiguration(const std::map<std::string, sdbus::Variant> &config) {
    std::lock_guard lock(mutex_);
    try {
      timeout_ =
          std::chrono::milliseconds(config.at("Timeout").get<uint32_t>());
      phrase_ = config.at("TimeoutPhrase").get<std::string>();
      cv_.notify_all();
    } catch (const std::exception &e) {
      std::cerr << "Configuration error: " << e.what() << std::endl;
    }
  }

  std::unique_ptr<sdbus::IProxy> proxy_;
  std::thread worker_;
  std::mutex mutex_;
  std::condition_variable cv_;
  std::atomic<bool> stop_{false};
  std::chrono::milliseconds timeout_{1000};
  std::string phrase_{"Default Phrase"};
};

int main() {
  ConfigClient client;
  client.run();
  while (true) {
    sleep(0);
  }
  return 0;
}
