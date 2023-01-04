#include "fpcsc/perf_util/sleep.h"
#include "fpcsc/perf_util/feature_cmd.h"

#include <string>
#include <algorithm>

static void send(void *Data) {
  asm volatile("" : : "g"(Data) : "memory");
}

class PackageData {
public:
  PackageData() = default;
  PackageData(std::string Data) : Data{std::move(Data)}, Size{this->Data.size()} {}

  std::string Data;
  unsigned long Size;
};

static bool UseEncryption = false;
static bool UseCompression =  false;
static long Iterations = 0;

void loadConfigFromArgv(int argc, char *argv[]) {
  if (fpcsc::isFeatureEnabled(argc, argv, std::string("--enc"))) {
    UseEncryption = true;
  }
  if (fpcsc::isFeatureEnabled(argc, argv, std::string("--compress"))) {
    UseCompression = true;
  }

  Iterations = fpcsc::getFeatureValue(argc, argv, "--iterations");
}


void sendPackage(PackageData Data) {
  if (Iterations) {
    fpcsc::sleep_for_secs(2*Iterations);
  }

  if (UseCompression) {
    Data.Data = Data.Data.substr(0, Data.Data.size() / 2);
    fpcsc::sleep_for_secs(3);

    if (Iterations) {
      fpcsc::sleep_for_secs(Iterations);
    }
  }

  if (UseEncryption) {
    std::reverse(Data.Data.begin(), Data.Data.end());
    fpcsc::sleep_for_secs(10);
  }

  // Sending
  fpcsc::sleep_for_secs(2);
  send(&Data);

  std::puts(Data.Data.c_str());
}

int main(int argc, char *argv[] ) {

  loadConfigFromArgv(argc, argv);

  auto UserData = PackageData(std::string(*argv));

  sendPackage(UserData);

  return 0;
}
