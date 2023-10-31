#include "lib/globals.h"
#include "lib/flags.h"
#include "lib/fluxmap.h"
#include "lib/sector.h"
#include "lib/proto.h"
#include "lib/readerwriter.h"
#include "lib/imagereader/imagereader.h"
#include "lib/imagewriter/imagewriter.h"
#include "lib/fluxsource/fluxsource.h"
#include "lib/decoders/decoders.h"
#include "fluxengine.h"
#include "lib/vfs/sectorinterface.h"
#include "lib/vfs/vfs.h"
#include "lib/utils.h"
#include "src/fileutils.h"
#include <google/protobuf/text_format.h>
#include <fstream>

static FlagGroup flags({&fileFlags});

static StringFlag directory({"-p", "--path"}, "path to work on", "");

int mainGetFileInfo(int argc, const char* argv[])
{
    if (argc == 1)
        showProfiles("getfileinfo", formats);
    flags.parseFlagsWithConfigFiles(argc, argv, formats);

    try
    {
        auto filesystem = Filesystem::createFilesystemFromConfig();
        auto dirent = filesystem->getDirent(Path(directory));

        for (const auto& e : dirent->attributes)
            fmt::print("{}={}\n", e.first, quote(e.second));
    }
    catch (const FilesystemException& e)
    {
        error("{}", e.message);
    }

    return 0;
}
