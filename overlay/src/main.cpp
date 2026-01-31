#define TESLA_INIT_IMPL // If you have more than one file using the tesla header, only define this in the main one
#define STBTT_STATIC
#include <tesla.hpp>    // The Tesla Header
#include <string_view>
#include "minIni/minIni.h"

using namespace tsl;

namespace {

constexpr auto CONFIG_PATH = "/config/sys-patch/config.ini";
constexpr auto LOG_PATH = "/config/sys-patch/log.ini";

std::string map_section_to_text(std::string &section) {
    std::string sectionDisplay{section};
    if (sectionDisplay == "fs") {
        sectionDisplay = "FsGuiLogListItemText"_tr;
    } else if (sectionDisplay == "ldr") {
        sectionDisplay = "LdrGuiLogListItemText"_tr;
    } else if (sectionDisplay == "erpt") {
        sectionDisplay = "ErptGuiLogListItemText"_tr;
    } else if (sectionDisplay == "es") {
        sectionDisplay = "EsGuiLogListItemText"_tr;
    } else if (sectionDisplay == "olsc") {
        sectionDisplay = "OlscGuiLogListItemText"_tr;
    } else if (sectionDisplay == "nifm") {
        sectionDisplay = "NifmGuiLogListItemText"_tr;
    } else if (sectionDisplay == "nim") {
        sectionDisplay = "NimGuiLogListItemText"_tr;
    } else if (sectionDisplay == "stats") {
        sectionDisplay = "StatsGuiLogListItemText"_tr;
    }
    return sectionDisplay;
}

std::string map_stats_to_text(const char *stat) {
    std::string statDisplay{stat};
    if (statDisplay == "version") {
        statDisplay = "VersionGuiLogListItemText"_tr;
    } else if (statDisplay == "build_date") {
        statDisplay = "BuildDateGuiLogListItemText"_tr;
    } else if (statDisplay == "fw_version") {
        statDisplay = "FWVersionGuiLogListItemText"_tr;
    } else if (statDisplay == "ams_version") {
        statDisplay = "AMSVersionGuiLogListItemText"_tr;
    } else if (statDisplay == "ams_target_version") {
        statDisplay = "AMSTargetVersionGuiLogListItemText"_tr;
    } else if (statDisplay == "ams_keygen") {
        statDisplay = "AMSKeygenGuiLogListItemText"_tr;
    } else if (statDisplay == "ams_hash") {
        statDisplay = "AMSHashGuiLogListItemText"_tr;
    } else if (statDisplay == "is_emummc") {
        statDisplay = "IsEmummcGuiLogListItemText"_tr;
    } else if (statDisplay == "heap_size") {
        statDisplay = "HeapSizeGuiLogListItemText"_tr;
    } else if (statDisplay == "buffer_size") {
        statDisplay = "BufferSizeGuiLogListItemText"_tr;
    } else if (statDisplay == "patch_time") {
        statDisplay = "Patch_timeGuiLogListItemText"_tr;
    }
    return statDisplay;
}

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

PatchConfig PATCH_CONFIGS[] = {
    {
        "FSGuiToggleToggleCategoryHeaderText"_tr.c_str(), "0100000000000000",
        {
            {"FSNoAcidSigChkFat32V1GuiToggleToggleListItemText"_tr.c_str(), "noacidsigchk_1.0.0-9.2.0"},
            {"FSNoAcidSigChkExFatV1GuiToggleToggleListItemText"_tr.c_str(), "noacidsigchk_1.0.0-9.2.0"},
            {"FSNoNCASigChkV1GuiToggleToggleListItemText"_tr.c_str(), "noncasigchk_1.0.0-3.0.2"},
            {"FSNoNCASigChkV2GuiToggleToggleListItemText"_tr.c_str(), "noncasigchk_4.0.0-16.1.0"},
            {"FSNoNCASigChkV3GuiToggleToggleListItemText"_tr.c_str(), "noncasigchk_17.0.0+"},
            {"FSNoCNTChkV1GuiToggleToggleListItemText"_tr.c_str(), "nocntchk_1.0.0-18.1.0"},
            {"FSNoCNTChkV2GuiToggleToggleListItemText"_tr.c_str(), "nocntchk_19.0.0+"},
        },
        7
    },
    {
        "LDRGuiToggleCategoryHeaderText"_tr.c_str(), "0100000000000001",
        {
            {"LDRNoAcidSigChkV1GuiToggleToggleListItemText"_tr.c_str(), "noacidsigchk_10.0.0+"},
        },
        1
    },
    {
        "ERPTGuiToggleCategoryHeaderText"_tr.c_str(), "010000000000002B",
        {
            {"ERPTNoErrorReportGuiToggleToggleListItemText"_tr.c_str(), "no_erpt"},
        },
        1
    },
    {
        "ESGuiToggleCategoryHeaderText"_tr.c_str(), "0100000000000033",
        {
            {"ESeTicketChkV1GuiToggleToggleListItemText"_tr.c_str(), "es_1.0.0-8.1.1"},
            {"ESeTicketChkV2GuiToggleToggleListItemText"_tr.c_str(), "es_9.0.0-11.0.1"},
            {"ESeTicketChkV3GuiToggleToggleListItemText"_tr.c_str(), "es_12.0.0-18.1.0"},
            {"ESeTicketChkV4GuiToggleToggleListItemText"_tr.c_str(), "es_19.0.0+"},
        },
        4
    },
    {
        "OLSCGuiToggleCategoryHeaderText"_tr.c_str(), "010000000000003E",
        {
            {"OnlineSaveStorageChkV1GuiToggleToggleListItemText"_tr.c_str(), "olsc_6.0.0-14.1.2"},
            {"OnlineSaveStorageChkV2GuiToggleToggleListItemText"_tr.c_str(), "olsc_15.0.0-18.1.0"},
            {"OnlineSaveStorageChkV3GuiToggleToggleListItemText"_tr.c_str(), "olsc_19.0.0+"},
        },
        3
    },
    {
        "NIFMGuiToggleCategoryHeaderText"_tr.c_str(), "010000000000000F",
        {
            {"NIFMCtestV1GuiToggleToggleListItemText"_tr.c_str(), "ctest_1.0.0-19.0.1"},
            {"NIFMCtestV2GuiToggleToggleListItemText"_tr.c_str(), "ctest_20.0.0+"},
        },
        2
    },
    {
        "NIMGuiToggleCategoryHeaderText"_tr.c_str(), "0100000000000025",
        {
            {"NIMFixBlankCal0CrashGuiToggleToggleListItemText"_tr.c_str(), "blankcal0crashfix_17.0.0+"},
            {"NIMBlockFWUpgradeV1GuiToggleToggleListItemText"_tr.c_str(), "blockfirmwareupdates_1.0.0-5.1.0"},
            {"NIMBlockFWUpgradeV2GuiToggleToggleListItemText"_tr.c_str(), "blockfirmwareupdates_6.0.0-6.2.0"},
            {"NIMBlockFWUpgradeV3GuiToggleToggleListItemText"_tr.c_str(), "blockfirmwareupdates_7.0.0-10.2.0"},
            {"NIMBlockFWUpgradeV4GuiToggleToggleListItemText"_tr.c_str(), "blockfirmwareupdates_11.0.0-11.0.1"},
            {"NIMBlockFWUpgradeV5GuiToggleToggleListItemText"_tr.c_str(), "blockfirmwareupdates_12.0.0+"},
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
        auto frame = new tsl::elm::OverlayFrame("PluginName"_tr, VERSION_WITH_HASH);
        auto list = new tsl::elm::List();

        list->addItem(new tsl::elm::CategoryHeader("OptionsGuiOptionsCategoryHeaderText"_tr));

        auto add_option = [list](const char* section, const char* key, bool default_value, const char* label) {
            const auto value = ini_getbool(section, key, default_value, CONFIG_PATH);
            auto item = new tsl::elm::ToggleListItem(label, value);
            item->setStateChangedListener([section, key](bool new_value) {
                ini_putl(section, key, new_value, CONFIG_PATH);
            });
            list->addItem(item);
        };

        add_option("options", "patch_sysmmc", true, "PatchSysMMCGuiOptionsToggleListItemText"_tr.c_str());
        add_option("options", "patch_emummc", true, "PatchEmuMMCGuiOptionsToggleListItemText"_tr.c_str());
        add_option("options", "enable_logging", true, "LoggingGuiOptionsToggleListItemText"_tr.c_str());
        add_option("options", "version_skip", true, "VersionSkipGuiOptionsToggleListItemText"_tr.c_str());

        frame->setContent(list);
        return frame;
    }
};

class GuiToggle final : public tsl::Gui {
public:
    GuiToggle() { }
    tsl::elm::Element* createUI() override {
        auto frame = new tsl::elm::OverlayFrame("PluginName"_tr, VERSION_WITH_HASH);
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
        auto frame = new tsl::elm::OverlayFrame("PluginName"_tr, VERSION_WITH_HASH);
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
                    data->list->addItem(new tsl::elm::CategoryHeader("LogGuiLogCategoryHeaderText"_tr + map_section_to_text(data->last_section)));
                }

                if (value.starts_with("Patched")) {
                    const auto color = value.ends_with("(sys-patch)") ? COLOR_SYSPATCH : COLOR_FILE;
                    data->list->addItem(new tsl::elm::ListItem(Key, "PatchedGuiLogListItemText"_tr, color));
                } else if (value.starts_with("Unpatched")) {
                    data->list->addItem(new tsl::elm::ListItem(Key, "UnPatchedGuiLogListItemText"_tr, COLOR_UNPATCHED));
                } else if (value.starts_with("Disabled")) {
                    data->list->addItem(new tsl::elm::ListItem(Key, "DisabledGuiLogListItemText"_tr, COLOR_UNPATCHED));
                } else if (data->last_section == "stats") {
                    data->list->addItem(new tsl::elm::ListItem(map_stats_to_text(Key), Value, tsl::style::color::ColorDescription));
                } else {
                    data->list->addItem(new tsl::elm::ListItem(Key, Value, tsl::style::color::ColorText));
                }

                return 1;
            }, &callback_data, LOG_PATH);
        } else {
            list->addItem(new tsl::elm::ListItem("NoLogFoundGuiLogListItemText"_tr.c_str()));
        }

        frame->setContent(list);
        return frame;
    }
};

class GuiMain final : public tsl::Gui {
public:
    GuiMain() {
        std::string jsonStr = R"(
            {
                "PluginName": "sys-patch",
                "OptionsGuiOptionsCategoryHeaderText": "Options",
                "PatchSysMMCGuiOptionsToggleListItemText": "Patch sysMMC",
                "PatchemuMMCGuiOptionsToggleListItemText": "Patch emuMMC",
                "LoggingGuiOptionsToggleListItemText": "Logging",
                "VersionSkipGuiOptionsToggleListItemText": "Version skip",
                "FSGuiToggleToggleCategoryHeaderText": "File System",
                "FSNoAcidSigChkFat32V1GuiToggleToggleListItemText": "Disable FAT32 ACID Signature Check [1.0.0-9.2.0]",
                "FSNoAcidSigChkExFatV1GuiToggleToggleListItemText": "Disable exFAT ACID Signature Check2 [1.0.0-9.2.0]",
                "FSNoNCASigChkV1GuiToggleToggleListItemText": "Disable NCA File Signature Check [1.0.0-3.0.2]",
                "FSNoNCASigChkV2GuiToggleToggleListItemText": "Disable NCA File Signature Check [4.0.0-16.1.0]",
                "FSNoNCASigChkV3GuiToggleToggleListItemText": "Disable NCA File Signature Check [17.0.0+]",
                "FSNoCNTChkV1GuiToggleToggleListItemText": "Disable Content Counter Check [1.0.0-18.1.0]",
                "FSNoCNTChkV2GuiToggleToggleListItemText": "Disable Content Counter Check [19.0.0+]",
                "LDRGuiToggleCategoryHeaderText": "Module Loader",
                "LDRNoAcidSigChkV1GuiToggleToggleListItemText": "Disable ACID Signature Check [10.0.0+]",
                "ERPTGuiToggleCategoryHeaderText": "Error Report",
                "ERPTNoErrorReportGuiToggleToggleListItemText": "Disable Error Report Upload",
                "ESGuiToggleCategoryHeaderText": "eTicket Service",
                "ESeTicketChkV1GuiToggleToggleListItemText": "Disable eTicket Validation [1.0.0-8.1.1]",
                "ESeTicketChkV2GuiToggleToggleListItemText": "Disable eTicket Validation [9.0.0-11.0.1]",
                "ESeTicketChkV3GuiToggleToggleListItemText": "Disable eTicket Validation [12.0.0-18.1.0]",
                "ESeTicketChkV4GuiToggleToggleListItemText": "Disable eTicket Validation [19.0.0+]",
                "OLSCGuiToggleCategoryHeaderText": "Online Save Storage",
                "OnlineSaveStorageChkV1GuiToggleToggleListItemText": "Disable Online Save Storage Validation [6.0.0-14.1.2]",
                "OnlineSaveStorageChkV2GuiToggleToggleListItemText": "Disable Online Save Storage Validation [15.0.0-18.1.0]",
                "OnlineSaveStorageChkV3GuiToggleToggleListItemText": "Disable Online Save Storage Validation [19.0.0+]",
                "NIFMGuiToggleCategoryHeaderText": "Network Connection",
                "NIFMCtestV1GuiToggleToggleListItemText": "Disable Network Connection Test [1.0.0-19.0.1]",
                "NIFMCtestV2GuiToggleToggleListItemText": "Disable Network Connection Test [20.0.0+]",
                "NIMGuiToggleCategoryHeaderText": "Network Identity",
                "NIMFixBlankCal0CrashGuiToggleToggleListItemText": "Fix Blank CAL0 Crash [17.0.0+]",
                "NIMBlockFWUpgradeV1GuiToggleToggleListItemText": "Block System Firmware Update [1.0.0-5.1.0]",
                "NIMBlockFWUpgradeV2GuiToggleToggleListItemText": "Block System Firmware Update [6.0.0-6.2.0]",
                "NIMBlockFWUpgradeV3GuiToggleToggleListItemText": "Block System Firmware Update [7.0.0-10.2.0]",
                "NIMBlockFWUpgradeV4GuiToggleToggleListItemText": "Block System Firmware Update [11.0.0-11.0.1]",
                "NIMBlockFWUpgradeV5GuiToggleToggleListItemText": "Block System Firmware Update [12.0.0+]",
                "LogGuiLogCategoryHeaderText": "Logs:",
                "PatchedGuiLogListItemText": "Patched",
                "UnPatchedGuiLogListItemText": "Unpatched",
                "DisabledGuiLogListItemText": "Disabled",
                "FsGuiLogListItemText": "File Signature",
                "LdrGuiLogListItemText": "Module Loader",
                "ErptGuiLogListItemText": "Error Report",
                "EsGuiLogListItemText": "eTicket Service",
                "OlscGuiLogListItemText": "Online Save Storage",
                "NifmGuiLogListItemText": "Network Connection",
                "NimGuiLogListItemText": "Network Identity",
                "StatsGuiLogListItemText": "Statistics",
                "VersionGuiLogListItemText": "Version",
                "BuildDateGuiLogListItemText": "Build Date",
                "FWVersionGuiLogListItemText": "System Firmware Version",
                "AMSVersionGuiLogListItemText": "Atmosphere Version",
                "AMSTargetVersionGuiLogListItemText": "Atmosphere Target Version",
                "AMSKeygenGuiLogListItemText": "Atmosphere Keygen Version",
                "AMSHashGuiLogListItemText": "Atmosphere Hash Value",
                "IsEmummcGuiLogListItemText": "Is EmuMMC",
                "HeapSizeGuiLogListItemText": "Heap Size",
                "BufferSizeGuiLogListItemText": "Buffer Size",
                "Patch_timeGuiLogListItemText": "Patch Application Time",
                "NoLogFoundGuiLogListItemText": "No Logs Found!",
                "OptionsGuiMainListItemText": "Options",
                "ToggleGuiMainListItemText": "Toggle Patches",
                "LogGuiMainListItemText": "Logs",
                "MenuGuiMainCategoryHeaderText": "Menu"
            }
        )";
        std::string lanPath = std::string("sdmc:/switch/.overlays/lang/") + APPTITLE + "/";
        fsdevMountSdmc();
        tsl::hlp::doWithSmSession([&lanPath, &jsonStr]{
            tsl::tr::InitTrans(lanPath, jsonStr);
        });
        fsdevUnmountDevice("sdmc");
    }

    tsl::elm::Element* createUI() override {
        auto frame = new tsl::elm::OverlayFrame("PluginName"_tr, VERSION_WITH_HASH);
        auto list = new tsl::elm::List();

        auto options = new tsl::elm::ListItem("OptionsGuiMainListItemText"_tr.c_str());
        auto toggle = new tsl::elm::ListItem("ToggleGuiMainListItemText"_tr.c_str());
        auto log = new tsl::elm::ListItem("LogGuiMainListItemText"_tr.c_str());

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

        list->addItem(new tsl::elm::CategoryHeader("MenuGuiMainCategoryHeaderText"_tr));
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