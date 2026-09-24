#include "includes.h"
#include "options.h"
#include "interface.h"

#include "utils/lib-utils/ida-utils.h"
#include "utils/lib-utils/discord-utils.h"

#ifdef _Release
#include "utils/build-version/ver.h"
#endif
#ifdef _Release64
#include "utils/build-version/ver64.h"
#endif

static int64_t   start_time;
static const char* app_id = "1407895875249770587";

static const char* IDAP_comment = "IDA plugin by HUTAOSHUSBAND";
static const char* IDAP_help    = "IDA plugin by HUTAOSHUSBAND";
static const char* IDAP_name    = "IDA RPC";
static const char* IDAP_hotkey  = "Ctrl-Alt-R";

// IDA Update: 9.4 - plugin converted to the plugmod_t model, hook_to_notification_point() is deprecated
struct plugin_ctx_t : public plugmod_t
{
    struct idp_listener_t : public event_listener_t
    {
        virtual ssize_t idaapi on_event( ssize_t code, va_list va ) override
        {
            switch ( code ) {
            case processor_t::event_t::ev_oldfile:
            case processor_t::event_t::ev_newfile:
            case processor_t::event_t::ev_newbinary:
            case processor_t::event_t::ev_rename:
                discord_utils::update_discord_presence( start_time );
                break;

            default:
                break;
            }

            return 0;
        }
    };

    struct idb_listener_t : public event_listener_t
    {
        virtual ssize_t idaapi on_event( ssize_t code, va_list va ) override
        {
            switch ( code ) {
            case idb_event::savebase:
            case idb_event::func_updated:
            case idb_event::set_func_start:
            case idb_event::renamed:
            case idb_event::func_added:
            // IDA Update: 9.4 - deleting_func renamed to deleting_function
            case idb_event::deleting_function:
            case idb_event::allsegs_moved:
                discord_utils::update_discord_presence( start_time );
                break;

            default:
                break;
            }

            return 0;
        }
    };

    struct ui_listener_t : public event_listener_t
    {
        virtual ssize_t idaapi on_event( ssize_t code, va_list va ) override
        {
            switch ( code ) {
            case ui_load_file:
            case ui_updated_actions:
            case ui_refresh:
            case ui_get_cursor:
            case ui_get_curline:
                discord_utils::update_discord_presence( start_time );
                break;

            default:
                break;
            }

            return 0;
        }
    };

    struct view_listener_t : public event_listener_t
    {
        virtual ssize_t idaapi on_event( ssize_t code, va_list va ) override
        {
            switch ( code ) {
            case view_loc_changed:
            case view_switched:
            case view_click:
            case view_curpos:
                discord_utils::update_discord_presence( start_time );
                break;

            default:
                break;
            }

            return 0;
        }
    };

    idp_listener_t  idp_listener;
    idb_listener_t  idb_listener;
    ui_listener_t   ui_listener;
    view_listener_t view_listener;

    plugin_ctx_t( )
    {
        start_time = time( nullptr );
    }

    ~plugin_ctx_t( )
    {
        // IDA Update: 9.4 - listeners unregister themselves on destruction, replaces
        // unhook_callbacks() which unhooked the idb callback from HT_IDP and the view callback from HT_UI
        Discord_ClearPresence( );
        Discord_Shutdown( );
    }

    // IDA Update: 9.4 - HKCB_GLOBAL keeps the listeners alive across database switches (PLUGIN_FIX plugin)
    bool hook_listeners( )
    {
        bool ok = true;

        ok &= hook_event_listener( HT_IDP, &idp_listener, HKCB_GLOBAL );
        ok &= hook_event_listener( HT_IDB, &idb_listener, HKCB_GLOBAL );
        ok &= hook_event_listener( HT_UI, &ui_listener, HKCB_GLOBAL );
        ok &= hook_event_listener( HT_VIEW, &view_listener, HKCB_GLOBAL );

        if ( !ok && g_options.output_type >= ( int ) output_type::errors_only && g_options.output_enabled ) {
            msg( "[%s] hook_event_listener failed for one or more notification points\n", IDAP_name );
        }

        return ok;
    }

    virtual bool idaapi run( size_t arg ) override
    {
        show_options( );

        if ( g_options.rpc_enabled ) {

            if ( ida_utils::is_idb_loaded( ) && g_options.output_type >= ( int ) output_type::errors_and_results && g_options.output_enabled ) {
                msg( "[%s] Version: %.2f\n", IDAP_name, AUTO_VERSION_RELEASE );
                // IDA Update: 9.4 - IDA_SDK_VERSION reports 940, IDP_INTERFACE_VERSION stays 900 across 9.x
                msg( "[%s] Built for IDA SDK: %i\n", IDAP_name, IDA_SDK_VERSION );
                msg( "[%s] Processor module: %s \n", IDAP_name, ida_utils::get_current_processor_module( ) );
                msg( "[%s] Currently open file: %s\n", IDAP_name, ida_utils::get_current_filename( ) );
                msg( "[%s] Current function: %s - 0x%a\n", IDAP_name, ida_utils::get_current_function_name( ), ida_utils::get_current_function_start_address( ) );
                msg( "[%s] Current selected address: 0x%a\n", IDAP_name, ida_utils::get_current_cursor_address( ) );
            }

            discord_utils::update_discord_presence( start_time );
        }

        return true;
    }
};

static plugmod_t* idaapi init( )
{
    addon_info_t addon;
    addon.id       = "HUTAOSHUSBAND.IDA.RPC";
    addon.name     = "IDA RPC";
    addon.producer = "HUTAOSHUSBAND";
    addon.url      = "https://www.github.com/HUTAOSHUSBAND";
    addon.version  = AUTO_VERSION_STR;
    addon.freeform = "Copyright (c) 2018 HUTAOSHUSBAND <https://keybase.io/HUTAOSHUSBAND>\n"
                     "All rights reserved.\n";

    register_addon( &addon );

    g_options.load( );

    plugin_ctx_t* ctx = new plugin_ctx_t;

    if ( g_options.rpc_enabled ) {

        if ( !ctx->hook_listeners( ) ) {
            delete ctx;
            return nullptr;
        }
    }

    msg( "[%s] v%.2f by HUTAOSHUSBAND loaded\n", IDAP_name, AUTO_VERSION_RELEASE );

    discord_utils::discord_init( app_id );

    return ctx;
}

plugin_t PLUGIN = {
    // IDA Update: 9.4 - PLUGIN_FIX | PLUGIN_MULTI with plugmod_t, term()/run() replaced by virtuals
    IDP_INTERFACE_VERSION,
    PLUGIN_FIX | PLUGIN_MULTI,
    init,
    nullptr,
    nullptr,
    IDAP_comment,
    IDAP_help,
    IDAP_name,
    IDAP_hotkey
};
