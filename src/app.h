#ifndef SCMSEED_APP_H_
#define SCMSEED_APP_H_

#include <memory>
#include <string>
#include <vector>

#include <boost/optional.hpp>

#include <seiscomp/client/streamapplication.h>
#include <seiscomp/core/record.h>

namespace Seiscomp {
namespace MiniSeed {

class Application : public Client::StreamApplication {
public:
  Application(int argc, char **argv);
  ~Application();

  const char *version() override;

protected:
  void handleRecord(Record *record) override;

  bool init() override;
  bool run() override;

  bool initConfiguration() override;
  void createCommandLineDescription() override;
  bool validateParameters() override;

private:
  void parseList(std::istream &is);

  struct Config {
    std::string pathList;
  };

  struct StreamConfig {
    std::string netCode;
    std::string staCode;
    std::string locCode;
    std::string chaCode;
    boost::optional<Core::Time> startTime;
    boost::optional<Core::Time> endTime;
  };

  Config _config;

  using StreamConfigs = std::vector<StreamConfig>;
  StreamConfigs _streamConfigs;
};

} // namespace MiniSeed
} // namespace Seiscomp

#endif // SCMSEED_APP_H_
