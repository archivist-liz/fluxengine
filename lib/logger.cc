#include "globals.h"
#include "bytes.h"
#include "fluxmap.h"
#include "sector.h"
#include "flux.h"
#include "fmt/format.h"
#include "logger.h"

static bool indented = false;
static std::function<void(std::shared_ptr<const AnyLogMessage>)> loggerImpl =
    Logger::textLogger;

Logger& Logger::operator<<(std::shared_ptr<const AnyLogMessage> message)
{
    loggerImpl(message);
    return *this;
}

void Logger::setLogger(
    std::function<void(std::shared_ptr<const AnyLogMessage>)> cb)
{
    loggerImpl = cb;
}

void Logger::textLogger(std::shared_ptr<const AnyLogMessage> message)
{
    std::cout << toString(*message) << std::flush;
}

std::string Logger::toString(const AnyLogMessage& message)
{
    std::stringstream stream;

    auto indent = [&]()
    {
        if (!indented)
            stream << "      ";
        indented = false;
    };

    std::visit(
        overloaded{
            /* Fallback --- do nothing */
            [&](const auto& m)
            {
            },

            /* Start measuring the rotational speed */
            [&](const BeginSpeedOperationLogMessage& m)
            {
                stream << "Measuring rotational speed... ";
            },

            /* Finish measuring the rotational speed */
            [&](const EndSpeedOperationLogMessage& m)
            {
                stream << fmt::format("{:.1f}ms ({:.1f}rpm)\n",
                    m.rotationalPeriod / 1e6,
                    60e9 / m.rotationalPeriod);
            },

            /* Indicates that we're starting a write operation. */
            [&](const BeginWriteOperationLogMessage& m)
            {
                stream << fmt::format("{:2}.{}: ", m.track, m.head);
                indented = true;
            },

            /* Indicates that we're starting a read operation. */
            [&](const BeginReadOperationLogMessage& m)
            {
                stream << fmt::format("{:2}.{}: ", m.track, m.head);
                indented = true;
            },

            /* We've just read a track (we might reread it if there are errors)
             */
            [&](const TrackReadLogMessage& m)
            {
                const auto& track = *m.track;
                const auto& trackdataflux = track.trackDatas.end()[-1];

                indent();
                stream << fmt::format("{} raw records, {} raw sectors",
                    trackdataflux->records.size(),
                    trackdataflux->sectors.size());
                if (trackdataflux->sectors.size() > 0)
                {
                    nanoseconds_t clock =
                        (*trackdataflux->sectors.begin())->clock;
                    stream << fmt::format("; {:.2f}us clock ({:.0f}kHz)",
                        clock / 1000.0,
                        1000000.0 / clock);
                }

                stream << '\n';

                indent();
                stream << "sectors:";

                std::vector<std::shared_ptr<const Sector>> sectors(
                    track.sectors.begin(), track.sectors.end());
                std::sort(
                    sectors.begin(), sectors.end(), sectorPointerSortPredicate);

                for (const auto& sector : sectors)
                    stream << fmt::format(" {}{}",
                        sector->logicalSector,
                        Sector::statusToChar(sector->status));

                stream << '\n';

                int size = 0;
                std::set<std::pair<int, int>> track_ids;
                for (const auto& sector : m.track->sectors)
                {
                    track_ids.insert(std::make_pair(
                        sector->logicalTrack, sector->logicalSide));
                    size += sector->data.size();
                }

                if (!track_ids.empty())
                {
                    std::vector<std::string> ids;

                    for (const auto& i : track_ids)
                        ids.push_back(fmt::format("{}.{}", i.first, i.second));

                    indent();
                    stream << fmt::format(
                        "logical track {}\n", fmt::join(ids, "; "));
                }

                indent();
                stream << fmt::format("{} bytes decoded\n", size);
            },

            /* Generic text message */
            [&](const std::string& s)
            {
                indent();
                stream << s << '\n';
            },
        },
        message);
    return stream.str();
}