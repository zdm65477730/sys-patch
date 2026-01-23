#define TESLA_INIT_IMPL // If you have more than one file using the tesla header, only define this in the main one
#define STBTT_STATIC
#include <tesla.hpp>    // The Tesla Header
#include <string_view>
#include "minIni/minIni.h"

namespace {

constexpr auto CONFIG_PATH = "/config/sys-patch/config.ini";
constexpr auto LOG_PATH = "/config/sys-patch/log.ini";

// Color constants for logging
constexpr tsl::Color COLOR_SYSPATCH{0, 255, 200, 255};
constexpr tsl::Color COLOR_FILE{255, 177, 66, 255};
constexpr tsl::Color COLOR_UNPATCHED{250, 90, 58, 255};

// Patch configuration metadata
struct PatchConfig {
    const char* section;
    const char* title_id_str;
    struct {
        const char* name;
        const char* key;
    } patches[10];
    u8 patch_count;
};

constexpr PatchConfig PATCH_CONFIGS[] = {
    {
        "fs", "0100000000000000",
        {
            {"noacidsigchk_1.0.0-9.2.0", "noacidsigchk_1.0.0-9.2.0"},
            {"noacidsigchk_1.0.0-9.2.0", "noacidsigchk_1.0.0-9.2.0"},
            {"noncasigchk_1.0.0-3.0.2", "noncasigchk_1.0.0-3.0.2"},
            {"noncasigchk_4.0.0-16.1.0", "noncasigchk_4.0.0-16.1.0"},
            {"noncasigchk_17.0.0+", "noncasigchk_17.0.0+"},
            {"nocntchk_1.0.0-18.1.0", "nocntchk_1.0.0-18.1.0"},
            {"nocntchk_19.0.0+", "nocntchk_19.0.0+"},
        },
        7
    },
    {
        "ldr", "0100000000000001",
        {
            {"noacidsigchk_10.0.0+", "noacidsigchk_10.0.0+"},
        },
        1
    },
    {
        "erpt", "010000000000002B",
        {
            {"no_erpt", "no_erpt"},
        },
        1
    },
    {
        "es", "0100000000000033",
        {
            {"es_1.0.0-8.1.1", "es_1.0.0-8.1.1"},
            {"es_9.0.0-11.0.1", "es_9.0.0-11.0.1"},
            {"es_12.0.0-18.1.0", "es_12.0.0-18.1.0"},
            {"es_19.0.0+", "es_19.0.0+"},
        },
        4
    },
    {
        "olsc", "010000000000003E",
        {
            {"olsc_6.0.0-14.1.2", "olsc_6.0.0-14.1.2"},
            {"olsc_15.0.0-18.1.0", "olsc_15.0.0-18.1.0"},
            {"olsc_19.0.0+", "olsc_19.0.0+"},
        },
        3
    },
    {
        "nifm", "010000000000000F",
        {
            {"ctest_1.0.0-19.0.1", "ctest_1.0.0-19.0.1"},
            {"ctest_20.0.0+", "ctest_20.0.0+"},
        },
        2
    },
    {
        "nim", "0100000000000025",
        {
            {"blankcal0crashfix_17.0.0+", "blankcal0crashfix_17.0.0+"},
            {"blockfirmwareupdates_1.0.0-5.1.0", "blockfirmwareupdates_1.0.0-5.1.0"},
            {"blockfirmwareupdates_6.0.0-6.2.0", "blockfirmwareupdates_6.0.0-6.2.0"},
            {"blockfirmwareupdates_7.0.0-10.2.0", "blockfirmwareupdates_7.0.0-10.2.0"},
            {"blockfirmwareupdates_11.0.0-11.0.1", "blockfirmwareupdates_11.0.0-11.0.1"},
            {"blockfirmwareupdates_12.0.0+", "blockfirmwareupdates_12.0.0+"},
        },
        6
    },
};

// Common file system helper
auto fs_operation(const char* path, Result (*operation)(FsFileSystem&, const char*)) -> Result {
    Result rc{};
    FsFileSystem fs{};
    char path_buf[FS_MAX_PATH]{};

    if (R_FAILED(fsOpenSdCardFileSystem(&fs))) {
        return rc;
    }

    strcpy(path_buf, path);
    rc = operation(fs, path_buf);
    fsFsClose(&fs);
    return rc;
}

auto does_file_exist(const char* path) -> bool {
    return R_SUCCEEDED(fs_operation(path, [](FsFileSystem& fs, const char* p) {
        FsFile file{};
        auto rc = fsFsOpenFile(&fs, p, FsOpenMode_Read, &file);
        fsFileClose(&file);
        return rc;
    }));
}

// creates a directory, non-recursive!
auto create_dir(const char* path) -> bool {
    return R_SUCCEEDED(fs_operation(path, [](FsFileSystem& fs, const char* p) {
        return fsFsCreateDirectory(&fs, p);
    }));
}

class GuiOptions final : public tsl::Gui {
public:
    GuiOptions() { }

    tsl::elm::Element* createUI() override {
        auto frame = new tsl::elm::OverlayFrame("sys-patch", VERSION_WITH_HASH);
        auto list = new tsl::elm::List();

        list->addItem(new tsl::elm::CategoryHeader("Options"));
        
        auto add_option = [list](const char* section, const char* key, bool default_value, const char* label) {
            const auto value = ini_getbool(section, key, default_value, CONFIG_PATH);
            auto item = new tsl::elm::ToggleListItem(label, value);
            item->setStateChangedListener([section, key](bool new_value) {
                ini_putl(section, key, new_value, CONFIG_PATH);
            });
            list->addItem(item);
        };

        add_option("options", "patch_sysmmc", true, "Patch sysMMC");
        add_option("options", "patch_emummc", true, "Patch emuMMC");
        add_option("options", "enable_logging", true, "Logging");
        add_option("options", "version_skip", true, "Version skip");

        frame->setContent(list);
        return frame;
    }
};

class GuiToggle final : public tsl::Gui {
public:
    GuiToggle() { }

    tsl::elm::Element* createUI() override {
        auto frame = new tsl::elm::OverlayFrame("sys-patch", VERSION_WITH_HASH);
        auto list = new tsl::elm::List();

        for (const auto& config : PATCH_CONFIGS) {
            list->addItem(new tsl::elm::CategoryHeader(
                std::string(config.section) + " - " + config.title_id_str
            ));

            for (u8 i = 0; i < config.patch_count; ++i) {
                const auto& patch = config.patches[i];
                auto item = new tsl::elm::ToggleListItem(
                    patch.name,
                    ini_getbool(config.section, patch.key, true, CONFIG_PATH)
                );
                item->setStateChangedListener([section = config.section, key = patch.key](bool new_value) {
                    ini_putl(section, key, new_value, CONFIG_PATH);
                });
                list->addItem(item);
            }
        }

        frame->setContent(list);
        return frame;
    }
};

class GuiLog final : public tsl::Gui {
public:
    GuiLog() { }

    tsl::elm::Element* createUI() override {
        auto frame = new tsl::elm::OverlayFrame("sys-patch", VERSION_WITH_HASH);
        auto list = new tsl::elm::List();

        if (does_file_exist(LOG_PATH)) {
            struct CallbackData {
                tsl::elm::List* list;
                std::string last_section;
            } callback_data{list};

            ini_browse([](const mTCHAR *Section, const mTCHAR *Key, const mTCHAR *Value, void *UserData) {
                auto data = (CallbackData*)UserData;
                std::string_view value{Value};

                if (value == "Skipped") {
                    return 1;
                }

                if (data->last_section != Section) {
                    data->last_section = Section;
                    data->list->addItem(new tsl::elm::CategoryHeader("Log: " + data->last_section));
                }

                if (value.starts_with("Patched")) {
                    const auto color = value.ends_with("(sys-patch)") ? COLOR_SYSPATCH : COLOR_FILE;
                    data->list->addItem(new tsl::elm::ListItem(Key, "Patched", color));
                } else if (value.starts_with("Unpatched") || value.starts_with("Disabled")) {
                    data->list->addItem(new tsl::elm::ListItem(Key, Value, COLOR_UNPATCHED));
                } else if (data->last_section == "stats") {
                    data->list->addItem(new tsl::elm::ListItem(Key, Value, tsl::style::color::ColorDescription));
                } else {
                    data->list->addItem(new tsl::elm::ListItem(Key, Value, tsl::style::color::ColorText));
                }

                return 1;
            }, &callback_data, LOG_PATH);
        } else {
            list->addItem(new tsl::elm::ListItem("No log found!"));
        }

        frame->setContent(list);
        return frame;
    }
};

class GuiMain final : public tsl::Gui {
public:
    GuiMain() { }

    tsl::elm::Element* createUI() override {
        auto frame = new tsl::elm::OverlayFrame("sys-patch", VERSION_WITH_HASH);
        auto list = new tsl::elm::List();

        auto options = new tsl::elm::ListItem("Options");
        auto toggle = new tsl::elm::ListItem("Toggle patches");
        auto log = new tsl::elm::ListItem("Log");

        options->setClickListener([](u64 keys) -> bool {
            if (keys & HidNpadButton_A) {
                tsl::changeTo<GuiOptions>();
                return true;
            }
            return false;
        });

        toggle->setClickListener([](u64 keys) -> bool {
            if (keys & HidNpadButton_A) {
                tsl::changeTo<GuiToggle>();
                return true;
            }
            return false;
        });

        log->setClickListener([](u64 keys) -> bool {
            if (keys & HidNpadButton_A) {
                tsl::changeTo<GuiLog>();
                return true;
            }
            return false;
        });

        list->addItem(new tsl::elm::CategoryHeader("Menu"));
        list->addItem(options);
        list->addItem(toggle);
        list->addItem(log);

        frame->setContent(list);
        return frame;
    }
};

// libtesla already initialized fs, hid, pl, pmdmnt, hid:sys and set:sys
class SysPatchOverlay final : public tsl::Overlay {
public:
    std::unique_ptr<tsl::Gui> loadInitialGui() override {
        return initially<GuiMain>();
    }
};

} // namespace

int main(int argc, char **argv) {
    create_dir("/config/");
    create_dir("/config/sys-patch/");
    return tsl::loop<SysPatchOverlay>(argc, argv);
}
