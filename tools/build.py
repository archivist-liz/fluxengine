from build.c import cxxprogram
import config

emu = []
if config.windows:
    emu = ["dep/emu"]

cxxprogram(
    name="brother120tool",
    srcs=["./brother120tool.cc"],
    deps=["+lib", "lib+config_proto_lib"] + emu,
)

cxxprogram(
    name="brother240tool",
    srcs=["./brother240tool.cc"],
    deps=["+lib", "lib+config_proto_lib"] + emu,
)

cxxprogram(
    name="upgrade-flux-file",
    srcs=["./upgrade-flux-file.cc"],
    deps=[
        "+lib",
        "src/formats",
        "lib+config_proto_lib",
        "+protocol",
        "+fl2_proto_lib",
        "+sqlite3_lib",
        "dep/libusbp",
    ],
)
