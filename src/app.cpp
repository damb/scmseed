#include "app.h"

#include <fstream>
#include <iomanip>
#include <sstream>
#include <vector>

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

#include "log.h"
#include <seiscomp/io/records/mseedrecord.h>
#include <seiscomp/logging/log.h>
#include <seiscomp/utils/files.h>

#include "version.h"

namespace Seiscomp {
namespace MiniSeed {

namespace Detail {

bool parseTimeString(std::tm &t, const std::string &timeString) {
  std::stringstream iss{timeString};
  iss >> std::get_time(&t, "%Y-%m-%dT%H:%M:%S");
  return !iss.fail();
}

} // namespace Detail

Application::Application(int argc, char **argv)
    : Client::StreamApplication{argc, argv} {
  setDaemonEnabled(false);
  setMessagingEnabled(false);
  setDatabaseEnabled(false, false);
}

Application::~Application() {}

const char *Application::version() { return kVersion; }

void Application::handleRecord(Record *record) {
  if (!record->data()) {
    return;
  }

  SEISCOMP_DEBUG("[%s] %s - %s", record->streamID().c_str(),
                 record->startTime().iso().c_str(),
                 record->endTime().iso().c_str());

  if (record->typeInfo() != IO::MSeedRecord::TypeInfo()) {
    IO::MSeedRecord{*record}.write(std::cout);
  } else {
    record->write(std::cout);
  }
}

bool Application::init() {
  if (!StreamApplication::init()) {
    return false;
  }

  if (!_config.pathList.empty()) {
    // parse recordstream config from scart list file
    if (_config.pathList == "-") {
      parseList(std::cin);
    } else {
      std::ifstream ifs{_config.pathList};
      if (!ifs.is_open()) {
        SEISCOMP_ERROR("Failed to open list file");
      }
      parseList(ifs);
      if (ifs.bad()) {
        SEISCOMP_ERROR("Error while reading list file");
      }
      ifs.close();
    }
  } else {
    _streamConfigs.emplace_back(StreamConfig{"*", "*", "*", "*"});
  }

  return true;
}

bool Application::run() {
  for (const auto &streamConfig : _streamConfigs) {
    if (streamConfig.startTime && streamConfig.endTime) {
      recordStream()->addStream(streamConfig.netCode, streamConfig.staCode,
                                streamConfig.locCode, streamConfig.chaCode,
                                *streamConfig.startTime, *streamConfig.endTime);
    } else {
      recordStream()->addStream(streamConfig.netCode, streamConfig.staCode,
                                streamConfig.locCode, streamConfig.chaCode);
    }
  }

  return StreamApplication::run();
}

bool Application::initConfiguration() {
  if (!StreamApplication::initConfiguration()) {
    return false;
  }

  try {
    _config.pathList = configGetPath("list");
  } catch (...) {
  }

  return true;
}

void Application::createCommandLineDescription() {
  commandline().addGroup("Input");
  commandline().addOption("Input", "list",
                          "path to a list configuration file or '-' for stdin",
                          &_config.pathList);
}

bool Application::validateParameters() {
  if (commandline().hasOption("list")) {
    const auto pathList{commandline().option<std::string>("list")};
    if (pathList != "-") {
      Environment *env{Environment::Instance()};
      _config.pathList = env->absolutePath(pathList);

      if (!_config.pathList.empty() && !Util::fileExists(_config.pathList)) {
        SEISCOMP_ERROR("Invalid path to list configuration file: %s",
                       _config.pathList.c_str());
        return false;
      }
    }
  }

  return StreamApplication::validateParameters();
}

void Application::parseList(std::istream &is) {
  std::string timeFormat{"%Y-%m-%dT%H:%M:%S"};
  auto tmToTime = [](std::tm *tm) {
    return Core::Time{timeval{std::mktime(tm)}};
  };

  std::string line;
  while (std::getline(is, line)) {
    boost::algorithm::trim(line);
    // skip commented lines
    if (line.empty() || (!line.empty() && line[0] == '#')) {
      continue;
    }

    std::vector<std::string> tokensLine;
    boost::algorithm::split(tokensLine, line, boost::is_any_of(";"));
    if (tokensLine.size() != 3) {
      SEISCOMP_WARNING("Parsing error: skipping: \"%s\"", line.c_str());
      continue;
    }

    // parse SNCL
    const auto &streamID{tokensLine[2]};
    std::vector<std::string> tokensStream;
    boost::algorithm::split(tokensStream, streamID, boost::is_any_of("."));
    if (tokensStream.size() != 4) {
      SEISCOMP_WARNING("Parsing error (invalid stream id): skipping: \"%s\"",
                       line.c_str());
      continue;
    }

    boost::optional<Core::Time> startTime;
    if (!tokensLine[0].empty()) {
      std::tm start{};
      if (!Detail::parseTimeString(start, tokensLine[0])) {
        SEISCOMP_WARNING("[%s] Failed to parse starttime: skipping: \"%s\"",
                         streamID.c_str(), line.c_str());
        continue;
      }
      startTime = tmToTime(&start);
    } else {
      SEISCOMP_DEBUG("[%s] No starttime specified", streamID.c_str());
    }

    boost::optional<Core::Time> endTime;
    if (!tokensLine[0].empty()) {
      std::tm t{};
      if (!Detail::parseTimeString(t, tokensLine[1])) {
        SEISCOMP_WARNING("[%s] Failed to parse endtime: skipping: \"%s\"",
                         streamID.c_str(), line.c_str());
        continue;
      }
      endTime = tmToTime(&t);
    } else {
      SEISCOMP_DEBUG("[%s] No endtime specified", streamID.c_str());
    }

    _streamConfigs.emplace_back(StreamConfig{tokensStream[0], tokensStream[1],
                                             tokensStream[2], tokensStream[3],
                                             startTime, endTime});
  }
}
} // namespace MiniSeed
} // namespace Seiscomp
