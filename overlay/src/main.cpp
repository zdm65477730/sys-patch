#define TESLA_INIT_IMPL // If you have more than one file using the tesla header, only define this in the main one
#define STBTT_STATIC
#include <tesla.hpp>    // The Tesla Header
#include <string_view>
#include "minIni/minIni.h"

using namespace tsl;

namespace {

constexpr auto CONFIG_PATH = "/config/sys-patch/config.ini";
constexpr auto LOG_PATH = "/config/sys-patch/log.ini";

auto does_file_exist(const char* path) -> bool {
    Result rc{};
    FsFileSystem fs{};
    FsFile file{};
    char path_buf[FS_MAX_PATH]{};

    if (R_FAILED(fsOpenSdCardFileSystem(&fs))) {
        return false;
    }

    strcpy(path_buf, path);
    rc = fsFsOpenFile(&fs, path_buf, FsOpenMode_Read, &file);
    fsFileClose(&file);
    fsFsClose(&fs);
    return R_SUCCEEDED(rc);
}

// creates a directory, non-recursive!
auto create_dir(const char* path) -> bool {
    Result rc{};
    FsFileSystem fs{};
    char path_buf[FS_MAX_PATH]{};

    if (R_FAILED(fsOpenSdCardFileSystem(&fs))) {
        return false;
    }

    strcpy(path_buf, path);
    rc = fsFsCreateDirectory(&fs, path_buf);
    fsFsClose(&fs);
    return R_SUCCEEDED(rc);
}

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

struct ConfigEntry {
    ConfigEntry(const char* _section, const char* _key, bool default_value) :
        section{_section}, key{_key}, value{default_value} {
            this->load_value_from_ini();
        }

    void load_value_from_ini() {
        this->value = ini_getbool(this->section, this->key, this->value, CONFIG_PATH);
    }

    auto create_list_item(const char* text) {
        auto item = new tsl::elm::ToggleListItem(text, value);
        item->setStateChangedListener([this](bool new_value){
            this->value = new_value;
            ini_putl(this->section, this->key, this->value, CONFIG_PATH);
        });
        return item;
    }

    const char* const section;
    const char* const key;
    bool value;
};

class GuiOptions final : public tsl::Gui {
public:
    GuiOptions() { }

    tsl::elm::Element* createUI() override {
        auto frame = new tsl::elm::OverlayFrame("PluginName"_tr, VERSION_WITH_HASH);
        auto list = new tsl::elm::List();

        list->addItem(new tsl::elm::CategoryHeader("OptionsGuiOptionsCategoryHeaderText"_tr));
        list->addItem(config_patch_sysmmc.create_list_item("PatchSysMMCGuiOptionsToggleListItemText"_tr.c_str()));
        list->addItem(config_patch_emummc.create_list_item("PatchemuMMCGuiOptionsToggleListItemText"_tr.c_str()));
        list->addItem(config_logging.create_list_item("LoggingGuiOptionsToggleListItemText"_tr.c_str()));
        list->addItem(config_version_skip.create_list_item("VersionSkipGuiOptionsToggleListItemText"_tr.c_str()));

        frame->setContent(list);
        return frame;
    }

    ConfigEntry config_patch_sysmmc{"options", "patch_sysmmc", true};
    ConfigEntry config_patch_emummc{"options", "patch_emummc", true};
    ConfigEntry config_logging{"options", "enable_logging", true};
    ConfigEntry config_version_skip{"options", "version_skip", true};
};

class GuiToggle final : public tsl::Gui {
public:
    GuiToggle() { }

    tsl::elm::Element* createUI() override {
        auto frame = new tsl::elm::OverlayFrame("PluginName"_tr, VERSION_WITH_HASH);
        auto list = new tsl::elm::List();

        list->addItem(new tsl::elm::CategoryHeader("FSGuiToggleToggleCategoryHeaderText"_tr));
        list->addItem(config_noacidsigchk1.create_list_item("FSNoAcidSigChk1V1GuiToggleToggleListItemText"_tr.c_str()));
        list->addItem(config_noacidsigchk2.create_list_item("FSNoAcidSigChk2V1GuiToggleToggleListItemText"_tr.c_str()));
        list->addItem(config_noncasigchk1.create_list_item("FSNoNCASigChkV1GuiToggleToggleListItemText"_tr.c_str()));
        list->addItem(config_noncasigchk2.create_list_item("FSNoNCASigChkV2GuiToggleToggleListItemText"_tr.c_str()));
        list->addItem(config_noncasigchk3.create_list_item("FSNoNCASigChkV3GuiToggleToggleListItemText"_tr.c_str()));
        list->addItem(config_nocntchk1.create_list_item("FSNoCNTChkV1GuiToggleToggleListItemText"_tr.c_str()));
        list->addItem(config_nocntchk2.create_list_item("FSNoCNTChkV2GuiToggleToggleListItemText"_tr.c_str()));

        list->addItem(new tsl::elm::CategoryHeader("LDRGuiToggleCategoryHeaderText"_tr));
        list->addItem(config_noacidsigchk3.create_list_item("LDRNoAcidSigChkV3GuiToggleToggleListItemText"_tr.c_str()));

        list->addItem(new tsl::elm::CategoryHeader("ERPTGuiToggleCategoryHeaderText"_tr));
        list->addItem(config_no_erpt.create_list_item("ERPTNoErrorReportGuiToggleToggleListItemText"_tr.c_str()));

        list->addItem(new tsl::elm::CategoryHeader("ESGuiToggleCategoryHeaderText"_tr));
        list->addItem(config_es1.create_list_item("ESeTicketChkV1GuiToggleToggleListItemText"_tr.c_str()));
        list->addItem(config_es2.create_list_item("ESeTicketChkV2GuiToggleToggleListItemText"_tr.c_str()));
        list->addItem(config_es3.create_list_item("ESeTicketChkV3GuiToggleToggleListItemText"_tr.c_str()));
        list->addItem(config_es4.create_list_item("ESeTicketChkV4GuiToggleToggleListItemText"_tr.c_str()));

        list->addItem(new tsl::elm::CategoryHeader("OLSCGuiToggleCategoryHeaderText"_tr));
        list->addItem(config_olsc1.create_list_item("OnlineSaveStorageChkV1GuiToggleToggleListItemText"_tr.c_str()));
        list->addItem(config_olsc2.create_list_item("OnlineSaveStorageChkV2GuiToggleToggleListItemText"_tr.c_str()));
        list->addItem(config_olsc3.create_list_item("OnlineSaveStorageChkV3GuiToggleToggleListItemText"_tr.c_str()));

        list->addItem(new tsl::elm::CategoryHeader("NIFMGuiToggleCategoryHeaderText"_tr));
        list->addItem(config_ctest1.create_list_item("NIFMCtestV1GuiToggleToggleListItemText"_tr.c_str()));
        list->addItem(config_ctest2.create_list_item("NIFMCtestV2GuiToggleToggleListItemText"_tr.c_str()));

        list->addItem(new tsl::elm::CategoryHeader("NIMGuiToggleCategoryHeaderText"_tr));
        list->addItem(config_nim1.create_list_item("NIMFixBlankCal0CrashGuiToggleToggleListItemText"_tr.c_str()));
        list->addItem(config_nim_fw1.create_list_item("NIMBlockFWUpgradeV1GuiToggleToggleListItemText"_tr.c_str()));
        list->addItem(config_nim_fw2.create_list_item("NIMBlockFWUpgradeV2GuiToggleToggleListItemText"_tr.c_str()));
        list->addItem(config_nim_fw3.create_list_item("NIMBlockFWUpgradeV3GuiToggleToggleListItemText"_tr.c_str()));
        list->addItem(config_nim_fw4.create_list_item("NIMBlockFWUpgradeV4GuiToggleToggleListItemText"_tr.c_str()));
        list->addItem(config_nim_fw5.create_list_item("NIMBlockFWUpgradeV5GuiToggleToggleListItemText"_tr.c_str()));

        frame->setContent(list);
        return frame;
    }

    ConfigEntry config_noacidsigchk1{"fs", "noacidsigchk_1.0.0-9.2.0", true};
    ConfigEntry config_noacidsigchk2{"fs", "noacidsigchk_1.0.0-9.2.0", true};
    ConfigEntry config_noncasigchk1{"fs", "noncasigchk_1.0.0-3.0.2", true};
    ConfigEntry config_noncasigchk2{"fs", "noncasigchk_4.0.0-16.1.0", true};
    ConfigEntry config_noncasigchk3{"fs", "noncasigchk_17.0.0+", true};
    ConfigEntry config_nocntchk1{"fs", "nocntchk_1.0.0-18.1.0", true};
    ConfigEntry config_nocntchk2{"fs", "nocntchk_19.0.0+", true};
    ConfigEntry config_noacidsigchk3{"ldr", "noacidsigchk_10.0.0+", true};
    ConfigEntry config_no_erpt{"erpt", "no_erpt", true};
    ConfigEntry config_es1{"es", "es_1.0.0-8.1.1", true};
    ConfigEntry config_es2{"es", "es_9.0.0-11.0.1", true};
    ConfigEntry config_es3{"es", "es_12.0.0-18.1.0", true};
    ConfigEntry config_es4{"es", "es_19.0.0+", true};
    ConfigEntry config_olsc1{"olsc", "olsc_6.0.0-14.1.2", true};
    ConfigEntry config_olsc2{"olsc", "olsc_15.0.0-18.1.0", true};
    ConfigEntry config_olsc3{"olsc", "olsc_19.0.0+", true};
    ConfigEntry config_ctest1{"nifm", "ctest_1.0.0-19.0.1", true};
    ConfigEntry config_ctest2{"nifm", "ctest_20.0.0+", true};
    ConfigEntry config_nim1{"nim", "blankcal0crashfix_17.0.0+", true};
    ConfigEntry config_nim_fw1{"nim", "blockfirmwareupdates_1.0.0-5.1.0", true};
    ConfigEntry config_nim_fw2{"nim", "blockfirmwareupdates_6.0.0-6.2.0", true};
    ConfigEntry config_nim_fw3{"nim", "blockfirmwareupdates_7.0.0-10.2.0", true};
    ConfigEntry config_nim_fw4{"nim", "blockfirmwareupdates_11.0.0-11.0.1", true};
    ConfigEntry config_nim_fw5{"nim", "blockfirmwareupdates_12.0.0+", true};
};

class GuiLog final : public tsl::Gui {
public:
    GuiLog() { }

    tsl::elm::Element* createUI() override {
        auto frame = new tsl::elm::OverlayFrame("PluginName"_tr, VERSION_WITH_HASH);
        auto list = new tsl::elm::List();

        if (does_file_exist(LOG_PATH)) {
            struct CallbackUser {
                tsl::elm::List* list;
                std::string last_section;
            } callback_userdata{list};

            ini_browse([](const mTCHAR *Section, const mTCHAR *Key, const mTCHAR *Value, void *UserData){
                auto user = (CallbackUser*)UserData;
                std::string_view value{Value};

                if (value == "Skipped") {
                    return 1;
                }

                if (user->last_section != Section) {
                    user->last_section = Section;
                    user->list->addItem(new tsl::elm::CategoryHeader("LogGuiLogCategoryHeaderText"_tr + map_section_to_text(user->last_section)));
                }

                #define F(x) ((x) >> 4) // 8bit -> 4bit
                constexpr tsl::Color colour_syspatch{F(0), F(255), F(200), F(255)};
                constexpr tsl::Color colour_file{F(255), F(177), F(66), F(255)};
                constexpr tsl::Color colour_unpatched{F(250), F(90), F(58), F(255)};
                #undef F

                if (value.starts_with("Patched")) {
                    if (value.ends_with("(sys-patch)")) {
                        user->list->addItem(new tsl::elm::ListItem(Key, "PatchedGuiLogListItemText"_tr, colour_syspatch));
                    } else {
                        user->list->addItem(new tsl::elm::ListItem(Key, "PatchedGuiLogListItemText"_tr, colour_file));
                    }
                } else if (value.starts_with("Unpatched")) {
                    user->list->addItem(new tsl::elm::ListItem(Key, "UnPatchedGuiLogListItemText"_tr, colour_unpatched));
                } else if (value.starts_with("Disabled")) {
                    user->list->addItem(new tsl::elm::ListItem(Key, "DisabledGuiLogListItemText"_tr, colour_unpatched));
                } else if (user->last_section == "stats") {
                    user->list->addItem(new tsl::elm::ListItem(map_stats_to_text(Key), Value, tsl::style::color::ColorDescription));
                } else {
                    user->list->addItem(new tsl::elm::ListItem(Key, Value, tsl::style::color::ColorText));
                }

                return 1;
            }, &callback_userdata, LOG_PATH);
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
                "FSGuiToggleToggleCategoryHeaderText": "File System - 0100000000000000",
                "FSNoAcidSigChk1V1GuiToggleToggleListItemText": "Disable ACID Signature Check [1.0.0-9.2.0]",
                "FSNoAcidSigChk2V1GuiToggleToggleListItemText": "Disable ACID Signature Check2 [1.0.0-9.2.0]",
                "FSNoNCASigChkV1GuiToggleToggleListItemText": "Disable NCA File Signature Check [1.0.0-3.0.2]",
                "FSNoNCASigChkV2GuiToggleToggleListItemText": "Disable NCA File Signature Check [4.0.0-16.1.0]",
                "FSNoNCASigChkV3GuiToggleToggleListItemText": "Disable NCA File Signature Check [17.0.0+]",
                "FSNoCNTChkV1GuiToggleToggleListItemText": "Disable Content Counter Check [1.0.0-18.1.0]",
                "FSNoCNTChkV2GuiToggleToggleListItemText": "Disable Content Counter Check [19.0.0+]",
                "LDRGuiToggleCategoryHeaderText": "Module Loader - 0100000000000001",
                "LDRNoAcidSigChkV3GuiToggleToggleListItemText": "Disable ACID Signature Check [10.0.0+]",
                "ERPTGuiToggleCategoryHeaderText": "Error Report - 010000000000002B",
                "ERPTNoErrorReportGuiToggleToggleListItemText": "Disable Error Report Upload",
                "ESGuiToggleCategoryHeaderText": "eTicket Service - 0100000000000033",
                "ESeTicketChkV1GuiToggleToggleListItemText": "Disable eTicket Validation [1.0.0-8.1.1]",
                "ESeTicketChkV2GuiToggleToggleListItemText": "Disable eTicket Validation [9.0.0-11.0.1]",
                "ESeTicketChkV3GuiToggleToggleListItemText": "Disable eTicket Validation [12.0.0-18.1.0]",
                "ESeTicketChkV4GuiToggleToggleListItemText": "Disable eTicket Validation [19.0.0+]",
                "OLSCGuiToggleCategoryHeaderText": "Online Save Storage - 010000000000003E",
                "OnlineSaveStorageChkV1GuiToggleToggleListItemText": "Disable Online Save Storage Validation [6.0.0-14.1.2]",
                "OnlineSaveStorageChkV2GuiToggleToggleListItemText": "Disable Online Save Storage Validation [15.0.0-18.1.0]",
                "OnlineSaveStorageChkV3GuiToggleToggleListItemText": "Disable Online Save Storage Validation [19.0.0+]",
                "NIFMGuiToggleCategoryHeaderText": "Network Connection - 010000000000000F",
                "NIFMCtestV1GuiToggleToggleListItemText": "Disable Network Connection Test [1.0.0-19.0.1]",
                "NIFMCtestV2GuiToggleToggleListItemText": "Disable Network Connection Test [20.0.0+]",
                "NIMGuiToggleCategoryHeaderText": "Network Identity - 0100000000000025",
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