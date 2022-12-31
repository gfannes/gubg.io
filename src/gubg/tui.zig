const std = @import("std");

pub const Error = error{
    Unknown,
    TCGetAttr,
    TCSetAttr,
    IOCtl,
};

pub const Terminal = struct {
    const Self = @This();
    const platform = std.os.linux;

    termios: std.c.termios = undefined,

    const Config = struct {
        timeout: bool = false,
    };
    pub fn init(config: Config) Error!Terminal {
        var self = Terminal{};

        if (std.c.tcgetattr(0, &self.termios) != 0)
            return Error.TCGetAttr;

        var my = self.termios;

        // Canonical mode: do not wait for <enter>
        my.lflag &= ~platform.ICANON;
        // Do not echo each typed character
        my.lflag &= ~platform.ECHO;

        if (config.timeout) {
            my.cc[platform.V.TIME] = 1;
            my.cc[platform.V.MIN] = 0;
        } else {
            my.cc[platform.V.TIME] = 0;
            my.cc[platform.V.MIN] = 1;
        }

        if (std.c.tcsetattr(0, platform.TCSA.NOW, &my) != 0)
            return Error.TCSetAttr;

        return self;
    }

    pub fn deinit(self: *Self) void {
        _ = std.c.tcsetattr(0, platform.TCSA.NOW, &self.termios);
    }

    const CharSize = struct {
        width: u32 = 0,
        height: u32 = 0,
    };
    pub fn get_charsize() Error!CharSize {
        var sz: platform.winsize = undefined;

        if (std.c.ioctl(0, platform.T.IOCGWINSZ, &sz) != 0)
            return Error.IOCtl;

        return CharSize{
            .width = sz.ws_col,
            .height = sz.ws_row,
        };
    }

    pub fn read_char(_: *Self) ?u8 {
        var buf: [1]u8 = undefined;
        if (std.c.read(0, &buf, buf.len) == 1)
            return buf[0];
        return null;
    }
};
