#include <cstdlib>

#include "log.h"
#include <seiscomp/core/baseobject.h>
#include <seiscomp/logging/log.h>

#include "app.h"

int main(int argc, char **argv) {
  int code{EXIT_SUCCESS};

  // Create an own block to make sure the application object
  // is destroyed when printing the overall object count.
  { code = Seiscomp::MiniSeed::Application{argc, argv}(); }

  SEISCOMP_DEBUG("EXIT(%d), remaining objects: %d", code,
                 Seiscomp::Core::BaseObject::ObjectCount());

  return code;
}
