// -*- mode: js; js-indent-level: 4; indent-tabs-mode: nil -*-

const Gio = imports.gi.Gio;
const GLib = imports.gi.GLib;
const Lang = imports.lang;

const ScrollWMShellIface = '<node> \
<interface name="org.scrollwm.Shell"> \
<method name="Eval"> \
    <arg type="s" direction="in" name="script" /> \
    <arg type="b" direction="out" name="success" /> \
    <arg type="s" direction="out" name="result" /> \
</method> \
</interface> \
</node>';

const ScrollWMDBus = new Lang.Class({
    Name: 'ScrollWMShellDBus',

    _init: function() {
        this._dbusImpl = Gio.DBusExportedObject.wrapJSObject(ScrollWMShellIface, this);
        this._dbusImpl.export(Gio.DBus.session, '/org/scrollwm/Shell');
    },

    /**
     * Eval:
     * @code: A string containing JavaScript code
     *
     * This function executes arbitrary code in the main
     * loop, and returns a boolean success and
     * JSON representation of the object as a string.
     *
     * If evaluation completes without throwing an exception,
     * then the return value will be [true, JSON.stringify(result)].
     * If evaluation fails, then the return value will be
     * [false, JSON.stringify(exception)];
     *
     */
    Eval: function(code) {
        let result;
        let success = true;
        try {
            result = eval(code);
        } catch(e) {
            result = '' + e;
            success = false;
        }
        return [success, result === undefined ? "" : result.toString()];
    },

    ShellVersion: "0.1"
});

function dummy() {}

Gio.bus_own_name(Gio.BusType.SESSION,
                 "org.scrollwm.Shell",
                 Gio.BusNameOwnerFlags.NONE,
                 dummy,
                 dummy,
                 dummy
                );

let shellDBusService = new ScrollWMDBus();
